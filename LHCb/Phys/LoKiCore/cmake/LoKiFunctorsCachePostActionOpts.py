#!/usr/bin/env gaudirun.py
# =============================================================================
## @file
#  helper configuration file needed for autogeneration of functors from
#  Stripping code
#
#  @author Vanya BELYAEV Ivan.Belayev@itep.ru
#  @date 2015-02-13
#  $Revision$
#  Last modification $Date$
#                 by $Author$
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
    dod                       = DataOnDemandSvc()
    dod.PreInitialize         = True  ## this is the most important line here

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
    from Configurables import DetDataSvc
    from Configurables import LoKiSvc

    #
    ## some reshuffling of order of services is needed
    #    in particular DOD should come after PPSVC, LoKiSvc and ToolSvc
    #

    services = app.ExtSvc
    app.ExtSvc = [ DetDataSvc('DetectorDataSvc'), PPSvc() ,
                   LoKiSvc(Welcome=False) ] + services + [ dod ]

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
    for factory in ('Tool', 'CoreFactory', 'TrackFunctorFactory', 'HltFactory'):
        factory = 'LoKi__Hybrid__%s' % factory
        try:
            factory = getattr(Configurables, factory)
            factory(OutputLevel=2, StatPrint=True)
        except AttributeError:
            # ignore unknown factories
            pass


#
## use this action!
#

from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( post_action_for_cpp )

# =============================================================================
# The END
# =============================================================================
