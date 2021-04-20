#include "gdn/sqlite/gdn_sqlite.hpp"

#include "gdn/sqlite/plugin_cfg.gen.h"
#include "gdn/sqlite/version.gen.h"

void gdn::sqlite::GDNSQLite::_register_methods() {
  godot::register_method("git_version_hash", &GDNSQLite::git_version_hash);

  godot::register_method("plugin_name", &GDNSQLite::plugin_name);
  godot::register_method("plugin_description", &GDNSQLite::plugin_description);
  godot::register_method("plugin_author", &GDNSQLite::plugin_author);
  godot::register_method("plugin_version", &GDNSQLite::plugin_version);
}

gdn::sqlite::GDNSQLite::GDNSQLite() {}

gdn::sqlite::GDNSQLite::~GDNSQLite() {}

void gdn::sqlite::GDNSQLite::_init() {}

godot::String gdn::sqlite::GDNSQLite::git_version_hash() const {
  return GIT_VERSION_HASH;
}

godot::String gdn::sqlite::GDNSQLite::plugin_name() const {
  return PLUGIN_NAME;
}

godot::String gdn::sqlite::GDNSQLite::plugin_description() const {
  return PLUGIN_DESCRIPTION;
}

godot::String gdn::sqlite::GDNSQLite::plugin_author() const {
  return PLUGIN_AUTHOR;
}

godot::String gdn::sqlite::GDNSQLite::plugin_version() const {
  return PLUGIN_VERSION;
}
