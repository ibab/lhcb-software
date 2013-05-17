class VanDerMeerScan_BeamGasConf_May12:
    '''
    Configuration for the beam-gas lines to be used in the VDM scan TCKs
    @author P. Hopchev
    @date 2012-05-16
    '''
    
    __all__ = ( 'ActiveHlt1Lines', 'GetRateLimits', 'Thresholds' )

    ### Variable specifiying if rate limits should be applied on the output rate of the BG lines
    ### If True: use rate limit of 1.e7 Hz, else use rate limits of O(100) Hz (see function "GetRateLimits")
    UseMaxRateLimits = True
    
    def ActiveHlt1Lines(self):
        ''' Returns a list of Hlt1 Lines to be used in the VDM TCK '''
        
        lines = [ 'Hlt1BeamGasNoBeamBeam1'
                , 'Hlt1BeamGasNoBeamBeam2'
                , 'Hlt1BeamGasBeam1'
                , 'Hlt1BeamGasBeam2'
                , 'Hlt1BeamGasCrossingForcedReco'
                , 'Hlt1BeamGasCrossingForcedRecoFullZ'
                , 'Hlt1VeloClosingMicroBias'
                ]

        return lines

    def GetRateLimits(boolUseMaxRate, MaxRate = 1.e7):
        ''' Provides dictionaries with rate limits for each Hlt1 BeamGas Line to be used in the VDM TCK.
            The function argument 'boolUseMaxRate' specifies if real rate limits will be applied or
            an unresonably high rate limit will be used to emulate the absence of rate limits.
            Rate limits are specified only for the output rate, while there is no need for limits
            on the events passing the corresponding L0 requiremets.
            NB! The rate limits for the L0 req. should be 'float' (or 'SCALE(float)'), while the output
            rate limits should be of the form 'RATE(float)'
        ''' 
        RateLimL0  = { 'NoBeamBeam1'        : MaxRate
                     , 'NoBeamBeam2'        : MaxRate
                     , 'Beam1'              : MaxRate
                     , 'Beam2'              : MaxRate 
                     , 'CrossingForcedReco' : MaxRate
                     , 'CrossingForcedRecoFullZ' : MaxRate
                     }

        RateLimOut = { 'NoBeamBeam1'        : 'RATE(%f)'%MaxRate if boolUseMaxRate else 'RATE(200)'
                     , 'NoBeamBeam2'        : 'RATE(%f)'%MaxRate if boolUseMaxRate else 'RATE(200)'
                     , 'Beam1'              : 'RATE(%f)'%MaxRate if boolUseMaxRate else 'RATE(200)'
                     , 'Beam2'              : 'RATE(%f)'%MaxRate if boolUseMaxRate else 'RATE(200)'
                     , 'CrossingForcedReco' : 'RATE(%f)'%MaxRate if boolUseMaxRate else 'RATE(500)'
                     , 'CrossingForcedRecoFullZ' : 'RATE(%f)'%MaxRate if boolUseMaxRate else 'RATE(500)'
                     }

        return RateLimL0, RateLimOut                   

    ### Get the Rate Limit Dictionaries
    BGRateLimitsL0, BGRateLimitsOut = GetRateLimits(UseMaxRateLimits)

    def Thresholds(self):
        ''' Sets different settings on top of the default ones (defined in Hlt1BeamGasLines.py)
            The rate limits are taken from already defined dictionaries. '''

        presc = 1.; prescFullZ = 1.
            
        from Hlt1Lines.Hlt1BeamGasLines import Hlt1BeamGasLinesConf

        dictThresholds = \
        {
            Hlt1BeamGasLinesConf :

                {
                ### L0DU requirements
                  'L0Filter'     : { 'NoBeamBeam1'   : "L0_ALL"
                                   , 'NoBeamBeam2'   : "L0_ALL"
                                   , 'Beam1'         : "L0_ALL"
                                   , 'Beam2'         : "L0_ALL"
                                   , 'BB'            : "L0_ALL"
                                   }

                ### Rate Limits of events passing the L0 requirements
                ### (The Input prescale for all lines is 1., i.e. pass all)
                , 'L0RateLimit'  : { 'NoBeamBeam1'     : self.BGRateLimitsL0['NoBeamBeam1']
                                   , 'NoBeamBeam2'     : self.BGRateLimitsL0['NoBeamBeam2']
                                   , 'Beam1'           : self.BGRateLimitsL0['Beam1']
                                   , 'Beam2'           : self.BGRateLimitsL0['Beam2']
                                   , 'ForcedReco'      : self.BGRateLimitsL0['CrossingForcedReco']
                                   , 'ForcedRecoFullZ' : self.BGRateLimitsL0['CrossingForcedRecoFullZ']
                                   }

                ### No GECs in the VDM configuration  
                , 'UseGEC' : 'None'

                ### Minimum number of tracks for the produced vertices (#tr/vtx > X)
                , 'VertexMinNTracks' : '10'

                ### Input Prescales   
                , 'Prescale'     : { 'Hlt1BeamGasNoBeamBeam1'             : presc
                                   , 'Hlt1BeamGasNoBeamBeam2'             : presc
                                   , 'Hlt1BeamGasBeam1'                   : presc
                                   , 'Hlt1BeamGasBeam2'                   : presc
                                   , 'Hlt1BeamGasCrossingForcedReco'      : presc
                                   , 'Hlt1BeamGasCrossingForcedRecoFullZ' : prescFullZ
                                   }

                ### Output Postscales
                , 'Postscale'    : { 'Hlt1BeamGasNoBeamBeam1'        : self.BGRateLimitsOut['NoBeamBeam1']
                                   , 'Hlt1BeamGasNoBeamBeam2'        : self.BGRateLimitsOut['NoBeamBeam2']
                                   , 'Hlt1BeamGasBeam1'              : self.BGRateLimitsOut['Beam1']
                                   , 'Hlt1BeamGasBeam2'              : self.BGRateLimitsOut['Beam1']
                                   , 'Hlt1BeamGasCrossingForcedReco' : self.BGRateLimitsOut['CrossingForcedReco']
                                   , 'Hlt1BeamGasCrossingForcedRecoFullZ' : self.BGRateLimitsOut['CrossingForcedRecoFullZ']
                                   }
                }
        }


        return dictThresholds

