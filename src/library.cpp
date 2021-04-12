#include "gdn/sqlite/database.hpp"
#include "gdn/sqlite/gdn_sqlite.hpp"
#include "gdn/sqlite/sqlite.hpp"
#include "gdn/sqlite/statement.hpp"

extern "C" void GDN_EXPORT
gdn_sqlite_gdnative_init(godot_gdnative_init_options *o) {
  godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT
gdn_sqlite_gdnative_terminate(godot_gdnative_terminate_options *o) {
  godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT gdn_sqlite_nativescript_init(void *handle) {
  godot::Godot::nativescript_init(handle);

  godot::register_tool_class<gdn::sqlite::SQLite>();
  godot::register_tool_class<gdn::sqlite::SQLiteDatabase>();
  godot::register_tool_class<gdn::sqlite::SQLiteStatement>();

  godot::register_tool_class<gdn::sqlite::GDNSQLite>();
}
