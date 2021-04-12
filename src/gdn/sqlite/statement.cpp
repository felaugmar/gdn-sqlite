#include "gdn/sqlite/statement.hpp"

#include <PoolArrays.hpp>
#include <StreamPeerBuffer.hpp>

#include "gdn/sqlite/database.hpp"

void finalize_stmt(sqlite3_stmt *stmt) {
  auto err = sqlite3_finalize(stmt);

  if (err)
    ERR_PRINTS("Couldn't finalize statement. Error: " +
               godot::String(sqlite3_errstr(err)));
}

void gdn::sqlite::SQLiteStatement::_register_methods() {
  godot::register_method("step", &SQLiteStatement::step);
  godot::register_method("reset", &SQLiteStatement::reset);
  godot::register_method("busy", &SQLiteStatement::busy);
  godot::register_method("readonly", &SQLiteStatement::readonly);
  godot::register_method("status", &SQLiteStatement::status);
  godot::register_method("get_database", &SQLiteStatement::get_database);

  godot::register_method("bind", &SQLiteStatement::bind);
  godot::register_method("parameter_count", &SQLiteStatement::parameter_count);
  godot::register_method("parameter_index", &SQLiteStatement::parameter_index);
  godot::register_method("parameter_name", &SQLiteStatement::parameter_name);
  godot::register_method("clear_bindings", &SQLiteStatement::clear_bindings);

  godot::register_method("bind_named", &SQLiteStatement::bind_named);
  godot::register_method("bind_array", &SQLiteStatement::bind_array);
  godot::register_method("bind_dict", &SQLiteStatement::bind_dict);

  godot::register_method("column", &SQLiteStatement::column);
  godot::register_method("column_bytes", &SQLiteStatement::column_bytes);
  godot::register_method("column_type", &SQLiteStatement::column_type);
  godot::register_method("column_count", &SQLiteStatement::column_count);
  godot::register_method("data_count", &SQLiteStatement::data_count);
  godot::register_method("column_name", &SQLiteStatement::column_name);
#ifdef SQLITE_ENABLE_COLUMN_METADATA
  godot::register_method("column_database_name",
                         &SQLiteStatement::column_database_name);
  godot::register_method("column_table_name",
                         &SQLiteStatement::column_table_name);
  godot::register_method("column_origin_name",
                         &SQLiteStatement::column_origin_name);
#endif  // #ifdef SQLITE_ENABLE_COLUMN_METADATA
  godot::register_method("column_decltype", &SQLiteStatement::column_decltype);

  godot::register_method("columns_array", &SQLiteStatement::columns_array);
  godot::register_method("columns_dict", &SQLiteStatement::columns_dict);

  godot::register_method("sql", &SQLiteStatement::sql);
  godot::register_method("expanded_sql", &SQLiteStatement::expanded_sql);
#ifdef SQLITE_ENABLE_NORMALIZE
  godot::register_method("normalized_sql", &SQLiteStatement::normalized_sql);
#endif  // SQLITE_ENABLE_NORMALIZE

  godot::register_method("finalize", &SQLiteStatement::finalize);
  godot::register_method("is_finalized", &SQLiteStatement::is_finalized);

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
      return {(int64_t)sqlite3_column_int64(stmt.get(), index)};
    case SQLITE_FLOAT:
      return {sqlite3_column_double(stmt.get(), index)};
    case SQLITE3_TEXT:
      return {(const char *)sqlite3_column_text(stmt.get(), index)};
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
