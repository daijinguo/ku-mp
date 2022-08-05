
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 11)

SET(include_dirs libmp
        ${STAGED_INSTALL_PREFIX}/include
        ${CODE_ROOT_DIR}/libcommon/include
        ${CODE_ROOT_DIR}/libhttp/include)

SET(src_files ku-mp.c)


# static library
ADD_LIBRARY(ku-mp_a STATIC ${src_files})
ADD_DEPENDENCIES(ku-mp_a common_a http_a)

TARGET_INCLUDE_DIRECTORIES(ku-mp_a PUBLIC ${include_dirs})
TARGET_LINK_DIRECTORIES(ku-mp_a PUBLIC ${STAGED_INSTALL_PREFIX}/lib)
TARGET_LINK_LIBRARIES(ku-mp_a common_a http_a)


# shared library
ADD_LIBRARY(ku-mp SHARED ${src_files})
ADD_DEPENDENCIES(ku-mp common_a http_a)

TARGET_INCLUDE_DIRECTORIES(ku-mp PUBLIC ${include_dirs})
TARGET_LINK_DIRECTORIES(ku-mp PUBLIC ${STAGED_INSTALL_PREFIX}/lib)
TARGET_LINK_LIBRARIES(ku-mp common_a http_a)


# execute
ADD_EXECUTABLE(ku-media main.c)
ADD_DEPENDENCIES(ku-media ku-mp_a)

TARGET_INCLUDE_DIRECTORIES(ku-media PUBLIC ${include_dirs})
TARGET_LINK_LIBRARIES(ku-media ku-mp_a)
