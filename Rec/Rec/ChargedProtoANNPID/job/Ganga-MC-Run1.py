
def jobExists(jobname):
    from Ganga.GPI import jobs
    slice = jobs.select(name=jobname)
    return len(slice) > 0

datasets = [

    ## # Inclusive b
    
    ## { "name"   : "Incb",
    ##   "EvType" : "10000000",
    ##   "year"   : "2011",
    ##   "nu"     : "Nu2",
    ##   "en"     : "3500GeV",
    ##   "trig"   : "Trig0x40760037",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20r1" },
    
    ## { "name"   : "Incb",
    ##   "EvType" : "10000000",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## { "name"   : "Incb-KKGammaX",
    ##   "EvType" : "10002202",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },
    
    ## { "name"   : "Incb-KpiGammaX",
    ##   "EvType" : "10002212",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## { "name"   : "Incb-DiMuontau23mu",
    ##   "EvType" : "10012013",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Inclusive c

    ## { "name"   : "Incc-DiMuontau23mu",
    ##   "EvType" : "20072002",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Electrons

    ## ## K* ee
    ## { "name"   : "BdKstar-ee",
    ##   "EvType" : "11124001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08b"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },
    ## { "name"   : "BdKstar-ee",

    ## ## K* ee phsp
    ## { "name"   : "BdKstar-ee-phsp",
    ##   "EvType" : "11124003",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08b","Sim08c"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bd J/Psi(ee) K* 
    ## { "name"   : "BdJpsiKstar-ee",
    ##   "EvType" : "11154001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08b"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bd J/psi(ee) X
    ## { "name"   : "BdJpsiX-ee",
    ##   "EvType" : "11453001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08c","Sim08d"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bu Kee
    ## { "name"   : "BuKee",
    ##   "EvType" : "12123002",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08b"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bu JPsi(ee) K
    ## { "name"   : "BuJPsiK-ee",
    ##   "EvType" : "12153001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bu J/Psi(ee) X
    ## { "name"   : "BuJPsiX-ee",
    ##   "EvType" : "12952000",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08b","Sim08d"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Lb J/Psi(ee) X
    ## { "name"   : "LbJPsiX-ee",
    ##   "EvType" : "15454101",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08c","Sim08d"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Ks pipiee
    ## { "name"   : "Ks-pipiee",
    ##   "EvType" : "34124004",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08c"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Z
    ## { "name"   : "Zg-ee40GeV",
    ##   "EvType" : "42122001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Ks 4e
    ## { "name"   : "Ks4e",
    ##   "EvType" : "34124001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08c"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Muons

    ## ## Bd ->K* mumu
    ## { "name"   : "BdKstarMuMu",
    ##   "EvType" : "11114001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08b"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bd mumumumu
    ## { "name"   : "Bd4mu",
    ##   "EvType" : "11114041",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08d"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bd Ks mumu
    ## { "name"   : "BdKsMuMu",
    ##   "EvType" : "11114101",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bd J/psi(mumu) K*
    ## { "name"   : "BdJPsiKstar-MuMu",
    ##   "EvType" : "11144001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08c"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bd J/Psi(mumu) X
    ## { "name"   : "BdJPsiX-MuMu",
    ##   "EvType" : "11442001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08d"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bu K mumu
    ## { "name"   : "BuKmumu",
    ##   "EvType" : "12113001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bu K* mumu
    ## { "name"   : "BuKstarMuMu",
    ##   "EvType" : "12115102",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bu J/Psi(mumu) K
    ## { "name"   : "BuJpsiK-mumu",
    ##   "EvType" : "12143001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08e"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bu J/Psi(mumu) pi
    ## { "name"   : "BuJpsipi-mumu",
    ##   "EvType" : "12143010",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bs mumumumu
    ## { "name"   : "Bs4mu",
    ##   "EvType" : "13114020",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08d"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bs mumumumu phsp
    ## { "name"   : "Bs4mu-phsp",
    ##   "EvType" : "13114024",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08e"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bs J/psi(mumu) X
    ## { "name"   : "BsJpsiX-mumu",
    ##   "EvType" : "13442001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08d"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Bs J/psi(mumu) phi
    ## { "name"   : "BsJpsiPhi-mumu",
    ##   "EvType" : "13344002",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Ks 2 mu mu
    ## { "name"   : "Ksmumu",
    ##   "EvType" : "34112100",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08b","Sim08c"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Ks 2 mu mu tight
    ## { "name"   : "Ksmumu-tight",
    ##   "EvType" : "34112104",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08c"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Ks 4mu
    ## { "name"   : "Ks4mu",
    ##   "EvType" : "34114101",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08c"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Sigma pmumu
    ## { "name"   : "SigmaPMuMu",
    ##   "EvType" : "32313001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a","Sim08b"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## tau 3mu
    ## { "name"   : "Tau3mu",
    ##   "EvType" : "31113002",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## tau mu phi
    ## { "name"   : "TauMuPhi",
    ##   "EvType" : "31113045",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08c"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## Protons

    ## ## Lb pK
    ## { "name"   : "LbPKa",
    ##   "EvType" : "15102001",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Lb ppi
    ## { "name"   : "LbPPi",
    ##   "EvType" : "15102011",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## Lb pKphi(KK)
    ## { "name"   : "LbPKaPhi-KK",
    ##   "EvType" : "15104020",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08e"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },
    
    ## ## Lb pKmumu
    ## { "name"   : "LbPKaMuMu",
    ##   "EvType" : "15114011",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08a"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" },

    ## ## lb LcK
    ## { "name"   : "LbLcKa",
    ##   "EvType" : "15164131",
    ##   "year"   : "2012",
    ##   "nu"     : "Nu2.5",
    ##   "en"     : "4000GeV",
    ##   "trig"   : "Trig0x409f0045",
    ##   "sim"    : ["Sim08e"],
    ##   "digi"   : "Digi13",
    ##   "reco"   : "Reco14a",
    ##   "strp"   : "Stripping20" }

    ## Kaons

    ## Bd KK
    { "name"   : "BdKK",
      "EvType" : "11102021",
      "year"   : "2012",
      "nu"     : "Nu2.5",
      "en"     : "4000GeV",
      "trig"   : "Trig0x409f0045",
      "sim"    : ["Sim08b"],
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20" },
    { "name"   : "BdKK",
      "EvType" : "11102021",
      "year"   : "2011",
      "nu"     : "Nu2",
      "en"     : "3500GeV",
      "trig"   : "Trig0x40760037",
      "sim"    : ["Sim08b"],
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20r1" },

    ## Bd KKpi0
    { "name"   : "BdKKPi0",
      "EvType" : "11102422",
      "year"   : "2012",
      "nu"     : "Nu2.5",
      "en"     : "4000GeV",
      "trig"   : "Trig0x409f0045",
      "sim"    : ["Sim08a"],
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20" },
    { "name"   : "BdKKPi0",
      "EvType" : "11102422",
      "year"   : "2011",
      "nu"     : "Nu2",
      "en"     : "3500GeV",
      "trig"   : "Trig0x40760037",
      "sim"    : ["Sim08a"],
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20r1" },

    ## Bd D0Kstar-Kpi
    { "name"   : "BdD0KstarKpi",
      "EvType" : "11164011",
      "year"   : "2012",
      "nu"     : "Nu2.5",
      "en"     : "4000GeV",
      "trig"   : "Trig0x409f0045",
      "sim"    : ["Sim08a","Sim08e"],
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20" },
    { "name"   : "BdD0KstarKpi",
      "EvType" : "11164011",
      "year"   : "2011",
      "nu"     : "Nu2",
      "en"     : "3500GeV",
      "trig"   : "Trig0x40760037",
      "sim"    : ["Sim08e"],
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20r1" },

    ## Bd D-K KKPi
    { "name"   : "BdD-K-KKPi",
      "EvType" : "11264041",
      "year"   : "2012",
      "nu"     : "Nu2.5",
      "en"     : "4000GeV",
      "trig"   : "Trig0x409f0045",
      "sim"    : ["Sim08e"],
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20" },

    ## Bu KKK
    { "name"   : "BuKKK",
      "EvType" : "12103011",
      "year"   : "2012",
      "nu"     : "Nu2.5",
      "en"     : "4000GeV",
      "trig"   : "Trig0x409f0045",
      "sim"    : ["Sim08a"],
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20" },
    { "name"   : "BuKKK",
      "EvType" : "12103012",
      "year"   : "2012",
      "nu"     : "Nu2.5",
      "en"     : "4000GeV",
      "trig"   : "Trig0x409f0045",
      "sim"    : ["Sim08e"],
      "digi"   : "Digi13",
      "reco"   : "Reco14a",
      "strp"   : "Stripping20" },

    ## Bs D0Phi Kpi
    { "name"   : "BsD0PhiKpi",
      "EvType" : "13164001",
      "year"   : "2012",
      "nu"     : "Nu2.5",
      "en"     : "4000GeV",
      "trig"   : "Trig0x409f0045",
      "sim"    : ["Sim08a"],
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
    sims   = dataset["sim"]
    digi   = dataset["digi"]
    reco   = dataset["reco"]
    strp   = dataset["strp"]

    for pythia in ["Pythia6","Pythia8"] :
        for polarity in ["MagUp","MagDown"] :

            for sim in sims :

                jname = name+"-MC"+year+"-ANNPID-"+polarity+"-"+pythia+sim+digi+reco+strp
                if jobExists(jname) :

                    print "Job", jname, "already exists. Skipping."

                else:

                    datapath = "/MC/"+year+"/Beam"+en+"-"+year+"-"+polarity+"-"+nu+"-"+pythia+"/"+sim+"/"+digi+"/"+trig+"/"+reco+"/"+strp+"NoPrescalingFlagged/"+EvType+"/ALLSTREAMS.DST"
        
                    datalfns = BKQuery(path=datapath).getDataset()
                    print "Extracted", len(datalfns), "LFNS for", datapath

                    if len(datalfns) > 0 :

                        j = Job( name = jname, application = DaVinci( version = 'v38r1' ) )

                        # Main options
                        j.application.optsfile = [ File('options-MC'+year+'.py') ]
                        
                        j.splitter = SplitByFiles ( filesPerJob = 4, maxFiles = 999999 )
                        
                        rootfiles = [ SandboxFile('ProtoPIDANN.MC.tuples.root') ]
                        
                        j.outputfiles = rootfiles
                        
                        j.inputdata = datalfns
                        
                        j.do_auto_resubmit = True
                        
                        j.backend = Dirac()
                        
                        print "Submitting job", j.name
                        j.submit()
                        #j.remove()
