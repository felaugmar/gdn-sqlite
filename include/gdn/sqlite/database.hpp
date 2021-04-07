#pragma once

#include <sqlite3.h>

#include <Godot.hpp>
#include <memory>

void destroy_connection(sqlite3 *connection);

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

  /// Prepare this statement with this database and SQL.
  /// See: https://www.sqlite.org/c3ref/prepare.html
  godot::Ref<SQLiteStatement> prepare(godot::String sql);

  /// Closes the database
  void close();

  bool is_closed() const;

  godot::String get_path() const;

 protected:
  friend class SQLite;
  friend class SQLiteStatement;

  /// Opens the database
  int open(godot::String path);

  godot::String path;

 protected:
  connection_ptr connection;
};

};  // namespace sqlite
};  // namespace gdn