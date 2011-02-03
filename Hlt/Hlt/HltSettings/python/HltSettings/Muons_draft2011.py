class Muons_draft2011 :
    """
    Threshold settings for Hlt2 muon lines: minimal draft for 2011
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht
    @date 2010-12-02
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [

            #'Hlt2MuonFromHLT1',
            'Hlt2SingleMuon',
            'Hlt2SingleMuonHighPT',

            'Hlt2DiMuon',  
            'Hlt2DiMuonLowMass',
            #'Hlt2DiMuonSameSign',

            # control rate via: 1) mass: 120 --> 70,
            # then rediscuss with beta_s DO NOT INCREASE PT!!
            'Hlt2DiMuonJPsi',
            'Hlt2DiMuonPsi2S',
            'Hlt2DiMuonB',
            'Hlt2DiMuonZ',

            # control rate via: DLS cut, dimuon PT
            'Hlt2DiMuonDetached',
            'Hlt2DiMuonDetachedJPsi',
            'Hlt2DiMuonDetachedHeavy',

            'Hlt2DiMuonNoPV',

            # control rate via: prescale
            'Hlt2DiMuonDY1',
            'Hlt2DiMuonDY2',
            'Hlt2DiMuonDY3',
            'Hlt2DiMuonDY4',

            # control rate via flight distance chi2, sum IP chi2 cuts - avoid PT cuts
            'Hlt2Mu1Track',
            'Hlt2Mu2Track',
            'Hlt2Mu3Track'
                 
            ]
            
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2InclusiveMuonLines  import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines  import Hlt2InclusiveDiMuonLinesConf
        from Hlt2Lines.Hlt2MuNTrackLines  import Hlt2MuNTrackLinesConf
        d.update( { Hlt2InclusiveDiMuonLinesConf : { 
            ## Cut values
            'UnbiasedDiMuonMinMass'    : 2900      # MeV
            ,'UnbiasedDiMuonPt'        : 0.0      # MeV
            ,'UnbiasedDiMuonMuPt'      : 0.0      # MeV
            ,'UnbiasedDiMuonLowPt'     : 0.0      # MeV
            ,'UnbiasedDiMuonLowMuPt'   : 0.0      # MeV
            ,'UnbiasedJPsiPt'          : 0.0      # MeV
            ,'UnbiasedJPsiMuPt'        : 0.0      # MeV
            ,'UnbiasedJPsiMassWindow'  : 120      # MeV
            ,'UnbiasedPsi2SPt'         : 0.0      # MeV
            ,'UnbiasedPsi2SMuPt'       : 0.0      # MeV
            ,'UnbiasedPsi2SMassWindow' : 120      # MeV
            ,'UnbiasedBmmMinMass'      : 4700     # MeV
            ,'UnbiasedZmmMinMass'      :40000     # MeV
            ,'UnbiasedZmmPt'           :  0.0     # MeV
            ,'DetachedDiMuonPt'        : 1500     #MeV
            ,'DetachedDiMuonDLS'       :   7

            
            #,'DYPt'                    : 0.0      # MeV

            , 'Prescale'   : { 'Hlt2DiMuon'          : 0.1
                               , 'Hlt2DiMuonLowMass' : 0.002
                               , 'Hlt2DiMuonDY1'              :  0.1
                               #,'Hlt2DiMuonDY2'              :  1
                               #,'Hlt2BiasedDiMuonIP'         :  1
                               }           
            }}
                  )

        d.update( { Hlt2InclusiveMuonLinesConf : {
            'SingleMuonPt'         : 1300      # MeV
            #,'SingleMuonIP'        : 0.11     # mm
            ,'SingleMuonHighPt'    : 10000     # MeV
            ,'Prescale'   : { 'Hlt2MuonFromHLT1'       : 0.0001
                              ,'Hlt2SingleMuon'        : 0.2 
                              ,'Hlt2SingleHighPTMuon'  : 1.0
                              }
            }}
                  )
        
        d.update( { Hlt2MuNTrackLinesConf : {
            'L0FILTER'            : "L0_CHANNEL_RE('.*Muon')"
            ,'HLT1FILTER'         : "HLT_PASS_RE('Hlt1TrackMuon.*Decision')"
            #mu + n tracks filter cuts
            ,'MCOR_MAX'           : 7000.0  # MeV
            ,'MCOR_MIN'           : 0.0     # MeV 
            ,'MCOR_NTR_MIN'       : 4000.0  # MeV 
            ,'SUM_PT_1TR_MIN'     : 2000.0  # MeV
            ,'SUM_PT_2TR_MIN'     : 2000.0  # MeV
            ,'SUM_PT_3TR_MIN'     : 2600.0  # MeV
            ,'MAX_PT_MIN'         : 1500.0  # MeV 
            ,'MAX_PT_NTR_MIN'     : 1500.0  # MeV 
            ,'SUM_IPCHI2_1TR_MIN' : 50      # unitless
            ,'SUM_IPCHI2_2TR_MIN' : 75      # unitless
            ,'SUM_IPCHI2_3TR_MIN' : 100     # unitless
            ,'BPVVDCHI2_MIN'      : 36.0    # unitless
            ,'MIN_TRCHI2DOF_MAX'  : 3       # unitless
            #combination cuts
            ,'AMAXDOCA_MAX'       : 0.12    # mm 
            ,'AMAXDOCA_MIN'       : 0.12    # mm 
            ,'DIRA_LOOSE_MIN'     : 0.99    # rad
            ,'DIRA_TIGHT_MIN'     : 0.995   # rad
            # mother cuts
            ,'MASS_1TR_VETO'      : 2000.0  # MeV 
            ,'MASS_2TR_VETO'      : 3000.0  # MeV 
            ,'MASS_3TR_VETO'      : 4000.0  # MeV 
            # cuts on input particles
            ,'ALL_MIPCHI2DV_MIN'  : 16.0    # unitless
            ,'ALL_TRCHI2DOF_MAX'  : 3.0     # unitless
            ,'ALL_MU_PT_MIN'      : 800.0   # MeV
            ,'ALL_TR_PT_MIN'      : 600.0   # MeV
            ,'ALL_P_MIN'          : 5000.0  # MeV
            ,'Prescale'   : { 'Hlt2Mu1Track'   : 1.0
                              ,'Hlt2Mu2Track'  : 1.0
                              ,'Hlt2Mu3Track'  : 0.0
                              }
            }}
                  )


        return d
    


