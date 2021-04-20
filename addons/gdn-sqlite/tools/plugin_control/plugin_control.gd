tool

extends MarginContainer

onready var _version_manager := $'tab_container/Version Manager'


func enable() -> void:
  _version_manager.enable()
