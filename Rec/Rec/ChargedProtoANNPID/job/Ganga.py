
sim="Sim08"
digi="Digi13"
reco="Reco14"
strp="Stripping20"

for pythia in ["Pythia6","Pythia8"] :
    for polarity in ["MagUp","MagDown"]:

        j = Job( application = DaVinci( version = 'v33r2' ) )
        j.name = "MC2012-ANNPID-"+polarity+"-"+pythia+sim+digi+reco+strp

        datapath = "/MC/2012/Beam4000GeV-2012-"+polarity+"-Nu2.5-"+pythia+"/"+sim+"/"+digi+"/Trig0x409f0045/"+reco+"/"+strp+"NoPrescalingFlagged/10000000/ALLSTREAMS.DST"
        
        datalfns = BKQuery(path=datapath).getDataset()
        print "Extracted", len(datalfns), "LFNS for", datapath

        if len(datalfns) > 0 :

            # Main options
            j.application.optsfile = [ File('options.py') ]
            
            j.splitter = SplitByFiles ( filesPerJob = 1, maxFiles = 999999 )
            
            rootfiles = [ SandboxFile('ProtoPIDANN.tuples.root') ]
            
            j.outputfiles = rootfiles
            
            j.inputdata = datalfns

            j.do_auto_resubmit = True
            
            j.backend = Dirac()
            
            print "Submitting job", j.name
            j.submit()
