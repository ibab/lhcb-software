__author__ = ['Liming Zhang']
__date__ = '24/03/2010'
__version__ = '$Revision: 1.2 $'
"""
Bs->phiMuMu and f0(980)MuMu selections
"""
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions

__all__ = ('Bs2PhiMuMuLinesConf',
           'makeBs2XMuMu',
           'confdict')

confdict = {
    "MINIPCHI2"      : 4.00   # adimensiional
    ,"TRCHI2"        : 10.0   # adimensiional
    ,"KaonPIDK"      : 1e-10  # adimensiional
    ,"PhiPT"         : 100    # MeV
    ,"MuonMINIPCHI2" : 2.25   # adimensiional
    ,"MuonPIDmu"     : -5.0  # adimensiional
    ,"MuonTRCHI2"    : 10.0   # adimensiional
    ,"BsMassWin"     : 250.0  # MeV
    ,"BsVCHI2DOF"    : 8.0   # adimensiional
    ,"BsDIRA"        : 0.9993  # adimensiional
    ,"BsFDCHI2"      : 25.0   # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"f0MassWin"     : 200.0  # MeV
    ,"VCHI2"         : 10.0   # adimensiional
    ,"BsIPCHI2"      : 36.0  # adimensiional
    ,"DocaChi2Max"   : 20  #mm
    }

class Bs2PhiMuMuLinesConf(LineBuilder) :

    __configuration_keys__ = (
        "MINIPCHI2"      
        ,"TRCHI2"        
        ,"KaonPIDK"      
        ,"PhiPT"         
        ,"MuonMINIPCHI2" 
        ,"MuonPIDmu"     
        ,"MuonTRCHI2"    
        ,"BsMassWin"     
        ,"BsVCHI2DOF"    
        ,"BsDIRA"        
        ,"BsFDCHI2"      
        ,"PionPIDK"      
        ,"f0MassWin"     
        ,"VCHI2"         
        ,"BsIPCHI2"      
        ,"DocaChi2Max"   
        )

    __confdict__={}
    
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config
        
        self.seldiMuon = Selection( "diMuonfor" + name, 
                                    Algorithm = self._diMuonFilter(),
                                    RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles")] )
        
        self.selPhi =  Selection( "phifor" + name, 
                                  Algorithm = self._phiFilter(), 
                                  RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseDetachedPhi2KK/Particles")] )
        
        self.self0 = Selection( "f0for" + name, 
                                Algorithm = self._f02PiPi(),
                                RequiredSelections = [ StdLoosePions ])

        self.selBs2PhiMuMu = makeBs2XMuMu("Bs2PhiMuMu" + name,
                                          DecayDescriptor =  'B_s0 -> J/psi(1S) phi(1020)',
                                          DaughterLists = [ self.seldiMuon, self.selPhi ],
                                          BsMassWin = config['BsMassWin'],
                                          BsVCHI2DOF = config['BsVCHI2DOF'],
                                          BsDIRA = config['BsDIRA'],
                                          BsIPCHI2 = config['BsIPCHI2'],
                                          BsFDCHI2 = config['BsFDCHI2']
                                          )
        self.selBs2f0MuMu = makeBs2XMuMu("Bs2f0MuMu" + name,
                                         DecayDescriptor =  'B_s0 -> J/psi(1S) f_0(980)',
                                         DaughterLists = [ self.seldiMuon, self.self0 ],
                                         BsMassWin = config['BsMassWin'],
                                         BsVCHI2DOF = config['BsVCHI2DOF'],
                                         BsDIRA = config['BsDIRA'],
                                         BsIPCHI2 = config['BsIPCHI2'],
                                         BsFDCHI2 = config['BsFDCHI2']
                                         )
        
        """
        The Signal line Bs->PhiMuMu
        """
        self.Bs2PhiMuMuLine = StrippingLine('Bs2PhiMuMu' + name + 'Line'
                                            , prescale = 1
                                            , selection = self.selBs2PhiMuMu
                                            )
        """
        The Signal line Bs->f0MuMu
        """
        self.Bs2f0MuMuLine = StrippingLine('Bs2f0MuMu' + name + 'Line'
                                           , prescale = 1
                                           , selection = self.selBs2f0MuMu
                                           )

        self.registerLine(self.Bs2PhiMuMuLine)
        self.registerLine(self.Bs2f0MuMuLine)  
        
    def _phiFilter( self ):
        _code = "(PT > %(PhiPT)s *MeV) & (VFASPF(VCHI2) < %(VCHI2)s) & (2==NINTREE( ('K+'==ABSID) & (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s ) & (TRCHI2DOF < %(TRCHI2)s) & (PIDK> %(KaonPIDK)s)))" % self.__confdict__
        _phi = FilterDesktop( Code = _code )
        return _phi
    
    def _diMuonFilter( self ):
        _code = "(VFASPF(VCHI2) < %(VCHI2)s) & (2==NINTREE( ('mu+'==ABSID) & (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s) & (HASMUON) & (PIDmu> %(MuonPIDmu)s) & (TRCHI2DOF< %(MuonTRCHI2)s))) & (BPVDIRA > 0.0)" % self.__confdict__
        _diMuon = FilterDesktop( Code = _code )
        return _diMuon 
    
    def _f02PiPi( self ):
        _decayDescriptors = ["f_0(980) -> pi+ pi-"]
        _combinationCut = "(ADAMASS('f_0(980)') < %(f0MassWin)s *MeV) & (APT > %(PhiPT)s) & (ADOCACHI2CUT( %(DocaChi2Max)s, ''))" % self.__confdict__
        _daughtersCuts = { "pi+" : "  (MIPCHI2DV(PRIMARY)> %(MINIPCHI2)s )"\
                               "& (TRCHI2DOF < %(TRCHI2)s) & (PIDK< %(PionPIDK)s)" % self.__confdict__ }
        _motherCut = "(VFASPF(VCHI2) < %(VCHI2)s)"  % self.__confdict__   
        _f0 = CombineParticles(DecayDescriptors = _decayDescriptors,
                               CombinationCut = _combinationCut,
                               DaughtersCuts = _daughtersCuts,
                               MotherCut = _motherCut)                                         
        return _f0

def makeBs2XMuMu(name,
                 DecayDescriptor,
                 DaughterLists,
                 BsMassWin,
                 BsVCHI2DOF,
                 BsDIRA,
                 BsIPCHI2,
                 BsFDCHI2):
    from Configurables import CombineParticles, OfflineVertexFitter
    combinerName = "Combine" + name
    _Bs = CombineParticles(combinerName)
    _Bs.DecayDescriptor = DecayDescriptor 
    _Bs.CombinationCut = "ADAMASS('B_s0') < %(BsMassWin)s *MeV" % locals()
    _Bs.MotherCut = "  (VFASPF(VCHI2/VDOF) < %(BsVCHI2DOF)s)" \
                    "& (BPVDIRA > %(BsDIRA)s)" \
                    "& (BPVIPCHI2() < %(BsIPCHI2)s)" \
                    "& (BPVVDCHI2 > %(BsFDCHI2)s)" % locals()
    #        _Bs.ReFitPVs = True
    # Set the OfflineVertexFitter to keep the 4 tracks
    _Bs.addTool( OfflineVertexFitter )
    _Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    _Bs.OfflineVertexFitter.useResonanceVertex = False
    return Selection(name, 
                     Algorithm = _Bs,
                     RequiredSelections = DaughterLists)
        
           
 
        
    
