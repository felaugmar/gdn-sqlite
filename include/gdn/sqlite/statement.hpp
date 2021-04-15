#pragma once

#include <sqlite3.h>

#include <Godot.hpp>

void finalize_stmt(sqlite3_stmt *stmt);

namespace gdn {
namespace sqlite {

class SQLiteDatabase;

class SQLiteStatement : public godot::Reference {
  GODOT_CLASS(SQLiteStatement, Reference)

 public:
  using stmt_ptr = std::unique_ptr<sqlite3_stmt, decltype(&finalize_stmt)>;

  static void _register_methods();

  SQLiteStatement();
  ~SQLiteStatement();

  void _init();

  /// Evaluate An SQL Statement.
  /// See: https://www.sqlite.org/c3ref/step.html
  int step();

  /// Reset A Prepared Statement Object.
  /// See: https://www.sqlite.org/c3ref/reset.html
  int reset();

  /// Determine If A Prepared Statement Has Been Reset.
  /// See: https://www.sqlite.org/c3ref/stmt_busy.html
  bool busy() const;

  /// Determine If An SQL Statement Writes The Database.
  /// See: https://www.sqlite.org/c3ref/stmt_readonly.html
  bool readonly() const;

  /// Prepared Statement Status.
  /// See: https://www.sqlite.org/c3ref/stmt_status.html
  int status(int operation, bool reset);

  /// Get the database this statement belongs to.
  godot::Ref<SQLiteDatabase> get_database() const;

  /// Bind
  ////////////////////////////////////////////////////////////

  /// Binding Values To Prepared Statements.
  /// See: https://www.sqlite.org/c3ref/bind_blob.html
  int bind(int index, godot::Variant data);

  /// Number Of SQL Parameters.
  /// See: https://www.sqlite.org/c3ref/bind_parameter_count.html
  int parameter_count() const;

  /// Index Of A Parameter With A Given Name.
  /// See: https://www.sqlite.org/c3ref/bind_parameter_index.html
  int parameter_index(godot::String name) const;

  /// Name Of A Host Parameter.
  /// See: https://www.sqlite.org/c3ref/bind_parameter_name.html
  godot::String parameter_name(int index) const;

  /// Reset All Bindings On A Prepared Statement.
  /// See: https://www.sqlite.org/c3ref/clear_bindings.html
  int clear_bindings();

  /// Bind helper
  //////////////////////////////

  /// Bind by the index returned from `parameter_index(name)`.
  int bind_named(godot::String name, godot::Variant data);

  /// Bind array values.
  /// Binds each array element in sequence (1, 2, 3, 4, 5)
  /// Note: using `?NNN` parameters can leave holes (1, 2, 4, 5).
  int bind_array(godot::Array array);

  /// Bind dict values.
  /// If the key is `int`, bind by index.
  /// If the key is `String`, bind by name.
  /// Any other key type is ignored.
  int bind_dict(godot::Dictionary dict);

  //////////////////////////////

  ////////////////////////////////////////////////////////////

  /// Column
  ////////////////////////////////////////////////////////////

  /// Result Values From A Query.
  /// See: https://www.sqlite.org/c3ref/column_blob.html
  godot::Variant column(int index) const;

  /// Size of a BLOB or a UTF-8 TEXT result in bytes.
  /// See: https://www.sqlite.org/c3ref/column_blob.html
  int column_bytes(int index) const;

  /// Default datatype of the result.
  /// See: https://www.sqlite.org/c3ref/column_blob.html
  int column_type(int index) const;

  /// Number Of Columns In A Result Set.
  /// See: https://www.sqlite.org/c3ref/column_count.html
  int column_count() const;

  /// Number of columns in a result set.
  /// See: https://www.sqlite.org/c3ref/data_count.html
  /// See:
  /// https://stackoverflow.com/questions/2228159/what-is-the-benefit-of-using-sqlite3-data-count-over-sqlite3-column-count-in/2228567
  int data_count() const;

  /// Column Names In A Result Set.
  /// See: https://www.sqlite.org/c3ref/column_name.html
  godot::String column_name(int index) const;

#ifdef SQLITE_ENABLE_COLUMN_METADATA

  /// Database Name In A Query Result.
  /// See: https://www.sqlite.org/c3ref/column_database_name.html
  godot::String column_database_name(int index) const;

  /// Table Name In A Query Result.
  /// See: https://www.sqlite.org/c3ref/column_database_name.html
  godot::String column_table_name(int index) const;

  /// Column Name In A Query Result.
  /// See: https://www.sqlite.org/c3ref/column_database_name.html
  godot::String column_origin_name(int index) const;

#endif  // SQLITE_ENABLE_COLUMN_METADATA

  /// Declared Datatype Of A Query Result.
  /// See: https://www.sqlite.org/c3ref/column_decltype.html
  godot::String column_decltype(int index) const;

  // Column helper
  //////////////////////////////

  /// Returns current columns in an `Array`.
  godot::Array columns_array() const;

  /// Returns current columns in a `Dictionary`.
  /// The key is the `column_name` if available, otherwise the column's index.
  godot::Dictionary columns_dict() const;

  //////////////////////////////

  ////////////////////////////////////////////////////////////

  /// SQL
  ////////////////////////////////////////////////////////////

  /// Retrieving Statement SQL.
  /// SQL text of prepared statement.
  /// See: https://www.sqlite.org/c3ref/expanded_sql.html
  godot::String sql() const;

  /// Retrieving Statement SQL.
  /// SQL text of prepared statement with bound parameters expanded.
  /// See: https://www.sqlite.org/c3ref/expanded_sql.html
  godot::String expanded_sql() const;

#ifdef SQLITE_ENABLE_NORMALIZE

  /// Retrieving Statement SQL.
  /// Normalized SQL text of prepared statement.
  /// See: https://www.sqlite.org/c3ref/expanded_sql.html
  godot::String normalized_sql() const;

#endif  // SQLITE_ENABLE_NORMALIZE

  ////////////////////////////////////////////////////////////

  /// Finalize this statement.
  /// See: https://www.sqlite.org/c3ref/finalize.html
  void finalize();

  /// Whether or not the statement is finalized.
  bool is_finalized() const;

 protected:
  friend class SQLiteDatabase;

  /// Prepare this statement with a given database and SQL.
  /// See: https://www.sqlite.org/c3ref/prepare.html
  int prepare(godot::Ref<SQLiteDatabase> database, godot::String sql);

 protected:
  stmt_ptr stmt;

  godot::Ref<SQLiteDatabase> database;
};

};  // namespace sqlite
};  // namespace gdn