################################################################################
## StrippingPIDCalib.py                                                       ##
## Lucio Anderlini, Sneha Malde, Barbara Sciascia                             ##
##                                                                            ##
## LineBuilder to create StrippingLines for the calibration samples used in   ##
## PIDCalib. Only single-vertex decays are defined here. Dedicated line       ##
## builders are defined for other samples (such as D* and Lb decay chains).   ##
##                                                                            ##
## Implemented lines:                                                         ##
##  . StrippingPIDCalibJpsi2MuMuLine                                          ##
##  . StrippingPIDCalibBu2KMuMuLine                                           ##
##  . StrippingPIDCalibL02ppiLowPTLine                                        ##
##  . StrippingPIDCalibL02ppiHighPTLine                                       ##
##  . StrippingPIDCalibJpsi2eeLine                                            ##
##  . StrippingPIDCalibBu2KeeLine                                             ##
##                                                                            ##
################################################################################


'''
Module defining PIDCalib single-vertex stripping lines
'''

__author__  = ['Lucio Anderlini']
__date__    =  '25/02/2015'
__version__ =  '$Revision: 1.0 $'


__all__ = (
    'PIDCalibLineBuilder',
    'default_config'
    )

default_config = {
    'NAME'              : 'PIDCalib',
    'BUILDERTYPE'       : 'DiMuonConf',
    'WGs'               : [ 'ALL' ],
    'BUILDERTYPE'       : 'PIDCalibLineBuilder',
    'STREAMS'           : [ 'PID' ],
    'CONFIG'            : {
      'Jpsi2MuMu'       : { # Replaces old StrippingMuIDCalib_JpsiKFromBNoPIDNoMip
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'DecayDescriptor'    : "J/psi(1S) -> mu+ mu-"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'    : [ 'Phys/StdNoPIDsMuons/Particles' ]
        , 'DaughterCuts'       : {
            'mu+' : "(ISLONG) & (P>3*GeV) & (PT>800*MeV) & (MIPCHI2DV(PRIMARY)>10) & (TRCHI2DOF<3)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'CombinationCut'     :
            "(in_range(3096-200, AM, 3096+200)) & (ACHI2DOCA(1,2) < 10)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2)<5) & (BPVVDCHI2 > 225)"
      },


      'Bu2KMuMu'       : {
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'DecayDescriptor'    : "[B+ -> mu+ mu- K+]cc"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'    : [ 'Phys/StdNoPIDsKaons/Particles', 'Phys/StdNoPIDsMuons/Particles' ]
        , 'DaughterCuts'       : {
            'mu+' : "(MIPCHI2DV(PRIMARY)>10)",
            'K+'  : "(MIPCHI2DV(PRIMARY)>25)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'Combination12Cut'   :
            "(in_range(3096-200, AM, 3096+200)) & (ACHI2DOCA(1,2) < 10)"
        , 'CombinationCut'     :
            "(in_range(5279-500, AM, 5279+500)) & (ACHI2DOCA(1,3) < 10) & (ACHI2DOCA(2,3) < 10)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2)<10)"
#            "(VFASPF(VCHI2)<10) & (BPVDLS > 50) & (BPVIPCHI2()<25)"
      },


      'L02ppiLowPT' : {
        'Prescale'      : .14
        , 'CheckPV'     : True
        , 'DecayDescriptor'    : "[Lambda0 -> p+ pi-]cc"
        , 'InputTES'    : [ 'Phys/StdAllNoPIDsPions/Particles', 
                                   'Phys/StdAllNoPIDsProtons/Particles' ]
        , 'DaughterCuts'       : {
            'p+'               : "( 2.0 * GeV < P ) &  ( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 35 )",
            'pi-'              : "( 2.0 * GeV < P ) &  ( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 35 )"
          }
        , 'CombinationCut'     :
            "(AM < 1.5 * GeV) & (ACHI2DOCA(1,2) < 20)"
        , 'MotherCut'          : (
            "   ( ADMASS ( 'Lambda0') < 25 ) " + 
            " & ( in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) )" + 
            " & ( VFASPF ( VZ ) < 2200 ) "  +
#            " & ( in_range ( 0 , BPVLTFITCHI2() , 49 ) )" + 
            " & ( BPVDLS > 100  ) " +
            " & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 20 )"
          )
      },

      'L02ppiHighPT' : {
        'CloneLine' : "L02ppiLowPT"
        , 'Prescale'  : 1.0
        , 'DaughterCuts'       : {
            'p+'               : "(40.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiIsMuon' : {
        'CloneLine' : "L02ppiLowPT"
        , 'Prescale'  : 1.0
        , 'DaughterCuts'       : {
            'p+'               : "(ISMUON) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'Jpsi2ee'       : { 
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'DecayDescriptor'    : "J/psi(1S) -> e+ e-"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'           : [ 'Phys/StdNoPIDsElectrons/Particles' ]
        , 'DaughterCuts'       : {
            'e+' : "( P > 3*GeV ) & ( PT > 500*MeV ) & (BPVIPCHI2()> 9.0)  " 
          }
        , 'TagAndProbeCut'     :
            "(ACHILD(PIDe, #tag) > 5.0) & (ACHILD(PT, #tag) > 1500*MeV) & (ACHILD(P, #tag) > 6*GeV) "
        , 'CombinationCut'     :
            "(in_range(2100, AM, 4300)) & (ACHI2DOCA(1,2) < 18)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2)<18) & (BPVDLS > 50) & (PT > 2*GeV)"
      },

      'Bu2Kee'       : {
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'DecayDescriptor'    : "[B+ -> e+ e- K+]cc"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'           : [ 'Phys/StdNoPIDsKaons/Particles', 'Phys/StdNoPIDsElectrons/Particles' ]
        , 'DaughterCuts'       : {
            'e+'  : "( P  > 3*GeV )   & ( PT > 500*MeV ) & (BPVIPCHI2()> 9.0) ",
            'K+'  : "( PT > 1.0*GeV ) & ( PIDK > 0 )& (BPVIPCHI2()> 9.0) " 
          }
        , 'TagAndProbeCut'     :
            "(ACHILD(PIDe, #tag) > 5.0) & (ACHILD(PT, #tag) > 1500) & (ACHILD(P, #tag) > 6*GeV) "
        , 'Combination12Cut'   :
            "(in_range(2100, AM, 4300)) & (ACHI2DOCA(1,2) < 18)"
        , 'CombinationCut'     :
            "(in_range(5279-1200, AM, 5279+1000)) & (ACHI2DOCA(1,3) < 18) & (ACHI2DOCA(2,3) < 18)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2)<18) & (BPVVDCHI2 > 225)"
      },
    }
  }

from Gaudi.Configuration import *
from Configurables import CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import DaVinci__N3BodyDecays 

class PIDCalibLineBuilder(LineBuilder):
    
    __configuration_keys__ = (
        'Jpsi2MuMu',
        'Bu2KMuMu',
        'L02ppiLowPT',
        'L02ppiHighPT', 
        'L02ppiIsMuon', 
        'Jpsi2ee',
        'Bu2Kee'
      )

    
##==============================================================================
## PIDCalibLineBuilder constructor
##==============================================================================
    def __init__(self, name, config ): 
      
      LineBuilder.__init__(self, name, config)
      self.name = name 
      self.config = config

      self.registerLine ( self.buildPIDLine ( 'Jpsi2MuMu'    , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'Bu2KMuMu'     , bodies = 3) )
      self.registerLine ( self.buildPIDLine ( 'L02ppiLowPT'  , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'L02ppiHighPT' , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'L02ppiIsMuon' , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'Jpsi2ee'      , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'Bu2Kee'       , bodies = 3) )


##==============================================================================
## PIDCalibLineBuilder::_builderTagProbeCut 
##    takes as input a cutPattern defining tag and probe requirements using 
##    #tag and #probe key words which are replaced by tge indices of the 
##    tag and probe particles as defined in the configuration dictionary.
##    Tag and probe tracks are then swapped and the resulting cuts AND-ed.
##==============================================================================
    def _buildTagProbeCut(self, cutPattern, indices):
      "service function to swap tag and probe and build the combination cut"
      cut  = '(('
      cut +=  cutPattern.replace('#tag',indices[0]).replace('#probe',indices[1])
      cut += ")|("
      cut +=  cutPattern.replace('#tag',indices[1]).replace('#probe',indices[0])
      cut += "))"
      return cut


##==============================================================================
## PIDCalibLineBuilder::buildPIDLine
##   Parses the configuration dictionary to build the lines as defined in the 
##   configuration dictionaries
##==============================================================================
    def buildPIDLine(self, configRowId, bodies):
      "Parses the configuration dictionary to build the line"
      _config = self.config [ configRowId ]

      ## If the line inherit another line clones and updates the dictionary
      if 'CloneLine' in _config:
        _newConfig = {}
        _newConfig.update ( self.config[_config['CloneLine']] ) 
        _newConfig.update ( _config )
        _config = _newConfig
      
      ## Selects the algorithm according to the number of bodies
      _algorithm = None
      if (bodies == 2)   :
        _algorithm = CombineParticles(self.name + configRowId + 'Algorithm')
      elif (bodies == 3) :
        _algorithm = DaVinci__N3BodyDecays(self.name + configRowId + 'Line')
        _algorithm.Combination12Cut = _config['Combination12Cut']

      ## Prepares the tag&probe cut if needed
      _combinationCut = _config['CombinationCut']
      if 'TagAndProbeIndices' in _config:   ## Tag & Probe defined
        _id = [str(x) for x in _config['TagAndProbeIndices']]
        _combinationCut += ' & '
        _combinationCut += self._buildTagProbeCut(_config['TagAndProbeCut'], _id)

      _algorithm.CombinationCut  = _combinationCut
      _algorithm.DaughtersCuts   = _config['DaughterCuts']
      _algorithm.MotherCut       = _config['MotherCut']
      _algorithm.DecayDescriptor = _config['DecayDescriptor']
      
      _requiredSelections = [DataOnDemand(x) for x in _config['InputTES']]
      _selection = Selection(self.name + configRowId + 'Selection'
                              , Algorithm = _algorithm
                              , RequiredSelections = _requiredSelections
                            )

      _line = StrippingLine( self.name + configRowId + 'Line'
                              , prescale = _config['Prescale']
                              , checkPV  = _config['CheckPV']
                              , algos    = [ _selection ]
                           )
      
      return _line


