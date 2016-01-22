from ROOT import TFile, TTree
import os, sys

job_all = 2331

jobs_lines = range(2410, 2483) #jobs [662,859]

gangadir = '/afs/cern.ch/work/a/acontu/gangadir/workspace/acontu/LocalXML/' # Location of ganga job logs

for job in jobs_lines : 

    path1 = gangadir + str(job)
    path2 = gangadir + str(job_all)
    #files = sorted(os.listdir(path1))
    
#    print "AAAA"
#    print linename
    #for filename in files : 
    fullname1 = path1 + '/0/output/tuple.root'
    fullname2 = path2 + '/0/output/tupleall.root'
    if not os.path.isfile(fullname1) or not os.path.isfile(fullname2) : continue

    file1 = TFile(fullname1)
    tuple_line = file1.Get("tuple/EventTuple")
    file2 = TFile(fullname2)
    tuple_all = file2.Get("tuple/EventTuple")
    
#	tuple_line.Print()
#	tuple_all.Print()
    
    branches = tuple_line.GetListOfBranches()
    #print branches[0].GetName()
    for br in branches:
        if "EventInSequence"==br.GetName(): continue
        if "StrippingStream" in br.GetName(): continue
        if "Stripping" not in br.GetName(): continue
        linename = None
        events = 0 
        selected = 0
        
        #print branches[2].GetName()
        if linename != None and br.GetName() != linename : 
            print 'Line name is different in the same job! %s != %s' % (linename, br.GetName())
            sys.exit(0)
            
        if linename == None :
            linename = br.GetName()
            print 'Checking line %s (job %d)' % (linename, job)

        nevents = tuple_all.GetEntries()

        if tuple_line.GetEntries() != nevents : 
            print 'Number of events does not match: %d (all lines), %d (%s line)' % (nevents, tuple_line.GetEntries(), linename)
            #sys.exit(0)
            continue

        list_all = []
        for i in tuple_all : 
            list_all += [ eval("i." + linename) ]

        list_line = []
        for i in tuple_line : 
            list_line += [ eval("i." + linename) ]
    #        print "AAAAA"
    #        print list_all
    #        print list_line
        for k in range(0, len(list_all)) : 
            v = list_all[k]
            if list_line[k] != v : 
                l = "Stripping decision is different: line %s, event %d (job %d): all=%d, single=%d\n" % (linename, k, job, v, list_line[k])
                print l
                file = open('independence.log', 'a')
                file.write(l)
                file.close()
            events += 1
            if v == 1: selected += 1

        print 'checked, %d events, %d selected' % (events, selected)
        
    file1.Close()
    file2.Close()
