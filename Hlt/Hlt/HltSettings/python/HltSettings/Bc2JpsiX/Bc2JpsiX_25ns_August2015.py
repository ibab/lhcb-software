from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class Bc2JpsiX_25ns_August2015 :
    """
    Threshold settings for Hlt2 Bc2JpsiX lines: Bc2JpsiX_25ns_August2015
    
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
            'Hlt2TFBc2JpsiMuX',
            'Hlt2Bc2JpsiH'
            ]
        
        return lines


    def Thresholds(self) :
        
        d = {}
        
        from Hlt2Lines.Bc2JpsiX.Lines import Bc2JpsiXLines

        d.update (
            {
              Bc2JpsiXLines : {
                'TFBc2JpsiMuX' : {            
                              'Prescale': 1.0,
                              'JpsiCuts': """
                                          (MINTREE('mu+'==ABSID,PT) >  1.2*GeV)
                                          & (ADMASS('J/psi(1S)') < 220 *MeV)
                                          & (VFASPF(VCHI2PDOF) < 20)
                                          """,
                              'DauCuts' : """
                                          (PT > 2.0*GeV)
                                          & (TRCHI2DOF < 3)
                                          """,
                              'ComCuts' : """
                                          (in_range( 3.0*GeV, AM, 7.1*GeV))
                                          """,
                              'MomCuts' : """
                                          (VFASPF(VCHI2PDOF) < 25)
                                          & (in_range( 3.1*GeV, MM, 7.0*GeV))
                                          & (PT> 5.0 *GeV)
                                          """
                              },
                'Bc2JpsiH'     : {
                              'Prescale': 1.0,
                              'JpsiCuts': """
                                          (MINTREE('mu+'==ABSID,PT) > 0.8*GeV)
                                          & (ADMASS('J/psi(1S)') < 100*MeV)
                                          & (VFASPF(VCHI2PDOF) < 25)
                                          """,
                              'DauCuts' : """
                                          (PT > 1.5*GeV)
                                          & (TRCHI2DOF < 3)
                                          """,
                              'ComCuts' : """
                                          (in_range( 6.0*GeV, AM, 6.7*GeV))
                                          """,
                              'MomCuts' : """
                                          (VFASPF(VCHI2PDOF) < 25)
                                          & (in_range( 6.1*GeV, MM, 6.6*GeV))
                                          & (PT > 5.*GeV)
                                          & (BPVLTIME()>0.2*ps)
                                          """
                              }
                }              
              }
            )
        
        return d


    
