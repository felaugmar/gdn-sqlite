#include "gdn/sqlite/sqlite.hpp"

#include <sqlite3.h>

#include "gdn/sqlite/database.hpp"

void gdn::sqlite::SQLite::_register_methods() {
  godot::register_method("errstr", &SQLite::errstr);

  godot::register_method("version", &SQLite::version);
  godot::register_method("version_number", &SQLite::version_number);
  godot::register_method("source_id", &SQLite::source_id);

  godot::register_method("open", &SQLite::open);
}

gdn::sqlite::SQLite::SQLite() {}

gdn::sqlite::SQLite::~SQLite() {}

void gdn::sqlite::SQLite::_init() {}

godot::String gdn::sqlite::SQLite::errstr(int result_code) const {
  return {sqlite3_errstr(result_code)};
}

godot::String gdn::sqlite::SQLite::version() const { return {SQLITE_VERSION}; }

uint32_t gdn::sqlite::SQLite::version_number() const {
  return SQLITE_VERSION_NUMBER;
}

godot::String gdn::sqlite::SQLite::source_id() const {
  return {SQLITE_SOURCE_ID};
}

godot::Ref<gdn::sqlite::SQLiteDatabase> gdn::sqlite::SQLite::open(
    godot::String path) const {
  godot::Ref<SQLiteDatabase> database = SQLiteDatabase::_new();

  database->open(path);

  return database;
}
