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

class TrackingOnly( object ):
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
        return '0x0044'

    def HltType(self) :
        self.verifyType( TrackingOnly ) 
        return          'TrackingOnly'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """

        from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf

        thresholds = { Hlt1TrackLinesConf :    { 'AllL0Tight_PT'         :  1700
                                               , 'AllL0Tight_P'          :  3000
                                               , 'AllL0Tight_IP'         :  0.100
                                               , 'AllL0Tight_IPChi2'     :  16  
                                               , 'AllL0Tight_TrChi2'     :  1.5 
                                               , 'AllL0Tight_GEC'        : 'Loose'
                                               , 'AllL0Tight_Velo_NHits' : 9 
                                               , 'AllL0Tight_Velo_Qcut'  : 3   
                                               , 'AllL0Tight_ValidateTT' : True 
                                               , 'AllL0_PT'         :  1600
                                               , 'AllL0_P'          :  3000
                                               , 'AllL0_IP'         :  0.100
                                               , 'AllL0_IPChi2'     :  16
                                               , 'AllL0_TrChi2'     :  2.0
                                               , 'AllL0_GEC'        : 'Loose'
                                               , 'AllL0_Velo_NHits' : 9 
                                               , 'AllL0_Velo_Qcut'  : 3  
                                               , 'AllL0_ValidateTT' : True
                                               ,   'AllL0VeloTT_Velo_NHits' : 9.
                                               ,   'AllL0VeloTT_Velo_Qcut'  : 3.
                                               ,   'AllL0VeloTT_IP'         : 0.100
                                               ,   'AllL0VeloTT_PT'          : 1600.
                                               ,   'AllL0VeloTT_P'           : 3000.
                                               ,   'AllL0VeloTT_IP'          : 0.100
                                               ,   'AllL0VeloTT_IPChi2'      : 16.
                                               ,   'AllL0VeloTT_TrChi2'      : 2.0
                                               ,   'AllL0VeloTT_TrNTHits'    : 16.
                                               ,   'AllL0VeloTT_GEC'         : 'Loose'
                                               ,   'AllL0VeloTT_ValidateTT'  : True
                                               ,   'AllL0VeloTTForw_Velo_NHits' : 9.
                                               ,   'AllL0VeloTTForw_Velo_Qcut'  : 3.
                                               ,   'AllL0VeloTTForw_IP'         : 0.1
                                               ,   'AllL0VeloTTForw_PT'          : 800.
                                               ,   'AllL0VeloTTForw_P'           : 3000.
                                               ,   'AllL0VeloTTForw_IPChi2'      : 16.
                                               ,   'AllL0VeloTTForw_TrChi2'      : 2.
                                               ,   'AllL0VeloTTForw_TrNTHits'    : 16.
                                               ,   'AllL0VeloTTForw_GEC'         : 'Loose'
                                               ,   'AllL0VeloTTForw_ValidateTT'  : True
                                               ,   'AllL0LifetimeUnbiased_Velo_Qcut'  : 3.
                                               ,   'AllL0LifetimeUnbiased_PT'          : 5000.
                                               ,   'AllL0LifetimeUnbiased_P'           : 15000.
                                               ,   'AllL0LifetimeUnbiased_TTPT'          : 8000.
                                               ,   'AllL0LifetimeUnbiased_TTP'           : 20000.
                                               ,   'AllL0LifetimeUnbiased_TrChi2'      : 3.
                                               ,   'AllL0LifetimeUnbiased_TrNTHits'    : 16.
                                               ,   'AllL0LifetimeUnbiased_GEC'         : 'Loose'
                                               ,   'AllL0LifetimeUnbiased_ValidateTT'  : False
                                               , 'Muon_PT'       :  1000 
                                               , 'Muon_P'        :  3000 
                                               , 'Muon_IP'       :     0.100
                                               , 'Muon_IPChi2'   :    16
                                               , 'Muon_TrChi2'   :     2.5  
                                               , 'Muon_GEC'      : 'Loose'
                                               , 'Muon_ValidateTT' : False
                                               , 'Muon_L0Channels' : 'Muon,DiMuon,MuonNoSPD,DiMuonNoSPD' 
                                               , 'Photon_PT'     :  1200
                                               , 'Photon_P'      :  3000
                                               , 'Photon_IP'     :     0.100
                                               , 'Photon_IPChi2' :    16
                                               , 'Photon_TrChi2' :     2.0
                                               , 'Photon_L0Channels' : 'PhotonHi,ElectronHi' 
                                               , 'Photon_GEC'        : 'Loose'
                                               , 'Photon_ValidateTT' : True
                                               , 'Prescale'          : {'Hlt1TrackAllL0'                   : 1.0, 
                                                                        'Hlt1TrackAllL0Tight'              : 1.0,
                                                                        'Hlt1TrackForwardPassThrough'      : 0,
                                                                        'Hlt1TrackForwardPassThroughLoose' : 0}
 
                                               }
                       , Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 0.0001 
                                                                       , 'Hlt2Forward'      : 0.00001
                                                                       , 'Hlt2DebugEvent'   : 0.000001  }
                                                        , 'Postscale' : { 'Hlt2ErrorEvent'   : 'RATE(0.01)' } }
                       }

        return thresholds

      
 
                       
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return ['Hlt2PassThrough','Hlt2Lumi','Hlt2DebugEvent','Hlt2Forward','Hlt2ErrorEvent','Hlt2Transparent','Hlt2LowMultHadron','Hlt2B2HH']
 
       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines =  [ 'Hlt1TrackAllL0VeloTTForw', 'Hlt1TrackMuon',  'Hlt1TrackPhoton', ]
        #lines =  [ 'Hlt1TrackMuon', 'Hlt1TrackAllL0Tight', 'Hlt1TrackPhoton' ]
       
        return lines 
