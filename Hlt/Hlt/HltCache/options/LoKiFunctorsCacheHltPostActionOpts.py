#!/usr/bin/env gaudirun.py
# =============================================================================
## @file
#  helper configuration file needed for autogeneration of functors from
#  Stripping code
#
#  @author Vanya BELYAEV Ivan.Belayev@itep.ru
#  @date 2015-02-13
#  $Revision: 194051 $
#  Last modification $Date: 2015-08-21 20:04:57 +0200 (Fri, 21 Aug 2015) $
#                 by $Author: jonrob $
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
    from Configurables import ApplicationMgr
    app         = ApplicationMgr( OutputLevel = 3 )
    app.EvtMax  = 0
    app.EvtSel  = 'NONE'


    from Configurables import LHCb__ParticlePropertySvc as PPSvc
    from Configurables import DetDataSvc
    from Configurables import LoKiSvc

    #
    ## some reshuffling of order of services is needed
    #    in particular DOD should come after PPSVC, LoKiSvc and ToolSvc
    #

    services = app.ExtSvc
    app.ExtSvc = [ DetDataSvc('DetectorDataSvc'), PPSvc() ,
                   LoKiSvc(Welcome=False) ] + services

    #
    ## suppress some prints
    #
    from Configurables import TimingAuditor
    timer = TimingAuditor()
    from Configurables import SequencerTimerTool
    timer.addTool ( SequencerTimerTool , 'TIMER' )
    timer.TIMER.OutputLevel = 5

    # suppress printout of various summaries from algorithms.
    from Gaudi.Configuration import allConfigurables
    for conf in allConfigurables.itervalues():
        for opt in ('StatPrint', 'ErrorsPrint', 'HistoPrint'):
            if opt in conf.__slots__:
                setattr(conf, opt, False)

    # ensure that prints from the main tools/factories are not suppressed
    import Configurables
    from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
    from Configurables import LoKi__Hybrid__Tool as HybridFactory
    from Configurables import LoKi__Hybrid__HltFactory as HltFactory
    for Factory, names in {HltFactory : ("HltFactory",
                                         "Hlt1HltFactory",
                                         "Hlt2HltFactory"),
                           CoreFactory : ("Hlt1CoreFactory",
                                          "Hlt2CoreFactory",
                                          "Hlt1Factory"),
                           HybridFactory : ("Hlt1HybridFactory", "Hlt2HybridFactory")}.iteritems():
        for name in names:
            f = Factory(name)
            f.OutputLevel = 2
            f.StatPrint = True

#
## use this action!
#

from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( post_action_for_cpp )

# =============================================================================
# The END
# =============================================================================
