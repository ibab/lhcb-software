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
##  . StrippingPIDCalibL02ppiLowPLine                                         ##
##  . StrippingPIDCalibL02ppiHighPLine                                        ##
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
        , 'DaughtersCuts'       : {
            'mu+' : "(ISLONG) & (P>3*GeV) & (PT>800*MeV) & (MIPCHI2DV(PRIMARY)>10) & (TRCHI2DOF<3)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'CombinationCut'     :
            "(in_range(3096-200, AM, 3096+200)) & (ACHI2DOCA(1,2) < 10)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2)<5) & (BPVVDCHI2 > 25)"
      },


      'Bu2KMuMu'       : {
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'DecayDescriptor'    : "[B+ -> mu+ mu- K+]cc"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'    : [ 'Phys/StdNoPIDsKaons/Particles', 'Phys/StdNoPIDsMuons/Particles' ]
        , 'DaughtersCuts'       : {
            'mu+' : "(MIPCHI2DV(PRIMARY)>10)",
            'K+'  : "(MIPCHI2DV(PRIMARY)>25)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'Combination12Cut'   :
            "(in_range(3096-200, AM, 3096+200)) & (ACHI2DOCA(1,2) < 100)"
        , 'CombinationCut'     :
            "(in_range(5279-500, AM, 5279+500)) & (ACHI2DOCA(1,3) < 100) & (ACHI2DOCA(2,3) < 100)"
        , 'MotherCut'          :
#            "(VFASPF(VCHI2)<10)"
            "(VFASPF(VCHI2)<100) & (BPVDLS > 4) & (BPVIPCHI2()<25)"
      },


      'L02ppiLowP' : {
        'Prescale'      : .14
        , 'CheckPV'     : True
        , 'DecayDescriptor'    : "[Lambda0 -> p+ pi-]cc"
        , 'InputTES'    : [ 'Phys/StdAllNoPIDsPions/Particles', 
                                   'Phys/StdAllNoPIDsProtons/Particles' ]
        , 'DaughtersCuts'       : {
            'p+'               : "( 2.0 * GeV < P ) &  ( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 35 )",
            'pi-'              : "( 2.0 * GeV < P ) &  ( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 35 )"
          }
        , 'CombinationCut'     :
            "(AM < 1.5 * GeV) & (ACHI2DOCA(1,2) < 20)"
        , 'MotherCut'          : (
            "   ( ADMASS ( 'Lambda0') < 25 ) " + 
            " & ( in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) )" + 
            " & ( VFASPF ( VZ ) < 2200 ) "  +
            " & ( in_range ( 0 , BPVLTFITCHI2() , 49 ) )" + 
#            " & ( BPVDLS > 100  ) " +
            " & ( BPVLTIME() * c_light > 5  ) " +
            " & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 20 )"
          )
      },

      'L02ppiHighP' : {
        'CloneLine' : "L02ppiLowP"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(40.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiIsMuon' : {
        'CloneLine' : "L02ppiLowP"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(ISMUON) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiDDLowP' : {
        'CloneLine' : "L02ppiLowP"
        , 'Prescale'  : .14
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'L02ppiDDHighP' : {
        'CloneLine' : "L02ppiHighP"
        , 'Prescale'  : 1.0
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'Jpsi2ee'       : { 
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'DecayDescriptor'    : "J/psi(1S) -> e+ e-"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'           : [ 'Phys/StdNoPIDsElectrons/Particles' ]
        , 'DaughtersCuts'       : {
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
        , 'DaughtersCuts'       : {
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
            "(VFASPF(VCHI2)<18) & (BPVVDCHI2 > 25)"
      },

      'Ks02pipi'      :{
        'Prescale'      : 0.024
        , 'CheckPV'     : True
        , 'DecayDescriptor'       : "KS0 -> pi+ pi-"
        , 'InputTES'              : ['Phys/StdNoPIDsPions']
        , 'DaughtersCuts'         : {
            'pi+' : " ( 2.0 * GeV < P ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
        , 'CombinationCut'        : " ( ADAMASS('KS0') < 100 ) & (ACHI2DOCA(1,2) < 20 )"
        , 'MotherCut'             : (
            "( ADMASS ( 'KS0') < 50 ) & "+
            " in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) & " + 
            " ( VFASPF ( VZ ) < 2200 ) & " + 
            " (BPVVDCHI2 > 25) & " +
            " ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > 9 ) & " + 
            " ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > 9 )"
          )
      },
    
      'Ks02pipiDD' : {
        'CloneLine'    : 'Ks02pipi'
        , 'Prescale'   : 0.024
        , 'CheckPV'     : True
        , 'InputTES'   : ['Phys/StdNoPIDsDownPions']
        , 'MotherCut'             : (
            "( ADMASS ( 'KS0') < 50 ) & "+
            " in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) & " + 
            " ( VFASPF ( VZ ) < 2200 ) & " + 
            " (BPVVDCHI2 > 25) & " +
            " ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > 18 ) & " + 
            " ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > 18 )"
        )
      },
    
      'Lc2pKpi' : {
        'Prescale':   1.0
        , 'CheckPV'     : True
        , 'InputTES' : [  'Phys/StdNoPIDsKaons/Particles' , 
                          'Phys/StdNoPIDsPions/Particles' , 
                          'Phys/StdNoPIDsProtons/Particles' 
                       ]
        , 'DecayDescriptor' : "[Lambda_c+ -> K- p+ pi+]cc"
        , 'DaughtersCuts' : {
          'K+'    : '( PT>250*MeV ) & ( P>2*GeV ) & ( TRPCHI2>0.0001 ) & ( MIPCHI2DV(PRIMARY)>8. ) ' 
          , 'p+'  : '( PT>250*MeV ) & ( P>2*GeV ) & ( TRPCHI2>0.0001 ) & ( MIPCHI2DV(PRIMARY)>8. ) ' 
          , 'pi+' : '( PT>250*MeV ) & ( P>2*GeV ) & ( TRPCHI2>0.0001 ) & ( MIPCHI2DV(PRIMARY)>8. ) '
          }
        , 'Combination12Cut' : "( ACHI2DOCA(1,2) < 25 )"
        , 'CombinationCut'   : "( ADAMASS('Lambda_c+')<150*MeV ) & ( APT>1.*GeV ) & ( ADOCACHI2CUT(50, '') )"
        , 'MotherCut'        : (
            "( M > 2.240*GeV ) & " +
            " ( M<2.330*GeV ) & " +
            " ( VFASPF(VCHI2/VDOF)<8 ) & " +
            " ( BPVDIRA>0.99999 ) & " +
            " ( MIPCHI2DV(PRIMARY)<4. ) & " +
            " in_range( 0.85*GeV, M13, 0.95*GeV ) & " +
            " ( (WM( 'K-' , 'pi+' , 'pi+' )>1.89*GeV) | (WM( 'K-' , 'pi+' , 'pi+' )<1.80*GeV) )"
        )
    }
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
        'L02ppiLowP',
        'L02ppiDDHighP', 
        'L02ppiDDLowP',
        'L02ppiHighP', 
        'L02ppiIsMuon', 
        'Jpsi2ee',
        'Bu2Kee',
        'Ks02pipi',
        'Ks02pipiDD',
        'Lc2pKpi'
      )

    
##==============================================================================
## PIDCalibLineBuilder constructor
##==============================================================================
    def __init__(self, name, config ): 
      
      LineBuilder.__init__(self, name, config)
      self.name = name 
      self.config = config

      print 1
      self.registerLine ( self.buildPIDLine ( 'Jpsi2MuMu'    , bodies = 2) )
      print 2
      self.registerLine ( self.buildPIDLine ( 'Bu2KMuMu'     , bodies = 3) )
      print 3
      self.registerLine ( self.buildPIDLine ( 'L02ppiLowP'   , bodies = 2) )
      print 4
      self.registerLine ( self.buildPIDLine ( 'L02ppiHighP'  , bodies = 2) )
      print 5
      self.registerLine ( self.buildPIDLine ( 'L02ppiDDLowP' , bodies = 2) )
      print 6
      self.registerLine ( self.buildPIDLine ( 'L02ppiDDHighP', bodies = 2) )
      print 7
      self.registerLine ( self.buildPIDLine ( 'L02ppiIsMuon' , bodies = 2) )
      print 8
      self.registerLine ( self.buildPIDLine ( 'Jpsi2ee'      , bodies = 2) )
      print 9
      self.registerLine ( self.buildPIDLine ( 'Bu2Kee'       , bodies = 3) )
      print 10
      self.registerLine ( self.buildPIDLine ( 'Ks02pipi'     , bodies = 2) )
      print 11
      self.registerLine ( self.buildPIDLine ( 'Ks02pipiDD'   , bodies = 2) )
      print 12
      self.registerLine ( self.buildPIDLine ( 'Lc2pKpi'      , bodies = 3) )
      print 13


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
      
      from copy import copy
      _config = copy(self.config [ configRowId ])

      ## If the line inherit another line clones and updates the dictionary
      while 'CloneLine' in _config:
        print "Cloning " + configRowId + " from " + _config['CloneLine']
        _newConfig = {}
        _newConfig.update ( self.config[_config['CloneLine']] ) 
        del _config['CloneLine']
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

        print _combinationCut

      _algorithm.CombinationCut  = _combinationCut
      _algorithm.DaughtersCuts   = _config['DaughtersCuts']
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

