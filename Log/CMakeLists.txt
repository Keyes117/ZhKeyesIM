# ======================== 日志库配置 =========================

set(LOG_SOURCE
    Logger.cpp
)

set(LOG_HEADERS
    Logger.h
    log_export.h
)

add_library(Log SHARED ${LOG_SOURCE} ${LOG_HEADERS})

set_target_properties(Log PROPERTIES
    OUTPUT_NAME "Log"
    DEBUG_POSTFIX "d"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)


target_compile_definitions(Log PRIVATE LOG_EXPORTS)

if(WIN32)
    target_compile_definitions(Log PRIVATE _WIN32)
    set_target_properties(Log PROPERTIES
        IMPORT_PREFIX "lib"
    )
elseif(UNIX AND NOT APPLE)
    set_target_properties(Log PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

install(TARGETS Log
    EXPORT ZhKeyesIMTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(FILES ${LOG_HEADERS} DESTINATION include/Log )