#!/usr/bin/env python
# =============================================================================
# @file StripingSelections/StrippingV0.py
#
# Configuration file for V0 ( K0S and  Lambda0) stripping.
#
# This stripping is intended for 2009 data, and probably
# it can serve as some starting point for refined stripping for 2010 data
#
# Acknowledgemens:
#
#   - Matt & Manuel for track quality definition
#   - Matt for maximum-Z cut 
#
# For more information see:
#
#  http://indico.cern.ch/conferenceDisplay.py?confId=74536
#  http://indico.cern.ch/materialDisplay.py?contribId=13&sessionId=2&materialId=0&confId=78767
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date 2010-01-14
# =============================================================================
"""

Configuration file for V0 ( K0S and  Lambda0) stripping.

This stripping is intended for 2009 data, and probably
 it can serve as some starting point for refined stripping for 2010 data

Acknowledgemens:

   - Matt & Manuel for track quality definition
   - Matt for maximum-Z cut 

For more information see:

  http://indico.cern.ch/conferenceDisplay.py?confId=74536
  http://indico.cern.ch/materialDisplay.py?contribId=13&sessionId=2&materialId=0&confId=78767

Usage of configurable is fairly trivial:

  v0 = StrippingV0Conf ( .... )

Get the list of all configured lines for K0S :

  lines_for_K0S     =  v0.K0S()

Get the list of all configured lines for Lambda0 :

  lines_for_Lambda0 =  v0.Lambda0 ()


Get the list of *ALL* configured lines
    
   all_lines =  v0.lines ()
    

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2010-01-14'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.10 $'
# =============================================================================

from Gaudi.Configuration       import *
from LHCbKernel.Configuration  import *
from GaudiKernel.SystemOfUnits import mm, cm , MeV, GeV

# =============================================================================
# @class StrippingV0Conf
#
# Configurable for V0 ( K0S and  Lambda0) stripping.
#
# This stripping is intended for 2009 data, and probably
# it can serve as some starting point for refined stripping for 2010 data
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date 2010-01-14 
class StrippingV0Conf(LHCbConfigurableUser):
    """
    Configurable for V0 ( K0S and  Lambda0 ) stripping.
    
    This stripping is intended for 2009 data, and probably
    it can serve as some starting point for refined stripping for 2010 data
    
    Acknowledgemens:
    
       - Matt & Manuel for track quality definition
       - Matt for maximum-Z cut 
    
    For more information see:
    
       http://indico.cern.ch/conferenceDisplay.py?confId=74536
       http://indico.cern.ch/materialDisplay.py?contribId=13&sessionId=2&materialId=0&confId=78767
    
    Usage of configurable is fairly trivial:
    
       v0 = StrippingV0Conf ( .... )
    
    Get the list of all configured lines for K0S :
    
       lines_for_K0S     =  v0.K0S()
    
    Get the list of all configured lines for Lambda0 :
    
       lines_for_Lambda0 =  v0.Lambda0 ()
    
    Get the list of *ALL* configured lines
    
       all_lines =  v0.lines ()
    
       """
    __slots__ = { 
        'Use_Geo_K0S'            : True               ## Use 'Geometrical' selection of K0S
        , 'Use_Geo_Lambda'       : True               ## Use 'Geometrical' selection of Lambda0
        , 'SameSignPrescale'     : 0.10               ## Prescale same-sign combinations
        , 'K0S_DDPrescale'       : 0.20               ## Prescale K0S DD-combinations
        , 'Lambda0_DDPrescale'   : 1.00               ## Prescale Lambda0 DD-combinations
        , 'TrackCutSet'          : "NULL"             ## TrackCutsSet to be used
        , 'TrackQuality'         : " TRCHI2DOF < 25 " ## Track quality selection 
        , 'VertexChi2'           :  25                ## Cut on Vertex chi2-quality
        , 'DeltaMassK0S'         :  50 * MeV          ## Mass-window (half)-width for K0S 
        , 'DeltaMassLambda'      :  25 * MeV          ## Mass-window (half)-width for Lambda 
        , 'MaxZ'                 : 220 * cm           ## Maximal allowed vertex Z-position
        , 'DaughtersIPChi2'      :  25                ## minimal IP-chi2 for daughter particles  
        , 'LTimeFitChi2'         :  49                ## Chi2 from LifeTime Fitter
        ##
        , 'CTauK0S'              :   1 * mm           ## Cut on c*tau for K0S 
        , 'CTauK0S_DD'           :  10 * mm           ## Cut on c*tau for K0S (DD-case)
        ##
        , 'CTauLambda0'          :   5 * mm           ## Cut on c*tau for Lambda0
        , 'CTauLambda0_DD'       :  20 * mm           ## Cut on c*tau for Lambda0 (DD-case)
        ## 
        , 'Monitor'              : False              ## Activate the monitoring ?
        ##
        , "HLT"                  : "HLT_PASS_RE('Hlt1MB.*Decision')" ## HLT-cut
        }
    
    _propertyDocDct = {  
        'Use_Geo_K0S'             : """ Use 'Geometrical' selection of K0S ?       """
        , 'Use_Geo_Lambda'        : """ Use 'Geometrical' selection of Lambda0 ? """ 
        , 'SameSignPrescale'      : """ Prescale same-sign combinations          """
        , 'TrackCutSet'           : """ 'TrackCutsSet' to be used                """
        , 'TrackQuality'          : """ Track quality selection                  """ 
        , 'VertexChi2'            : """ Cut on Vertex chi2-quality               """ 
        , 'DeltaMassK0S'          : """ Mass-window (half)width for K0S          """ 
        , 'DeltaMassK0S_noPV'     : """ Mass-window (half)width for K0S_noPV     """ 
        , 'DeltaMassLambda0'      : """ Mass-window (half)width for Lambda0      """ 
        , 'DeltaMassLambda0_noPV' : """ Mass-window (half)width for Lambda0_noPV """ 
        , 'MaxZ'                  : """ Maximal allowed vertex Z-position        """ 
        , 'LTimeFitChi2'          : """ Chi2 from LifeTime Fitter                """
        , 'CTau'                  : """ Cut on c*tau                             """
        , 'CTauForDD'             : """ Cut on c*tau cut for DD-case             """
        , 'Monitor'               : """ Activate the monitoring ?                """
        , 'HLT'                   : """ LoKi functor, acting on HLT-lines        """
        }
    
    
    ## Return the list of all configured stripping lines
    #  @return the list of all configured stripping lines
    def lines   ( self ) :
        """
        Return the list of all configured stripping lines
        
        """        
        return self.K0S() + self.Lambda0()

    ## create the list of stipping lines for K0S
    #  @return the list of stripping line for K0S
    def K0S     ( self ) :
        """
        Stripping lines for K0S 
        """
        
        from CommonParticles.Utils import DefaultTrackingCuts
        DefaultTrackingCuts().useCutSet( self.getProp( 'TrackCutSet' ) )
        
        from Configurables import CombineParticles
        
        StripK0S = CombineParticles("StrippingK0S")
        
        StripK0S.InputLocations  = [ "StdNoPIDsPions" ]
        StripK0S.DecayDescriptor = "KS0 -> pi+ pi-"
        
        StripK0S.DaughtersCuts   = self._k0s_daughtersCuts () 
        StripK0S.CombinationCut  = "AM < 1.0 * GeV " 
        StripK0S.MotherCut       = self._k0s_motherCut     ()        
        StripK0S.Preambulo       = self._preambulo         ()

        if self.getProp ( 'Monitor' ) :
            
            StripK0S.Preambulo    += self._k0s_histos() 
            StripK0S.Monitor       = True
            StripK0S.HistoProduce  = True
            StripK0S.HistoPrint    = True
            StripK0S.MotherMonitor = """
            process ( switch ( LL , massLL , switch ( DD , massDD , massLD ) ) )
            >> EMPTY 
            """
            
        from StrippingConf.StrippingLine import StrippingLine

        lines = []

        line_K0 = StrippingLine (
            'K0SLine'          ,
            prescale   = 1     ,
            checkPV    = True  , ## attention! PV is required!
            postscale  = 1     ,
            HLT        = self.getProp('HLT') , 
            algos      = [ StripK0S ]
            )
        lines.append ( line_K0 ) 
        
        line_WS = None
        alg_WS  = None 
        ## use "wrong-sign" combination
        if 0 < self.getProp  ( 'SameSignPrescale' ) :
            
            alg_WS = StripK0S.clone (
                'StrippingK0S_ws'     ,
                DecayDescriptor  = "" , 
                DecayDescriptors = [ "KS0 -> pi+ pi+" , "KS0 -> pi- pi-" ]
                )
            
            line_WS = line_K0.clone ( 
                'K0S_wsLine'  ,
                algos    = [ alg_WS ] ,
                prescale = self.getProp ('SameSignPrescale') 
                
                )
            lines.append ( line_WS ) 
        
        line_DD = None 
        alg_DD  = None 
        ## use DD-case 
        if 0 < self.getProp  ( "K0S_DDPrescale" ) :
            
            alg_DD   = StripK0S.clone (
                "StrippingK0S_DD" ,
                InputLocations   = [ "StdNoPIDsDownPions" ]
                ) 
            line_DD = line_K0.clone (
                'K0S_DDLine'  ,
                algos    = [ alg_DD ] ,
                prescale = self.getProp ('K0S_DDPrescale') 
                )
            lines.append ( line_DD ) 

        ## wrong sign DD-case 
        if line_DD and line_WS :
            
            alg_DDws   = alg_WS.clone  ( "StrippingK0S_DDws" , InputLocations = [ "StdNoPIDsDownPions" ] ) 
            line_DDws  = line_WS.clone (
                'K0S_DDwsLine'  ,
                algos    = [ alg_DDws ] ,
                prescale = self.getProp ('K0S_DDPrescale') * self.getProp("SameSignPrescale" ) 
                )
            lines.append ( line_DDws ) 
            
        
        if self.getProp ( 'Use_Geo_K0S') :
            
            import StrippingSelections.StrippingV0Geo as V0Geo

            line3  = V0Geo.line_KS_all
            
            if self.getProp ( 'Monitor' ) :
                geoalg               = V0Geo.KsAllCombineGeo
                geoalg               = getConfigurable ( geoalg.name ( line3._name ) ) 
                geoalg.Preambulo    += self._k0s_histos()
                geoalg.Monitor       = True              
                geoalg.HistoProduce  = True
                geoalg.HistoPrint    = True
                geoalg.MotherMonitor = """
                process ( switch ( LL , massLL , switch ( DD , massDD , massLD ) ) )
                >> EMPTY 
                """ 
                
            lines.append ( line3 ) 

        return lines 
    
    ## create the list of stipping lines for Lambda0
    #  @return the list of stripping line for Lambda
    def Lambda0   ( self ) :
        """
        Stripping lines for Lambda0
        """
        
        from CommonParticles.Utils import DefaultTrackingCuts
        DefaultTrackingCuts().useCutSet ( self.getProp ( 'TrackCutSet' ) )
        
        from Configurables import CombineParticles
        
        StripLambda0 = CombineParticles ( "StrippingLambda0" )
        
        StripLambda0.InputLocations  = [ "StdNoPIDsPions"        ,
                                         "StdNoPIDsProtons"      ]
        StripLambda0.DecayDescriptor = " [ Lambda0 -> p+ pi-]cc"
        
        StripLambda0.DaughtersCuts   = self._lam0_daughtersCuts () 
        StripLambda0.CombinationCut  = "AM < 1.5 * GeV " 
        StripLambda0.MotherCut       = self._lam0_motherCut     ()
        StripLambda0.Preambulo       = self._preambulo          ()         

        if self.getProp ( 'Monitor' ) :
            
            StripLambda0.Preambulo    += self._lam0_histos() 
            StripLambda0.Monitor       = True
            StripLambda0.HistoProduce  = True
            StripLambda0.HistoPrint    = True
            StripLambda0.MotherMonitor = """
            process ( switch ( LL , massLL , switch ( DD , massDD , massLD ) ) ) 
            >> EMPTY 
            """

        from StrippingConf.StrippingLine import StrippingLine

        lines = []
        
        line_L0 = StrippingLine (
            'Lambda0Line'      ,
            prescale   = 1     ,
            checkPV    = True  , ## attention! PV is required!
            postscale  = 1     ,
            HLT        = self.getProp('HLT') , 
            algos      = [ StripLambda0 ]
            )
        
        lines = [ line_L0 ]

        alg_WS  = None
        line_WS = None 
        ## use wrong sign?
        if 0 < self.getProp  ( 'SameSignPrescale' ) :
            
            alg_WS  = StripLambda0.clone (
                'StrippingLambda0_ws' ,
                DecayDescriptor  = "" , 
                DecayDescriptors = [ "Lambda0 ->  p+ pi+" , "Lambda~0 -> p~- pi-" ]
                )
            
            line_WS = line_L0.clone ( 
                'Lambda0_wsLine'  ,
                algos    = [ alg_WS ] ,
                prescale = self.getProp('SameSignPrescale') 
                )
            lines.append ( line_WS ) 

        alg_DD  = None
        line_DD = None
        ## use DD-combinations
        if 0 < self.getProp  ( 'Lambda0_DDPrescale' ) :
            
            alg_DD  = StripLambda0.clone (
                'StrippingLambda0_DD' ,
                InputLocations  = [ 'StdNoPIDsDownPions' , 'StdNoPIDsDownProtons'  ]
                )
            line_DD = line_L0.clone ( 
                'Lambda0_DDLine'      ,
                algos    = [ alg_DD ] ,
                prescale = self.getProp('Lambda0_DDPrescale') 
                )
            lines.append ( line_DD ) 

        ## wrong sign DD-combinations 
        if line_DD  and line_WS :
                
            alg_DDws  = alg_WS.clone (
                'StrippingLambda0_DDws' ,
                InputLocations  = [ 'StdNoPIDsDownPions' , 'StdNoPIDsDownProtons'  ]
                )
            line_DDws = line_WS.clone ( 
                'Lambda0_DDwsLine'      ,
                algos    = [ alg_DDws ] ,
                prescale = self.getProp('SameSignPrescale') * self.getProp('Lambda0_DDPrescale') 
                )
            lines.append ( line_DDws ) 
                
        if self.getProp ( 'Use_Geo_Lambda') :
            
            import StrippingSelections.StrippingV0Geo as V0Geo
            
            line3  = V0Geo.line_lambda_all
            if self.getProp ( 'Monitor' ) : 
                geoalg               = V0Geo.LambdaAllCombineGeo
                geoalg               = getConfigurable ( geoalg.name ( line3._name ) ) 
                geoalg.Preambulo    += self._lam0_histos()
                geoalg.Monitor       = True              
                geoalg.HistoProduce  = True
                geoalg.HistoPrint    = True
                geoalg.MotherMonitor = """
                process ( switch ( LL , massLL , switch ( DD , massDD , massLD ) ) )
                >> EMPTY 
                """ 
                
            lines.append ( line3 ) 
            
        return lines   

    ## define daughter cuts for K0S 
    def _k0s_daughtersCuts ( self ) :
        """
        Define 'DaughtersCuts' for K0S
        """
        cuts = {}
        cuts [''] = " ( 2.0 * GeV < P) & ( ISLONG | ISDOWN ) & ( %(TrackQuality)s ) & ( MINIPCHI2WITHDV() > %(DaughtersIPChi2)g ) " % self.getProps()
        return cuts
    
    ## define daughter cuts for Lambda0
    def _lam0_daughtersCuts ( self ) :
        """
        Define 'DaughtersCuts' for Lambda0
        """
        return self._k0s_daughtersCuts () 

    ## define "MotherCut" for K0s 
    def _k0s_motherCut      ( self ) :
        """
        Define 'MotherCut' for K0S
        """
        cut       = """
        ( ADMASS ( 'KS0') < %(DeltaMassK0S)g  )
        & in_range ( 0 , VFASPF ( VCHI2 ) ,  %(VertexChi2)g   )
        & ( VFASPF ( VZ   ) < %(MaxZ)g          )
        & in_range ( 0 , BPVLTFITCHI2()   ,  %(LTimeFitChi2)g )
        & ( BPVLTIME()*c_light > switch ( DD , %(CTauK0S_DD)g , %(CTauK0S)g ) ) 
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' )  
    
    ## define "MotherCut" for Lambda0
    def _lam0_motherCut      ( self ) :
        """
        Define 'MotherCut' for Lambda0
        """
        cut       = """
        ( ADMASS ( 'Lambda0')   < %(DeltaMassLambda)g )
        & in_range ( 0 , VFASPF ( VCHI2 ) ,  %(VertexChi2)g   )
        & ( VFASPF ( VZ       ) < %(MaxZ)g            )
        & in_range ( 0 , BPVLTFITCHI2()   ,  %(LTimeFitChi2)g )
        & ( BPVLTIME()*c_light > switch ( DD , %(CTauLambda0_DD)g , %(CTauLambda0)g ) ) 
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

    ## define the common preambulo 
    def _preambulo ( self ) :
        """
        define the common preambulo
        """
        return [
            ## import c_light
            "from GaudiKernel.PhysicalConstants import c_light" ,
            ## define DD-category of K0S 
            "DD =    CHILDCUT ( ISDOWN , 1 ) & CHILDCUT ( ISDOWN , 2 ) " ,
            ## define LL-category of K0S 
            "LL =    CHILDCUT ( ISLONG , 1 ) & CHILDCUT ( ISLONG , 2 ) " ,
            ## define LD-category of K0S 
            "LD =  ( 1 == NINTREE ( ISLONG ) ) & ( 1 == NINTREE ( ISDOWN ) ) " , 
            ## redefine track chi2/nDoF 
            "TRCHI2DOF  = monitor ( TRCHI2DOF , 'chi2/nDoF' , LoKi.Monitoring.ContextSvc) " 
            ]
        
    ## define the list of K0S monitoring histograms 
    def _k0s_histos ( self ) :
        """
        Define the list of K0S monitoring histograms 
        """
        
        return [
            ## define historam type (shortcut)
            "Histo  = Gaudi.Histo1DDef"  ,
            ## monitor LL-case
            "massLL     = monitor ( M / GeV , Histo ( 'K0S, LL-case' , 0.4 , 0.6 , 200 ) , 'LL'     ) " ,
            ## monitor DD-case
            "massDD     = monitor ( M / GeV , Histo ( 'K0S, DD-case' , 0.4 , 0.6 , 200 ) , 'DD'     ) " ,
            ## monitor LD-case
            "massLD     = monitor ( M / GeV , Histo ( 'K0S, LD-case' , 0.4 , 0.6 , 200 ) , 'LD'     ) " 
            ]

    ## define the list of Lambda0 monitoring histograms 
    def _lam0_histos ( self ) :
        """
        Define the list of Lambda0 monitoring histograms 
        """
        return  [
            ## define historam type (shortcut)
            "Histo  = Gaudi.Histo1DDef"  ,
            ## monitor LL-case
            "massLL     = monitor ( M / GeV , Histo ( 'Lambda0, LL-case' , 1.080 , 1.180 , 200 ) , 'LL'     ) " ,
            ## monitor DD-case
            "massDD     = monitor ( M / GeV , Histo ( 'Lambda0, DD-case' , 1.080 , 1.180 , 200 ) , 'DD'     ) " ,
            ## monitor LD-case
            "massLD     = monitor ( M / GeV , Histo ( 'Lambda0, LD-case' , 1.080 , 1.180 , 200 ) , 'LD'     ) " 
            ]
    

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d



from HltLine.HltLine import prnt , len1


def __enroll__ ( self       ,   ## the object
                 level = 0  ,   ## the recursion level
                 lst   = [] ) : ## the major properties  
    """
    The helper function for narcissic self-print of sequences  & algorithms 
    @param self  the object to be inspected
    @param level the recursion level
    @param lst   the list of major properties/attributes 
    @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    @date 2008-08-06
    
    """
    
    if type(self) == str :
        cfg = string2Configurable(self)
        if cfg : self = cfg
    if hasattr ( self , 'sequencer' ) :
        return __enroll__ ( self.sequencer() , level )

    _tab = 50
    _indent_ = ('%-3d'%level) + level * '   ' 
    try:     line = _indent_ + self.name ()
    except:  line = _indent_ + ( self if type(self) == str else '<UNKNOWN>' )
        
    if len1(line)>( _tab-1): line +=  '\n'+ _tab*' '
    else :                   line +=  (_tab-len1(line))*' '
    if hasattr ( self , 'getType' ) : line +=  '%-25.25s'%self.getType()
    else                            : line +=  '%-25.25s'%type(self)

    line = prnt ( self , lst , line, l1 = _tab+25 ) + '\n'
    
    # use the recursion 
    if hasattr ( self , 'Members'  ) :
        for _m in getattr(self,'Members') : line += __enroll__ ( _m , level + 1 , lst ) 
    if hasattr ( self , '_algos'   ) :
        for _m in getattr(self,'_algos')  : line += __enroll__ ( _m , level + 1 , lst ) 

    from StrippingConf.StrippingLine import StrippingLine
    
    if type(self) is StrippingLine :
        for i in [ 'Prescale','ODIN','L0DU','HLT','Filter0','Filter1','Postscale' ] :
            if hasattr(self,i) : line += __enroll__( getattr(self,i), level + 1, lst )

    return line



if '__main__' == __name__ :

    print __doc__
    print 'Author : %s ' % __author__
    print 'Date   : %s ' % __date__
    print __version__

    conf = StrippingV0Conf()
    
    print conf

    props = [ 'DaughtersCuts'   ,
              'CombinationCut'  ,
              'MotherCut'       ,
              'DecauDescriptor' ,
              'DecayDescriptors'] 
                           
    for l in conf.K0S     () : print __enroll__ ( l , lst = props ) 
    for l in conf.Lambda0 () : print __enroll__ ( l , lst = props )
    
# =============================================================================
# The END 
# =============================================================================

##    2: Stream                StrippingStreamV0: 2118
##      3: Line                 StrippingK0SLine: 1165
##      4: Line              StrippingK0S_wsLine: 936
##      5: Line            StrippingKSAllGeoLine: 1017
##      6: Line             StrippingLambda0Line: 1435
##      7: Line          StrippingLambda0_wsLine: 1305
##      8: Line        StrippingLambdaAllGeoLine: 690
## ========================== SUMMARY ================================
## Global decision:           StrippingGlobal: 2118
## Stream                   StrippingStreamV0: 2118
## -------------------------------------------------------------------

## + mips 
##    2: Stream                StrippingStreamV0: 1442
##      3: Line                 StrippingK0SLine: 811
##      4: Line              StrippingK0S_wsLine: 471
##      5: Line            StrippingKSAllGeoLine: 1017
##      6: Line             StrippingLambda0Line: 574
##      7: Line          StrippingLambda0_wsLine: 463
##      8: Line        StrippingLambdaAllGeoLine: 690
## ========================== SUMMARY ================================
## Global decision:           StrippingGlobal: 1442
## Stream                   StrippingStreamV0: 1442

## -------------------------------------------------------------------
## -------------------------------------------------------------------
##    2: Stream                StrippingStreamV0: 1197
##      3: Line                 StrippingK0SLine: 641
##      4: Line            StrippingKSAllGeoLine: 1017
##      5: Line             StrippingLambda0Line: 340
##      6: Line        StrippingLambdaAllGeoLine: 690
## ========================== SUMMARY ================================
## Global decision:           StrippingGlobal: 1197
## Stream                   StrippingStreamV0: 1197
## -------------------------------------------------------------------


##  1: Global decision           StrippingGlobal: 625
## -------------------------------------------------------------------
## -------------------------------------------------------------------
##    2: Stream                StrippingStreamV0: 625
##      3: Line                 StrippingK0SLine: 519
##      4: Line             StrippingLambda0Line: 264
## ========================== SUMMARY ================================
## Global decision:           StrippingGlobal: 625
## Stream                   StrippingStreamV0: 625
## -------------------------------------------------------------------
