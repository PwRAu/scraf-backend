gtest_dep = dependency('gtest', main: true)
libcurl_dep = dependency('libcurl')

test_names = [
	'create_student',
	'search_student'
]

foreach test_name : test_names
	test(
		test_name,
		executable(
			test_name,
			sources: [
				test_name + '.cpp',
				meson.project_source_root()/'scraf-backend'/'scraf_curl.cpp',
				odb_src
			],
			dependencies: [
				scraf_dependencies,
				gtest_dep,
				libcurl_dep
			],
			include_directories: [include_directories('..'/'scraf-backend'), include_directories('..')],
			cpp_args: cpp_args
		),
		protocol: 'gtest'
	)
endforeach
