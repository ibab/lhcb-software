class CharmHadronLines :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [ ## from Hlt2CharmHadD02HHLines
                  'Hlt2Charm2Body'
                  #, 'Hlt2Charm2BodyInc'
                  , 'Hlt2Charm2BodyWideMass'
             ]
        
        return lines

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        d = { }

        from Hlt2Lines.Hlt2CharmHadD02HHLines import Hlt2CharmHadD02HHLinesConf
        d.update( { Hlt2CharmHadD02HHLinesConf       : {
                        ## Cut values for one stage track-fit (OSTF) lines.
                        ## 2Body
                          'OSTFAllTrkPtLL_2Body'          : 800.0      # in MeV
                        , 'OSTFAllTrkPLL_2Body'           : 5000.0     # in MeV
                        , 'OSTFAllTrkPVIPChi2LL_2Body'    : 2.0        # unitless
                        , 'OSTFAllTrkChi2UL_2Body'        : 5.0        # unitless
                        , 'OSTFPairMinDocaUL_2Body'       : 0.10       # in mm
                        , 'OSTFTrkMaxPtLL_2Body'          : 1500.0     # in MeV
                        , 'OSTFVtxPVDispChi2LL_2Body'     : 25.0       # unitless
                        , 'OSTFVtxChi2UL_2Body'           : 10.0       # unitless
                        , 'OSTFPointLL_2Body'             : 0.99985    # unitless
                        ## 2-body inclusive!
		                , 'OSTFAllTrkPtLL_2BodyInc'          : 500.0      # in MeV
		                , 'OSTFAllTrkPLL_2BodyInc'           : 5000.0     # in MeV
		                , 'OSTFAllTrkPVIPChi2LL_2BodyInc'    : 16.0        # unitless
		                , 'OSTFAllTrkChi2UL_2BodyInc'        : 3.0        # unitless
		                , 'OSTFPairMinDocaUL_2BodyInc'       : 0.15       # in mm
                        , 'OSTFVtxPVDispChi2LL_2BodyInc'     : 100.       # unitless  
		                , 'OSTFTrkMaxPtLL_2BodyInc'          : 1000.0     # in MeV
		                , 'OSTFDPtLL_2BodyInc'               : 1500.0     # in MeV
		                , 'OSTFDSumPtLL_2BodyInc'            : 1800.0     # in MeV
		                , 'OSTFDIPChi2LL_2BodyInc'           : 2.0       # unitless
		                , 'OSTFMCOR_MIN_2BodyInc'            : 300.        # MeV
		                , 'OSTFMCOR_MAX_2BodyInc'            : 3200.        # MeV
                        , 'Prescale'                : {
                                  'Hlt2CharmOSTF2BodyWideMass' : 0.01
                                }
                        , 'Postscale'               : { }
                    
                    }
                  } )

        return d
            
   
