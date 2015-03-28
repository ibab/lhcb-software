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

class Hlt1_TrackingOnly( object ):
    """
    Settings for only running the tracking in the HLT

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author S. Neubert, J. Albrecht, V. Gligorov
    @date 2013-03-27
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds', 'L0TCK' )

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or
             self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self) ) :
            raise RuntimeError( 'Must update HltType when modifying ActiveHlt.Lines()' )
        
    def L0TCK(self) :
        return '0x0046'

    def HltType(self) :
        self.verifyType( Hlt1_TrackingOnly ) 
        return          'Hlt1_TrackingOnly'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """

        from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf

        thresholds = { Hlt1TrackLinesConf :    { 'DoTiming'              : True
                                               , 'AllL0_PT'         :  1300
                                               , 'AllL0_P'          :  3000
                                               , 'AllL0_IPChi2'     :  13.
                                               , 'AllL0_TrChi2'     :  2.
                                               , 'AllL0_GEC'        : 'Loose'
                                               , 'AllL0_Velo_NHits' : 9 
                                               , 'AllL0_Velo_Qcut'  : 3  
                                               , 'AllL0_TrNTHits'   : 16
                                               , 'Muon_TrNTHits'    : 0 #OFF
                                               , 'Muon_Velo_NHits'  : 0 #OFF
                                               , 'Muon_Velo_Qcut'   : 999 #OFF
                                               , 'Muon_PT'          : 800.
                                               , 'Muon_P'           : 6000.
                                               , 'Muon_IPChi2'      : 6.
                                               , 'Muon_TrChi2'      : 2.
                                               , 'Muon_GEC'         : 'Loose'
                                               , 'Photon_PT'     :  1200
                                               , 'Photon_P'      :  3000
                                               , 'Photon_IPChi2' :    13.
                                               , 'Photon_TrChi2' :     2.
                                               , 'Photon_GEC'        : 'Loose'
                                               , 'L0Channels'        : {'AllL0'  : 'L0_DECISION_PHYSICS',
                                                                        'Muon'   : ('Muon', 'DiMuon'),
                                                                        'Photon' : ("Photon", "Electron")}
                                               , 'Prescale'          : {'Hlt1TrackAllL0'                   : 1.0}
                                                }
                        }
        return thresholds
                       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines = ['Hlt1TrackAllL0',]
       
        return lines 


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return []
