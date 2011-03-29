class VanDerMeerScan_Feb11 :
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
        self.verifyType( VanDerMeerScan_Feb11 )
        return          'VanDerMeerScan_Feb11'
    
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
    
        
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        
        thresholds = { Hlt1L0LinesConf :{ 'Prescale'  : { 'Hlt1L0CALO'   : 'RATE(1000)' }
                                          }
                       }
        return thresholds
    '''
                       , Hlt1LumiLinesConf : { 'MaxRate'    : 40000000 #take all
                                              , 'Prescale'  : { 'Hlt1Lumi' :  1. }
                                              , 'Postscale' : { 'Hlt1Lumi' :  1. }
            }

                       , Hlt1BeamGasLinesConf : { 'ForcedInputRateLimit'  :  5000
                                                  , 'Prescale' :  { 'Hlt1BeamGasBeam1' :                1.0
                                                                    , 'Hlt1BeamGasBeam2' :                1.0
                                                                    , 'Hlt1BeamGasCrossing' :             1.0
                                                                    , 'Hlt1BeamGasCrossingForcedRZReco' : 1.0
                                                                    }
                                                  ,'Postscale' :  { 'Hlt1BeamGasBeam1' :            'RATE(100)'
                                                                    , 'Hlt1BeamGasBeam2' :          'RATE(100)'
                                                                    , 'Hlt1BeamGasCrossingForcedRZReco' : 'RATE(25 )'
                                                                    
                                                                    }
                                                  }
                       }
                       
        return thresholds
    '''

    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines = [ 'Hlt1L0CALO'
                  #, 'Hlt1Lumi'
                  #, 'Hlt1BeamGasBeam1'
                  #, 'Hlt1BeamGasBeam2'
                  #, 'Hlt1BeamGasCrossingForcedRZReco'
                 ]              
        return lines 


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active Hlt2 lines
        """
        return  [ ]





