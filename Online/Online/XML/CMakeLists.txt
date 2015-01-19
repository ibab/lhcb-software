################################################################################
# Package: XML
################################################################################
gaudi_subdir(XML v0r5)

gaudi_add_library(XML src/*.cpp Evaluator/*.cpp
                  PUBLIC_HEADERS XML)
