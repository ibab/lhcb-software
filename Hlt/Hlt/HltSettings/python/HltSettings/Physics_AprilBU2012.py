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
	    

from Physics_April2012 import Physics_April2012

class Physics_AprilBU2012( Physics_April2012 ):
   def L0TCK(self) :
       return '0x003B'
       
   def Thresholds(self) :
       from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf
       thresholds = Physics_April2012.Thresholds(self)
       thresholds[ Hlt1TrackLinesConf ]['AllL0_PT'] = 1700
       return thresholds
