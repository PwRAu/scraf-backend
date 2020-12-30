# Imposto la versione minima richiesta da CMake (3.13 è quella presente in Debian 10, quindi va bene)
cmake_minimum_required(VERSION 3.13)

# Imposto il nome del progetto più qualche dettaglio
project(Scraf
	LANGUAGES CXX
	HOMEPAGE_URL "https://github.com/PwRAu/scraf-backend"
)

# Dico a CMake di creare "compile_commands.json", che serve a Clangd per evidenziare gli errori in Visual Studio Code
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# Imposto gli standard di C e C++ alle ultime disponibili, disabilitando delle estensioni specifiche di Linux che non servono
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/vendor/OdbCmake/cmake/Modules")
find_package(ODB REQUIRED COMPONENTS pgsql)
include(${ODB_USE_FILE})
set(Scraf_SOURCES src/main.cpp)
set(Scraf_ODB_HEADERS src/school.hpp)
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR "GNU") AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10)
	set(ODB_CPP_STANDARD_FLAG "-std=c++20")
else()
	set(ODB_CPP_STANDARD_FLAG "-std=c++2a")
	message("Paoilo")
endif()
odb_compile(Scraf_SOURCES FILES ${Scraf_ODB_HEADERS} DB pgsql STANDARD "c++17" X_CPP_OPTION "-fconcepts" X_CPP_OPTION ${ODB_CPP_STANDARD_FLAG} GENERATE_QUERY)

# Per creare l'eseguibile, devo scrivere qui tutti i file .cpp appartenenti al progetto
add_executable(${PROJECT_NAME} ${Scraf_SOURCES})

# Se il compilatore è Clang, imposto una serie di parametri specifici che sono utili a debuggare
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# -stdlib=libc++
	add_compile_options(-fuse-ld=lld -Wno-unused-command-line-argument)
	add_link_options(-fuse-ld=lld)
	if(CMAKE_BUILD_TYPE MATCHES Debug)
		# TODO: Check -g3 -g difference
		add_compile_options(--optimize=g --debug=3 -gdwarf-4 -glldb -fstandalone-debug -fdebug-info-for-profiling -fdebug-macro -fdebug-default-version=4 -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wall -Wextra -Weverything -Wpedantic -Wno-unused-parameter -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-global-constructors -Wno-exit-time-destructors -Wno-shadow -Wno-shorten-64-to-32 -Wno-documentation -Wno-documentation-unknown-command)
	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		include(CheckIPOSupported)
		check_ipo_supported(RESULT ipo_result OUTPUT ipo_output)
		if(ipo_result)
			if(CMAKE_VERSION VERSION_GREATER 3.8)
				# Enable IPO for CMake versions that support it
				cmake_policy(SET CMP0069 NEW)
				set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
			endif()
			set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
			set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE) #così da abilitare l'LTO solo per Scraf in caso dia problemi col resto del progetto
		endif()
		add_compile_options(--optimize=3 -fwhole-program-vtables)
		add_link_options(-fwhole-program-vtables)
	endif()
else()
	if(CMAKE_BUILD_TYPE MATCHES Release)
		include(CheckIPOSupported)
		check_ipo_supported(RESULT ipo_result OUTPUT ipo_output)
		if(ipo_result)
			if(CMAKE_VERSION VERSION_GREATER 3.8)
				# Enable IPO for CMake versions that support it
				cmake_policy(SET CMP0069 NEW)
				set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
			endif()
			set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
			#set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE) così da abilitare l'LTO solo per Scraf in caso dia problemi col resto del progetto
		endif()
	endif(CMAKE_BUILD_TYPE MATCHES Release)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Only statically linked library")
set(PISTACHE_INSTALL OFF CACHE BOOL "Only statically linked library")
set(PISTACHE_USE_SSL ON CACHE BOOL "Only statically linked library")
add_subdirectory(vendor/pistache)

target_link_libraries(${PROJECT_NAME}
	pistache_static
	${ODB_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${ODB_INCLUDE_DIRS}
		${ODB_COMPILE_OUTPUT_DIR}
		src
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		DATABASE_PGSQL
)
