"""
Stripping options for (pre-)selecting Bd,s -> baryon antibaryon.
For now it only contains options for Bd,s -> p pbar
(ultimately it will also select Bd,s -> p Lambda_bar, Lambda Lambda_bar).

Authors: Eduardo Rodrigues
"""

########################################################################
__author__ = ['Eduardo Rodrigues']
__date__ = '22/03/2012'
__version__ = '$Revision: 1.5 $'

__all__ = ('B2TwoBaryonLines',
           'makeB2PPbar')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from StandardParticles                     import StdLooseProtons

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig

default_config = { 'PrescaleB2PPbar'   : 1,
                   'MinPTB2PPbar'      : 900,
                   'MinIPChi2B2PPbar'  : 10,
                   #'TrChi2'            : 4,
                   'PIDppi'            : -1,
                   'PIDpk'             : -2,
                   'MaxPTB2PPbar'      : 2100,
                   'MaxIPChi2B2PPbar'  : 25,
                   'CombMassWindow'    : 200,
                   'VertexChi2B2PPbar' : 9,
                   'BPTB2PPbar'        : 1100,
                   'BIPChi2B2PPbar'    : 16,
                   'BDIRA'             : 0.9997,
                   'MaxGhostProb'      : 0.4
                 }


class B2TwoBaryonLines( LineBuilder ) :
    """Class defining the Bd,s -> baryon antibaryon stripping line"""
    
    __configuration_keys__ = ( 'PrescaleB2PPbar',
                               'MinPTB2PPbar',
                               'MinIPChi2B2PPbar',
                               #'TrChi2',
                               'PIDppi',
                               'PIDpk',
                               'MaxPTB2PPbar',
                               'MaxIPChi2B2PPbar',
                               'CombMassWindow',
                               'VertexChi2B2PPbar',
                               'BPTB2PPbar',
                               'BIPChi2B2PPbar',
                               'BDIRA',
                               'MaxGhostProb'
                             )
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        B2PPbarName  = name + "B2PPbar"
        
        self.B2PPbar = makeB2PPbar( B2PPbarName,
                                    config['MinPTB2PPbar'],
                                    #config['TrChi2'],
                                    config['PIDppi'],
                                    config['PIDpk'],
                                    config['MinIPChi2B2PPbar'],
                                    config['MaxIPChi2B2PPbar'],
                                    config['CombMassWindow'],
                                    config['MaxPTB2PPbar'], 
                                    config['VertexChi2B2PPbar'],
                                    config['BIPChi2B2PPbar'],
                                    config['BPTB2PPbar'],
                                    config['BDIRA'],
                                    config['MaxGhostProb']
                                    )
        
        self.lineB2PPbar = StrippingLine( B2PPbarName+"Line",
                                          prescale = config['PrescaleB2PPbar'],
                                          selection = self.B2PPbar )
        
        
        self.registerLine(self.lineB2PPbar)    

def makeB2PPbar( name,
                 minPT,
                 #trChi2,
                 pidPPi, pidPK, minIPChi2, maxIPChi2,
                 combMassWindow, maxPT,
                 vertexChi2, bIPChi2, bPT, bDIRA, maxGhostProb ) :

    #_daughters_cuts = "(PT > %(minPT)s * MeV) & (TRCHI2DOF < %(trChi2)s) & ((PIDp-PIDpi) > %(pidPPi)s) & ( (PIDp-PIDK) > %(pidPK)s ) & (MIPCHI2DV(PRIMARY) > %(minIPChi2)s)" %locals()
    _daughters_cuts = "(PT > %(minPT)s * MeV) & ((PIDp-PIDpi) > %(pidPPi)s) & ( (PIDp-PIDK) > %(pidPK)s ) & (MIPCHI2DV(PRIMARY) > %(minIPChi2)s) & (TRGHP < %(maxGhostProb)s )" %locals()
    _combination_cuts = "( (ADAMASS('B0') < %(combMassWindow)s * MeV) | (ADAMASS('B_s0') < %(combMassWindow)s * MeV) ) & ( AMAXCHILD(MAXTREE('p+'==ABSID,PT)) > %(maxPT)s * MeV ) & ( AMAXCHILD(MAXTREE('p+'==ABSID,MIPCHI2DV(PRIMARY))) > %(maxIPChi2)s )" %locals()
    _mother_cuts = "(PT > %(bPT)s * MeV) & ( VFASPF(VCHI2PDOF) < %(vertexChi2)s ) & ( BPVDIRA > %(bDIRA)s ) & ( BPVIPCHI2() < %(bIPChi2)s )" %locals()

    CombineB2PPbar = CombineParticles( DecayDescriptor = 'B0 -> p+ p~-',
                                       DaughtersCuts = { "p+" : _daughters_cuts },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts )

    return Selection( name,
                      Algorithm = CombineB2PPbar,
                      RequiredSelections = [ StdLooseProtons ] )

########################################################################  
