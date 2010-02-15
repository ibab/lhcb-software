class Default :

    __all__ = ( 'Thresholds' )
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from StrippingSelections.StrippingPass import StrippingPassConf
        return { StrippingPassConf() : { 
                                         'PassPrescale' : 0.1, 
                                         'PhysPrescale' : 0.1
                                       } 
               }
