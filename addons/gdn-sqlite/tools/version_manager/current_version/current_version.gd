tool

extends MarginContainer

const GDNSQLite = preload('res://addons/gdn-sqlite/gdn_sqlite.gdns')

class MethodSorter:
  static func sort_by_order_ascending(a: Dictionary, b: Dictionary):
    return a.get('order', 0) < b.get('order', 0)


onready var _tree := $v_box/tree

var _gdn_sqlite : GDNSQLite

func _ready() -> void:
  _gdn_sqlite = GDNSQLite.new()
  
  _setup_tree()


func _setup_tree() -> void:
  # create root
  _tree.create_item()
  
  # info
  _setup_tree_info()
  
  # types
  _setup_tree_types()


func _setup_tree_info() -> void:
  var info := _tree.create_item() as TreeItem
  
  info.set_text(0, 'info')
  info.set_editable(0, false)
  info.set_selectable(0, false)
  info.set_expand_right(0, true)
  info.collapsed = true
  
  var info_methods := ['git_version_hash', 'plugin_name', 'plugin_description', 'plugin_author', 'plugin_version']
  
  for info_method in info_methods:
    var item := _tree.create_item(info) as TreeItem
    item.set_text(0, info_method)
    item.set_text(1, _gdn_sqlite.call(info_method))


func _setup_tree_types() -> void:
  var types := _tree.create_item() as TreeItem
  types.set_text(0, 'types')
  types.set_editable(0, false)
  types.set_selectable(0, false)
  types.set_expand_right(0, true)
  types.collapsed = false
  
  var gdns_files := _gdn_sqlite.gdns_files() as Array
  
  for gdns_file in gdns_files:    
    _setup_tree_type(types, gdns_file)


func _setup_tree_type(types: TreeItem, gdns_file: Dictionary) -> void:
  var gdns_filename := gdns_file['filename'] as String
    
  var gdns_path := 'res://addons/gdn-sqlite/%s' % gdns_filename
  
  var gdns := ResourceLoader.load(gdns_path) as NativeScript
  
  var type := _tree.create_item(types) as TreeItem
  type.set_text(0, '%s (%s)' % [gdns.get_class_name(), gdns.resource_path])
  type.set_editable(0, false)
  type.set_selectable(0, false)
  type.set_expand_right(0, true)
  
  type.set_text(1, gdns.get_class_documentation())
  type.set_editable(1, false)
  type.set_selectable(1, false)
  
  type.collapsed = true
  
  # type methods
  _setup_tree_type_methods(type, gdns)
  
  # type constants
  _setup_tree_type_constants(type, gdns)


func _setup_tree_type_methods(type: TreeItem, gdns: NativeScript) -> void:
  var methods := _get_tree_type_methods(type, gdns)
  
  if methods.empty():
    return
  
  methods.sort_custom(MethodSorter, 'sort_by_order_ascending')
  
  var type_methods := _tree.create_item(type) as TreeItem
  type_methods.set_text(0, 'methods')
  type_methods.set_editable(0, false)
  type_methods.set_selectable(0, false)
  type_methods.set_expand_right(0, true)
  type_methods.collapsed = true
  
  for method in methods:
    var signature := method['signature'] as String
    var documentation := method['documentation'] as String
    
    var type_method := _tree.create_item(type_methods) as TreeItem
    type_method.set_text(0, method['name'] if signature.empty() else signature)
    type_method.set_text(1, documentation.replace('\n', ' ').strip_escapes())
    type_method.set_tooltip(1, documentation)


func _get_tree_type_methods(type: TreeItem, gdns: NativeScript) -> Array:
  var type_dummy = gdns.new()
  
  var methods := []
  
  for type_method_dict in type_dummy.get_method_list():
    var method_name := type_method_dict['name'] as String
    
    # skip Reference methods
    if ClassDB.class_has_method('Reference', method_name):
      continue
    
    # methods starting with `_` and `free` aren't included in `class_has_method`
    if method_name.begins_with('_') or method_name == 'free':
      continue
    
    var md := gdns.get_method_documentation(method_name) as String
    var md_lines := md.split('\n')
    
    var signature_marker := 'signature:'
    var order_marker := 'order:'
    
    var signature := ''
    var order := 0
    var documentation := ''
    
    for md_line in md_lines:
      if md_line.begins_with(signature_marker):
        signature = md_line.substr(signature_marker.length()).strip_edges()
      elif md_line.begins_with(order_marker):
        order = int(md_line.substr(order_marker.length()).strip_edges())
      else:
        documentation += '%s\n' % md_line
    
    methods.push_back({
      'name': method_name,
      'signature': signature,
      'order': order,
      'documentation': documentation
    })
  
  return methods


func _setup_tree_type_constants(type: TreeItem, gdns: NativeScript) -> void:
  var constants := _get_tree_type_constants(type, gdns)
  
  if constants.empty():
    return
  
  var type_constants := _tree.create_item(type) as TreeItem
  type_constants.set_text(0, 'constants')
  type_constants.set_editable(0, false)
  type_constants.set_selectable(0, false)
  type_constants.set_expand_right(0, true)
  type_constants.collapsed = true
  
  var categories := {} # [String, TreeItem]
  
  for constant in constants:
    var constant_category := constant['category'] as String
    
    if not (constant_category in categories):
      var type_constant_category := _tree.create_item(type_constants) as TreeItem
      type_constant_category.set_text(0, 'Uncategorized' if constant_category.empty() else constant_category)
      type_constant_category.set_editable(0, false)
      type_constant_category.set_selectable(0, false)
      type_constant_category.set_expand_right(0, true)
      type_constant_category.collapsed = true
      
      categories[constant_category] = type_constant_category
      
    var type_constant_category := categories[constant_category] as TreeItem
    
    var type_constant := _tree.create_item(type_constant_category) as TreeItem
    type_constant.set_text(0, constant['name'])
    type_constant.set_text(1, str(constant['value']))


func _get_tree_type_constants(type: TreeItem, gdns: NativeScript) -> Array:
  var type_dummy = gdns.new()
  
  var constants := []
  
  for type_property_dict in type_dummy.get_property_list():
    var property_hint := type_property_dict.get('hint', PROPERTY_HINT_NONE) as int
    var property_hint_string := type_property_dict.get('hint_string', '') as String
    
    # constants are marked with `hint` == PROPERTY_HINT_NONE and `hint_string` starting with 'constant'
    if property_hint != PROPERTY_HINT_NONE or not property_hint_string.begins_with('constant'):
      continue
    
    var hs_lines := property_hint_string.split('\n')
    
    var category_marker := 'category:'
    
    var property_name := type_property_dict['name'] as String
    var category := ''
    
    for hs_line in hs_lines:
      if hs_line.begins_with(category_marker):
        category = hs_line.substr(category_marker.length()).strip_edges()
    
    constants.push_back({
      'name': property_name,
      'category': category,
      'value': type_dummy.get(property_name)
    })
  
  return constants
