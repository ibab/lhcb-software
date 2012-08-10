
for polarity in ["MagUp","MagDown"]:

    reco="Reco13a"
    strp="Stripping19a"

    j = Job( application = DaVinci( version = 'v32r1p2' ) )
    j.name = "MC2012-ANNPID-"+polarity+"-"+reco+"-"+strp+"-TEST"

    datapath = "/MC/DEV/Beam4000GeV-MayJune2012-"+polarity+"-Nu2.5-EmNoCuts/Sim06a/Trig0x0097003dFlagged/"+reco+"/"+strp+"NoPrescalingFlagged/10000000/ALLSTREAMS.DST"

    # Main options
    j.application.optsfile = [ File('options.py') ]

    j.splitter = DiracSplitter ( filesPerJob = 2, maxFiles = 999999 )
    #j.splitter = SplitByFiles ( filesPerJob = 2, maxFiles = 6 )

    rootfiles = [ 'ProtoPIDANN.tuples.root' ]

    j.outputsandbox = rootfiles

    j.inputdata = BKQuery(path=datapath).getDataset()

    # j.merger = SmartMerger( files        = rootfiles,
    #                        ignorefailed = True,
    #                        overwrite    = True )

    j.do_auto_resubmit = True
    
    j.backend = Dirac()

    print "Submitting job", j.name
    j.submit()
