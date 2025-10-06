add_library(Account src/Account.cpp)
target_include_directories(Account PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Account PUBLIC Calculator)
