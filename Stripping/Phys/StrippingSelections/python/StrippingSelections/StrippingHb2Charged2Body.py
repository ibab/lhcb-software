"""
Stripping options for (pre-)selecting B -> hh', B -> p pbar
and Bs -> mu mu events.
Authors: Angelo Carboni, Stefano Perazzini, Eduardo Rodrigues
"""

########################################################################
__author__ = ['Stefano Perazzini','Angelo Carbone','Eduardo Rodrigues']
__date__ = '18/02/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('Hb2Charged2BodyLines',
           'makeB2Charged2Body',
           'makeB2PPbar')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from StandardParticles                     import StdNoPIDsPions, StdLooseProtons 

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig

class Hb2Charged2BodyLines( LineBuilder ) :
    """Class defining the Hb -> hh stripping lines"""
    
    __configuration_keys__ = ( 'PrescaleB2Charged2Body',  'PrescaleB2PPbar',
                               'MinPTB2Charged2Body',     'MinPTB2PPbar',
                               'MinIPB2Charged2Body',
                               'MinIPChi2B2Charged2Body', 'MinIPChi2B2PPbar',
                               'TrChi2',
                               'PIDppi',
                               'PIDpk',
                               'MaxPTB2Charged2Body',     'MaxPTB2PPbar',
                               'MaxIPB2Charged2Body',
                               'MaxIPChi2B2Charged2Body', 'MaxIPChi2B2PPbar',
                               'CombMassLow',
                               'CombMassHigh',            'CombMassWindow',
                               'DOCA',                    'VertexChi2B2PPbar',
                               'BPT',                     'BPTB2PPbar',
                               'BIP',
                               'BIPChi2B2Charged2Body',   'BIPChi2B2PPbar',
                               'BDIRA',
                               'BTAU',
                               'MassLow',
                               'MassHigh' 
                               )
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        B2Charged2BodyName = name+"B2Charged2Body"
        B2PPbarName = name+"B2PPbar"
        
        # make the various stripping selections
        self.B2Charged2Body = makeB2Charged2Body( B2Charged2BodyName,
                                                  config['TrChi2'],
                                                  config['MinPTB2Charged2Body'],
                                                  config['MinIPB2Charged2Body'],
                                                  config['MinIPChi2B2Charged2Body'],
                                                  config['MaxPTB2Charged2Body'],
                                                  config['MaxIPB2Charged2Body'],
                                                  config['MaxIPChi2B2Charged2Body'],
                                                  config['CombMassLow'],
                                                  config['CombMassHigh'],
                                                  config['DOCA'],
                                                  config['BPT'],
                                                  config['BIP'],
                                                  config['BIPChi2B2Charged2Body'],
                                                  config['BTAU'],
                                                  config['MassLow'],
                                                  config['MassHigh']
                                                )
                                                  
        self.B2PPbar = makeB2PPbar( B2PPbarName,
                                    config['MinPTB2PPbar'],
                                    config['TrChi2'],
                                    config['PIDppi'],
                                    config['PIDpk'],
                                    config['MinIPChi2B2PPbar'],
                                    config['MaxIPChi2B2PPbar'],
                                    config['CombMassWindow'],
                                    config['MaxPTB2PPbar'], 
                                    config['VertexChi2B2PPbar'],
                                    config['BIPChi2B2PPbar'],
                                    config['BPTB2PPbar'],
                                    config['BDIRA'] )
        
        self.lineB2Charged2Body = StrippingLine( B2Charged2BodyName+"Line",
                                                 prescale  = config['PrescaleB2Charged2Body'],
                                                 selection = self.B2Charged2Body )
        
        self.lineB2PPbar = StrippingLine( B2PPbarName+"Line",
                                          prescale = config['PrescaleB2PPbar'],
                                          selection = self.B2PPbar )
        
        self.registerLine(self.lineB2PPbar)    
        self.registerLine(self.lineB2Charged2Body)


def makeB2Charged2Body( name, 
                        trChi2,minPT,minIP,minIPChi2,
                        maxPT,maxIP,maxIPChi2,combMassLow,combMassHigh,doca,
                        bPT,bIP,bIPChi2,bTAU,massLow,massHigh ) : 
    
    _daughters_cuts = "(TRCHI2DOF < %(trChi2)s) & (PT > %(minPT)s * MeV) & ( (MIPDV(PRIMARY) > %(minIP)s ) | ( MIPCHI2DV(PRIMARY) > %(minIPChi2)s  ) )" %locals()
    
    _combination_cuts = "(AMAXCHILD(MAXTREE('pi+'==ABSID,PT)) > %(maxPT)s ) & ( (AMAXCHILD(MAXTREE('pi+'==ABSID,MIPDV(PRIMARY))) > %(maxIP)s) | (AMAXCHILD(MAXTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))) > %(maxIPChi2)s ) ) & (AMAXDOCA('') < %(doca)s ) & (AM > %(combMassLow)s * MeV) & (AM < %(combMassHigh)s * MeV)" %locals()
    
    _mother_cuts = "(PT > %(bPT)s * MeV) & (M > %(massLow)s * MeV) & (M < %(massHigh)s * MeV) & ( ( BPVIP() < %(bIP)s ) | ( BPVIPCHI2() < %(bIPChi2)s ) ) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(bTAU)s )" %locals()
    
    CombineHb2Charged2Body = CombineParticles( DecayDescriptor = 'B0 -> pi+ pi-',
                                               DaughtersCuts = { "pi+" : _daughters_cuts },
                                               CombinationCut = _combination_cuts,
                                               MotherCut = _mother_cuts )
    
    return Selection( name,
                      Algorithm = CombineHb2Charged2Body,
                      RequiredSelections = [ StdNoPIDsPions ] )    

def makeB2PPbar( name,
                 minPT, trChi2, pidPPi, pidPK, minIPChi2, maxIPChi2,
                 combMassWindow, maxPT,
                 vertexChi2, bIPChi2, bPT, bDIRA ) :

    _daughters_cuts = "(PT > %(minPT)s * MeV) & (TRCHI2DOF < %(trChi2)s) & ((PIDp-PIDpi) > %(pidPPi)s) & ( (PIDp-PIDK) > %(pidPK)s ) & (MIPCHI2DV(PRIMARY) > %(minIPChi2)s)" %locals()
    _combination_cuts = "(ADAMASS('B0') < %(combMassWindow)s * MeV) & ( AMAXCHILD(MAXTREE('p+'==ABSID,PT)) > %(maxPT)s * MeV ) & ( AMAXCHILD(MAXTREE('p+'==ABSID,MIPCHI2DV(PRIMARY))) > %(maxIPChi2)s )" %locals()
    _mother_cuts = "(PT > %(bPT)s * MeV) & ( VFASPF(VCHI2PDOF) < %(vertexChi2)s ) & ( BPVDIRA > %(bDIRA)s ) & ( BPVIPCHI2() < %(bIPChi2)s )" %locals()

    CombineB2PPbar = CombineParticles( DecayDescriptor = 'B0 -> p+ p~-',
                                       DaughtersCuts = { "p+" : _daughters_cuts },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts )

    return Selection( name,
                      Algorithm = CombineB2PPbar,
                      RequiredSelections = [ StdLooseProtons ] )

########################################################################  
