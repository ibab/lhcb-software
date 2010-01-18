#!/usr/bin/env python
# =============================================================================
# @file StripingSelections/StrippingV0.py
#
# Configuration file for V0 ( K0S and  Lambda0) stripping.
#
# This stripping is intended for 2009 data, and probably
# it can serve as some starting point for refined stripping for 2010 data
#
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
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $'
# =============================================================================

from Gaudi.Configuration       import *
from LHCbKernel.Configuration  import *
from GaudiKernel.SystemOfUnits import mm, cm , MeV, GeV

# =============================================================================
# @class StrippingV0Conf 
# Configurable for V0 ( K0S and  Lambda0) stripping.
# This stripping is intended for 2009 data, and probably
# it can serve as some starting point for refined stripping for 2010 data
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
        'Use_noPV_V0'            : True               ## Use V0 for 'no-PV'-events ?
        , 'Use_Geo_K0S'          : True               ## Use 'Geometrical' selection of K0S
        , 'Use_Geo_Lambda'       : True               ## Use 'Geometrical' selection of Lambda0
        , 'AddSameSign'          : True               ## Use  same-sign combinations 
        , 'TrackQuality'         : " TRCHI2DOF < 10 " ## Track quality selection 
        , 'VertexChi2'           : 25                 ## Cut on Vertex chi2-quality
        , 'DeltaMassK0S'         : 100 * MeV          ## Mass-window (half)width for K0S 
        , 'DeltaMassK0S_noPV'    : 100 * MeV          ## Mass-window (half)width for K0S_noPV 
        , 'DeltaMassLambda'      : 100 * MeV          ## Mass-window (half)width for Lambda 
        , 'DeltaMassLambda_noPV' : 100 * MeV          ## Mass-window (half)width for Lambda_noPV
        , 'MaxZ'                 : 220 * cm           ## Maximal allowed vertex Z-position
        , 'LTimeFitChi2'         : 100                ## Chi2 from LifeTime Fitter 
        , 'CTau'                 :  1  * mm           ## Cut on c*tau
        , 'CTauForDD'            : 10  * mm           ## Cut on c*tau for DD-case
        , 'Monitor'              : False              ## Activate the monitoring ?
        }
    
    _propertyDocDct = {  
        'Use_noPV_V0'             : """ Use V0 for 'no-PV'-events ?              """ 
        , 'Use_Geo_K0S'           : """ Use 'Geometrical' selection of K0S ?     """
        , 'Use_Geo_Lambda'        : """ Use 'Geometrical' selection of Lambda0 ? """ 
        , 'AddSameSign'           : """ Use  same-sign combinations              """
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
        from Configurables import CombineParticles
        
        StripK0S = CombineParticles("StrippingK0S")
        
        StripK0S.InputLocations  = [ "StdNoPIDsPions", "StdNoPIDsDownPions" ]
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
        
        line1 = StrippingLine (
            'K0SLine'          ,
            prescale   = 1     ,
            checkPV    = True  , ## attention! PV is required!
            postscale  = 1     ,
            algos      = [ StripK0S ]
            )
        
        lines = [ line1 ]

        ## use "wrong-sign" combination
        if self.getProp  ( 'AddSameSign' ) :

            
            wrong_sign = StripK0S.clone ( 'StrippingK0S_ws' )
            wrong_sign.DecayDescriptor  = ""
            wrong_sign.DecayDescriptors = [
                "KS0 -> pi+ pi+" , "KS0 -> pi- pi-" 
                ]
            
            line1ss = line1.clone ( 
                'K0S_wsLine'  , algos = [ wrong_sign ]
                )
            lines.append ( line1ss ) 
            
        if self.getProp ( 'Use_noPV_V0' ) :
            
            from Configurables import CheckPV
            
            noPV = CheckPV( "noPV" , MinPVs = 0 , MaxPVs = 0 )
            
            StripK0S_noPV           = StripK0S.clone ( 'StrippingK0S_noPV' ) 
            StripK0S_noPV.MotherCut = self._k0s_noPV_motherCut () 

            line2 = StrippingLine (
                'K0S_noPVLine'      ,
                prescale   = 1      ,
                checkPV    = False  , ## attention! PV is *not* required 
                postscale  = 1      ,
                algos      = [ noPV , StripK0S_noPV  ]
                )
            
            lines.append ( line2 ) 
            
            ## use "wrong-sign" combination
            if self.getProp ( 'AddSameSign' ) :
                
                wrong_sign = StripK0S_noPV.clone ( 'StrippingK0S_noPV_ws' )
                wrong_sign.DecayDescriptor  = ""
                wrong_sign.DecayDescriptors = [
                    "KS0 -> pi+ pi+" , "KS0 -> pi- pi-" 
                    ]
                
                line2ss = line2.clone ( 
                    'K0S_noPV_wsLine'  , algos = [ noPV , wrong_sign ]
                    )
                lines.append ( line2ss ) 
                            
        line3 = None 
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
        
        from Configurables import CombineParticles
        
        StripLambda0 = CombineParticles ( "StrippingLambda0" )
        
        StripLambda0.InputLocations  = [ "StdNoPIDsPions"        ,
                                         "StdNoPIDsDownPions"    ,
                                         "StdNoPIDsProtons"      ,
                                         "StdNoPIDsDownProtons"  ]
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
        
        line1 = StrippingLine (
            'Lambda0Line'      ,
            prescale   = 1     ,
            checkPV    = True  , ## attention! PV is required!
            postscale  = 1     ,
            algos      = [ StripLambda0 ]
            )
        
        lines = [ line1 ]

        if self.getProp  ( 'AddSameSign' ) :
            
            wrong_sign = StripLambda0.clone ( 'StrippingLambda0_ws' )
            wrong_sign.DecayDescriptor  = ""
            wrong_sign.DecayDescriptors = [
                "Lambda0 ->  p+ pi+" , "Lambda~0 -> p~- pi-" 
                ]
            
            line1ss = line1.clone ( 
                'Lambda0_wsLine'  , algos = [ wrong_sign ]
                )
            lines.append ( line1ss ) 
            

        if self.getProp ( 'Use_noPV_V0') :
            
            from Configurables import CheckPV
            
            noPV = CheckPV( "noPV" , MinPVs = 0 , MaxPVs = 0 )
            
            StripLambda0_noPV            = StripLambda0.clone ( 'StrippingLambda0_noPV' ) 
            StripLambda0_noPV.MotherCut  = self._lam0_noPV_motherCut () 
            
            line2 = StrippingLine (
                'Lambda0_noPVLine'      ,
                prescale   = 1      ,
                checkPV    = False  , ## attention! PV is *not* required 
                postscale  = 1      ,
                algos      = [ noPV , StripLambda0_noPV  ]
                )

            lines.append ( line2 ) 

            if self.getProp ( 'AddSameSign' ) :
                
                wrong_sign = StripLambda0_noPV.clone ( 'StrippingLambda0_noPV_ws' )
                wrong_sign.DecayDescriptor  = ""
                wrong_sign.DecayDescriptors = [
                    "Lambda0 -> p+ pi+" , "Lambda~0 -> p~- pi-" 
                    ]
                
                line2ss = line2.clone ( 
                    'Lambda0_noPV_wsLine'  , algos = [ noPV , wrong_sign ]
                    )
                lines.append ( line2ss ) 


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
        cuts [''] = " ( 1.5 * GeV < P) & ( ISLONG | ISDOWN ) & ( %(TrackQuality)s ) " % self.getProps()
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
        ( ADMASS ( 'KS0') < %(DeltaMassK0S)g  ) & in_range ( 0 , VFASPF ( VCHI2 ) ,  %(VertexChi2)g  ) &
        ( VFASPF ( VZ   ) < %(MaxZ)g ) & in_range ( 0 , BPVLTFITCHI2()  ,  %(LTimeFitChi2)g ) &
        ( BPVLTIME()*c_light > switch ( DD , %(CTauForDD)g , %(CTau)g ) ) 
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' )  

    ## define "MotherCut" for Lambda0
    def _lam0_motherCut      ( self ) :
        """
        Define 'MotherCut' for Lambda0
        """
        cut       = """
        ( ADMASS ( 'Lambda0') < %(DeltaMassLambda)g ) & in_range ( 0 , VFASPF ( VCHI2 ) ,  %(VertexChi2)g ) &
        ( VFASPF ( VZ   ) < %(MaxZ)g ) & in_range ( 0 , BPVLTFITCHI2()  ,  %(LTimeFitChi2)g ) &
        ( BPVLTIME()*c_light > switch ( DD , %(CTauForDD)g , %(CTau)g ) ) 
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

    ## define "MotherCut" for K0S-noPV 
    def _k0s_noPV_motherCut    ( self ) :
        """
        define 'MotherCut' for K0S-noPV 
        """
        cut       = """
        ( ADMASS ( 'KS0') < %(DeltaMassK0S_noPV)g  ) & in_range ( 0 , VFASPF ( VCHI2 ) ,  %(VertexChi2)g  ) &
        ( VFASPF ( VZ   ) < %(MaxZ)g )
        """
        cut = cut % self.getProps()
        return cut.replace('\n',' ') 

    ## define "MotherCut" for Lambda-noPV 
    def _lam0_noPV_motherCut    ( self ) :
        """
        define 'MotherCut' for K0S-noPV 
        """
        cut       = """
        ( ADMASS ( 'Lambda0') < %(DeltaMassLambda_noPV)g ) & in_range ( 0 , VFASPF ( VCHI2 ) , %(VertexChi2)g ) &
        ( VFASPF ( VZ   ) < %(MaxZ)g )
        """
        cut = cut % self.getProps()
        return cut.replace('\n',' ') 


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
            "massLL = monitor ( M / GeV , Histo ( 'K0S, LL-case' , 0.4 , 0.6 , 200 ) , 'LL' ) " ,
            ## monitor DD-case
            "massDD = monitor ( M / GeV , Histo ( 'K0S, DD-case' , 0.4 , 0.6 , 200 ) , 'DD' ) " ,
            ## monitor LD-case
            "massLD = monitor ( M / GeV , Histo ( 'K0S, LD-case' , 0.4 , 0.6 , 200 ) , 'LD' ) " 
            ]

    ## define the list of Lambda0 monitoring histograms 
    def _lam0_histos ( self ) :
        """
        Define the list of Lambda0 mionitoring histograms 
        """
        return  [
            ## define historam type (shortcut)
            "Histo  = Gaudi.Histo1DDef"  ,
            ## monitor LL-case
            "massLL = monitor ( M / GeV , Histo ( 'Lambda0, LL-case' , 1.030 , 1.230 , 200 ) , 'LL' ) " ,
            ## monitor DD-case
            "massDD = monitor ( M / GeV , Histo ( 'Lambda0, DD-case' , 1.030 , 1.230 , 200 ) , 'DD' ) " ,
            ## monitor LD-case
            "massLD = monitor ( M / GeV , Histo ( 'Lambda0, LD-case' , 1.030 , 1.230 , 200 ) , 'LD' ) " 
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

    props = [ 'DaughtersCuts'   , 'CombinationCut' , 'MotherCut' ,
              'DecauDescriptor' , 'DecayDescriptors'] 
                           
    for l in conf.K0S     () : print __enroll__ ( l , lst = props ) 
    for l in conf.Lambda0 () : print __enroll__ ( l , lst = props )
    
