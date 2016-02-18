from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class Bottomonium_25ns_Draft2016 :
    """
    Threshold settings for Hlt2 Bottomonium lines: Bottomonium_25ns_August2015
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author S. Stracka < simone.stracka@unipi.it > 
    @date 2016-02-12
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2BottomoniumDiPhi',
            #'Hlt2BottomoniumDiKstar',
            #'Hlt2BottomoniumDiPhiTurbo',
            'Hlt2BottomoniumDiKstarTurbo'
            ]
        
        return lines


    def Thresholds(self) :
        
        d = {}
        
        from Hlt2Lines.Bottomonium.Lines import BottomoniumLines

        d.update (
            {
              BottomoniumLines : {
            'DiKstar': { 'Prescale': 1.0,
                         'L0Req'   : "(L0_DATA('Spd(Mult)') < 600 )",
                         'Hlt1Req' : "HLT_PASS_RE('^Hlt1Bottomonium2KstarKstarDecision$')",
                         'DauCuts' : """
                            (INTREE( ('K+'==ABSID) & (PT > 1.*GeV)
                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                            & (INTREE( ('pi+'==ABSID) & (PT > 1.*GeV)
                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK < 0.)))
                            & (PT > 2.*GeV)
                            & (VFASPF(VCHI2PDOF) < 9)
                            & (ADMASS('K*(892)0') < 75.*MeV)
                            """,
                         'ComCuts' : """
                            (in_range( 8.8*GeV, AM, 10.1*GeV))
                            & (APT> 1.9 *GeV)
                            """,
                         'MomCuts' : """
                            (VFASPF(VCHI2PDOF) < 9)
                            & (in_range( 8.9*GeV, MM, 10.0*GeV))
                            & (PT> 2.0 *GeV)
                            """
                         },
            'DiPhi' :  { 'Prescale': 1.0,
                         'L0Req'   : "(L0_DATA('Spd(Mult)') < 600 )",
                         'Hlt1Req' : "HLT_PASS_RE('^Hlt1Bottomonium2PhiPhiDecision$')",
                         'DauCuts' : """
                            (INTREE( ('K+'==ID) & (PT > 0.65*GeV)
                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                            & (INTREE( ('K-'==ID) & (PT > 0.65*GeV)
                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                            & (PT > 1.2*GeV)
                            & (VFASPF(VCHI2PDOF) < 9)
                            & (ADMASS('phi(1020)') < 30.*MeV)
                            """,
                         'ComCuts' : """
                            (in_range( 8.8*GeV, AM, 10.7*GeV))
                            & (APT> 0.9 *GeV)
                            """,
                         'MomCuts' : """
                            (VFASPF(VCHI2PDOF) < 9)
                            & (in_range( 8.9*GeV, MM, 10.6*GeV))
                            & (PT> 1.0 *GeV)
                            """
                         }
                 }
              }
            )
        
        return d
