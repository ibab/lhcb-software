
def post_action_disable_python( ) :
    import os
    os.environ["LOKI_DISABLE_PYTHON"] = "1"

from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( post_action_disable_python )
