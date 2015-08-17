#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
""" Decorate the algorithm with helper methods for TisTosTob'ing

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    
                                                                 
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
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2011-06-07"
# =============================================================================
__all__= (
    ##
    'decisions'    , ## collect trigger decisions
    'trgDecs'      , ## print trigger statistics 
    'tisTos'       , ## fill N-tuple with TisTos information
    ##
    'Hlt1_Physics' , ## regex pattern for Hlt1-physics 
    'Hlt2_Physics' , ## regex pattern for Hlt2-physics 
    'L0_Physics'   , ## regex pattern for L0-physics 
    )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.TisTos' )
else                      : logger = getLogger ( __name__ )
# ==============================================================================
from   LoKiCore.basic  import cpp,LoKi
import LHCbMath.Types 
# ==============================================================================
SUCCESS                 = cpp.StatusCode( cpp.StatusCode.SUCCESS , True )
FAILURE                 = cpp.StatusCode( cpp.StatusCode.FAILURE , True )
ITriggerSelectionTisTos = cpp.ITriggerSelectionTisTos
TisTosTob               = cpp.ITisTos.TisTosTob
# ==============================================================================
logger.info ("Add useful methods to class Algo   for TisTos'sing ") 
# ==============================================================================
## from Hlt/HltConf v9r18 
# ==============================================================================
routing_bit_46 = 'Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision'
routing_bit_77 = 'Hlt2(?!Forward)(?!DebugEvent)(?!Express)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision'
Hlt1_Physics   = routing_bit_46
Hlt2_Physics   = routing_bit_77
L0_Physics     = 'L0(Muon|DiMuon|Electron|Photon|Hadron).*Decision'
Hlt1_Global    = 'Hlt1Global'
Hlt2_Global    = 'Hlt2Global'
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
#                       self.l1tistos   ,
#                       self.l2tistos   )
#
#  @endcode 
#
#  "triggers" here is in/out-argument (dictionary) that accumulates the
#  information about trigger lines ...
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
#                          self.l1tistos      ,
#                          self.l2tistos      )
#     ...
#
#  @endcode 
#
#
#  @param p        (input)         the particle
#  @param triggers (update/output) the triggers
#  @param l0tistos (input)         the tool for L0-tistos 
#  @param l1tistos (input)         the tool for Hlt1-tistos 
#  @param l2tistos (input)         the tool for Hlt2-tistos 
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-21
def decisions ( self             ,
                p                ,
                triggers         , 
                l0tistos  = None ,
                l1tistos  = None ,
                l2tistos  = None ) : 
    """
    ## Collect the important trigger lines

    ## ... 
    ## particles = ...
    ## for D in particles : 
    ##     self.decisions ( D               ,
    ##     triggers        ,
    ##     self.l0tistos   ,
    ##     self.l1tistos   ,
    ##     self.l2tistos   )
        
    ## ``triggers'' here is in/out-argument (dictionary) that accumulates the
    ## information abot trigger lines ...
    
    ## Technically it is useful to keep it as ``per-particle-type''
    ## dictionary
    
    ## def initialize ( self ) :
    ##     ...
    ##     self.triggers        = {}
    ##     self.triggers['B']   = {}
    ##     self.triggers['H']   = {}
    ##     self.triggers['psi'] = {}
    ##     ...
    ##     return SUCCESS
    
    ## def analyse ( self ) :
    ##     ... 
    ##     particles = ...
    ##     for B in particles : 
    ##         self.decisions ( B                  ,
    ##                          self.triggers['B'] ,
    ##                          self.l0tistos      ,
    ##                          self.l1tistos      ,
    ##                          self.l2tistos      )
    ##     ...
    ##     return SUCCESS
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
    if not l1tistos and hasattr ( self , 'l1tistos' ) : l1tistos = self.l1tistos
    if not l2tistos and hasattr ( self , 'l2tistos' ) : l2tistos = self.l2tistos
    
    if not l0tistos : return self.Error ( 'decisions: Invalid "l0tistos"-argument ')
    if not l1tistos : return self.Error ( 'decisions: Invalid "l1tistos"-argument ')
    if not l2tistos : return self.Error ( 'decisions: Invalid "l2tistos"-argument ')
    
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

        
    for i,tistos in zip ( ( 'Hlt1' , 'Hlt2' ) , ( l1tistos, l2tistos ) ) :

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
            if not tos.has_key(t)   : tos[t]  = 1 
            else                    : tos[t] += 1 

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
            if not tis.has_key(t)   : tis[t]  = 1 
            else                    : tis[t] += 1
            
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
            if not tis.has_key(t)   : tis[t]  = 1 
            else                    : tis[t] += 1 
            
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
#                      self.l1tistos     ,
#                      self.l2tistos     )
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
#                      self.l1tistos     ,
#                      self.l2tistos     )
#  @endcode
#
#
#  The function adds few columns to n-tuple, the most important are:
#   - "label"l0tos_1   : corresponds to   'L0-TOS'  
#   - "label"l0tis_2   : corresponds to   'L0-TIS'  
#   - "label"l1tos_1   : corresponds to 'Hlt1-TOS'  
#   - "label"l1tis_2   : corresponds to 'Hlt1-TIS'  
#   - "label"l2tos_1   : corresponds to 'Hlt2-TOS'  
#   - "label"l2tis_2   : corresponds to 'Hlt2-TIS'
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-21  
def tisTos ( self              ,
             p                 , 
             ntuple            ,
             label             , 
             lines             , 
             l0tistos  = None  ,
             l1tistos  = None  ,
             l2tistos  = None  ,
             verbose   = False ) :
    """
    ## Fill TisTos information into n-tuple
    
    ## for d in particles : 
    ##    self.tisTos ( d     ,
    ##                  tup   ,
    ##                 'd0_'  ,
    ##                 self.lines ['D0'] , 
    ##                 self.l0tistos     ,
    ##                 self.l1tistos     ,
    ##                 self.l2tistos     )
    

  
    ## ``lines'' here is a dictionary of lines (or regex-patterns) with
    ## following keys:
    ## - L0TOS
    ## - L0TIS
    ## - Hlt1TOS
    ## - Hlt1TIS
    ## - Hlt2TOS
    ## - Hlt2TIS
    
    ## e.g.
    
    ## lines = {}
    ## lines ['L0TOS'  ] = 'L0HadronDecision'
    ## lines ['L0TIS'  ] = 'L0(Hadron|Muon|DiMuon)Decision'
    ## lines ['Hlt1TOS'] = ...
    ## lines ['Hlt1TIS'] = 'Hlt1Topo.*Decision'
    ## lines ['Hlt2TOS'] = ...
    ## lines ['Hlt2TIS'] = ...
    
    ## Technically it is useful to keep it as ``per-particle-type'' dictionary
    
    ## def initialize ( self ) :
    ##     ...
    ##     self.lines           = {}
    ##     self.lines ['B'  ]   = {}
    ##     self.lines ['B'  ]['L0TOS'] = ...
    ##     self.lines ['B'  ]['L0TOS'] = ...
    ##     ...
    ##     self.lines ['psi']   = {}
    ##     self.lines ['psi']['L0TOS'] = ...
    ##     ...
    ##     return SUCCESS
    
    ## def analyse ( self ) : 
        
    ##     particles = ...
        
    ##     for B in particles : 
    ##         self.tisTos ( B     ,
    ##                       tup   ,
    ##                       'B0_' ,
    ##                       self.lines['B']   , 
    ##                       self.l0tistos     ,
    ##                       self.l1tistos     ,
    ##                       self.l2tistos     )

    ##     ...
    ##     return SUCCESS
    
    ## The function adds few columns to n-tuple, the most important are
    ## <label>l0tos_1  that corresponds to   'L0-TOS'  
    ## <label>l0tis_2  that corresponds to   'L0-TIS'  
    ## <label>l1tos_1  that corresponds to 'Hlt1-TOS'  
    ## <label>l1tis_2  that corresponds to 'Hlt1-TIS'  
    ## <label>l2tos_1  that corresponds to 'Hlt2-TOS'  
    ## <label>l2tis_2  that corresponds to 'Hlt2-TIS'

    """
    if hasattr ( p , 'particle' ) :  p = p.particle()
    
    if not l0tistos and hasattr ( self , 'l0tistos' ) : l0tistos = self.l0tistos
    if not l1tistos and hasattr ( self , 'l1tistos' ) : l1tistos = self.l1tistos
    if not l2tistos and hasattr ( self , 'l2tistos' ) : l2tistos = self.l2tistos
    #

    #
    ## create placeholders
    #
    l0_tos  = TisTosTob () 
    l0_tis  = TisTosTob () 
    l0_phys = TisTosTob ()  ## physics 
    #
    l1_tos  = TisTosTob () 
    l1_tis  = TisTosTob () 
    l1_phys = TisTosTob ()  ## physics 
    l1_glob = TisTosTob ()  ## global 
    #
    l2_tos  = TisTosTob () 
    l2_tis  = TisTosTob () 
    l2_phys = TisTosTob ()  ## physics 
    l2_glob = TisTosTob ()  ## global 
    ##

    
    tistosdata = {}
    
    if p and l0tistos and l1tistos and l2tistos : 

        #
        ## start actual Tis/Tos/Tob'ing
        #
        l0tistos . setOfflineInput ( p )
        l1tistos . setOfflineInput ( p )
        l2tistos . setOfflineInput ( p )
        #
        ## L0
        # 
        l0tistos . setTriggerInput ( lines [ 'L0TOS'      ]  )
        l0_tos   = l0tistos.tisTosTobTrigger ()
        l0tistos . setTriggerInput ( lines [ 'L0TIS'      ]  )
        l0_tis   = l0tistos.tisTosTobTrigger ()
        l0tistos . setTriggerInput ( lines [ 'L0Phys'     ]  )
        l0_phys  = l0tistos.tisTosTobTrigger () 
        #
        ## L1
        #
        l1tistos . setTriggerInput ( lines [ 'Hlt1TOS'    ]  )
        l1_tos   = l1tistos.tisTosTobTrigger ()
        l1tistos . setTriggerInput ( lines [ 'Hlt1TIS'    ]  )
        l1_tis   = l1tistos.tisTosTobTrigger ()
        l1tistos . setTriggerInput ( lines [ 'Hlt1Phys'   ]  )
        l1_phys  = l1tistos.tisTosTobTrigger () 
        l1tistos . setTriggerInput ( lines [ 'Hlt1Global' ]  )
        l1_glob  = l1tistos.tisTosTobTrigger () 
        #

        #
        ## L2
        #
        l2tistos . setTriggerInput ( lines [ 'Hlt2TOS'    ]  )
        l2_tos   = l2tistos.tisTosTobTrigger ()
        l2tistos . setTriggerInput ( lines [ 'Hlt2TIS'    ]  )
        l2_tis   = l2tistos.tisTosTobTrigger ()
        l2tistos . setTriggerInput ( lines [ 'Hlt2Phys'   ]  )
        l2_phys  = l2tistos.tisTosTobTrigger () 
        l2tistos . setTriggerInput ( lines [ 'Hlt2Global' ]  )
        l2_glob  = l2tistos.tisTosTobTrigger ()
        
        tistosdata [ 'L0TOS'      ] = l0_tos
        tistosdata [ 'L0TIS'      ] = l0_tis
        tistosdata [ 'L0Phys'     ] = l0_phys
        
        tistosdata [ 'Hlt1TOS'    ] = l1_tos
        tistosdata [ 'Hlt1TIS'    ] = l1_tis
        tistosdata [ 'Hlt1Phys'   ] = l1_phys
        tistosdata [ 'Hlt1Global' ] = l1_glob
        
        tistosdata [ 'Hlt2TOS'    ] = l2_tos
        tistosdata [ 'Hlt2TIS'    ] = l2_tis
        tistosdata [ 'Hlt2Phys'   ] = l2_phys
        tistosdata [ 'Hlt2Global' ] = l2_glob
        
    #
    ## fill n-tuples
    #
    ntuple.column_aux ( label + 'l0tos'  , l0_tos  , verbose )
    ntuple.column_aux ( label + 'l0tis'  , l0_tis  , verbose )
    ntuple.column_aux ( label + 'l0phys' , l0_phys , verbose )
    #
    ntuple.column_aux ( label + 'l1tos'  , l1_tos  , verbose )
    ntuple.column_aux ( label + 'l1tis'  , l1_tis  , verbose )
    ntuple.column_aux ( label + 'l1phys' , l1_phys , verbose )
    ntuple.column_aux ( label + 'l1glob' , l1_glob , verbose )
    #
    ntuple.column_aux ( label + 'l2tos'  , l2_tos  , verbose )
    ntuple.column_aux ( label + 'l2tis'  , l2_tis  , verbose )
    ntuple.column_aux ( label + 'l2phys' , l2_phys , verbose )
    ntuple.column_aux ( label + 'l2glob' , l2_glob , verbose )
    #
    # keep data for debugging
    #
    
    return SUCCESS, tistosdata 

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
def tistos_initialize ( self , triggers = {} , lines = {} ) :
    """
    Initialize tis-tos machinery, acquire tools, etc...  
    """
    if self.tistos_initialized :
        self.Warning("Re-initialization TisTos-machinery is invoked", SUCCESS)
        
    ## locate tools 
    self.l0tistos  = self.tool ( cpp.ITriggerTisTos ,   'L0TriggerTisTos' , parent = self )  
    self.l1tistos  = self.tool ( cpp.ITriggerTisTos , 'Hlt1TriggerTisTos' , parent = self )
    self.l2tistos  = self.tool ( cpp.ITriggerTisTos , 'Hlt2TriggerTisTos' , parent = self )

    self.l0tistos . setOfflineInput()
    self.l1tistos . setOfflineInput()
    self.l2tistos . setOfflineInput()
    
    self.triggers = {}
    
    from copy import deepcopy    
    if triggers : self.triggers = deepcopy ( triggers )  
    
    if lines :
        
        ## copy it! 
        self.lines    = deepcopy ( lines    )

        parts  = self.lines.keys()
        parts.sort()

        if not parts :
            self.Warning ( 'No PARTICLES are defined for Tis/Tos' , SUCCESS )
        
        ## extend it!     
        for p in parts :
            lns = self.lines [ p ]
            ##
            if not lns.has_key ( 'L0Phys'    ) : lns [ 'L0Phys'    ] =   L0_Physics  
            if not lns.has_key ( 'Hlt1Phys'  ) : lns [ 'Hlt1Phys'  ] = Hlt1_Physics  
            if not lns.has_key ( 'Hlt1Global') : lns [ 'Hlt1Global'] = Hlt1_Global  
            if not lns.has_key ( 'Hlt2Phys'  ) : lns [ 'Hlt2Phys'  ] = Hlt2_Physics  
            if not lns.has_key ( 'Hlt2Global') : lns [ 'Hlt2Global'] = Hlt2_Global  
            ##
            
        ## print it      
        for p in parts :
            lns = self.lines [ p ]
            self.Print ( ' Tis/Tos lines for %s ' % p , SUCCESS , 7 )
            trgs = lns.keys()
            trgs.sort()
            for t in trgs :
                self.Print ( '-  %-10s  :  %s ' % ( t , lns[t]  ) , SUCCESS , 7 )

    self.tistos_initialized = True 
    return SUCCESS 


# =============================================================================
## finalize  TisTos
#  helper method to finalize TisTosTob'ing machinery
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
def tistos_finalize ( self , triggers = None ) :
    """ Finalize tis-tos machinery
    """    
    ## finalize only initialized stuff: 
    if not self.tistos_initialized : return SUCCESS
    
    if not triggers and hasattr ( self , 'triggers' ) : triggers = self.triggers

    ## collect triggers 
    trgDecs ( self , triggers )

    if hasattr ( self , 'l0tistos' ) : self . l0tistos = None
    if hasattr ( self , 'l1tistos' ) : self . l1tistos = None
    if hasattr ( self , 'l2tistos' ) : self . l2tistos = None
    
    if not hasattr ( self , 'lines' ) :
        self.Warning ( 'No LINES are defined ' , SUCCESS )
    else :
        parts  = self.lines.keys()
        parts.sort()
        for p in parts :
            lns  = self.lines[ p ]
            self.Print ( ' Tis/Tos lines for %s ' % p , SUCCESS , 7 )
            trgs = lns.keys()
            trgs.sort()
            for t in trgs :
                self.Print ( '-  %-10s  :  %s ' % ( t , lns[t]  ) , SUCCESS , 7 )
        if not parts :
            self.Warning ( 'No PARTICLES are defined for Tis/Tos' , SUCCESS )

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
            for t in trigs_m :
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
LoKi.Algo.decisions          = decisions 
LoKi.Algo.trgDecs            = trgDecs 
LoKi.Algo.tisTos             = tisTos 
LoKi.Algo.tisTos_initialize  = tistos_initialize
LoKi.Algo.tisTos_finalize    = tistos_finalize
LoKi.Algo.tistos_initialize  = tistos_initialize
LoKi.Algo.tistos_finalize    = tistos_finalize
## class attribute 
LoKi.Algo.tistos_initialized = False ## class attribute 
# =============================================================================
# make report 
# =============================================================================
logger.info('Add TisTos functionality to Bender.Algo class')
# =============================================================================
## insert "fill" finalization into the global finalization
def decorateTisTos ( ALGO ) :
    "Insert ``fill'' finalization into the global finalization"
    print 'TISTOS DECORATE', ALGO 
    if hasattr ( ALGO , '_prev_tistos_finalize_' ) : return 
    print 'SKIP TISTOS DECORATE', ALGO 
    # =========================================================================
    ## updated finalization 
    def _algo_tistos_finalize_ ( algorithm ) :
        """Finalize ``specific'' stuff related BenderTools.TisTos module
        and then continue with the base-class finalization
        """
        if algorithm.tistos_initialized :
            algorithm.Print( 'Finalize "TisTos"-machinery' , 10 , 2 )
            algorithm.tistos_finalize  ()
        return algorithm._prev_tistos_finalize_()
    # =========================================================================
    ALGO._prev_tistos_finalize_ = ALGO.finalize 
    ALGO.finalize               = _algo_tistos_finalize_ 

# =============================================================================
if '__main__' == __name__ :
    
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) ) 
    logger.info ( 80*'*'  ) 
 
# =============================================================================
# The END 
# =============================================================================
