cmake_minimum_required(VERSION 3.10)
project(ChopMoneyRaffleApi_test CXX)

add_executable(${PROJECT_NAME} test_main.cc)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

ParseAndAddDrogonTests(${PROJECT_NAME})

add_executable(ticket_date_filter_test ticket_date_filter_test.cc)
add_test(NAME ticket_date_filter_test COMMAND ticket_date_filter_test)
