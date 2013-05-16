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
    if confs not in ThresholdSettings : return
    for (k,v) in ThresholdSettings[confs].iteritems() :
        if hasattr(conf,k) and v==getattr(conf,k) : continue # nothing to do, move on to next item...

        # configurables have an exception for list and dict: 
        #   even if not explicitly set, if you ask for them, you get one...
        #   this is done to make foo().somelist += ... work.
        # hence we _assume_ that, even if we have an attr, but it matches the
        # default, it wasn't set explicitly, and we overrule it...
        if hasattr(conf,k) and conf.getProp(k) != conf.getDefaultProperty(k) :
                from Gaudi.Configuration import log
                log.warning('%s.%s has explicitly been set, NOT using requested predefined threshold %s, but keeping explicit value: %s '%(conf.name(),k,str(v),getattr(conf,k)))
        else :
            if ( type(v) == dict ): # special case for dictionaries (needed in topo)
                val = conf.getProp(k)
                val.update(v)
                setattr(conf,k,val)
            else :
                setattr(conf,k,v)


def Name2Threshold(name) :
    if not hasattr(Name2Threshold,'_dict') : Name2Threshold._dict = {}
    if name not in Name2Threshold._dict : 
        from Gaudi.Configuration import log
        log.warning(' '+'#'*(41+len(name)) )
        log.warning(' ## Using trigger threshold settings "%s" ##'%name)
        mod = getattr( __import__('HltSettings.%s' % name ), name )
        cls = getattr( mod, name )
        Name2Threshold._dict[name] = cls()
        #log.warning(' ## type is %-30s ##' % Name2Threshold._dict[name].HltType() )
        log.warning(' '+'#'*(41+len(name)) )
    return Name2Threshold._dict[name]
