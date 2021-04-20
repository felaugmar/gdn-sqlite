tool

extends EditorPlugin

const AddonConstants = preload('res://addons/gdn-sqlite/tools/utils/constants.gd')
const DirUtils = preload('res://addons/gdn-sqlite/tools/utils/directory_utils.gd')

var PluginControlScene : PackedScene

var plugin_control : Control
var tool_button : ToolButton

func _enter_tree() -> void:
  _make_current()
  
  PluginControlScene = load('res://addons/gdn-sqlite/tools/plugin_control/plugin_control.tscn')

   # add the plugin control
  plugin_control = PluginControlScene.instance()

  tool_button = add_control_to_bottom_panel(plugin_control, 'SQLite')
  tool_button.hint_tooltip = "GDN SQLite"

  plugin_control.enable()


func _exit_tree() -> void:
  # remove the plugin control
  if plugin_control:
    remove_control_from_bottom_panel(plugin_control)
    plugin_control = null


func _make_current() -> void:  
  var dir := Directory.new()
  
  if not dir.file_exists(AddonConstants.MAKE_CURRENT_FILE):
    return

  var file := File.new()

  var err := file.open(AddonConstants.MAKE_CURRENT_FILE, File.READ)
  assert(err == OK)

  var release := file.get_line().strip_escapes().strip_edges()

  file.close()

  if not dir.dir_exists(AddonConstants.get_release_dir(release)):
    return
  
  # remove current version files
  _remove_old_binaries()
  _remove_gdnlib_gdns()
  
  # copy files from the release
  _copy_release_gdnlib_gdns(release)
  _copy_release_binaries(release)
  
  get_editor_interface().get_resource_filesystem().scan()
  
  # remove .make_current
  if dir.file_exists(AddonConstants.MAKE_CURRENT_FILE):
    err = dir.remove(AddonConstants.MAKE_CURRENT_FILE)
    assert(err == OK)


func _remove_old_binaries() -> void:
  var dir := Directory.new()
  
  for gdnlib in DirUtils.get_dir_files_by_regex(AddonConstants.ADDON_DIR, 
                                                AddonConstants.GDNLIB_RE_PATTERN):
    assert(dir.file_exists(gdnlib))
    
    var library := ResourceLoader.load(gdnlib, 'GDNativeLibrary', true) as GDNativeLibrary
    
    for property_dict in library.get_property_list():
      var property_name := property_dict['name'] as String

      if not property_name.begins_with('entry/'):
        continue
      
      var binary_filepath := library.get(property_name) as String
      
      if dir.file_exists(binary_filepath):
        var err := dir.remove(binary_filepath)
        assert(err == OK)


func _remove_gdnlib_gdns() -> void:
  var dir := Directory.new()
  
  for gdnlib_gdns in DirUtils.get_dir_files_by_regex(AddonConstants.ADDON_DIR, 
                                                     AddonConstants.GDNLIB_GDNS_RE_PATTERN):
    assert(dir.file_exists(gdnlib_gdns))
    
    var err := dir.remove(gdnlib_gdns)
    assert(err == OK)


func _copy_release_gdnlib_gdns(release: String) -> void:
  var dir := Directory.new()
  
  for gdnlib_gdns in DirUtils.get_dir_files_by_regex(AddonConstants.get_release_assets_dir(release), 
                                                     AddonConstants.GDNLIB_GDNS_RE_PATTERN):
    assert(dir.file_exists(gdnlib_gdns))
    
    var err := dir.copy(gdnlib_gdns, AddonConstants.ADDON_DIR.plus_file(gdnlib_gdns.get_file()))
    assert(err == OK)


func _copy_release_binaries(release: String) -> void:
  var dir := Directory.new()
  
  for gdnlib in DirUtils.get_dir_files_by_regex(AddonConstants.ADDON_DIR, 
                                                AddonConstants.GDNLIB_RE_PATTERN):
    assert(dir.file_exists(gdnlib))
    
    var library := ResourceLoader.load(gdnlib, 'GDNativeLibrary', true) as GDNativeLibrary
    
    for property_dict in library.get_property_list():
      var property_name := property_dict['name'] as String

      if not property_name.begins_with('entry/'):
        continue

      var binary_filepath := library.get(property_name) as String
      var binary_filename := binary_filepath.get_file()
      
      var asset_filepath := AddonConstants.get_asset_filepath(release, binary_filename)

      if not dir.file_exists(asset_filepath):
        continue

      var err := dir.copy(asset_filepath, binary_filepath)
      assert(err == OK)


func get_plugin_name() -> String:
  return 'gdn-sqlite'


func get_plugin_icon() -> Texture:
  return preload("res://addons/gdn-sqlite/icon.svg")
