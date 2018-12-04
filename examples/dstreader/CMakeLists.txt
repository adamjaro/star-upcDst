
project (dstreader)
cmake_minimum_required (VERSION 2.6)

#sources for LIB_READER
set (SRCS
  src/ArgumentParser.cxx
  )

#name of LIB_READER
set (LIB_READER dstreader)

#binary executables
set (BIN AnalysisTemplate)

#UPC picoDsts library
set (UPC_LIB /star/u/jaroslav/star-upcDst/build/libstar-upc.so)
set (UPC_INC /star/u/jaroslav/star-upcDst/include)

#general compiler options
set (CMAKE_CXX_COMPILER /usr/bin/g++)
#option -Wl,--no-as-needed is necessary only with root5
set (CMAKE_CXX_FLAGS "-Wall -g -Wl,--no-as-needed")
include_directories (include ${UPC_INC})

#ROOT section

#compiler flags
execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_FLAGS_CMD)
string(REPLACE "\n" "" ROOT_FLAGS "${ROOT_FLAGS_CMD}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_FLAGS}")

#libraries
set(ROOT_DEPS Core EG Hist Physics RIO Tree MathCore)

#include dir
execute_process(COMMAND root-config --libdir OUTPUT_VARIABLE ROOT_LIBDIR_CMD)
string(REPLACE "\n" "" ROOT_LIBDIR "${ROOT_LIBDIR_CMD}")
link_directories (${ROOT_LIBDIR})

#end of ROOT section

#create library LIB_READER
add_library (${LIB_READER} SHARED ${SRCS})
target_link_libraries(${LIB_READER} ${ROOT_DEPS} )

#create executables in BIN
foreach(IBIN ${BIN})
  add_executable(${IBIN} src/${IBIN}.cxx)
  target_link_libraries(${IBIN} ${LIB_READER} ${UPC_LIB} ${ROOT_DEPS} )
endforeach(IBIN ${BIN})














