# gdn-sqlite

### Getting the plugin

- Download the zip of the repository;
- Unzip and copy the **addons** folder to your project root;
- Enable the plugin **gdn-sqlite** in the **Project Settings**;
- A **SQLite** item will appear in the bottom panel, click on it;
- Select a version in **Version Manager -> Version Updater** and press **Use this version** (network is required);
- The version will be downloaded and you can start using the plugin after restarting the editor.

*Note: The version is downloaded from Github Releases, you can get it yourself and do as below:*
- Place the binaries in **addons/gdn-sqlite/bin**;
- Place **\*.gdnlib** and **\*.gdns** in **addons/gdn-sqlite**.

### Using

```gdscript
var SQLite = load('res://addons/gdn-sqlite/sqlite.gdns')

var sqlite = SQLite.new()

var db = sqlite.open(':memory:')

var stmt = db.prepare('SELECT 1')

var err = stmt.step()
assert(err == sqlite.ROW)

assert(stmt.columns_array() == [1])
```
