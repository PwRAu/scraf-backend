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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

# Se il compilatore è Clang, imposto una serie di parametri specifici che sono utili a debuggare
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# -stdlib=libc++
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fuse-ld=lld -Wno-unused-command-line-argument")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld -Wno-unused-command-line-argument")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
	if(CMAKE_BUILD_TYPE MATCHES Debug)
		# TODO: Check -g3 -g difference
		set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}   --optimize=g --debug=3 -gdwarf-4 -glldb -fstandalone-debug -fdebug-info-for-profiling -fdebug-macro -fdebug-default-version=4 -fno-omit-frame-pointer -fno-optimize-sibling-calls")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --optimize=g --debug=3 -gdwarf-4 -glldb -fstandalone-debug -fdebug-info-for-profiling -fdebug-macro -fdebug-default-version=4 -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wall -Wextra -Weverything -Wpedantic -Wno-unused-parameter -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-missing-prototypes -Wno-global-constructors -Wno-exit-time-destructors -Wno-shadow -Wno-shorten-64-to-32 -Wno-documentation -Wno-documentation-unknown-command")
	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   --optimize=3 -flto -fwhole-program-vtables")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} --optimize=3 -flto -fwhole-program-vtables")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto -fwhole-program-vtables")
	endif(CMAKE_BUILD_TYPE MATCHES Debug)
else()
	if(CMAKE_BUILD_TYPE MATCHES Release)
		include(CheckIPOSupported)
		check_ipo_supported(RESULT ipo_result OUTPUT ipo_output)
		if(ipo_result)
			if(CMAKE_VERSION VERSION_GREATER 3.8)
				# Enable IPO for CMake versions that support it
				cmake_policy(SET CMP0069 NEW)
			endif()
			set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
			# set_property(TARGET target-name-Scraf PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE) così da abilitare l'LTO solo per Scraf in caso dia problemi col resto del progetto
		endif()
	endif(CMAKE_BUILD_TYPE MATCHES Release)
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/vendor/OdbCmake/cmake/Modules")
find_package(ODB REQUIRED COMPONENTS pgsql)
include(${ODB_USE_FILE})
set(Scraf_SOURCES main.cpp)
set(Scraf_ODB_HEADERS school.hpp)
odb_compile(Scraf_SOURCES FILES ${Scraf_ODB_HEADERS} DB pgsql STANDARD "c++17" GENERATE_QUERY)


# Per creare l'eseguibile, devo scrivere qui tutti i file .cpp appartenenti al progetto
add_executable(${PROJECT_NAME} ${Scraf_SOURCES})

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
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		DATABASE_PGSQL
)
