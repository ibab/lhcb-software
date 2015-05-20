"""
Stripping options for (pre-)selecting B -> hh'

Authors: Stefano Perazzini, Lars Eklumd
"""

########################################################################
__author__ = ['Stefano Perazzini', 'Lars Eklund']
__date__ = '21/08/2012'
__version__ = '$Revision: 1.5 $'

__all__ = ('Hb2Charged2BodyLines',
           'makeB2Charged2Body',
           'default_config')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from StandardParticles                     import StdNoPIDsPions, StdLooseProtons, StdLooseKaons

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig
from Configurables               import SubstitutePID, FilterDesktop


default_config = {
    'Hb2Charged2Body' : {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Hb2Charged2BodyLines',
    'CONFIG'      : {'PrescaleB2Charged2Body'  : 1,
                     'MinPTB2Charged2Body'     : 1000,
                     'MinIPChi2B2Charged2Body' : 12,
                     'TrChi2'                  : 3,
                     'TrGhostProb'             : 0.5,
                     'MaxPTB2Charged2Body'     : 1400,
                     'MaxIPChi2B2Charged2Body' : 40,
                     'CombMassLow'             : 4600,
                     'CombMassHigh'            : 6000,
                     'DOCA'                    : 0.08,
                     'BPT'                     : 1200,
                     'BIPChi2B2Charged2Body'   : 12,
                     'BTAU'                    : 0.0006,
                     'MassLow'                 : 4800,
                     'MassHigh'                : 5800
                    },
    'STREAMS'     : ['BhadronCompleteEvent']
    }
}

class Hb2Charged2BodyLines( LineBuilder ) :
    """Class defining the Hb -> hh stripping lines"""
    
    __configuration_keys__ = ('PrescaleB2Charged2Body',
                              'MinPTB2Charged2Body',
                              'MinIPChi2B2Charged2Body',
                              'TrChi2',           
                              'TrGhostProb',      
                              'MaxPTB2Charged2Body',
                              'MaxIPChi2B2Charged2Body',
                              'CombMassLow',            
                              'CombMassHigh',           
                              'DOCA',                   
                              'BPT',                    
                              'BIPChi2B2Charged2Body',  
                              'BTAU',                   
                              'MassLow',                
                              'MassHigh'               
                              )
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        B2Charged2BodyName   = name + "B2Charged2Body"
        
        # make the various stripping selections
        self.B2Charged2Body = makeB2Charged2Body( B2Charged2BodyName,
                                                  config['TrChi2'],
                                                  config['TrGhostProb'],
                                                  config['MinPTB2Charged2Body'],
                                                  config['MinIPChi2B2Charged2Body'],
                                                  config['MaxPTB2Charged2Body'],
                                                  config['MaxIPChi2B2Charged2Body'],
                                                  config['CombMassLow'],
                                                  config['CombMassHigh'],
                                                  config['DOCA'],
                                                  config['BPT'],
                                                  config['BIPChi2B2Charged2Body'],
                                                  config['BTAU'],
                                                  config['MassLow'],
                                                  config['MassHigh']
                                                )
        
        self.lineB2Charged2Body = StrippingLine( B2Charged2BodyName+"Line",
                                                 prescale  = config['PrescaleB2Charged2Body'],
                                                 selection = self.B2Charged2Body,
                                                 EnableFlavourTagging = False,
                                                 RequiredRawEvents = ["Trigger","Muon","Calo","Rich","Velo","Tracker"])
        
        self.registerLine(self.lineB2Charged2Body)

def makeB2Charged2Body( name, 
                        trChi2,trGhostProb,minPT,minIPChi2,
                        maxPT,maxIPChi2,combMassLow,combMassHigh,doca,
                        bPT,bIPChi2,bTAU,massLow,massHigh ) : 
    
    _daughters_cuts = "(TRGHOSTPROB < %(trGhostProb)s) & (TRCHI2DOF < %(trChi2)s) & (PT > %(minPT)s * MeV) & ( MIPCHI2DV(PRIMARY) > %(minIPChi2)s  )" %locals()
    
    _combination_cuts = "(AMAXCHILD(MAXTREE('pi+'==ABSID,PT)) > %(maxPT)s ) & ( AMAXCHILD(MAXTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))) > %(maxIPChi2)s ) & (AMAXDOCA('') < %(doca)s ) & (AM > %(combMassLow)s * MeV) & (AM < %(combMassHigh)s * MeV)" %locals()
    
    _mother_cuts = "(PT > %(bPT)s * MeV) & (M > %(massLow)s * MeV) & (M < %(massHigh)s * MeV) & ( BPVIPCHI2() < %(bIPChi2)s ) & (BPVLTIME() > %(bTAU)s )" %locals()
    
    CombineHb2Charged2Body = CombineParticles( DecayDescriptor = 'B0 -> pi+ pi-',
                                               DaughtersCuts = { "pi+" : _daughters_cuts },
                                               CombinationCut = _combination_cuts,
                                               MotherCut = _mother_cuts )
    
    return Selection( name,
                      Algorithm = CombineHb2Charged2Body,
                      RequiredSelections = [ StdNoPIDsPions ] )    


########################################################################  

