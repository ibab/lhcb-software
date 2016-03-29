#!/usr/bin/env python

def getGangaIDs(jobname):

    print "Getting Job IDs matching name", jobname

    from Ganga.GPI import jobs
 
    jobids = [ j.id for j in jobs.select(name=jobname) ]

    return jobids

def moveCalibrationFiles():

    import glob, os, shutil, sys, random

    from Ganga.GPI import jobs
    
    gangaDir = "/usera/jonesc/gangadir/workspace/jonesc/LocalXML"

    outputRoot = "/r02/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/MC/Sim08xem/Reco15em/"

    mergeJobs = [

        # Run1
        
        ## { "Name" : "Incb",   "Regex" : "Incb-MC20*-ANNPID-*",   "FirstID" : 1 },
        ## { "Name" : "Incc-X", "Regex" : "Incc-*-MC20*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Incb-X", "Regex" : "Incb-*-MC20*-ANNPID-*", "FirstID" : 1 },

        ## { "Name" : "El-BdKstar", "Regex" : "BdKstar-ee-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "El-BdJPsiKstar", "Regex" : "BdJpsiKstar-ee-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "El-BdJPsiX", "Regex" : "BdJpsiX-ee-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "El-BuKee", "Regex" : "BuKee-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "El-BuJPsiK", "Regex" : "BuJPsiK-ee-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "El-BuJPsiX", "Regex" : "BuJPsiX-ee-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "El-LbJPsiX", "Regex" : "LbJPsiX-ee-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "El-Kspipiee", "Regex" : "Ks-pipiee-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "El-Zgee40GeV", "Regex" : "Zg-ee40GeV-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "El-Ks4e", "Regex" : "Ks4e-*-ANNPID-*", "FirstID" : 1 },

        ## { "Name" : "Mu-BdKstarmumu", "Regex" : "BdKstarMuMu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-Bd4mu", "Regex" : "Bd4mu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-BdKsmumu", "Regex" : "BdKsMuMu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-BdJPsiKstar", "Regex" : "BdJPsiKstar-MuMu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-BdJPsiX", "Regex" : "BdJPsiX-MuMu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-BuKmumu", "Regex" : "BuKmumu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-BuKstarmumu", "Regex" : "BuKstarMuMu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-BuJPsiK", "Regex" : "BuJpsiK-mumu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-BuJPsiPi", "Regex" : "BuJpsipi-mumu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-Bs4mu", "Regex" : "Bs4mu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-BsJPsiX", "Regex" : "BsJpsiX-mumu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-BsJPsiPhi", "Regex" : "BsJpsiPhi-mumu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-Ksmumu", "Regex" : "Ksmumu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-Ks4mu", "Regex" : "Ks4mu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-SigmaPmumu", "Regex" : "SigmaPMuMu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-Tau3mu", "Regex" : "Tau3mu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Mu-TauMuPhi", "Regex" : "TauMuPhi-*-ANNPID-*", "FirstID" : 1 },

        ## { "Name" : "Pr-LbPKa", "Regex" : "LbPKa-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Pr-LbPPi", "Regex" : "LbPPi-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Pr-LbPKaPhi", "Regex" : "LbPKaPhi-KK-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Pr-LbPKaMuMu", "Regex" : "LbPKaMuMu-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Pr-LbLcKa", "Regex" : "LbLcKa-*-ANNPID-*", "FirstID" : 1 },

        ## { "Name" : "Ka-BdKK", "Regex" : "BdKK-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Ka-BdKKPi0", "Regex" : "BdKKPi0-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Ka-BdD0KstarKpi", "Regex" : "BdD0KstarKpi-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Ka-BdD-K-KKPi", "Regex" : "BdD-K-KKPi-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Ka-BuKKK", "Regex" : "BuKKK-*-ANNPID-*", "FirstID" : 1 },
        ## { "Name" : "Ka-BsD0PhiKpi", "Regex" : "BsD0PhiKpi-*-ANNPID-*", "FirstID" : 1 }

        # Run 2
        #{ "Name" : "Dev03", "Regex" : "*-ANNPID-*Sim09*Reco15Turbo01a*", "FirstID" : 1 },
        #{ "Name" : "Dev02", "Regex" : "*-ANNPID-*Sim09*Reco15emTurbo01*", "FirstID" : 1 }
        
        { "Name" : "Sim08EM", "Regex" : "*-ANNPID-*Sim08*Reco15emTurbo01*", "FirstID" : 1 }
        
        ]

    for mergeJob in mergeJobs :

        outPath = outputRoot + mergeJob["Name"]
    
        if not os.path.exists(outPath) : os.makedirs(outPath)

        jobids = getGangaIDs( mergeJob["Regex"] )

        rootfiles = [ ]
        for id in jobids :
            newfiles = glob.glob(gangaDir+"/"+str(id)+'/*/output/ProtoPIDANN.MC.tuples.root')
            print " -> Globbed", len(newfiles), "root files from", jobs(id).name
            for file in newfiles : rootfiles.append(file)
        random.shuffle(rootfiles)

        filelist = mergeJob["Name"] + "-list.txt"
        
        file = open(filelist, "w")

        i = mergeJob["FirstID"]
        for rootfile in rootfiles :
            target = outPath + "/ANNPID." + str(i) + ".root"
            print "Moving", rootfile, "to", target
            shutil.move(rootfile,target)
            file.write(target+":ANNPID/DecayTree\n")
            i = i + 1

        file.close()
