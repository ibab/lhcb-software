from LHCbAlgs import TTParser

TT1=TTParser.parse("../refs/timing.dat")
TT2=TTParser.parse("../refs/timing.csv")

comp=TTParser.compare("../refs/timing.dat","../refs/timing.csv")
if comp!=(['TestAlg4'], ['TestAlg2', 'TestAlg3'], ['TestAlg5']):
    print "Found: ", comp
    print "Expected: ",(['TestAlg4'], ['TestAlg2', 'TestAlg3'], ['TestAlg5'])
    raise ValueError("compare produced the wrong results, cehck stdout")

print "PASS"
