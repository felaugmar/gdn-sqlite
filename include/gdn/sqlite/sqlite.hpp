#pragma once

#include <Godot.hpp>
#include <unordered_map>

namespace gdn {
namespace sqlite {

class SQLiteDatabase;

class SQLite : public godot::Reference {
  GODOT_CLASS(SQLite, Reference)

 public:
  static void _register_methods();

  SQLite();
  ~SQLite();

  void _init();

  /// Result code
  ////////////////////////////////////////////////////////////

  /// Error Codes And Messages.
  /// English-language text that describes the result code.
  /// See: https://www.sqlite.org/c3ref/errcode.html
  godot::String errstr(int result_code) const;

  ////////////////////////////////////////////////////////////

  /// Compile-Time Library Version Numbers
  /// See: https://www.sqlite.org/c3ref/c_source_id.html
  ////////////////////////////////////////////////////////////

  godot::String version() const;
  uint32_t version_number() const;
  godot::String source_id() const;

  ////////////////////////////////////////////////////////////

  /// Opens the database
  godot::Ref<SQLiteDatabase> open(godot::String path) const;

  godot::Array _get_property_list() const;

  godot::Variant _get(godot::String property) const;

 protected:
  static std::unordered_map<std::string, int32_t> constants_int32_t;
};

};  // namespace sqlite
};  // namespace gdn