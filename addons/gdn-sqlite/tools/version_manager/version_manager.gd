tool

extends PanelContainer

onready var _current_version := $h_split/current_version
onready var _version_updater := $h_split/version_updater


func enable() -> void:
  _current_version.enable()
  _version_updater.enable()
