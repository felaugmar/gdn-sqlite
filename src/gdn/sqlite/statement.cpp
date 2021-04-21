#include "gdn/sqlite/statement.hpp"

#include <PoolArrays.hpp>
#include <StreamPeerBuffer.hpp>

#include "gdn/sqlite/database.hpp"
#include "gdn/sqlite/godot_utils.hpp"

void finalize_stmt(sqlite3_stmt *stmt) {
  auto err = sqlite3_finalize(stmt);

  if (err)
    ERR_PRINTS("Couldn't finalize statement. Error: " +
               godot::String(sqlite3_errstr(err)));
}

void gdn::sqlite::SQLiteStatement::_register_methods() {
  register_class_documentation<SQLiteStatement>("Prepared Statement");

  godot::register_method("step", &SQLiteStatement::step);
  register_method_documentation<SQLiteStatement>("step",
                                                 "signature: step() -> int\n"
                                                 "order: 0\n"
                                                 "Evaluate An SQL Statement.");

  godot::register_method("reset", &SQLiteStatement::reset);
  register_method_documentation<SQLiteStatement>(
      "reset",
      "signature: reset() -> int\n"
      "order: 1\n"
      "Reset A Prepared Statement Object.");

  godot::register_method("busy", &SQLiteStatement::busy);
  register_method_documentation<SQLiteStatement>(
      "busy",
      "signature: busy() -> bool\n"
      "order: 2\n"
      "Determine If A Prepared Statement Has Been Reset.");

  godot::register_method("readonly", &SQLiteStatement::readonly);
  register_method_documentation<SQLiteStatement>(
      "readonly",
      "signature: readonly() -> bool\n"
      "order: 3\n"
      "Determine If An SQL Statement Writes The Database.");

  godot::register_method("status", &SQLiteStatement::status);
  register_method_documentation<SQLiteStatement>(
      "status",
      "signature: status(operation: int, reset: bool) -> int\n"
      "order: 4\n"
      "Prepared Statement Status.");
  register_method_argument_information<SQLiteStatement>(
      "status",
      godot::Array::make(get_arg_dict("operation", godot::Variant::Type::INT),
                         get_arg_dict("reset", godot::Variant::Type::BOOL)));

  godot::register_method("get_database", &SQLiteStatement::get_database);
  register_method_documentation<SQLiteStatement>(
      "get_database",
      "signature: get_database() -> SQLiteDatabase\n"
      "order: 5\n"
      "Get the database this statement belongs to.");

  godot::register_method("bind", &SQLiteStatement::bind);
  register_method_documentation<SQLiteStatement>(
      "bind",
      "signature: bind(index: int, data: Variant) -> int\n"
      "order: 6\n"
      "Binding Values To Prepared Statements.");
  register_method_argument_information<SQLiteStatement>(
      "bind",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::INT),
                         get_arg_dict("data", godot::Variant::Type::NIL)));

  godot::register_method("parameter_count", &SQLiteStatement::parameter_count);
  register_method_documentation<SQLiteStatement>(
      "parameter_count",
      "signature: parameter_count() -> int\n"
      "order: 7\n"
      "Number Of SQL Parameters.");

  godot::register_method("parameter_index", &SQLiteStatement::parameter_index);
  register_method_documentation<SQLiteStatement>(
      "parameter_index",
      "signature: parameter_index(name: String) -> int\n"
      "order: 8\n"
      "Index Of A Parameter With A Given Name.");
  register_method_argument_information<SQLiteStatement>(
      "parameter_index",
      godot::Array::make(get_arg_dict("name", godot::Variant::Type::STRING)));

  godot::register_method("parameter_name", &SQLiteStatement::parameter_name);
  register_method_documentation<SQLiteStatement>(
      "parameter_name",
      "signature: parameter_name(index: int) -> String\n"
      "order: 9\n"
      "Name Of A Host Parameter.");
  register_method_argument_information<SQLiteStatement>(
      "parameter_name",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::INT)));

  godot::register_method("clear_bindings", &SQLiteStatement::clear_bindings);
  register_method_documentation<SQLiteStatement>(
      "clear_bindings",
      "signature: clear_bindings() -> int\n"
      "order: 10\n"
      "Reset All Bindings On A Prepared Statement.");

  godot::register_method("bind_named", &SQLiteStatement::bind_named);
  register_method_documentation<SQLiteStatement>(
      "bind_named",
      "signature: bind_named(name: String, data: Variant) -> int\n"
      "order: 11\n"
      "Bind by the index returned from `parameter_index(name)`.");
  register_method_argument_information<SQLiteStatement>(
      "bind_named",
      godot::Array::make(get_arg_dict("name", godot::Variant::Type::STRING),
                         get_arg_dict("data", godot::Variant::Type::NIL)));

  godot::register_method("bind_array", &SQLiteStatement::bind_array);
  register_method_documentation<SQLiteStatement>(
      "bind_array",
      "signature: bind_array(array: Array) -> int\n"
      "order: 12\n"
      "Bind array values.\n"
      "Binds each array element in sequence (1, 2, 3, 4, 5)\n"
      "Note: using `?NNN` parameters can leave holes (1, 2, 4, 5).");
  register_method_argument_information<SQLiteStatement>(
      "bind_array",
      godot::Array::make(get_arg_dict("array", godot::Variant::Type::ARRAY)));

  godot::register_method("bind_dict", &SQLiteStatement::bind_dict);
  register_method_documentation<SQLiteStatement>(
      "bind_dict",
      "signature: bind_dict(dict: Dictionary) -> int\n"
      "order: 13\n"
      "Bind dict values.\n"
      "If the key is `int`, bind by index.\n"
      "If the key is `String`, bind by name.\n"
      "Any other key type is ignored.");
  register_method_argument_information<SQLiteStatement>(
      "bind_dict", godot::Array::make(
                       get_arg_dict("dict", godot::Variant::Type::DICTIONARY)));

  godot::register_method("column", &SQLiteStatement::column);
  register_method_documentation<SQLiteStatement>(
      "column",
      "signature: column(index: int) -> Variant\n"
      "order: 14\n"
      "Result Values From A Query.");
  register_method_argument_information<SQLiteStatement>(
      "column",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::NIL)));

  godot::register_method("column_bytes", &SQLiteStatement::column_bytes);
  register_method_documentation<SQLiteStatement>(
      "column_bytes",
      "signature: column_bytes(index: int) -> int\n"
      "order: 15\n"
      "Size of a BLOB or a UTF-8 TEXT result in bytes.");
  register_method_argument_information<SQLiteStatement>(
      "column_bytes",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::INT)));

  godot::register_method("column_type", &SQLiteStatement::column_type);
  register_method_documentation<SQLiteStatement>(
      "column_type",
      "signature: column_type(index: int) -> int\n"
      "order: 16\n"
      "Default datatype of the result.");
  register_method_argument_information<SQLiteStatement>(
      "column_type",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::INT)));

  godot::register_method("column_count", &SQLiteStatement::column_count);
  register_method_documentation<SQLiteStatement>(
      "column_count",
      "signature: column_count() -> int\n"
      "order: 17\n"
      "Number Of Columns In A Result Set.");

  godot::register_method("data_count", &SQLiteStatement::data_count);
  register_method_documentation<SQLiteStatement>(
      "data_count",
      "signature: data_count() -> int\n"
      "order: 18\n"
      "Number of columns in a result set.");

  godot::register_method("column_name", &SQLiteStatement::column_name);
  register_method_documentation<SQLiteStatement>(
      "column_name",
      "signature: column_name(index: int) -> String\n"
      "order: 19\n"
      "Column Names In A Result Set.");
  register_method_argument_information<SQLiteStatement>(
      "column_name",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::INT)));

#ifdef SQLITE_ENABLE_COLUMN_METADATA
  godot::register_method("column_database_name",
                         &SQLiteStatement::column_database_name);
  register_method_documentation<SQLiteStatement>(
      "column_database_name",
      "signature: column_database_name(index: int) -> String\n"
      "order: 20\n"
      "Database Name In A Query Result.");
  register_method_argument_information<SQLiteStatement>(
      "column_database_name",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::INT)));

  godot::register_method("column_table_name",
                         &SQLiteStatement::column_table_name);
  register_method_documentation<SQLiteStatement>(
      "column_table_name",
      "signature: column_table_name(index: int) -> String\n"
      "order: 21\n"
      "Table Name In A Query Result.");
  register_method_argument_information<SQLiteStatement>(
      "column_table_name",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::INT)));

  godot::register_method("column_origin_name",
                         &SQLiteStatement::column_origin_name);
  register_method_documentation<SQLiteStatement>(
      "column_origin_name",
      "signature: column_origin_name(index: int) -> String\n"
      "order: 22\n"
      "Column Name In A Query Result.");
  register_method_argument_information<SQLiteStatement>(
      "column_origin_name",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::INT)));

#endif  // #ifdef SQLITE_ENABLE_COLUMN_METADATA
  godot::register_method("column_decltype", &SQLiteStatement::column_decltype);
  register_method_documentation<SQLiteStatement>(
      "column_decltype",
      "signature: column_decltype(index: int) -> String\n"
      "order: 23\n"
      "Declared Datatype Of A Query Result.");
  register_method_argument_information<SQLiteStatement>(
      "column_decltype",
      godot::Array::make(get_arg_dict("index", godot::Variant::Type::INT)));

  godot::register_method("columns_array", &SQLiteStatement::columns_array);
  register_method_documentation<SQLiteStatement>(
      "columns_array",
      "signature: columns_array() -> Array\n"
      "order: 24\n"
      "Returns current columns in an `Array`.");

  godot::register_method("columns_dict", &SQLiteStatement::columns_dict);
  register_method_documentation<SQLiteStatement>(
      "columns_dict",
      "signature: columns_dict() -> Dictionary\n"
      "order: 25\n"
      "Returns current columns in a `Dictionary`.\n"
      "The key is the `column_name` if available, otherwise the column's "
      "index.");

  godot::register_method("sql", &SQLiteStatement::sql);
  register_method_documentation<SQLiteStatement>(
      "sql",
      "signature: sql() -> String\n"
      "order: 26\n"
      "SQL text of prepared statement.");

  godot::register_method("expanded_sql", &SQLiteStatement::expanded_sql);
  register_method_documentation<SQLiteStatement>(
      "expanded_sql",
      "signature: expanded_sql() -> String\n"
      "order: 27\n"
      "SQL text of prepared statement with bound parameters expanded.");

#ifdef SQLITE_ENABLE_NORMALIZE
  godot::register_method("normalized_sql", &SQLiteStatement::normalized_sql);
  register_method_documentation<SQLiteStatement>(
      "normalized_sql",
      "signature: normalized_sql() -> String\n"
      "order: 28\n"
      "Normalized SQL text of prepared statement.");
#endif  // SQLITE_ENABLE_NORMALIZE

  godot::register_method("finalize", &SQLiteStatement::finalize);
  register_method_documentation<SQLiteStatement>(
      "finalize",
      "signature: finalize() -> void\n"
      "order: 29\n"
      "Finalize this statement.");

  godot::register_method("is_finalized", &SQLiteStatement::is_finalized);
  register_method_documentation<SQLiteStatement>(
      "is_finalized",
      "signature: is_finalized() -> bool\n"
      "order: 30\n"
      "Whether or not the statement is finalized.");

  godot::register_method("_to_string", &SQLiteStatement::expanded_sql);
}

gdn::sqlite::SQLiteStatement::SQLiteStatement()
    : stmt(nullptr, &finalize_stmt) {}

gdn::sqlite::SQLiteStatement::~SQLiteStatement() {}

void gdn::sqlite::SQLiteStatement::_init() {}

int gdn::sqlite::SQLiteStatement::prepare(
    godot::Ref<gdn::sqlite::SQLiteDatabase> database, godot::String sql) {
  ERR_FAIL_COND_V(database.is_null(), SQLITE_ERROR);
  ERR_FAIL_COND_V(database->is_closed(), SQLITE_ERROR);

  auto utf8_sql = sql.utf8();

  sqlite3_stmt *stmt;

  auto err = sqlite3_prepare_v2(database->connection.get(), utf8_sql.get_data(),
                                utf8_sql.length(), &stmt, nullptr);

  if (err == SQLITE_OK) {
    this->database = database;
    this->stmt = stmt_ptr(stmt, &finalize_stmt);
  } else {
    if (stmt) sqlite3_finalize(stmt);

    ERR_PRINTS("Couldn't prepare statement. Error: " +
               godot::String(sqlite3_errstr(err)));
  }

  return err;
}

int gdn::sqlite::SQLiteStatement::step() {
  ERR_FAIL_COND_V(is_finalized(), SQLITE_ERROR);

  return sqlite3_step(stmt.get());
}

int gdn::sqlite::SQLiteStatement::reset() {
  ERR_FAIL_COND_V(is_finalized(), SQLITE_ERROR);

  return sqlite3_reset(stmt.get());
}

bool gdn::sqlite::SQLiteStatement::busy() const {
  return sqlite3_stmt_busy(stmt.get());
}

bool gdn::sqlite::SQLiteStatement::readonly() const {
  ERR_FAIL_COND_V(is_finalized(), false);

  return sqlite3_stmt_readonly(stmt.get());
}

int gdn::sqlite::SQLiteStatement::status(int operation, bool reset) {
  ERR_FAIL_COND_V(is_finalized(), 0);

  return sqlite3_stmt_status(stmt.get(), operation, reset);
}

godot::Ref<gdn::sqlite::SQLiteDatabase>
gdn::sqlite::SQLiteStatement::get_database() const {
  return database;
}

int gdn::sqlite::SQLiteStatement::bind(int index, godot::Variant data) {
  ERR_FAIL_COND_V(is_finalized(), SQLITE_ERROR);

  auto type = data.get_type();

  int bind_result = SQLITE_ERROR;

  switch (type) {
    case godot::Variant::Type::NIL:
      bind_result = sqlite3_bind_null(stmt.get(), index);
      break;
    case godot::Variant::Type::BOOL:
      bind_result = sqlite3_bind_int(stmt.get(), index, (bool)data);
      break;
    case godot::Variant::Type::INT:
      bind_result = sqlite3_bind_int64(stmt.get(), index, (int64_t)data);
      break;
    case godot::Variant::Type::REAL:
      bind_result = sqlite3_bind_double(stmt.get(), index, (double)data);
      break;
    case godot::Variant::Type::STRING: {
      godot::String string_data = data;
      auto utf8_data = string_data.utf8();
      bind_result = sqlite3_bind_text(stmt.get(), index, utf8_data.get_data(),
                                      utf8_data.length(), SQLITE_TRANSIENT);
      break;
    }
    default: {
      if (type < 0 || type >= godot::Variant::Type::VARIANT_MAX) break;

      godot::Ref<godot::StreamPeerBuffer> spb = godot::StreamPeerBuffer::_new();

      spb->put_var(data, false);

      auto da = spb->get_data_array();
      auto dar = da.read();

      bind_result = sqlite3_bind_blob64(stmt.get(), index, dar.ptr(), da.size(),
                                        SQLITE_TRANSIENT);

      break;
    }
  }

  return bind_result;
}

int gdn::sqlite::SQLiteStatement::parameter_count() const {
  ERR_FAIL_COND_V(is_finalized(), 0);

  return sqlite3_bind_parameter_count(stmt.get());
}

int gdn::sqlite::SQLiteStatement::parameter_index(godot::String name) const {
  ERR_FAIL_COND_V(is_finalized(), 0);

  return sqlite3_bind_parameter_index(stmt.get(), name.utf8().get_data());
}

godot::String gdn::sqlite::SQLiteStatement::parameter_name(int index) const {
  ERR_FAIL_COND_V(is_finalized(), "");

  return sqlite3_bind_parameter_name(stmt.get(), index);
}

int gdn::sqlite::SQLiteStatement::clear_bindings() {
  ERR_FAIL_COND_V(is_finalized(), SQLITE_ERROR);

  return sqlite3_clear_bindings(stmt.get());
}

int gdn::sqlite::SQLiteStatement::bind_named(godot::String name,
                                             godot::Variant data) {
  ERR_FAIL_COND_V(is_finalized(), SQLITE_ERROR);

  return bind(parameter_index(name), data);
}

int gdn::sqlite::SQLiteStatement::bind_array(godot::Array array) {
  ERR_FAIL_COND_V(is_finalized(), SQLITE_ERROR);

  for (int i = 0; i < array.size(); i++) {
    // Binding starts at 1 (so, `i + 1`)
    auto err = bind(i + 1, array[i]);

    if (err) {
      WARN_PRINTS(godot::String("Failed to bind element at index {0}")
                      .format(godot::Array::make(i)));

      return err;
    }
  }

  // Success
  return SQLITE_OK;
}

int gdn::sqlite::SQLiteStatement::bind_dict(godot::Dictionary dict) {
  ERR_FAIL_COND_V(is_finalized(), SQLITE_ERROR);

  auto dict_keys = dict.keys();

  int err;

  for (auto i = 0; i < dict_keys.size(); ++i) {
    auto key = dict_keys[i];
    auto var = dict[key];

    switch (key.get_type()) {
      case godot::Variant::Type::INT:
        err = bind(key, var);
        break;

      case godot::Variant::Type::STRING:
        err = bind_named(key, var);
        break;

      default:
        // OK, any other type is ignored
        err = SQLITE_OK;
        break;
    }

    if (err) {
      WARN_PRINTS(godot::String("Failed to bind element at key {0}")
                      .format(godot::Array::make(key)));

      return err;
    }
  }

  return SQLITE_OK;
}

godot::Variant gdn::sqlite::SQLiteStatement::column(int index) const {
  ERR_FAIL_COND_V(is_finalized(), {});
  ERR_FAIL_COND_V(busy() == false,
                  {});  // The statement must be busy to use the `column` API.
  ERR_FAIL_INDEX_V(index, column_count(),
                   {});  // The index must be within the range

  auto col_type = column_type(index);

  switch (col_type) {
    case SQLITE_INTEGER:
      return (int64_t)sqlite3_column_int64(stmt.get(), index);
    case SQLITE_FLOAT:
      return sqlite3_column_double(stmt.get(), index);
    case SQLITE3_TEXT:
      return (const char *)sqlite3_column_text(stmt.get(), index);
    case SQLITE_BLOB: {
      // Read the BLOB into a StreamPeerBuffer,
      // try to get a variant from it and return if valid,
      // otherwise return the data as PoolByteArray.

      auto col_bytes = column_bytes(index);

      if (col_bytes == 0) return {};  // empty, return NULL

      // resize and copy 🙏
      auto data_array = godot::PoolByteArray();
      data_array.resize(col_bytes);

      auto write = data_array.write();
      auto blob = (uint8_t *)sqlite3_column_blob(stmt.get(), index);

      memcpy(write.ptr(), blob, col_bytes);

      godot::Ref<godot::StreamPeerBuffer> spb = godot::StreamPeerBuffer::_new();

      spb->put_data(data_array);
      spb->seek(0);

      auto var = spb->get_var(false);

      // the var is null, return the PoolByteArray
      if (var.get_type() == godot::Variant::Type::NIL) return data_array;

      return var;
    }
    case SQLITE_NULL:
    default:  // NULL variant as default
      return {};
  }
}

int gdn::sqlite::SQLiteStatement::column_bytes(int index) const {
  ERR_FAIL_COND_V(is_finalized(), 0);

  return sqlite3_column_bytes(stmt.get(), index);
}

int gdn::sqlite::SQLiteStatement::column_type(int index) const {
  ERR_FAIL_COND_V(is_finalized(), SQLITE3_TEXT);

  return sqlite3_column_type(stmt.get(), index);
}

int gdn::sqlite::SQLiteStatement::column_count() const {
  ERR_FAIL_COND_V(is_finalized(), 0);

  return sqlite3_column_count(stmt.get());
}

int gdn::sqlite::SQLiteStatement::data_count() const {
  ERR_FAIL_COND_V(is_finalized(), 0);

  return sqlite3_data_count(stmt.get());
}

godot::String gdn::sqlite::SQLiteStatement::column_name(int index) const {
  ERR_FAIL_COND_V(is_finalized(), "");

  auto column_name_data = sqlite3_column_name(stmt.get(), index);

  if (column_name_data == nullptr) return "";

  return column_name_data;
}

#ifdef SQLITE_ENABLE_COLUMN_METADATA

godot::String gdn::sqlite::SQLiteStatement::column_database_name(
    int index) const {
  ERR_FAIL_COND_V(is_finalized(), "");

  auto column_database_name_data =
      sqlite3_column_database_name(stmt.get(), index);

  if (column_database_name_data == nullptr) return "";

  return column_database_name_data;
}

godot::String gdn::sqlite::SQLiteStatement::column_table_name(int index) const {
  ERR_FAIL_COND_V(is_finalized(), "");

  auto column_table_name_data = sqlite3_column_table_name(stmt.get(), index);

  if (column_table_name_data == nullptr) return "";

  return column_table_name_data;
}

godot::String gdn::sqlite::SQLiteStatement::column_origin_name(
    int index) const {
  ERR_FAIL_COND_V(is_finalized(), "");

  auto column_origin_name_data = sqlite3_column_origin_name(stmt.get(), index);

  if (column_origin_name_data == nullptr) return "";

  return column_origin_name_data;
}

#endif  // SQLITE_ENABLE_COLUMN_METADATA

godot::String gdn::sqlite::SQLiteStatement::column_decltype(int index) const {
  ERR_FAIL_COND_V(is_finalized(), "");

  auto column_decltype_data = sqlite3_column_decltype(stmt.get(), index);

  if (column_decltype_data == nullptr) return "";

  return column_decltype_data;
}

godot::Array gdn::sqlite::SQLiteStatement::columns_array() const {
  ERR_FAIL_COND_V(is_finalized(), {});
  ERR_FAIL_COND_V(busy() == false,
                  {});  // The statement must be busy to use the `column` API.

  godot::Array array;

  for (int i = 0; i < column_count(); i++) array.append(column(i));

  return array;
}

godot::Dictionary gdn::sqlite::SQLiteStatement::columns_dict() const {
  ERR_FAIL_COND_V(is_finalized(), {});
  ERR_FAIL_COND_V(busy() == false,
                  {});  // The statement must be busy to use the `column` API.

  godot::Dictionary dict;

  for (int i = 0; i < column_count(); i++) {
    auto c_name = column_name(i);
    auto c_variant = column(i);

    // Use the index as the dict key if the column name is blank
    if (c_name.empty())
      dict[i] = c_variant;
    else
      dict[c_name] = c_variant;
  }

  return dict;
}

godot::String gdn::sqlite::SQLiteStatement::sql() const {
  ERR_FAIL_COND_V(is_finalized(), "");

  auto sql_data = sqlite3_sql(stmt.get());

  if (sql_data == nullptr) return "";

  return sql_data;
}

godot::String gdn::sqlite::SQLiteStatement::expanded_sql() const {
  ERR_FAIL_COND_V(is_finalized(), "");

  auto expanded_sql_data = sqlite3_expanded_sql(stmt.get());

  if (expanded_sql_data == nullptr) return "";

  godot::String expanded_sql_string(expanded_sql_data);

  // Note: The `expanded_sql` must be manually free using `sqlite3_free`
  // as stated here: https://www.sqlite.org/c3ref/expanded_sql.html
  sqlite3_free(expanded_sql_data);

  return expanded_sql_string;
}

#ifdef SQLITE_ENABLE_NORMALIZE

godot::String gdn::sqlite::SQLiteStatement::normalized_sql() const {
  ERR_FAIL_COND_V(is_finalized(), "");

  auto normalized_sql_data = sqlite3_normalized_sql(stmt.get());

  if (normalized_sql_data == nullptr) return "";

  return normalized_sql_data;
}

#endif  // SQLITE_ENABLE_NORMALIZE

void gdn::sqlite::SQLiteStatement::finalize() {
  // Call the reset on the unique_ptr containing the statement.
  // This will call our deleter (`finalize_stmt`).
  stmt.reset();
}

bool gdn::sqlite::SQLiteStatement::is_finalized() const {
  return stmt == nullptr;
}
