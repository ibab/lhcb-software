################################################
#      Stripping lines for A_1 -> mu+ mu-      #
#                                              #
#                                              #
# Author: Ch. Elsasser elsasser@cern.ch (2012) #
# Modified by C. Vazquez Sierra (06/Apr/2016)  #
#             carlos.vazquez@cern.ch           #
################################################

"""
    A_1 -> mu+ mu-, based on functionality from StrippingDiMuonNew by Jibo He.
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions
from StandardParticles import StdLoosePions,StdTightPions

__author__= ['Carlos Vazquez Sierra, Christian Elsasser']
__date__= '06/04/2016'
__version__='$Revision: 2.0$'

default_config = {'NAME'        : 'A1MuMu',
                  'WGs'         : ['QEE'],
                  'BUILDERTYPE' : 'A1MuMuConf',
                  'CONFIG'      : {'A1MuMu_LinePrescale'               : 1.0,
                                   'A1MuMu_LinePostscale'              : 1.0,
                                   'A1MuMu_checkPV'                    : False,
                                   'DIMUON_LOW_MASS'                   : '5000.0',    # MeV/c2
                                   #'DIMUON_HIGH_MASS'                 : not set,     # MeV/c2
                                   'PT_MUON_MIN'                       : '2500.0',    # MeV/c
                                   'P_MUON_MIN'                        : '2500.0',    # MeV/c (sanity cut: de facto no cut)
                                   'TRACKCHI2_MUON_MAX'                : '10',        # dimensionless
                                   'PT_DIMUON_MIN'                     : '7500.0',    # MeV/c
                                   'VCHI2_DIMUON_MAX'                  : '12'         # dimensionless
                                   },
                  'STREAMS'     : [ 'Dimuon' ]} # FullDST for iso5 variables.


__all__ = ('A1MuMuConf', 'filterDiMuon', 'filterDiMuonSameSign', 'default_config')

class A1MuMuConf(LineBuilder) :
    A1MuMuLine             = None
    A1MuMuSameSignLine     = None
    __configuration_keys__ = ('A1MuMu_LinePrescale',
                              'A1MuMu_LinePostscale',
                              'A1MuMu_checkPV',
                              'DIMUON_LOW_MASS',
                              #'DIMUON_HIGH_MASS',
                              'PT_MUON_MIN',
                              'P_MUON_MIN',
                              'TRACKCHI2_MUON_MAX',
                              'PT_DIMUON_MIN',
                              'VCHI2_DIMUON_MAX')

    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        # if name not set outside, set it to empty 
        if name == None:
            name = ""

        ### A1 -> mu+ mu- line:
        self.SelA1MuMu           = filterDiMuon( name,
                                                 MuonPT        = config['PT_MUON_MIN'],
                                                 MuonP         = config['P_MUON_MIN'],
                                                 MuonTRCHI2DOF = config['TRACKCHI2_MUON_MAX'],
                                                 MuMuMinMass   = config['DIMUON_LOW_MASS'],
                                                 MuMuVCHI2PDOF = config['VCHI2_DIMUON_MAX'],
                                                 MuMuPT        = config['PT_DIMUON_MIN'])

        self.A1MuMuLine          = StrippingLine(name + 'Line',
                                                 prescale  = config['A1MuMu_LinePrescale'],
                                                 postscale = config['A1MuMu_LinePrescale'],
                                                 RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                                 checkPV   = config['A1MuMu_checkPV'],
                                                 selection = self.SelA1MuMu)#,
                                                 #MDSTFlag = True)

        ### A1 -> mu+ mu- line:
        self.SelA1MuMuSameSign          = filterDiMuonSameSign( name + 'SameSign',
                                                                MuonPT        = config['PT_MUON_MIN'],
                                                                MuonP         = config['P_MUON_MIN'],
                                                                MuonTRCHI2DOF = config['TRACKCHI2_MUON_MAX'],
                                                                MuMuMinMass   = config['DIMUON_LOW_MASS'],
                                                                MuMuVCHI2PDOF = config['VCHI2_DIMUON_MAX'],
                                                                MuMuPT        = config['PT_DIMUON_MIN'])
                
        self.A1MuMuSameSignLine = StrippingLine(name + 'SameSign' + 'Line',
                                                prescale  = config['A1MuMu_LinePrescale'],
                                                postscale = config['A1MuMu_LinePrescale'],
                                                RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                                checkPV   = config['A1MuMu_checkPV'],
                                                selection = self.SelA1MuMuSameSign)#,
                                                #MDSTFlag = True)
            
        self.registerLine( self.A1MuMuLine )
        self.registerLine( self.A1MuMuSameSignLine )

# Filtering DiMuon selection:
def filterDiMuon( name,
                  MuonPT,
                  MuonP,
                  MuonTRCHI2DOF,
                  MuMuMinMass,
                  MuMuVCHI2PDOF,
                  MuMuPT ):
    
    _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' )
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    MuMuCut = "(MM > %(MuMuMinMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name + "_SelMuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ] )

# Filtering DiMuon same sign selection:
def filterDiMuonSameSign( name,
                          MuonPT,
                          MuonP,
                          MuonTRCHI2DOF,
                          MuMuMinMass,
                          MuMuVCHI2PDOF,
                          MuMuPT ):
    
    _StdLooseDiMuonSameSign = DataOnDemand( Location = 'Phys/StdLooseDiMuonSameSign/Particles' )
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    MuMuCut = "(MM > %(MuMuMinMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s *MeV)" % locals()
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name + "_SelMuMuSS",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuonSameSign ] )

### END OF FILE ###
