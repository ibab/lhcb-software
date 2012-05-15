
gcc -Wall -c clbr.cpp -I`root-config --incdir`
gcc -Wall -c readT0s.cpp -I"/afs/cern.ch/sw/lcg/external/Boost/1.48.0_python2.6/x86_64-slc5-gcc43-opt/include/boost-1_48"
gcc clbr.o readT0s.o -o clbr `root-config --libs`
