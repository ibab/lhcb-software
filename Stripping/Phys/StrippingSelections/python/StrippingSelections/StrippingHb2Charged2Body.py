"""
Stripping options for (pre-)selecting B -> hh', B -> p pbar
and Bs -> mu mu events.

Uses the selection framework.

Authors: Angelo Carboni, Stefano Perazzini, Eduardo Rodrigues

Usage:
from StrippingConf.StrippingStream                import StrippingStream
from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines
stream = StrippingStream( 'Bhadron' )
stream.appendLines( Hb2Charged2BodyLines().lines() )
"""

########################################################################
uthor__ = ['Stefano Perazzini','Angelo Carbone','Eduardo Rodrigues']
__date__ = '07/02/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('Hb2Charged2BodyLines',
           'makeB2Charged2Body',
           'makeB2PPbar')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from StandardParticles import StdNoPIDsPions, StdLooseProtons 

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils   import checkConfig 
from StrippingUtils.Utils import LineBuilder

name = "Presel"

config = { 'PrescaleB2Charged2Body' : 1, 'PrescaleB2PPbar' : 1,
           'MinPTB2Charged2Body' : 700,  'MinPTB2PPbar' : 900,
           'MinIP' : 0.08,               'MinIPChi2' : 9,
           'TrChi2' : 5,
                                         'PIDppi' : 0, 
                                         'PIDpk' : -2,
           'MaxPTB2Charged2Body' : 2400, 'MaxPTB2PPbar' : 2500,
           'MaxIP' : 0.2,
           'CombMassLow' : 4500, 
           'CombMassHigh' : 6500,        'CombMassWindow' : 200,
           'BPT' : 1000,
           'BTAU' : 0.0005,              'VertexChi2' : 16,
                                         'BIPChi2' : 36, 
                                         'BDIRA' : 0.9995,
           'MassLow' : 4800,
           'MassHigh' : 5800
         }

class Hb2Charged2BodyLines( LineBuilder ) :
    """Class defining the Hb -> hh stripping lines"""
   
    __configuration_keys__ = ( 'PrescaleB2Charged2Body', 'PrescaleB2PPbar',
                               'MinPTB2Charged2Body',    'MinPTB2PPbar',
                               'MinIP',                  'MinIPChi2',
                               'TrChi2', 
                                                         'PIDppi', 
                                                         'PIDpk',
                               'MaxPTB2Charged2Body',    'MaxPTB2PPbar', 
                               'MaxIP',
                               'CombMassLow', 
                               'CombMassHigh',           'CombMassWindow',
                               'BPT',        
                               'BTAU',                   'VertexChi2',
                                                         'BIPChi2',
                                                         'BDIRA',      
                               'MassLow',  
                               'MassHigh'
                             )

    def __init__( self,name,config ) :
        

        LineBuilder.__init__(self, name, config)

        B2Charged2BodyName = name+"B2Charged2Body"
        B2PPbarName = name+"B2PPbar"

        # make the various stripping selections
        self.B2Charged2Body = makeB2Charged2Body( B2Charged2BodyName,
                                                  config['MinPTB2Charged2Body'],
                                                  config['MinIP'],
                                                  config['TrChi2'],
                                                  config['MaxPTB2Charged2Body'],
                                                  config['MaxIP'],
                                                  config['CombMassLow'],
                                                  config['CombMassHigh'],
                                                  config['BPT'],
                                                  config['BTAU'],
                                                  config['MassLow'],
                                                  config['MassHigh'] )

        self.B2PPbar = makeB2PPbar( B2PPbarName,
                                    config['MinPTB2PPbar'],
                                    config['TrChi2'],
                                    config['PIDppi'],
                                    config['PIDpk'],
                                    config['MinIPChi2'],
                                    config['CombMassWindow'],
                                    config['MaxPTB2PPbar'], 
                                    config['VertexChi2'], 
                                    config['BIPChi2'], 
                                    config['BDIRA'] )                            
        
        self.lineB2Charged2Body = StrippingLine( B2Charged2BodyName+"Line",
                                                 prescale  = config['PrescaleB2Charged2Body'],
                                                 selection = self.B2Charged2Body )

        self.lineB2PPbar = StrippingLine( B2PPbarName+"Line",
                                          prescale = ['PrescaleB2PPbar'],
                                          selection = self.B2PPbar )

        self.registerLine(self.lineB2Charged2Body)
        self.registerLine(self.lineB2PPbar)    

def makeB2Charged2Body( name, 
                        minPT,minIP,trChi2,
                        maxPT,maxIP,combMassLow,combMassHigh,
                        bPT,bTAU,massLow,massHigh ) :
    
    _daughters_cuts = "(PT > %(minPT)s * MeV) & (MIPDV(PRIMARY) > %(minIP)s ) & (TRCHI2DOF < %(trChi2)s)" %locals()
    _combination_cuts = "(AM > %(combMassLow)s * MeV) & (AM < %(combMassHigh)s * MeV) & (AMAXCHILD(MAXTREE('pi+'==ABSID,PT)) > %(maxPT)s ) & (AMAXCHILD(MAXTREE('pi+'==ABSID,MIPDV(PRIMARY))) > %(maxIP)s)" %locals()
    _mother_cuts = "(M > %(massLow)s * MeV) & (M < %(massHigh)s * MeV) & (PT > %(bPT)s * MeV) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(bTAU)s )" %locals()

    CombineHb2Charged2Body = CombineParticles( DecayDescriptor = 'B0 -> pi+ pi-',
                                               DaughtersCuts = { "pi+" : _daughters_cuts },
                                               CombinationCut = _combination_cuts,
                                               MotherCut = _mother_cuts )
    
    return Selection( name,
                      Algorithm = CombineHb2Charged2Body,
                      RequiredSelections = [ StdNoPIDsPions ] )    

def makeB2PPbar( name,
                 minPT, trChi2, pidPPi, pidPK, minIPChi2,
                 combMassWindow, maxPT,
                 vertexChi2, bIPChi2, bDIRA ) :

    _daughters_cuts = "(PT > %(minPT)s * MeV) & (TRCHI2DOF < %(trChi2)s) & ((PIDp-PIDpi) > %(pidPPi)s) & ( (PIDp-PIDK) > %(pidPK)s ) & (MIPCHI2DV(PRIMARY) > %(minIPChi2)s)" %locals()
    _combination_cuts = "(ADAMASS('B0') < %(combMassWindow)s * MeV) & ( AMAXCHILD(MAXTREE('p+'==ABSID,PT)) > %(maxPT)s * MeV )" %locals()
    _mother_cuts = "( VFASPF(VCHI2PDOF) < %(vertexChi2)s ) & ( BPVIPCHI2() < %(bIPChi2)s ) & ( BPVDIRA > %(bDIRA)s )" %locals()

    CombineB2PPbar = CombineParticles( DecayDescriptor = 'B0 -> p+ p~-',
                                       DaughtersCuts = { "p+" : _daughters_cuts },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts )

    return Selection( name,
                      Algorithm = CombineB2PPbar,
                      RequiredSelections = [ StdLooseProtons ] )

########################################################################  
