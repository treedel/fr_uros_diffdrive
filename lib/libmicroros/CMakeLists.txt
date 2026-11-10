target_include_directories(${PROJECT_TITLE} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(${PROJECT_TITLE}
    ${CMAKE_CURRENT_LIST_DIR}/libmicroros.a
)