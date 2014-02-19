
import sys

f1 = open(sys.argv[1]);
f2 = open(sys.argv[2]);

list1 = []
list2 = []

import difflib

errorcounter = 0 
trackcounter = 0

for l1 in f1 :
    if not "LHCbIDs" in l1 : continue
    else : 
        #print l1
        trackcounter+=1
        # buffer the LHCbIDs
        for l1a in f1 :
            #print l1a
            if "]" in l1a : break
            else : list1.append(l1a)
        
        for l2 in f2 :
            if not "LHCbIDs" in l2 : continue
            else : 
                #print l2
                # now buffer 
                for l2a in f2 :
                    #print l2a
                    if "]" in l2a : break
                    else : 
                        list2.append(l2a)
                # now compare
                #print list1
                #print list2
                difflib.SequenceMatcher(None,list1[0],list2[0])
                if list1 != list2 :
                    errorcounter +=1
                    if len(list1) != len(list2) :
                        print "Different number of LHCbIDs in track!"
                        print
                        print "".join(list1)
                        print "".join(list2)
                    else : 
                        for i in range(0,len(list1)) :
                            if list1[i] != list2[i] : 
                                print list1[i]
                                print list2[i]
                list1[:] =[]
                list2[:] =[]
                break

        if list1 != [] : print "Tracks missing in second file!"


print
            
if errorcounter > 0 :
    print "Found " + str(errorcounter) + " mismatches in " + str(trackcounter) + " tracks"
else : print "No mismatch found in " +str(trackcounter) +" tracks. All good. Except for things we did not think of. Yet."
