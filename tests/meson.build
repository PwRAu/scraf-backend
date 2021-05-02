gtest_dep = dependency('gtest', main: true)

test_names = [
	'students',
	'studentsid',
	'studentsmarks',
	'studentssubjects',
	'teacherid',
	'teachers',
	'teachersmarks',
	'teacherssubjectid',
	'teacherssubjects'
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
				gtest_dep
			],
			include_directories: ['..', '..'/'scraf-backend'],
			cpp_args: cpp_args
		)
	)
endforeach
