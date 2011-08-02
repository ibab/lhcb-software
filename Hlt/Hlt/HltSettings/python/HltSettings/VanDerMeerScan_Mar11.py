
class VanDerMeerScan_Mar11 :
    """
    Threshold settings for Hlt1 for 2010 conditions with pileup
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author V. Balagura, J. Albrecht
    @date 2011-02-25
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
        self.NanoBanks =  ['ODIN','HltLumiSummary','HltRoutingBits','DAQ', 'Velo' ]
   
    def L0TCK(self) :
        return '0x1710'

    def HltType(self) :
        self.verifyType( VanDerMeerScan_Mar11 )
        return          'VanDerMeerScan_Mar11'
    
    def ActiveHlt1Lines(self) :
        """ Returns a list of active Hlt1 Lines """
        lines = [ 'Hlt1Lumi'
                , 'Hlt1L0CALO'
                , 'Hlt1L0PU'
                , 'Hlt1MBMicroBiasVeloRateLimited' 
                ]              

        ### Add the beam-gas Lines
        from VanDerMeerScan_BeamGasConf_Mar11 import VanDerMeerScan_BeamGasConf_Mar11
        lines.extend(VanDerMeerScan_BeamGasConf_Mar11().ActiveHlt1Lines())

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
        from VanDerMeerScan_BeamGasConf_Mar11 import VanDerMeerScan_BeamGasConf_Mar11
        bgThres = VanDerMeerScan_BeamGasConf_Mar11().Thresholds()

        thresholds.update(bgThres)

        
        return thresholds




