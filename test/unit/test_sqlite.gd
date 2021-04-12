extends "res://addons/gut/test.gd"

const SQLite = preload('res://addons/gdn-sqlite/sqlite.gdns')

const EXPECTED_VERSION := '3.35.4'
const EXPECTED_VERSION_NUMBER := 3035004
const EXPECTED_SOURCE_ID := '2021-04-02 15:20:15 5d4c65779dab868b285519b19e4cf9d451d50c6048f06f653aa701ec212df45e'

var sqlite : SQLite


func before_all() -> void:
  sqlite = SQLite.new()


func after_all()  -> void:
  sqlite = null


func test_constants() -> void:
  assert_eq(sqlite.OK, 0)
  assert_eq(sqlite.ERROR, 1)
  assert_eq(sqlite.EMPTY, 16)
  # ...


func test_errstr() -> void:
  var err_msg_mapping := {
    sqlite.OK: 'not an error',
    sqlite.ERROR: 'SQL logic error',
    sqlite.INTERNAL: 'unknown error',
    sqlite.PERM: 'access permission denied',
    # ...
  }
  
  for err in err_msg_mapping:
    assert_eq(sqlite.errstr(err), err_msg_mapping[err])


func test_version() -> void:
  assert_eq(sqlite.version(), EXPECTED_VERSION)


func test_version_number() -> void:
  assert_eq(sqlite.version_number(), EXPECTED_VERSION_NUMBER)


func test_source_id() -> void:
  assert_eq(sqlite.source_id(), EXPECTED_SOURCE_ID)
