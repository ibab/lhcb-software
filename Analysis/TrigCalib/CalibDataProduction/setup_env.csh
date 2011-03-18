setenv CALDATATOOLS ${PWD}

if(-e /system/SL5/cern/root/x86_64/OxfordSetup-5.27.04.csh) then
setenv ROOTSYS /system/SL5/cern/root/x86_64/root-5.27.04
else
source /afs/cern.ch/sw/lcg/external/gcc/4.3/x86_64-slc5-gcc43-opt/setup.csh
setenv ROOTSYS /afs/cern.ch/sw/lcg/app/releases/ROOT/5.28.00/x86_64-slc5-gcc43-opt/root
endif

setenv PATH ${ROOTSYS}/bin/:${PATH}

if ($?LD_LIBRARY_PATH ) then
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:${LD_LIBRARY_PATH}:${CALDATATOOLS}/lib
else
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib
endif
