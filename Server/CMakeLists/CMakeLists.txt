###################################################################################################
##CMakeLists.txt°¸Àý
###################################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_CXX_COMPILER       g++ )
CMAKE_POLICY(SET CMP0015 NEW)

#ADD_DEFINITIONS("-g -DDEBUG -Wall  -MMD  -funroll-loops -D__USE_STRING_INLINES  -fPIC ")
ADD_DEFINITIONS(" -DDEBUG -fPIC -g -MMD -std=c++0x")

INCLUDE_DIRECTORIES(
			../../include
			../../incProto
			../inc
			../../third_party/
			../../mongodb/include
			../db
			../src/db
			/usr/local/mysql/include/
			/usr/include/mysql
			/usr/local/include
			../ez_bench/
			)

LINK_DIRECTORIES(/usr/lib /usr/local/lib ../../lib)
LINK_LIBRARIES(mongodb pthread ev dl log config hiredis curl tinyxml dcheck mysqlhandle mongoclient boost_thread boost_filesystem boost_program_options)

#AUX_SOURCE_DIRECTORY(../../new_ez_bench SRC_LIST)
AUX_SOURCE_DIRECTORY(../ez_bench SRC_LIST)
AUX_SOURCE_DIRECTORY(../src SRC_LIST)
AUX_SOURCE_DIRECTORY(../../incProto SRC_LIST)


AUX_SOURCE_DIRECTORY(../../third_party/json SRC_LIST)
AUX_SOURCE_DIRECTORY(../../third_party/gt SRC_LIST)
AUX_SOURCE_DIRECTORY(../../third_party/base64 SRC_LIST)
AUX_SOURCE_DIRECTORY(../../third_party/sha1 SRC_LIST)
AUX_SOURCE_DIRECTORY(../db SRC_LIST)
AUX_SOURCE_DIRECTORY(../src/db SRC_LIST)
#AUX_SOURCE_DIRECTORY(../../mongo_interface SRC_LIST)

SET(EXECUTABLE_OUTPUT_PATH ../bin)
ADD_EXECUTABLE(game_server ${SRC_LIST})

