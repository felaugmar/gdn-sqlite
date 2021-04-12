#include "gdn/sqlite/sqlite.hpp"

#include <sqlite3.h>

#include "gdn/sqlite/database.hpp"

#define CONSTANT_PAIR(Const) \
  { #Const, SQLITE_##Const }

std::unordered_map<std::string, int32_t>
    gdn::sqlite::SQLite::constants_int32_t = {
        // Fundamental Datatypes
        CONSTANT_PAIR(INTEGER),
        CONSTANT_PAIR(FLOAT),
        CONSTANT_PAIR(TEXT),
        CONSTANT_PAIR(BLOB),
        CONSTANT_PAIR(NULL),

        // Result Codes
        CONSTANT_PAIR(OK),
        CONSTANT_PAIR(ERROR),
        CONSTANT_PAIR(INTERNAL),
        CONSTANT_PAIR(PERM),
        CONSTANT_PAIR(ABORT),
        CONSTANT_PAIR(BUSY),
        CONSTANT_PAIR(LOCKED),
        CONSTANT_PAIR(NOMEM),
        CONSTANT_PAIR(READONLY),
        CONSTANT_PAIR(INTERRUPT),
        CONSTANT_PAIR(IOERR),
        CONSTANT_PAIR(CORRUPT),
        CONSTANT_PAIR(NOTFOUND),
        CONSTANT_PAIR(FULL),
        CONSTANT_PAIR(CANTOPEN),
        CONSTANT_PAIR(PROTOCOL),
        CONSTANT_PAIR(EMPTY),
        CONSTANT_PAIR(SCHEMA),
        CONSTANT_PAIR(TOOBIG),
        CONSTANT_PAIR(CONSTRAINT),
        CONSTANT_PAIR(MISMATCH),
        CONSTANT_PAIR(MISUSE),
        CONSTANT_PAIR(NOLFS),
        CONSTANT_PAIR(AUTH),
        CONSTANT_PAIR(FORMAT),
        CONSTANT_PAIR(RANGE),
        CONSTANT_PAIR(NOTADB),
        CONSTANT_PAIR(NOTICE),
        CONSTANT_PAIR(WARNING),
        CONSTANT_PAIR(ROW),
        CONSTANT_PAIR(DONE),

        // Extended Result Codes
        CONSTANT_PAIR(ERROR_MISSING_COLLSEQ),
        CONSTANT_PAIR(ERROR_RETRY),
        CONSTANT_PAIR(ERROR_SNAPSHOT),
        CONSTANT_PAIR(IOERR_READ),
        CONSTANT_PAIR(IOERR_SHORT_READ),
        CONSTANT_PAIR(IOERR_WRITE),
        CONSTANT_PAIR(IOERR_FSYNC),
        CONSTANT_PAIR(IOERR_DIR_FSYNC),
        CONSTANT_PAIR(IOERR_TRUNCATE),
        CONSTANT_PAIR(IOERR_FSTAT),
        CONSTANT_PAIR(IOERR_UNLOCK),
        CONSTANT_PAIR(IOERR_RDLOCK),
        CONSTANT_PAIR(IOERR_DELETE),
        CONSTANT_PAIR(IOERR_BLOCKED),
        CONSTANT_PAIR(IOERR_NOMEM),
        CONSTANT_PAIR(IOERR_ACCESS),
        CONSTANT_PAIR(IOERR_CHECKRESERVEDLOCK),
        CONSTANT_PAIR(IOERR_LOCK),
        CONSTANT_PAIR(IOERR_CLOSE),
        CONSTANT_PAIR(IOERR_DIR_CLOSE),
        CONSTANT_PAIR(IOERR_SHMOPEN),
        CONSTANT_PAIR(IOERR_SHMSIZE),
        CONSTANT_PAIR(IOERR_SHMLOCK),
        CONSTANT_PAIR(IOERR_SHMMAP),
        CONSTANT_PAIR(IOERR_SEEK),
        CONSTANT_PAIR(IOERR_DELETE_NOENT),
        CONSTANT_PAIR(IOERR_MMAP),
        CONSTANT_PAIR(IOERR_GETTEMPPATH),
        CONSTANT_PAIR(IOERR_CONVPATH),
        CONSTANT_PAIR(IOERR_VNODE),
        CONSTANT_PAIR(IOERR_AUTH),
        CONSTANT_PAIR(IOERR_BEGIN_ATOMIC),
        CONSTANT_PAIR(IOERR_COMMIT_ATOMIC),
        CONSTANT_PAIR(IOERR_ROLLBACK_ATOMIC),
        CONSTANT_PAIR(IOERR_DATA),
        CONSTANT_PAIR(IOERR_CORRUPTFS),
        CONSTANT_PAIR(LOCKED_SHAREDCACHE),
        CONSTANT_PAIR(LOCKED_VTAB),
        CONSTANT_PAIR(BUSY_RECOVERY),
        CONSTANT_PAIR(BUSY_SNAPSHOT),
        CONSTANT_PAIR(BUSY_TIMEOUT),
        CONSTANT_PAIR(CANTOPEN_NOTEMPDIR),
        CONSTANT_PAIR(CANTOPEN_ISDIR),
        CONSTANT_PAIR(CANTOPEN_FULLPATH),
        CONSTANT_PAIR(CANTOPEN_CONVPATH),
        CONSTANT_PAIR(CANTOPEN_DIRTYWAL),
        CONSTANT_PAIR(CANTOPEN_SYMLINK),
        CONSTANT_PAIR(CORRUPT_VTAB),
        CONSTANT_PAIR(CORRUPT_SEQUENCE),
        CONSTANT_PAIR(CORRUPT_INDEX),
        CONSTANT_PAIR(READONLY_RECOVERY),
        CONSTANT_PAIR(READONLY_CANTLOCK),
        CONSTANT_PAIR(READONLY_ROLLBACK),
        CONSTANT_PAIR(READONLY_DBMOVED),
        CONSTANT_PAIR(READONLY_CANTINIT),
        CONSTANT_PAIR(READONLY_DIRECTORY),
        CONSTANT_PAIR(ABORT_ROLLBACK),
        CONSTANT_PAIR(CONSTRAINT_CHECK),
        CONSTANT_PAIR(CONSTRAINT_COMMITHOOK),
        CONSTANT_PAIR(CONSTRAINT_FOREIGNKEY),
        CONSTANT_PAIR(CONSTRAINT_FUNCTION),
        CONSTANT_PAIR(CONSTRAINT_NOTNULL),
        CONSTANT_PAIR(CONSTRAINT_PRIMARYKEY),
        CONSTANT_PAIR(CONSTRAINT_TRIGGER),
        CONSTANT_PAIR(CONSTRAINT_UNIQUE),
        CONSTANT_PAIR(CONSTRAINT_VTAB),
        CONSTANT_PAIR(CONSTRAINT_ROWID),
        CONSTANT_PAIR(CONSTRAINT_PINNED),
        CONSTANT_PAIR(NOTICE_RECOVER_WAL),
        CONSTANT_PAIR(NOTICE_RECOVER_ROLLBACK),
        CONSTANT_PAIR(WARNING_AUTOINDEX),
        CONSTANT_PAIR(AUTH_USER),
        CONSTANT_PAIR(OK_LOAD_PERMANENTLY),
        CONSTANT_PAIR(OK_SYMLINK),

        // Status Parameters for prepared statements
        CONSTANT_PAIR(STMTSTATUS_FULLSCAN_STEP),
        CONSTANT_PAIR(STMTSTATUS_SORT),
        CONSTANT_PAIR(STMTSTATUS_AUTOINDEX),
        CONSTANT_PAIR(STMTSTATUS_VM_STEP),
        CONSTANT_PAIR(STMTSTATUS_REPREPARE),
        CONSTANT_PAIR(STMTSTATUS_RUN),
        CONSTANT_PAIR(STMTSTATUS_MEMUSED),
};

void gdn::sqlite::SQLite::_register_methods() {
  godot::register_method("errstr", &SQLite::errstr);

  godot::register_method("version", &SQLite::version);
  godot::register_method("version_number", &SQLite::version_number);
  godot::register_method("source_id", &SQLite::source_id);

  godot::register_method("open", &SQLite::open);

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

  for (auto constant : constants_int32_t)
    properties.append(godot::Dictionary::make(
        "name", constant.first.data(), "type", godot::Variant::Type::INT));

  return properties;
}

godot::Variant gdn::sqlite::SQLite::_get(godot::String property) const {
  auto search = constants_int32_t.find(property.ascii().get_data());

  if (search != constants_int32_t.end()) return search->second;

  return {};
}
