cmake_minimum_required(VERSION 2.6)
project(homeplatform)

add_executable(homeplatform main.cpp datastruct/datatag.cpp datastruct/adamtag.cpp adam6x50/adam6x50_conf.cpp adam6x50/adam6x50_thread.cpp log/log.cpp mbserver/mbserver_conf.cpp mbserver/mbserver_thread.cpp cqueue/cqueue.cpp SQLManager/sqlmanager_update_thread.cpp SQLManager/sqlmanager_writes_thread.cpp)

install(TARGETS homeplatform RUNTIME DESTINATION bin)

target_link_libraries (homeplatform pq pthread modbus)

add_subdirectory(adam6x50)
add_subdirectory(log)
add_subdirectory(mbserver)
add_subdirectory(datastruct)
add_subdirectory(cqueue)
add_subdirectory(SQLManager)