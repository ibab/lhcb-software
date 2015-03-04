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

class TurboTest( object ):
    """
    Settings for 2012 running.

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author Sean Benson
    @date 2014-11-02
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
        return '0x0044'

    def HltType(self) :
        self.verifyType( TurboTest ) 
        return          'TurboTest'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """

        from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf

        thresholds = { Hlt1TrackLinesConf :    { 'AllL0_PT'         :  1250
                                               , 'AllL0_P'          :  3000
                                               , 'AllL0_IPChi2'     :  16
                                               , 'AllL0_TrChi2'     :  2.0
                                               , 'AllL0_GEC'        : 'Loose'
                                               , 'Muon_PT'       :  1000 
                                               , 'Muon_P'        :  3000 
                                               , 'Muon_IPChi2'   :    16
                                               , 'Muon_TrChi2'   :     2.5  
                                               , 'Muon_GEC'      : 'Loose'
                                               , 'Photon_PT'     :  1200
                                               , 'Photon_P'      :  3000
                                               , 'Photon_IPChi2' :    16
                                               , 'Photon_TrChi2' :     2.0
                                               , 'Prescale'          : {'Hlt1TrackAllL0'                   : 1.0 }} 
                       , Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 0.000001 
                                                                       , 'Hlt2Forward'      : 0.00001
                                                                       , 'Hlt2DebugEvent'   : 0.000001  }
                                               } 
                       }

        from Hadrons_September2012 import Hadrons_September2012
        __update_conf__(thresholds,  Hadrons_September2012().Thresholds() )

        return thresholds
                       
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = ['Hlt2PassThrough', 'Hlt2DiMuonJPsiTurbo'
                ]
        
        from Hadrons_September2012 import Hadrons_September2012
        hlt2.extend( Hadrons_September2012().ActiveHlt2Lines() )
       
        return hlt2
       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines =  [ 'Hlt1TrackAllL0', 'Hlt1TrackMuon',  'Hlt1TrackPhoton', 'Hlt1Lumi' ]
        
        return lines 
