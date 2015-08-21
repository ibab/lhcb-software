
#import os
#os.environ["LOKI_DISABLE_CACHE"] = "1"

def post_action_disable_cache( ) :
    for factory in ('Tool', 'CoreFactory', 'TrackFunctorFactory',
                    'HltFactory', 'Hlt1HltFactory', 'Hlt2HltFactory'):
        factory = 'LoKi__Hybrid__%s' % factory
        try:
            factory = getattr(Configurables, factory)
            factory( UseCache = False )
        except AttributeError:
            # ignore unknown factories
            pass


from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( post_action_disable_cache )
