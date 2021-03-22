project(
	'Scraf',
	'cpp',
	default_options: [
		'cpp_std=c++20',
		'c_std=c17',
		'b_ndebug=if-release',
		'b_lto=true'
	],
	meson_version: '>=0.57.0'
)

compiler = meson.get_compiler('cpp')

clang_warnings = []
if compiler.get_id() == 'clang'
	add_project_arguments(['-fuse-ld=lld', '-Wno-unused-command-line-argument'], language: 'cpp')
	add_project_link_arguments('-fuse-ld=lld', language: 'cpp')
	if get_option('debug')
		add_project_arguments(['-Og', '-gdwarf-4', '-glldb', '-fstandalone-debug', '-fdebug-info-for-profiling', '-fdebug-macro', '-fdebug-default-version=4', '-fno-omit-frame-pointer', '-fno-optimize-sibling-calls'], language: 'cpp')
		clang_warnings = ['-Weverything', '-Wno-unused-parameter', '-Wno-c++98-compat', '-Wno-c++98-compat-pedantic', '-Wno-missing-prototypes', '-Wno-global-constructors', '-Wno-exit-time-destructors', '-Wno-shadow', '-Wno-documentation', '-Wno-documentation-unknown-command']
	endif
endif

if not meson.is_cross_build()
	add_project_arguments('-march=native', language: 'cpp')
endif

cpp_args = compiler.get_supported_arguments(['-Wno-unknown-pragmas', '-Wno-comment'])

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

simdjson_dep = dependency('simdjson', required: false)
if not simdjson_dep.found()
	cmake = import('cmake')
	simdjson_options = cmake.subproject_options()
	simdjson_options.add_cmake_defines({'SIMDJSON_JUST_LIBRARY': true, 'BUILD_SHARED_LIBS': false, 'SIMDJSON_BUILD_STATIC': true})
	simdjson_dep = cmake.subproject('simdjson', options: simdjson_options).dependency('simdjson')
endif

src = ['scraf-backend/main.cpp']

odb_src = odb_generator.process(
	'scraf-backend/student.hpp',
	'scraf-backend/school_class.hpp',
	'scraf-backend/school.hpp',
	'scraf-backend/teacher.hpp'
)

scraf_dependencies = [
	dependency('pistache', default_options: 'default_library=static'),
	dependency('libodb', version: '>=2.5.0'),
	dependency('libodb-pgsql', version: '>=2.5.0'),
	simdjson_dep,
	dependency('nlohmann_json'),
	dependency('libsodium')
]

executable(
	'Scraf',
	sources: [
		src,
		odb_src
	],
	include_directories: 'scraf-backend',
	dependencies: scraf_dependencies,
	cpp_args: [cpp_args]
)

if get_option('scraf_build_tests')
	test(
		'ScrafTest',
		executable(
			'ScrafTest',
			sources: [
				'scraf-backend/main.test.cpp',
				'scraf-backend/scraf.test.cpp',
				'scraf-backend/scraf_curl.cpp',
				odb_src
			],
			include_directories: 'scraf-backend',
			dependencies: [
				scraf_dependencies,
				dependency('gtest'),
				dependency('libcurl')
			],
			cpp_args: cpp_args
		)
	)
endif
