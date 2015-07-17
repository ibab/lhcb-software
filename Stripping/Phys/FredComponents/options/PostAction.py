#!/usr/bin/env gaudirun.py
# =============================================================================
## @file
#  helper configuration file needed for autogeneration of functors from
#  Stripping code
#
#  @author Vanya BELYAEV Ivan.Belayev@itep.ru
#  @date 2015-02-13
#  $Revision: 184170 $
#  Last modification $Date: 2015-02-22 12:45:31 +0100(dom, 22 feb 2015) $
#                 by $Author: ibelyaev $
# =============================================================================


# =============================================================================
## post-config action  :
#   - enforce initialization of DoD-algorithms
#   - (remove some of them)
#   - reduce a bit the printout 
# @author Vanya BELYAEV Ivan.Belayev@itep.ru
# @date 2015-02-13
#
def post_action_for_cpp ( ) :
    """
    Post-configh action to enforce initialization of DoD-algorithms
    """
    
    from Gaudi.Configuration import log
    
    log.info('# VB: Post-action to enforce the initialization of DoD-algorithms') 
    
    from Configurables import DataOnDemandSvc    
    dod                           = DataOnDemandSvc()
    dod.PreInitialize             = True  ## this is the most important line here 

    ## dod.AllowPreInitializeFailure = True 
    ## dod.Dump                      = True

    removed = [] 
    for key in dod.AlgMap :
        if 0 <= key.find('Raw/UT') :
            removed.append ( key )
            log.warning('# VB:   REMOVE UT decoding from DataOnDemand!')

    dct = dod.AlgMap
    for k in removed : del dct[k]
    dod.AlgMap = dct 
    
    
    from Configurables import ApplicationMgr 
    app         = ApplicationMgr( OutputLevel = 5 )
    app.EvtMax  = 0  
    app.EvtSel  = 'NONE'

    
    from Configurables import LHCb__ParticlePropertySvc as PPSvc 
    from Configurables import LoKiSvc 
    
    #
    ## some reshuffling of order of services is needed
    #    in particular DOD should come after PPSVC, LoKiSvc and ToolSvc 
    # 

    
    services = app.ExtSvc
    app.ExtSvc = [ PPSvc() , LoKiSvc()       ] + services + [dod ]
    ## app.ExtSvc = [ PPSvc() , LoKiSvc() , dod , ToolSvc() ] + services  

    #
    ## suppress some prints
    #
    from Configurables import TimingAuditor
    timer = TimingAuditor()
    from Configurables import SequencerTimerTool
    timer.addTool ( SequencerTimerTool , 'TIMER' )  
    timer.TIMER.OutputLevel = 5

    #
    ## ensure that prints from the main tools/factories  are not suprpessed  
    #
    from Configurables import LoKi__Hybrid__Tool                as PF 
    from Configurables import LoKi__Hybrid__CoreFactory         as CF 
    from Configurables import LoKi__Hybrid__TrackFunctorFactory as TF 
    from Configurables import LoKi__Hybrid__HltFactory          as HF 
    
    for factory in ( PF , CF , TF , HF ) :
        factory ( OutputLevel = 2 , StatPrint = True  )

    #
    ## here I try to suppress printout of various summaries from the algorithms..
    #  However it does not work as expected :-(
    #
    
    from Gaudi.Configuration import allConfigurables
    from Gaudi.Configuration import getConfigurable
    
    def suppressStat ( a ) :
        
        if hasattr ( a , 'StatPrint'   ) : a.StatPrint   = False
        if hasattr ( a , 'ErrorsPrint' ) : a.ErrorsPrint = False
        if hasattr ( a , 'HistoPrint'  ) : a.HistoPrint  = False
            
        if hasattr ( a , 'Members'   ) :
            for m in a.Members :
                if isinstance ( m , str ) :
                    if not m in allConfigurables : continue
                    m = getConfigurable ( m )    
                suppressStat ( m ) 
            
        
    algs = app.TopAlg
    for a in algs : suppressStat ( a )

    from Configurables import DaVinci
    dv = DaVinci()
    for a in dv.sequence().Members :
        suppressStat ( a ) 
    


#
## use this action!
# 

from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( post_action_for_cpp ) 

# =============================================================================
# The END 
# =============================================================================
