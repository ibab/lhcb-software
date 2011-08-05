compile_only()
{
    for i in `ls Test/*.cpp` `ls *.cpp`; 
    do 
	echo "Compile ${i}";
	g++ -msse2 -msse3 -msse4 -Wall -Wextra -ansi -pthread -lrt -ldl \
	    -I`pwd` -I`pwd`/Test ${i};
    done;
}

#
g++ -msse2 -msse3 -msse4 -Wall -Wextra -ansi -pthread -lrt -ldl \
     -I`pwd` -I`pwd`/Test `ls Test/*.cpp` `ls *.cpp`;
#
#
#g++ -msse2 -msse3 -msse4 -pthread -I`pwd` -I`pwd`/Test `ls Test/*.cpp` -c
#g++ -msse2 -msse3 -msse4 -pthread -I`pwd` `ls RTL/*.cpp` -c -o rtl.o
#g++ -msse2 -msse3 -msse4 -pthread -I`pwd` `ls Framework/*.cpp` -c -o frame.o
#g++ -msse2 -msse3 -msse4 -pthread -lrt `ls *.o` `ls Test/*.o`

 