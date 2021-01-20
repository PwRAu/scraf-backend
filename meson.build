project('Scraf', 'cpp', default_options: ['cpp_std=c++2a', 'c_std=c11', 'b_ndebug=if-release', 'b_lto=true'])

cpp_compiler = meson.get_compiler('cpp')

if cpp_compiler.get_id() == 'clang' and get_option('buildtype').startswith('debug')
	extra_flags = ['-gdwarf-4', '-glldb', '-fstandalone-debug', '-fdebug-info-for-profiling', '-fdebug-macro', '-fdebug-default-version=4', '-fno-omit-frame-pointer', '-fno-optimize-sibling-calls', '-Wall', '-Wextra', '-Weverything', '-Wpedantic', '-Wno-unused-parameter', '-Wno-c++98-compat', '-Wno-c++98-compat-pedantic', '-Wno-missing-prototypes', '-Wno-global-constructors', '-Wno-exit-time-destructors', '-Wno-shadow', '-Wno-documentation', '-Wno-documentation-unknown-command']
else
	extra_flags = []
endif

if not meson.is_cross_build()
	add_project_arguments('-march=native', language: 'cpp')
endif

src = ['src/main.cpp']

#odb_compiler = find_program('odb')
#generated_src = custom_target(
#	'odb_gen[]',
#	input: ['src/school.hpp', 'src/student.hpp'],
#	output: ['odb_gen/school_odb.hpp', 'odb_gen/student_odb.hpp'],
#	command: [
#		odb_compiler,
#		'--database', 'pgsql',
#		'--generate-query',
#		'--generate-schema',
#		'--std', 'c++17',
#		'--output-dir', 'odb_gen',
#		'--odb-file-suffix', '_odb',
#		'--hxx-suffix', '.hpp',
#		'--ixx-suffix', '_inline.hpp',
#		'--cxx-suffix', '.cpp',
#		'-x', 'c++20',
#		'@INPUT@',
#	]
#)
odb_generator = generator(
	find_program('odb'),
	output: ['@BASENAME@_odb.cpp', '@BASENAME@_odb.hpp', '@BASENAME@_inline.hpp'],
	arguments: [
		'--database', 'pgsql',
		'--generate-query',
		'--generate-schema',
		'--std', 'c++17',
		'--output-dir', 'odb_gen',
		'--odb-file-suffix', '_odb',
		'--hxx-suffix', '.hpp',
		'--ixx-suffix', '_inline.hpp',
		'--cxx-suffix', '.cpp',
		'-x', 'c++20',
		'@INPUT@',
	]
)

odb_school = odb_generator.process('src/school.hpp')
odb_student = odb_generator.process('src/student.hpp')

executable(
	'matto',
	sources: src,
	dependencies: [
		dependency('gtest'),
		dependency(
			'ODB',
			components: 'pgsql',
			method: 'cmake',
			cmake_module_path: 'vendor/OdbCmake/cmake/Modules'
		)
	]
)