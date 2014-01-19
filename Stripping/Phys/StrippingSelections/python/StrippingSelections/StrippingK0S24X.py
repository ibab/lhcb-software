
__author__ = ['Marc-Olivier Bettler']
__date__ = '16/01/2014'
__version__ = '$Revision: 0.1 $'

'''
stripping code for Kshort --> MuMuPiPi and  Kshort --> MuMuMuMu
'''

config_params =  {#'MuonP'         : 3000. ,    #MeV
                  #'MuonPT'        : 500.  ,    #MeV
                  'MuonMINIPCHI2' : 36     ,    #adminensional
                  'MuonTRCHI2'    : 5     ,    #adminensional
                  
                  #'PionP'         : 2000. ,    #MeV
                  #'PionPT'        : 300.  ,    #MeV
                  'PionMINIPCHI2' : 36     ,    #adminensional
                  'PionTRCHI2'    : 5     ,    #adminensional
                  

                  #4body
                  #'MINIPCHI2_hhmumu' : 4. ,  #adminensional --  for the individual pions
                  #'PT_hhmumu'        : 300 , #MeV
                  #'MuonPIDmu_hhmumu' : -1,   #adimensional
                  #'DPT_hhmumu'           : 2500.,     #MeV
                  #'DVCHI2DOF_hhmumu'     : 8     ,    #adminensional         
                  #'DMAXDOCA_hhmumu'      : 0.2  ,    #mm
                  #'KsDauMAXIPCHI2_hhmumu' : 15    ,    #adimensinal
                  #'DFDCHI2_hhmumu'       : 9     ,   #adimensional
                  #'DIPCHI2_hhmumu'       : 20    ,    #adimensional


                  #'DDIRA'         : 0.9999,    #adimensional
                  #'MaxDimuonMass'    : 260.  ,    #MeV
                  #'MaxKsMass'                   : 550.  ,    #MeV, comb mass window
                  #'KsMotherMassCut'             : 540.  ,    # MeV, mother mass cut
                  
                  'hhmm_MaxDimuonMass' : 400. , # MeV
                  'hhmm_KS0_MassWin' : 1500., #MeV
                  'hhmm_DMAXDOCA' : 0.8, # mm
                  'hhmm_DIRA' : 0., #adimensional
                  'hhmm_KsMotherMass_min' : 400. , #MeV
                  'hhmm_MinIP_dist' : 1., #mm
                  'hhmm_lifetimeCut': 0.05*89.53*2.9979e-01, #
                  
                  'mmmm_MaxDimuonMass' : 400. , # MeV
                  'mmmm_KS0_MassWin' : 1500., #MeV
                  'mmmmL_DMAXDOCA' : 10., # mm
                  'mmmmD_DMAXDOCA' : 20., # mm
                  'mmmm_DIRA' : 0., #adimensional
                  'mmmm_KsMotherMass_min' : 400. , #MeV
                  'mmmmL_MinIP_dist' : 20., #mm
                  'mmmmD_MinIP_dist' : 20., #mm
                  'mmmm_lifetimeCut': 0.05*89.53*2.9979e-01, #
                  
                  'K0S2PiPiMuMuLinePrescale'  : 1 ,
                  'K0S2PiPiMuMuLinePostscale'  : 1 ,
                  'K0S24MuLLinePrescale'  : 1 ,
                  'K0S24MuLLinePostscale'  : 1 ,
                  'K0S24MuDLinePrescale'  : 1 ,
                  'K0S24MuDLinePostscale'  : 1 ,
                  'K0S24XLLinePrescale'  : 1 ,
                  'K0S24XLLinePostscale'  : 1 ,
                  'K0S24XDLinePrescale'  : 1 ,
                  'K0S24XDLinePostscale'  : 1 
                  }

__all__ = ('Kshort2PiPiMuMuConf',
           'Kshort2MuMuMuMuConf',
           'config_params')


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdLoosePions, StdLooseMuons, StdNoPIDsPions, StdLooseDownMuons, StdNoPIDsDownPions
from StandardParticles import  StdAllLoosePions, StdAllLooseMuons, StdAllNoPIDsPions

default_name = "K0S2"

class K0S24XConf(LineBuilder) :
    """
    Builder for Kshort --> pi,pi,mu,mu and  Kshort --> mu,mu,mu,mu
    """
    
        
    __configuration_keys__ = (    'MuonMINIPCHI2'
                                , 'MuonTRCHI2'
                                , 'PionMINIPCHI2'
                                , 'PionTRCHI2'
                                
                                #4body
                                , 'hhmm_MaxDimuonMass'
                                , 'hhmm_KS0_MassWin'
                                , 'hhmm_DMAXDOCA'
                                , 'hhmm_DIRA'
                                , 'hhmm_KsMotherMass_min'
                                , 'hhmm_MinIP_dist'
                                , 'hhmm_lifetimeCut'
                                , 'mmmm_MaxDimuonMass'
                                , 'mmmm_KS0_MassWin'
                                , 'mmmmL_DMAXDOCA'
                                , 'mmmmD_DMAXDOCA'
                                , 'mmmm_DIRA'
                                , 'mmmm_KsMotherMass_min'
                                , 'mmmmL_MinIP_dist'
                                , 'mmmmD_MinIP_dist'
                                , 'mmmm_lifetimeCut'
                                
                                , 'K0S2PiPiMuMuLinePrescale'
                                , 'K0S2PiPiMuMuLinePostscale'
                                , 'K0S24MuLLinePrescale'
                                , 'K0S24MuLLinePostscale'
                                , 'K0S24MuDLinePrescale'
                                , 'K0S24MuDLinePostscale'
                                , 'K0S24XLLinePrescale'
                                , 'K0S24XLLinePostscale'
                                , 'K0S24XDLinePrescale'
                                , 'K0S24XDLinePostscale'
                                )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        

        base_name = name
        
        # 5 : Make Muons L and D 
        selMuonsForXXMuMu = makeLongMuonsForXXMuMu(name="MuonsFor"+name
                                               , MuonMINIPCHI2 = config['MuonMINIPCHI2']
                                               , MuonTRCHI2 = config['MuonTRCHI2'])
        
        selDownstreamMuonssForMuMuMuMu = makeDownstreamMuonsForMuMuMuMu(name="DownMuonsFor"+name
                                               , MuonMINIPCHI2 = config['MuonMINIPCHI2']
                                               , MuonTRCHI2 = config['MuonTRCHI2'])


        # 7 : Make Pions
        selLPions = makeLPionsForXXXX(name="PionsFor"+name
                                      , PionMINIPCHI2 = config['PionMINIPCHI2']
                                      , PionTRCHI2 = config['PionTRCHI2'])
        
        selDPions = makeDPionsForXXXX(name="DownPionsFor"+name
                                      , PionMINIPCHI2 = config['PionMINIPCHI2']
                                      , PionTRCHI2 = config['PionTRCHI2'])

                                        
        # 7 : Combine
        
        selK0S2PiPiMuMu = self._makeKshort2PiPiMuMu(name=base_name+'PiPiMuMu',
                                           pionSel = selLPions,
                                           muonSel = selMuonsForXXMuMu,
                                           config = config)

        selK0S2MuMuMuMuL = self._makeKshort24MuL(name=base_name+'MuMuMuMuL',
                                      muonSel = selMuonsForXXMuMu,
                                      config = config)

        selK0S2MuMuMuMuD = self._makeKshort24MuD(name=base_name+'MuMuMuMuD',
                                           muonSel = selDownstreamMuonssForMuMuMuMu,
                                           config = config)
        
        selK0S2XXXXL = self._makeKshort24MuL(name=base_name+'XXXXL',
                                           muonSel = selLPions,
                                           config = config)
        
        selK0S2XXXXD = self._makeKshort24MuD(name=base_name+'XXXXD',
                                           muonSel = selDPions,
                                           config = config)
        
        # 8 : Declare Lines
        
        self.PiPi2MuLine = StrippingLine(base_name+'PiPiMuMuLine',
                                    prescale = config['K0S2PiPiMuMuLinePrescale'],
                                    postscale = config['K0S2PiPiMuMuLinePostscale'],
                                    selection = selK0S2PiPiMuMu
                                    )

        self.FourMuLongLine = StrippingLine(base_name+'MuMuMuMuLLine',
                                    prescale = config['K0S24MuLLinePrescale'],
                                    postscale = config['K0S24MuLLinePostscale'],
                                    selection = selK0S2MuMuMuMuL
                                    )
        
        self.FourMuDownLine = StrippingLine(base_name+'MuMuMuMuDLine',
                                    prescale = config['K0S24MuDLinePrescale'],
                                    postscale = config['K0S24MuDLinePostscale'],
                                    selection = selK0S2MuMuMuMuD
                                    )
        
        self.FourXLongLine = StrippingLine(base_name+'XXXXLLine',
                                    prescale = config['K0S24XLLinePrescale'],
                                    postscale = config['K0S24XLLinePostscale'],
                                    selection = selK0S2XXXXL
                                    )
        
        self.FourXDownLine = StrippingLine(base_name+'XXXXDLine',
                                    prescale = config['K0S24XDLinePrescale'],
                                    postscale = config['K0S24XDLinePostscale'],
                                    selection = selK0S2XXXXD
                                    )
        

        # 9 : register Line

        self.registerLine( self.PiPi2MuLine )
        self.registerLine( self.FourMuLongLine )
        self.registerLine( self.FourMuDownLine )
        self.registerLine( self.FourXLongLine )
        self.registerLine( self.FourXDownLine )



    def _makeKshort24MuL(self, name,  muonSel, config):
        """
        Handy interface for Kshort2MuMuMuMu
        """
        return makeKshort2MuMuMuMu(name
                                   , muonSel
                                   , KS0_MassWin = config ['mmmm_KS0_MassWin']
                                   , MaxDimuonMass =  config['mmmm_MaxDimuonMass']
                                   , DMAXDOCA =  config['mmmmL_DMAXDOCA']
                                   , DIRA = config ['mmmm_DIRA']
                                   , lifetimeCut =  config['mmmm_lifetimeCut']
                                   , MinIP_dist =  config['mmmmL_MinIP_dist']
                                   , KsMotherMass_min =  config['mmmm_KsMotherMass_min'])

    def _makeKshort24MuD(self, name,  muonSel, config):
        """
        Handy interface for Kshort2MuMuMuMu
        """
        return makeKshort2MuMuMuMu(name
                                   , muonSel
                                   , KS0_MassWin = config ['mmmm_KS0_MassWin']
                                   , MaxDimuonMass =  config['mmmm_MaxDimuonMass']
                                   , DMAXDOCA =  config['mmmmD_DMAXDOCA']
                                   , DIRA = config ['mmmm_DIRA']
                                   , lifetimeCut =  config['mmmm_lifetimeCut']
                                   , MinIP_dist =  config['mmmmD_MinIP_dist']
                                   , KsMotherMass_min =  config['mmmm_KsMotherMass_min'])


    def _makeKshort2PiPiMuMu(self, name, pionSel, muonSel, config):
        """
        Handy interface for Kshort2PiPiMuMu
        """
        return makeKshort2PiPiMuMu(name
                                   , pionSel
                                   , muonSel
                                   , KS0_MassWin =  config['hhmm_KS0_MassWin']
                                   , MaxDimuonMass =  config['hhmm_MaxDimuonMass']
                                   , DMAXDOCA =  config['hhmm_DMAXDOCA']
                                   , DIRA = config['hhmm_DIRA']
                                   , lifetimeCut =  config['hhmm_lifetimeCut']
                                   , MinIP_dist =  config['hhmm_MinIP_dist']
                                   , KsMotherMass_min =  config['hhmm_KsMotherMass_min']
                                   )



def makeKshort2MuMuMuMu(name, muonSel, KS0_MassWin, MaxDimuonMass, DMAXDOCA, DIRA
                        , lifetimeCut, MinIP_dist, KsMotherMass_min):

    """
    Makes the KS0 -> mu+ mu+ mu- mu-
    """
    

    _combcut = "(ADAMASS('KS0') < %(KS0_MassWin)s *MeV) & "\
               "(AM12 < %(MaxDimuonMass)s *MeV) & "\
               "(AM13 < %(MaxDimuonMass)s *MeV) & "\
               "(AM14 < %(MaxDimuonMass)s *MeV) & "\
               "(AM23 < %(MaxDimuonMass)s *MeV) & "\
               "(AM24 < %(MaxDimuonMass)s *MeV) & "\
               "(AM34 < %(MaxDimuonMass)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s)"%locals()

    #print _combcut
    _mothercut   = "(BPVDIRA > %(DIRA)s) & "\
                   "((BPVVDSIGN*M/P) > %(lifetimeCut)s ) & "\
                   "(MIPDV(PRIMARY)< %(MinIP_dist)s *mm) & "\
                   "(M > %(KsMotherMass_min)s *MeV) " %locals()

    #print _mothercut

    _Combine = CombineParticles(DecayDescriptor = "KS0 -> mu+ mu+ mu- mu-",
                                CombinationCut = _combcut,
                                MotherCut = _mothercut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel] )



def makeKshort2PiPiMuMu(name, pionSel, muonSel, KS0_MassWin, MaxDimuonMass, DMAXDOCA, DIRA
                        , lifetimeCut, MinIP_dist, KsMotherMass_min):

    """
    Makes the KS0 -> pi+ pi- mu+ mu-
    """

    _combcut = "(ADAMASS('KS0') < %(KS0_MassWin)s *MeV) & "\
               "(AM34 < %(MaxDimuonMass)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s)" %locals()

    ## _combcut = "(AM < %(MaxKsMass)s *MeV) & "\
    ##            "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
    ##            "(AM34 < %(MaxDimuonMass)s *MeV) &"\
    ##            "(AHASCHILD( (MIPCHI2DV(PRIMARY)> %(MinIP_dist_M)s)  )   )" %locals()

    _mothercut   = "(BPVDIRA > %(DIRA)s) & "\
                   "((BPVVDSIGN*M/P) > %(lifetimeCut)s ) & "\
                   "(MIPDV(PRIMARY)< %(MinIP_dist)s *mm) & "\
                   "(M > %(KsMotherMass_min)s *MeV) " %locals()

    ## _mothercut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
    ##                "(PT > %(DPT)s *MeV) &"\
    ##                "(M < %(KsMotherMassCut)s *MeV) &"\
    ##                "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
    ##                "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "KS0 -> pi+ pi- mu+ mu-",
                                CombinationCut = _combcut,
                                MotherCut = _mothercut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, pionSel] )                    


def makeLPionsForXXXX(name, PionMINIPCHI2, PionTRCHI2):
    """
    Pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsPions ] )

def makeDPionsForXXXX(name, PionMINIPCHI2, PionTRCHI2):
    """
    Pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdNoPIDsDownPions ] )


def makeLongMuonsForXXMuMu(name, MuonMINIPCHI2, MuonTRCHI2):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLooseMuons ] )



def makeDownstreamMuonsForMuMuMuMu(name, MuonMINIPCHI2, MuonTRCHI2):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseDownMuons ] )


