cmake_minimum_required(VERSION 3.16)
project(RoboBankPortfolio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(P1_Calculator)
add_subdirectory(P2_Ledger)
add_subdirectory(P3_Account)
add_subdirectory(P4_Portfolio)
