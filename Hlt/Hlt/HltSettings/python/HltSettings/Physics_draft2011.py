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
                print 'Warning: potential collision detected: %s -- %s' % (conf,k)
                print 'current: %s' % cur[k]
                print 'request: %s' % v
                if type(cur[k])==dict :
                    cur[k].update( v )
                else :
                    cur[k] = v
                print 'result: %s' % cur[k]

class Physics_draft2011:
    """
    draft settings for 2011 runnings
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, J. Albrecht
    @date 2010-06-23
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or
             self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self) ) :
            raise RuntimeError('Must update HltType when modifying ActiveHlt.Lines()')

    
    def L0TCK(self) :
        return '0x002c'

    def HltType(self) :
        self.verifyType( Physics_draft2011 ) 
        return          'Physics_draft2011'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
	"""
	from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf

        thresholds = { Hlt1TrackLinesConf : {   'AllL0_PT'      : 1450
                                            ,   'AllL0_P'       : 13300
                                            ,   'AllL0_IP'      : 0.110
                                            ,   'AllL0_IPChi2'  : 34
                                            ,   'AllL0_TrChi2'  : 3
                                            ,   'Muon_PT'       : 800 
                                            ,   'Muon_P'        : 8000 
                                            ,   'Muon_IP'       : 0.110
                                            ,   'Muon_IPChi2'   : 25
                                            ,   'Muon_TrChi2'   : 10                    
                                            ,   'Photon_PT'     : 800
                                            ,   'Photon_P'      : 8000
                                            ,   'Photon_IP'     : 0.13
                                            ,   'Photon_IPChi2' : 50
                                            ,   'Photon_TrChi2' : 5
                                            ,   'Velo_NHits'    : 9 
                                            ,   'Velo_Qcut'     : 999 
                                                }
                       }

        from Muons_draft2011 import Muons_draft2011
        __update_conf__(thresholds,  Muons_draft2011().Thresholds() )

        from Hadrons_draft2011 import Hadrons_draft2011
        __update_conf__(thresholds,  Hadrons_draft2011().Thresholds() )

        return thresholds




                       
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = []

        from Muons_draft2011 import Muons_draft2011
        hlt2.extend( Muons_draft2011().ActiveHlt2Lines() )

        from Hadrons_draft2011 import Hadrons_draft2011
        hlt2.extend( Hadrons_draft2011().ActiveHlt2Lines() )
        
        return hlt2
      

       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines = [ 'Hlt1TrackAllL0',
                  'Hlt1TrackMuon',
                  'Hlt1TrackPhoton' ]

        return lines 






