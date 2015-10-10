from GaudiKernel.SystemOfUnits import GeV, mm, MeV 

class SingleMuon_25ns_October2015(object) :
    """
    Threshold settings for Hlt2 single muon lines
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author F. Dettori
    @date 2015-07-01
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2SingleMuon',
            'Hlt2SingleMuonNoSPD',
            'Hlt2SingleMuonHighPT',
            'Hlt2SingleMuonVHighPT',
            'Hlt2SingleMuonLowPT',
            'Hlt2SingleMuonRare',
            ]
        
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }


        from Hlt2Lines.SingleMuon.Lines import SingleMuonLines
        d.update({SingleMuonLines : {
            'HltReq'  : {"SingleMuon" :  "HLT_PASS_RE('Hlt1TrackMuonDecision')",
                         "NoSPD" :  "HLT_PASS_RE('Hlt1TrackMuonNoSPDDecision')"
                         },
            'Common' :        {'TrChi2'     :   5,    # Adimensional
                               'Pt':            1000 * MeV },
            # Control with prescale
            'SingleMuon' :    {'IP'     : 0.25 * mm,
                               'IPChi2' : 100, # Adimensional
                               },
            'NoSPD' :  {'IP'     : 0.25 * mm,
                                    'IPChi2' : 100, # Adimensional
                                   },
            'HighPT':         { 'HighPt' : 10000 *MeV },
            
            'VHighPT':        { 'HighPt' : 15000 *MeV },
            
            'LowPT'   :       { 'HighPt' : 4800 * MeV },
            # Control with IpChi2
            'Rare'    :       { 'RarePtMin'          : 5000 * MeV,
                                'RarePtMax'       : 40000* MeV,
                                'RareIPChi2'      : 500, # Adimensional
                                'RareTrChi2Tight' : 2 ,
                                'RarePMax'  : 500 * GeV,
                                'muID'  : 1
                                },
            'Prescale'   : { 'Hlt2SingleMuon'      : 0.1,
                             'Hlt2SingleMuonNoSPD'      : 0.1
                             , 'Hlt2SingleMuonHighPT'    : 1.0
                             , 'Hlt2SingleMuonLowPT' : 0.002}
            }
                  }
                 )
           
        return d
    


