# $Id: StrippingB2DMuMuSS.py Exp $

'''
Module for construction of Majoranne study
'''

__author__=['Wenbin Qian, Fred Blanc']
__date__ = '01/05/2011'
__version__= '$Revision: 1.0 $'

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions
from StandardParticles import StdLooseMuons
from StandardParticles import StdLooseKaons


__all__ = ('StrippingB2SameMuonConf')

config =  {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.
    }

class StrippingB2SameChargeMuonConf(LineBuilder):
    __configuration_keys__ = (
        'LinePrescale',
        'LinePostscale'
        )
    
    def __init__(self, name, config ):
        
        LineBuilder.__init__(self,name,config)
        self.name = name
        self.config = config
        
        self.MuonList = self._MuonFilter()

        self.KaonList = self._KaonFilter()

        self.PionList = self._PionFilter()

        self.JpsiList = self._JpsiMaker([self.MuonList])

        self.D0List = self._D0Maker([self.KaonList,self.PionList])

        self.DpList = self._DpMaker([self.KaonList,self.PionList])

        self.DsList = self._DsMaker([self.KaonList,self.PionList])

        self._makeB2JpsiKPiPi([self.JpsiList,self.KaonList,self.PionList])

        self._makeB2D0MuMuPi([self.MuonList,self.D0List,self.PionList])

        self._makeB2DpMuMu([self.MuonList,self.DpList])

        self._makeB2DsMuMu([self.MuonList,self.DsList])

        self._makeBc2DsMuMu([self.MuonList,self.DsList])
    
    def _MuonFilter( self ):       
        Muoncut = "(TRCHI2DOF < 5.0) & (PIDmu > 0.0)  & (MIPCHI2DV(PRIMARY) > 4.0) & (PT > 300*MeV)"
        muonfilter =  FilterDesktop(Code = Muoncut)
        SelectedMuon = Selection(self.name+"SelMuon",
                                 Algorithm = muonfilter,
                                 RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseMuons/Particles")])
        return SelectedMuon
    
    def _KaonFilter( self ):
        Kaoncut = "(TRCHI2DOF < 5.0) & (PIDK > -1.0)  & (MIPCHI2DV(PRIMARY) > 4.0)  & (PT > 300*MeV)"
        kaonfilter = FilterDesktop(Code = Kaoncut)
        SelectedKaon = Selection(self.name+"SelKaon",
                                 Algorithm = kaonfilter,
                                 RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKaons/Particles")])
        
        return SelectedKaon
    
    def _PionFilter( self ):
        Pioncut = "(TRCHI2DOF < 5.0) & (PIDK < 6.0)  & (MIPCHI2DV(PRIMARY) > 4.0)  & (PT > 300*MeV)"
        pionfilter = FilterDesktop(Code = Pioncut)
        SelectedPion = Selection(self.name+"SelPion",
                                 Algorithm = pionfilter,
                                 RequiredSelections = [DataOnDemand(Location = "Phys/StdLoosePions/Particles")])
        return SelectedPion
    
    def _JpsiMaker( self , inputList):
        Jpsi2MuMu = CombineParticles( 
                                      DecayDescriptor =  "J/psi(1S) -> mu- mu+" ,
                                      CombinationCut = "(ADAMASS('J/psi(1S)') < 100*MeV)",
                                      MotherCut = "(ADMASS('J/psi(1S)') < 100*MeV) & (VFASPF(VCHI2/VDOF) < 6.0)",
                                      ReFitPVs = False)
        return Selection ( self.name+"Jpsi2MuMu",
                           Algorithm = Jpsi2MuMu,
                           RequiredSelections = inputList)


    def _D0Maker( self,inputList ):
        D02kpi = CombineParticles( 
                                   DecayDescriptor = "[D0 -> K- pi+]cc" ,
                                   CombinationCut = "(ADAMASS('D0') < 100*MeV)",
                                   MotherCut = "(ADMASS('D0') < 80*MeV) & (VFASPF(VCHI2/VDOF) < 6.0)",
                                   ReFitPVs = False)
        return Selection ( self.name+"D02kpi",
                           Algorithm = D02kpi,
                           RequiredSelections = inputList)
    
    def _DsMaker( self, inputList ):
        Ds2kkpi = CombineParticles( 
                                    DecayDescriptor = "[D_s+ -> K+ K- pi+]cc",
                                    CombinationCut = "(DAMASS('D_s+') < 100*MeV) & (DAMASS('D+')> -100 *MeV)",
                                    MotherCut = "(DMASS('D_s+') < 80 *MeV) & (DMASS('D+') > -80 *MeV) & (VFASPF(VCHI2/VDOF) < 6.0)",
                                    ReFitPVs = False)
        return Selection ( self.name+"Ds2kkpi",
                           Algorithm = Ds2kkpi,
                           RequiredSelections = inputList)
    
    def _DpMaker( self , inputList):
        Dp2kpipi = CombineParticles( 
                                     DecayDescriptor = "[D+ -> K- pi+ pi+]cc",
                                     CombinationCut = "(ADAMASS('D+') < 300*MeV)",
                                     MotherCut = "(ADMASS('D+') < 250*MeV) & (VFASPF(VCHI2/VDOF) < 6.0)" ,
                                     ReFitPVs = False)
        return Selection ( self.name+"Dp2kpipi",
                           Algorithm = Dp2kpipi,
                           RequiredSelections = inputList)
    
    
    def _makeB2JpsiKPiPi(self,inputList):
        combineJpsi = CombineParticles( 
                                        DecayDescriptor = "[B- -> J/psi(1S) K- pi- pi+]cc",
                                        CombinationCut = "(ADAMASS('B+')< 400*MeV)",
                                        MotherCut = "(ADMASS('B+')< 300*MeV) & (VFASPF(VCHI2/VDOF) < 6.0) & (BPVDIRA> 0.9999)" ,
                                        ReFitPVs = False)
        B2JpsiKPiPi = Selection ( self.name+"B2JpsiKPiPi",
                                  Algorithm = combineJpsi,
                                  RequiredSelections = inputList)
        
        B2JpsiKPiPiLine = StrippingLine(self.name+"B2JpsiKPiPiLine",
                                        prescale = 1,
                                        algos = [ B2JpsiKPiPi]
                                        )
        self.registerLine(B2JpsiKPiPiLine)
        
    def _makeB2D0MuMuPi(self, inputList):
        combineD0 = CombineParticles( 
                                      DecayDescriptor = "[B- -> D0 mu- mu- pi+]cc",
                                      CombinationCut = "(ADAMASS('B+')< 400*MeV)",
                                      MotherCut = "(ADMASS('B+')< 300*MeV) & (VFASPF(VCHI2/VDOF) < 6.0) & (BPVDIRA> 0.9999)" ,
                                      ReFitPVs = False)
        B2D0MuMuPi = Selection ( self.name+"B2D0MuMuPi",
                                 Algorithm = combineD0,
                                 RequiredSelections = inputList)
        
        B2D0MuMuPiLine = StrippingLine( self.name+"B2D0MuMuPiLine"
                                        , prescale = 1
                                        , algos = [ B2D0MuMuPi ]
                                        )
        self.registerLine(B2D0MuMuPiLine)
        
    def _makeB2DpMuMu(self,inputList):
        combineDp = CombineParticles( 
                                      DecayDescriptor = "[B- -> D+ mu- mu-]cc",
                                      CombinationCut = "(ADAMASS('B+')< 400*MeV)",
                                      MotherCut = "(ADMASS('B+')< 300*MeV) & (VFASPF(VCHI2/VDOF) < 6.0) & (BPVDIRA> 0.9999)" ,
                                      ReFitPVs = False)
        B2DpMuMu = Selection ( self.name+"B2DpMuMu",
                               Algorithm = combineDp,
                               RequiredSelections = inputList)
        
        B2DpMuMuLine = StrippingLine(self.name+"B2DpMuMuLine"
                                     , prescale = 1
                                     , algos = [ B2DpMuMu ]
                                     )
        self.registerLine(B2DpMuMuLine)

    def _makeB2DsMuMu(self,inputList):
        combineDs = CombineParticles( 
                                      DecayDescriptor = "[B- -> D_s+ mu- mu-]cc",
                                      CombinationCut = "(ADAMASS('B+')< 400*MeV)",
                                      MotherCut = "(ADMASS('B+')< 300*MeV) & (VFASPF(VCHI2/VDOF) < 6.0) & (BPVDIRA> 0.9999)" ,
                                      ReFitPVs = False)
        B2DsMuMu = Selection ( self.name+"B2DsMuMu",
                               Algorithm = combineDs,
                               RequiredSelections = inputList)

        B2DsMuMuLine = StrippingLine(self.name+"B2DsMuMuLine"
                                     , prescale = 1
                                     , algos = [ B2DsMuMu ]
                                     )
        self.registerLine(B2DsMuMuLine)

    def _makeBc2DsMuMu(self,inputList):
        combineBcDs = CombineParticles( 
                                       DecayDescriptor = "[B_c- -> D_s+ mu- mu-]cc",
                                       CombinationCut = "(ADAMASS('B_c+')< 400*MeV)",
                                       MotherCut = "(ADMASS('B_c+')< 300*MeV) & (VFASPF(VCHI2/VDOF) < 6.0) & (BPVDIRA> 0.9999)" ,
                                       ReFitPVs = False)
        Bc2DsMuMu = Selection ( self.name+"Bc2DsMuMu",
                                Algorithm = combineBcDs,
                                RequiredSelections = inputList)
        
        Bc2DsMuMuLine = StrippingLine(self.name+"Bc2DsMuMuLine"
                                      , prescale = 1
                                      , algos = [ Bc2DsMuMu ]
                                      )
        self.registerLine(Bc2DsMuMuLine)
