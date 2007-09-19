import sys
for i in xrange(len(sys.argv)-1):
    if sys.argv[i+1] == "-PROJ":
        print sys.argv[i+2]
