import glob
import os
import sys
import subprocess

vars = Variables('settings.py')
vars.AddVariables(
	EnumVariable('mode', 'Build mode', 'debug', allowed_values=('debug', 'release')),
)

target_os = str(Platform())

env = Environment(variables = vars, ENV = os.environ)
conf = env.Configure()

MajVer = 0
MinVer = 0
PatVer = 0
Stable = False

if not env.GetOption('clean'):
	if "MajVer" not in os.environ:
		print("Major Version Not Specified!")
		sys.exit(-1)
	elif "MinVer" not in os.environ:
		print("Minor Version Not Specified!")
		sys.exit(-1)
	elif "PatVer" not in os.environ:
		print("Patch Version Not Specified!")
		sys.exit(-1)
	else:
		MajVer = int(os.environ['MajVer'])
		MinVer = int(os.environ['MinVer'])
		PatVer = int(os.environ['PatVer'])
		Stable = env['mode'] != "debug"

	env.Append(
		CDEFINES=[
			"ARU_VERSION_MAJOR={0}".format(MajVer),
			"ARU_VERSION_MINOR={0}".format(MinVer),
			"ARU_VERSION_PATCH={0}".format(PatVer),
			"ARU_BUILD_STABLE={0}".format(int(Stable))
		],
		CPPDEFINES=[
			"ENABLE_WIN_ICON",
			"ARU_VERSION_MAJOR={0}".format(MajVer),
			"ARU_VERSION_MINOR={0}".format(MinVer),
			"ARU_VERSION_PATCH={0}".format(PatVer),
			"ARU_BUILD_STABLE={0}".format(int(Stable))
		],
	)

if os.environ.get('CC') == 'gcc' or env['mode'] == 'debug':
	env.Replace(CC='gcc', CXX='g++')
else:
	env.Replace(CC='clang', CXX='clang++')

# Asan & Ubsan (need to come first).
if env['mode'] == 'debug' and target_os == 'posix':
	env.Append(
		CCFLAGS=['-fsanitize=address', '-fsanitize=undefined'],
		LINKFLAGS=['-fsanitize=address', '-fsanitize=undefined'],
		LIBS=['asan', 'ubsan']
	)

# Global compilation flags.
# CCFLAGS   : C and C++
# CFLAGS    : only C
# CXXFLAGS  : only C++
env.Append(
	CFLAGS=['-std=c99'],
	CXXFLAGS=['-std=c++17']
)

if env['mode'] == 'release':
	env.Append(
		CCFLAGS='-Os'
	)

if env['mode'] == 'debug':
	env.Append(
		CCFLAGS=['-O0', '-g', '-Werror', '-Wno-unused-function']
	)

def GatherFiles(Directories):
	if type(Directories) is not list:
		Directories = [Directories];

	files = [];
	for directory in Directories:
		for root, dirnames, filenames in os.walk(directory):
			for filename in filenames:
				if filename.endswith('.c') or filename.endswith('.cpp'):
					files.append(os.path.join(root, filename))

	return files

# Get all the c and c++ files in src, recursively.
sources = GatherFiles(['src', 'lib/imgui-1.88', 'lib/log'])
# sources += glob.glob('lib/tinyfiledialogs.c')

# Header Directories.
env.Append(
	CPATH=['src/', 'lib/'],
	CPPPATH=['src/', 'lib/imgui-1.88/', 'lib/']
)

# Windows compilation support.
if target_os == 'msys':
	env.Append(
		LIBS=[
			'SDL2main',
			'SDL2', 'mingw32', 'opengl32', 'comdlg32', 'imagehlp', 'dinput8',
			'dxguid', 'dxerr8', 'user32', 'gdi32', 'winmm', 'imm32', 'ole32', 'oleaut32',
			'shell32', 'version', 'uuid', 'setupapi'
		],
		LINKFLAGS=[
			"-mwindows", # Fix Console From Popping-Up
			"--static"   # Link GLFW & Stuff Statically
		]
	)
else:
	env.Append(
		LIBS=['SDL2', 'dl', 'm'],
		CXXFLAGS=['-Wall', '-Wno-narrowing'],
		CFLAGS=['-Wall', '-Wno-unknown-pragma'],
		CDEFINES=['LOG_USE_COLOR']
	)

# # OSX Compilation support.
if target_os == 'darwin':
	env.Append(FRAMEWORKS=['OpenGL', 'Cocoa'])
	env.Append(LIBS=['m', 'sdl2', 'objc'])

# Append external environment flags
env.Append(
	CFLAGS=os.environ.get("CFLAGS", "").split(),
	CXXFLAGS=os.environ.get("CXXFLAGS", "").split(),
	LINKFLAGS=os.environ.get("LDFLAGS", "").split()
)

env.Program(target='aru', source=sorted(sources))
