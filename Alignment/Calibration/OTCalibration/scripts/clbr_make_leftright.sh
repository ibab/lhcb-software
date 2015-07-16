
gcc -Wall -c -std=c++11 clbr_OTIS_leftright.cpp -I`root-config --incdir`
#gcc -Wall -c readT0s.cpp -I"/afs/cern.ch/sw/lcg/external/Boost/1.48.0_python2.6/x86_64-slc5-gcc43-opt/include/boost-1_48"
gcc -Wall -c -std=c++11 readT0s.cpp -I"/afs/cern.ch/sw/lcg/external/Boost/1.53.0_python2.7/x86_64-slc6-gcc47-opt/include/boost-1_53"
gcc clbr_OTIS_leftright.o readT0s.o -o clbr_OTIS_leftright `root-config --libs`
#gcc clbr_new.o -o clbr_new `root-config --libs`
