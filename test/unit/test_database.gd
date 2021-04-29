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


func test_last_insert_rowid() -> void:
  var db := _open_db()
  
  assert_eq(db.last_insert_rowid(), 0)
  
  var create := 'CREATE TABLE test (id INTEGER PRIMARY KEY, name TEXT);'
  var insert := "INSERT INTO test (name) VALUES ('hehe');"
  var insert_with_id := "INSERT INTO test (id, name) VALUES (5, 'haha');"
  var drop := 'DROP TABLE test;'
  
  var err = db.prepare(create).step()
  assert(err == sqlite.DONE)
  
  # test auto incremented rowid
  err = db.prepare(insert).step()
  assert(err == sqlite.DONE)
  
  assert_eq(db.last_insert_rowid(), 1)
  
  # test defined rowid
  err = db.prepare(insert_with_id).step()
  assert(err == sqlite.DONE)
  
  assert_eq(db.last_insert_rowid(), 5)
  
  err = db.prepare(drop).step()
  assert(err == sqlite.DONE)


func test_set_last_insert_rowid() -> void:
  var db := _open_db()
  
  db.set_last_insert_rowid(2)
  
  assert_eq(db.last_insert_rowid(), 2)


func test_get_path() -> void:
  var db := _open_db()
  
  assert_eq(db.get_path(), DB_PATH)


func test_signal_updated() -> void:
  var db := _open_db()
  
  assert_false(db.is_updated_enabled())
  
  watch_signals(db)
  
  var create := 'CREATE TABLE test (id INTEGER PRIMARY KEY, name TEXT);'
  var insert := "INSERT INTO test (name) VALUES ('hehe');"
  var update := "UPDATE test SET name = 'haha' WHERE id = 2;"
  var delete := "DELETE FROM test WHERE id = 2;"
  var drop := 'DROP TABLE test;'
  
  var err = db.prepare(create).step()
  assert(err == sqlite.DONE)
  
  # `updated` disabled by default
  err = db.prepare(insert).step()
  assert(err == sqlite.DONE)
  
  assert_signal_not_emitted(db, 'updated')
  
  db.set_updated(true)
  
  assert_true(db.is_updated_enabled())
  
  # `updated` enabled INSERT
  err = db.prepare(insert).step()
  assert(err == sqlite.DONE)
  
  assert_signal_emitted_with_parameters(db, 'updated', [sqlite.INSERT, 'main', 'test', 2])
  
  # `updated` enabled UPDATE  
  err = db.prepare(update).step()
  assert(err == sqlite.DONE)
  
  assert_signal_emitted_with_parameters(db, 'updated', [sqlite.UPDATE, 'main', 'test', 2])
  
  # `updated` enabled DELETE  
  err = db.prepare(delete).step()
  assert(err == sqlite.DONE)
  
  assert_signal_emitted_with_parameters(db, 'updated', [sqlite.DELETE, 'main', 'test', 2])
  
  assert_signal_emit_count(db, 'updated', 3)
  
  # `updated` disabled
  db.set_updated(false)
  
  assert_false(db.is_updated_enabled())
  
  err = db.prepare(insert).step()
  assert(err == sqlite.DONE)  
  
  assert_signal_emit_count(db, 'updated', 3)
  
  err = db.prepare(drop).step()
  assert(err == sqlite.DONE)  


func test_to_string() -> void:
  var db := _open_db()
  
  assert_eq(str(db), DB_PATH);


func _open_db() -> SQLiteDB:
  sqlite = SQLite.new()
  
  return sqlite.open(DB_PATH)
