#!/usr/bin/env python

import glob, os, shutil, sys, random

gangaDir = "/usera/jonesc/gangadir/workspace/jonesc/LocalXML"

mergeJobs = [

    { "OutputDir" : "/r03/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/Sim08a/Digi13/Reco14a/MC/",
      "EventType" : "10000000",
      "JobIDs"    : [ 269, 270, 271, 272, 273, 274, 275, 276 ]
      }

    ]


for mergeJob in mergeJobs :

    outPath = mergeJob["OutputDir"] + mergeJob["EventType"]
    
    if not os.path.exists(outPath) : os.makedirs(outPath)

    rootfiles = [ ]
    for id in mergeJob["JobIDs"] :
        for file in glob.glob(gangaDir+"/"+str(id)+'/*/output/ProtoPIDANN.MC.tuples.root') :
            rootfiles.append(file)
        
    random.shuffle(rootfiles)

    filelist = mergeJob["EventType"] + "-list.txt"
    file = open(filelist, "w")

    i = 0
    for rootfile in rootfiles :
        i = i + 1
        target = outPath + "/ProtoPIDANN." + str(i) + ".root"
        print "Moving", rootfile, "to", target
        #os.rename(rootfile,target)
        file.write(target+"\n")

    file.close()
        

sys.exit(0)
