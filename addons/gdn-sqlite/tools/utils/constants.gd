extends Reference

const ADDON_DIR := 'res://addons/gdn-sqlite'

const GDN_SQLITE_DIR := 'user://gdn-sqlite'

const RELEASES_DIR      := GDN_SQLITE_DIR + '/releases'
const MAKE_CURRENT_FILE := GDN_SQLITE_DIR + '/.make_current'

const GDNLIB_GDNS_RE_PATTERN := '\\.(gdnlib|gdns)$'
const GDNLIB_RE_PATTERN := '\\.gdnlib$'
const GDNS_RE_PATTERN := '\\.gdns$'


static func get_release_dir(release: String) -> String:
  return RELEASES_DIR.plus_file(release)


static func get_release_assets_dir(release: String) -> String:
  return get_release_dir(release).plus_file('assets')


static func get_asset_filepath(release: String, asset: String) -> String:
  return get_release_assets_dir(release).plus_file(asset)
