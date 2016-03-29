
# 2015
reco = "Reco15a"
strp = "Stripping24r0"
    
#for polarity in ["MagUp","MagDown"]:
#for polarity in ["MagDown"]:
for polarity in ["MagUp"]:

    #datapath = "163994-163995/Real Data/"+reco+"/"+strp+"/90000000/MINIBIAS.DST" # MagDown
    #datapath = "166666-166667/Real Data/"+reco+"/"+strp+"/90000000/MINIBIAS.DST" # MagUp
    #datalfns = BKQuery(path=datapath,dqflag="OK",type="Run").getDataset()
    
    #datapath = "/LHCb/Collision15/Beam6500GeV-VeloClosed-"+polarity+"/Real Data/"+reco+"/90000000/RDST"
    #datapath = "163994-163995/Real Data/"+reco+"/90000000/RDST" # MagDown
    datapath = "166666-166667/Real Data/"+reco+"/90000000/RDST" # MagUp
    datalfns = BKQuery(path=datapath,dqflag="OK",type="Run").getDataset()
        
    print "Extracted", len(datalfns), "LFNS for", datapath
    
    if len(datalfns) > 0 :

        j = Job( application = DaVinci( version = 'v38r1p1' ) )
        
        #j.name = "2015-ANNPID-"+polarity+"-"+reco+strp
        j.name = "2015-ANNPID-"+polarity+"-"+reco+"-RDST"

        # Main options
        j.application.optsfile = [ File('options-Data-Run2.py') ]
        
        j.splitter = SplitByFiles ( filesPerJob = 5, maxFiles = 50 )
        
        rootfiles = [ LocalFile('ProtoPIDANN.Data.tuples.root') ]
        
        j.outputfiles = rootfiles
        
        j.inputdata = datalfns
        
        j.do_auto_resubmit = True
        
        j.backend = Dirac()
        
        print "Submitting job", j.name
        j.submit()
        #queues.add( j.submit )
        
