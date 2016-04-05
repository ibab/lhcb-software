
# 2015
reco = "Reco15a"
#strp = "Stripping24"
strp = "Stripping23r1"

for polarity in ["MagUp","MagDown"]:
#for polarity in ["MagDown"]:
#for polarity in ["MagUp"]:

    datatype = "BHADRONCOMPLETEEVENT"
    #datatype = "MINIBIAS"

    datapath = "/LHCb/Collision15/Beam6500GeV-VeloClosed-"+polarity+"/Real Data/"+reco+"/"+strp+"/90000000/"+datatype+".DST"
    datalfns = BKQuery(path=datapath,dqflag="OK").getDataset()
    
    #datapath = "163994-163995/Real Data/"+reco+"/"+strp+"/90000000/"+datatype+".DST" # MagDown
    #datapath = "166666-166667/Real Data/"+reco+"/"+strp+"/90000000/"+datatype+".DST" # MagUp
    #datalfns = BKQuery(path=datapath,dqflag="OK",type="Run").getDataset()
    
    #datapath = "/LHCb/Collision15/Beam6500GeV-VeloClosed-"+polarity+"/Real Data/"+reco+"/90000000/RDST"
    #datapath = "163994-163995/Real Data/"+reco+"/90000000/RDST" # MagDown
    #datapath = "166666-166667/Real Data/"+reco+"/90000000/RDST" # MagUp
    #datalfns = BKQuery(path=datapath,dqflag="OK",type="Run").getDataset()

    import random
    random.shuffle(datalfns.files)
        
    print "Extracted", len(datalfns), "LFNS for", datapath
    
    if len(datalfns) > 0 :

        j = Job( application = DaVinci( version = 'v38r1p1' ) )
        
        j.name = "2015-ANNPID-"+datatype+"-"+polarity+"-"+reco+strp
        #j.name = "2015-ANNPID-"+polarity+"-"+reco+"-RDST"

        # Main options
        j.application.optsfile = [ File('options-Data-Run2.py') ]
        
        j.splitter = SplitByFiles ( filesPerJob = 4, maxFiles = 50 )

        import glob
        for tune in ["MC12TuneV4","MC15TuneV1"] :
            configfiles = glob.glob("/usera/jonesc/cmtuser/DaVinci_v38r1p1/Rec/ChargedProtoANNPID/data/"+tune+"/*")
            j.inputsandbox += [ File( name = f, subdir = "data/"+tune+"/" ) for f in configfiles ]
        
        rootfiles = [ LocalFile('ProtoPIDANN.Data.tuples.root') ]
        
        j.outputfiles = rootfiles
        
        j.inputdata = datalfns
        
        j.do_auto_resubmit = True
        
        j.backend = Dirac()
        
        print "Submitting job", j.name
        j.submit()
        #queues.add( j.submit )
        
