# =============================================================================
# @file   DiElectron_25ns_Draft2016.py
# @author Jessica Prisciandaro (jessica.prisciandaro@cern.ch)
# @date   12.02.2016
# =============================================================================
"""Threshold settings for Hlt2 DiElectron lines for 2016.
WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS 
"""

from GaudiKernel.SystemOfUnits import GeV, mm, MeV 

class DiElectrons_25ns_Draft2016(object) :
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            
            'Hlt2DiElectronElSoft'
            ]
            
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.DiElectron.Lines     import DiElectronLines            
        d.update({DiElectronLines :
                      {'ElSoft' :          {'MaxMass'    :   1000 * MeV,
                                            'MinVDZ'     :     0,
                                            'MinBPVDira' :     0
                                            }                    
                       }
                  })
        
        return d
    


