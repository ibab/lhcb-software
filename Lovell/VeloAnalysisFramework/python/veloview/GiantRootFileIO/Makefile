TARGETS = VeloGUIUtilsRflx.cxx libVeloGUIUtils.so
alldicts += VeloGUIUtilsRflx.cxx

include Rules.mk

VeloGUIUtilsRflx.cxx: VeloGUIUtilsRflx.h VeloGUIUtilsRflx.xml

libVeloGUIUtils.so: VeloGUIUtilsRflx.os $(patsubst %.cc,%.os,$(allsrc))
