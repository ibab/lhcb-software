
class VanDerMeerScan_Mar12_BeamGasOn:
    """
    Threshold settings for VDM scans with all beam-gas lines active.
    Intended to be used when the beams are at a fixed position and do not collide.

    @author V. Balagura, J. Albrecht
    @date 2011-02-25
    @author P. Hopchev
    @date 2012-03-07
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
        return '0x0041'

    def HltType(self) :
        self.verifyType( VanDerMeerScan_Mar12_BeamGasOn )
        return          'VanDerMeerScan_Mar12'
    
    def ActiveHlt1Lines(self) :
        """ Returns a list of active Hlt1 Lines """
        lines = [ 'Hlt1Lumi'
                , 'Hlt1L0CALO'
                , 'Hlt1L0PU'
                , 'Hlt1MBMicroBiasVeloRateLimited' 
                ]              

        ### Add the beam-gas Lines
        from VanDerMeerScan_BeamGasConf_Mar12 import VanDerMeerScan_BeamGasConf_Mar12
        lines.extend(VanDerMeerScan_BeamGasConf_Mar12().ActiveHlt1Lines())

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
                                           , 'Prescale'   : { 'Hlt1Lumi' :  0.5 }
                                           , 'Postscale'  : { 'Hlt1Lumi' :  1.  }
                                           }
                     , Hlt1MBLinesConf : { 'Postscale'  : { 'Hlt1MBMicroBias.*RateLimited' : 'RATE(100)' } 
                                         , 'MicroBiasOdin': 'scale( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger, RATE(2000,LoKi.Scalers.RandomPhasePeriodicLimiter) )' 
                                         }
                     }
        ### Extend the config dictionary with the beam-gas VDM settings
        from VanDerMeerScan_BeamGasConf_Mar12 import VanDerMeerScan_BeamGasConf_Mar12
        bgThres = VanDerMeerScan_BeamGasConf_Mar12().Thresholds('BeamGasOn')

        thresholds.update(bgThres)

        
        return thresholds




