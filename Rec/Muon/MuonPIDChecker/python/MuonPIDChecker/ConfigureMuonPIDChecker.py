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

    import importlib
    info = None
    for m in mod:
      try:
        info = importlib.import_module("MuonPIDChecker." + m )
      except ImportError:
        continue

    ## set final module with info to be laoded
    self.info=info
    self.info.DEBUG = debug
    self.initializeAll = True

  def configure(self,name="MuonPID",UseMC=True,HistosLevel="OfflineFull" ):
    """
    configure the MuonPIDchecker algorithm
    """

    if self.debug: print "CONFIGURING MUONPIDCHECKER"


    ## check if input is already an instance or this must be created
    ext = "Checker"
    if (not UseMC): ext = "Monitor"

    myalg1 = MuonPIDChecker(name+ext+"Long")
    myalg1.HistoTopDir = "Muon/"
    myalg1.RunningMC = UseMC
    myalg1.TrackType = 0
    myalg1.MonitorCutValues= self.info.MonitorCutValues
    myalg1.HistoDir = name+"/"+ext+"Long"

    if (HistosLevel == "Expert" or HistosLevel == "OfflineFull"):
      myalg1.HistosOutput = 4
      # Configure and additional instance to look at Downstream tracks
      myalg2 = MuonPIDChecker(name+ext+"Down")
      myalg2.HistoTopDir = "Muon/"
      myalg2.RunningMC = UseMC
      myalg2.HistosOutput = 1
      myalg2.TrackType = 1
      myalg2.MonitorCutValues= self.info.MonitorCutValues
      myalg2.HistoDir = name+"/"+ext+"Down"
      myalg2.HistosOutput = 4
      if (HistosLevel == "OfflineFull"):
    myalg1.HistosOutput = 3
    myalg2.HistosOutput = 3
      if (UseMC):
    GaudiSequencer("CheckMUONSeq").Members += [ "MuonPIDChecker/"+name+ext+"Down" ]
      else:
    GaudiSequencer("MoniMUONSeq").Members += [ "MuonPIDChecker/"+name+ext+"Down" ]

    else:

      if (HistosLevel == "OfflineExpress"):
    myalg1.HistosOutput = 2
      if (HistosLevel == "Online"):
    myalg1.HistosOutput = 1
      if (HistosLevel == "None"):
    myalg1.HistosOutput = 0

    if (UseMC):
      GaudiSequencer("CheckMUONSeq").Members += [ "MuonPIDChecker/"+name+ext+"Long" ]
    else:
      GaudiSequencer("MoniMUONSeq").Members += [ "MuonPIDChecker/"+name+ext+"Long" ]
