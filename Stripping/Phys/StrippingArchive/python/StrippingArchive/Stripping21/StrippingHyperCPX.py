#!/usr/bin/env python
# =============================================================================
# $Id:
# =============================================================================
# @file StrippingSelections/StrippingHyperCPX.py
#
# The strippingline for extra rare decay: Sigma+ -> p mu+ mu-
# search for ``HyperCP-X'' particle
#
# see H.K.~Park {\it et al.} [HyperCP Collaboration],
#            ``Evidence for the decay Sigma+ ---> p mu+ mu-,''
#             Phys.\ Rev.\ Lett.\  {\bf 94}, 021801 (2005)
#             [arXiv:hep-ex/0501014].
# 
# see e.g. H.K.~Park, ``Search for the HyperCP event at the LHCb experiment,''
#      JHEP {\bf 1010}, 052 (2010).
#
# The set of lines allows to select
#
#   - Sigma+  -> p+   mu+ mu-
#             -> p~-  mu+ mu+                 for backround studies 
#   - D+      -> pi+  mu+ mu-
#             -> pi-  mu+ mu+                 for backround studies 
#   - D_s+    -> pi+  (   phi   -> mu+ mu- ) 
#             -> pi-  ( ``phi'' -> mu+ mu+ )  for backround studies 
#
#
# The default selection criteria:
#
#  - ``StdLoose''-verison of muons, protons and pions
#            chi2(Tr)/nDoF < 5
#            chi2(IP)      > 12
#  - muons   : ISMUON
#  - protons : DLL(p-pi) > 0 
#  - chi2(VX) for 3-prong vertex     < 25
#  - mass-windows for Sigma+/D+/Ds+  :  +-250 MeV
#  - mass-window  for phi            :  +-100 MeV 
#  - chi2(IP)     for Sigma+/D+/D_s+ <  9
#  - c*tau :
#      Sigma+ : c*tau >   5 mm 
#      D+     : c*tau > 200 um 
#      Ds+    : c*tau > 100 um 
#      
# The performance for 100k events:
#        RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.p
# +=======================================================================================+
# |             StrippingReport   INFO Event 100000, Good event 92017                     |
# +=======================================================================================+
# |*Decision name*                   |*Rate,%*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*|
# +----------------------------------+--------+----------+-------+--------+-------+-------+
# |_StrippingGlobal_                 |  0.0130|        12|       |   5.614|       |       |       
# |_StrippingSequenceStreamTest_     |  0.0130|        12|       |   5.610|       |       |       
# |!StrippingSigma2PMuMuForHyperCPX  |  0.0087|         8|  1.000|   4.439|      0|      0|    
# |!StrippingDs2PhiPiForHyperCPX     |  0.0043|         4|  1.000|   0.632|      0|      0|     
# |!StrippingDplus2PiMuMuForHyperCPX |  0.0043|         4|  1.000|   0.153|      0|      0|     
# +---------------------------------------------------------------------------------------+
#
# CPU is totally dominated by creation of standard basic particles
# 
# @author Vanya  BELYAEV   Ivan.Belyaev@cern.ch
# @author Andrey GOLUTVIN  Andrey.Goloutvin@cern.ch
# @date 2011-04-20
#
#                   $Revision$
# Last modification $Date: 2012-08-23 13:25:51 +0200
#                by $Author$
# =============================================================================
"""

The stripping line for extra rare decay: Sigma+ -> p mu+ mu-
search for ``HyperCP-X'' particle

See H.K.~Park {\it et al.} [HyperCP Collaboration],
            ``Evidence for the decay Sigma+ ---> p mu+ mu-,''
             Phys.\ Rev.\ Lett.\  {\bf 94}, 021801 (2005)
             [arXiv:hep-ex/0501014].

See e.g. H.K.~Park, ``Search for the HyperCP event at the LHCb experiment,''
JHEP {\bf 1010}, 052 (2010).

The set of lines allows to select

   - Sigma+  -> p+   mu+ mu-
             -> p~-  mu+ mu+                 for backround studies 
   - D+      -> pi+  mu+ mu-
             -> pi-  mu+ mu+                 for backround studies 
   - D_s+    -> pi+  (   phi   -> mu+ mu- ) 
             -> pi-  ( ``phi'' -> mu+ mu+ )  for backround studies 


The default selection criteria:

  - ``StdLoose''-version of muons, protons and pions 
            chi2(Tr)/nDoF < 5
            chi2(IP)      > 12
  - muons   : ISMUON
  - protons : DLL(p-pi) > 0 
  - chi2(VX) for 3-prong vertex     < 25
  - mass-windows for Sigma+/D+/Ds+  :  +-250 MeV
  - mass-window  for phi            :  +-100 MeV 
  - chi2(IP)     for Sigma+/D+/D_s+ <  9
  - c*tau :
      Sigma+ : c*tau >   5 mm 
      D+     : c*tau > 200 um 
      Ds+    : c*tau > 100 um       

The performance for 100k events:
      RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.p
 +=======================================================================================+
 |             StrippingReport   INFO Event 100000, Good event 92017                     |
 +=======================================================================================+
 |*Decision name*                   |*Rate,%*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*|
 +----------------------------------+--------+----------+-------+--------+-------+-------+
 |_StrippingGlobal_                 |  0.0130|        12|       |   5.614|       |       |       
 |_StrippingSequenceStreamTest_     |  0.0130|        12|       |   5.610|       |       |       
 |!StrippingSigma2PMuMuForHyperCPX  |  0.0087|         8|  1.000|   4.439|      0|      0|    
 |!StrippingDs2PhiPiForHyperCPX     |  0.0043|         4|  1.000|   0.632|      0|      0|     
 |!StrippingDplus2PiMuMuForHyperCPX |  0.0043|         4|  1.000|   0.153|      0|      0|     
 +---------------------------------------------------------------------------------------+

CPU is totally dominated by creation of standard basic particles
"""
# =============================================================================
__all__ = (
    'StrippingHyperCPXConf'   ,
    'default_config'        
    )
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@cern.ch'
__date__    = '2011-04-20'
__version__ = '$Revision$'
# =============================================================================
from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import   FilterDesktop, CombineParticles
from PhysSelPython.Wrappers                import   Selection , VoidEventSelection
from StandardParticles                     import ( StdLoosePions   ,
                                                    StdLooseProtons , 
                                                    StdLooseMuons   ) 
from StrippingConf.StrippingLine           import StrippingLine
from StrippingUtils.Utils                  import LineBuilder
from GaudiKernel.SystemOfUnits             import mm, micrometer, MeV 

# =============================================================================
## Define the default configuration 
_default_configuration_ = {
    #
    # Selection of basic particles
    #
    'ProtonCuts' : ' ( TRCHI2DOF < 5 ) & ( 0 < PIDp  - PIDpi ) & ( BPVIPCHI2() > 12 ) & (TRGHOSTPROB<0.3)' , 
    'MuonCuts'   : ' ( TRCHI2DOF < 2 ) & ISMUON                & ( BPVIPCHI2() > 12 ) & (TRGHOSTPROB<0.3)' ,                
    'PionCuts'   : ' ( TRCHI2DOF < 3 )                         & ( BPVIPCHI2() > 12 ) & (TRGHOSTPROB<0.3)' ,
    'MuonCuts_forTau23Mu'   : ' ( PT > 300 * MeV ) & ( TRCHI2DOF < 4  ) & ISMUON                & ( BPVIPCHI2() > 9 ) ' ,                
    'PionCuts_forTau23Mu'   : ' ( PT > 300 * MeV ) & ( TRCHI2DOF < 4 )                         & ( BPVIPCHI2() > 9 ) ' ,
    #
    'SigmaCTau'  :   5 *         mm ,
    'SigmaMass'  : 250 *        MeV ,
    #
    'DsCTau'     : 200 * micrometer ,
    'Ds23PiMass'     : 80 *        MeV ,
    'Ds2PhiPiMass'     : 250 *        MeV,
    #
    'DplusCTau'  : 200 * micrometer ,
    'DplusMass'  : 250 *        MeV ,
    #
    # ``Global Event Cuts''
    #
    'PrimaryVertices' : True ,
    #
    # Technicalities:
    #
    'Preambulo'       : [
    # shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                    , 
    # shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" , 
    ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau   = BPVLTIME ( 9 ) * c_light "  ,
    "ctau_forDs   = BPVLTIME ( 225 ) * c_light "  ,
    ## phi(1020) mass-window 
    "phi    = in_range ( 920 * MeV , AM23 , 1120 * MeV )"
    ] , 
    #
    # Prescales
    #
    'SigmaPrescale' : 1.0 ,
    'DplusPrescale' : 1.0 ,
    'DsPrescale'    : 1.0 ,
    'Ds3PiPrescale' : 1 ,
    }

# =============================================================================
## @class StrippingHyperCPXConf
#  Helper class to construct stripping for search Sigma+ -> p mu+ mu- 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-04-20
class StrippingHyperCPXConf(LineBuilder) :
    """
    Helper class to construct stirppinglines for search Sigma+ -> p mu+ mu- 
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() ) 
    
    ## get the default configuration 
    @staticmethod
    def defaultConfiguration( key = None ) :
        """
        Get the defualt configurtaion
        >>> conf = StrippingHyperCPXConf.defaultConfiguration()
     
        Get the elements of default configurtaion:
        
        >>> SigmaPrescale = StrippingHyperCPXConf.defaultConfiguration( 'SigmaPrescale' )
        """
        from copy import deepcopy
        _config_ = deepcopy ( _default_configuration_ )
        if key : return _config_[ key ]
        return _config_
 
    
    ## constructor
    def __init__( self , name , config ) :
        """
        Constructor
        """

        ##
        if isinstance ( config , dict ) : 
            from copy import deepcopy
            _config_ = deepcopy ( _default_configuration_ )
            _config_ . update   ( config )
        else :
            _config_ = config
                
        ## base class 
        LineBuilder.__init__( self , name , _config_ )
        
        for line in self._lines_private() :
            self.registerLine(line)
            
    ## define ``main'' selection for Sigma+ -> p mu+ mu-     
    def sigma    ( self ) :
        """
        define ``main'' selection for Sigma+ -> p mu+ mu-
        """
        
        if hasattr ( self , 'Sigma' )  : return self.Sigma
        
        _SigmaAlg = CombineParticles (
            #
            DecayDescriptors = [
            " [ Sigma+  -> p+  mu+ mu- ]cc " ,  # signal           
            " [ Sigma~+ -> p~- mu+ mu+ ]cc "    # background 
            ] ,
            #
            DaughtersCuts = {
            'p+'                                 : self._config['ProtonCuts']
            } , 
            #
            Preambulo       =                      self._config [ 'Preambulo' ] , 
            # 
            CombinationCut  = """
            ADAMASS('Sigma+') < %s
            """                                   % self._config[ 'SigmaMass' ] , 
            #
            MotherCut       = """
            ( chi2vx < 25 ) &
            ( ctau   > %s   )
            """                                   % self._config['SigmaCTau']
            )
        
        self.Sigma = Selection (
            'SelSigma2pMuMuFor' + self.name()   ,
            Algorithm  = _SigmaAlg ,
            RequiredSelections = [ self.twoMuons () ,
                                   StdLooseProtons  ]
            )
        
        return self.Sigma

    
    ## define ``normalization'' selection for Ds+ -> ( phi -> mu+ mu- ) pi+
    def ds2PhiPi    ( self ) :
        """
        Define ``normalization'' selection for Ds+ -> ( phi -> mu+ mu- ) pi+
        """
        if hasattr ( self , 'PhiPi' )  : return self.PhiPi
        
        _PhiPiAlg = CombineParticles (
            #
            DecayDescriptors = [
            " [ D_s+  -> pi+  mu+ mu- ]cc " ,  # signal           
            " [ D_s+  -> pi-  mu+ mu+ ]cc "    # backghround  
            ] ,
            #
            DaughtersCuts = {
            'pi+' :                           self._config['PionCuts_forTau23Mu']
            } , 
            #
            Preambulo        =                self._config [ 'Preambulo' ] , 
            # 
            CombinationCut  = """
                        ( ADAMASS ( 'D_s+' ) < %s ) & phi 
            """                              % self._config[ 'Ds2PhiPiMass' ]  , 
            #
            MotherCut       = """
            ( chi2vx < 20 ) &
            ( ctau_forDs   > %s   ) &
            ( BPVIPCHI2() < 225 )
            """                              % self._config['DsCTau']
            )
        
        self.PhiPi = Selection (
            'SelDs2PhiPiFor' + self.name()  ,
            Algorithm  = _PhiPiAlg    ,
            RequiredSelections = [ self.twoMuons_forTau23Mu () ,
                                   StdLoosePions    ]
            )
        
        return self.PhiPi


    ## define ``normalization'' selection for Ds+ -> pi+ pi- pi+
    def ds2PiPiPi    ( self ) :
        """
        Define ``normalization'' selection for Ds+ ->  pi+ pi- pi+
        """

        def makeTISTOS( name, _input, _trigger ) :
            from Configurables import TisTosParticleTagger
            _tisTosFilter = TisTosParticleTagger( name + "Tagger" )
            _tisTosFilter.TisTosSpecs = { _trigger : 0 }
            #_tisTosFilter.ProjectTracksToCalo = False
            #_tisTosFilter.CaloClustForCharged = False
            #_tisTosFilter.CaloClustForNeutral = False
            #_tisTosFilter.TOSFrac = { 4:0.0, 5:0.0 }
            return Selection( name
                              , Algorithm = _tisTosFilter
                              , RequiredSelections = [ _input ]
                              )        


        _PiPiPiAlg = CombineParticles (
            #
            DecayDescriptors = [" [ D_s+  -> pi+ pi+ pi- ]cc " ] ,
            #
            DaughtersCuts = {
            'pi+' :                           self._config['PionCuts_forTau23Mu']
            } , 
            #
            Preambulo        =                self._config [ 'Preambulo' ] , 
            # 
            CombinationCut  = """
                        ( ADAMASS ( 'D_s+' ) < %s ) 
            """                              % self._config[ 'Ds23PiMass' ]  , 
            #
            MotherCut       = """
            ( chi2vx < 20 ) &
            ( ctau_forDs   > %s   ) &
            ( BPVIPCHI2() < 225 )
            """                              % self._config['DsCTau']
            )
        
       
        self.combPiPiPi = Selection (
            'SelDs2PiPiPiFor' + self.name()  ,
            Algorithm  = _PiPiPiAlg ,
            RequiredSelections = [ StdLoosePions  ]
            )
        
        self.selDs23PiHlt1TIS = makeTISTOS( self.name() + "Ds23PiHlt1TIS"
                                    , self.combPiPiPi
                                    , "Hlt1.*Decision%TIS"
                                            )
        self.selDs23PiHlt2TIS = makeTISTOS( self.name() + "Ds23PiHlt2TIS"
                                            , self.selDs23PiHlt1TIS
                                            , "Hlt2.*Decision%TIS"
                                            )
       
        return self.selDs23PiHlt2TIS

    
    ## define selection for D+ -> pi mu+ mu-
    def d2PiMuMu    ( self ) :
        """
        Define selection for D+ -> pi mu+ mu-
        """
        
        if hasattr ( self , 'PiMuMu' )  : return self.PiMuMu
        
        _PiMuMuAlg = CombineParticles (
            #
            DecayDescriptors = [
            " [ D+  -> pi+  mu+ mu- ]cc " ,  # signal           
            " [ D+  -> pi-  mu+ mu+ ]cc "    # backghround  
            ] ,
            #
            DaughtersCuts = {
            'pi+' :                                 self._config['PionCuts']
            } , 
            #
            Preambulo       =                       self._config [ 'Preambulo' ] , 
            # 
            CombinationCut  = """
            ( APT > 2 * GeV         ) &
            ( ADAMASS ( 'D+' ) < %s ) 
            """                                   % self._config[ 'DplusMass' ] , 
            #
            MotherCut       = """
            ( chi2vx < 15 ) &
            ( ctau   > %s   )
            """                                   % self._config['DplusCTau']
            )
        
        self.PiMuMu = Selection (
            'SelDplus2PiMuMuFor' + self.name() ,
            Algorithm  = _PiMuMuAlg ,
            RequiredSelections = [ self.twoMuons () ,
                                   StdLoosePions    ]
            )
        
        #return self.PhiPi#was bug ??? JA 20110825
        return self.PiMuMu
    
    
    ## get the muon selection 
    def twoMuons ( self ) :
        """
        Get the proton selection
        """
        if hasattr ( self , 'TwoMuons' ) : return self.TwoMuons 
        
        _MuonFilter = FilterDesktop (
            Code = " ( 'mu+' == ABSID ) & " + self._config['MuonCuts'] 
            )
        
        self.Muons = Selection (
            'SelMuonsFor' + self.name ()  ,
            Algorithm          = _MuonFilter ,
            RequiredSelections = [ StdLooseMuons ]
            )
        
        ## require at least 2 muons 
        self.TwoMuons = VoidEventSelection (
            "TwoMuonsFor" + self.name()   ,
            Code      = """
            CONTAINS('%s') > 1.5
            """       % self.Muons.outputLocation() , 
            RequiredSelection = self.Muons 
            )
        #
        return self.TwoMuons


    ## get the muon selection for Tau->3mu Ds Normalization Channels
    def twoMuons_forTau23Mu ( self ) :
        """
        Get the proton selection
        """
        if hasattr ( self , 'TwoMuons_forTau23Mu' ) : return self.TwoMuons_forTau23Mu 
        
        _MuonFilter_forTau23Mu = FilterDesktop (
            Code = " ( 'mu+' == ABSID ) & " + self._config['MuonCuts_forTau23Mu'] 
            )
        
        self.Muons_forTau23Mu = Selection (
            'Sel_T23Mu_MuonsFor' + self.name ()  ,
            Algorithm          = _MuonFilter_forTau23Mu ,
            RequiredSelections = [ StdLooseMuons ]
            )
        
        ## require at least 2 muons 
        self.TwoMuons_forTau23Mu = VoidEventSelection (
            "Two_T23Mu_MuonsFor" + self.name()   ,
            Code      = """
            CONTAINS('%s') > 1.5
            """       % self.Muons_forTau23Mu.outputLocation() , 
            RequiredSelection = self.Muons_forTau23Mu 
            )
        #
        return self.TwoMuons_forTau23Mu
     

    ## get all stripping lines 
    def _lines_private ( self ) :
        
        if hasattr ( self , '_Lines' ) : return self._Lines
        
        self._Lines = [
            ##
            StrippingLine (
            "Sigma2PMuMuFor"  + self.name()            ,
            prescale = self._config['SigmaPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self._config['PrimaryVertices'] ,
            algos    = [ self.sigma() ]
            ) ,
            ##
            #StrippingLine (
            #"Ds2PhiPiFor"  + self.name ()              ,
            #prescale = self._config['DsPrescale'     ] , ## ATTENTION! Prescale here !!
            #checkPV  = self._config['PrimaryVertices'] ,
            #algos    = [ self.ds2PhiPi () ]
            #) ,
            ##
            #StrippingLine (
            #"Ds2PiPiPiFor"  + self.name ()              ,
            #prescale = self._config['Ds3PiPrescale'     ] , ## ATTENTION! Prescale here !!
            #checkPV  = self._config['PrimaryVertices'] ,
            #algos    = [ self.ds2PiPiPi () ]
            #) ,
            ##
            StrippingLine (
            "Dplus2PiMuMuFor"  + self.name ()          ,
            prescale = self._config['DplusPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self._config['PrimaryVertices'] ,
            algos    = [ self.d2PiMuMu () ]
            ) 
            ]
        #
        return self._Lines


# =============================================================================
## the simplest ``default-configuration''
default_config = {
    'SigmaPrescale'  : 1.00 ,
    'DsPrescale'     : 1.00 ,
    'Ds3PiPrescale'  : 1 ,
    'DplusPrescale'  : 1.00 ,
    }

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print ' The output locations for default configuration: '
    _conf = StrippingHyperCPXConf( "HyperCPX", config = default_config  ) 
    for l in _conf.lines() : print ' \t ', l.outputLocation  () , l 
    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================
