
__author__ = ['Sophie Redford','Wenbin Qian']
__date__ = '21/05/2010'
__version__ = '$Revision: 1.4 $'

"""
B -> X mu mu , X=pi/K channels
"""

config_params =  {'MuonP'         : 3000. ,    #MeV
                  'MuonPT'        : 500.  ,    #MeV
                  #'MuonPIDK'      : -5.   ,    #adimensional
                  #'MuonPIDmu'     : -5.   ,    #adimensional
                  #'MuonPIDp'      : -5.   ,    #adimensional
                  'MuonMINIPCHI2' : 5     ,    #adminensional
                  
                  'PionP'         : 2000. ,    #MeV
                  'PionPT'        : 500.  ,    #MeV
                  'PionMINIPCHI2' : 5     ,    #adminensional
                  
                  'KaonP'         : 2000. ,    #MeV
                  'KaonPT'        : 500.  ,    #MeV
                  #'KaonPIDK'      : -1.   ,    #adimensional
                  'KaonMINIPCHI2' : 5     ,    #adminensional
                  
                  'DimuonMass'    : 0.  ,    #MeV
                  'BVCHI2DOF'     : 7    ,    #adminensional                              
                  #'BFDCHI2'       : 9     ,    #adimensional
                  'BDIRA'         : 0.9998 ,    #adimensional
                  'BIPCHI2'       : 30    ,    #adimensional
                  'BMassWin'      : 400.  ,    #MeV, mass window
                  
                  'B2PiMuMuOSLinePrescale'  : 1 ,
                  'B2PiMuMuOSLinePostscale' : 1 ,
                  'B2PiMuMuSSLinePrescale'  : 1 ,
                  'B2PiMuMuSSLinePostscale' : 1 ,
                  'B2KMuMuOSLinePrescale'   : 1 ,
                  'B2KMuMuOSLinePostscale'  : 1 ,
                  'B2KMuMuSSLinePrescale'   : 1 ,
                  'B2KMuMuSSLinePostscale'  : 1 }

__all__ = ('B2XMuMuSSConf', 'makeB2PiMuMuOS', 'makeB2PiMuMuSS', 'makeB2KMuMuOS', 'makeB2KMuMuSS' )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsKaons, StdLoosePions, StdLooseMuons

default_name = "B2XMuMuSS"

class B2XMuMuSSConf(LineBuilder) :
    """
    Builder for B2XMuMu, X = pi/K 
    """
    
    PiOSLine = None
    PiSSLine = None
    KOSLine = None
    KSSLine = None
        
    __configuration_keys__ = ('MuonP'
                              , 'MuonPT'
                              #, 'MuonPIDK'
                              #, 'MuonPIDmu'
                              #, 'MuonPIDp'
                              , 'MuonMINIPCHI2'
                              
                              , 'PionP'
                              , 'PionPT'
                              , 'PionMINIPCHI2'
        
                              , 'KaonP'
                              , 'KaonPT'
                              #, 'KaonPIDK'
                              , 'KaonMINIPCHI2'
                              
                              , 'DimuonMass'
                              , 'BVCHI2DOF'
                              #, 'BFDCHI2'
                              , 'BIPCHI2'
                              , 'BDIRA'
                              , 'BMassWin'

                              , 'B2PiMuMuOSLinePrescale'
                              , 'B2PiMuMuOSLinePostscale'
                              , 'B2PiMuMuSSLinePrescale'
                              , 'B2PiMuMuSSLinePostscale'
                              , 'B2KMuMuOSLinePrescale'
                              , 'B2KMuMuOSLinePostscale'
                              , 'B2KMuMuSSLinePrescale'
                              , 'B2KMuMuSSLinePostscale'
                              )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)

        PiOSLine_name = name+"_PiOS"
        PiSSLine_name = name+"_PiSS"
        KOSLine_name = name+"_KOS"
        KSSLine_name = name+"_KSS"
        
        # 1 : Make kaons
        selKaons = makeKaons(name="KaonsFor"+name
                             , KaonP = config['KaonP']
                             , KaonPT = config['KaonPT']
                             , KaonMINIPCHI2 = config['KaonMINIPCHI2'])

        # 2 : Make pions
        selPions = makePions(name="PionsFor"+name
                             , PionP = config['PionP']
                             , PionPT = config['PionPT']
                             , PionMINIPCHI2 = config['PionMINIPCHI2'])
                                                        
        # 3 : Make muons
        selMuons = makeMuons(name="MuonsFor"+name
                             , MuonP = config['MuonP']
                             , MuonPT = config['MuonPT']
                             , MuonMINIPCHI2 = config['MuonMINIPCHI2'])
                                        
        # 4 : Combine
        selB2PiMuMuOS = self._makeB2PiMuMuOS(name=PiOSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             config = config)

        selB2PiMuMuSS = self._makeB2PiMuMuSS(name=PiSSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             config = config)

        selB2KMuMuOS = self._makeB2KMuMuOS(name=KOSLine_name,
                                           kaonSel = selKaons,
                                           muonSel = selMuons,
                                           config = config)

        selB2KMuMuSS = self._makeB2KMuMuSS(name=KSSLine_name,
                                           kaonSel = selKaons,
                                           muonSel = selMuons,
                                           config = config)

        # 5 : Declare Lines
        self.PiOSLine = StrippingLine(PiOSLine_name+"Line",
                                      prescale = config['B2PiMuMuOSLinePrescale'],
                                      postscale = config['B2PiMuMuOSLinePostscale'],
                                      selection = selB2PiMuMuOS
                                      )
        
        self.PiSSLine = StrippingLine(PiSSLine_name+"Line",
                                      prescale = config['B2PiMuMuSSLinePrescale'],
                                      postscale = config['B2PiMuMuSSLinePostscale'],
                                      selection = selB2PiMuMuSS
                                      )
        
        self.KOSLine = StrippingLine(KOSLine_name+"Line",
                                     prescale = config['B2KMuMuOSLinePrescale'],
                                     postscale = config['B2KMuMuOSLinePostscale'],
                                     selection = selB2KMuMuOS
                                     )
        
        self.KSSLine = StrippingLine(KSSLine_name+"Line",
                                     prescale = config['B2KMuMuSSLinePrescale'],
                                     postscale = config['B2KMuMuSSLinePostscale'],
                                     selection = selB2KMuMuSS
                                     )

        # 6 : register Line
        self.registerLine( self.PiOSLine )
        self.registerLine( self.PiSSLine )
        self.registerLine( self.KOSLine )
        self.registerLine( self.KSSLine )

#####################################################
    def _makeB2PiMuMuOS(self, name, pionSel, muonSel, config):
        """
        Handy interface for B2XMuMu
        """
        return makeB2PiMuMuOS(name
                              , pionSel
                              , muonSel
                              , BVCHI2DOF = config['BVCHI2DOF']
                              , BDIRA = config['BDIRA']
                              , BIPCHI2 = config['BIPCHI2']
                              , BMassWin = config['BMassWin']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeB2PiMuMuSS(self, name, pionSel, muonSel, config):
        """
        Handy interface for B2XMuMu
        """
        return makeB2PiMuMuSS(name
                              , pionSel
                              , muonSel
                              , BVCHI2DOF = config['BVCHI2DOF']
                              , BDIRA = config['BDIRA']
                              , BIPCHI2 = config['BIPCHI2']
                              , BMassWin = config['BMassWin']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeB2KMuMuOS(self, name, kaonSel, muonSel, config):
        """
        Handy interface for B2XMuMu
        """
        return makeB2KMuMuOS(name
                             , kaonSel
                             , muonSel
                             , BVCHI2DOF = config['BVCHI2DOF']
                             , BDIRA = config['BDIRA']
                             , BIPCHI2 = config['BIPCHI2']
                             , BMassWin = config['BMassWin']
                             , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeB2KMuMuSS(self, name, kaonSel, muonSel, config):
        """
        Handy interface for B2XMuMu
        """
        return makeB2KMuMuSS(name
                             , kaonSel
                             , muonSel
                             , BVCHI2DOF = config['BVCHI2DOF']
                             , BDIRA = config['BDIRA']
                             , BIPCHI2 = config['BIPCHI2']
                             , BMassWin = config['BMassWin']
                             , DimuonMass = config['DimuonMass'])
#
# Out of class
#####################################################
def makeB2PiMuMuOS(name, pionSel, muonSel, BMassWin, BVCHI2DOF, BIPCHI2, BDIRA, DimuonMass):
    """
    Makes the B+ -> pi+ mu+ mu- 
    """

    _combcut = "((ADAMASS('B+') < %(BMassWin)s *MeV) | (ADAMASS('B_c+') < %(BMassWin)s *MeV) ) & "\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()
   
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(BVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(BIPCHI2)s) & "\
              "(BPVDIRA > %(BDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[B+ -> pi+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, pionSel ] )

#####################################################
def makeB2PiMuMuSS(name, pionSel, muonSel, BMassWin, BVCHI2DOF, BIPCHI2, BDIRA, DimuonMass):
    """
    Makes the B- -> pi+ mu- mu-
    """
    
    _combcut = "((ADAMASS('B+') < %(BMassWin)s *MeV) | (ADAMASS('B_c+') < %(BMassWin)s *MeV)) & "\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(BVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(BIPCHI2)s) & "\
              "(BPVDIRA > %(BDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[B- -> pi+ mu- mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, pionSel ] )

#####################################################
def makeB2KMuMuOS(name, kaonSel, muonSel, BMassWin, BVCHI2DOF, BIPCHI2, BDIRA, DimuonMass):
    """
    Makes the B+ -> K+ mu+ mu-
    """
    
    _combcut = "((ADAMASS('B+') < %(BMassWin)s *MeV) | (ADAMASS('B_c+') < %(BMassWin)s *MeV) ) & "\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(BVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(BIPCHI2)s) & "\
              "(BPVDIRA > %(BDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[B+ -> K+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel ] )

#####################################################
def makeB2KMuMuSS(name, kaonSel, muonSel, BMassWin, BVCHI2DOF, BIPCHI2, BDIRA, DimuonMass):
    """
    Makes the B- -> K+ mu- mu-
    """
    
    _combcut = "((ADAMASS('B+') < %(BMassWin)s *MeV) | (ADAMASS('B_c+') < %(BMassWin)s *MeV) ) & "\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(BVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(BIPCHI2)s) & "\
              "(BPVDIRA > %(BDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[B- -> K+ mu- mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel ] )
                    
#####################################################
def makeKaons(name, KaonP, KaonPT, KaonMINIPCHI2):
    """
    Kaon selection
    """
    _code = "(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdNoPIDsKaons ] )

#####################################################
def makePions(name, PionP, PionPT, PionMINIPCHI2):
    """
    Pion selection
    """
    _code = "(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s *MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
                    
#####################################################
def makeMuons(name, MuonP, MuonPT, MuonMINIPCHI2):
    """
    Muon selection
    """
    _code = "(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseMuons ] )

                    

