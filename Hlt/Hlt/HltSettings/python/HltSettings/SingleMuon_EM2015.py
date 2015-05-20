from GaudiKernel.SystemOfUnits import GeV, mm, MeV 

class SingleMuon_EM2015(object) :
    """
    Threshold settings for Hlt2 SingelMuon lines: EM 2015
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author C. Linn
    @date 2015-05-19
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2SingleMuon',
            'Hlt2SingleMuonHighPT',
            'Hlt2SingleMuonLowPT',
                                   ]
            
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }


        from Hlt2Lines.SingleMuon.Lines import SingleMuonLines
        d.update({SingleMuonLines : 
                  {'Prescale'   :   {'SingleMuon'    : 0.5, 
                                     'LowPT'         : 0.1},
                   'HltReq'     :   {"SingleMuon"    : "HLT_PASS_RE('Hlt1TrackMuonDecision')"},
                   'Common'     :   {'TrChi2'        :    3,    # Adimensional
                                     'Pt'            : 1300 * MeV },
                   'SingleMuon' :   {'IP'            : 0.0 * mm,
                                     'IPChi2'        : 16 },  # Adimensional
                   'HighPT'     :   {'HighPt'        : 10000 *MeV },
                   'LowPT'      :   { 'HighPt'       : 4800 * MeV }
                   }
                 })
           
        return d
    


