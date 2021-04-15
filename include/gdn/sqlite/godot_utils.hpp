#pragma once

#include <Godot.hpp>
#include <vector>

namespace gdn {
namespace sqlite {

template <class T>
void register_method_argument_information(const char *function_name,
                                          godot::Array args) {
  static_assert(T::___CLASS_IS_SCRIPT,
                "This function must only be used on custom classes");

  std::vector<godot_method_arg> godot_method_args;
  godot_method_args.reserve(args.size());

  for (int i = 0; i < args.size(); ++i) {
    auto arg = args[i];

    if (arg.get_type() != godot::Variant::Type::DICTIONARY) continue;

    godot::Dictionary arg_dict = arg;

    if (!arg_dict.has("name") || !arg_dict.has("type")) continue;

    godot_method_arg godot_method_arg_;

    godot::String name = arg_dict["name"];
    int type = arg_dict["type"];

    godot_method_arg_.name = *(godot_string *)&name;
    godot_method_arg_.type = (godot_variant_type)type;

    godot_method_args.push_back(std::move(godot_method_arg_));
  }

  godot::nativescript_1_1_api
      ->godot_nativescript_set_method_argument_information(
          godot::_RegisterState::nativescript_handle, T::___get_class_name(),
          function_name, (int)godot_method_args.size(),
          godot_method_args.data());
}

godot::Dictionary get_arg_dict(godot::String name, godot::Variant::Type type);

template <class T>
void register_class_documentation(godot::String documentation) {
  static_assert(T::___CLASS_IS_SCRIPT,
                "This function must only be used on custom classes");

  godot::nativescript_1_1_api->godot_nativescript_set_class_documentation(
      godot::_RegisterState::nativescript_handle, T::___get_class_name(),
      *(godot_string *)&documentation);
}

template <class T>
void register_method_documentation(const char *name,
                                   godot::String documentation) {
  static_assert(T::___CLASS_IS_SCRIPT,
                "This function must only be used on custom classes");

  godot::nativescript_1_1_api->godot_nativescript_set_method_documentation(
      godot::_RegisterState::nativescript_handle, T::___get_class_name(), name,
      *(godot_string *)&documentation);
}

template <class T>
void register_property_documentation(const char *name,
                                     godot::String documentation) {
  static_assert(T::___CLASS_IS_SCRIPT,
                "This function must only be used on custom classes");

  godot::nativescript_1_1_api->godot_nativescript_set_property_documentation(
      godot::_RegisterState::nativescript_handle, T::___get_class_name(), name,
      *(godot_string *)&documentation);
}

template <class T>
void register_signal_documentation(const char *name,
                                   godot::String documentation) {
  static_assert(T::___CLASS_IS_SCRIPT,
                "This function must only be used on custom classes");

  godot::nativescript_1_1_api->godot_nativescript_set_signal_documentation(
      godot::_RegisterState::nativescript_handle, T::___get_class_name(), name,
      *(godot_string *)&documentation);
}

};  // namespace sqlite
};  // namespace gdn