
# 2012
reco = "Reco14"
#strp = "Stripping21"
strp = "Stripping20"
    
for polarity in ["MagUp","MagDown"]:

    datatype = "BHADRONCOMPLETEEVENT"
    #datatype = "MINIBIAS"
     
    datapath = "/LHCb/Collision12/Beam4000GeV-VeloClosed-"+polarity+"/Real Data/"+reco+"/"+strp+"/90000000/"+datatype+".DST"
    datalfns = BKQuery(path=datapath,dqflag="OK").getDataset()
    print "Extracted", len(datalfns), "LFNS for", datapath

    import random
    random.shuffle(datalfns.files)
    
    if len(datalfns) > 0 :

        j = Job( application = DaVinci( version = 'v38r1p1' ) )
        j.name = "2012-ANNPID-"+datatype+"-"+polarity+"-"+reco+strp

        # Main options
        j.application.optsfile = [ File('options-Data-Run1.py') ]

        j.splitter = SplitByFiles ( filesPerJob = 4, maxFiles = 50 )

        import glob
        for tune in ["MC12TuneV4","MC15TuneV1"] :
            configfiles = glob.glob("/usera/jonesc/cmtuser/DaVinci_v38r1p1/Rec/ChargedProtoANNPID/data/"+tune+"/*")
            j.inputsandbox += [ File( name = f, subdir = "data/"+tune+"/" ) for f in configfiles ]
            #j.inputfiles += [ LocalFile( namePattern = f, localDir = "data/"+tune+"/" ) for f in configfiles ]
        #j.inputfiles += [ LocalFile("/usera/jonesc/ann.tar") ]
        
        rootfiles = [ LocalFile('ProtoPIDANN.Data.tuples.root') ]
        
        j.outputfiles = rootfiles
        
        j.inputdata = datalfns
        
        j.do_auto_resubmit = True
        
        j.backend = Dirac()

        print "Submitting job", j.name
        j.submit()
