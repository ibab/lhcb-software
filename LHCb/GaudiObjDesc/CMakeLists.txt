################################################################################
# Package: GaudiObjDesc
################################################################################
gaudi_subdir(GaudiObjDesc v11r18)

gaudi_depends_on_subdirs(GaudiKernel)

# required for nosetests and lxml
find_package(pytools)

gaudi_install_python_modules()
gaudi_install_scripts()

gaudi_install_cmake_modules()

#install(FILES xml_files/gdd.dtd DESTINATION data/GaudiObjDesc)
gaudi_install_resources(xml_files/gdd.dtd)

file(GLOB templates templates/*.tpl)
gaudi_install_resources(${templates} DESTINATION templates)

gaudi_add_library(GaudiObjDescLib *.cpp
                  LINK_LIBRARIES GaudiKernel
                  PUBLIC_HEADERS GaudiObjDesc)

gaudi_add_unit_test(test_allocatorReleaser ../tests/src/test_allocatorReleaser.cpp
                    LINK_LIBRARIES GaudiObjDescLib)

gaudi_add_test(QMTest QMTEST)

gaudi_add_test(nosetests
               COMMAND nosetests -v --with-doctest --with-xunit
                       ${CMAKE_CURRENT_SOURCE_DIR}/python)
