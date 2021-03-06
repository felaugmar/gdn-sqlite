#pragma once

#include <sqlite3.h>

#include <Godot.hpp>
#include <memory>

void destroy_connection(sqlite3* connection);
void update_callback(void* udp, int type, const char* db_name,
                     const char* tbl_name, sqlite3_int64 rowid);

namespace gdn {
namespace sqlite {

class SQLite;
class SQLiteStatement;

class SQLiteDatabase : public godot::Reference {
  GODOT_CLASS(SQLiteDatabase, Reference)

 public:
  using connection_ptr =
      std::unique_ptr<sqlite3, decltype(&destroy_connection)>;

  static void _register_methods();

  SQLiteDatabase();
  ~SQLiteDatabase();

  void _init();

  /// Result code
  ////////////////////////////////////////////////////////////

  /// Error Codes And Messages.
  /// Current result code of this database instance.
  /// See: https://www.sqlite.org/c3ref/errcode.html
  int errcode() const;

  /// Error Codes And Messages.
  /// Current extended result code of this database instance.
  /// See: https://www.sqlite.org/c3ref/errcode.html
  int extended_errcode() const;

  /// Error Codes And Messages.
  /// English-language text that describes the error.
  /// See: https://www.sqlite.org/c3ref/errcode.html
  godot::String errmsg() const;

  ////////////////////////////////////////////////////////////

  /// Low-level system error code
  /// Attempt to return the underlying operating system error code or error
  /// number that caused the most recent I/O error or failure to open a file.
  /// See: https://www.sqlite.org/c3ref/system_errno.html
  int system_errno() const;

  /// Compiling An SQL Statement.
  /// See: https://www.sqlite.org/c3ref/prepare.html
  godot::Ref<SQLiteStatement> prepare(godot::String sql);

  /// Last Insert Rowid
  /// Returns the rowid of the most recent successful INSERT
  /// into a rowid table or virtual table.
  /// See: https://www.sqlite.org/c3ref/last_insert_rowid.html
  int64_t last_insert_rowid() const;

  /// Set the Last Insert Rowid value
  /// Set the value returned by calling `last_insert_rowid()`
  /// without inserting a row into the database.
  /// See: https://www.sqlite.org/c3ref/set_last_insert_rowid.html
  void set_last_insert_rowid(int64_t last_insert_rowid);

  /// Closes the database.
  void close();

  /// Whether or not the database is closed.
  bool is_closed() const;

  /// Database path.
  godot::String get_path() const;

  /// Enables or disables the signal `updated`.
  /// The signal `updated` is called for every rowid update.
  void set_updated(bool enable);

  /// Whether or not the signal `updated` is enabled.
  bool is_updated_enabled() const;

 protected:
  friend class SQLite;
  friend class SQLiteStatement;

  /// Opens the database
  int open(godot::String path);

  godot::String path;

  bool updated_enabled = false;

 protected:
  connection_ptr connection;
};

};  // namespace sqlite
};  // namespace gdn