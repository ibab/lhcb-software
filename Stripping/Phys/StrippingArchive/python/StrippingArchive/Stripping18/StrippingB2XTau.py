
__author__ = ['V. Gligorov']
__date__ = '24/05/2010'
__version__ = '$Revision: 1.0 $'

"""
B->TauTau, B->TauMu
"""

config_params =  {  'PT_HAD_ALL_FINAL_STATE'        : '200',  # MeV
                    'P_HAD_ALL_FINAL_STATE'         : '2000', # MeV
                    'IPCHI2_HAD_ALL_FINAL_STATE'    : '9',    # dimensionless
                    'TRACKCHI2_HAD_ALL_FINAL_STATE' : '4',    # dimensionless
                    #
                    'PT_MU'                         : '800',  # MeV
                    'P_MU'                          : '6000', # MeV 
                    'IPCHI2_MU'                     : '16',    # MeV   
                    #
                    'PT_B_TT'                       : '5000', # MeV
                    'PT_B_TT_HIGH'                  : '10000', # MeV 
                    'PT_B_TM'                       : '2000', # MeV
                    'PT_B_TM_HIGH'                  : '7500', # MeV  
                    'VCHI2_B'                       : '100',  # dimensionless
                    'FDCHI2_B'                      : '144',  # dimensionless
                    'DIRA_B'                        : '0.99', # dimensionless
                    'MASS_LOW_B'                    : '2000', # MeV  
                    'MASS_HIGH_B'                   : '5750', # MeV
                    'MCOR_LOW_B'                    : '4000', # MeV
                    'MCOR_HIGH_B'                   : '7000', # MeV
                    'MIPCHI2_B'                     : '150',  # dimensionless 
                    'MIPCHI2_B_HIGH'                : '36',   # dimensionless  
                    #
                    'PT_TAU'                        : '1500', # MeV
                    'VCHI2_TAU'                     : '20',   # dimensionless
                    'IPCHI2_TAU'                    : '9',    # dimensionless
                    'FDCHI2_TAU'                    : '144',  # dimensionless
                    'MASS_LOW_TAU'                  : '700',  # MeV
                    'MASS_HIGH_TAU'                 : '1800', # MeV
                    #
                    'PT_B_CHILD_BEST'               : '1800', # MeV
                    'P_B_CHILD_BEST'                : '10000',# MeV
                    'IPCHI2_B_CHILD_BEST'           : '16',   # dimensionless
                    'PT_B_TAU_CHILD_BEST'           : '3000', # MeV
                    'IPCHI2_B_TAU_CHILD_BEST'       : '16',   # dimensionless
                    #
                    'MASS_LOW_D'                    : '1800', # MeV
                    'MASS_HIGH_D'                   : '2030', # MeV  
                    #
                    'B2TauTau_TOSLinePrescale'          : 1,
                    'B2TauTau_TOSLinePostscale'         : 1,
                    'B2DD_TOSLinePrescale'              : 1,
                    'B2DD_TOSLinePostscale'             : 1,
                    'B2TauMu_TOSLinePrescale'           : 1,
                    'B2TauMu_TOSLinePostscale'          : 1,
                    'B2DMu_TOSLinePrescale'             : 0.2,
                    'B2DMu_TOSLinePostscale'            : 1,
                    'B2TauTau_TISLinePrescale'          : 1,
                    'B2TauTau_TISLinePostscale'         : 1,
                    'B2DD_TISLinePrescale'              : 1,
                    'B2DD_TISLinePostscale'             : 1,
                    'B2TauMu_TISLinePrescale'           : 1,
                    'B2TauMu_TISLinePostscale'          : 1,
                    'B2DMu_TISLinePrescale'             : 0.2,
                    'B2DMu_TISLinePostscale'            : 1.}

__all__ = ('B2XTauConf')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsKaons, StdNoPIDsPions, StdLooseMuons
from StandardParticles import StdLoosePions,StdTightPions
from StandardParticles import StdLooseKaons

default_name = "B2XTau"
HLT_DECISIONS_HAD   = "Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision"
HLT_DECISIONS_MUON  = "Hlt2(TopoMu|SingleMuon).*Decision"
HLT1_DECISIONS_TIS  = "Hlt1TrackAllL0Decision"
HLT2_DECISIONS_TIS  = "Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision"

class B2XTauConf(LineBuilder) :
    """
    Builder for B->TauTau, B->TauMu
    """
    
    TauTau_TOSLine  = None
    TauMu_TOSLine   = None
    DMu_TOSLine     = None    
    DD_TOSLine      = None     
   
    TauTau_TISLine  = None
    TauMu_TISLine   = None
    DMu_TISLine     = None
    DD_TISLine      = None

    __configuration_keys__ = (  'PT_HAD_ALL_FINAL_STATE',
                                'P_HAD_ALL_FINAL_STATE',
                                'IPCHI2_HAD_ALL_FINAL_STATE',
                                'TRACKCHI2_HAD_ALL_FINAL_STATE',
                                #
                                'PT_MU',            
                                'P_MU',
                                'IPCHI2_MU',
                                #   
                                'PT_B_TT',
                                'PT_B_TT_HIGH',
                                'PT_B_TM',
                                'PT_B_TM_HIGH',
                                'VCHI2_B' ,
                                'FDCHI2_B',
                                'DIRA_B',
                                'MASS_LOW_B',
                                'MASS_HIGH_B',
                                'MCOR_LOW_B',
                                'MCOR_HIGH_B',
                                'MIPCHI2_B',
                                'MIPCHI2_B_HIGH',
                                #   
                                'PT_TAU',
                                'VCHI2_TAU',
                                'IPCHI2_TAU',
                                'FDCHI2_TAU',
                                'MASS_LOW_TAU',
                                'MASS_HIGH_TAU',
                                #
                                'PT_B_CHILD_BEST',
                                'P_B_CHILD_BEST',
                                'IPCHI2_B_CHILD_BEST',   
                                'PT_B_TAU_CHILD_BEST', 
                                'IPCHI2_B_TAU_CHILD_BEST',
                                #
                                'MASS_LOW_D',
                                'MASS_HIGH_D',
                                #
                                'B2TauTau_TOSLinePrescale',
                                'B2TauTau_TOSLinePostscale',
                                'B2DD_TOSLinePrescale',
                                'B2DD_TOSLinePostscale',
                                'B2TauMu_TOSLinePrescale',
                                'B2TauMu_TOSLinePostscale',
                                'B2DMu_TOSLinePrescale',
                                'B2DMu_TOSLinePostscale',
                                'B2TauTau_TISLinePrescale',
                                'B2TauTau_TISLinePostscale',
                                'B2DD_TISLinePrescale',
                                'B2DD_TISLinePostscale',
                                'B2TauMu_TISLinePrescale',
                                'B2TauMu_TISLinePostscale',
                                'B2DMu_TISLinePrescale',
                                'B2DMu_TISLinePostscale'  
                              )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        #
        selPions        = self._makePions(      name    = "PionsFor"+name, 
                                                config  = config)
        selKaons        = self._makeKaons(      name    = "KaonsFor"+name,
                                                config  = config)
        selMuons        = self._makeMuons(      name    = "MuonsFor"+name,
                                                config  = config)
        #
        selTau          = self._makeTau(        name    = "TauFor"+name, 
                                                pionSel = selPions, 
                                                config  = config)
        selD            = self._makeD(          name    = "DFor"+name,
                                                pionSel = selPions,
                                                kaonSel = selKaons,
                                                config  = config)
        #
        selB2TauTau,selB2DD = self._makeB2XX(   name    = name,
                                                tauSel  = selTau,
                                                DSel    = selD,    
                                                config  = config)
        selB2TauMu,selB2DMu = self._makeB2XMu(  name    = name,
                                                tauSel  = selTau,
                                                DSel    = selD,
                                                muonSel = selMuons,
                                                config  = config)
        #
        self.TauTau_TOSLine = StrippingLine(name+"_TauTau_TOSLine",
                                            #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                            prescale    = config['B2TauTau_TOSLinePrescale'],
                                            postscale   = config['B2TauTau_TOSLinePostscale'],
                                            selection   = self._makeTOS(name+"_TOSForTauTau",selB2TauTau)
                                      )
        self.DD_TOSLine     = StrippingLine(name+"_DD_TOSLine",
                                            #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                            prescale    = config['B2DD_TOSLinePrescale'],
                                            postscale   = config['B2DD_TOSLinePostscale'],
                                            selection   = self._makeTOS(name+"_TOSForDD",selB2DD)
                                      )
        self.TauMu_TOSLine  = StrippingLine(name+"_TauMu_TOSLine",
                                            #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                            prescale    = config['B2TauMu_TOSLinePrescale'],
                                            postscale   = config['B2TauMu_TOSLinePostscale'],
                                            selection   = self._makeTOS(name+"_TOSForTauMu",selB2TauMu)
                                      ) 
        self.DMu_TOSLine    = StrippingLine(name+"_DMu_TOSLine",
                                            #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                            prescale    = config['B2DMu_TOSLinePrescale'],
                                            postscale   = config['B2DMu_TOSLinePostscale'],
                                            selection   = self._makeTOS(name+"_TOSForDMu",selB2DMu)
                                      )
        #
        self.TauTau_TISLine = StrippingLine(name+"_TauTau_TISLine",
                                            #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                            prescale    = config['B2TauTau_TISLinePrescale'],
                                            postscale   = config['B2TauTau_TISLinePostscale'],
                                            selection   = self._makeTIS(name+"_TISForTauTau",selB2TauTau)
                                      )
        self.DD_TISLine     = StrippingLine(name+"_DD_TISLine",
                                            #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                            prescale    = config['B2DD_TISLinePrescale'],
                                            postscale   = config['B2DD_TISLinePostscale'],
                                            selection   = self._makeTIS(name+"_TISForDD",selB2DD)
                                      )
        self.TauMu_TISLine  = StrippingLine(name+"_TauMu_TISLine",
                                            #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                            prescale    = config['B2TauMu_TISLinePrescale'],
                                            postscale   = config['B2TauMu_TISLinePostscale'],
                                            selection   = self._makeTIS(name+"_TISForTauMu",selB2TauMu)
                                      ) 
        self.DMu_TISLine    = StrippingLine(name+"_DMu_TISLine",
                                            #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                            prescale    = config['B2DMu_TISLinePrescale'],
                                            postscale   = config['B2DMu_TISLinePostscale'],
                                            selection   = self._makeTIS(name+"_TISForDMu",selB2DMu)
                                      )   
        #
        self.registerLine( self.TauTau_TOSLine )
        self.registerLine( self.DD_TOSLine )
        self.registerLine( self.TauMu_TOSLine )
        self.registerLine( self.DMu_TOSLine )    
        self.registerLine( self.TauTau_TISLine )
        self.registerLine( self.DD_TISLine )
        self.registerLine( self.TauMu_TISLine )
        self.registerLine( self.DMu_TISLine ) 

    #####################################################
    def _makeB2XX(self, name, tauSel, DSel, config):
   
        preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                     "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
 
        _combcut = "(APT > "                    + config['PT_B_TT']       + "*MeV) & "\
                   "(AM  > "                    + config['MASS_LOW_B']    + "*MeV) & "\
                   "(AM  < "                    + config['MASS_HIGH_B']   + "*MeV)"

        _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
                   "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
                   "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
                   "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
                   "((PT                >   "   + config['PT_B_TT_HIGH']  + "*MeV) | \
                     (BPVIPCHI2()       <   "   + config['MIPCHI2_B_HIGH']+ ")) &"\
                   "(INGENERATION((PT   >   "   + config['PT_B_TAU_CHILD_BEST']+ "*MeV),1)) & "\
                   "(INGENERATION((MIPCHI2DV(PRIMARY) >" + config['IPCHI2_B_TAU_CHILD_BEST'] + "),1)) & "\
                   "(INGENERATION((PT   >   "   + config['PT_B_CHILD_BEST']+ "*MeV),2)) & "\
                   "(INGENERATION((P    >   "   + config['P_B_CHILD_BEST'] + "*MeV),2)) & "\
                   "(INGENERATION((MIPCHI2DV(PRIMARY) >" + config['IPCHI2_B_CHILD_BEST'] + "),2)) & "\
                   "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV))"      
    
        _CombineTau = CombineParticles( DecayDescriptors = ["B0 -> tau+ tau-"],
                                        CombinationCut   = _combcut,
                                        MotherCut        = _bcut,
                                        Preambulo        = preambulo)
        
        _CombineD   = CombineParticles( DecayDescriptors = ["B0 -> D+ D-"],
                                        CombinationCut   = _combcut + " & (AM > 5000)",
                                        MotherCut        = _bcut,
                                        Preambulo        = preambulo)

        return (Selection(name+"_TauTau",
                         Algorithm          = _CombineTau,
                         RequiredSelections = [ tauSel ] ),
                Selection(name+"_DD",
                         Algorithm          = _CombineD,
                         RequiredSelections = [ DSel ] ))

    #####################################################
    def _makeB2XMu(self, name, tauSel, DSel, muonSel, config):
   
        preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                     "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
 
        _combcut = "(APT > "                    + config['PT_B_TM']       + "*MeV) & "\
                   "(AM  > "                    + config['MASS_LOW_B']    + "*MeV) & "\
                   "(AM  < "                    + config['MASS_HIGH_B']   + "*MeV)"

        _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
                   "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
                   "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
                   "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
                   "((PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) | \
                     (BPVIPCHI2()       <   "   + config['MIPCHI2_B_HIGH']+ "))&"\
                   "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV))"    
    
        _CombineTau = CombineParticles( DecayDescriptors = ["[B0 -> tau+ mu-]cc"],
                                        CombinationCut   = _combcut,
                                        MotherCut        = _bcut,
                                        Preambulo        = preambulo)
     
        _CombineD   = CombineParticles( DecayDescriptors = ["[B0 -> D+ mu-]cc"],
                                        CombinationCut   = _combcut,
                                        MotherCut        = _bcut,
                                        Preambulo        = preambulo)

        return (Selection(name+"_TauMu",
                         Algorithm          = _CombineTau,
                         RequiredSelections = [ tauSel, muonSel ] ),
                Selection(name+"_DMu",
                         Algorithm          = _CombineD,
                         RequiredSelections = [ DSel, muonSel ] ))

    #####################################################
    def _makeTau(self, name, pionSel, config) :
        """
        Tau selection
        """
        getcuts = self._hadIntermediateKinematicCuts(config)

        _combcut = getcuts[0] +\
                   "(AM      > "    + config['MASS_LOW_TAU']         + "*MeV)    & "\
                   "(AM      < "    + config['MASS_HIGH_TAU']        + "*MeV)      "

        _taucut  = getcuts[1]

        _Combine = CombineParticles(DecayDescriptor = "[tau+ -> pi+ pi+ pi-]cc ",
                                    CombinationCut  = _combcut,
                                    MotherCut       = _taucut)   
 
        return Selection(name,
                         Algorithm          = _Combine,
                         RequiredSelections = [ pionSel ] )    

    #####################################################
    def _makeD(self, name, pionSel, kaonSel, config) :
        getcuts = self._hadIntermediateKinematicCuts(config)

        _combcut = getcuts[0] +\
                   "(AM      > "    + config['MASS_LOW_D']         + "*MeV)    & "\
                   "(AM      < "    + config['MASS_HIGH_D']        + "*MeV)      "

        _dcut  = getcuts[1]

        _Combine = CombineParticles(DecayDescriptors = ["[D+ -> K- pi+ pi+]cc ",
                                                        "[D+ -> K- K+  pi+]cc "],
                                    CombinationCut   = _combcut,
                                    MotherCut        = _dcut)

        return Selection(name,
                         Algorithm          = _Combine,
                         RequiredSelections = [ pionSel, kaonSel ] )

    #####################################################
    def _makePions(self, name, config) :
        """
        Pion selection
        """
        _code = self._hadFinalStateKinematicCuts(config) +"& ~(ISMUON)"#+ " & (PIDp < 5) & (PIDe < 5)"

        _Filter = FilterDesktop(Code = _code)
    
        return Selection(name,
                         Algorithm          = _Filter,
                         RequiredSelections = [ StdTightPions ] )

    #####################################################
    def _makeKaons(self, name, config) :
        """ 
        Kaon selection
        """
        _code = self._hadFinalStateKinematicCuts(config) +"& ~(ISMUON)"#+ " & ((PIDK-PIDe) > -5)" 

        _Filter = FilterDesktop(Code = _code)
    
        return Selection(name,
                         Algorithm          = _Filter,
                         RequiredSelections = [ StdLooseKaons ] )

    #####################################################
    def _makeMuons(self, name, config) :
        """                     
        Muon selection          
        """                     
        _code = self._muFinalStateKinematicCuts(config) 
                                
        _Filter = FilterDesktop(Code = _code)
                                
        return Selection(name,  
                         Algorithm          = _Filter,
                         RequiredSelections = [ StdLooseMuons ] )    

    #####################################################   
    def _hadIntermediateKinematicCuts(self, config) : 
        _combcode   = "(APT     > "                + config['PT_TAU']       + "*MeV) & "
        _postvxcode = "(MIPCHI2DV(PRIMARY) > "     + config['IPCHI2_TAU']   + ")     & "\
                      "(VFASPF(VCHI2PDOF)  < "     + config['VCHI2_TAU']    + ")     & "\
                      "(BPVVDCHI2          > "     + config['FDCHI2_TAU']   + ")       "
        return [_combcode,_postvxcode]    

    #####################################################   
    def _hadFinalStateKinematicCuts(self, config) :
        _code = "(P  > "                    + config['P_HAD_ALL_FINAL_STATE']           + "*MeV) & "\
                "(PT > "                    + config['PT_HAD_ALL_FINAL_STATE']          + "*MeV) & "\
                "(MIPCHI2DV(PRIMARY) > "    + config['IPCHI2_HAD_ALL_FINAL_STATE']      + ")     & "\
                "((TRCHI2DOF)        < "    + config['TRACKCHI2_HAD_ALL_FINAL_STATE']   + ")       "
        return _code
    
    #####################################################   
    def _muFinalStateKinematicCuts(self, config) :
        _code = "(P  > "                    + config['P_MU']           + "*MeV) & "\
                "(PT > "                    + config['PT_MU']          + "*MeV) & "\
                "(MIPCHI2DV(PRIMARY) > "    + config['IPCHI2_MU']      + ")       "
        return _code

    #####################################################
    # TISTOSING
    #####################################################
    def _makeTISTOSFilter(self,name,specs):
        from Configurables import TisTosParticleTagger
        tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
        tisTosFilter.TisTosSpecs = specs
        tisTosFilter.ProjectTracksToCalo = False
        tisTosFilter.CaloClustForCharged = False
        tisTosFilter.CaloClustForNeutral = False
        tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
        return tisTosFilter
    
    def _makeTOS(self, name, sel):
        ''' TOS filters selections'''
        if ((name.find('TauMu') > -1) or (name.find('DMu') > -1)):
            tisTosFilter = self._makeTISTOSFilter(name,{HLT_DECISIONS_MUON+'%TOS':0})
        else :
            tisTosFilter = self._makeTISTOSFilter(name,{HLT_DECISIONS_HAD+'%TOS':0})
        return Selection(name, Algorithm=tisTosFilter, RequiredSelections=[sel])
    
    def _makeTIS(self, name, sel):
        ''' TIS filters selections'''
        tisTosFilter = self._makeTISTOSFilter(name,
                                        {HLT1_DECISIONS_TIS+'%TIS':0,HLT2_DECISIONS_TIS+'%TIS':0})
        return Selection(name, Algorithm=tisTosFilter, RequiredSelections=[sel])
