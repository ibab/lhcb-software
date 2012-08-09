
for polarity in ["MagUp","MagDown"]:

    reco="Reco13d"
    strp="Stripping19b"

    j = Job( application = DaVinci( version = 'v32r1p1' ) )
    j.name = "MC2012-ANNPID-"+polarity+"-"+reco+"-"+strp

    datapath = "/MC/DEV/Beam4000GeV-MayJune2012-"+polarity+"-Nu2.5-EmNoCuts/Sim06a/Trig0x0097003dFlagged/"+reco+"/"+strp+"NoPrescalingFlagged/10000000/ALLSTREAMS.DST"

    # Main options
    j.application.optsfile = [ File('options.py') ]

    j.splitter = SplitByFiles ( filesPerJob = 3, maxFiles = 999999 )

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
