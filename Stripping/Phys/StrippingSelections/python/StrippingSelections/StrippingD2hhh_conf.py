# $Id: StrippingD2hhh_conf.py,v 1.0 2010/09/06 
'''
Module for construction of D->hhh stripping Selections and StrippingLines.

TODO: maybe need different stripping lines for signal and background mass windows
TODO: Cut on more global event variables (ntracks is done, nPVs is to-do) and pion hasRich

On RecoStripping08:
mean CPU (INC)
Retention rate

 - modified by Erica to use CombineParticles. In the next release we should use stdDplus
 - In this release the same cuts are being used
 - all stripping lines use the same cut variables, apart from KKK (although not necessarily the same values)
 - I have tried to include all cut variables that could possibly be useful in all stripping lines where they can be used. Their configurable values can be set to zero if we decide not to cut on this variable, but it should make the lines more flexible. Can remove if there are CPU time issues

'''

__author__ = ['Hamish  Gordon']
__date__ = '06/09/2010'
__version__ = '$Revision: 1.3 $'

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
      'DaugTrkCHI2'     : 5.0       #StdLooseDplus cut at 10 
      , 'DaugPTexc'     : 250.0     # min track PT cut for lines using StdLooseDplus - StdLooseDplus already cuts at PT > 250 so no point going looser than this
      , 'DaugPTinc'     : 250.0     # min track PT cut for inclusive line
      , 'DaugPTpid'     : 250.0     # min track PT cut for KKP and KKK (the 'pid' modes)
      , 'DaugP'         : 2000.0    #same as StdLooseDplus cut - don't go looser without loosening StdLooseDplus cut
      , 'DaugIPCHI2exc' : 4.0       # track IP chi2 cut for lines using StdLooseDplus - StdLooseDplus already cuts at IPchi2 > 4 so no point going looser than this
      , 'DaugIPCHI2inc' : 4.0       # track IP chi2 cut for inclusive line
      , 'DaugIPCHI2pid' : 0.0       # Should set looser IPChi2 cut for KKP and KKK (the 'pid' modes)
      
      , 'KPIDKmin'      : 3.0      # Minimum kaon PIDK (StdTightKaons uses 0.0)
      , 'piPIDKmax'     : 10.0     # Maximum pion PIDK - also need to require hasRich
      
      , 'Daug2IPCHI2'   : 4.0      # Require 2 out of 3 tracks to have IPchi2 greater than this value
      , 'Daug2IPCHI2pid': 4.0      # Should set looser cut for KKP and KKK (the 'pid' modes)
      , 'Daug1IPCHI2'   : 4.0      # Require 1 out of 3 tracks to have IPchi2 greater than this value. Currently only used in KKP and KKK lines
      , 'Daug2PT'       : 250.0      # Require 2 out of 3 tracks to have PT greater than this value
      , 'SumDaugPT'     : 2000.0   # Algebraic (scalar) sum of track PT. StdLooseDplus cuts at 1000 on APT as CombinationCut - I think the cut variable (sum (daughter PT) is equivalent
      , 'DaugDOCA'      : 0.20     # I don't know how to implement DOCA in FilterDesktop, so we don't have this cut in lines that use StdLooseDplus
      
      , 'DPT'           : 1000.0   # PT of D+
      , 'DFDCHI2'       : 120.0     # FD chi2 of D+ ,used also for dcaSVPV 
      , 'POINT'         : 0.12     # Pointing variable
      , 'DDIRA'         : 0.9999   # Angle of track to PV
      , 'DVtxCHI2NDOF'  : 7.0     # Vertex chi2/NDOF
      
      , 'MinMassPreFitinc'  : 1000.0
      , 'MinMassPostFitinc' : 1100.0
      , 'MinMassPreFitpid'  : 1769.0 #same as StdLooseDplus cut - but can go looser without loosening StdLooseDplus cut
      , 'MinMassPostFitexc' : 1800.0 #same as StdLooseDplus cut - don't go looser without loosening StdLooseDplus cut
      , 'MaxMassPreFit'     : 2170.0 #same as StdLooseDplus cut - don't go looser without loosening StdLooseDplus cut
      , 'MaxMassPostFitkpp' : 1940.0 #same as StdLooseDplus cut - don't go looser without loosening StdLooseDplus cut
      , 'MaxMassPostFit'    : 2040.0 #same as StdLooseDplus cut - don't go looser without loosening StdLooseDplus cut
      , 'MaxMassPostFitinc' : 2070.0 #same as StdLooseDplus cut - don't go looser without loosening StdLooseDplus cut

      , 'MaxTracksInEvent'  : 250.0 # this is tight enough to have a significant effect on the rate without killing lines altogether
      , 'CutOnTracksInEvent': True
      
      , 'prescale_PPP'   : 0.6
      , 'prescale_KPP'   : 0.6
      , 'prescale_KPPos' : 0.6
      , 'prescale_KKP'   : 0.6
      , 'prescale_KKK'   : 0.6
      , 'prescale_inc'   : 0.05
      }
   

   #de exclusive stripping lines, all use PID where possible, including PPP
   def stripD2PPP(self) :
      algos = [] 
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.makeD2PPP())
      lineD2PPP = StrippingLine('D2PPP', prescale = "%(prescale_PPP)s" % self.getProps(), algos = algos)
      return lineD2PPP
   def stripD2KPP(self) :
      algos = [] 
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.makeD2KPP())
      lineD2KPP = StrippingLine('D2KPP', prescale = "%(prescale_KPP)s" % self.getProps(), algos = algos)
      return lineD2KPP
   def stripD2KKP(self) :
      algos = [] 
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.makeD2KKP())
      lineD2KKP = StrippingLine('D2KKP', prescale = "%(prescale_KKP)s" % self.getProps(), algos = algos)
      return lineD2KKP
   def stripD2KPPos(self) :
      algos = [] 
      if self.getProp("CutOnTracksInEvent"):
         algos.append(self._filterNTracksInEvent())
      algos.append(self.makeD2KPPos())
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

   
   # Make D2PPP 
   def makeD2PPP(self):
      StdLoosePIDsPi = DataOnDemand(Location = "Phys/StdLoosePions")
      str_cutsDaug_inc ="(MIPCHI2DV(PRIMARY) > %(DaugIPCHI2exc)s) & (PT > %(DaugPTexc)s *MeV) &  (P > %(DaugP)s *MeV) &  (TRCHI2DOF < %(DaugTrkCHI2)s) "  % self.getProps()
      str_cutsDaug_pidpi = "(PIDK-PIDpi < %(piPIDKmax)s )"  % self.getProps()
      str_cutsComb_inc = " (ACUTDOCA( %(DaugDOCA)s * mm, '' )) & (ANUM(PT > %(Daug2PT)s ) >= 2) & (ANUM(MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) >= 2) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(SumDaugPT)s*MeV)" % self.getProps()
      str_cutsMassPreFit = "(AM > %(MinMassPreFitpid)s) & (AM < %(MaxMassPreFit)s)" % self.getProps()
      str_cutsMassPostFit = "(M > %(MinMassPostFitexc)s) & (M < %(MaxMassPostFit)s)" % self.getProps()
      str_cutsMoth_inc = "(BPVDIRA > %(DDIRA)s) & (BPVVDCHI2 > %(DFDCHI2)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVTRGPOINTINGWPT < %(POINT)s)" % self.getProps()
      cbpinc = CombineParticles("CombineD2PPP"
                                , DecayDescriptor =  '[D+ -> pi- pi+ pi+]cc'
                                , DaughtersCuts = { "pi+" : '(' +str_cutsDaug_inc+ '&' + str_cutsDaug_pidpi +')' } 
                                , CombinationCut ='('+ str_cutsComb_inc +'&' + str_cutsMassPreFit +')'
                                , MotherCut ='('+ str_cutsMoth_inc + '&' + str_cutsMassPostFit + ')'
                                )
      selD2PPP = Selection("selD2PPP", Algorithm = cbpinc, RequiredSelections = [StdLoosePIDsPi] )
      return selD2PPP
    
   # Make D2KPP
   def makeD2KPP(self):
      StdLoosePIDsPi = DataOnDemand(Location = "Phys/StdLoosePions")
      StdTightPIDsK = DataOnDemand(Location = "Phys/StdTightKaons")
      str_cutsDaug_inc ="(MIPCHI2DV(PRIMARY) > %(DaugIPCHI2exc)s) & (PT > %(DaugPTexc)s *MeV) &  (P > %(DaugP)s *MeV) &  (TRCHI2DOF < %(DaugTrkCHI2)s) "  % self.getProps()
      str_cutsDaug_pidK = "(PIDK-PIDpi > %(KPIDKmin)s )"  % self.getProps()
      str_cutsDaug_pidpi = "(PIDK-PIDpi < %(piPIDKmax)s )"  % self.getProps()
      str_cutsComb_inc = " (ACUTDOCA( %(DaugDOCA)s * mm, '' )) & (ANUM(PT > %(Daug2PT)s ) >= 2) & (ANUM(MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) >= 2) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(SumDaugPT)s*MeV)" % self.getProps()
      str_cutsMassPreFit = "(AM > %(MinMassPreFitpid)s) & (AM < %(MaxMassPreFit)s)" % self.getProps()
      str_cutsMassPostFit = "(M > %(MinMassPostFitexc)s) & (M < %(MaxMassPostFitkpp)s)" % self.getProps()
      str_cutsMoth_inc = "(BPVDIRA > %(DDIRA)s) & (BPVVDCHI2 > %(DFDCHI2)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVTRGPOINTINGWPT < %(POINT)s)" % self.getProps()
      cbpinc = CombineParticles("CombineD2KPP"
                                , DecayDescriptor =  '[D+ -> K- pi+ pi+]cc'
                                , DaughtersCuts = { "pi+" : '(' +str_cutsDaug_inc+ '&' + str_cutsDaug_pidpi +')' ,  "K-" : '(' + str_cutsDaug_inc+ '&' + str_cutsDaug_pidK+')'} 
                                , CombinationCut ='('+ str_cutsComb_inc +'&' + str_cutsMassPreFit +')'
                                , MotherCut ='('+ str_cutsMoth_inc + '&' + str_cutsMassPostFit + ')'
                                )
      selD2KPP = Selection("selD2KPP", Algorithm = cbpinc, RequiredSelections = [StdLoosePIDsPi, StdTightPIDsK] )
      return selD2KPP

   # Make D2KKP
   def makeD2KKP(self):
      StdLoosePIDsPi = DataOnDemand(Location = "Phys/StdLoosePions")
      StdTightPIDsK = DataOnDemand(Location = "Phys/StdTightKaons")
      str_cutsDaug_inc ="(MIPCHI2DV(PRIMARY) > %(DaugIPCHI2exc)s) & (PT > %(DaugPTexc)s *MeV) &  (P > %(DaugP)s *MeV) &  (TRCHI2DOF < %(DaugTrkCHI2)s) "  % self.getProps()
      str_cutsDaug_pidK = "(PIDK-PIDpi > %(KPIDKmin)s )"  % self.getProps()
      str_cutsDaug_pidpi = "(PIDK-PIDpi < %(piPIDKmax)s )"  % self.getProps()
      str_cutsComb_inc = "(ACUTDOCA( %(DaugDOCA)s * mm, '' )) & (ANUM(PT > %(Daug2PT)s ) >= 2) & (ANUM(MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) >= 2) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(SumDaugPT)s*MeV)" % self.getProps()
      str_cutsMassPreFit = "(AM > %(MinMassPreFitpid)s) & (AM < %(MaxMassPreFit)s)" % self.getProps()
      str_cutsMassPostFit = "(M > %(MinMassPostFitexc)s) & (M < %(MaxMassPostFit)s)" % self.getProps()
      str_cutsMoth_inc = "(BPVDIRA > %(DDIRA)s) & (BPVVDCHI2 > %(DFDCHI2)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVTRGPOINTINGWPT < %(POINT)s)" % self.getProps()
      cbpinc = CombineParticles("CombineD2KKP"
                                , DecayDescriptor =  '[D+ -> K- K+ pi+]cc'
                                , DaughtersCuts = { "pi+" : '(' +str_cutsDaug_inc+ '&' + str_cutsDaug_pidpi +')' ,  "K-" : '(' + str_cutsDaug_inc+ '&' + str_cutsDaug_pidK+')'} 
                                , CombinationCut ='('+ str_cutsComb_inc +'&' + str_cutsMassPreFit +')'
                                , MotherCut ='('+ str_cutsMoth_inc + '&' + str_cutsMassPostFit + ')'
                                )
      selD2KKP = Selection("selD2KKP", Algorithm = cbpinc, RequiredSelections = [StdLoosePIDsPi, StdTightPIDsK] )
      return selD2KKP


   def makeD2hhhinc(self):
      StdNoPIDsPi = DataOnDemand(Location = "Phys/StdNoPIDsPions")
      str_cutsDaug_inc ="((MIPCHI2DV(PRIMARY) > %(DaugIPCHI2inc)s) & (PT > %(DaugPTinc)s *MeV) &  (P > %(DaugP)s *MeV) &  (TRCHI2DOF < %(DaugTrkCHI2)s) )"  % self.getProps()
      str_cutsComb_inc = " (ACUTDOCA( %(DaugDOCA)s * mm, '' )) & (ANUM(PT > %(Daug2PT)s ) >= 2) & (ANUM(MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) >= 2) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(SumDaugPT)s*MeV)" % self.getProps()
      str_cutsMassPreFit_inc = "(AM > %(MinMassPreFitinc)s) & (AM < %(MaxMassPreFit)s)" % self.getProps()
      str_cutsMassPostFit_inc = "(M > %(MinMassPostFitinc)s) & (M < %(MaxMassPostFitinc)s)" % self.getProps()
      str_cutsMoth_inc = "(BPVDIRA > %(DDIRA)s) & (BPVVDCHI2 > %(DFDCHI2)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVTRGPOINTINGWPT < %(POINT)s)" % self.getProps()
      cbpinc = CombineParticles("CombineD2hhh_inc"
                                , DecayDescriptor =  '[D+ -> pi- pi+ pi+]cc'
                                , DaughtersCuts = { "pi+" : str_cutsDaug_inc }
                                , CombinationCut = '(' + str_cutsComb_inc +'&' + str_cutsMassPreFit_inc +')'
                                , MotherCut = '(' + str_cutsMoth_inc + '&' + str_cutsMassPostFit_inc + ')'
                                )
      selD2hhhinc = Selection("selD2hhhinc", Algorithm = cbpinc, RequiredSelections = [StdNoPIDsPi] )
      return selD2hhhinc

   #make new particles for KKK(with PID cuts on tracks)
   def makeD2KKK(self):
      StdTightK = DataOnDemand(Location = "Phys/StdTightKaons")
      str_cutsDaug_pid = "( (MIPCHI2DV(PRIMARY) > %(DaugIPCHI2pid)s) & (PT > %(DaugPTpid)s *MeV) & (P > %(DaugP)s *MeV) & (TRCHI2DOF < %(DaugTrkCHI2)s) )" % self.getProps()
      str_cutsDaug_pidK = "(PIDK-PIDpi > %(KPIDKmin)s )"  % self.getProps()
      str_cutsComb_pid = "(ACUTDOCA( %(DaugDOCA)s * mm, '' )) & (ANUM(PT > %(Daug2PT)s ) >= 2) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(SumDaugPT)s*MeV) & (ANUM(MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2pid)s ) >= 2) & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Daug1IPCHI2)s))" % self.getProps()
      str_cutsMassPreFit_pid = "(AM > %(MinMassPreFitpid)s) & (AM < %(MaxMassPreFit)s)" % self.getProps()
      str_cutsMassPostFit_pid = "(M > %(MinMassPostFitexc)s) & (M < %(MaxMassPostFit)s)" % self.getProps()
      str_cutsMoth_pid = "(BPVDIRA > %(DDIRA)s) & (BPVVDCHI2 > %(DFDCHI2)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVTRGPOINTINGWPT < %(POINT)s)" % self.getProps()
      cbppid = CombineParticles("CombineD2KKK"
                                , DecayDescriptor =  '[D+ -> K- K+ K+]cc'
                                , DaughtersCuts = { "K+" : '(' + str_cutsDaug_pidK +'&' + str_cutsDaug_pid +  ')' }
                                , CombinationCut = '(' + str_cutsComb_pid +'&' + str_cutsMassPreFit_pid + ')'
                                , MotherCut = '(' + str_cutsMoth_pid + '&' + str_cutsMassPostFit_pid + ')'
                                 )
      selD2KKK = Selection("selD2KKK", Algorithm = cbppid, RequiredSelections = [StdTightK] )
      return selD2KKK

   # make D2KPP with opposite sign
   def makeD2KPPos(self):
      StdLoosePIDsPi = DataOnDemand(Location = "Phys/StdLoosePions")
      StdTightPIDsK = DataOnDemand(Location = "Phys/StdTightKaons")
      str_cutsDaug_inc ="(MIPCHI2DV(PRIMARY) > %(DaugIPCHI2exc)s) & (PT > %(DaugPTexc)s *MeV) &  (P > %(DaugP)s *MeV) &  (TRCHI2DOF < %(DaugTrkCHI2)s) "  % self.getProps()
      str_cutsDaug_pidK = "(PIDK-PIDpi > %(KPIDKmin)s )"  % self.getProps()
      str_cutsDaug_pidpi = "(PIDK-PIDpi < %(piPIDKmax)s )"  % self.getProps()
      str_cutsComb_inc = " (ACUTDOCA( %(DaugDOCA)s * mm, '' )) & (ANUM(PT > %(Daug2PT)s ) >= 2) & (ANUM(MIPCHI2DV(PRIMARY) > %(Daug2IPCHI2)s ) >= 2) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(SumDaugPT)s*MeV)" % self.getProps()
      str_cutsMassPreFit = "(AM > %(MinMassPreFitpid)s) & (AM < %(MaxMassPreFit)s)" % self.getProps()
      str_cutsMassPostFit = "(M > %(MinMassPostFitexc)s) & (M < %(MaxMassPostFitkpp)s)" % self.getProps()
      str_cutsMoth_inc = "(BPVDIRA > %(DDIRA)s) & (BPVVDCHI2 > %(DFDCHI2)s) & (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DFDCHI2)s) & (VFASPF(VCHI2/VDOF) < %(DVtxCHI2NDOF)s) & (PT > %(DPT)s) & (BPVTRGPOINTINGWPT < %(POINT)s)" % self.getProps()
      cbpinc = CombineParticles("CombineD2KPPos"
                                , DecayDescriptor =  '[D+ -> pi- pi+ K+]cc'
                                , DaughtersCuts = { "pi+" : '(' +str_cutsDaug_inc+ '&' + str_cutsDaug_pidpi +')' ,  "K-" : '(' + str_cutsDaug_inc+ '&' + str_cutsDaug_pidK+')'} 
                                , CombinationCut ='('+ str_cutsComb_inc +'&' + str_cutsMassPreFit +')'
                                , MotherCut ='('+ str_cutsMoth_inc + '&' + str_cutsMassPostFit + ')'
                                )
      selD2KPPos = Selection("selD2KPPos", Algorithm = cbpinc, RequiredSelections = [StdLoosePIDsPi, StdTightPIDsK] )
      return selD2KPPos

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
         'FilterNTracks'
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
