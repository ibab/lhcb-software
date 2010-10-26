# $Id: StrippingD2hhh_conf.py,v 1.0 2010/07/22 
'''
Module for construction of D->hhh stripping Selections and StrippingLines.

TODO: Cut on more global event variables (ntracks is done, nPVs is to-do) and pion hasRich

On Reco06-Stripping12:
StrippingReport                                                INFO Event 100000, Good event 56518
 |                                    *Decision name*|  *Rate*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
 |_StrippingGlobal_                                  |0.028009|      1583|       |  17.811|       |       |       |
 |_StrippingSequenceStreamTest_                      |0.028009|      1583|       |  17.806|       |       |       |
 |!StrippingD2PPP                                    |0.012421|       702|  1.221|   6.747|      0|      0|    146|
 |!StrippingD2KPP                                    |0.008599|       486|  1.058|   2.793|      0|      0|      0|
 |!StrippingD2KKP                                    |0.004494|       254|  1.134|   4.086|      0|      0|      0|
 |!StrippingD2KPPos                                  |0.003468|       196|  1.179|   1.726|      0|      0|      0|
 |!StrippingD2KKK                                    |0.000690|        39|  1.026|   1.410|      0|      0|      0|
 |!StrippingD2hhh_inc                                |0.002459|       139|  1.568|   0.518|      0|      0|      0|

 - all stripping lines use the same cut variables, although not necessarily the same values.

'''

__author__ = ['Hamish  Gordon']
__date__ = '09/06/2010'
__version__ = '$Revision: 1.0 $'

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from StrippingSelections.Utils import checkConfig

name = "D2hhh"

class StrippingD2hhhConf(LHCbConfigurableUser) :
   """
   Definitions
   """
   __slots__ = {
      'DaugTrkCHI2'     : 5.0       #same as StdLooseDplus cut - don't go looser without loosening StdLooseDplus cut
      , 'DaugPTexc'     : 250.0     # min track PT cut - StdLooseDplus already cuts at PT > 250 so no point going looser than this
      , 'DaugPTinc'     : 250.0     # min track PT cut for inclusive line
      , 'DaugPTpid'     : 250.0     # min track PT cut for KKP and KKK (the 'pid' modes)
      , 'DaugP'         : 2000.0    # same as StdLooseDplus cut - don't go looser without loosening StdLooseDplus cut
      , 'DaugIPCHI2ppp' : 4.0       # track IP chi2 cut for ppp line - StdLooseDplus already cuts at IPchi2 > 4 so no point going looser than this
      , 'DaugIPCHI2'    : 4.0       # track IP chi2 cut - StdLooseDplus already cuts at IPchi2 > 4 so no point going looser than this
      , 'DaugIPCHI2pid' : 2.0       # Should set looser IPChi2 cut for KKK (the 'pid' modes)
      
      , 'KPIDKmin'      : 3.0      # Minimum kaon PIDK (StdTightKaons uses 0.0)
      , 'piPIDKmax'     : 10.0     # Maximum pion PIDK - also need to require hasRich
      
      , 'Daug2IPCHI2'   : 10.0     # Require 2 out of 3 tracks to have IPchi2 greater than this value
      , 'Daug2IPCHI2pid': 4.0      # Should set looser cut for KKK (the 'pid' modes)
      , 'Daug1IPCHI2'   : 10.0     # Require 1 out of 3 tracks to have IPchi2 greater than this value. Currently only used in KKK line
      , 'Daug2PT'       : 250.0    # Require 2 out of 3 tracks to have PT greater than this value
      , 'SumDaugPT'     : 1800.0   # Algebraic (scalar) sum of track PT. 
      , 'DaugDOCA'      : 50       # For KKK and hhh. StdLooseDplus uses 50 
      
      , 'DPT'           : 1000.0   # PT of D+ (StdDplus uses 1000)
      , 'DIPCHI2'       : 15.0     # IPCHI2 of D+
      , 'DFDCHI2'       : 100.0    # FD chi2 of D+ wrt ALL PV
      , 'DDIRA'         : 0.0      # Angle of track to PV
      , 'DVtxCHI2NDOF'  : 10.0      # Vertex chi2/NDOF - StdLooseDplus already cuts at <10 so no point going looser than this
      
      , 'MinMassPreFitinc'  : 1000.0 # For D2hhh only
      , 'MaxMassPreFitinc'  : 2170.0 #
      , 'MinMassPostFitinc' : 1100.0 # For D2hhh only
      , 'MaxMassPostFitinc' : 2070.0 #
      , 'MinMassPreFit'     : 1769.0 # For KKK only
      , 'MaxMassPreFit'     : 2170.0 #
      , 'MinMassPostFit'    : 1800.0 # For all exclusive modes using StdDplus
      , 'MaxMassPostFit'    : 1940.0 # For KPP mode only
      , 'MinMassPostFitDs'  : 1940.0 # not used now
      , 'MaxMassPostFitDs'  : 2040.0 # For all exclusive modes using StdDplus, except to KPP

      , 'MaxTracksInEvent'  : 350.0  # this is tight enough to have a significant effect on the rate without killing lines altogether
      , 'CutOnTracksInEvent': True
      
      , 'prescale_PPP'   : 1.0
      , 'prescale_KPP'   : 1.0
      , 'prescale_KPPos' : 0.4
      , 'prescale_KKP'   : 1.0
      , 'prescale_KKK'   : 1.0
      , 'prescale_inc'   : 0.04
      }
   

   #define exclusive stripping lines, all use PID where possible, including PPP
   def stripD2PPP(self) :
      algos = [] 
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.getD2PPP())
      lineD2PPP = StrippingLine('D2PPP', prescale = "%(prescale_PPP)s" % self.getProps(), algos = algos)
      return lineD2PPP
   def stripD2KPP(self) :
      algos = [] 
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.getD2KPP())
      lineD2KPP = StrippingLine('D2KPP', prescale = "%(prescale_KPP)s" % self.getProps(), algos = algos)
      return lineD2KPP
   # built with the same cuts as other exclusive modes 
   def stripD2KKP(self) :
      algos = [] 
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.getD2KKP())
      lineD2KKP = StrippingLine('D2KKP', prescale = "%(prescale_KKP)s" % self.getProps(), algos = algos)
      return lineD2KKP
   def stripD2KPPos(self) :
      algos = [] 
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.getD2KPPos())
      lineD2KPPos = StrippingLine('D2KPPos', prescale = "%(prescale_KPPos)s" % self.getProps(), algos = algos)
      return lineD2KPPos
   def stripD2KKK(self) :
      algos = []
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.makeD2KKK())
      lineD2KKK = StrippingLine('D2KKK', prescale = "%(prescale_KKK)s" % self.getProps(), algos = algos)
      return lineD2KKK
     
   #define inclusive stripping line, which uses no PID
   def stripD2hhh_inc(self) :
      algos = []
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.makeD2hhhinc())
      lineD2hhh_inc = StrippingLine('D2hhh_inc', prescale = "%(prescale_inc)s" % self.getProps(), algos = algos)
      return lineD2hhh_inc

   # Get D2PPP from StdLooseDplus
   def getD2PPP(self):
      StdDplusPPP = DataOnDemand(Location = "Phys/StdLooseDplus2PiPiPi")
      str_cutsDaug_Pi =  "(MAXTREE('pi+'==ABSID, PIDK-PIDpi) < %(piPIDKmax)s ) & (MINTREE('pi+'==ABSID, MIPCHI2DV(PRIMARY)) > %(DaugIPCHI2ppp)s ) & (MAXTREE('pi+'==ABSID,TRCHI2DOF) < %(DaugTrkCHI2)s) & (MINTREE('pi+'==ABSID, P ) > %(DaugP)s) & (MINTREE('pi+'==ABSID, PT) > %(DaugPTexc)s )" % self.getProps()
      str_cutsComb_PPP =" (2 <= NINGENERATION((PT > %(Daug2PT)s * MeV ) , 1)) & (2 <= NINGENERATION((MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) , 1)) & (SUMTREE( ISBASIC , PT ) > %(SumDaugPT)s*MeV) "% self.getProps()
      str_cutsMoth_PPP = "(BPVDIRA > %(DDIRA)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVIPCHI2() < %(DIPCHI2)s)" % self.getProps()
      str_cutsMassPostFit_pid = "(M > %(MinMassPostFit)s) & (M < %(MaxMassPostFitDs)s)" % self.getProps()
      filterPPP = FilterDesktop('filterPPP', Code = '('+str_cutsDaug_Pi+'&'+str_cutsMoth_PPP+'&'+str_cutsComb_PPP+'&'+ str_cutsMassPostFit_pid+')') 
      selD2PPP = Selection("selD2PPP", Algorithm = filterPPP, RequiredSelections = [StdDplusPPP] )
      return selD2PPP

   # Get D2KPP from StdLooseDplus
   def getD2KPP(self):
      StdDplusKPP = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi")
      str_cutsDaug_K =  "(MINTREE('K-'==ABSID, PIDK-PIDpi) > %(KPIDKmin)s ) & (MINTREE('K-'==ABSID, MIPCHI2DV(PRIMARY)) > %(DaugIPCHI2)s ) & (MAXTREE('K-'==ABSID,TRCHI2DOF) < %(DaugTrkCHI2)s) & (MINTREE('K-'==ABSID, P ) > %(DaugP)s) & (MINTREE('K-'==ABSID, PT) > %(DaugPTexc)s )" % self.getProps()
      str_cutsDaug_Pi =  "(MAXTREE('pi+'==ABSID, PIDK-PIDpi) < %(piPIDKmax)s ) &  (MINTREE('pi+'==ABSID, MIPCHI2DV(PRIMARY)) > %(DaugIPCHI2)s ) & (MAXTREE('pi+'==ABSID,TRCHI2DOF) < %(DaugTrkCHI2)s) & (MINTREE('pi+'==ABSID, P ) > %(DaugP)s) & (MINTREE('pi+'==ABSID, PT) > %(DaugPTexc)s )" % self.getProps()
      str_cutsComb_KPP =" (2 <= NINGENERATION((PT > %(Daug2PT)s * MeV ) , 1)) & (2 <= NINGENERATION((MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) , 1)) & (SUMTREE( ISBASIC , PT ) > %(SumDaugPT)s*MeV) "% self.getProps()
      str_cutsMoth_KPP = "(BPVDIRA > %(DDIRA)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVIPCHI2() < %(DIPCHI2)s)" % self.getProps()
      str_cutsMassPostFit_pid = "(M > %(MinMassPostFit)s) & (M < %(MaxMassPostFit)s)" % self.getProps()
      filterKPP = FilterDesktop('filterKPP', Code ='('+str_cutsDaug_K+'&'+str_cutsDaug_Pi+'&'+str_cutsMoth_KPP+'&'+str_cutsComb_KPP+'&'+ str_cutsMassPostFit_pid+')') 
      
      selD2KPP = Selection("selD2KPP", Algorithm = filterKPP, RequiredSelections = [StdDplusKPP] )
      return selD2KPP

   # Get D2KKP from StdLooseDplus
   def getD2KKP(self):
      StdDplusKKP = DataOnDemand(Location = "Phys/StdLooseDplus2KKPi")
      str_cutsDaug_Pi =  "(MAXTREE('pi+'==ABSID, PIDK-PIDpi) < %(piPIDKmax)s ) &  (MINTREE('pi+'==ABSID, MIPCHI2DV(PRIMARY)) > %(DaugIPCHI2)s ) & (MAXTREE('pi+'==ABSID,TRCHI2DOF) < %(DaugTrkCHI2)s) & (MINTREE('pi+'==ABSID, P ) > %(DaugP)s) & (MINTREE('pi+'==ABSID, PT) > %(DaugPTexc)s )" % self.getProps()
      str_cutsDaug_K =  "(MINTREE('K-'==ABSID, PIDK-PIDpi) > %(KPIDKmin)s ) & (MINTREE('K-'==ABSID, MIPCHI2DV(PRIMARY)) > %(DaugIPCHI2)s ) & (MAXTREE('K-'==ABSID,TRCHI2DOF) < %(DaugTrkCHI2)s) & (MINTREE('K-'==ABSID, P ) > %(DaugP)s) & (MINTREE('K-'==ABSID, PT) > %(DaugPTexc)s )" % self.getProps()
      str_cutsComb_KKP =" (2 <= NINGENERATION((PT > %(Daug2PT)s * MeV ) , 1)) & (2 <= NINGENERATION((MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) , 1)) & (SUMTREE( ISBASIC , PT ) > %(SumDaugPT)s*MeV) "% self.getProps()
      str_cutsMoth_KKP = "(BPVDIRA > %(DDIRA)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVIPCHI2() < %(DIPCHI2)s) " % self.getProps()
      str_cutsMassPostFit_pid = "(M > %(MinMassPostFit)s) & (M < %(MaxMassPostFitDs)s)" % self.getProps()
      filterKKP = FilterDesktop('filterKKP', Code ='('+str_cutsDaug_K+'&'+str_cutsDaug_Pi+'&'+str_cutsMoth_KKP+'&'+str_cutsComb_KKP+'&'+ str_cutsMassPostFit_pid+')') 
      selD2KKP = Selection("selD2KKP", Algorithm = filterKKP, RequiredSelections = [StdDplusKKP] )
      return selD2KKP

   # Get opposite-sign D2KPP from StdLooseDplus
   def getD2KPPos(self):
      StdDplusKPPos = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPiOppSignPi")
      str_cutsDaug_K =  "(MINTREE('K+'==ABSID, PIDK-PIDpi) > %(KPIDKmin)s ) &  (MINTREE('K+'==ABSID, MIPCHI2DV(PRIMARY)) > %(DaugIPCHI2)s ) &(MAXTREE('K+'==ABSID,TRCHI2DOF) < %(DaugTrkCHI2)s) & (MINTREE('K+'==ABSID, P ) > %(DaugP)s) & (MINTREE('K-'==ABSID, PT) > %(DaugPTexc)s )" % self.getProps()
      str_cutsDaug_Pi =  "(MAXTREE('pi+'==ABSID, PIDK-PIDpi) < %(piPIDKmax)s ) & (MINTREE('K-'==ABSID, MIPCHI2DV(PRIMARY)) > %(DaugIPCHI2)s ) & (MAXTREE('pi+'==ABSID,TRCHI2DOF) < %(DaugTrkCHI2)s) & (MINTREE('pi+'==ABSID, P ) > %(DaugP)s) & (MINTREE('pi+'==ABSID, PT) > %(DaugPTexc)s )" % self.getProps()
      str_cutsComb_KPP =" (2 <= NINGENERATION((PT > %(Daug2PT)s * MeV ) , 1)) & (2 <= NINGENERATION((MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) , 1)) & (SUMTREE( ISBASIC , PT ) > %(SumDaugPT)s*MeV)"% self.getProps()
      str_cutsMoth_KPP = "(BPVDIRA > %(DDIRA)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVIPCHI2() < %(DIPCHI2)s) " % self.getProps()
      str_cutsMassPostFit_pid = "(M > %(MinMassPostFit)s) & (M < %(MaxMassPostFitDs)s)" % self.getProps()
      filterKPP = FilterDesktop('filterKPP', Code = '('+str_cutsDaug_K+'&'+str_cutsDaug_Pi+'&'+str_cutsMoth_KPP+'&'+str_cutsComb_KPP+'&'+ str_cutsMassPostFit_pid+')') 
      selD2KPPos = Selection("selD2KPPos", Algorithm = filterKPP, RequiredSelections = [StdDplusKPPos] )
      return selD2KPPos

   #make new particles for KKK(with PID cuts on tracks)
   def makeD2KKK(self):
      StdTightK = DataOnDemand(Location = "Phys/StdTightKaons")
      str_cutsDaug_pid = "( (MIPCHI2DV(PRIMARY) > %(DaugIPCHI2pid)s) & (PT > %(DaugPTpid)s *MeV) & (P > %(DaugP)s *MeV) & (TRCHI2DOF < %(DaugTrkCHI2)s) )" % self.getProps()
      str_cutsDaug_pidK = "(PIDK-PIDpi > %(KPIDKmin)s )"  % self.getProps()
      str_cutsComb_pid = "(ADOCACHI2CUT( %(DaugDOCA)s , '' )) & (ANUM(PT > %(Daug2PT)s ) >= 2) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(SumDaugPT)s*MeV) & (ANUM(MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2pid)s ) >= 2) & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Daug1IPCHI2)s))" % self.getProps()
      str_cutsMassPreFit_pid = "(AM > %(MinMassPreFit)s) & (AM < %(MaxMassPreFit)s)" % self.getProps()
      str_cutsMassPostFit_pid = "(M > %(MinMassPostFit)s) & (M < %(MaxMassPostFitDs)s)" % self.getProps()
      str_cutsMoth_pid = "(BPVDIRA > %(DDIRA)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVIPCHI2() < %(DIPCHI2)s) " % self.getProps()
      cbppid = CombineParticles("CombineD2KKK"
                                , DecayDescriptor =  '[D+ -> K- K+ K+]cc'
                                , DaughtersCuts = { "K+" : '(' + str_cutsDaug_pidK +'&' + str_cutsDaug_pid +  ')' }
                                , CombinationCut = '(' + str_cutsComb_pid +'&' + str_cutsMassPreFit_pid + ')'
                                , MotherCut = '(' + str_cutsMoth_pid + '&' + str_cutsMassPostFit_pid + ')'
                                 )
      selD2KKK = Selection("selD2KKK", Algorithm = cbppid, RequiredSelections = [StdTightK] )
      return selD2KKK
     
   def makeD2hhhinc(self):
      StdNoPIDsPi = DataOnDemand(Location = "Phys/StdNoPIDsPions")
      str_cutsDaug_inc ="((MIPCHI2DV(PRIMARY) > %(DaugIPCHI2)s) & (PT > %(DaugPTinc)s *MeV) &  (P > %(DaugP)s *MeV) &  (TRCHI2DOF < %(DaugTrkCHI2)s) )"  % self.getProps()
      str_cutsComb_inc = " (ADOCACHI2CUT( %(DaugDOCA)s , '' )) & (ANUM(PT > %(Daug2PT)s ) >= 2) & (ANUM(MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) >= 2) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(SumDaugPT)s*MeV)" % self.getProps()
      str_cutsMassPreFit_inc = "(AM > %(MinMassPreFitinc)s) & (AM < %(MaxMassPreFitinc)s)" % self.getProps()
      str_cutsMassPostFit_inc = "(M > %(MinMassPostFitinc)s) & (M < %(MaxMassPostFitinc)s)" % self.getProps()
      str_cutsMoth_inc = "(BPVDIRA > %(DDIRA)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVIPCHI2() < %(DIPCHI2)s) " % self.getProps()
      cbpinc = CombineParticles("CombineD2hhh_inc"
                                , DecayDescriptor =  '[D+ -> pi- pi+ pi+]cc'
                                , DaughtersCuts = { "pi+" : str_cutsDaug_inc }
                                , CombinationCut = '(' + str_cutsComb_inc +'&' + str_cutsMassPreFit_inc +')'
                                , MotherCut = '(' + str_cutsMoth_inc + '&' + str_cutsMassPostFit_inc + ')'
                                )
      selD2hhhinc = Selection("selD2hhhinc", Algorithm = cbpinc, RequiredSelections = [StdNoPIDsPi] )
      return selD2hhhinc

   def _filterNTracksInEvent(self):
      '''
      Returns a VoidFilter instance to filter on the maximum number of tracks in an event (stolen from Philip Hunt)
      '''
      if not self.getProp("CutOnTracksInEvent"):
         return None
      from Configurables import LoKi__VoidFilter as VoidFilter
      from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
      modules = CoreFactory('CoreFactory').Modules
      for i in ['LoKiTrigger.decorators']:
         if i not in modules : modules.append(i)

      filterTooManyIP = VoidFilter(
         'FilterNTracks_forD2hhh'
         ,Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < %(MaxTracksInEvent)s )" %self.getProps()
         )
      return filterTooManyIP
      
    
   #utility function
   def getProps(self) :
      """
      From HltLinesConfigurableUser
      @todo Should be shared between Hlt and stripping
      """
      d = dict()
      for (k,v) in self.getDefaultProperties().iteritems() :
         d[k] = getattr(self,k) if hasattr(self,k) else v
      return d

