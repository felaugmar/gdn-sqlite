extends "res://addons/gut/test.gd"

const DB_PATH := 'user://gdn-sqlite/test_stmt.db3'

const DUMMY_SQL := 'SELECT 1'

const SQLite = preload('res://addons/gdn-sqlite/sqlite.gdns')
const SQLiteConst = preload('res://addons/gdn-sqlite/constants.gd')
const SQLiteDB = preload('res://addons/gdn-sqlite/database.gdns')
const SQLiteStmt = preload('res://addons/gdn-sqlite/statement.gdns')


func before_all() -> void:
  var dir := Directory.new()
  
  var err := dir.make_dir_recursive(DB_PATH.get_base_dir())
  assert_true(err == OK or err == ERR_ALREADY_EXISTS)


func after_all() -> void:
  var dir := Directory.new()
  
  if dir.file_exists(DB_PATH):
    var err := dir.remove(DB_PATH)
    assert_true(err == OK)


func test_prepare_finalize() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_false(stmt.is_finalized())
  
  stmt.finalize()
  
  assert_true(stmt.is_finalized())


func test_step() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_eq(stmt.step(), SQLiteConst.ROW)
  assert_eq(stmt.step(), SQLiteConst.DONE)


func test_reset() -> void:
  var stmt := _prepare_dummy_stmt()
  
  # before step
  assert_eq(stmt.reset(), SQLiteConst.OK)
  
  var err = stmt.step()
  assert(err == SQLiteConst.ROW)
  
  # on ROW
  assert_eq(stmt.reset(), SQLiteConst.OK)
  
  err = stmt.step()
  assert(err == SQLiteConst.ROW)
  
  err = stmt.step()
  assert(err == SQLiteConst.DONE)
  
  # on DONE
  assert_eq(stmt.reset(), SQLiteConst.OK)


func test_busy() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_false(stmt.busy())
  
  var err = stmt.step()
  assert(err == SQLiteConst.ROW)
  
  assert_true(stmt.busy())


func test_readonly() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_true(stmt.readonly())
  

func test_status() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_eq(stmt.status(SQLiteConst.STMTSTATUS_RUN, false), 0)
  
  var err = stmt.step()
  assert(err == SQLiteConst.ROW)
  
  err = stmt.reset()
  assert(err == SQLiteConst.OK)
  
  assert_eq(stmt.status(SQLiteConst.STMTSTATUS_RUN, false), 1)


func test_get_database() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_not_null(stmt.get_database())


func test_to_string() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_eq(str(stmt), DUMMY_SQL)


func _open_db() -> SQLiteDB:
  var sqlite := SQLite.new()
  
  return sqlite.open(DB_PATH)


func _prepare_dummy_stmt() -> SQLiteStmt:
  var db := _open_db()
  
  return db.prepare(DUMMY_SQL)
