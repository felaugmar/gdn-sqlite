#include "gdn/sqlite/sqlite.hpp"

#include <sqlite3.h>

#include "gdn/sqlite/database.hpp"
#include "gdn/sqlite/godot_utils.hpp"

#define CONSTANT(Const, Category) \
  { #Const, SQLITE_##Const, Category }

#define FD_CONSTANT(Const) CONSTANT(Const, "Fundamental Datatypes")

#define RC_CONSTANT(Const) CONSTANT(Const, "Result Codes")

#define ERC_CONSTANT(Const) CONSTANT(Const, "Extended Result Codes")

#define SPPS_CONSTANT(Const) \
  CONSTANT(Const, "Status Parameters for Prepared Statements")

#define AC_CONSTANT(Const) CONSTANT(Const, "Action Codes")

const gdn::sqlite::ConstantMapping<int32_t> gdn::sqlite::SQLite::constants_int32_t = {
    // Fundamental Datatypes
    FD_CONSTANT(INTEGER),
    FD_CONSTANT(FLOAT),
    FD_CONSTANT(TEXT),
    FD_CONSTANT(BLOB),
    CONSTANT(NULL, "Fundamental Datatypes"),

    // Result Codes
    RC_CONSTANT(OK),
    RC_CONSTANT(ERROR),
    RC_CONSTANT(INTERNAL),
    RC_CONSTANT(PERM),
    RC_CONSTANT(ABORT),
    RC_CONSTANT(BUSY),
    RC_CONSTANT(LOCKED),
    RC_CONSTANT(NOMEM),
    RC_CONSTANT(READONLY),
    RC_CONSTANT(INTERRUPT),
    RC_CONSTANT(IOERR),
    RC_CONSTANT(CORRUPT),
    RC_CONSTANT(NOTFOUND),
    RC_CONSTANT(FULL),
    RC_CONSTANT(CANTOPEN),
    RC_CONSTANT(PROTOCOL),
    RC_CONSTANT(EMPTY),
    RC_CONSTANT(SCHEMA),
    RC_CONSTANT(TOOBIG),
    RC_CONSTANT(CONSTRAINT),
    RC_CONSTANT(MISMATCH),
    RC_CONSTANT(MISUSE),
    RC_CONSTANT(NOLFS),
    RC_CONSTANT(AUTH),
    RC_CONSTANT(FORMAT),
    RC_CONSTANT(RANGE),
    RC_CONSTANT(NOTADB),
    RC_CONSTANT(NOTICE),
    RC_CONSTANT(WARNING),
    RC_CONSTANT(ROW),
    RC_CONSTANT(DONE),

    // Extended Result Codes
    ERC_CONSTANT(ERROR_MISSING_COLLSEQ),
    ERC_CONSTANT(ERROR_RETRY),
    ERC_CONSTANT(ERROR_SNAPSHOT),
    ERC_CONSTANT(IOERR_READ),
    ERC_CONSTANT(IOERR_SHORT_READ),
    ERC_CONSTANT(IOERR_WRITE),
    ERC_CONSTANT(IOERR_FSYNC),
    ERC_CONSTANT(IOERR_DIR_FSYNC),
    ERC_CONSTANT(IOERR_TRUNCATE),
    ERC_CONSTANT(IOERR_FSTAT),
    ERC_CONSTANT(IOERR_UNLOCK),
    ERC_CONSTANT(IOERR_RDLOCK),
    ERC_CONSTANT(IOERR_DELETE),
    ERC_CONSTANT(IOERR_BLOCKED),
    ERC_CONSTANT(IOERR_NOMEM),
    ERC_CONSTANT(IOERR_ACCESS),
    ERC_CONSTANT(IOERR_CHECKRESERVEDLOCK),
    ERC_CONSTANT(IOERR_LOCK),
    ERC_CONSTANT(IOERR_CLOSE),
    ERC_CONSTANT(IOERR_DIR_CLOSE),
    ERC_CONSTANT(IOERR_SHMOPEN),
    ERC_CONSTANT(IOERR_SHMSIZE),
    ERC_CONSTANT(IOERR_SHMLOCK),
    ERC_CONSTANT(IOERR_SHMMAP),
    ERC_CONSTANT(IOERR_SEEK),
    ERC_CONSTANT(IOERR_DELETE_NOENT),
    ERC_CONSTANT(IOERR_MMAP),
    ERC_CONSTANT(IOERR_GETTEMPPATH),
    ERC_CONSTANT(IOERR_CONVPATH),
    ERC_CONSTANT(IOERR_VNODE),
    ERC_CONSTANT(IOERR_AUTH),
    ERC_CONSTANT(IOERR_BEGIN_ATOMIC),
    ERC_CONSTANT(IOERR_COMMIT_ATOMIC),
    ERC_CONSTANT(IOERR_ROLLBACK_ATOMIC),
    ERC_CONSTANT(IOERR_DATA),
    ERC_CONSTANT(IOERR_CORRUPTFS),
    ERC_CONSTANT(LOCKED_SHAREDCACHE),
    ERC_CONSTANT(LOCKED_VTAB),
    ERC_CONSTANT(BUSY_RECOVERY),
    ERC_CONSTANT(BUSY_SNAPSHOT),
    ERC_CONSTANT(BUSY_TIMEOUT),
    ERC_CONSTANT(CANTOPEN_NOTEMPDIR),
    ERC_CONSTANT(CANTOPEN_ISDIR),
    ERC_CONSTANT(CANTOPEN_FULLPATH),
    ERC_CONSTANT(CANTOPEN_CONVPATH),
    ERC_CONSTANT(CANTOPEN_DIRTYWAL),
    ERC_CONSTANT(CANTOPEN_SYMLINK),
    ERC_CONSTANT(CORRUPT_VTAB),
    ERC_CONSTANT(CORRUPT_SEQUENCE),
    ERC_CONSTANT(CORRUPT_INDEX),
    ERC_CONSTANT(READONLY_RECOVERY),
    ERC_CONSTANT(READONLY_CANTLOCK),
    ERC_CONSTANT(READONLY_ROLLBACK),
    ERC_CONSTANT(READONLY_DBMOVED),
    ERC_CONSTANT(READONLY_CANTINIT),
    ERC_CONSTANT(READONLY_DIRECTORY),
    ERC_CONSTANT(ABORT_ROLLBACK),
    ERC_CONSTANT(CONSTRAINT_CHECK),
    ERC_CONSTANT(CONSTRAINT_COMMITHOOK),
    ERC_CONSTANT(CONSTRAINT_FOREIGNKEY),
    ERC_CONSTANT(CONSTRAINT_FUNCTION),
    ERC_CONSTANT(CONSTRAINT_NOTNULL),
    ERC_CONSTANT(CONSTRAINT_PRIMARYKEY),
    ERC_CONSTANT(CONSTRAINT_TRIGGER),
    ERC_CONSTANT(CONSTRAINT_UNIQUE),
    ERC_CONSTANT(CONSTRAINT_VTAB),
    ERC_CONSTANT(CONSTRAINT_ROWID),
    ERC_CONSTANT(CONSTRAINT_PINNED),
    ERC_CONSTANT(NOTICE_RECOVER_WAL),
    ERC_CONSTANT(NOTICE_RECOVER_ROLLBACK),
    ERC_CONSTANT(WARNING_AUTOINDEX),
    ERC_CONSTANT(AUTH_USER),
    ERC_CONSTANT(OK_LOAD_PERMANENTLY),
    ERC_CONSTANT(OK_SYMLINK),

    // Status Parameters for Prepared Statements
    SPPS_CONSTANT(STMTSTATUS_FULLSCAN_STEP),
    SPPS_CONSTANT(STMTSTATUS_SORT),
    SPPS_CONSTANT(STMTSTATUS_AUTOINDEX),
    SPPS_CONSTANT(STMTSTATUS_VM_STEP),
    SPPS_CONSTANT(STMTSTATUS_REPREPARE),
    SPPS_CONSTANT(STMTSTATUS_RUN),
    SPPS_CONSTANT(STMTSTATUS_MEMUSED),

    // Action Codes
    AC_CONSTANT(INSERT),
    AC_CONSTANT(UPDATE),
    AC_CONSTANT(DELETE),
};

void gdn::sqlite::SQLite::_register_methods() {
  register_class_documentation<SQLite>("General Functions and Constants");

  godot::register_method("errstr", &SQLite::errstr);
  register_method_documentation<SQLite>(
      "errstr",
      "signature: errstr(result_code: int) -> String\n"
      "order: 0\n"
      "English-language text that describes the result code.");
  register_method_argument_information<SQLite>(
      "errstr", godot::Array::make(
                    get_arg_dict("result_code", godot::Variant::Type::INT)));

  godot::register_method("version", &SQLite::version);
  register_method_documentation<SQLite>(
      "version",
      "signature: version() -> String\n"
      "order: 1\n"
      "SQLite version in the format \"X.Y.Z\" where\n"
      "X is the major version number (always 3 for SQLite3)\n"
      "and Y is the minor version number and Z is the release number.");

  godot::register_method("version_number", &SQLite::version_number);
  register_method_documentation<SQLite>(
      "version_number",
      "signature: version_number() -> int\n"
      "order: 2\n"
      "SQLite version in the format(X*1000000 + Y*1000 + Z)\n"
      "where X, Y, and Z are the same numbers used in version().");

  godot::register_method("source_id", &SQLite::source_id);
  register_method_documentation<SQLite>(
      "source_id",
      "signature: source_id() -> String\n"
      "order: 3\n"
      "Contains the date and time of the check-in (UTC)\n"
      "and a SHA1 or SHA3-256 hash of the entire source tree.");

  godot::register_method("open", &SQLite::open);
  register_method_documentation<SQLite>(
      "open",
      "signature: open(path: String) -> SQLiteDatabase\n"
      "order: 4\n"
      "Opens the database at `path`.");
  register_method_argument_information<SQLite>(
      "open",
      godot::Array::make(get_arg_dict("path", godot::Variant::Type::STRING)));

  godot::register_method("_get_property_list", &SQLite::_get_property_list);
  godot::register_method("_get", &SQLite::_get);
}

gdn::sqlite::SQLite::SQLite() {}

gdn::sqlite::SQLite::~SQLite() {}

void gdn::sqlite::SQLite::_init() {}

godot::String gdn::sqlite::SQLite::errstr(int result_code) const {
  return {sqlite3_errstr(result_code)};
}

godot::String gdn::sqlite::SQLite::version() const { return {SQLITE_VERSION}; }

uint32_t gdn::sqlite::SQLite::version_number() const {
  return SQLITE_VERSION_NUMBER;
}

godot::String gdn::sqlite::SQLite::source_id() const {
  return {SQLITE_SOURCE_ID};
}

godot::Ref<gdn::sqlite::SQLiteDatabase> gdn::sqlite::SQLite::open(
    godot::String path) const {
  godot::Ref<SQLiteDatabase> database = SQLiteDatabase::_new();

  database->open(path);

  return database;
}

godot::Array gdn::sqlite::SQLite::_get_property_list() const {
  godot::Array properties;

  for (const auto& constant : constants_int32_t.get_constants()) {
    std::string category;

    try {
      category = constants_int32_t.get_category(constant);
    } catch (const std::out_of_range&) {
    }

    properties.append(godot::Dictionary::make(
        "name", constant.data(), "type", godot::Variant::Type::INT, "hint",
        GODOT_PROPERTY_HINT_NONE, "hint_string",
        "constant\n" + godot::String("category: {0}")
                           .format(godot::Array::make(category.data()))));
  }

  return properties;
}

godot::Variant gdn::sqlite::SQLite::_get(godot::String property) const {
  try {
    return constants_int32_t.get_value(property.ascii().get_data());
  } catch (const std::out_of_range&) {
    return {};
  }
}
