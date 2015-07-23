from GaudiKernel.SystemOfUnits import MeV, mm

class B2Kpi0_25ns_August2015 :
    """
    Threshold settings for Hlt2 B2Kpi0lines: August 2015
    """

    __all__ = ( 'ActiveHlt2Lines' )

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        lines = [
            'Hlt2B2Kpi0',
            'Hlt2B2K0pi0'
                ]

        return lines


    def Thresholds(self) :

        d = {}
        from Hlt2Lines.B2Kpi0.Lines import B2Kpi0Lines
        d.update ({B2Kpi0Lines : {
            'Common'       : {'NTRACK_MAX':300,
                              'TRACK_TRCHI2DOF_MAX'  :3.0,
                              'TRACK_TRGHOSTPROB_MAX':0.5,
                              'L0FILTER'  :"L0_CHANNEL('Photon')|L0_CHANNEL('Electron')",
                              'PI0_TISTOS':'L0(Photon|Electron).*Decision%TOS'},

            'FilteredPi0s' : {'PI0_PT_MIN':3500 * MeV,
                              'PI0_P_MIN' :5000 * MeV },

            'FilteredKaons': {'TRACK_TISTOS'    :'Hlt1TrackMVADecision%TOS',
                              'TRACK_PT_MIN'    :1200 * MeV,
                              'TRACK_P_MIN'     :12000 * MeV,
                              'TRACK_IPCHI2_MIN':50,
                              'TRACK_PIDK_MIN'  :-0.5 },

            'FilteredKSs'  : {'KS0_TISTOS':None,
                              'KS0_PT_MIN':500 * MeV,
                              'KS0_P_MIN' :8000 * MeV,
                              'KS0_ADMASS':15 * MeV,
                              'KS0_VCHI2PDOF_MAX':15,
                              'KS0_IPCHI2_MIN'   :10},

            'B2Kpi0'       : {'HLT1FILTER' :"HLT_PASS('Hlt1TrackMVADecision')",
                              'MASS_MIN'   :4000, ## units (MeV) are in cut string
                              'MASS_MAX'   :6200, ## units (MeV) are in cut string
                              'ASUM_PT_MIN':6500 * MeV,
                              'PT_MIN'     :5000 * MeV,
                              'MTDOCACHI2_MAX':8.0 },

            'B2K0pi0'      : {'HLT1FILTER' :"HLT_PASS_RE('Hlt1(Two)?TrackMVADecision')",
                              'MASS_MIN'   :4000, ## units (MeV) are in cut string
                              'MASS_MAX'   :6200, ## units (MeV) are in cut string
                              'ASUM_PT_MIN':5000 * MeV,
                              'PT_MIN'     :4000 * MeV,
                              'MTDOCACHI2_MAX':10.0 },
                    }

                                  })

        return d
                  
