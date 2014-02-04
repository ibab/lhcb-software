"""
Stripping options for (pre-)selecting B -> hh'

Authors: Stefano Perazzini
"""

########################################################################
__author__ = ['Stefano Perazzini']
__date__ = '21/08/2012'
__version__ = '$Revision: 1.5 $'

__all__ = ('B2HHBDTLines',
           'makeB2HHBDT',
           'applyBDT')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles                     import StdNoPIDsPions

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig


default_config = {
           'PrescaleB2HHBDT' : 1.,
           'MinPT'           : 1000,
           'MinIP'           : 0.12,
           'TrChi2'          : 3,
           'TrGhostProb'     : 0.5,
           'CombMassLow'     : 4600,
           'CombMassHigh'    : 6400,
           'DOCA'            : 0.1,
           'BPT'             : 1200,
           'BIP'             : 0.12,
           'BTAU'            : 0.0006,
           'MassLow'         : 4800,
           'MassHigh'        : 6200,
           'BDTCut'          : -0.3,
           'BDTWeightsFile'  : "$TMVAWEIGHTSROOT/data/B2HHBDT.xml"
           }

## Change decay descriptor and re-fit decay tree

class B2HHBDTLines( LineBuilder ) :
    """Class defining the Hb -> hh stripping lines"""
    
    __configuration_keys__ = ( 'PrescaleB2HHBDT',
                               'MinPT',    
                               'MinIP',
                               'TrChi2',
                               'TrGhostProb',
                               'CombMassLow',
                               'CombMassHigh',        
                               'DOCA',               
                               'BPT',              
                               'BIP',
                               'BTAU',
                               'MassLow',
                               'MassHigh',
                               'BDTCut',
                               'BDTWeightsFile'
                               )
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        B2HHBDTName   = "B2HHBDT"
        
        # make the various stripping selections
        self.B2HHBDT = makeB2HHBDT( B2HHBDTName,
                                    config['TrChi2'],
                                    config['TrGhostProb'],
                                    config['MinPT'],
                                    config['MinIP'],
                                    config['CombMassLow'],
                                    config['CombMassHigh'],
                                    config['DOCA'],
                                    config['BPT'],
                                    config['BIP'],
                                    config['BTAU'],
                                    config['MassLow'],
                                    config['MassHigh']
                                  )
        
        self.CutBDT  = applyBDT( "CutBDT_" + B2HHBDTName,
                                 LineName       = B2HHBDTName + "Line",
                                 SelB2HHBDT     = self.B2HHBDT,
                                 BDTCutValue    = config['BDTCut'],
                                 BDTWeightsFile = config['BDTWeightsFile']
                               )

        self.lineB2HHBDT = StrippingLine( B2HHBDTName+"Line",
                                          prescale  = config['PrescaleB2HHBDT'],
                                          selection = self.CutBDT,
                                          EnableFlavourTagging = True )
        
        self.registerLine(self.lineB2HHBDT)

def makeB2HHBDT( name, 
                 trChi2,trGhostProb,minPT,minIP,
                 combMassLow,combMassHigh,doca,
                 bPT,bIP,bTAU,massLow,massHigh ) : 
    
    _daughters_cuts = "(TRGHOSTPROB < %(trGhostProb)s) & (TRCHI2DOF < %(trChi2)s) & (PT > %(minPT)s * MeV) & ( MIPDV(PRIMARY) > %(minIP)s )" %locals()
    
    _combination_cuts = "( AMAXDOCA('') < %(doca)s ) & ( AM > %(combMassLow)s * MeV ) & ( AM < %(combMassHigh)s * MeV )" %locals()
    
    _mother_cuts = "( PT > %(bPT)s * MeV ) & ( M > %(massLow)s * MeV ) & ( M < %(massHigh)s * MeV ) & ( BPVIP() < %(bIP)s ) & ( BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(bTAU)s )" %locals()
    
    CombineB2HHBDT = CombineParticles( DecayDescriptor = 'B0 -> pi+ pi-',
                                       DaughtersCuts = { "pi+" : _daughters_cuts },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts )
    
    return Selection( name,
                      Algorithm = CombineB2HHBDT,
                      RequiredSelections = [ StdNoPIDsPions ] )    


def applyBDT( name,
              LineName,
              SelB2HHBDT,
              BDTCutValue,
              BDTWeightsFile ):

    _FilterB2HH = FilterDesktop( Code = "FILTER('B2hhBDTSelection/B2hhBDT')" )

    BDTSel = Selection( name,
                        Algorithm = _FilterB2HH,
                        RequiredSelections = [ SelB2HHBDT ]
                        )

    """          
    Name is special here, since this is the last algorithm,
    whose name seems to be the one of the stripping line....
    """
    from Configurables import B2hhBDTSelection
    MyBDT = B2hhBDTSelection( LineName + '.B2hhBDT' )
    MyBDT.BDTCut = BDTCutValue
    MyBDT.BDTWeightsFile = BDTWeightsFile

    return BDTSel


########################################################################  

