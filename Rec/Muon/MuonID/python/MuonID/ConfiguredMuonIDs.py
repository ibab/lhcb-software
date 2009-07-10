"""
 =============================================================
 Class to configure MuonID tools and algos. To import do
 'from MuonID import ConfiguredMuonIDs'
 author -> Xabier Cid Vidal (xabier.cid.vidal@cern.ch)
 =============================================================
"""

import os

from Gaudi.Configuration import *

from Configurables import MuonIDAlg,Chi2MuIDTool,DistMuIDTool
from Configurables import TrackMasterFitter,IsMuonCandidateC,MakeMuonMeasurements, CLTool, GetArrival
from Configurables import GaudiSequencer


class ConfiguredMuonIDs():

  def __init__(self, data="DC06",version="def",debug=False):
    """
    initialization for the class. Use as input data type (DC06,MC08,etc) and version of it if necessary.
    """
    self.debug=debug
    if self.debug: print "# INITIALIZING CONFIGUREDMUONIDs"

    ## from datatype and version look for module with data. Store extra modules in case desired does not exist
    mod=[data+"_"+version,data+"_def","DC06_def"]
    mod = map(lambda x: "Muon_"+x,mod)

    PYMUONIDROOT = os.environ["MUONIDROOT"]+"/python/MuonID/"
    pname=[]
    for m in mod: pname.append(PYMUONIDROOT+m+".py")

    if self.debug: print "# \tpname -> ",pname

    ## check if modules exist and load them
    if os.path.exists(pname[0]): exec("from MuonID import "+mod[0]+" as info")
    else:
      if debug: print "# WARNING: not available info for DATA=%s,VERSION=%s. Loading default" %(data,version)
      if os.path.exists(pname[1]): exec("from MuonID import "+mod[1]+" as info")
      else:  exec("from MuonID import "+mod[2]+" as info")

    ## set final module with info to be laoded
    self.info=info
    self.info.DEBUG = debug
    self.initializeAll = True

  def configureFitter(self,fitter):
    """
    configure fitter to be used inside MuonID chi2 calculation
    """

    if self.debug: print "# CONFIGURING FITTER"
    ## check if input is already an instance or this must be created
    if isinstance(fitter,TrackMasterFitter): myfitter=fitter
    else: myfitter=TrackMasterFitter(str(fitter))
    
    myfitter.UseSeedStateErrors = self.info.UseSeedStateErrors
    myfitter.FitUpstream = self.info.FitUpstream
    myfitter.MaxNumberOutliers = self.info.MaxNumberOutliers
    myfitter.NumberFitIterations = self.info.NumberFitIterations
    myfitter.ZPositions = self.info.ZPositions
    myfitter.StateAtBeamLine = self.info.StateAtBeamLine


  def configureIsMuonCandidateC(self, ismc):
    """
    configure ismuoncandidatec tool. select ismuon type
    """
    if self.debug: print "# CONFIGURING ISMUONCANDIDATEC"

    ## check if input is already an instance or this must be created
    if isinstance(ismc,IsMuonCandidateC): myismc=ismc
    else: myismc=CLTool(str(ismc))

    myismc.MomRangeIsMuon = self.info.MomRangeIsMuon
    myismc.IsMuonOpt = self.info.IsMuonOpt
    myismc.MinHits = self.info.MinHits


  def configureGetArrival(self,geta):
    """
    configure getarrival tool. set histograms for arrival prob and clarr
    """
    if self.debug: print "# CONFIGURING GETARRIVAL"
    ## check if input is already an instance or this must be created
    if isinstance(geta,GetArrival): mygeta=geta
    else: mygeta=GetArrival(str(geta))

    mygeta.moms = self.info.gamoms
    mygeta.probs = self.info.gaprobs
    mygeta.useFunct = self.info.useFunct
    mygeta.alpha = self.info.alpha
    mygeta.beta = self.info.beta
    mygeta.eff = self.info.eff
    mygeta.MinHits = self.info.MinHits


  def configureCLTool(self, cltool, use_dist=False):
    """
    configure cltool used to get dll from chi2 or distance.
    Use of chi2 or dist histos as input.
    """

    if self.debug: print "# CONFIGURING CLTOOL"
    ## check if input is already an instance or this must be created
    if isinstance(cltool,CLTool): mycltool=cltool
    else: mycltool=CLTool(str(cltool))

    mycltool.Range = self.info.Range
    mycltool.RangeNmuons = self.info.RangeNmuons

    mycltool.LeftRight = self.info.LeftRight
    mycltool.Overlap = self.info.Overlap
    mycltool.lbinCenter = self.info.lbinCenter
    mycltool.lbinCenterNmuons = self.info.lbinCenterNmuons
    mycltool.applyOvlapLast = self.info.applyOvlapLast

    ## depending on use_dist or no load histograms from dist or chi2
    if use_dist:
      if self.debug: print "# \tCONFIGURING CLTOOL: use_dist=True"
      mycltool.Signal = self.info.distSignal
      mycltool.Bkg = self.info.distBkg
    else:
      if self.debug: print "# \tCONFIGURING CLTOOL: use_dist=False"
      mycltool.Signal = self.info.chi2Signal
      mycltool.Bkg = self.info.chi2Bkg

    

  def configureMuIDTool(self,muidtool,use_dist=False):
    """
    configure either chiMuIDtool or DistMuIDTool with all subdependent tools.
    If one of the two instances is input, ignore use_dist.
    If not, use_dist will either set chi2 or dist tools.
    """

    if self.debug: print "# CONFIGURING MUIDTOOL"

    ## check if input is already an instance or this must be created.
    ## also set use_dist1, to be used when configuring CLQuality tool
    use_dist1 = use_dist
    mymuidtool = muidtool
    
    if isinstance(muidtool,Chi2MuIDTool): use_dist1 = False
    elif isinstance(muidtool,DistMuIDTool): use_dist1 = True
    else:
      if use_dist: mymuidtool=DistMuIDTool(str(muidtool))
      else: mymuidtool=Chi2MuIDTool(str(muidtool))

    if self.debug: print "# \tCONFIGURING MUIDTOOL: use_dist1 = ",use_dist1

    ## general muid tool options
    mymuidtool.NSigmas = self.info.NSigmas
    mymuidtool.DiscrValue = self.info.DiscrValue
    mymuidtool.UseBkg = self.info.UseBkg
    mymuidtool.MinMomSt3 = self.info.MinMomSt3
    mymuidtool.MinMomSt4 = self.info.MinMomSt4

    ## add and configure fitter to be used for finding chi2
    if self.debug: print "# \tCONFIGURING MUIDTOOL: adding tools"
    mymuidtool.addTool(TrackMasterFitter(), name='fitter') 
    self.configureFitter(mymuidtool.fitter)

    ## for MuonIDAlg, can stop here (no use or extra subtools)
    if self.initializeAll:
      if self.debug: print "# \tCONFIGURING MUIDTOOL: initializeAll=True"
      ## add and configure ismuoncandidatec tool
      mymuidtool.addTool(IsMuonCandidateC(), name='IsMuonCandidateC')
      self.configureIsMuonCandidateC(mymuidtool.IsMuonCandidateC)

      ## add and configure getarrival tool
      mymuidtool.addTool(GetArrival(), name='GetArrival')
      self.configureGetArrival(mymuidtool.GetArrival)

      ## add and configure CLQuality. use_dist1 will set either chi2 or dist histos
      mymuidtool.addTool(CLTool(), name='CLQuality')
      self.configureCLTool(mymuidtool.CLQuality,use_dist1)

    

  def configureMuonIDAlg (self,muonid,use_dist=False):
    """
    general configuration of MuonIDAlg. Equivalent to old MuonID.py
    Also configures muonIDtool used as extra_info. This can either be chi2 or dist.
    Default chi2.
    """

    if self.debug: print "# CONFIGURING MUONIDALG"

    ## check if input is already an instance or this must be created
    if isinstance(muonid,MuonIDAlg): mymuid=muonid
    else: mymuid=MuonIDAlg(str(muonid))

    ## general MuonIDAlg properties
    mymuid.MomentumCuts  = self.info.MomentumCuts
    mymuid.AllMuonTracks = self.info.AllMuonTracks
    mymuid.FindQuality = self.info.FindQuality
    
    mymuid.XFOIParameter1 = self.info.XFOIParameter1
    mymuid.XFOIParameter2 = self.info.XFOIParameter2
    mymuid.XFOIParameter3 = self.info.XFOIParameter3
    mymuid.YFOIParameter1 = self.info.YFOIParameter1
    mymuid.YFOIParameter2 = self.info.YFOIParameter2
    mymuid.YFOIParameter3 = self.info.YFOIParameter3
    mymuid.FOIfactor = self.info.FOIfactor
      
    mymuid.distMuon= self.info.distMuon
    mymuid.distPion= self.info.distPion

    ## GL & SF:
    if "DLL_flag" in dir(self.info): mymuid.DLL_flag= self.info.DLL_flag
    if "Weight_flag" in dir(self.info): mymuid.Weight_flag= self.info.Weight_flag
    if "step" in dir(self.info): mymuid.step=self.info.step
    if "nMax_bin" in dir(self.info): mymuid.nMax_bin=self.info.nMax_bin

    #Landau's paramateres:
    #Muons - Region1:
    if "MuLandauParameterR1_1" in dir(self.info): mymuid.MuLandauParameterR1_1= self.info.MuLandauParameterR1_1
    if "MuLandauParameterR1_2" in dir(self.info): mymuid.MuLandauParameterR1_2= self.info.MuLandauParameterR1_2
    if "MuLandauParameterR1_3" in dir(self.info): mymuid.MuLandauParameterR1_3= self.info.MuLandauParameterR1_3
    if "MuLandauParameterR1_4" in dir(self.info): mymuid.MuLandauParameterR1_4= self.info.MuLandauParameterR1_4
    if "MuLandauParameterR1_5" in dir(self.info): mymuid.MuLandauParameterR1_5= self.info.MuLandauParameterR1_5
    if "MuLandauParameterR1_6" in dir(self.info): mymuid.MuLandauParameterR1_6= self.info.MuLandauParameterR1_6
    if "MuLandauParameterR1_7" in dir(self.info): mymuid.MuLandauParameterR1_7= self.info.MuLandauParameterR1_7
    #Muons - Region2:
    if "MuLandauParameterR2_1" in dir(self.info): mymuid.MuLandauParameterR2_1= self.info.MuLandauParameterR2_1
    if "MuLandauParameterR2_2" in dir(self.info): mymuid.MuLandauParameterR2_2= self.info.MuLandauParameterR2_2
    if "MuLandauParameterR2_3" in dir(self.info): mymuid.MuLandauParameterR2_3= self.info.MuLandauParameterR2_3
    if "MuLandauParameterR2_4" in dir(self.info): mymuid.MuLandauParameterR2_4= self.info.MuLandauParameterR2_4
    if "MuLandauParameterR2_5" in dir(self.info): mymuid.MuLandauParameterR2_5= self.info.MuLandauParameterR2_5
    #Muons - Region3:
    if "MuLandauParameterR3_1" in dir(self.info): mymuid.MuLandauParameterR3_1= self.info.MuLandauParameterR3_1
    if "MuLandauParameterR3_2" in dir(self.info): mymuid.MuLandauParameterR3_2= self.info.MuLandauParameterR3_2
    if "MuLandauParameterR3_3" in dir(self.info): mymuid.MuLandauParameterR3_3= self.info.MuLandauParameterR3_3
    if "MuLandauParameterR3_4" in dir(self.info): mymuid.MuLandauParameterR3_4= self.info.MuLandauParameterR3_4
    if "MuLandauParameterR3_5" in dir(self.info): mymuid.MuLandauParameterR3_5= self.info.MuLandauParameterR3_5
    #Muons - Region4:
    if "MuLandauParameterR4_1" in dir(self.info): mymuid.MuLandauParameterR4_1= self.info.MuLandauParameterR4_1
    if "MuLandauParameterR4_2" in dir(self.info): mymuid.MuLandauParameterR4_2= self.info.MuLandauParameterR4_2
    if "MuLandauParameterR4_3" in dir(self.info): mymuid.MuLandauParameterR4_3= self.info.MuLandauParameterR4_3
    if "MuLandauParameterR4_4" in dir(self.info): mymuid.MuLandauParameterR4_4= self.info.MuLandauParameterR4_4
    if "MuLandauParameterR4_5" in dir(self.info): mymuid.MuLandauParameterR4_5= self.info.MuLandauParameterR4_5

    #Non-Muons - Region1-2-3-4:
    if "NonMuLandauParameterR1" in dir(self.info): mymuid.NonMuLandauParameterR1= self.info.NonMuLandauParameterR1
    if "NonMuLandauParameterR2" in dir(self.info): mymuid.NonMuLandauParameterR2= self.info.NonMuLandauParameterR2
    if "NonMuLandauParameterR3" in dir(self.info): mymuid.NonMuLandauParameterR3= self.info.NonMuLandauParameterR3    
    if "NonMuLandauParameterR4" in dir(self.info): mymuid.NonMuLandauParameterR4= self.info.NonMuLandauParameterR4

    # Binning for the Distance for muons:
    if "nMupBinsR1" in dir(self.info): mymuid.nMupBinsR1= self.info.nMupBinsR1
    if "nMupBinsR2" in dir(self.info): mymuid.nMupBinsR2= self.info.nMupBinsR2
    if "nMupBinsR3" in dir(self.info): mymuid.nMupBinsR3= self.info.nMupBinsR3
    if "nMupBinsR4" in dir(self.info): mymuid.nMupBinsR4= self.info.nMupBinsR4

    if "MupBinsR1" in dir(self.info): mymuid.MupBinsR1= self.info.MupBinsR1
    if "MupBinsR2" in dir(self.info): mymuid.MupBinsR2= self.info.MupBinsR2
    if "MupBinsR3" in dir(self.info): mymuid.MupBinsR3= self.info.MupBinsR3
    if "MupBinsR4" in dir(self.info): mymuid.MupBinsR4= self.info.MupBinsR4
    
    
    ## add and configure either DistMuIDTool or Chi2MuIDTool.
    ## no need to initialize all sub tools there
    self.initializeAll = False
    if use_dist: mymuid.addTool(DistMuIDTool(), name='myMuIDTool')
    else: mymuid.addTool(Chi2MuIDTool(), name='myMuIDTool')
  
    self.configureMuIDTool(mymuid.myMuIDTool,use_dist)
    return mymuid

  #def applyConf(self):
  def getMuonIDSeq(self):
    """
    general method for MuonIDAlg configuration.
    Creates MuonIDAlg instance and configures it.
    Finally puts it in gaudi sequencer.
    """

    if self.debug: print "# APPLYING GENERAL MUONID CONFIGURATION"
    ## create output gaudi sequencer
    myg = GaudiSequencer("MuonIDSeq")
    ## create and configure MuonIDAlg instance
    muid = MuonIDAlg()
    self.configureMuonIDAlg(muid)
    ## add to gaudi sequencer and return
    myg.Members.append(muid)
    return myg
