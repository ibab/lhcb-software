## #####################################################################
# A stripping selection for muon identification calibration trough
# the study of J/psi(1S) -> mu+ mu-
## #####################################################################

'''
largely cloned from line written by 'S.Furcas', 'G.Lanfranchi', 'M.Palutan', 'A.Sarti', 'D.Milanes', 'MuID Team'
'''

__author__  = [ 'G.Ciezarek', ]
__date__    = '03/12/2015'
__version__ = '$Revision: 1.0 $'

__all__     = ( 'LowPTMuConf',
                'makeLowPTSelection',
                'default_config'
                )

default_config = {
    'LowPTMuID':{
        'WGs'         : ['ALL'],
        'BUILDERTYPE' : 'LowPTMuConf',
        'CONFIG'      : {'Prescale'           : 1,
                         'MassWindow'         : 200,
                         'ProbeIPCHI2'        : 25,
                         'TagIPCHI2'          : 45,
                         'TagPT'              : 1000,
                         'VertexCHI2'         : 5,
                         'FDCHI2'             : 225,
                         'DIRA'               : 0.9995
                          
                         },
        'STREAMS'     :  ['PID']
        }
    }
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StrippingConf.StrippingLine import StrippingLine
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence, MergedSelection
from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdAllNoPIDsMuons

class LowPTMuConf( LineBuilder ):
    __configuration_keys__ = ('Prescale',
                              'MassWindow',
                              'ProbeIPCHI2',
                              'TagIPCHI2',
                              'TagPT',
                              'VertexCHI2',
                              'FDCHI2',
                              'DIRA',    
                              )

    def __init__( self, name, config ) :
        LineBuilder.__init__( self, name, config )
        
        self.config=config


        self.selStdAllNoPIDMuons = StdAllNoPIDsMuons 
        #Detached
        self.sel_DetachedLowPT  = makeLowPTSelection( name + "_JpsiFromBLowPTCombine", self.selStdAllNoPIDMuons,self )
        self.line_DetachedLowPT = StrippingLine( name + '_JpsiFromBLowPT', 
                                                 prescale = config[ 'Prescale' ], 
                                                 selection = self.sel_DetachedLowPT, 
#                                                 RequiredRawEvents = ["Muon"] 
                                                 ) 
        self.registerLine( self.line_DetachedLowPT )



'''
Selection Makers
'''
def makeLowPTSelection( name, muons,self):

    mucocut = '(MIPCHI2DV(PRIMARY)>%(ProbeIPCHI2)s)' 
    combination = CombineParticles( DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                    DaughtersCuts = { 'mu+' : mucocut % self.config, 'mu-' : mucocut % self.config},
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<%(MassWindow)s*MeV)" % self.config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<%(VertexCHI2)s) & (BPVVDCHI2 > %(FDCHI2)s) & (BPVDIRA > %(DIRA)s) & ( ( (CHILDCUT(ISMUON,1)) & (CHILDCUT((PT>%(TagPT)s),1)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>%(TagIPCHI2)s),1)) ) | ((CHILDCUT(ISMUON,2)) & (CHILDCUT((PT>%(TagPT)s),2)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>%(TagIPCHI2)s),2)) ) ) " % self.config
                                    )
    return Selection( name, 
                      Algorithm = combination,
                      RequiredSelections = [muons] )

