#########################################################################################
# Utility function for setting thresholds both in Hlt1 and 2
#
def setThresholds(ThresholdSettings,confs):
    """
    Look in ThresholdSettings for configurable confs
    and set the appropriate settings
    
    @author G. Raven, P. Koppenburg
    @date 23/7/2009 (moved)
    """
    conf = confs()  # The configurable _must_ be called even if not configured. Or it will be ignored
    from Gaudi.Configuration import log
    if confs in ThresholdSettings : 
        #       print '# Found', conf.name()
        for (k,v) in ThresholdSettings[confs].iteritems() :
            # configurables have an exception for list and dict: 
            #   even if not explicitly set, if you ask for them, you get one...
            #   this is done to make foo().somelist += ... work.
            # hence we _assume_ that, even if we have an attr, but it matches the
            # default, it wasn't set explicitly, and we overrule it...
            if hasattr(conf,k) and conf.getProp(k) != conf.getDefaultProperty(k) :
                log.warning('# WARNING: %s.%s has explictly been set, NOT using requested predefined threshold %s, but keeping explicit value: %s '%(conf.name(),k,str(v),getattr(conf,k)))
            else :
                if ( type(v) == type({})): # special case for dictionaries (needed in topo)
                    val = conf.getProp(k)
                    val.update(v)                                
                    #                    print '# SETTING dictionary', conf.name(), val
                    setattr(conf,k,val)
                else :
                    #                    print '# SETTING           ', conf.name(), v
                    setattr(conf,k,v)

