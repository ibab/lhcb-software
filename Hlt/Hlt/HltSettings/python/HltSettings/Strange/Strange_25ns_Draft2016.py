# =============================================================================
# @file   Strange_25ns_Draft2016.py
# @author Jessica Prisciandaro (jessica.prisciandaro@cern.ch)
# @date   05.03.2016
# =============================================================================
"""Threshold settings for Hlt2 Strange lines for 2016.
WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS 
"""

from GaudiKernel.SystemOfUnits import GeV, mm, MeV , picosecond

class Strange_25ns_Draft2016(object) :
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            
            'Hlt2StrangeKPiPiPi'
            ]
            
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Strange.Lines     import StrangeLines            
        d.update({StrangeLines :
                      {'Common' : {'TrChi2' : 3,
                                   'TrGP' : 0.3
                                   },
                       'KPiPiPi' : { 'piMinIpChi2' :  25. ,
                                     'KMassWin' : 100 * MeV ,
                                     'KMaxDOCA' : 2. * mm ,
                                     'KVtxChi2' : 25. ,  # adimensional
                                     'KMinPt' : 300 * MeV ,
                                     'KMinDIRA' : 0.999 ,   # adimensional
                                     'KMaxIpChi2' : 25 ,  # adimensional
                                     'KMinTauPs'  : 10 * picosecond
                                     }
                       
                       }
                  })
        
        return d
    


