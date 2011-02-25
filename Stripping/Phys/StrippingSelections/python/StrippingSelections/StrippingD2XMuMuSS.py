
__author__ = ['Malcolm John']
__date__ = '24/2/2011'
__version__ = '$Revision: 1.5 $'

'''
D->Xu mu mu , Xu=pi/K channels
'''

config_params =  {'MuonP'         : 3000. ,    #MeV
                  'MuonPT'        : 500.  ,    #MeV
                  'MuonMINIPCHI2' : 4     ,    #adminensional
									'MuonTRCHI2'    : 8     ,    #adminensional
                  
                  'PionP'         : 2000. ,    #MeV
                  'PionPT'        : 300.  ,    #MeV
                  'PionMINIPCHI2' : 6     ,    #adminensional
									'PionTRCHI2'    : 8     ,    #adminensional
                  
                  'KaonP'         : 2000. ,    #MeV
                  'KaonPT'        : 300.  ,    #MeV
                  'KaonPIDK'      : -1.   ,    #adimensional
                  'KaonMINIPCHI2' : 6     ,    #adminensional
									'KaonTRCHI2'    : 8     ,    #adminensional
                  
                  'DimuonMass'    : 250.  ,    #MeV
                  'DVCHI2DOF'     : 5    ,    #adminensional                              
                  #'DFDCHI2'       : 9     ,    #adimensional
                  'DDIRA'         : 0.9999,    #adimensional
                  'DIPCHI2'       : 30    ,    #adimensional
                  'DMassWin'      : 200.  ,    #MeV, mass window
                  'DMassLow'      :1763.  ,    #MeV, low-mass veto
                  
                  'D2PiMuMuOSLinePrescale'  : 1 ,
                  'D2PiMuMuOSLinePostscale' : 1 ,
                  'D2PiMuMuSSLinePrescale'  : 1 ,
                  'D2PiMuMuSSLinePostscale' : 1 ,
                  'D2KMuMuOSLinePrescale'   : 1 ,
                  'D2KMuMuOSLinePostscale'  : 1 ,
                  'D2KMuMuSSLinePrescale'   : 1 ,
                  'D2KMuMuSSLinePostscale'  : 1 ,
                  'D2PiPiPiCalLinePrescale' : 0.01 ,
                  'D2PiPiPiCalLinePostscale': 1 }

__all__ = ('D2XMuMuConf', 'makeD2PiPiPi', 'makeD2PiMuMuOS', 'makeD2PiMuMuSS', 'makeD2KMuMuOS', 'makeD2KMuMuSS' )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons, StdLoosePions, StdLooseMuons

name = "D2XMuMu"

class D2XMuMuConf(LineBuilder) :
    """
    Builder for D2XMuMu, X = pi/K 
    """
    
    PiCalLine = None
    PiOSLine = None
    PiSSLine = None
    KOSLine = None
    KSSLine = None
        
    __configuration_keys__ = (  'MuonP'
                              , 'MuonPT'
                              , 'MuonMINIPCHI2'
															, 'MuonTRCHI2'
                              
                              , 'PionP'
                              , 'PionPT'
                              , 'PionMINIPCHI2'
															, 'PionTRCHI2'
        
                              , 'KaonP'
                              , 'KaonPT'
                              , 'KaonPIDK'
                              , 'KaonMINIPCHI2'
															, 'KaonTRCHI2'
                              
                              , 'DimuonMass'
                              , 'DVCHI2DOF'
                              #, 'DFDCHI2'
                              , 'DIPCHI2'
                              , 'DDIRA'
                              , 'DMassWin'
                              , 'DMassLow'

                              , 'D2PiPiPiCalLinePrescale'
                              , 'D2PiPiPiCalLinePostscale'
                              , 'D2PiMuMuOSLinePrescale'
                              , 'D2PiMuMuOSLinePostscale'
                              , 'D2PiMuMuSSLinePrescale'
                              , 'D2PiMuMuSSLinePostscale'
                              , 'D2KMuMuOSLinePrescale'
                              , 'D2KMuMuOSLinePostscale'
                              , 'D2KMuMuSSLinePrescale'
                              , 'D2KMuMuSSLinePostscale'
                              )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)

        PiCalLine_name = name+"_PiCal"
        PiOSLine_name = name+"_PiOS"
        PiSSLine_name = name+"_PiSS"
        KOSLine_name = name+"_KOS"
        KSSLine_name = name+"_KSS"
        
        # 1 : Make kaons
        selKaons = makeKaons(name="KaonsFor"+name
                             , KaonP = config['KaonP']
                             , KaonPT = config['KaonPT']
                             , KaonPIDK = config['KaonPIDK']
                             , KaonMINIPCHI2 = config['KaonMINIPCHI2']
														 , KaonTRCHI2 = config['KaonTRCHI2'])

        # 2 : Make pions
        selPions = makePions(name="PionsFor"+name
                             , PionP = config['PionP']
                             , PionPT = config['PionPT']
                             , PionMINIPCHI2 = config['PionMINIPCHI2']
														 , PionTRCHI2 = config['PionTRCHI2'])
                                                        
        # 3 : Make muons
        selMuons = makeMuons(name="MuonsFor"+name
                             , MuonP = config['MuonP']
                             , MuonPT = config['MuonPT']
                             , MuonMINIPCHI2 = config['MuonMINIPCHI2']
														 , MuonTRCHI2 = config['MuonTRCHI2'])
                                                             
        # 3b : Make muons candidates from pions
        pionsAsMuons = makePionsAsMuons(name="PionsAsMuonsFor"+name
                             , MuonP = config['MuonP']
                             , MuonPT = config['MuonPT']
                             , MuonMINIPCHI2 = config['MuonMINIPCHI2']
														 , MuonTRCHI2 = config['MuonTRCHI2'])
                                        
        # 4 : Combine
        selD2PiPiPi   = self._makeD2PiPiPi(name=PiCalLine_name,
                                             pionSel = selPions,
                                             muonSel = pionsAsMuons,
                                             config = config)
																						 
        selD2PiMuMuOS = self._makeD2PiMuMuOS(name=PiOSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             config = config)

        selD2PiMuMuSS = self._makeD2PiMuMuSS(name=PiSSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             config = config)

        selD2KMuMuOS = self._makeD2KMuMuOS(name=KOSLine_name,
                                           kaonSel = selKaons,
                                           muonSel = selMuons,
                                           config = config)

        selD2KMuMuSS = self._makeD2KMuMuSS(name=KSSLine_name,
                                           kaonSel = selKaons,
                                           muonSel = selMuons,
                                           config = config)

        # 5 : Declare Lines
        self.PiCalLine = StrippingLine(PiCalLine_name+"Line",
                                      prescale = config['D2PiPiPiCalLinePrescale'],
                                      postscale = config['D2PiPiPiCalLinePostscale'],
                                      selection = selD2PiPiPi
                                      )
																			
        self.PiOSLine = StrippingLine(PiOSLine_name+"Line",
                                      prescale = config['D2PiMuMuOSLinePrescale'],
                                      postscale = config['D2PiMuMuOSLinePostscale'],
                                      selection = selD2PiMuMuOS
                                      )
        
        self.PiSSLine = StrippingLine(PiSSLine_name+"Line",
                                      prescale = config['D2PiMuMuSSLinePrescale'],
                                      postscale = config['D2PiMuMuSSLinePostscale'],
                                      selection = selD2PiMuMuSS
                                      )
        
        self.KOSLine = StrippingLine(KOSLine_name+"Line",
                                     prescale = config['D2KMuMuOSLinePrescale'],
                                     postscale = config['D2KMuMuOSLinePostscale'],
                                     selection = selD2KMuMuOS
                                     )
        
        self.KSSLine = StrippingLine(KSSLine_name+"Line",
                                     prescale = config['D2KMuMuSSLinePrescale'],
                                     postscale = config['D2KMuMuSSLinePostscale'],
                                     selection = selD2KMuMuSS
                                     )

        # 6 : register Line
        self.registerLine( self.KSSLine )
        self.registerLine( self.KOSLine )
        self.registerLine( self.PiSSLine )
        self.registerLine( self.PiOSLine )
        self.registerLine( self.PiCalLine )

        print "Created lines"

#####################################################
    def _makeD2PiPiPi(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiPiPi
        """
        return makeD2PiPiPi(name
                              , pionSel
                              , muonSel
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiMuMuOS(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiMuMuOS
        """
        return makeD2PiMuMuOS(name
                              , pionSel
                              , muonSel
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiMuMuSS(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiMuMuSS
        """
        return makeD2PiMuMuSS(name
                              , pionSel
                              , muonSel
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KMuMuOS(self, name, kaonSel, muonSel, config):
        """
        Handy interface for D2XMuMu
        """
        return makeD2KMuMuOS(name
                             , kaonSel
                             , muonSel
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KMuMuSS(self, name, kaonSel, muonSel, config):
        """
        Handy interface for D2XMuMu
        """
        return makeD2KMuMuSS(name
                             , kaonSel
                             , muonSel
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])
#
# Out of class
#####################################################
def makeD2PiPiPi(name, pionSel, muonSel, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ (pi+ pi-)
    """
    rhos=Selection("pre"+name,
									 Algorithm = CombineParticles(DecayDescriptor = "[rho(770)0 -> pi+ pi-]cc",MotherCut = "ALL"),
									 RequiredSelections=[muonSel] )

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ rho(770)0]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ pionSel, rhos ] )

#####################################################
def makeD2PiMuMuOS(name, pionSel, muonSel, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ mu+ mu- 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ pionSel, muonSel ] )

#####################################################
def makeD2PiMuMuSS(name, pionSel, muonSel, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> pi+ mu- mu-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> pi+ mu- mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ pionSel, muonSel ] )

#####################################################
def makeD2KMuMuOS(name, kaonSel, muonSel, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> K+ mu+ mu-
    """
    
    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, muonSel ] )

#####################################################
def makeD2KMuMuSS(name, kaonSel, muonSel, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> K+ mu- mu-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> K+ mu- mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, muonSel ] )
                    
#####################################################
def makeKaons(name, KaonP, KaonPT, KaonPIDK, KaonMINIPCHI2, KaonTRCHI2):
    """
    Kaon selection
    """
    _code = "(TRCHI2DOF < %(KaonTRCHI2)s) & "\
						"(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(PIDK-PIDpi > %(KaonPIDK)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseKaons ] )

#####################################################
def makePions(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2):
    """
    Pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
						"(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s *MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
                    
#####################################################
def makeMuons(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
						"(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseMuons ] )
                        
#####################################################
def makePionsAsMuons(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
						"(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
                    




'''


from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingD2XMuMuSSConf(LHCbConfigurableUser):
    """
    Definition of nominal D->Xu mu nu stripping
    """
    
    __slots__ = {
	"prefix"	: "",
        #Muons
        "MuonTRCHI2"    : 10.   ,#adimensional
        "MuonP"         : 3000. ,#MeV
        "MuonPT"        : 500. ,#MeV
#        "MuonPIDK"      : -5.   ,#adimensional
#        "MuonPIDmu"     : -5.   ,#adimensional
#        "MuonPIDp"      : -5.   ,#adimensional
        "MuonMINIPCHI2" : 5     ,#adminensional
        #Xu
        #Pi Channel
        "PionTRCHI2"    : 10.   ,#adimensional
        "PionP"         : 2000. ,#MeV
        "PionPT"        : 300.  ,#MeV
        "PionPIDK"      : -10.  ,#adimensional
#        "PionPIDmu"     : -10.  ,#adimensional
#        "PionPIDp"      : -10.  ,#adimensional
        "PionMINIPCHI2" : 9     ,#adminensional
        #K Channel
        "KaonTRCHI2"    : 10.   ,#adimensional
        "KaonP"         : 2000. ,#MeV
        "KaonPT"        : 300.  ,#MeV
        "KaonPIDK"      : -1.   ,#adimensional
        "KaonMINIPCHI2" : 9     ,#adminensional
        #D
        "DVCHI2DOF"     : 8   ,#adminensional
        "DFDCHI2"       : 64   ,#adminensional
        "DDIRA"         : 0.9999 ,#adimensional
        'DMassWin'      : 200,   # MeV, high mass window
        'DMassLow'      : 1760   # MeV, high mass window
        }


    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
      
    def _NominalPiSelection( self ):
        return "(TRCHI2DOF < %(PionTRCHI2)s )&  (P> %(PionP)s *MeV) &  (PT> %(PionPT)s *MeV)"\
               "& (PIDpi-PIDK> %(PionPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s )"
      
    def _NominalKSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def PiSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2PiMuMuSSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._D2PiMuMuSS()])
    def PiOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2PiMuMuOSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._D2PiMuMuOS()])

    def KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2KMuMuSSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._D2KMuMuSS()])
    
    def KOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2KMuMuOSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._D2KMuMuOS()])

    def _muonFilter( self ):
        from Configurables import FilterDesktop
        _mu = FilterDesktop(self.getProp("prefix") + "Mu_forD2XMuMu",
                            InputLocations = ["Phys/StdLooseMuons"])
        _mu.Code = self._NominalMuSelection() % self.getProps()
        return _mu

    def _pionFilter( self ):
        from Configurables import FilterDesktop
        _pi = FilterDesktop(self.getProp("prefix") + "Pi_forD2XMuMu",
                            InputLocations = ["Phys/StdLoosePions"])
        _pi.Code = self._NominalPiSelection() % self.getProps()
        return _pi

    def _kaonFilter( self ):
        from Configurables import FilterDesktop
        _ka = FilterDesktop(self.getProp("prefix") + "K_forD2XMuMu",
                            InputLocations = ["Phys/StdLooseKaons"])
        _ka.Code = self._NominalKSelection() % self.getProps()
        return _ka
    
    def _D2PiMuMuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMuMu = CombineParticles(self.getProp("prefix") + "D2PiMuMuSS")
        _PiMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
				  "Phys/" + self.getProp("prefix") + "Pi_forD2XMuMu"]
        _PiMuMu.DecayDescriptors = ["[D- -> pi+ mu- mu-]cc"]
        _PiMuMu.CombinationCut = " (AM>%(DMassLow)s*MeV) & (ADAMASS('D-') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
                          "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _PiMuMu.ReFitPVs = True
        return _PiMuMu

    def _D2PiMuMuOS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMuMu = CombineParticles(self.getProp("prefix") + "D2PiMuMuOS")
        _PiMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
				  "Phys/" + self.getProp("prefix") + "Pi_forD2XMuMu"]
        _PiMuMu.DecayDescriptors = ["[D+ -> pi+ mu+ mu-]cc"]
        _PiMuMu.CombinationCut = "(AM>%(DMassLow)s*MeV)& (ADAMASS('D+') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
                          "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _PiMuMu.ReFitPVs = True
        return _PiMuMu
    
    def _D2KMuMuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMuMu = CombineParticles(self.getProp("prefix") + "D2KMuMuSS")
        _KMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
				 "Phys/" + self.getProp("prefix") + "K_forD2XMuMu"]
        _KMuMu.DecayDescriptors = ["[D- -> K+ mu- mu-]cc"]
        _KMuMu.CombinationCut = "(AM>%(DMassLow)s*MeV)& (ADAMASS('D-') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
            "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _KMuMu.ReFitPVs = True
        return _KMuMu
    
    def _D2KMuMuOS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMuMu = CombineParticles(self.getProp("prefix") + "D2KMuMuOS")
        _KMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
				 "Phys/" + self.getProp("prefix") + "K_forD2XMuMu"]
        _KMuMu.DecayDescriptors = ["[D+ -> K+ mu+ mu-]cc"]
        _KMuMu.CombinationCut = " (AM>%(DMassLow)s*MeV)& (ADAMASS('D+') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
            "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _KMuMu.ReFitPVs = True
        return _KMuMu
    
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
        
'''