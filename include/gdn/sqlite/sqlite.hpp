#pragma once

#include <Godot.hpp>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace gdn {
namespace sqlite {

class SQLiteDatabase;

template <class T>
class ConstantMapping {
 public:
  using init_tuple = std::tuple<const std::string, T, const std::string>;
  using constants_vector = std::vector<std::string>;

  ConstantMapping(std::initializer_list<init_tuple> init) {
    for (auto const_tuple : init) {
      auto name = std::get<0>(const_tuple);

      constants.push_back(name);
      constants_value.emplace(name, std::get<1>(const_tuple));
      constants_category.emplace(name, std::get<2>(const_tuple));
    }
  }

 public:
  const constants_vector &get_constants() const { return constants; }

  const T &get_value(const std::string &constant) const {
    return constants_value.at(constant);
  }

  const std::string &get_category(const std::string &constant) const {
    return constants_category.at(constant);
  }

 private:
  constants_vector constants;

  std::unordered_map<std::string, std::string> constants_category;
  std::unordered_map<std::string, T> constants_value;
};

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

  /// SQLite version in the format "X.Y.Z" where
  /// X is the major version number (always 3 for SQLite3)
  /// and Y is the minor version number and Z is the release number.
  godot::String version() const;

  /// SQLite version in the format(X*1000000 + Y*1000 + Z)
  /// where X, Y, and Z are the same numbers used in version().
  uint32_t version_number() const;

  /// Contains the date and time of the check-in (UTC)
  /// and a SHA1 or SHA3-256 hash of the entire source tree.
  godot::String source_id() const;

  ////////////////////////////////////////////////////////////

  /// Opens the database at `path`.
  godot::Ref<SQLiteDatabase> open(godot::String path) const;

  godot::Array _get_property_list() const;

  godot::Variant _get(godot::String property) const;

 protected:
  static const ConstantMapping<int32_t> constants_int32_t;
};

};  // namespace sqlite
};  // namespace gdn