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

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand, SimpleSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV
from StandardParticles import StdLooseDiMuon, StdLooseDiMuonSameSign


__author__  = 'Carlos Vazquez Sierra', 'Christian Elsasser'
__date__    = '06/04/2016'
__version__ = '$Revision: 2.0$'
__all__     = 'A1MuMuConf', 'default_config'

default_config = {
  'NAME'        : 'A1MuMu',
  'BUILDERTYPE' : 'A1MuMuConf',
  'WGs'         : [ 'QEE' ],
  'STREAMS'     : [ 'Dimuon' ], # FullDST for iso5 variables.
  'CONFIG'      : {
    'A1MuMu_LinePrescale'   : 1.0,
    'A1MuMu_LinePostscale'  : 1.0,
    'A1MuMu_checkPV'        : False,
    'DIMUON_LOW_MASS'       : 5000.0 * MeV,
    'PT_MUON_MIN'           : 2500.0 * MeV,
    'P_MUON_MIN'            : 2500.0 * MeV, # (sanity cut: de facto no cut)
    'TRACKCHI2_MUON_MAX'    : 10,         # dimensionless
    'PT_DIMUON_MIN'         : 7500.0 * MeV,
    'VCHI2_DIMUON_MAX'      : 12,         # dimensionless
  },
}


class A1MuMuConf(LineBuilder) :
  __configuration_keys__ = default_config['CONFIG'].keys()

  def __init__(self, name, config):
    LineBuilder.__init__(self, name, config)

    ### A1 -> mu+ mu- line:
    sel = filterDiMuon( name + '_SelMuMu', 
      [StdLooseDiMuon],
      MuonPT        = config['PT_MUON_MIN'],
      MuonP         = config['P_MUON_MIN'],
      MuonTRCHI2DOF = config['TRACKCHI2_MUON_MAX'],
      MuMuMinMass   = config['DIMUON_LOW_MASS'],
      MuMuVCHI2PDOF = config['VCHI2_DIMUON_MAX'],
      MuMuPT        = config['PT_DIMUON_MIN'],
    )

    self.registerLine(StrippingLine(name + 'Line',
      prescale  = config['A1MuMu_LinePrescale'],
      postscale = config['A1MuMu_LinePrescale'],
      RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
      checkPV   = config['A1MuMu_checkPV'],
      selection = sel,
    ))

    ### [A1 -> mu- mu-]cc line:
    sel = filterDiMuon( name + '_SelMuMuSS',
      [StdLooseDiMuonSameSign],
      MuonPT        = config['PT_MUON_MIN'],
      MuonP         = config['P_MUON_MIN'],
      MuonTRCHI2DOF = config['TRACKCHI2_MUON_MAX'],
      MuMuMinMass   = config['DIMUON_LOW_MASS'],
      MuMuVCHI2PDOF = config['VCHI2_DIMUON_MAX'],
      MuMuPT        = config['PT_DIMUON_MIN'],
    )
            
    self.registerLine(StrippingLine(name + 'SameSign' + 'Line',
      prescale  = config['A1MuMu_LinePrescale'],
      postscale = config['A1MuMu_LinePrescale'],
      RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
      checkPV   = config['A1MuMu_checkPV'],
      selection = sel,
    ))


# Filtering DiMuon selection:
def filterDiMuon( name,
                  inputs,
                  MuonPT,
                  MuonP,
                  MuonTRCHI2DOF,
                  MuMuMinMass,
                  MuMuVCHI2PDOF,
                  MuMuPT ):

  MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
  MuMuCut = "(MM > %(MuMuMinMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
  code    = MuonCut + " & " + MuMuCut
  return SimpleSelection( name, FilterDesktop, inputs, Code=code)

### END OF FILE ###
