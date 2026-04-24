add_executable(oms
    main.cpp
    service/OMSService.cpp
)

target_include_directories(oms PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})