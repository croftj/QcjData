IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

set(CMAKE_AUTOUIC ON)
add_compile_options(-fPIC)
cmake_minimum_required(VERSION 3.10)
#find_package(Qt5 COMPONENTS Sql Core Network REQUIRED)
#find_package ( Qt5 COMPONENTS Core REQUIRED)
find_package ( Qt5Core REQUIRED )
find_package ( Qt5Widgets REQUIRED )
find_package ( Qt5Sql REQUIRED )
find_package ( Qt5Network REQUIRED )
find_package ( Qt5Concurrent REQUIRED )
find_package ( Qt5PrintSupport REQUIRED )
find_package ( Qt5Xml REQUIRED )

ADD_LIBRARY(QcjData STATIC
    QcjDataFrame.cpp
    QcjDataConfigure.cpp
    QcjDataForm.cpp
    QcjDataHelpers.cpp
    QcjDataKeyValue.cpp
    QcjDataLogin.cpp
    QcjDataReport.cpp
    QcjDataReportDocument.cpp
    QcjDataSheet.cpp
    QcjDataSqlTable.cpp
    QcjDataStatics.cpp
    QcjDataTable.cpp
    QcjDataXML.cpp
    Qcjdoubleentry.cpp
    Qcjfileentry.cpp
    QcjHttpService.cpp
    Qcjintegerentry.cpp
    Qcjsinglelineentry.cpp
    QcjSqlRestApi.cpp
    QcjTcpService.cpp
    Qcjtruefalse.cpp
    Qcjyesnoentry.cpp
    
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjAction.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataConfigure.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataDelegate.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataForm.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataFrame.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataHelpers.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataKeyValue.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataLogin.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataReportDocument.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataReport.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataSheet.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataSqlTable.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataStatics.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataTable.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataXML.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_Qcjdoubleentry.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_Qcjfileentry.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_Qcj.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjHttpServiceFactory.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjHttpService.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_Qcjintegerentry.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_Qcjsinglelineentry.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjSqlRestApi.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_QcjTcpService.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_Qcjtruefalse.cpp
    ${CMAKE_BINARY_DIR}/moc_qcj_Qcjyesnoentry.cpp

    QcjDataConfigure.ui
    QcjDataLogin.ui
    QcjDataReport.ui
    Qcjdoubleentry.ui
    Qcjfileentry.ui
    Qcjintegerentry.ui
    Qcjsinglelineentry.ui
    Qcjtruefalse.ui
    Qcjyesnoentry.ui
)
target_include_directories(QcjData PUBLIC .. .
                           ${Qt5Concurrent_INCLUDE_DIRS}
                           ${Qt5_INCLUDE_DIRS}
                           ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                           ${Qt5Network_INCLUDE_DIRS}
                           ${Qt5Sql_INCLUDE_DIRS}
                           ${Qt5Widgets_INCLUDE_DIRS}
                           ${Qt5Xml_INCLUDE_DIRS}
                           ${Qt5PrintSupport_INCLUDE_DIRS}
                          )
target_compile_features(QcjData PUBLIC cxx_std_17)
target_link_libraries(QcjData)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS " QcjData ${_variableName}=${${_variableName}}")
#endforeach()
#
#get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
#foreach(INCPATH ${inc_dirs})
#    message("QcjData, Include: ${INCPATH}")
#endforeach()

qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjAction.h              ${CMAKE_BINARY_DIR}/moc_qcj_QcjAction.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataConfigure.h       ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataConfigure.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataDelegate.h        ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataDelegate.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataForm.h            ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataForm.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataFrame.h           ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataFrame.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataHelpers.h         ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataHelpers.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataKeyValue.h        ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataKeyValue.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataLogin.h           ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataLogin.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataReportDocument.h  ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataReportDocument.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataReport.h          ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataReport.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataSheet.h           ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataSheet.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataSqlTable.h        ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataSqlTable.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataStatics.h         ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataStatics.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataTable.h           ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataTable.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjDataXML.h             ${CMAKE_BINARY_DIR}/moc_qcj_QcjDataXML.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qcjdoubleentry.h         ${CMAKE_BINARY_DIR}/moc_qcj_Qcjdoubleentry.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qcjfileentry.h           ${CMAKE_BINARY_DIR}/moc_qcj_Qcjfileentry.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qcj.h                    ${CMAKE_BINARY_DIR}/moc_qcj_Qcj.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjHttpServiceFactory.h  ${CMAKE_BINARY_DIR}/moc_qcj_QcjHttpServiceFactory.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjHttpService.h         ${CMAKE_BINARY_DIR}/moc_qcj_QcjHttpService.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qcjintegerentry.h        ${CMAKE_BINARY_DIR}/moc_qcj_Qcjintegerentry.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qcjsinglelineentry.h     ${CMAKE_BINARY_DIR}/moc_qcj_Qcjsinglelineentry.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjSqlRestApi.h          ${CMAKE_BINARY_DIR}/moc_qcj_QcjSqlRestApi.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/QcjTcpService.h          ${CMAKE_BINARY_DIR}/moc_qcj_QcjTcpService.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qcjtruefalse.h           ${CMAKE_BINARY_DIR}/moc_qcj_Qcjtruefalse.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qcjyesnoentry.h          ${CMAKE_BINARY_DIR}/moc_qcj_Qcjyesnoentry.cpp)         

