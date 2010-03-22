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
    
    if (HistosLevel == "Expert"): 
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
      if (UseMC):
	GaudiSequencer("CheckMUONSeq").Members += [ "MuonPIDChecker/"+name+ext+"Down" ]
      else:
	GaudiSequencer("MoniMUONSeq").Members += [ "MuonPIDChecker/"+name+ext+"Down" ]

    else:

      if (HistosLevel == "OfflineFull"): 
	myalg1.HistosOutput = 3 
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
