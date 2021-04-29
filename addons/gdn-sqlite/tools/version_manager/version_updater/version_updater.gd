tool

extends MarginContainer

const GIT_RELEASES := 'https://api.github.com/repos/felaugmar/gdn-sqlite/releases'

const _ETAG_HEADER := 'etag'
const _IF_NONE_MATCH_HEADER := 'if-none-match'

const _X_RATELIMIT_LIMIT_HEADER := 'x-ratelimit-limit'
const _X_RATELIMIT_REMAINING_HEADER := 'x-ratelimit-remaining'
const _X_RATELIMIT_RESET_HEADER := 'x-ratelimit-reset'

const _UPDATE_TOOLTIP_FORMAT := (
  'Update (%s/%s requests remaining)\n' + 
  'Resets at %s')

const AddonConstants = preload('res://addons/gdn-sqlite/tools/utils/constants.gd')

onready var _git_releases_hr := $git_releases_http_request
onready var _asset_downloader_hr := $asset_downloader_http_request

onready var _update_btn := $v_box/v_box/h_box/update
onready var _update_anim_player := $v_box/v_box/h_box/update/animation_player

onready var _use_this_version_btn := $v_box/v_box/h_box/use_this_version
onready var _force_download_cb := $v_box/v_box/h_box/force_download

onready var _restart_to_take_effect_lb := $v_box/v_box/restart_to_take_effect
onready var _download_pb := $v_box/v_box/download_progress_bar

onready var _releases_option := $v_box/v_box/h_box/git_releases
onready var _release_tree := $v_box/v_box/git_release_tree

var _etag := ''

var _ratelimit_limit := -1
var _ratelimit_remaining := -1
var _ratelimit_reset := -1

var _assets_to_download := []
var _total_downloaded_bytes := 0

signal _finished_downloading_assets()


func _ready() -> void:
  _release_tree.set_column_expand(0, false)
  _release_tree.set_column_min_width(0, 250)


func enable() -> void:
  set_process(false)
  
  request_git_releases()
  
  _update_restart_take_effect_label()
  _on_git_releases_item_selected(0)


func _process(delta: float) -> void:
  _download_pb.value = float(_total_downloaded_bytes + _asset_downloader_hr.get_downloaded_bytes())


func request_git_releases() -> void:
  _git_releases_hr.cancel_request()
  
  var headers := PoolStringArray();
  
  if not _etag.empty():
    headers.push_back('%s:%s' % [_IF_NONE_MATCH_HEADER, _etag])
  
  _git_releases_hr.request(GIT_RELEASES, headers)


func _on_git_releases_request_completed(result: int, response_code: int, headers: PoolStringArray, body: PoolByteArray) -> void:
  _set_loading(false)
  
  if result != HTTPRequest.RESULT_SUCCESS:
    return
  
  # headers
  for header in headers:    
    var header_split := header.split(':', true, 1) as Array
    
    var header_name : String = header_split[0].to_lower()
    var header_value : String = header_split[1].strip_edges()
    
    match header_name:
      _ETAG_HEADER:
        _etag = header_value
      _X_RATELIMIT_LIMIT_HEADER:
        _ratelimit_limit = int(header_value)
      _X_RATELIMIT_REMAINING_HEADER:
        _ratelimit_remaining = int(header_value)
      _X_RATELIMIT_RESET_HEADER:
        _ratelimit_reset = int(header_value)      
  
  _update_loading_tooltip()
  
  if response_code == HTTPClient.RESPONSE_NOT_MODIFIED:
    return # not modified since last request
  
  if _ratelimit_remaining == 0:
    return # reached the github API usage limit
  
  var releases = parse_json(body.get_string_from_utf8())
  
  if releases == null or typeof(releases) != TYPE_ARRAY:
    return
  
  var previous_selected_tag = ''
  
  if _releases_option.selected != -1:
    previous_selected_tag = _releases_option.get_item_text(_releases_option.selected)
  
  _releases_option.clear()
  
  var current_selection := 0
  
  for release_dict in releases:
    var tag_name := release_dict['tag_name'] as String
    
    if bool(release_dict['prerelease']) or bool(release_dict['draft']):
      continue # skip pre-releases and drafts
    
    _releases_option.add_item(tag_name)
    
    var item_id : int = _releases_option.get_item_count() - 1
    _releases_option.set_item_metadata(item_id, release_dict)
    
    if previous_selected_tag == tag_name:
      current_selection = item_id    
  
  if _releases_option.get_item_count() > 0:
    _releases_option.select(current_selection)
  
  _on_git_releases_item_selected(current_selection)


func _on_asset_downloader_request_completed(result: int, response_code: int, headers: PoolStringArray, body: PoolByteArray) -> void:
  if result != HTTPRequest.RESULT_SUCCESS:
    return
  
  if response_code != HTTPClient.RESPONSE_OK:
    return # error downloading
  
  _total_downloaded_bytes += _asset_downloader_hr.get_downloaded_bytes()
  
  _request_next_asset()


func _on_update_pressed() -> void:
  _set_loading(true)
  request_git_releases()


func _on_git_releases_item_selected(index: int) -> void:
  _release_tree.clear()
  
  var is_valid : bool = index < _releases_option.get_item_count()
  
  _releases_option.disabled = not is_valid
  _use_this_version_btn.disabled = not is_valid
  _force_download_cb.disabled = not is_valid
  
  if not is_valid:
    return
  
  # root
  _release_tree.create_item()
  
  var release_dict := _releases_option.get_item_metadata(index) as Dictionary
  
  var version_item := _release_tree.create_item() as TreeItem
  version_item.set_text(0, 'version')
  version_item.set_text(1, release_dict['tag_name'])
  
  var name_item := _release_tree.create_item() as TreeItem
  name_item.set_text(0, 'name')
  name_item.set_text(1, release_dict['name'])
  
  var description_item := _release_tree.create_item() as TreeItem
  description_item.set_text(0, 'description')
  description_item.set_text(1, release_dict['body'])
  
  var created_at_item := _release_tree.create_item() as TreeItem
  created_at_item.set_text(0, 'created at')
  created_at_item.set_text(1, release_dict['created_at'].replace('T', ' ').trim_suffix("Z"))
  
  var published_at_item := _release_tree.create_item() as TreeItem
  published_at_item.set_text(0, 'published at')
  published_at_item.set_text(1, release_dict['published_at'].replace('T', ' ').trim_suffix("Z"))
  
  var assets_item := _release_tree.create_item() as TreeItem
  assets_item.set_text(0, 'assets')
  assets_item.set_editable(0, false)
  assets_item.set_selectable(0, false)
  assets_item.set_expand_right(0, true)
  assets_item.disable_folding = true
  assets_item.collapsed = false
  
  for asset_dict in release_dict['assets']:
    var asset_item := _release_tree.create_item(assets_item) as TreeItem
    asset_item.set_text(0, asset_dict['name'])
    asset_item.set_text(1, String.humanize_size(asset_dict['size']))


func _set_loading(enable: bool) -> void:  
  if enable:
    _update_anim_player.play('spin')
  else:
    _update_anim_player.stop(false)


func _update_loading_tooltip() -> void:
  var limit_str     := '?' if _ratelimit_limit == -1 else str(_ratelimit_limit)
  var remaining_str := '?' if _ratelimit_remaining == -1 else str(_ratelimit_remaining)
  var reset_str     := '?'
  
  if _ratelimit_reset != -1:
    var timezone_reset := int(_ratelimit_reset + (OS.get_time_zone_info()['bias'] * 60))
    
    var reset_datetime := OS.get_datetime_from_unix_time(timezone_reset)
    
    reset_str = '%02d:%02d:%02d' % [reset_datetime['hour'], reset_datetime['minute'], reset_datetime['second']]
  
  _update_btn.hint_tooltip = _UPDATE_TOOLTIP_FORMAT % [remaining_str, limit_str, reset_str]


func _on_use_this_version_pressed() -> void:
  # TODO: download assets and mark with which version should be make available on restart.
  if _releases_option.selected == -1:
    return
  
  _update_btn.disabled = true
  _releases_option.disabled = true
  _use_this_version_btn.disabled = true
  _force_download_cb.disabled = true
  
  _download_pb.visible = true
  
  var release_dict := _get_selected_release_dict()
  
  var assets_to_download := []
  
  var dir := Directory.new()
  
  for asset_dict in release_dict['assets']:
    var asset_filepath := _get_asset_filepath(release_dict, asset_dict)
    
    var asset_file_exists := dir.file_exists(asset_filepath)
    var asset_file_len := 0
    
    if asset_file_exists:
      var file := File.new()
      
      var err := file.open(asset_filepath, File.READ)
      assert(err == OK)
      
      asset_file_len = file.get_len()
    
    if _force_download_cb.pressed or not asset_file_exists or asset_file_len != asset_dict['size']:
      assets_to_download.append(asset_dict)
  
  yield(_download_assets(assets_to_download), 'completed')
  
  var version := release_dict['tag_name'] as String
  
  # make sure the release folder is created
  var err := dir.make_dir_recursive(AddonConstants.get_release_dir(version))
  assert(err == OK or err == ERR_ALREADY_EXISTS)
  
  # store selected version
  var file := File.new()
  
  err = file.open(AddonConstants.MAKE_CURRENT_FILE, File.WRITE)
  assert(err == OK)
  
  file.store_string(version)
  
  file.close()
  
  _update_restart_take_effect_label()
  
  _update_btn.disabled = false
  _releases_option.disabled = false
  _use_this_version_btn.disabled = false
  _force_download_cb.disabled = false
  
  _download_pb.visible = false


func _download_assets(asset_dict_array: Array) -> void:
  yield(VisualServer, 'frame_post_draw')
  
  _assets_to_download = asset_dict_array
  
  var bytes_to_download := 0
  
  for asset_dict in _assets_to_download:    
    bytes_to_download += int(asset_dict['size'])
  
  _total_downloaded_bytes = 0
  _download_pb.max_value = bytes_to_download
  
  _request_next_asset()
  
  set_process(true)
  
  yield(self, '_finished_downloading_assets')
  
  set_process(false)


func _request_next_asset() -> void:
  yield(VisualServer, 'frame_post_draw')
  
  var asset_dict = _assets_to_download.pop_back()
  
  if not asset_dict:
    emit_signal('_finished_downloading_assets')
    return
  
  var asset_filepath := _get_asset_filepath(_get_selected_release_dict(), asset_dict)
  
  var dir := Directory.new()
  
  var err := dir.make_dir_recursive(asset_filepath.get_base_dir())
  assert(err == OK or err == ERR_ALREADY_EXISTS)
  
  _asset_downloader_hr.download_file = asset_filepath
  _asset_downloader_hr.request(asset_dict['browser_download_url'] as String)


func _get_selected_release_dict() -> Dictionary:
  return _releases_option.get_item_metadata(_releases_option.selected) as Dictionary


func _get_asset_filepath(release_dict: Dictionary, asset_dict: Dictionary) -> String:
  return AddonConstants.get_asset_filepath(release_dict['tag_name'], asset_dict['name'])


func _update_restart_take_effect_label() -> void:
  var dir := Directory.new()
  
  _restart_to_take_effect_lb.visible = dir.file_exists(AddonConstants.MAKE_CURRENT_FILE)
