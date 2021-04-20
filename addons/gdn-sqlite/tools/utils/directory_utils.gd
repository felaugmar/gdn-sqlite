extends Reference

static func get_dir_files_by_regex(dirpath: String, regex_pattern: String) -> Array: # of String
  var files := []
  
  var dir := Directory.new()
  
  if not dir.dir_exists(dirpath):
    return files
  
  if dir.open(dirpath) != OK:
    return files
  
  if dir.list_dir_begin(true) != OK:
    return files
  
  var regex := RegEx.new()
  if regex.compile(regex_pattern) != OK:
    return files
  
  var dir_entry := dir.get_next()
  
  while not dir_entry.empty():
    if not dir.current_is_dir():
      var file := dirpath.plus_file(dir_entry)
      
      if is_instance_valid(regex.search(file)):
        files.append(file)
    
    dir_entry = dir.get_next()
  
  dir.list_dir_end()
  
  return files
