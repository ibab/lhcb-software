from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class CcDiHadron_25ns_August2015 :
    """
    Threshold settings for Hlt2 CcDiHadron lines: CcDiHadron_25ns_August2015
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author S. Stracka < simone.stracka@unipi.it > 
    @date 2015-07-20
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2DiProton',
            'Hlt2DiProtonLowMult',
            'Hlt2DiPhi'
            ]
        
        return lines


    def Thresholds(self) :
        
        d = {}
        
        from Hlt2Lines.CcDiHadron.Lines import CcDiHadronLines

        d.update (
            {
              CcDiHadronLines : {
                'DiProton' : {
                          'Prescale': 1.0,
                          'L0Req'   : "(L0_DATA('Spd(Mult)') < 300 )",
                          'Hlt1Req' : "HLT_PASS('Hlt1DiProtonDecision')",
                          'DauCuts' : """
                                      (PT > 1.9*GeV)
                                      & (TRCHI2DOF < 3)
                                      & (PIDp > 20)
                                      & ((PIDp-PIDK) > 10)
                                      """,
                          'ComCuts' : """
                                      (in_range( 2.75*GeV, AM, 4.1*GeV))
                                      & (APT> 6.3 *GeV)
                                      """,
                          'MomCuts' : """
                                      (VFASPF(VCHI2PDOF) < 9)
                                      & (in_range( 2.8*GeV, MM, 4.0*GeV))
                                      & (PT> 6.5 *GeV)
                                      """
                          },
                'DiProtonLowMult' : { 
                          'Prescale': 1.0,
                          'L0Req'   : "(L0_DATA('Spd(Mult)') < 10 )",
                          'Hlt1Req' : "HLT_PASS('Hlt1DiProtonLowMultDecision')",
                          'DauCuts' : """
                                      (PT > 0.5*GeV)
                                      & (TRCHI2DOF < 3)
                                      & (PIDp > 10)
                                      & ((PIDp-PIDK) > 5)
                                      """,
                          'ComCuts' : """
                                      (AM > 2.75*GeV )
                                      """,
                          'MomCuts' : """
                                      (VFASPF(VCHI2PDOF) < 9)
                                      & (MM > 2.8*GeV )
                                      """
                          },
                'DiPhi' :    { 
                          'Prescale': 1.0,
                          'L0Req'   : "(L0_DATA('Spd(Mult)') < 600 )",
                          'Hlt1Req' : None,
                          'DauCuts' : """
                                      (INTREE( ('K+'==ID) & (PT > 0.65*GeV)
                                      & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                                      & (INTREE( ('K-'==ID) & (PT > 0.65*GeV)
                                      & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                                      & (PT > 0.8*GeV)
                                      & (VFASPF(VCHI2PDOF) < 9)
                                      & (ADMASS('phi(1020)') < 20.*MeV)
                                      """,
                          'ComCuts' : """
                                      (in_range( 2.75*GeV, AM, 4.85*GeV))
                                      & (APT> 1.9 *GeV)
                                      """,
                          'MomCuts' : """
                                      (VFASPF(VCHI2PDOF) < 9)
                                      & (in_range( 2.8*GeV, MM, 4.8*GeV))
                                      & (PT> 2.0 *GeV)
                                      """
                          }
                }              
              }
            )
        
        return d
    

    
