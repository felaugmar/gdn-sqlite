#include "gdn/sqlite/database.hpp"

#include <ProjectSettings.hpp>

#include "gdn/sqlite/godot_utils.hpp"
#include "gdn/sqlite/statement.hpp"

void destroy_connection(sqlite3 *db) {
  auto err = sqlite3_close_v2(db);

  if (err)
    ERR_PRINTS("Couldn't close database. Error: " +
               godot::String(sqlite3_errstr(err)));
}

void gdn::sqlite::SQLiteDatabase::_register_methods() {
  register_class_documentation<SQLiteDatabase>("Database Connection");

  godot::register_method("errcode", &SQLiteDatabase::errcode);
  register_method_documentation<SQLiteDatabase>(
      "errcode",
      "signature: errcode() -> int\n"
      "order: 0\n"
      "Current result code of this database instance.");

  godot::register_method("extended_errcode", &SQLiteDatabase::extended_errcode);
  register_method_documentation<SQLiteDatabase>(
      "extended_errcode",
      "signature: extended_errcode() -> int\n"
      "order: 1\n"
      "Current extended result code of this database instance.");

  godot::register_method("errmsg", &SQLiteDatabase::errmsg);
  register_method_documentation<SQLiteDatabase>(
      "errmsg",
      "signature: errmsg() -> String\n"
      "order: 2\n"
      "English-language text that describes the error.");

  godot::register_method("system_errno", &SQLiteDatabase::system_errno);
  register_method_documentation<SQLiteDatabase>(
      "system_errno",
      "signature: system_errno() -> int\n"
      "order: 3\n"
      "Attempt to return the underlying operating system error code or error\n"
      "number that caused the most recent I/O error or failure to open a "
      "file.");

  godot::register_method("prepare", &SQLiteDatabase::prepare);
  register_method_documentation<SQLiteDatabase>(
      "prepare",
      "signature: prepare(sql: String) -> SQLiteStatement\n"
      "order: 4\n"
      "Compiling An SQL Statement.");
  register_method_argument_information<SQLiteDatabase>(
      "prepare",
      godot::Array::make(get_arg_dict("sql", godot::Variant::Type::STRING)));

  godot::register_method("close", &SQLiteDatabase::close);
  register_method_documentation<SQLiteDatabase>("close",
                                                "signature: close() -> void\n"
                                                "order: 5\n"
                                                "Closes the database.");

  godot::register_method("is_closed", &SQLiteDatabase::is_closed);
  register_method_documentation<SQLiteDatabase>(
      "is_closed",
      "signature: is_closed() -> bool\n"
      "order: 6\n"
      "Whether or not the database is closed.");

  godot::register_method("get_path", &SQLiteDatabase::get_path);
  register_method_documentation<SQLiteDatabase>(
      "get_path",
      "signature: get_path() -> String\n"
      "order: 7\n"
      "Database path.");

  godot::register_method("_to_string", &SQLiteDatabase::get_path);
}

gdn::sqlite::SQLiteDatabase::SQLiteDatabase()
    : connection(nullptr, &destroy_connection) {}

gdn::sqlite::SQLiteDatabase::~SQLiteDatabase() {}

void gdn::sqlite::SQLiteDatabase::_init() {}

int gdn::sqlite::SQLiteDatabase::errcode() const {
  ERR_FAIL_COND_V(is_closed(), SQLITE_ERROR);

  return sqlite3_errcode(connection.get());
}

int gdn::sqlite::SQLiteDatabase::extended_errcode() const {
  ERR_FAIL_COND_V(is_closed(), SQLITE_ERROR);

  return sqlite3_extended_errcode(connection.get());
}

godot::String gdn::sqlite::SQLiteDatabase::errmsg() const {
  ERR_FAIL_COND_V(is_closed(), "");

  return sqlite3_errmsg(connection.get());
}

int gdn::sqlite::SQLiteDatabase::system_errno() const {
  ERR_FAIL_COND_V(is_closed(), 0);

  return sqlite3_system_errno(connection.get());
}

int gdn::sqlite::SQLiteDatabase::open(godot::String path) {
  auto path_clean = path.strip_edges();

  if (path_clean.empty())  // Empty path
    return SQLITE_IOERR;

  // Get the real path (`res://file` -> `c:/.../file`)
  auto real_path =
      godot::ProjectSettings::get_singleton()->globalize_path(path_clean);

  sqlite3 *db;

  auto err = sqlite3_open(real_path.utf8().get_data(), &db);

  if (db) {
    this->path = path;
    connection = connection_ptr(db, &destroy_connection);
  }

  if (err)
    ERR_PRINTS("Couldn't open database. Error: " +
               godot::String(sqlite3_errstr(err)));

  return err;
}

godot::Ref<gdn::sqlite::SQLiteStatement> gdn::sqlite::SQLiteDatabase::prepare(
    godot::String sql) {
  godot::Ref<SQLiteStatement> stmt = SQLiteStatement::_new();

  auto err = stmt->prepare(this, sql);

  if (err) return {};

  return stmt;
}

void gdn::sqlite::SQLiteDatabase::close() {
  // Call the reset on the unique_ptr containing the database connection.
  // This will call our deleter (`destroy_connection`).
  connection.reset();

  path = godot::String();
}

bool gdn::sqlite::SQLiteDatabase::is_closed() const {
  return connection == nullptr;
}

godot::String gdn::sqlite::SQLiteDatabase::get_path() const {
  ERR_FAIL_COND_V(is_closed(), "");

  return path;
}