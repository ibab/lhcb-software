#!/usr/bin/env python
# =============================================================================
## @file PhysConf/Filters.py
#
# Helper script to simpify the creation of various LoKi-filters
#
# @code 
# 
# >>> from PhysConf.Filters import LoKi_Filters
# 
# >>> fltrs = LoKi_Filters (
#       ODIN_Code  = ' in_range ( 5000 , ODIN_RUNNUM , 9000 ) '          ,
#       L0DU_Code  = ' L0_CHANNEL  ( ... )  '                            ,
#       HLT_Code   = ' HLT_PASS_RE ( 'Hlt1MBMicro.*Decision')  '         ,
#       STRIP_Code = ' HLT_PASS    ( 'StrippingBd2KstarGammaDecision') ' ,
#       VOID_Code  = ' CONTAINS    ('Rec/Vertex/Primary') == 1 '         , 
#       MC_Code    = ' count ( 'D0' == MCABSID ) > 0.5 '                 , 
#       GEN_Code   = ' count ( 'D0' ==  GABSID ) > 0.5 '                 , 
#      )
#
# >>> ## get the list of algorithms:
# >>> DaVinci ( ...
#      UserAlgorithms = fltrs.filters('MySeq')
#     )
# >>> ## or ( alternatively) get the sequencer:
# >>> DaVinci ( ...
#      UserAlgorithms = [ fltrs.sequencer('MySeq') ] 
#     )
#
#  @endcode
#
#  @see LoKi::ODINFilter 
#  @see LoKi::L0Filter 
#  @see LoKi:HDRFilter 
#  @see LoKi:VoidFilter
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2010-06-19
#
#                    $Revision$
#  Last modification $Date$
#                 by $author$
# =============================================================================
"""

Helper script to simplify the creation of various LoKi-filters


>>> from PhysConf.Filters import LoKi_Filters

>>> fltrs = LoKi_Filters (
     ODIN_Code  = ' in_range ( 5000 , ODIN_RUNNUM , 9000 ) '          ,
     L0DU _Code  = ' L0_CHANNEL  ( ... )  '                           ,
     HLT_Code   = ' HLT_PASS_RE ( 'Hlt1MBMicro.*Decision')  '         ,
     STRIP_Code = ' HLT_PASS    ( 'StrippingBd2KstarGammaDecision') ' ,
     VOID_Code  = ' CONTAINS    ('Rec/Vertex/Primary') == 1 '         , 
     MC_Code    = ' count ( 'D0' == MCABSID ) > 0.5 '                 , 
     GEN_Code   = ' count ( 'D0' ==  GABSID ) > 0.5 '                 , 
    )
    
>>> ## get the list of algorithms:
>>> DaVinci ( ...
     UserAlgorithms = fltrs.filters('MySeq')
     )
     
>>> ## or ( alternatively) get the sequencer:   
>>> DaVinci ( ...
     UserAlgorithms = [ fltrs.sequencer('MySeq') ] 
     )
     
"""
# =============================================================================
__author__   = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__     = "2010-06-19"
__version__  = "$Revision$"
__all__      = ( "LoKi_Filters" ,  )
# =============================================================================
from Gaudi.Configuration import *
from copy import deepcopy 
import logging
_log = logging.getLogger('LoKi_Filters')

# =============================================================================
## @class LoKi_Filters
#  Helper class to simplify the creation of LoKi-filters
#
# @code 
# 
# >>> from PhysConf.Filters import LoKi_Filters
# 
# >>> fltrs = Lo Ki_Filters (
#       ODIN_Code  = ' in_range ( 5000 , ODIN_RUNNUM , 9000 ) '          ,
#       L0DU_Code  = ' L0_CHANNEL  ( ... )  '                            ,
#       HLT_Code   = ' HLT_PASS_RE ( 'Hlt1MBMicro.*Decision')  '         ,
#       STRIP_Code = ' HLT_PASS    ( 'StrippingBd2KstarGammaDecision') ' ,
#       VOID_Code  = ' CONTAINS    ('Rec/Vertex/Primary') == 1 '         ,
#       MC_Code    = ' count ( 'D0' == MCABSID ) > 0.5 '                 , 
#       GEN_Code   = ' count ( 'D0' ==  GABSID ) > 0.5 ' 
#      )
#
# >>> ## get the list of algorithms:
# >>> DaVinci ( ...
#      UserAlgorithms = fltrs.filters('MySeq')
#     )
# >>> ## or ( alternatively) get the sequencer:
# >>> DaVinci ( ...
#      UserAlgorithms = [ fltrs.sequencer('MySeq') ] 
#     )
#
# @endcode
#
# @see LoKi::ODINFilter 
# @see LoKi::L0Filter 
# @see LoKi:HDRFilter 
# @see LoKi:VoidFilter
#
# @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# @date   2010-06-19
class LoKi_Filters ( object ) :
    """
    Helper class to simplify thr creation of LoKi-filters


    >>> from PhysConf.Filters import LoKi_Filters
    
    >>> fltrs = LoKi_Filters (
    ODIN_Code  = ' in_range ( 5000 , ODIN_RUNNUM , 9000 ) '          ,
    L0DU_Code  = ' L0_CHANNEL  ( ... )  '                            ,
    HLT_Code   = ' HLT_PASS_RE ( 'Hlt1MBMicro.*Decision')  '         ,
    STRIP_Code = ' HLT_PASS    ( 'StrippingBd2KstarGammaDecision') ' ,
    VOID_Code  = ' CONTAINS    ('Rec/Vertex/Primary') == 1 ' 
    MC_Code    = ' count ( 'D0' == MCABSID ) > 0.5 ' 
    GEN_Code   = ' count ( 'D0' ==  GABSID ) > 0.5 ' 
    )
    
    >>> ## get the list of algorithms:
    >>> DaVinci ( ...
    UserAlgorithms = fltrs.filters('MySeq')
    )
    
    >>> ## or ( alternatively) get the sequencer:
    >>> DaVinci ( ...
    UserAlgorithms = [ fltrs.sequencer('MySeq') ] 
    )
    
    """
    # =========================================================================
    ## Constructor from all properties 
    #  @code
    #
    #  >>> fltrs = LoKi_Filters (
    #       HLT_Code   = ' HLT_PASS_RE ( 'Hlt1MBMicro.*Decision')  '         ,
    #       STRIP_Code = ' HLT_PASS    ( 'StrippingBd2KstarGammaDecision') ' ,
    #       VOID_Code  = ' CONTAINS    ('Rec/Vertex/Primary') == 1 ' 
    #       )
    #  @endcode 
    def __init__ ( self , **kwargs ) :
        
        self.__ODIN_Code        = kwargs.pop ( 'ODIN_Code'        , ''   )
        self.__ODIN_Preambulo   = kwargs.pop ( 'ODIN_Preambulo'   , []   )
        self.__ODIN_Location    = kwargs.pop ( 'ODIN_Location'    , ''   )
        
        self.__L0DU_Code        = kwargs.pop ( 'L0DU_Code'        , ''   )
        self.__L0DU_Preambulo   = kwargs.pop ( 'L0DU_Preambulo'   , []   )
        self.__L0DU_Location    = kwargs.pop ( 'L0DU_Location'    , ''   )
        
        self.__HLT_Code         = kwargs.pop ( 'HLT_Code'         , ''   )
        self.__HLT_Preambulo    = kwargs.pop ( 'HLT_Preambulo'    , []   )
        self.__HLT_Location     = kwargs.pop ( 'HLT_Location'     , ''   )

        self.__STRIP_Code       = kwargs.pop ( 'STRIP_Code'       , ''   )
        self.__STRIP_Preambulo  = kwargs.pop ( 'STRIP_Preambulo'  , []   )
        self.__STRIP_Location   = kwargs.pop ( 'STRIP_Location'   , ''   )
        
        self.__VOID_Code        = kwargs.pop ( 'VOID_Code'        , ''   )
        self.__VOID_Preambulo   = kwargs.pop ( 'VOID_Preambulo'   , []   )

        self.__MC_Code          = kwargs.pop ( 'MC_Code'          , ''   )
        self.__MC_Preambulo     = kwargs.pop ( 'MC_Preambulo'     , []   )
        self.__MC_Location      = kwargs.pop ( 'MC_Location'      , ''   )
        
        self.__GEN_Code         = kwargs.pop ( 'GEN_Code'         , ''   )
        self.__GEN_Preambulo    = kwargs.pop ( 'GEN_Preambulo'    , []   )
        self.__GEN_Location     = kwargs.pop ( 'GEN_Location'     , ''   )

        self.__kwargs           = kwargs


    # =========================================================================
    ## construct the list of filters
    #  @code
    #
    #  >>> fltrs = LoKi_Filters ( ... )
    #  >>> lst   = fltrs.filters ( 'MySeq' )
    #    
    #  >>> seq.Members += lst
    #
    #  @endcode 
    #  @param name (INPUT) the basic name for the sequence
    #  @return the list of constructed algorothms
    def filters ( self , name , **kwargs ) :
        """
        construct the list of filters

        >>> fltrs = LoKi_Filters ( ... )
        >>> lst   = fltrs.filters ( 'MySeq' , MeasureTime = True )
        
        >>> seq.Members += lst
        
        """        
        _seq = []


        if self.__MC_Code   :
            #
            from Configurables import LoKi__MCFilter
            _mc = LoKi__MCFilter (
                name + '_MC'   , 
                Code  = self.__MC_Code 
                )
            if self.__MC_Preambulo : _mc.Preambulo = self.__MC_Preambulo
            if self.__MC_Location  : _mc.Location  = self.__MC_Location
            _seq += [ _mc ]
            
        if self.__GEN_Code   :
            #
            from Configurables import LoKi__GenFilter
            _gen = LoKi__GenFilter (
                name + '_GEN'   , 
                Code  = self.__GEN_Code 
                )
            if self.__GEN_Preambulo : _gen.Preambulo = self.__GEN_Preambulo
            if self.__GEN_Location  : _gen.Location  = self.__GEN_Location
            _seq += [ _gen ]

        if self.__ODIN_Code :
            #
            from Configurables import LoKi__ODINFilter
            _odin = LoKi__ODINFilter (
                name + '_ODIN'   , 
                Code  = self.__ODIN_Code 
                )
            if self.__ODIN_Preambulo : _odin.Preambulo = self.__ODIN_Preambulo
            if self.__ODIN_Location  : _odin.Location  = self.__ODIN_Location
            _seq += [ _odin ]
            
        if self.__L0DU_Code :
            #
            from Configurables import LoKi__L0Filter
            _l0du = LoKi__L0Filter (
                name + '_L0DU'    , 
                Code  = self.__L0DU_Code 
                )
            if self.__L0DU_Preambulo : _l0du.Preambulo = self.__L0DU_Preambulo
            if self.__L0DU_Location  : _l0du.Location  = self.__L0DU_Location
            _seq += [ _l0du ]

        if self.__HLT_Code :
            # 
            from Configurables import LoKi__HDRFilter
            _hlt = LoKi__HDRFilter (
                name + '_HLT'    , 
                Code  = self.__HLT_Code 
                )
            if self.__HLT_Preambulo : _hlt.Preambulo = self.__HLT_Preambulo
            if self.__HLT_Location  : _hlt.Location  = self.__HLT_Location
            _seq += [ _hlt ]

        if self.__STRIP_Code :
            #
            from Configurables import LoKi__HDRFilter
            _strip = LoKi__HDRFilter (
                name + '_STRIP'                     , 
                Code     = self.__STRIP_Code        ,
                Location = '/Event/Strip/Phys/DecReports' 
                )
            if self.__STRIP_Preambulo : _strip.Location = self.__STRIP_Preambulo
            if self.__STRIP_Location  : _strip.Location = self.__STRIP_Location
            _seq += [ _strip ]
            
        if self.__VOID_Code :

            from Configurables import LoKi__VoidFilter
            _void = LoKi__VoidFilter (
                name + '_VOID' , 
                Code = self.__VOID_Code                     
                )
            if self.__VOID_Preambulo : _void.Preambulo = self.__VOID_Preambulo
            _seq += [ _void ]

        ## Propagate the remaing properties
        args = deepcopy ( self.__kwargs )
        args.update ( kwargs )
            
        _used = [] 
        for key in args :
            for a in _seq :
                if hasattr ( a  , key ) :
                    setattr ( a , key , args[key] )
                    _used.append ( key )
                    
        for key in _used  : args.pop ( key , None ) 
        if args : _log.error ( "LoKiFilters: Unable propagate properties: %s " % args.keys() )
            
        return  _seq 

    # =========================================================================
    ## get the GaudiSequencer
    #  @code
    #
    #  >>> fltrs = LoKi_Filters ( ... )
    #  >>> seq   = fltrs.sequence ( 'MySeq' , MeasureTime = True )
    #
    #  @endcode 
    #  @param name (INPUT) the sequencer name
    #  @return the constructed sequencer
    def sequence  ( self , name , **kwargs ) :
        """
        Get sequence of constructed algorithms
                
        >>> fltrs = LoKi_Filters ( ... )
        >>> seq   = fltrs.sequence ( 'MySeq' , MeasureTime = True )
        
        """
        return self.sequencer ( name , **kwargs ) 

    # =========================================================================
    ## get the GaudiSequencer
    #  @code
    #
    #  >>> fltrs = LoKi_Filters ( ... )
    #  >>> seq   = fltrs.sequencer ( 'MySeq' , MeasureTime = True )
    #
    #  @endcode 
    #  @param name (INPUT) the sequencer name
    #  @return the constructed sequencer
    def sequencer ( self , name , **kwargs ) :
        """
        get sequencer constructed algorithms

        >>> fltrs = LoKi_Filters ( ... )
        >>> seq   = fltrs.sequencer ( 'MySeq' , MeasureTime = True )
                
        """
        from Configurables import GaudiSequencer

        args = deepcopy ( kwargs )
        seq  = GaudiSequencer (
            name      ,
            Members = self.filters ( name , **args )
            ) 
        
        for key in kwargs :
            if hasattr ( seq , key ) :
                setattr ( seq , key , kwargs [ key ] )
                
        return seq


## ============================================================================
if '__main__' == __name__ :
    
    ## make printout of the own documentation
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  

# =============================================================================
# The END 
# =============================================================================
