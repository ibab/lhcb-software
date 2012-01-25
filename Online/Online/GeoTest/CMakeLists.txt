cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(GeoTest)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake) 

find_package(XercesC)
find_package(ROOT)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} 
                     ${XERCESC_INCLUDE_DIR} ${ROOT_INCLUDE_DIR})

file(GLOB sources src/XML/*.cpp src/XML/lcdd/*.cpp src/XML/compact/*.cpp  
                  src/DetDesc/*.cpp  src/DetDesc/lcdd/*.cpp  src/DetDesc/compact/*.cpp  
                  src/DetDesc/detector/*.cpp
                  src/Evaluator/*.cpp)


add_executable(GeomConverterApp main/main.cpp ${sources})
target_link_libraries(GeomConverterApp ${XERCESC_LIBRARIES} ${ROOT_LIBRARIES} Geom XMLIO Rint)



file(GLOB detdesc_srcs src/DetDesc/*.cpp src/DetDesc/lcdd/*.cpp)
ROOT_GENERATE_DICTIONARY( G__DetDesc DetDesc/lcdd/LCDD.h 
                                     DetDesc/lcdd/Objects.h
                                     DetDesc/lcdd/Volumes.h
                                     DetDesc/compact/Detector.h
                                     DetDesc/detector/ILDExTPC.h
                          LINKDEF DetDesc/LinkDef.h)

add_library(DetDesc SHARED ${sources} G__DetDesc.cxx)
target_link_libraries(DetDesc ${ROOT_LIBRARIES} Geom XMLIO ${XERCESC_LIBRARIES})


add_executable(PyGeomConverter main/pymain.cpp)
target_link_libraries(PyGeomConverter DetDesc ${ROOT_LIBRARIES} PyROOT Rint)
