class RadiativeLines:
    """
    Threshold settings for Hlt2 topological lines.
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION!!!!!!!!
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = ['Hlt2RadiativeTopoTrackTOS',
                 'Hlt2RadiativeTopoPhotonL0']
        
        return lines

   
    
    def Thresholds(self) :
        """Returns a dictionary of cuts."""

        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2RadiativeTopoLines  import Hlt2RadiativeTopoConf
        d.update( { Hlt2RadiativeTopoConf : {
                    # Track cuts
                    'Track_PT_MIN'       : 700.0  , # MeV
                    'Track_P_MIN'        : 5000.0 , # MeV
                    'Track_IPCHI2_MIN'   : 10.0   , # Dimensionless
                    'Track_CHI2_MAX'     : 5    , # Dimensionless
                    # Track combination cuts
                    'DITRACK_AMAXDOCA_MAX'      : 0.15   , # mm        
                    'DITRACK_MINTRCHI2DOF_MAX'  : 3      , # Dimensionless
                    'DITRACK_VTXCHI2_MAX'       : 10     , # Dimensionless
                    'DITRACK_M_MAX'             : 2000   , # MeV
                    'DITRACK_PT_MIN'            : 1500   , # MeV
                    # Photon
                    'photon_PT_MIN'      : 2500.0 , # MeV
                    # Track + photon combination cuts
                    'B_SUMPT_MIN'        : 5000   , # MeV
                    'B_FDCHI2_MIN'       : 64     , # Dimensionless
                    'B_PT_MIN'           : 1000   , # MeV
                    'B_CORRM_MAX'        : 7000  , # MeV
                    'B_CORRM_MIN'        : 4000   , # MeV
                    # GEC
                    'GEC_USE'            : True   ,
                    'GEC_NTRACK_MAX'     : 120    , # max number of tracks
                   }})

        return d
