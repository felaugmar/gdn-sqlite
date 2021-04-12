tool

extends EditorPlugin

const GDNSQLiteLibrary = preload('res://addons/gdn-sqlite/gdn_sqlite.gdnlib')
const GDNSQLite = preload('res://addons/gdn-sqlite/gdn_sqlite.gdns')


func _enter_tree() -> void:
  _ensure_gdns_files()


func _ensure_gdns_files() -> void:
  var gdn_sqlite := GDNSQLite.new()
  
  var gdns_files := gdn_sqlite.gdns_files() as Array
  
  for gdns_file in gdns_files:
    var gdns_filename := gdns_file['filename'] as String
    var gdns_class_name := gdns_file['class_name'] as String

    var gdns_path := 'res://addons/gdn-sqlite/%s' % str(gdns_filename)

    var gdns : NativeScript

    if ResourceLoader.exists(gdns_path):
      gdns = ResourceLoader.load(gdns_path) as NativeScript
    else:
      gdns = NativeScript.new()

    var need_to_save := false

    if gdns.get_class_name() != gdns_class_name:
      gdns.set_class_name(gdns_class_name)

      need_to_save = true

    if gdns.get_library() != GDNSQLiteLibrary:
      gdns.set_library(GDNSQLiteLibrary)

      need_to_save = true

    if need_to_save:
      var err := ResourceSaver.save(gdns_path, gdns)
      assert(err == OK)


func get_plugin_name() -> String:
  return 'gdn-sqlite'


func get_plugin_icon() -> Texture:
  return preload("res://addons/gdn-sqlite/icon.svg")
