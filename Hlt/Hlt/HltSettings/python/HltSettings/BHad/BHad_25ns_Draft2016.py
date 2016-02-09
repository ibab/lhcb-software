from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class BHad_25ns_Draft2016 :
    """
    Threshold settings for Hlt2 B2HHHH lines: Draft 2016
    """
    __all__ = ( 'ActiveHlt2Lines' )

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        lines = [
                'Hlt2BHadB02PpPpPmPm',
                ]

        return lines


    def Thresholds(self) :

        d = {}
        from Hlt2Lines.BHad.Lines import BHadLines
        d.update ({BHadLines : {
                 'Prescale' : {},
                 'TrackGEC' : { 'NTRACK_MAX'           : 10000},
                 'Common' : {'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS",
                             'Trk_ALL_TRCHI2DOF_MAX'    :  3.0,
                             'Trk_ALL_P_MIN'            :  5000 * MeV,
                             'VCHI2PDOF_MAX'            :  10.0,
                            },
                 # Particles for the "Detached" CPV lines
                 'B2HHHHSharedDetachedB0Child_K' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  250 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'B2HHHHSharedDetachedB0Child_pi' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  250 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'B2HHHHSharedDetachedB0Child_TightK' : {
                                 'PID_LIM'                  :  7,
                                 'Trk_ALL_PT_MIN'           :  250 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'B2HHHHSharedDetachedB0Child_Tightpi' : {
                                 'PID_LIM'                  :  3,
                                 'Trk_ALL_PT_MIN'           :  250 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'B2HHHHSharedDetachedB0Child_Pr' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  250 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'TighterProtons' :  {
                                  'PIDp_MIN'              :  5.0,
                                  'DeltaPID_MIN'          :  5.0,
                                  'P_MIN'                 :  10000 * MeV
                                  },

                             }     })

        return d
