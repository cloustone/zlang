CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

SET(CMAKE_C_FLAGS "-std=gnu99")
SET(CMAKE_CXX_FLAGS "-std=c++17")

if(64_BIT_PLATFORM) 
    MESSAGE(STATUS "using 64bits")  
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")  
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")  
else()  
endif()  

PROJECT(zlang CXX)

ADD_SUBDIRECTORY(external/pugixml)
ADD_SUBDIRECTORY(compiler)
