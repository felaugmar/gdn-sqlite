#!python
import os
import subprocess
import methods

opts = Variables([], ARGUMENTS)

# Gets the standard flags CC, CCX, etc.
env = DefaultEnvironment()

# Define our options
opts.Add(EnumVariable('target', "Compilation target",
         'debug', ['d', 'debug', 'r', 'release']))
opts.Add(EnumVariable('platform', "Compilation platform",
         '', ['', 'windows', 'x11', 'linux', 'osx']))
opts.Add(EnumVariable('p', "Compilation target, alias for 'platform'",
         '', ['', 'windows', 'x11', 'linux', 'osx']))
opts.Add(BoolVariable('use_llvm', "Use the LLVM / Clang compiler", 'no'))
opts.Add(BoolVariable('enable_json1', "Enable the SQLite JSON1 Extension", 'yes'))
opts.Add(PathVariable('sqlite_amalgamation', 'SQLite Amalgamation dirpath',
         'third_party/sqlite-amalgamation-3350400/'))
opts.Add(PathVariable('target_path',
         'The path where the lib is installed.', 'addons/gdn-sqlite/bin/'))
opts.Add(PathVariable('target_name', 'The library name.',
         'libgdn-sqlite', PathVariable.PathAccept))

# Local dependency paths, adapt them to your setup
godot_headers_path = "third_party/godot-cpp/godot-headers/"
cpp_bindings_path = "third_party/godot-cpp/"
cpp_library = "libgodot-cpp"

# only support 64 at this time..
bits = 64

# Updates the environment with the option variables.
opts.Update(env)

# Process some arguments
if env['use_llvm']:
    env['CC'] = 'clang'
    env['CXX'] = 'clang++'

if env['p'] != '':
    env['platform'] = env['p']

if env['platform'] == '':
    print("No valid target platform selected.")
    quit()

# Check our platform specifics
if env['platform'] == "osx":
    cpp_library += '.osx'
    env['target_name'] += '.osx'

    env.Append(CCFLAGS=['-arch', 'x86_64'])
    env.Append(CXXFLAGS=['-std=c++14'])

    env.Append(LINKFLAGS=[
        '-arch',
        'x86_64',
        '-framework',
        'Cocoa',
        '-Wl,-undefined,dynamic_lookup',
    ])

    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS=['-Og', '-g'])
    else:
        env.Append(CCFLAGS=['-O3'])

elif env['platform'] in ('x11', 'linux'):
    cpp_library += '.linux'
    env['target_name'] += '.linux'

    env.Append(CCFLAGS=['-fPIC', '-Wwrite-strings'])
    env.Append(CXXFLAGS=['-std=c++14'])

    env.Append(LINKFLAGS=["-Wl,-R,'$$ORIGIN'"])

    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS=['-Og', '-g'])
    else:
        env.Append(CCFLAGS=['-O3'])

    if bits == 64:
        env.Append(CCFLAGS=['-m64'])
        env.Append(LINKFLAGS=['-m64'])
    elif bits == 32:
        env.Append(CCFLAGS=['-m32'])
        env.Append(LINKFLAGS=['-m32'])

elif env['platform'] == "windows":
    cpp_library += '.windows'
    env['target_name'] += '.windows'
    # This makes sure to keep the session environment variables on windows,
    # that way you can run scons in a vs 2017 prompt and it will find all the required tools
    env.Append(ENV=os.environ)

    env.Append(LINKFLAGS=['/WX'])

    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS=['/Z7', '/Od', '/EHsc', '/D_DEBUG', '/MDd'])
    else:
        env.Append(CCFLAGS=['/O2', '/EHsc', '/DNDEBUG', '/MD'])

if env['target'] in ('debug', 'd'):
    cpp_library += '.debug'
else:
    cpp_library += '.release'

cpp_library += '.' + str(bits)
env['target_name'] += '.' + str(bits)

# make sure our binding library is properly includes
env.Append(CPPPATH=['.', godot_headers_path, cpp_bindings_path + 'include/',
           cpp_bindings_path + 'include/core/', cpp_bindings_path + 'include/gen/'])
env.Append(LIBPATH=[cpp_bindings_path + 'bin/'])
env.Append(LIBS=[cpp_library])

# SQLite
sqlite_amalgamation = env['sqlite_amalgamation']
sqlite_amalgamation_source = os.path.join(sqlite_amalgamation, 'sqlite3.c')

if not os.path.exists(sqlite_amalgamation_source):
    print('the provided sqlite_amalgamation has missing files')
    quit()

if env['enable_json1']:
    env.Append(CPPDEFINES=['SQLITE_ENABLE_JSON1'])

env.Append(CPPPATH=[sqlite_amalgamation])
sources = [sqlite_amalgamation_source]

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=['include/'])
sources += [
    Glob('src/*.cpp'),
    Glob('src/gdn/sqlite/*.cpp')
]

methods.generate_version_files()

library = env.SharedLibrary(
    target=env['target_path'] + env['target_name'], source=sources)

Default(library)

# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))
