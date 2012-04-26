def __update_conf__( current, extra ) :
    for (conf,d) in extra.iteritems() :
        if conf not in current : 
            current[conf] = d
            continue
        cur = current[conf]
        for (k,v) in d.iteritems() :
            if k not in cur : 
                cur[k] = v
                continue
            if cur[k] == v : continue
            print 'Warning: potential collision detected: %s -- %s' % (conf,k)
            print 'current: %s' % cur[k]
            print 'request: %s' % v
            if type(cur[k])==dict :
                cur[k].update( v )
            else :
                cur[k] = v
            print 'result: %s' % cur[k]

from Physics_May2012 import Physics_May2012

class Physics_MayOPT_L03D_2012( Physics_May2012 ):
    """
    Settings for 2012 running.

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht, V. Gligorov
    @date 2012-04-17
    """
    def L0TCK(self) :
        return '0x003D'
	    
    def Thresholds(self) :
       from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf
       thresholds = Physics_May2012.Thresholds(self)
       thresholds[ Hlt1TrackLinesConf ]['AllL0_TrChi2'] = 2.0
       thresholds[ Hlt1TrackLinesConf ]['Photon_TrChi2'] = 2.0
       thresholds[ Hlt1TrackLinesConf ]['AllL0_ValidateTT'] = True
       thresholds[ Hlt1TrackLinesConf ]['Muon_ValidateTT'] = True
       thresholds[ Hlt1TrackLinesConf ]['Photon_ValidateTT'] = True
       return thresholds
