extends "res://addons/gut/test.gd"

const DB_PATH := 'user://gdn-sqlite/test_db.db3'

const SQLite = preload('res://addons/gdn-sqlite/sqlite.gdns')
const SQLiteDB = preload('res://addons/gdn-sqlite/database.gdns')

var sqlite : SQLite


func before_all() -> void:
  var dir := Directory.new()
  
  var err := dir.make_dir_recursive(DB_PATH.get_base_dir())
  assert_true(err == OK or err == ERR_ALREADY_EXISTS)


func after_all() -> void:
  var dir := Directory.new()
  
  if dir.file_exists(DB_PATH):
    var err := dir.remove(DB_PATH)
    assert_true(err == OK)


func test_open_close() -> void:
  var db := _open_db()
  
  assert_false(db.is_closed())
  
  db.close()
  
  assert_true(db.is_closed())


func test_errcode() -> void:
  var db := _open_db()
  
  assert_eq(db.errcode(), sqlite.OK)


func test_extended_errcode() -> void:
  var db := _open_db()
  
  assert_eq(db.extended_errcode(), sqlite.OK)


func test_errmsg() -> void:
  var db := _open_db()
  
  assert_eq(db.errmsg(), 'not an error')


func test_system_errno() -> void:
  var db := _open_db()
  
  assert_eq(db.system_errno(), sqlite.OK)


func test_get_path() -> void:
  var db := _open_db()
  
  assert_eq(db.get_path(), DB_PATH)


func test_to_string() -> void:
  var db := _open_db()
  
  assert_eq(str(db), DB_PATH);


func _open_db() -> SQLiteDB:
  sqlite = SQLite.new()
  
  return sqlite.open(DB_PATH)
