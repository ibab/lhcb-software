"""
Stripping options for selecting liftime unbiased charm

Authors: Vladimir Gligorov
"""

########################################################################
__author__ = ['Vladimir Gligorov']
__date__ = '14/07/2014'
__version__ = '$Revision: 1.0 $'

__all__ = ('LTUnbCharmLines',
           'makeLTUnbCharm',
           'applyBDT',
           'default_config')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles                     import StdAllNoPIDsPions,StdAllNoPIDsKaons,StdAllNoPIDsProtons

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig


default_config = {
    'NAME'        : 'LTUnbCharm',
    'WGs'         : ['Charm'],
    'BUILDERTYPE' : 'LTUnbCharmLines',
    'CONFIG'      : { 'PrescaleLTUnbCharmD2KPiTISLine' : 1.,
                      'PrescaleLTUnbCharmD2KPiTOSLine' : 1.,
                      'PrescaleLTUnbCharmD2KPiPiLine'  : 1.,
                      'PrescaleLTUnbCharmDs2KKPiLine'  : 1.,
                      'PrescaleLTUnbCharmLc2pKPiLine'  : 1.,
                      'MinPT3Body'         : 500.,
                      'MinPT2Body'         : 800.,
                      'PIDK_pi'            : 0,
                      'PIDK_k'             : 5,
                      'PIDp_pi'            : 0,
                      'PIDp_p'             : 5,  
                      'TrChi2'             : 3,
                      'TrGhostProb'        : 0.3,
                      'MassWindow'         : 65,
                      'DOCA'               : 0.1,
                      'VTXCHI2'            : 10,
                      'DPT'                : 2500,
                      'DDIRA'              : 0.98,
                      'DTAU'               : 0.00025
                    },
    'STREAMS'     : [ 'Charm' ]
    }

## Change decay descriptor and re-fit decay tree

class LTUnbCharmLines( LineBuilder ) :
    """Class defining the Hb -> hh stripping lines"""
    
    __configuration_keys__ = ( 'PrescaleLTUnbCharmD2KPiTISLine',
                               'PrescaleLTUnbCharmD2KPiTOSLine', 
                               'PrescaleLTUnbCharmD2KPiPiLine',
                               'PrescaleLTUnbCharmDs2KKPiLine',
                               'PrescaleLTUnbCharmLc2pKPiLine',
                               'MinPT3Body',        
                               'MinPT2Body',        
                               'PIDK_pi',           
                               'PIDK_k',            
                               'PIDp_pi',           
                               'PIDp_p',              
                               'TrChi2',            
                               'TrGhostProb',       
                               'MassWindow',        
                               'DOCA',              
                               'VTXCHI2',
                               'DPT',               
                               'DDIRA',             
                               'DTAU',              
                               )
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        LTUnbCharmName   = "LTUnbCharm"
        
        # make the various stripping selections
        self.LTUnbCharm = makeLTUnbCharm( LTUnbCharmName,
                                    config['MinPT3Body'],
                                    config['MinPT2Body'],
                                    config['PIDK_pi'],
                                    config['PIDK_k'],
                                    config['PIDp_pi'],
                                    config['PIDp_p'],
                                    config['TrChi2'],
                                    config['TrGhostProb'],
                                    config['MassWindow'],
                                    config['DOCA'],
                                    config['VTXCHI2'],
                                    config['DPT'],
                                    config['DDIRA'],
                                    config['DTAU']
                                  )
        
        for thisline in ["D2KPiTISLine","D2KPiTOSLine","D2KPiPiLine","Ds2KKPiLine","Lc2pKPiLine"] :
            self.lineLTUnbCharm = StrippingLine( LTUnbCharmName+thisline,
                                                 prescale  = config['Prescale'+LTUnbCharmName+thisline],
                                                 selection = self.LTUnbCharm[thisline],
                                                 EnableFlavourTagging = False,
                                                 RequiredRawEvents = ["Trigger"],
                                                 MDSTFlag = True)
        
            self.registerLine(self.lineLTUnbCharm)

def makeLTUnbCharm( name,
                    MinPT3Body,MinPT2Body,
                    PIDK_pi,PIDK_k,PIDp_pi,PIDp_p,
                    TrChi2,TrGhostProb,
                    MassWindow, DOCA, VTXCHI2,
                    DPT,DDIRA,DTAU) : 

    mybasics = {}
    
    mybasics["pion_2b"] = { "RequiredSelections" : [StdAllNoPIDsPions],
                            "BaseName" : name+'Pions2B' }
    mybasics["pion_2b"]["Code"] = "(TRGHOSTPROB < %(TrGhostProb)s) & (TRCHI2DOF < %(TrChi2)s) & (PT > %(MinPT2Body)s * MeV) & (PIDK < %(PIDK_pi)s) & (PIDp < %(PIDp_pi)s)" 
 
    mybasics["kaon_2b"] = { "RequiredSelections" : [StdAllNoPIDsKaons],
                            "BaseName" : name+'Kaons2B' }
    mybasics["kaon_2b"]["Code"] = "(TRGHOSTPROB < %(TrGhostProb)s) & (TRCHI2DOF < %(TrChi2)s) & (PT > %(MinPT2Body)s * MeV) & (PIDK > %(PIDK_k)s)"     
 
    mybasics["pion_3b"] = { "RequiredSelections" : [StdAllNoPIDsPions],
                            "BaseName" : name+'Pions3B' }
    mybasics["pion_3b"]["Code"] = "(TRGHOSTPROB < %(TrGhostProb)s) & (TRCHI2DOF < %(TrChi2)s) & (PT > %(MinPT3Body)s * MeV) & (PIDK < %(PIDK_pi)s) & (PIDp < %(PIDp_pi)s)" 

    mybasics["kaon_3b"] = { "RequiredSelections" : [StdAllNoPIDsKaons],
                            "BaseName" : name+'Kaons3B' }
    mybasics["kaon_3b"]["Code"] = "(TRGHOSTPROB < %(TrGhostProb)s) & (TRCHI2DOF < %(TrChi2)s) & (PT > %(MinPT3Body)s * MeV) & (PIDK > %(PIDK_k)s)" 

    mybasics["proton"]  = { "RequiredSelections" : [StdAllNoPIDsProtons],
                            "BaseName" : name+'Protons' }
    mybasics["proton"]["Code"] = "(TRGHOSTPROB < %(TrGhostProb)s) & (TRCHI2DOF < %(TrChi2)s) & (PT > %(MinPT3Body)s * MeV) & (PIDp > %(PIDp_p)s)" 

    filterdesktops = {}
    filterselections = {}
    for basicpart in mybasics :
        filterdesktops[basicpart] = FilterDesktop( Code = mybasics[basicpart]["Code"]%locals() )
        filterselections[basicpart] = Selection(  mybasics[basicpart]["BaseName"]+"FilterSel",
                                                  Algorithm = filterdesktops[basicpart],
                                                  RequiredSelections = mybasics[basicpart]["RequiredSelections"]) 

    _combination_cuts = "(APT > %(DPT)s * MeV ) & (AM > 1700.*MeV) & ( AMAXDOCA('') < %(DOCA)s )" %locals()
    
    _mother_cuts = "(VFASPF(VCHI2PDOF) < %(VTXCHI2)s) & ( PT > %(DPT)s * MeV ) & ( ADMASS('D0') < %(MassWindow)s * MeV ) & ( BPVDIRA > %(DDIRA)s ) & ( BPVLTIME() > %(DTAU)s )" %locals()
   
    CombineLTUnbCharmTIS = {}
 
    CombineLTUnbCharm_D2hh = CombineParticles( DecayDescriptor = '[D0 -> K+ pi-]cc',
                                               DaughtersCuts = { "pi+" : "ALL",
                                                                 "K+"  : "ALL"},
                                               CombinationCut = _combination_cuts,
                                               MotherCut = _mother_cuts )

    SelLTUnbCharm_D2hh     = Selection( name+"D2hh", Algorithm = CombineLTUnbCharm_D2hh,
                                        RequiredSelections = [ filterselections["pion_2b"],filterselections["kaon_2b"] ] )
                                   
    CombineLTUnbCharmTIS["D2KPiTISLine"] = tisSelection(SelLTUnbCharm_D2hh) 
    CombineLTUnbCharmTIS["D2KPiTOSLine"] = tosSelection(SelLTUnbCharm_D2hh)
    
    CombineLTUnbCharm_D2KPiPi = CombineParticles( DecayDescriptor = '[D+ -> K- pi+ pi+]cc',
                                                  DaughtersCuts = { "pi+" : "ALL",
                                                                    "K+"  : "ALL" },
                                                  CombinationCut = _combination_cuts,
                                                  MotherCut = _mother_cuts )

    SelLTUnbCharm_D2KPiPi     = Selection( name+"D2KPiPi", Algorithm = CombineLTUnbCharm_D2KPiPi,
                                           RequiredSelections = [ filterselections["pion_3b"],filterselections["kaon_3b"] ] )
    
    CombineLTUnbCharmTIS["D2KPiPiLine"] = tisSelection(SelLTUnbCharm_D2KPiPi)

    CombineLTUnbCharm_Ds2KKPi = CombineParticles( DecayDescriptor = '[D_s+ -> K- K+ pi+]cc',
                                                  DaughtersCuts = { "pi+" : "ALL",
                                                                    "K+"  : "ALL" },
                                                  CombinationCut = _combination_cuts,
                                                  MotherCut = _mother_cuts )

    SelLTUnbCharm_Ds2KKPi     = Selection( name+"Ds2KKPi", Algorithm = CombineLTUnbCharm_Ds2KKPi,
                                           RequiredSelections = [ filterselections["pion_3b"],filterselections["kaon_3b"] ] )

    CombineLTUnbCharmTIS["Ds2KKPiLine"] = tisSelection(SelLTUnbCharm_Ds2KKPi)

    CombineLTUnbCharm_Lc2pKPi = CombineParticles( DecayDescriptor = '[Lambda_c+ -> p+ K- pi+]cc',
                                                  DaughtersCuts = { "p+"  : "ALL",
                                                                    "pi+" : "ALL",
                                                                    "K+"  : "ALL" },
                                                  CombinationCut = _combination_cuts,
                                                  MotherCut = _mother_cuts )

    SelLTUnbCharm_Lc2pKPi     = Selection( name+"Lc2pKPi", Algorithm = CombineLTUnbCharm_Lc2pKPi,
                                           RequiredSelections = [ filterselections["pion_3b"],filterselections["kaon_3b"],filterselections["proton"] ] )

    CombineLTUnbCharmTIS["Lc2pKPiLine"] = tisSelection(SelLTUnbCharm_Lc2pKPi)

    return CombineLTUnbCharmTIS

########################################################################  
def makeTISFilter(name):
    specs = {'Hlt1.*Decision%TIS':0,
             'Hlt2.*Decision%TIS':0}
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISFilter')
    tisTosFilter.TisTosSpecs = specs
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter

def tisSelection(sel):
    '''Filters Selection sel to be TIS.'''
    tisTosFilter = makeTISFilter(sel.name())
    return Selection(sel.name()+'TIS', Algorithm=tisTosFilter,
                     RequiredSelections=[sel])
########################################################################  
def makeTOSFilter(name):
    specs = {'Hlt2CharmHadMinBiasD02KPiDecision%TOS':0}
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TOSFilter')
    tisTosFilter.TisTosSpecs = specs
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter

def tosSelection(sel):
    '''Filters Selection sel to be TOS.'''
    tisTosFilter = makeTOSFilter(sel.name())
    return Selection(sel.name()+'TOS', Algorithm=tisTosFilter,
                     RequiredSelections=[sel])
