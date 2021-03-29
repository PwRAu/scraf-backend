gtest_dep = dependency('gtest', main: true)
scraf_curl_dep = declare_dependency(
	link_with: library(
		'scraf_curl',
		sources: meson.project_source_root()/'scraf-backend'/'scraf_curl.cpp',
		include_directories: '..',
		dependencies: dependency('libcurl')
	)
)

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
				odb_src
			],
			dependencies: [
				scraf_dependencies,
				gtest_dep,
				scraf_curl_dep
			],
			include_directories: ['..', '..'/'scraf-backend'],
			cpp_args: cpp_args
		),
		protocol: 'gtest'
	)
endforeach
