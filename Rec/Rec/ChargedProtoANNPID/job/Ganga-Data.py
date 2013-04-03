
# 2012
reco = "Reco14"
strp = "Stripping20"
    
for polarity in ["MagUp","MagDown"]:

    datapath = "/LHCb/Collision12/Beam4000GeV-VeloClosed-"+polarity+"/Real Data/"+reco+"/"+strp+"/90000000/BHADRONCOMPLETEEVENT.DST"
    datalfns = BKQuery(path=datapath,dqflag="OK").getDataset()
    print "Extracted", len(datalfns), "LFNS for", datapath
    
    if len(datalfns) > 0 :

        j = Job( application = DaVinci( version = 'v33r2' ) )
        j.name = "2012-ANNPID-"+polarity+"-"+reco+strp

        # Main options
        j.application.optsfile = [ File('options-Data.py') ]
        
        j.splitter = SplitByFiles ( filesPerJob = 2, maxFiles = 50 )
        
        rootfiles = [ SandboxFile('ProtoPIDANN.Data.tuples.root') ]
        
        j.outputfiles = rootfiles
        
        j.inputdata = datalfns
        
        j.do_auto_resubmit = True
        
        j.backend = Dirac()
        
        print "Submitting job", j.name
        j.submit()
