#!/usr/bin/env python
# =============================================================================
# @file BenderTools/TisTos.py
#
# Decorate the algorithm with helper methods for TisTosTob'ing 
#  
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2011-06-21
#
#                   $Revision: 138072 $
# Last modification $Date: 2012-04-04 16:41:10 +0200 (Wed, 04 Apr 2012) $
#                by $Author: ibelyaev $
# =============================================================================
"""
Decorate the algorithm with helper methods for TisTosTob'ing 

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campain of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__version__ = "$Revision: 138072 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2011-06-07"
# =============================================================================
__all__= (
    'decision'     , ## collect trigger decisions
    'trgDecs'      , ## print trigger statistics 
    'tisTos'       , ## fill N-tuple with TisTos information
    )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# ==============================================================================
from   LoKiCore.basic  import cpp
from   Bender.Main     import SUCCESS, Algo
import LHCbMath.Types 
# ==============================================================================
ITriggerSelectionTisTos = cpp.ITriggerSelectionTisTos
# ==============================================================================
## Collect important trigger lines
#
#  @code
#
#  particles = ...
#  for D in particles : 
#      self.decisions ( D               ,
#                       triggers        ,
#                       self.l0tistos   ,
#                       self.tistos     )
#
#  @endcode 
#
#  "triggers" here is in/out-argument (dictionary) that accumulates the
#  information abot trigger lines ...
#
#  Technically it is useful to keep it as ``per-particle-type''
#  dictionary
#
#  @code
#
#  def initialize ( self ) :
#     ...
#     self.triggers        = {}
#     self.triggers['B']   = {}
#     self.triggers['H']   = {}
#     self.triggers['ppi'] = {}
#     ...
#
#  def analyse ( self ) :
#     ...
#
#     particles = ...
#     for B in particles : 
#         self.decisions ( B                  ,
#                          self.triggers['B'] ,
#                          self.l0tistos      ,
#                          self.tistos        )
#     ...
#
#  @endcode 
#
#
#  @param p        (input)         the particle
#  @param triggers (update/output) the triggers
#  @param l0tistos (input)         the tool for L0-tistos 
#  @param   tistos (input)         the tool for Hlt1/Hlt2-tistos
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-21
def decisions ( self             ,
                p                ,
                triggers         , 
                l0tistos  = None ,
                tistos    = None ) : 
    """
    Collect the important trigger lines

    ... 
    particles = ...
    for D in particles : 
        self.decisions ( D               ,
        triggers        ,
        self.l0tistos   ,
        self.tistos     )
        
    ``triggers'' here is in/out-argument (dictionary) that accumulates the
    information abot trigger lines ...
    
    Technically it is useful to keep it as ``per-particle-type''
    dictionary
    
    def initialize ( self ) :
        ...
        self.triggers        = {}
        self.triggers['B']   = {}
        self.triggers['H']   = {}
        self.triggers['psi'] = {}
        ...
        return SUCCESS
    
    def analyse ( self ) :
        ... 
        particles = ...
        for B in particles : 
            self.decisions ( B                  ,
                             self.triggers['B'] ,
                             self.l0tistos      ,
                             self.tistos        )
        ...
        return SUCCESS
    """
    
    if not triggers.has_key ( 'L0_TOS'   ) : triggers [ 'L0_TOS'   ] = {}
    if not triggers.has_key ( 'L0_TIS'   ) : triggers [ 'L0_TIS'   ] = {}
    if not triggers.has_key ( 'L0_TPS'   ) : triggers [ 'L0_TPS'   ] = {}
    if not triggers.has_key ( 'Hlt1_TOS' ) : triggers [ 'Hlt1_TOS' ] = {}
    if not triggers.has_key ( 'Hlt1_TIS' ) : triggers [ 'Hlt1_TIS' ] = {}
    if not triggers.has_key ( 'Hlt1_TPS' ) : triggers [ 'Hlt1_TPS' ] = {}
    if not triggers.has_key ( 'Hlt2_TOS' ) : triggers [ 'Hlt2_TOS' ] = {}
    if not triggers.has_key ( 'Hlt2_TIS' ) : triggers [ 'Hlt2_TIS' ] = {}
    if not triggers.has_key ( 'Hlt2_TPS' ) : triggers [ 'Hlt2_TPS' ] = {}


    if not l0tistos and hasattr ( self , 'l0tistos' ) : l0tistos = self.l0tistos
    if not   tistos and hasattr ( self ,   'tistos' ) :   tistos = self.  tistos
    
    if not l0tistos : return self.Error ( 'decisions: Invalid "l0tistos"-argument ')
    if not   tistos : return self.Error ( 'decisions: Invalid   "tistos"-argument ')
    
            
    if hasattr ( p , 'particle' ) : p = p.particle()
    
    ## TOS 
    trigs = l0tistos.triggerSelectionNames ( p  , 'L0.*Decision'  ,
                                             ITriggerSelectionTisTos.kTrueRequired , ## decision 
                                             ITriggerSelectionTisTos.kAnything     , ## TIS
                                             ITriggerSelectionTisTos.kTrueRequired , ## TOS
                                             ITriggerSelectionTisTos.kAnything     ) ## TPS
    
    tos = triggers['L0_TOS'] 
    if not tos.has_key('TOTAL') : tos['TOTAL']  = 1
    else                        : tos['TOTAL'] += 1
    for t in trigs :
        if not tos.has_key(t) : tos[t]  = 1 
        else                  : tos[t] += 1
        

    ## TIS 
    trigs = l0tistos.triggerSelectionNames ( p  , 'L0.*Decision'  ,
                                             ITriggerSelectionTisTos.kTrueRequired , ## decision 
                                             ITriggerSelectionTisTos.kTrueRequired , ## TIS
                                             ITriggerSelectionTisTos.kAnything     , ## TOS
                                             ITriggerSelectionTisTos.kAnything     ) ## TPS
    tis = triggers['L0_TIS']
    if not tis.has_key('TOTAL') : tis['TOTAL']  = 1
    else                        : tis['TOTAL'] += 1
    for t in trigs :
        if not tis.has_key(t) : tis[t]  = 1 
        else                  : tis[t] += 1 

    ## TPS 
    trigs = l0tistos.triggerSelectionNames ( p  , 'L0.*Decision'  ,
                                             ITriggerSelectionTisTos.kTrueRequired , ## decision 
                                             ITriggerSelectionTisTos.kAnything     , ## TIS
                                             ITriggerSelectionTisTos.kAnything     , ## TOS
                                             ITriggerSelectionTisTos.kTrueRequired ) ## TPS
    tis = triggers['L0_TPS']
    if not tis.has_key('TOTAL') : tis['TOTAL']  = 1
    else                        : tis['TOTAL'] += 1
    for t in trigs :
        if not tis.has_key(t) : tis[t]  = 1 
        else                  : tis[t] += 1 

        
    for i in ( 'Hlt1' , 'Hlt2' ) :

        ## TOS 
        trigs = tistos.triggerSelectionNames ( p  , i + '.*Decision'  ,
                                               ITriggerSelectionTisTos.kTrueRequired , ## decision 
                                               ITriggerSelectionTisTos.kAnything     , ## TIS
                                               ITriggerSelectionTisTos.kTrueRequired , ## TOS
                                               ITriggerSelectionTisTos.kAnything     ) ## TPS
        tos = triggers[ i + '_TOS']
        if not tos.has_key('TOTAL') : tos['TOTAL']  = 1
        else                        : tos['TOTAL'] += 1
        for t in trigs :
            if not tos.has_key(t) : tos[t]  = 1 
            else                  : tos[t] += 1 

        ## TIS 
        trigs = tistos.triggerSelectionNames ( p  , i + '.*Decision'  ,
                                               ITriggerSelectionTisTos.kTrueRequired , ## decision 
                                               ITriggerSelectionTisTos.kTrueRequired , ## TIS
                                               ITriggerSelectionTisTos.kAnything     , ## TOS
                                               ITriggerSelectionTisTos.kAnything     ) ## TPS
        tis = triggers[ i + '_TIS']
        if not tis.has_key('TOTAL') : tis['TOTAL']  = 1
        else                        : tis['TOTAL'] += 1
        for t in trigs :
            if not tis.has_key(t) : tis[t]  = 1 
            else                  : tis[t] += 1
            
        ## TPS 
        trigs = tistos.triggerSelectionNames ( p  , i + '.*Decision'  ,
                                               ITriggerSelectionTisTos.kTrueRequired , ## decision 
                                               ITriggerSelectionTisTos.kAnything     , ## TIS
                                               ITriggerSelectionTisTos.kAnything     , ## TOS
                                               ITriggerSelectionTisTos.kTrueRequired ) ## TPS
        tis = triggers[ i + '_TPS']
        if not tis.has_key('TOTAL') : tis['TOTAL']  = 1
        else                        : tis['TOTAL'] += 1
        for t in trigs :
            if not tis.has_key(t) : tis[t]  = 1 
            else                  : tis[t] += 1 
            
    return SUCCESS


# ============================================================================
## print the trigger decisions, collected by ``decisions'' method
#
#  @code
#
#   self.trgDecs ( self.triggers )
#
#  @endcode 
#  
#  @param p         (input)         the particle
#  @param triggers  (update/output) the triggers
#  @param l0tistos  (input)         the tool for L0-tistos 
#  @param   tistos  (input)         the tool for Hlt1/Hlt2-tistos
#  @param   dbname  (input)         the name of output shelve-file
#
#  It also dumps the statistics into shelve data base 
# 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-21 
def trgDecs ( self            ,
              triggers = None ,
              db_name  = ''   ) :
    """
    
    Print trigger decisions, collected by ``decisions'' method
    It also dumps the statistics into shelve data base 
    ...
    self.trgDec ( self.triggers )
    ... 
    
    """
    if not triggers and hasattr ( self , 'triggers' ) : triggers = self.triggers
        
    if not isinstance ( triggers , dict ) : 
        return self.Warning ( 'Invalid argument "triggers"' , SUCCESS )

    #
    #
    if not db_name :
        db_name = self.name () + '_tistos'
    #
    txt_file = db_name + '.txt'
    db_file  = db_name + '.db'
    #
    tistos_print ( triggers , self.Warning ) ## std-out 
    tistos_print ( triggers , self.Warning , open ( txt_file , 'w' ) ) ## txt-file
    #
    print 90*'*'
    #
    import shelve
    db = shelve.open ( db_file )
    db [ 'tistos' ] = triggers
    db.close()
    
    return SUCCESS

# =============================================================================
## fill TisTos information to N-tuple 
#
#  @code
#
#    particles = ...
#
#    for D in particles : 
#        self.tisTos ( D     ,
#                      tup   ,
#                      'D0_' ,
#                      lines             ,
#                      self.l0tistos     ,
#                      self.tistos       )
#  @endcode
# 
#  "lines" here is a dictionary of lines (or regex-patterns) with
#   following keys:
#   - <c>L0TOS</c> 
#   - <c>L0TIS</c>
#   - <c>Hlt1TOS</c>
#   - <c>Hlt1TIS</c>
#   - <c>Hlt2TOS</c>
#   - <c>Hlt2TIS</c>
#
#  e.g.
#  @code
#    lines = {}
#    lines ['L0TOS'  ] = 'L0HadronDecision'
#    lines ['L0TIS'  ] = 'L0(Hadron|Muon|DiMuon)Decision'
#    lines ['Hlt1TOS'] = ...
#    lines ['Hlt1TIS'] = 'Hlt1Topo.*Decision'
#    lines ['Hlt2TOS'] = ...
#    lines ['Hlt2TIS'] = ...
#
#  @endcode
#
#  Technically it is useful to keep it as ``per-particle-type''
#  dictionary
#
#  @code
#
#  def initialize ( self ) :
#     ...
#     self.lines           = {}
#     self.lines ['B'  ]   = {}
#     self.lines ['B'  ]['L0TOS'] = ...
#     self.lines ['B'  ]['L0TOS'] = ...
#     ...
#     self.lines ['psi']   = {}
#     self.lines ['psi']['L0TOS'] = ...
#     ...
#
#  def analyse ( self )
#
#    particles = ...
#
#    for B in particles : 
#        self.tisTos ( B     ,
#                      tup   ,
#                      'B0_' ,
#                      self.lines['B']   ,
#                      self.l0tistos     ,
#                      self.tistos       )
#  @endcode
#
#
#  The function adds few columns to n-tuple, themost important are:
#   - "label"l0tos_1   : corresponds to   'L0-TOS'  
#   - "label"l0tis_2   : corresponds to   'L0-TIS'  
#   - "label"l1tos_1   : corresponds to 'Hlt1-TOS'  
#   - "label"l1tis_2   : corresponds to 'Hlt1-TIS'  
#   - "label"l2tos_1   : corresponds to 'Hlt2-TOS'  
#   - "label"l2tis_2   : corresponds to 'Hlt2-TIS'
# 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-21  
def tisTos ( self             ,
             p                ,
             ntuple           ,
             label            , 
             lines            , 
             l0tistos  = None ,
             tistos    = None ) :
    """
    Fill TisTos information into n-tuple
    
    for d in particles : 
       self.tisTos ( d     ,
                     tup   ,
                    'd0_' ,
                    self.lines ['D0'] , 
                    self.l0tistos     ,
                    self.tistos       )
    

  
    ``lines'' here is a dictionary of lines (or regex-patterns) with
    following keys:
    - L0TOS
    - L0TIS
    - Hlt1TOS
    - Hlt1TIS
    - Hlt2TOS
    - Hlt2TIS
    
    e.g.
    
    lines = {}
    lines ['L0TOS'  ] = 'L0HadronDecision'
    lines ['L0TIS'  ] = 'L0(Hadron|Muon|DiMuon)Decision'
    lines ['Hlt1TOS'] = ...
    lines ['Hlt1TIS'] = 'Hlt1Topo.*Decision'
    lines ['Hlt2TOS'] = ...
    lines ['Hlt2TIS'] = ...
    
    Technically it is useful to keep it as ``per-particle-type'' dictionary
    
    def initialize ( self ) :
        ...
        self.lines           = {}
        self.lines ['B'  ]   = {}
        self.lines ['B'  ]['L0TOS'] = ...
        self.lines ['B'  ]['L0TOS'] = ...
        ...
        self.lines ['psi']   = {}
        self.lines ['psi']['L0TOS'] = ...
        ...
        return SUCCESS
    
    def analyse ( self ) : 
        
        particles = ...
        
        for B in particles : 
            self.tisTos ( B     ,
                          tup   ,
                          'B0_' ,
                          self.lines['B']   , 
                          self.l0tistos     ,
                          self.tistos       )

        ...
        return SUCCESS
    
    The function adds few columns to n-tuple, themost important are
    <label>l0tos_1  that corresponds to   'L0-TOS'  
    <label>l0tis_2  that corresponds to   'L0-TIS'  
    <label>l1tos_1  that corresponds to 'Hlt1-TOS'  
    <label>l1tis_2  that corresponds to 'Hlt1-TIS'  
    <label>l2tos_1  that corresponds to 'Hlt2-TOS'  
    <label>l2tis_2  that corresponds to 'Hlt2-TIS'
    
    """
    if hasattr ( p , 'particle' ) :  p = p.particle()
    
    if not l0tistos and hasattr ( self , 'l0tistos' ) : l0tistos = self.l0tistos
    if not   tistos and hasattr ( self ,   'tistos' ) :   tistos = self.  tistos
    #
    ## start actual Tis/Tos/Tob'ing
    #
    l0tistos . setOfflineInput ( p )
    tistos   . setOfflineInput ( p )
    #
    ## L0
    # 
    l0tistos . setTriggerInput ( lines [ 'L0TOS'   ]  )
    l0_tos   = l0tistos.tisTosTobTrigger ()
    l0tistos . setTriggerInput ( lines [ 'L0TIS'   ]  )
    l0_tis   = l0tistos.tisTosTobTrigger () 
    #
    ## L1
    #
    tistos   . setTriggerInput ( lines [ 'Hlt1TOS' ]  )
    l1_tos   =   tistos.tisTosTobTrigger ()
    tistos   . setTriggerInput ( lines [ 'Hlt1TIS' ]  )
    l1_tis   =   tistos.tisTosTobTrigger ()
    #
    ## L2
    #
    tistos   . setTriggerInput ( lines [ 'Hlt2TOS' ]  )
    l2_tos   =   tistos.tisTosTobTrigger ()
    tistos   . setTriggerInput ( lines [ 'Hlt2TIS' ]  )
    l2_tis   =   tistos.tisTosTobTrigger ()
    
    ##     l0_tos     = l0tistos.triggerTisTos ( p , lines [ 'L0TOS'   ]  )
    ##     l0_tis     = l0tistos.triggerTisTos ( p , lines [ 'L0TIS'   ]  )
    
    ##     l1_tos     =   tistos.triggerTisTos ( p , lines [ 'Hlt1TOS' ]  )
    ##     l1_tis     =   tistos.triggerTisTos ( p , lines [ 'Hlt1TIS' ]  )
    
    ##     l2_tos     =   tistos.triggerTisTos ( p , lines [ 'Hlt2TOS' ]  )
    ##     l2_tis     =   tistos.triggerTisTos ( p , lines [ 'Hlt2TIS' ]  )
    
    #
    ## main TOS & TIS categories:
    #
    
    ntuple.column ( label + 'l0tos_1' , l0_tos.tos      () , 0 , 1 )
    ntuple.column ( label + 'l0tis_1' , l0_tos.tis      () , 0 , 1 )
    ntuple.column ( label + 'l0dec_1' , l0_tos.decision () , 0 , 1 )
    
    ntuple.column ( label + 'l0tos_2' , l0_tis.tos      () , 0 , 1 )
    ntuple.column ( label + 'l0tis_2' , l0_tis.tis      () , 0 , 1 )
    ntuple.column ( label + 'l0dec_2' , l0_tis.decision () , 0 , 1 )
    
    ntuple.column ( label + 'l1tos_1' , l1_tos.tos      () , 0 , 1 )
    ntuple.column ( label + 'l1tis_1' , l1_tos.tis      () , 0 , 1 )
    ntuple.column ( label + 'l1dec_1' , l1_tos.decision () , 0 , 1 )
    
    ntuple.column ( label + 'l1tos_2' , l1_tis.tos      () , 0 , 1 )
    ntuple.column ( label + 'l1tis_2' , l1_tis.tis      () , 0 , 1 )
    ntuple.column ( label + 'l1dec_2' , l1_tis.decision () , 0 , 1 )
    
    ntuple.column ( label + 'l2tos_1' , l2_tos.tos      () , 0 , 1 )
    ntuple.column ( label + 'l2tis_1' , l2_tos.tis      () , 0 , 1 )
    ntuple.column ( label + 'l2dec_1' , l2_tos.decision () , 0 , 1 )
    
    ntuple.column ( label + 'l2tos_2' , l2_tis.tos      () , 0 , 1 )
    ntuple.column ( label + 'l2tis_2' , l2_tis.tis      () , 0 , 1 )
    ntuple.column ( label + 'l2dec_2' , l2_tis.decision () , 0 , 1 )

    #
    ## TPS & TUS categories:
    #

    ntuple.column ( label + 'l0tps_1' , l0_tos.tps      () , 0 , 1 )
    ntuple.column ( label + 'l0tus_1' , l0_tos.tus      () , 0 , 1 )

    ntuple.column ( label + 'l0tps_2' , l0_tis.tps      () , 0 , 1 )
    ntuple.column ( label + 'l0tus_2' , l0_tis.tus      () , 0 , 1 )
    
    ntuple.column ( label + 'l1tps_1' , l1_tos.tps      () , 0 , 1 )
    ntuple.column ( label + 'l1tus_1' , l1_tos.tus      () , 0 , 1 )

    ntuple.column ( label + 'l1tps_2' , l1_tis.tps      () , 0 , 1 )
    ntuple.column ( label + 'l1tus_2' , l1_tis.tus      () , 0 , 1 )

    ntuple.column ( label + 'l2tps_1' , l2_tos.tps      () , 0 , 1 )
    ntuple.column ( label + 'l2tus_1' , l2_tos.tus      () , 0 , 1 )

    ntuple.column ( label + 'l2tps_2' , l2_tis.tps      () , 0 , 1 )
    ntuple.column ( label + 'l2tus_2' , l2_tis.tus      () , 0 , 1 )

    return SUCCESS

# =============================================================================
## initialize TisTos, acquire tools, etc...
#  helper methdod to initialize TisTosTob'ing machinery
#
#  @code
#
#  def initialize ( self ) :
#
#     ... 
#     lines           = {}
#     lines ['B'  ]   = {}
#     lines ['B'  ]['L0TOS'] = ...
#     lines ['B'  ]['L0TOS'] = ...
#     ...
#     lines ['psi']   = {}
#     lines ['psi']['L0TOS'] = ...
#     ...
#     triggers        = {}
#     triggers['B']   = {}
#     triggers['psi'] = {}
#     ...
#     sc = self.tisTos_initialize ( triggers , lines  )
#
#  @endcode 
# 
#  @param triggers dictionnary to collect trigger line statistics
#  @param lines    dictionnary for TisTosTobing
#  
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-21  
# 
def _tisTosInit ( self , triggers = {} , lines = {} ) :
    """
    Initialize tis-tos machinery, acquire tools, etc...  
    """
    
    ## locate tools 
    self.l0tistos  = self.tool ( cpp.ITriggerTisTos , 'L0TriggerTisTos' , parent = self )  
    self.tistos    = self.tool ( cpp.ITriggerTisTos ,   'TriggerTisTos' , parent = self )
    
    self.tistos   . setOfflineInput()
    self.l0tistos . setOfflineInput()
    
    self.triggers = {}
    
    from copy import deepcopy
    if triggers : self.triggers = deepcopy ( triggers )  
    if lines    : self.lines    = deepcopy ( lines    )

    return SUCCESS 


# =============================================================================
## finalize  TisTos
#  helper methdod to finalize TisTosTob'ing machinery
#
#  @code
#
#  def finalize ( self ) :
#
#     ... 
#     sc = self.tisTos_finalize ()
#
#  @endcode 
# 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-21  
# 
def _tisTosFini ( self , triggers = None ) :
    """
    Finalize tis-tos machinery 
    """
    
    if not triggers and hasattr ( self , 'triggers' ) : triggers = self.triggers
    
    trgDecs ( self , triggers )

    if hasattr ( self , 'l0tistos' ) : self . l0tistos = None
    if hasattr ( self ,   'tistos' ) : self .   tistos = None
    
    return SUCCESS 

# =============================================================================
## print tis-tos statistics
#
#  @code
#
#  >>> tistos = ...
#  >>> tistos_print ( tistos )
#
#  @endcode
# 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-03-23  
# 
def tistos_print ( triggers     ,
                   warn  = None ,
                   ofile = None ) :
    """
    print tis-tos statistic
    
    >>> tistos = ...
    >>> tistos_print ( tistos )
    
    """
    VE = cpp.Gaudi.Math.ValueWithError
    be = cpp.Gaudi.Math.binomEff 

    if not ofile :
        import sys 
        ofile = sys.stdout
        
    for part in triggers :
        
        trigs = triggers[part]
        
        print >> ofile , 90*'*'
        print >> ofile , ' Triggers for ', part 
        print >> ofile , 90*'*'
        
        
        if not isinstance ( trigs , dict ) : 
            if warn :  warn ( 'Invalid key "%s"' % part  , SUCCESS )
            else    :  print >> ofile, 'Invalid key "%s"' % part 
            continue
        
        tkeys = trigs.keys()
        tkeys.sort()
        
        for k in tkeys :
            
            trg = trigs[ k ]            
            tot  = trg['TOTAL']
            print >> ofile, k , part, '  #lines: %5d #events %-5d ' %  ( max( len(trg)-1 , 0 ) , tot ) 
            keys = trg.keys()
            keys.sort()

            for k in keys :
                v   = trg[k]
                eff = be ( v , tot ) * 100
                print >> ofile,' %s  \t %s ' % ( eff.toString ("(%6.2f +-%5.2f )") , k ) 
                
                
    print >> ofile , 90*'*'

# =============================================================================
## merge tistos data-bases
#
#  @code
#
#    >>> db1 = ...
#    >>> db2 = ...
#    >>> merged = tistos_merge ( db1 , db2 )
#
#    >>> merged = tistos_merge ( [ 'db1' , 'db2' , 'db3'] ) 
#
#  @endcode 
# 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-03-23  
# 
def tistos_merge ( dbase , tistos = {} ) :
    
    import shelve, os, copy  

    ## the actual merge 
    if isinstance ( dbase , dict )  :
        merged = copy.deepcopy ( tistos )
        
        ## loop over particles 
        for particle in dbase :
            ## new key ? 
            if not merged.has_key( particle ) :
                merged[particle] = copy.deepcopy ( dbase[particle] ) 
                continue             
            trigs   = dbase  [ particle ]
            trigs_m = merged [ particle ]
            ## loop over trigger-sets 
            for t in trigs_n :
                ## new key?
                if not trigs_m.has_key ( t ) :
                    trigs_m [ t ] = copy.deepcopy ( trigs[t] )
                    continue 
                trg   = trigs   [t]
                trg_m = trigs_m [t]
                ## loop over trigger lines 
                for k in trg :
                    ## new key 
                    if not trg_m.has_key ( k ) :
                        trg_m [ k ] = copy.deepcopy ( trg[k] )
                        continue
                    cnt       = trg[k]  ## the actual counter                
                    trg_m[k] += cnt     ## update the counter 

        return merged

    ## db-name 
    if isinstance ( dbase , str )  :
        #
        dbase = os.path.expandvars ( dbase)
        dbase = os.path.expanduser ( dbase )
        dbase = shelve.open ( dbase ,  'r' )
        #
        return tistos_merge ( dbase , tistos )
    
    ## extract the dictionary from db 
    if isinstance ( dbase , shelve.Shelf )  :
        #
        try    :
            tt = dbase['tistos']
        except :
            tt = {}
            #
        return tistos_merge ( tt , tistos )
        
    ## merge the sequence 
    if isinstance ( dbase , ( list , tuple ) ) or hasattr ( dbase , '__iter__' ) :
        tt = tistos
        for db in dbase : tt = tistos_merge ( db , tt )
        return tt 
    
    return tistos 
        
    
# =============================================================================
Algo.decisions         =  decisions
Algo.trgDecs           =  trgDecs 
Algo.tisTos            =  tisTos 
Algo.tisTos_initialize = _tisTosInit
Algo.tisTos_finalize   = _tisTosFini
# =============================================================================
# make report 
# =============================================================================
logger.info('Add TisTos functionality to Bender.Algo class')
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
