#include "gdn/sqlite/database.hpp"

#include <ProjectSettings.hpp>

#include "gdn/sqlite/statement.hpp"

void destroy_connection(sqlite3 *db) {
  auto err = sqlite3_close_v2(db);

  if (err)
    ERR_PRINTS("Couldn't close database. Error: " +
               godot::String(sqlite3_errstr(err)));
}

void gdn::sqlite::SQLiteDatabase::_register_methods() {
  godot::register_method("errcode", &SQLiteDatabase::errcode);
  godot::register_method("extended_errcode", &SQLiteDatabase::extended_errcode);
  godot::register_method("errmsg", &SQLiteDatabase::errmsg);

  godot::register_method("system_errno", &SQLiteDatabase::system_errno);

  godot::register_method("prepare", &SQLiteDatabase::prepare);

  godot::register_method("close", &SQLiteDatabase::close);
  godot::register_method("is_closed", &SQLiteDatabase::is_closed);

  godot::register_method("get_path", &SQLiteDatabase::get_path);

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