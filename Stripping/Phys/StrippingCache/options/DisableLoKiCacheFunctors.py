
def post_action_disable_cache( ) :
    import os
    os.environ["LOKI_DISABLE_CACHE"] = "1"

from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( post_action_disable_cache )
