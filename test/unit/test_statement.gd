extends "res://addons/gut/test.gd"

const DB_PATH := 'user://gdn-sqlite/test_stmt.db3'

const DUMMY_SQL := 'SELECT 1'
const DUMMY_BIND_SQL := 'SELECT :p1, 2, :p3'
const DUMMY_COLUMN_SQL := 'SELECT 1, "2", 3.14, null'

const SQLite = preload('res://addons/gdn-sqlite/sqlite.gdns')
const SQLiteDB = preload('res://addons/gdn-sqlite/database.gdns')
const SQLiteStmt = preload('res://addons/gdn-sqlite/statement.gdns')

var sqlite : SQLite
var _db


func before_all() -> void:
  var dir := Directory.new()
  
  var err := dir.make_dir_recursive(DB_PATH.get_base_dir())
  assert_true(err == OK or err == ERR_ALREADY_EXISTS)
  
  sqlite = SQLite.new()
  
  _db = sqlite.open(DB_PATH)


func after_all() -> void:
  var dir := Directory.new()
  
  _db.close()
  
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
  
  assert_eq(stmt.step(), sqlite.ROW)
  assert_eq(stmt.step(), sqlite.DONE)


func test_reset() -> void:
  var stmt := _prepare_dummy_stmt()
  
  # before step
  assert_eq(stmt.reset(), sqlite.OK)
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  # on ROW
  assert_eq(stmt.reset(), sqlite.OK)
  
  err = stmt.step()
  assert(err == sqlite.ROW)
  
  err = stmt.step()
  assert(err == sqlite.DONE)
  
  # on DONE
  assert_eq(stmt.reset(), sqlite.OK)


func test_busy() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_false(stmt.busy())
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  assert_true(stmt.busy())


func test_readonly() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_true(stmt.readonly())
  

func test_status() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_eq(stmt.status(sqlite.STMTSTATUS_RUN, false), 0)
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  err = stmt.reset()
  assert(err == sqlite.OK)
  
  assert_eq(stmt.status(sqlite.STMTSTATUS_RUN, false), 1)


func test_get_database() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_not_null(stmt.get_database())


func test_to_string() -> void:
  var stmt := _prepare_dummy_stmt()
  
  assert_eq(str(stmt), DUMMY_SQL)


func test_bind() -> void:
  var stmt := _prepare_dummy_bind_stmt()
  
  assert_eq(stmt.bind(0, 123), sqlite.RANGE)
  assert_eq(stmt.bind(1, 123), sqlite.OK)
  assert_eq(stmt.bind(2, 123), sqlite.OK)
  assert_eq(stmt.bind(3, 123), sqlite.RANGE)


func test_parameter_count() -> void:
  var stmt := _prepare_dummy_bind_stmt()
  
  assert_eq(stmt.parameter_count(), 2)


func test_parameter_index() -> void:
  var stmt := _prepare_dummy_bind_stmt()
  
  assert_eq(stmt.parameter_index(':p1'), 1)
  assert_eq(stmt.parameter_index(':p2'), 0) # not found == 0
  assert_eq(stmt.parameter_index(':p3'), 2)


func test_parameter_name() -> void:
  var stmt := _prepare_dummy_bind_stmt()
  
  assert_eq(stmt.parameter_name(0), '')
  assert_eq(stmt.parameter_name(1), ':p1')
  assert_eq(stmt.parameter_name(2), ':p3')
  assert_eq(stmt.parameter_name(3), '')


func test_clear_bindings() -> void:
  var stmt := _prepare_dummy_bind_stmt()
  
  assert_eq(stmt.clear_bindings(), sqlite.OK)


func test_bind_named() -> void:
  var stmt := _prepare_dummy_bind_stmt()
  
  assert_eq(stmt.bind_named(':p1', 123), sqlite.OK)
  assert_eq(stmt.bind_named(':p2', 123), sqlite.RANGE)
  assert_eq(stmt.bind_named(':p3', 123), sqlite.OK)


func test_bind_array() -> void:
  var stmt := _prepare_dummy_bind_stmt()
  
  assert_eq(stmt.bind_array([123, 123]), sqlite.OK)
  assert_eq(stmt.bind_array([123, 123, 123]), sqlite.RANGE)


func test_bind_dict() -> void:
  var stmt := _prepare_dummy_bind_stmt()
  
  # named only
  assert_eq(stmt.bind_dict({
      ':p1': 123,
      ':p3': 123
  }), sqlite.OK)
  
  assert_eq(stmt.bind_dict({
      ':p1': 123,
      ':p2': 123,
      ':p3': 123
  }), sqlite.RANGE)
  
  # index only
  assert_eq(stmt.bind_dict({
      1: 123,
      2: 123
  }), sqlite.OK)
  
  assert_eq(stmt.bind_dict({
      1: 123,
      2: 123,
      3: 123
  }), sqlite.RANGE)
  
  # mixing index and named (1 == :p1, both will bind and the latest will keep, but it's fine)
  assert_eq(stmt.bind_dict({
      1: 123,
      ':p1': 123,
      ':p3': 123
  }), sqlite.OK)


func test_column() -> void:
  var stmt := _prepare_dummy_column_stmt()
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  assert_null(stmt.column(-1))
  
  assert_eq(stmt.column(0), 1)
  assert_eq(stmt.column(1), '2')
  assert_eq(stmt.column(2), 3.14)
  assert_null(stmt.column(3))
  
  assert_null(stmt.column(4))


func test_column_bytes() -> void:
  var stmt := _prepare_dummy_column_stmt()
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  # `column_bytes` works for TEXT and BLOB.
  # other types will be converted before applying.
  # See: https://www.sqlite.org/c3ref/column_blob.html
  
  assert_eq(stmt.column_bytes(0), 1)
  assert_eq(stmt.column_bytes(1), 1)
  assert_eq(stmt.column_bytes(2), 4)
  assert_eq(stmt.column_bytes(3), 0)


func test_column_type() -> void:
  var stmt := _prepare_dummy_column_stmt()
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  assert_eq(stmt.column_type(0), sqlite.INTEGER)
  assert_eq(stmt.column_type(1), sqlite.TEXT)
  assert_eq(stmt.column_type(2), sqlite.FLOAT)
  assert_eq(stmt.column_type(3), sqlite.NULL)


func test_column_count() -> void:
  var stmt := _prepare_dummy_column_stmt()
  
  assert_eq(stmt.column_count(), 4)
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  assert_eq(stmt.column_count(), 4)


func test_data_count() -> void:
  var stmt := _prepare_dummy_column_stmt()
  
  assert_eq(stmt.data_count(), 0)
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  assert_eq(stmt.data_count(), 4)


func test_column_name() -> void:
  var stmt := _prepare_dummy_column_stmt()
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  assert_eq(stmt.column_name(-1), '')
  
  assert_eq(stmt.column_name(0), '1')
  assert_eq(stmt.column_name(1), '"2"')
  assert_eq(stmt.column_name(2), '3.14')
  assert_eq(stmt.column_name(3), 'null')
  
  assert_eq(stmt.column_name(4), '')


func test_column_decltype() -> void:
  var stmt = _prepare_dummy_column_stmt()
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  # blank for 'not table column'
  for i in range(0, 4):
    assert_eq(stmt.column_decltype(i), '')
  
  # if the result comes from a table's column without modification,
  # it should have the same declared type as in the CREATE stmt.
  var dummy_table_create := 'CREATE TABLE IF NOT EXISTS t1(c1 VARIANT)'
  var dummy_table_insert := 'INSERT INTO t1 VALUES (null)'
  var dummy_table_select := 'SELECT c1 + 1, c1 FROM t1'
  var dummy_table_drop   := 'DROP TABLE IF EXISTS t1'
  
  err = _db.prepare(dummy_table_create).step()
  assert(err == sqlite.DONE)
  
  err = _db.prepare(dummy_table_insert).step()
  assert(err == sqlite.DONE)
  
  stmt = _db.prepare(dummy_table_select)
  
  err = stmt.step()
  assert(err == sqlite.ROW)
  
  assert_eq(stmt.column_decltype(0), '')
  assert_eq(stmt.column_decltype(1), 'VARIANT')
  
  err = _db.prepare(dummy_table_drop).step()
  assert(err == sqlite.LOCKED)
  
  stmt.finalize()
  
  err = _db.prepare(dummy_table_drop).step()
  assert(err == sqlite.DONE)


func test_columns_array() -> void:
  var stmt := _prepare_dummy_column_stmt()
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  assert_eq(stmt.columns_array(), [1, '2', 3.14, null])


func test_columns_dict() -> void:
  var stmt := _prepare_dummy_column_stmt()
  
  var err = stmt.step()
  assert(err == sqlite.ROW)
  
  var columns_dict_hash = stmt.columns_dict().hash()
  var expected_hash = {
    '1': 1,
    '"2"': '2',
    '3.14': 3.14,
    'null': null
  }.hash()
  
  assert_eq(columns_dict_hash, expected_hash)


func test_bind_column() -> void:
  var stmt := _prepare_dummy_bind_stmt()
  
  var values_to_test := [
    null,
    
    # atomic types
    true,
    123,
    123.123,
    '123',
    
    # math types
    Vector2.ZERO,
    Rect2(0, 0, 0, 0),
    Vector3.ZERO,
    Transform2D.IDENTITY,
    Plane.PLANE_XY,
    Quat.IDENTITY,
    AABB(Vector3.ZERO, Vector3.ZERO),
    Basis.IDENTITY,
    Transform.IDENTITY,
    
    # misc types
    Color.white,
    NodePath('.'),
    RID(),
    self,
    { 'test': 123 },
    [ 'test', 123 ],
    
    # arrays
    PoolByteArray([0, 255]),
    PoolIntArray([123, 321]),
    PoolRealArray([123.123, 321.321]),
    PoolStringArray(['123', '321']),
    PoolVector2Array([Vector2.ZERO, Vector2.ONE]),
    PoolVector3Array([Vector3.ZERO, Vector3.ONE]),
    PoolColorArray([Color.white, Color.black])
  ]
  
  var err
  
  for value in values_to_test:
    assert_eq(stmt.bind(1, value), sqlite.OK)
    
    err = stmt.step()
    assert(err == sqlite.ROW)
    
    match typeof(value):
      TYPE_BOOL:
        # booleans are stored and returned as int
        assert_eq(bool(stmt.column(0)), value)
      TYPE_DICTIONARY:
        var col_value = stmt.column(0)
        
        assert_typeof(col_value, TYPE_DICTIONARY)
        assert_eq(col_value.hash(), value.hash())
      TYPE_OBJECT:
        # objects are stored and returned as EncodedObjectAsID 
        var col_value = stmt.column(0)
        
        assert_true(col_value is EncodedObjectAsID)
        assert_eq(instance_from_id(col_value.object_id), value)
      _:
        assert_eq(stmt.column(0), value)
    
    err = stmt.reset()
    assert(err == sqlite.OK)
    
    err = stmt.clear_bindings()
    assert(err == sqlite.OK)


func test_sql() -> void:
  assert_eq(_prepare_dummy_stmt().sql(), DUMMY_SQL)
  assert_eq(_prepare_dummy_bind_stmt().sql(), DUMMY_BIND_SQL)
  assert_eq(_prepare_dummy_column_stmt().sql(), DUMMY_COLUMN_SQL)


func test_expanded_sql() -> void:
  assert_eq(_prepare_dummy_stmt().expanded_sql(), DUMMY_SQL)
  
  ###
  var stmt := _prepare_dummy_bind_stmt()
  
  assert_eq(stmt.expanded_sql(), DUMMY_BIND_SQL.replace(':p1', 'NULL').replace(':p3', 'NULL'))
  
  var err = stmt.bind_array([1, 3])
  assert(err == sqlite.OK)
  
  assert_eq(stmt.expanded_sql(), DUMMY_BIND_SQL.replace(':p1', '1').replace(':p3', '3'))
  ###
  
  assert_eq(_prepare_dummy_column_stmt().expanded_sql(), DUMMY_COLUMN_SQL)


func _prepare_dummy_stmt() -> SQLiteStmt:
  return _db.prepare(DUMMY_SQL)


func _prepare_dummy_bind_stmt() -> SQLiteStmt:
  return _db.prepare(DUMMY_BIND_SQL)


func _prepare_dummy_column_stmt() -> SQLiteStmt:
  return _db.prepare(DUMMY_COLUMN_SQL)
