
set(UTIL_SOURCES
    Util.cpp
)

set(UTIL_HEADERS
    Util.h
    util_export.h
)

add_library(util SHARED ${UTIL_SOURCES} ${UTIL_HEADERS})

set_target_properties(util PROPERTIES
    OUTPUT_NAME "util"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(util PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include> 
)

target_compile_definitions(util PRIVATE UTIL_EXPORTS)

if(WIN32)
    target_compile_definitions(util PRIVATE _WIN32)

    set_target_properties(util PROPERTIES

        IMPORT_PREFIX "lib"

        PREFIX ""
    )
elseif(UNIX AND NOT APPLE)

    set_target_properties(util PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

# 将目标添加到导出列表
install(TARGETS util
    EXPORT ZhKeyesIMTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)