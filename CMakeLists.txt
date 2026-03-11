cmake_minimum_required(VERSION 3.16)
project(SmittyServices VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system filesystem)

find_library(WT_LIB wt)
find_library(WT_HTTP_LIB wthttp)
find_path(WT_INCLUDE_DIR Wt/WApplication.h)

if(NOT WT_LIB OR NOT WT_HTTP_LIB)
    message(FATAL_ERROR "Wt libraries not found. Install libwt-dev and libwthttp-dev.")
endif()

find_package(CURL REQUIRED)

set(SOURCES
    src/main.cpp
    src/SmittyApplication.cpp
    src/NavBar.cpp
    src/SideBar.cpp
    src/Footer.cpp
    src/Entity.cpp
    src/EntityRegistry.cpp
    src/EntityListView.cpp
    src/EntityDetailView.cpp
    src/ApiClient.cpp
    src/Auth.cpp
    src/AppSettings.cpp
    src/Dashboard.cpp
    src/CustomerList.cpp
    src/CustomerDetail.cpp
    src/OrderList.cpp
    src/OrderDetail.cpp
    src/ProductList.cpp
    src/ProductDetail.cpp
    src/Settings.cpp
    src/pdfgen.c
    src/PdfReport.cpp
    src/OrderReport.cpp
    src/CustomerStatementReport.cpp
    src/RevenueReport.cpp
)

add_executable(smitty_services ${SOURCES})

target_include_directories(smitty_services
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    SYSTEM PRIVATE
        ${WT_INCLUDE_DIR}
        ${CURL_INCLUDE_DIRS}
)

target_link_libraries(smitty_services PRIVATE
    ${WT_LIB}
    ${WT_HTTP_LIB}
    Boost::system
    Boost::filesystem
    ${CURL_LIBRARIES}
    pthread
)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
file(COPY wt-resources DESTINATION ${CMAKE_BINARY_DIR})
file(COPY database DESTINATION ${CMAKE_BINARY_DIR})
file(GLOB SCRIPT_FILES scripts/*.sh)
file(COPY ${SCRIPT_FILES} DESTINATION ${CMAKE_BINARY_DIR})
