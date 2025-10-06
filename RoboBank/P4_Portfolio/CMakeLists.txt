add_executable(Portfolio src/main.cpp src/portfolio.cpp)
target_include_directories(Portfolio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/P3_Account/include ${CMAKE_SOURCE_DIR}/P1_Calculator/include ${CMAKE_SOURCE_DIR}/P2_Ledger/include)
target_link_libraries(Portfolio PRIVATE Ledger Account Calculator)
