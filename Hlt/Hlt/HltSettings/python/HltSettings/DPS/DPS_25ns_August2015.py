from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class DPS_25ns_August2015 :
    """
    Threshold settings for Hlt2 Double Parton Scattering lines: DPS_25ns_August2015
    
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
            'Hlt2DPS:2x2mu',
            'Hlt2DPS:2xHc',
            'Hlt2DPS:2mu&Hc'
            ]
        
        return lines


    def Thresholds(self) :
        
        d = {}
        
        from Hlt2Lines.DPS.Lines import DPSLines

        d.update (
            {
              DPSLines : {
                 'DPS:2x2mu' : {            

                               },
                 'DPS:2xHc' : {            

                               },
                 'DPS:2mu&Hc' : {            

                               },
                 }              
              }
            )
        
        return d

    
