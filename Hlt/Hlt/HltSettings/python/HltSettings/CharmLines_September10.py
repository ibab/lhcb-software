class CharmLines :
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

        lines = ['Hlt2Charm3BodySA'
                     , 'Hlt2Charm2BodySA'
                     , 'Hlt2Charm4BodySA'
                     , 'Hlt2CharmTF2BodySA'
                     , 'Hlt2CharmTF3BodySA'
                     , 'Hlt2CharmTF4BodySA'
                     , 'Hlt2CharmTF2BodySignal'
                     , 'Hlt2CharmTF3BodySignal'
                     , 'Hlt2CharmTF4BodySignal'
                     , 'Hlt2CharmTF2BodyWideMassSA'
                     , 'Hlt2CharmTF3BodyWideMassSA'
                     , 'Hlt2CharmTF4BodyWideMassSA'
                     , 'Hlt2CharmTF2BodyWideMass'
                     , 'Hlt2CharmTF3BodyWideMass'
                     , 'Hlt2CharmTF4BodyWideMass'
                     , 'Hlt2CharmOSTF2Body'
                     , 'Hlt2CharmOSTF3Body'
                     , 'Hlt2CharmOSTF2BodyInc'
                     , 'Hlt2CharmOSTF4Body'
                     , 'Hlt2CharmOSTF2BodyWideMass'
                     , 'Hlt2CharmOSTF3BodyWideMass'
                     , 'Hlt2CharmOSTF4BodyWideMass'
             ]
        
        return lines

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        d = { }

        from Hlt2Lines.Hlt2CharmLines        import Hlt2CharmLinesConf
        d.update( { Hlt2CharmLinesConf       : {
                        ## Cut values for robust stages.
                          'ComRobAllTrkPtLL'        : 300.0      # in MeV
                        , 'ComRobAllTrkPLL'         : 2000.0     # in MeV
                        , 'ComRobAllTrkPVIPLL'      : 0.025      # in mm
                        , 'ComRobPairMinDocaUL'     : 0.20       # in mm
                        , 'ComRobPairMaxDocaUL'     : 1.0        # in mm
                        , 'ComRobTrkMaxPtLL'        : 1500.0     # in MeV
                        , 'ComRobVtxPVDispLL'       : 2.0        # in mm
                        , 'ComRobVtxPVRDispLL'      : 0.2        # in mm
                        , 'RobustPointingUL'        : 0.20       # unitless
                        , 'ComRobUseGEC'            : True       # do or do not 
                        , 'ComRobGEC'               : 120        # max N trk
                        ## Cut values for post-track fit stages
                        , 'ComTFAllTrkPtLL'         : 300.0      # in MeV
                        , 'ComTFAllTrkPLL'          : 2000.0     # in MeV
                        , 'ComTFAllTrkPVIPChi2LL'   : 9.0        # unitless
                        , 'ComTFAllTrkChi2UL'       : 10.0       # unitless
                        , 'ComTFPairMinDocaUL'      : 0.20       # in mm
                        , 'ComTFPairMaxDocaUL'      : 1.0        # in mm
                        , 'ComTFTrkMaxPtLL'         : 1500.0     # in MeV
                        , 'ComTFVtxPVDispChi2LL'    : 25.0       # unitless
                        , 'TFPointUL'               : 0.20       # unitless
                        ## Cut values for one stage track-fit (OSTF) lines.
                        ## 2Body
                        , 'OSTFAllTrkPtLL_2Body'          : 800.0      # in MeV
                        , 'OSTFAllTrkPLL_2Body'           : 5000.0     # in MeV
                        , 'OSTFAllTrkPVIPChi2LL_2Body'    : 2.0        # unitless
                        , 'OSTFAllTrkChi2UL_2Body'        : 5.0        # unitless
                        , 'OSTFPairMinDocaUL_2Body'       : 0.10       # in mm
                        , 'OSTFTrkMaxPtLL_2Body'          : 1500.0     # in MeV
                        , 'OSTFVtxPVDispChi2LL_2Body'     : 25.0       # unitless
                        , 'OSTFVtxChi2UL_2Body'           : 10.0       # unitless
                        , 'OSTFPointLL_2Body'             : 0.99985    # unitless
                        ## 3Body
                        , 'OSTFAllTrkPtLL_3Body'          : 500.0      # in MeV
                        , 'OSTFAllTrkPLL_3Body'           : 5000.0     # in MeV
                        , 'OSTFAllTrkPVIPChi2LL_3Body'    : 4.0        # unitless
                        , 'OSTFAllTrkChi2UL_3Body'        : 5.0        # unitless
                        , 'OSTFPairMinDocaUL_3Body'       : 0.20       # in mm
                        , 'OSTFPairMaxDocaUL_3Body'       : 1.0       # in mm
                        , 'OSTFTrkMaxPtLL_3Body'          : 1000.0        # in MeV
                        , 'OSTFVtxPVDispChi2LL_3Body'     : 150.0        # unitless
                        , 'OSTFVtxChi2UL_3Body'           : 15.0       # unitless
                        , 'OSTFDIPChi2UL_3Body'           : 15.0        # unitless
                        , 'OSTFDPtLL_3Body'               : 2000.0     # PT
                        , 'OSTFDSumPtLL_3Body'            : 1800.0     # PT   
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
                        ## Slots for K_S h h' lines
                        , 'KshhTFHHTrkPLL'          : 1500.0     # in MeV
                        , 'KshhTFHHTrkChi2UL'       : 20.0       # unitless
                        , 'KshhTFHHTrkMaxPVIPLL'    : 0.1        # in mm
                        , 'KshhTFHHVtxChi2UL'       : 20.0       # in mm
                        , 'KshhTFHHVtxPVDispLL'     : -1.0       # in mm
                        , 'KshhTFHHPtLL'            : 500.0      # in mm
                        , 'KshhTFKsLLTrkPLL'        : 2000.0     # in MeV
                        , 'KshhTFKsLLTrkPVIPChi2LL' : 9.0        # unitless
                        , 'KshhTFKsLLTrkChi2UL'     : 20.0       # unitless
                        , 'KshhTFKsLLCombSymMassWin': 50.0       # in MeV
                        , 'KshhTFKsLLMothSymMassWin': 11.4       # in MeV
                        , 'KshhTFKsLLVtxChi2UL'     : 30.0       # unitless
                        , 'KshhTFKsLLVtxPVDispZLL'  : -1000.0    # in mm
                        , 'KshhTFKsLLVtxPVDispZUL'  : 650.0      # in mm
                        , 'KshhTFKsLLVtxPVDispChi2LL' : 100.0    # unitless
                        , 'KshhTFKsLLDiraLL'        : 0.9997     # unitless
                        , 'KshhTFKsDDTrkPLL'        : 2000.0     # in MeV
                        , 'KshhTFKsDDTrkPVIPChi2LL' : 4.0        # unitless
                        , 'KshhTFKsDDTrkChi2UL'     : 20.0       # unitless
                        , 'KshhTFKsDDCombSymMassWin': 80.0       # in MeV
                        , 'KshhTFKsDDMothSymMassWin': 24.9       # in MeV
                        , 'KshhTFKsDDVtxChi2UL'     : 30.0       # unitless
                        , 'KshhTFKsDDVtxPVDispZLL'  : 0.0        # in mm
                        , 'KshhTFKsDDVtxPVDispZUL'  : 2300.0     # in mm
                        , 'KshhTFKsDDVtxPVDispChi2LL' : 100.0    # unitless
                        , 'KshhTFKsDDDiraLL'        : 0.9997     # unitless
                        , 'KshhTFDVtxChi2UL'        : 20.0       # unitless
                        , 'KshhTFDVtxPVDispLL'      : -1.0       # in mm
                        , 'KshhTFDPtLL'             : 500.0      # in MeV
                        , 'KshhTFDDiraLL'           : 0.999      # unitless
                        , 'KshhTFDwKsLLSymMassWin'  : 100.0      # in MeV
                        , 'KshhTFDwKsDDSymMassWin'  : 120.0      # in MeV
                        , 'Prescale'                : {
                                  'Hlt2CharmTF2BodySA' : 1.0
                                , 'Hlt2CharmTF3BodySA' : 1.0
                                , 'Hlt2CharmTF4BodySA' : 1.0
                                , 'Hlt2CharmTF2BodyWideMass' : 0.1
                                , 'Hlt2CharmTF3BodyWideMass' : 0.1
                                , 'Hlt2CharmTF4BodyWideMass' : 0.1
                                , 'Hlt2CharmTF2BodyWideMassSA' : 0.1
                                , 'Hlt2CharmTF3BodyWideMassSA' : 0.1
                                , 'Hlt2CharmTF4BodyWideMassSA' : 0.1
                                , 'Hlt2CharmOSTF2BodyWideMass' : 0.01
                                , 'Hlt2CharmOSTF3BodyWideMass' : 0.01
                                , 'Hlt2CharmOSTF4BodyWideMass' : 0.01
                                }
                        , 'Postscale'               : {
                                  'Hlt2Charm2BodySA' : 0.05
                                , 'Hlt2Charm3BodySA' : 0.05
                                , 'Hlt2Charm4BodySA' : 0.05
                                , 'Hlt2CharmTF2BodyWideMassSA' : 0.05
                                , 'Hlt2CharmTF3BodyWideMassSA' : 0.05
                                , 'Hlt2CharmTF2BodyWideMass'   : 0.05
                                , 'Hlt2CharmTF2BodyWideMass'   : 0.05
                                }
                    }
                  } )

        return d
            
   
