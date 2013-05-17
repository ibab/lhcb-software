
class VanDerMeerScan_Jul12:
    """
    Threshold settings for VDM scans. This is a 'master' settings file
    which should be used to create 6 VDM TCKs differing only by the
    prescales of the beam-gas HLT lines (in VDM we use in total 6 beam-gas lines).
    NOTE: It is essential that the 6 VDM TCks are fast-run-changeable!

    TCK1: All 6 beam-gas lines: prescale = 0.
    TCK2: All 6 beam-gas lines: prescale = 1.
    TCK3: Hlt1BeamGasCrossingForcedRecoFullZ: prescale = 1/3.5;  The other 5 BG Lines: prescale = 1.
    TCK4: Hlt1BeamGasCrossingForcedRecoFullZ: prescale = 1/5.0;  The other 5 BG Lines: prescale = 1.
    TCK5: Hlt1BeamGasCrossingForcedRecoFullZ: prescale = 1/7.0;  The other 5 BG Lines: prescale = 1.
    TCK6: Hlt1BeamGasCrossingForcedRecoFullZ: prescale = 1/10.0; The other 5 BG Lines: prescale = 1.
    TCK7: Hlt1BeamGasCrossingForcedRecoFullZ: prescale = 1/15.0; The other 5 BG Lines: prescale = 1.
    TCK8: Hlt1BeamGasCrossingForcedRecoFullZ: prescale = 1/20.0; The other 5 BG Lines: prescale = 1.
    TCK9: Hlt1BeamGasCrossingForcedRecoFullZ: prescale = 0.;     The other 5 BG Lines: prescale = 1.
    
    @author V. Balagura, J. Albrecht
    @date 2011-02-25
    @author P. Hopchev
    @date 2012-05-16
    @author R. Matev
    @date 2012-07-01
    """
    
    __all__ = ( 'ActiveHlt1Lines','ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self) or
             self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self) ) : 
            raise RuntimeError('Must update HltType when modifying ActiveHlt.Lines()')


    def __init__(self) :
        self.NanoBanks =  ['ODIN','HltLumiSummary','HltRoutingBits','DAQ','Velo']
   
    def L0TCK(self) :
        return '0x0041'

    def HltType(self) :
        self.verifyType( VanDerMeerScan_Jul12 )
        return          'VanDerMeerScan_Jul12'
    
    def ActiveHlt1Lines(self) :
        """ Returns a list of active Hlt1 Lines """
        lines = [ 'Hlt1Lumi'
                , 'Hlt1L0CALO'
                , 'Hlt1L0PU'
                , 'Hlt1MBMicroBiasVeloRateLimited'
                ]

        ### Add the beam-gas Lines
        from VanDerMeerScan_BeamGasConf_Jul12 import VanDerMeerScan_BeamGasConf_Jul12
        lines.extend(VanDerMeerScan_BeamGasConf_Jul12().ActiveHlt1Lines())

        return lines 


    def ActiveHlt2Lines(self) :
        """ Returns a list of active Hlt2 Lines """
        return  [ ]

    def Thresholds(self) :
        """ Returns a dictionary of cuts """
        
        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        
        thresholds = {
                       Hlt1L0LinesConf  : { 'Prescale'   : { 'Hlt1L0CALO' : 1.
                                                           , 'Hlt1L0PU'   : 1. }
                                          , 'Postscale'  : { 'Hlt1L0CALO' : 'RATE(25)' 
                                                           , 'Hlt1L0PU'   : 'RATE(25)' } 
                                          , 'L0Channels' : [ 'PU', 'CALO' ]
                                          }
                     , Hlt1LumiLinesConf : { 'MaxRate'    : 40000000 #take all
                                           , 'Prescale'   : { 'Hlt1Lumi' :  1. }
                                           , 'Postscale'  : { 'Hlt1Lumi' :  1. }
                                           }
                     , Hlt1MBLinesConf : { 'Postscale'  : { 'Hlt1MBMicroBias.*RateLimited' : 'RATE(100)' } 
                                         , 'MicroBiasOdin': 'scale( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger, RATE(2000,LoKi.Scalers.RandomPhasePeriodicLimiter) )' 
                                         }
                     }
        ### Extend the config dictionary with the beam-gas VDM settings
        from VanDerMeerScan_BeamGasConf_Jul12 import VanDerMeerScan_BeamGasConf_Jul12
        bgThres = VanDerMeerScan_BeamGasConf_Jul12().Thresholds()

        thresholds.update(bgThres)
        
        return thresholds
