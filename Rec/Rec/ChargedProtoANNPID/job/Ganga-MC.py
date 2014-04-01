

datasets = [

    # Inclusive b
    
    ## { "name"   : "Incb",
    ##   "EvType" : "10000000",
    ##   "year"   : "2011",
    ##   "nu"     : "Nu2",
    ##   "en"     : "3500GeV",
    ##   "trig"   : "Trig0x40760037",
    ##   "sim"    : "Sim08a",
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20r1" },
    
    ## { "name"   : "Incb",
    ##   "EvType" : "10000000",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : "Sim08a",
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## { "name"   : "Incb-KKGammaX",
    ##   "EvType" : "10002202",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : "Sim08a",
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },
    
    ## { "name"   : "Incb-KpiGammaX",
    ##   "EvType" : "10002212",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : "Sim08a",
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## { "name"   : "Incb-DiMuontau23mu",
    ##   "EvType" : "10012013",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : "Sim08a",
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## Inclusive c

    { "name"   : "Incc-DiMuontau23mu",
      "EvType" : "20072002",
      "year"   : "2012",
      "nu"     : "Nu2.5",
      "en"     : "4000GeV",
      "trig"   : "Trig0x409f0045",
      "sim"    : "Sim08a",
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20" }
    
        ]

for dataset in datasets :

    name   = dataset["name"]
    EvType = dataset["EvType"]
    year   = dataset["year"]
    nu     = dataset["nu"]
    en     = dataset["en"]
    trig   = dataset["trig"]
    sim    = dataset["sim"]
    digi   = dataset["digi"]
    reco   = dataset["reco"]
    strp   = dataset["strp"]

    for pythia in ["Pythia6","Pythia8"] :
        for polarity in ["MagUp","MagDown"] : 

            datapath = "/MC/"+year+"/Beam"+en+"-"+year+"-"+polarity+"-"+nu+"-"+pythia+"/"+sim+"/"+digi+"/"+trig+"/"+reco+"/"+strp+"NoPrescalingFlagged/"+EvType+"/ALLSTREAMS.DST"
        
            datalfns = BKQuery(path=datapath).getDataset()
            print "Extracted", len(datalfns), "LFNS for", datapath

            if len(datalfns) > 0 :

                j = Job( application = DaVinci( version = 'v34r0' ) )
                j.name = name+"-MC"+year+"-ANNPID-"+polarity+"-"+pythia+sim+digi+reco+strp

                # Main options
                j.application.optsfile = [ File('options-MC'+year+'.py') ]
            
                j.splitter = SplitByFiles ( filesPerJob = 1, maxFiles = 1 )
                
                rootfiles = [ SandboxFile('ProtoPIDANN.MC.tuples.root') ]
            
                j.outputfiles = rootfiles
            
                j.inputdata = datalfns

                j.do_auto_resubmit = True
            
                j.backend = Dirac()
            
                print "Submitting job", j.name
                j.submit()
                #j.remove()
