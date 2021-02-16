project('Scraf', 'cpp', default_options: ['cpp_std=c++2a', 'c_std=c11', 'b_ndebug=if-release', 'b_lto=true'])

if meson.get_compiler('cpp').get_id() == 'clang' and get_option('buildtype').startswith('debug')
	cpp_args = ['-fuse-ld=lld', '-gdwarf-4', '-glldb', '-fstandalone-debug', '-fdebug-info-for-profiling', '-fdebug-macro', '-fdebug-default-version=4', '-fno-omit-frame-pointer', '-fno-optimize-sibling-calls', '-Wall', '-Wextra', '-Weverything', '-Wpedantic', '-Wno-unused-parameter', '-Wno-c++98-compat', '-Wno-c++98-compat-pedantic', '-Wno-missing-prototypes', '-Wno-global-constructors', '-Wno-exit-time-destructors', '-Wno-shadow', '-Wno-documentation', '-Wno-documentation-unknown-command']
	linker_args = ['-fuse-ld=lld']
else
	cpp_args = []
	linker_args = []
endif

if not meson.is_cross_build()
	add_project_arguments('-march=native', language: 'cpp')
endif

src = ['src/main.cpp']

odb_generator = generator(
	find_program('odb'),
	output: ['@BASENAME@_odb.hpp', '@BASENAME@_odb.cpp', '@BASENAME@_odb.ipp'],
	arguments: [
		'--database', 'pgsql',
		'--generate-query',
		'--generate-schema',
		'--std', 'c++17',
		'--output-dir', '@BUILD_DIR@',
		'--odb-file-suffix', '_odb',
		'--hxx-suffix', '.hpp',
		'--ixx-suffix', '.ipp',
		'--cxx-suffix', '.cpp',
		'-x', '-std=c++20',
		'@INPUT@',
	]
)

cmake = import('cmake')

pistache_options = cmake.subproject_options()
pistache_options.add_cmake_defines({'BUILD_SHARED_LIBS': false, 'PISTACHE_INSTALL': false, 'PISTACHE_USE_SSL': true})

executable(
	'Scraf',
	sources: [
		src,
		odb_generator.process(
			'src/student.hpp',
			'src/school_class.hpp',
			'src/school.hpp',
			'src/teacher.hpp'
		)
	],
	include_directories: 'src',
	dependencies: [
		cmake.subproject('pistache', options: pistache_options).dependency('pistache_static'),
		dependency('libodb'),
		dependency('libodb-pgsql'),
		dependency('gtest'),
		dependency('simdjson'),
		dependency('nlohmann_json'),
		dependency('libsodium')
	],
	cpp_args: cpp_args,
	link_args: linker_args
)
