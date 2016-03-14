from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class DPS_25ns_Draft2016 :
    """
    Threshold settings for Hlt2 Double Parton Scattering lines: DPS_25ns_Draft2016
    
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
            'Hlt2DPS2x2mu',
            'Hlt2DPS2xHc',
            'Hlt2DPS2muHc'
            ]
        
        return lines


    def Thresholds(self) :
        
        d = {}
        
        from Hlt2Lines.DPS.Lines import DPSLines

        d.update (
            {
              DPSLines : {
                  'Prescale'  : {'DPS2x2mu'  : 1.0,
                                 'DPS2xHc'   : 1.0,
                                 'DPS2muHc' : 1.0},
                  'Postscale' : {'DPS2x2mu'  : 1.0,
                                 'DPS2xHc'   : 1.0,
                                 'DPS2muHc' : 1.0}
                 }
              }
            )
        
        return d

    
