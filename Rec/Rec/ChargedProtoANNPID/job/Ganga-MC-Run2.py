
def jobExists(jobname):
    from Ganga.GPI import jobs
    slice = jobs.select(name=jobname)
    return len(slice) > 0

datasets = [

    { "EvTypes" : ["10000000"],
      "simcond" : "Dev",
      "year"    : "Jun2015",
      "lhccond" : "Nu1.6-25ns",
      "en"      : "6500GeV",
      "pols"    : ["MagUp","MagDown"],
      "trig"    : "Trig0x410700a1",
      "sim"     : ["Sim09Dev03"],
      "reco"    : "Reco15",
      "turbo"   : "Turbo01a",
      "strp"    : "Stripping23r1NoPrescalingFlagged",
      "pythias" : ["Pythia8"],
      "ftype"   : "ALLSTREAMS.DST" 
      }

    ## { "EvTypes" : ["10000000","11296013","11442001","12143001","12165051",
    ##                "12165151","12195001","12442001","13264021","13442001",
    ##                "15164001","18112021","18112251","24142001"],
    ##   "simcond" : "Dev",
    ##   "year"    : "Jun2015",
    ##   "lhccond" : "Nu1.6-25ns",
    ##   "en"      : "6500GeV",
    ##   "pols"    : ["MagUp","MagDown"],
    ##   "trig"    : "Trig0x410700a1",
    ##   "sim"     : ["Sim09Dev03"],
    ##   "reco"    : "Reco15",
    ##   "turbo"   : "Turbo01a",
    ##   "strp"    : "Stripping23r1NoPrescalingFlagged",
    ##   "pythias" : ["Pythia8"],
    ##   "ftype"   : "ALLSTREAMS.DST" 
    ##   },

    ## { "EvTypes"  : ["14143013"],
    ##   "simcond" : "Dev",
    ##   "year"    : "Jun2015",
    ##   "lhccond" : "Nu1.6-25ns",
    ##   "en"      : "6500GeV",
    ##   "pols"    : ["MagDown"],
    ##   "trig"    : "Trig0x410700a1",
    ##   "sim"     : ["Sim09Dev03"],
    ##   "reco"    : "Reco15",
    ##   "turbo"   : "Turbo01a",
    ##   "strp"    : "Stripping23r1NoPrescalingFlagged",
    ##   "pythias" : ["BcVegPyPythia8"],
    ##   "ftype"   : "ALLSTREAMS.DST" 
    ##   },

    ## { "EvTypes" : ["14143013"],
    ##   "simcond" : "Dev",
    ##   "year"    : "Jun2015",
    ##   "lhccond" : "Nu1.6",
    ##   "en"      : "6500GeV",
    ##   "pols"    : ["MagDown"],
    ##   "trig"    : "Trig0x40f9014e",
    ##   "sim"     : ["Sim09Dev02"],
    ##   "reco"    : "Reco15em",
    ##   "turbo"   : "Turbo01em",
    ##   "strp"    : "",
    ##   "pythias" : ["BcVegPyPythia8"],
    ##   "ftype"   : "DST" 
    ##   },

    ## { "EvTypes" : ["18112021","18112251","24142001","27163003","42112001"],
    ##   "simcond" : "Dev",
    ##   "year"    : "Jun2015",
    ##   "lhccond" : "Nu1.6",
    ##   "en"      : "6500GeV",
    ##   "pols"    : ["MagDown"],
    ##   "trig"    : "Trig0x40f9014e",
    ##   "sim"     : ["Sim09Dev02"],
    ##   "reco"    : "Reco15em",
    ##   "turbo"   : "Turbo01em",
    ##   "strp"    : "",
    ##   "pythias" : ["Pythia8"],
    ##   "ftype"   : "DST" 
    ##   },

    ## { "EvTypes" : ["28102062"],
    ##   "simcond" : "2015",
    ##   "year"    : "Jun2015",
    ##   "lhccond" : "Nu1.6",
    ##   "en"      : "6500GeV",
    ##   "pols"    : ["MagUp","MagDown"],
    ##   "trig"    : "Trig0x40f9014e",
    ##   "sim"     : ["Sim08h-em"],
    ##   "reco"    : "Reco15em",
    ##   "turbo"   : "Turbo01aEM",
    ##   "strp"    : "",
    ##   "pythias" : ["Pythia6","Pythia8"],
    ##   "ftype"   : "DST" 
    ##   },

    ## { "EvTypes" : ["13436000","13466011","21113001","21113015","21113016",
    ##                "21123011","21173001","23113001","23173001","49011005",
    ##                "42100001","42112001","42300001","42311003","49011004",
    ##                "30000000"],
    ##   "simcond" : "2015",
    ##   "year"    : "Jun2015",
    ##   "lhccond" : "Nu1.6",
    ##   "en"      : "6500GeV",
    ##   "pols"    : ["MagUp","MagDown"],
    ##   "trig"    : "Trig0x40f9014e",
    ##   "sim"     : ["Sim08h-em"],
    ##   "reco"    : "Reco15em",
    ##   "turbo"   : "Turbo01em",
    ##   "strp"    : "",
    ##   "pythias" : ["Pythia8"],
    ##   "ftype"   : "DST" 
    ##   }
    
        ]

for dataset in datasets :

    EvTypes = dataset["EvTypes"]
    simcond = dataset["simcond"]
    year    = dataset["year"]
    lhccond = dataset["lhccond"]
    en      = dataset["en"]
    pols    = dataset["pols"]
    trig    = dataset["trig"]
    sims    = dataset["sim"]
    reco    = dataset["reco"]
    turbo   = dataset["turbo"]
    strp    = dataset["strp"]
    pythias = dataset["pythias"]
    ftype   = dataset["ftype"]

    for EvType in EvTypes :
        for pythia in pythias :
            for polarity in pols :

                for sim in sims :

                    jname = EvType+"-"+simcond+"-MC"+year+"-ANNPID-"+polarity+"-"+pythia+sim+trig+reco+turbo+strp
                    if jobExists(jname) :
                        
                        print "Job", jname, "already exists. Skipping."
                        
                    else:

                        datapath = ("/MC/"+simcond+"/Beam"+en+"-"+year+"-"+polarity+"-"+lhccond+"-"+pythia+"/"+sim+"/"+trig+"/"+reco+"/"+turbo+"/"+strp+"/"+EvType+"/"+ftype).replace("//", "/")

                        datalfns = BKQuery(path=datapath).getDataset()

                        import random
                        random.shuffle(datalfns.files)
                        
                        print "Extracted", len(datalfns), "LFNS for", datapath

                        if len(datalfns) > 0 :
                            
                            j = Job( name = jname, application = DaVinci( version = 'v38r1p1' ) )
                            
                            # Main options
                            j.application.optsfile = [ File('options-MC'+year+'.py') ]
                            
                            j.splitter = SplitByFiles ( filesPerJob = 5, maxFiles = 999999 )
                            #j.splitter = SplitByFiles ( filesPerJob = 4, maxFiles = 50 )
                            
                            rootfiles = [ LocalFile('ProtoPIDANN.MC.tuples.root') ]
                            
                            j.outputfiles = rootfiles
                            
                            j.inputdata = datalfns
                            
                            j.do_auto_resubmit = True
                            
                            j.backend = Dirac()

                            import glob
                            for tune in ["MC12TuneV4","MC15TuneV1"] :
                                configfiles = glob.glob("/usera/jonesc/cmtuser/DaVinci_v38r1p1/Rec/ChargedProtoANNPID/data/"+tune+"/*")
                                j.inputsandbox += [ File( name = f, subdir = "data/"+tune+"/" ) for f in configfiles ]
                            
                            print "Submitting job", j.name
                            j.submit()
                            #queues.add( j.submit )
                            # j.remove()
