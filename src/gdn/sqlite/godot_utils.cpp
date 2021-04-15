#include "gdn/sqlite/godot_utils.hpp"

godot::Dictionary gdn::sqlite::get_arg_dict(godot::String name,
                                            godot::Variant::Type type) {
  return godot::Dictionary::make("name", name, "type", type);
}