add_library(Ledger src/RoboBankLedger.cpp)
target_include_directories(Ledger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Ledger PUBLIC Calculator)
