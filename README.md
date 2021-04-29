# GDN-SQLite

## Getting the plugin

### From Godot Asset Library

- Search for [**GDN-SQLite**](https://godotengine.org/asset-library/asset/931) in the asset library;
- Download the plugin files;
- Enable the plugin **gdn-sqlite** in the **Project Settings**;
- Follow the instructions at [Downloading the binaries](#Downloading-the-binaries).

### From Github Repository

- Download the zip of the repository;
- Unzip and copy the **addons** folder to your project root;
- Enable the plugin **gdn-sqlite** in the **Project Settings**;
- Follow the instructions at [Downloading the binaries](#Downloading-the-binaries).

---

## Downloading the binaries

### From the editor

With the plugin enabled, you can use the **SQLite** item at the Bottom Panel to choose a version in the **Version Updater** and press **Use this version** which will automatically download and configure everything for you. (*The editor may need to be restarted to take effect after the binaries are downloaded*)

##### See the image below for reference where you can find the **Version Updater**.
![GDN-SQLite Version Manager](/images/sqlite-version-manager.png "GDN-SQLite Version Manager")

### From the repository

The plugin downloads the version from Github Releases, you can get it [yourself](https://github.com/felaugmar/gdn-sqlite/releases) and do as below:
- Place the binaries in **addons/gdn-sqlite/bin**;
- Place **\*.gdnlib** and **\*.gdns** in **addons/gdn-sqlite**.

---

### Usage example

```gdscript
var SQLite = load('res://addons/gdn-sqlite/sqlite.gdns')

var sqlite = SQLite.new()

var db = sqlite.open(':memory:')

var stmt = db.prepare('SELECT 1')

var err = stmt.step()
assert(err == sqlite.ROW)

assert(stmt.columns_array() == [1])
```

You can find all classes and members in the [**Current Version**](#see-the-image-below-for-reference-where-you-can-find-the-version-updater) interface after the binaries are downloaded.
