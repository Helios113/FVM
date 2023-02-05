# Minimum cmake version 
cmake_minimum_required(VERSION 3.23.2)
# Set project name, version and description
project("ness" VERSION 0.0.1 DESCRIPTION "NESS" LANGUAGES CXX)

# Add libs subdirectory which holds all code
add_subdirectory(libs)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Find packages
find_package(Boost REQUIRED COMPONENTS
             program_options system filesystem) 
find_package(Conduit REQUIRED)
find_package(fmt REQUIRED)
find_package(catalyst REQUIRED)
find_package(MPI REQUIRED)


# CryptoPP doesn't support CMake so we need some custom work
add_library(cryptopp SHARED IMPORTED)
set_target_properties(cryptopp PROPERTIES IMPORTED_LOCATION ${CryptoPP_LIB_DIR}/libcryptopp.a)


# Link all libraries and include directories
set(LIB_LIST ${Boost_LIBRARIES} conduit::conduit catalyst::catalyst catalyst::conduit fmt::fmt ${MPI_C_LIBRARIES} msg cli interp crypto cryptopp vis grid poisson)
set(HEADER_LIST ${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/libs/global_definitions ${CryptoPP_INCLUDE_DIR})
set(TARGETS ${PROJECT_NAME} msg interp cli crypto grid vis poisson)

foreach(X IN LISTS TARGETS)
    message(STATUS "Linking ${X}=${LIB_LIST}")
    list(FIND LIB_LIST ${X} HAS_VAL)
    if (NOT ${HAS_VAL} EQUAL -1)
        list(REMOVE_ITEM LIB_LIST ${X})
    endif()
    target_link_libraries(${X} PRIVATE ${LIB_LIST})
    target_include_directories(${X} PRIVATE ${HEADER_LIST})
    if (NOT ${HAS_VAL} EQUAL -1)
        list(APPEND LIB_LIST ${X})
    endif()
endforeach()



# Encrypt Database

file(GLOB MAT_FILES
    database/*.yaml )
set(json "{}")
    
foreach(ITEM ${MAT_FILES})
    file(MD5 ${ITEM} HASH)
    cmake_path(GET ITEM FILENAME FNAME)
    message(STATUS "Name " ${FNAME} " Hash" ${HASH})
    string(JSON json SET ${json} ${FNAME} \"${HASH}\")
endforeach()
message(STATUS "JSON " ${json})
file(WRITE database/hash.json ${json})

file(COPY database DESTINATION .)

file(REMOVE database/hash.json)

# Set install destination
install(TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
