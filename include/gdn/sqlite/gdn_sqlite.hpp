#pragma once

#include <Godot.hpp>

namespace gdn {
namespace sqlite {

class GDNSQLite : public godot::Reference {
  GODOT_CLASS(GDNSQLite, Reference)

 public:
  static void _register_methods();

  GDNSQLite();
  ~GDNSQLite();

  void _init();

  godot::String git_version_hash() const;

  godot::String plugin_name() const;
  godot::String plugin_description() const;
  godot::String plugin_author() const;
  godot::String plugin_version() const;
};

};  // namespace sqlite
};  // namespace gdn
