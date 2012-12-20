"""
Stripping options for (pre-)selecting B -> hh'

Authors: Stefano Perazzini
"""

########################################################################
__author__ = ['Stefano Perazzini']
__date__ = '21/08/2012'
__version__ = '$Revision: 1.5 $'

__all__ = ('B2HHBkgLines',
           'makeB2HHBkg')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from StandardParticles                     import StdNoPIDsPions

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig


default_config = {
           'PrescaleB2HHBkg' : 0.5,
           'MinPT'           : 1000,
           'MinIP'           : 0.12,
           'TrChi2'          : 3,
           'CombMassLow'     : 4600,
           'CombMassHigh'    : 6000,
           'DOCA'            : 0.1,
           'BPT'             : 1200,
           'BIP'             : 0.12,
           'BTAU'            : 0.0003,
           'MassLow'         : 4800,
           'MassHigh'        : 5800
           }

## Change decay descriptor and re-fit decay tree

class B2HHBkgLines( LineBuilder ) :
    """Class defining the Hb -> hh stripping lines"""
    
    __configuration_keys__ = ( 'PrescaleB2HHBkg',
                               'MinPT',    
                               'MinIP',
                               'TrChi2',
                               'CombMassLow',
                               'CombMassHigh',        
                               'DOCA',               
                               'BPT',              
                               'BIP',
                               'BTAU',
                               'MassLow',
                               'MassHigh'
                               )
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        B2HHBkgName   = "B2HHBkg"
        
        # make the various stripping selections
        self.B2HHBkg = makeB2HHBkg( B2HHBkgName,
                                    config['TrChi2'],
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
        
        self.lineB2HHBkg = StrippingLine( B2HHBkgName+"Line",
                                          prescale  = config['PrescaleB2HHBkg'],
                                          selection = self.B2HHBkg )
        
        self.registerLine(self.lineB2HHBkg)

def makeB2HHBkg( name, 
                 trChi2,minPT,minIP,
                 combMassLow,combMassHigh,doca,
                 bPT,bIP,bTAU,massLow,massHigh ) : 
    
    _daughters_cuts = "(TRCHI2DOF < %(trChi2)s) & (PT > %(minPT)s * MeV) & ( MIPDV(PRIMARY) > %(minIP)s )" %locals()
    
    _combination_cuts = "( AMAXDOCA('') < %(doca)s ) & ( AM > %(combMassLow)s * MeV ) & ( AM < %(combMassHigh)s * MeV )" %locals()
    
    _mother_cuts = "( PT > %(bPT)s * MeV ) & ( M > %(massLow)s * MeV ) & ( M < %(massHigh)s * MeV ) & ( BPVIP() < %(bIP)s ) & ( BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(bTAU)s )" %locals()
    
    CombineB2HHBkg = CombineParticles( DecayDescriptor = 'B0 -> pi+ pi-',
                                       DaughtersCuts = { "pi+" : _daughters_cuts },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts )
    
    return Selection( name,
                      Algorithm = CombineB2HHBkg,
                      RequiredSelections = [ StdNoPIDsPions ] )    


########################################################################  

