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
Module defining PIDCalib single-vertex stripping lines, based on those by Lucio Anderlini
#####For HI, remove ee line, low mass mumu line, add prompt Jpsi2MuMu line
'''

__author__  = ['Yanxi ZHANG']
__date__    =  '25/02/2015'
__version__ =  '$Revision: 1.0 $'


__all__ = (
    'PIDCalibLineBuilder',
    'default_config'
    )

default_config = {
    'NAME'              : 'HeavyIonPIDCalib',
    'WGs'               : [ 'ALL' ],
    'BUILDERTYPE'       : 'HeavyIonPIDCalibConf',
    'STREAMS'           : [ 'ALL' ],
    'CONFIG'            : {
      "odin": ["NoBeam","Beam1","Beam2","BeamCrossing"], #"( ODIN_BXTYP == LHCb.ODIN.Beam1) | (ODIN_BXTYP == LHCb.ODIN.Beam2) | (ODIN_BXTYP == LHCb.ODIN.BeamCrossing) | (ODIN_BXTYP == LHCb.ODIN.NoBeam)",        ##[ 'NoBeam', 'BeamCrossing','Beam1','Beam2']

      'Jpsi2MuMuPrompt'       : { # Replaces old StrippingMuIDCalib_JpsiKFromBNoPIDNoMip
        'Prescale'      : 1.0
        , 'CheckPV'     : False
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None # "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None # "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "J/psi(1S) -> mu+ mu-"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'    : [ 'Phys/StdAllNoPIDsMuons/Particles' ]
        , 'DaughtersCuts'       : {
            'mu+' : "(ISLONG) & (P>3*GeV) & (PT>500*MeV) & (TRCHI2DOF<3)"
          }
        , 'TagAndProbeCut'     : "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV)" + "& (ACHILD(PIDmu, #tag)>0.)"
        , 'CombinationCut'     : "(in_range(3096.916-160, AM, 3096.916+160)) & (ACHI2DOCA(1,2) < 10)"
        , 'MotherCut'          : "(VFASPF(VCHI2)<5)"
      },


      'Jpsi2MuMu'       : { # Replaces old StrippingMuIDCalib_JpsiKFromBNoPIDNoMip
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
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
            "(VFASPF(VCHI2)<5) & (BPVVDCHI2 > 225)"
      },



      'L02ppi' : {
        'Prescale'      : .14
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
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
            " & ( BPVLTIME() * c_light > 5  ) " +
            " & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 20 )"
          )
      },

      'L02ppiHighPT' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(PT > 3.5*GeV) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiVeryHighPT' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(PT > 10*GeV) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiIsMuon' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(ISMUON) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiDD' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : .14
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'L02ppiDDHighPT' : {
        'CloneLine' : "L02ppiHighPT"
        , 'Prescale'  : 1.0
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'L02ppiDDVeryHighPT' : {
        'CloneLine' : "L02ppiVeryHighPT"
        , 'Prescale'  : 1.0
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },


      'Bu2Jpsi_mumuK' :  {
        'Prescale'       : 1.0
        , 'CheckPV'      : True
        , 'RawEvent'    : ['Calo']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "[B+ -> J/psi(1S) K+]cc"
        , 'DecayDescriptorJpsi': "J/psi(1S) -> mu+ mu-"
        , 'InputTES'    : [ 'Phys/StdNoPIDsKaons/Particles' ]
        , 'InputTESJpsi': [ 'Phys/StdNoPIDsMuons/Particles' ]
        , 'DaughtersCuts'       : {
            'mu+' : "(BPVIPCHI2()> 10.0) ",
            'K+'  : "(MIPCHI2DV(PRIMARY)>25)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'JpsiCombinationCut' :
            "(ADAMASS('J/psi(1S)')<200*MeV)"
        , 'JpsiMotherCut'      :
            "(VFASPF(VCHI2/VDOF)<5) & (BPVVDCHI2 > 225) "
        , 'CombinationCut'     :
            "ADAMASS('B+') < 500.*MeV"
        , 'MotherCut'          :
            "(BPVIPCHI2()<25)"
      },


      'Ks02pipi'      :{
        'Prescale'      : 0.024
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'       : "KS0 -> pi+ pi-"
        , 'InputTES'              : ['Phys/StdNoPIDsPions/Particles']
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
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'InputTES'   : ['Phys/StdNoPIDsDownPions/Particles']
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
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
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
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from Configurables import CombineParticles
#from Configurables import DaVinci__N3BodyDecays 
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 

class HeavyIonPIDCalibConf(LineBuilder):
    
    __configuration_keys__ = default_config['CONFIG'].keys()

    
##==============================================================================
## PIDCalibLineBuilder constructor
##==============================================================================
    def __init__(self, name, config ): 
      
      LineBuilder.__init__(self, name, config)
      self.name = name 
      self.config = config
      self.odin = "|".join( ["(ODIN_BXTYP == LHCb.ODIN.%s)"%(odin_type,) for odin_type in config['odin'] if odin_type in ["NoBeam","Beam1","Beam2","BeamCrossing"]])

      self.registerLine ( self.buildPIDLine ( 'Jpsi2MuMuPrompt'    , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'Jpsi2MuMu'          , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'L02ppi'             , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'L02ppiHighPT'       , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'L02ppiVeryHighPT'   , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'L02ppiDD'           , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'L02ppiDDHighPT'     , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'L02ppiDDVeryHighPT' , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'L02ppiIsMuon'       , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'Ks02pipi'           , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'Ks02pipiDD'         , bodies = 2) )
      self.registerLine ( self.buildPIDLine ( 'Lc2pKpi'            , bodies = 3) )
      self.registerLine ( self.buildPIDLineWithJpsi ( 'Bu2Jpsi_mumuK' ) )


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
## PIDCalibLineBuilder::_resolveCloning 
##    check if the config declares a "CloneLine" keyword, if yes it clones the 
##    pointed line, if the latter declares a "CloneLine" keyword imports from
##    that the missing keywords and so on.
##    Returns a _copy_ of the updated dictionary
##==============================================================================
    def _resolveCloning(self, configRowId):
      from copy import copy
      _config = copy(self.config [ configRowId ])
      ## If the line inherit another line clones and updates the dictionary
      while 'CloneLine' in _config:
###        print "Cloning " + configRowId + " from " + _config['CloneLine']
        _newConfig = {}
        _newConfig.update ( self.config[_config['CloneLine']] ) 
        del _config['CloneLine']
        _newConfig.update ( _config )
        _config = _newConfig
      return _config


##==============================================================================
## PIDCalibLineBuilder::buildPIDLine
##   Parses the configuration dictionary to build the lines as defined in the 
##   configuration dictionaries
##==============================================================================
    def buildPIDLine(self, configRowId, bodies):
      "Parses the configuration dictionary to build the line"

      ## Resolves possible clone statements
      _config = self._resolveCloning(configRowId)
      
      ## Selects the algorithm according to the number of bodies
      _algorithm = None
      if (bodies == 2)   :
        _algorithm = CombineParticles()
        #_algorithm = CombineParticles(self.name + configRowId + 'Algorithm')
      elif (bodies == 3) :
        #_algorithm = DaVinci__N3BodyDecays(self.name + configRowId + 'Line')
        #_algorithm = DaVinci__N3BodyDecays(self.name + configRowId + 'Algorithm')
        _algorithm = DaVinci__N3BodyDecays()
        _algorithm.Combination12Cut = _config['Combination12Cut']

      ## Prepares the tag&probe cut if needed
      _combinationCut = _config['CombinationCut']
      if 'TagAndProbeIndices' in _config:   ## Tag & Probe defined
        _id = [str(x) for x in _config['TagAndProbeIndices']]
        _combinationCut += ' & '
        _combinationCut += self._buildTagProbeCut(_config['TagAndProbeCut'], _id)

###        print _combinationCut

      _algorithm.CombinationCut  = _combinationCut
      _algorithm.DaughtersCuts   = _config['DaughtersCuts']
      _algorithm.MotherCut       = _config['MotherCut']
      _algorithm.DecayDescriptor = _config['DecayDescriptor']
      
      _requiredSelections = [DataOnDemand(x) for x in _config['InputTES']]
      _selection = Selection(self.name + configRowId + 'Selection'
                              , Algorithm = _algorithm
                              , RequiredSelections = _requiredSelections
                            )

      _line = StrippingLine( name                      =self.name + configRowId + 'Line'
                              , prescale               = _config['Prescale']
                              , checkPV                = _config['CheckPV']
                              , EnableFlavourTagging   = False
                              , RequiredRawEvents      = _config['RawEvent']
                              #, MDSTFlag              = _config['MDST.DST']
                              , HLT1             = _config['Hlt1Filter'] 
                              , HLT2             = _config['Hlt2Filter'] 
                              , algos    = [ _selection ]
                              , ODIN     = self.odin
                           )
      
      return _line

##==============================================================================
## PIDCalibLineBuilder::buildPIDLineWithJpsi
##   Parses the configuration dictionary to build the lines as defined in the 
##   configuration dictionaries, this function allows two subsequent fits,
##   and uses the output of the former as input of the latter
##==============================================================================
    def buildPIDLineWithJpsi(self, configRowId):
      ## Resolves possible clone statements
      _config = self._resolveCloning(configRowId)

      ## Prepares the tag&probe cut if needed
      _combinationCutJpsi = _config['JpsiCombinationCut']
      _combinationCut     = _config['CombinationCut']

      if 'TagAndProbeCut' in _config:
        _id = ["1", "2"]
        _combinationCutJpsi += ' & '
        _combinationCutJpsi += self._buildTagProbeCut(_config['TagAndProbeCut'], _id)

        ###print _combinationCut

      _algorithmJpsi = CombineParticles()
      #_algorithmJpsi = CombineParticles(self.name + configRowId + 'JpsiAlgorithm')

      _algorithmJpsi.CombinationCut  = _combinationCutJpsi
      _algorithmJpsi.DaughtersCuts   = _config['DaughtersCuts']
      _algorithmJpsi.MotherCut       = _config['JpsiMotherCut']
      _algorithmJpsi.DecayDescriptor = _config['DecayDescriptorJpsi']
      
      _requiredSelections = [DataOnDemand(x) for x in _config['InputTESJpsi']]
      _selectionJpsi = Selection(self.name + configRowId + 'SelectionJpsi'
                                  , Algorithm = _algorithmJpsi
                                  , RequiredSelections = _requiredSelections
                                )

      _algorithm = CombineParticles()
      #_algorithm = CombineParticles(self.name + configRowId + 'Algorithm')

      _algorithm.CombinationCut  = _combinationCut
      _algorithm.DaughtersCuts   = _config['DaughtersCuts']
      _algorithm.MotherCut       = _config['MotherCut']
      _algorithm.DecayDescriptor = _config['DecayDescriptor']
      
      _requiredSelections = [_selectionJpsi] + [DataOnDemand(x) for x in _config['InputTES']]
      _selection = Selection(self.name + configRowId + 'Selection'
                              , Algorithm = _algorithm
                              , RequiredSelections = _requiredSelections
                            )

      _line = StrippingLine( self.name + configRowId + 'Line'
                              , prescale = _config['Prescale']
                              , checkPV  = _config['CheckPV']
                              , EnableFlavourTagging = False
                              , RequiredRawEvents    = _config['RawEvent']
                              #, MDSTFlag             = _config['MDST.DST']
                              , HLT1                 = _config['Hlt1Filter'] 
                              , HLT2                 = _config['Hlt2Filter'] 
                              , algos    = [ _selection ]
                              , ODIN     = self.odin
                           )
      
      return _line
