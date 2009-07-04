"""
 =============================================================
 Class to configure MuonPIDChecker. 
 Author: Jose Angel Hernando Morata, Miriam Gandelman
 =============================================================
"""

import os

from Gaudi.Configuration import *

from Configurables import MuonPIDChecker
from Configurables import GaudiSequencer

class ConfigureMuonPIDChecker():

  def __init__(self, data="DC06",version="def", debug=False):
    """
    initialization for the class. Use as input data type (DC06,MC09,etc) and version of it if necessary.
    """
    self.debug=debug
    if self.debug:
      print "ConfigureMuonPIDChecker data ",data," version",version

    ## from datatype and version look for module with data. Store extra modules in case desired does not exist
    mod=[data+"_"+version,data+"_def","DC06_def"]
    mod = map(lambda x: "Muon_"+x,mod)

    PYMUONIDROOT = os.environ["MUONPIDCHECKERROOT"]+"/python/MuonPIDChecker/"
    pname=[]
    for m in mod: pname.append(PYMUONIDROOT+m+".py")

    if self.debug: print "\tpname -> ",pname

    ## check if modules exist and load them
    if os.path.exists(pname[0]): exec("from MuonPIDChecker import "+mod[0]+" as info")
    else:
      if debug: print "WARNING: not available info for DATA=%s,VERSION=%s. Loading default" %(data,version)
      if os.path.exists(pname[1]): exec("from MuonPIDChecker import "+mod[1]+" as info")
      else:  exec("from MuonPIDChecker import "+mod[2]+" as info")

    ## set final module with info to be laoded
    self.info=info
    self.info.DEBUG = debug
    self.initializeAll = True 
   
  def configure(self,name="MuonPID",mc=True,expertck=True):
    """
    configure the MuonPIDchecker algorithm
    """

    if self.debug: print "CONFIGURING MUONPIDCHECKER"

    ## check if input is already an instance or this must be created
    ext = "Checker"
    if (not mc): ext = "Monitor"
    myalg = MuonPIDChecker(name+ext)

    myalg.HistoTopDir = "Muon/"
    myalg.IsMuonFlag  = 0
    myalg.DLLFlag   = 1  
    myalg.DLLCut = 1.20
    myalg.ProbCut1 = 0.90
    myalg.ProbCut2 = 0.80
    myalg.DLLlower = -1.0
    myalg.DLLupper = 6.0
    myalg.DLLnbins = 35 
    myalg.NSHCut = 1
    myalg.RunningMC = mc
    myalg.ExpertCheck = expertck
    myalg.MuonTrackLocation = 'Rec/Track/Muon'
    myalg.HistoDir = name+"/"+ext
    myalg.RefEff1IM   = self.info.RefEff1IM
    myalg.RefEff1IML  = self.info.RefEff1IML
    myalg.RefEff1DLL  = self.info.RefEff1DLL
    myalg.RefEff1NSH  = self.info.RefEff1NSH
    myalg.RefdEff1IM  = self.info.RefdEff1IM
    myalg.RefdEff1IML = self.info.RefdEff1IML
    myalg.RefdEff1DLL = self.info.RefdEff1DLL
    myalg.RefdEff1NSH = self.info.RefdEff1NSH
    myalg.RefEff2IM   = self.info.RefEff2IM
    myalg.RefEff2IML  = self.info.RefEff2IML
    myalg.RefEff2DLL  = self.info.RefEff2DLL
    myalg.RefEff2NSH  = self.info.RefEff2NSH
    myalg.RefdEff2IM  = self.info.RefdEff2IM
    myalg.RefdEff2IML = self.info.RefdEff2IML
    myalg.RefdEff2DLL = self.info.RefdEff2DLL
    myalg.RefdEff2NSH = self.info.RefdEff2DLL

    if (mc):
      GaudiSequencer("CheckMUONSeq").Members += [ "MuonPIDChecker/"+name+ext ]
    else:
      GaudiSequencer("MoniMUONSeq").Members += [ "MuonPIDChecker/"+name+ext ]

