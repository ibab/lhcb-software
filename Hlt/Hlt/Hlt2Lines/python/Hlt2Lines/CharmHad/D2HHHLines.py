from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHHLines() :
    def localcuts(self) :
        return { # Now the combiner for the CPV lines
                 'Dpm2HHH' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  150.0,
                                 'BPVLTIME_MIN'             :  0.4 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99995,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  6,
                                 'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS"
                                },
                 # Be aware the following cuts are used for a flavour tagging calibration line
                 # as well as for the Charm physics lines. If you change them please be
                 # aware and consult as appropriate.
                 'Ds2HHH' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  100.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9999,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  6,
                                 'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS"
                                },
                 'Dpm2PPP' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  150.0,
                                 'BPVLTIME_MIN'             :  0.4 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99995,
                                 'ASUMPT_MIN'               :  3200 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  6,
                                },
                 'Dpm2PKK' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  150.0,
                                 'BPVLTIME_MIN'             :  0.4 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99995,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  6,
                                },
                 'Ds2PPP' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  100.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99995,
                                 'ASUMPT_MIN'               :  3200 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  6,
                                },
                 'Ds2KPP' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  100.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9999,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  6,
                                },
                 'Dpm2KKK' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  4.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  150.0,
                                 'BPVLTIME_MIN'             :  0.4 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99995,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  10,
                                },
                 'Ds2KKK' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  4.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  100.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9999,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  10,
                                },
                 ## The masses of Lambda_c+ and Xi_c+ are separated by 181 MeV,
                 ##   so the mass windows are adjacent unless they are very
                 ##   narrow.  It makes sense to do joint combinatorics.
                 'LcXic2HHH' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  9.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  16.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  25.0,
                                 'BPVLTIME_MIN'             :  0.15 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99995,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2553. * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2543.0 * MeV,
                                 'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS"
                                },
                 'Lc2HHH'    : {
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2362.0 * MeV,
                                },
                 'Xic2HHH'   : {
                                 'Mass_M_MIN'               :  2392.0 * MeV,
                                 'Mass_M_MAX'               :  2543.0 * MeV,
                                },

                 # The combiner for the lifetime unbiased lines
                 'Dpm2HHH_LTUNB' : {
                                 'TisTosSpec'               : "Hlt1Track.*Decision%TIS",
                                 'BPVLTIME_MIN'             :  0.5 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                },
                 'Ds2HHH_LTUNB' : {
                                 'TisTosSpec'               : "Hlt1Track.*Decision%TIS",
                                 'BPVLTIME_MIN'             :  0.25 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                },
                 # Wide mass window accomodates the Xi_c+
                 'Lc2HHH_LTUNB'    : {
                                 'TisTosSpec'               : "Hlt1Track.*Decision%TIS",
                                 'BPVLTIME_MIN'             :  0.15 * picosecond,
                                 'BPVDIRA_MIN'              :  0.95,
                                 'ASUMPT_MIN'               :  5000. * MeV,
                                 'AM_MIN'                   :  2201. * MeV,
                                 'AM_MAX'                   :  2553. * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2543.0 * MeV,
                                },
                 # The combiner for a KPi asymmetry line
                 'Dpm2KPiPi_ForKPiAsym' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  25.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  49.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  100.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  200.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  200.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  0.0,
                                 'BPVLTIME_MIN'             :  0.5 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9999,
                                 'ASUMPT_MIN'               :  1000 * MeV,
                                 'AM_MIN'                   :  1809 * MeV,
                                 'AM_MAX'                   :  1929 * MeV,
                                 'Mass_M_MIN'               :  1819.0 * MeV,
                                 'Mass_M_MAX'               :  1919.0 * MeV,
                                },
                 'DConeVar05' : { "ConeAngle" : 0.5,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "Location"  : 'DConeVar05' },
                 #'DConeVar10' : { "ConeAngle" : 1.0,
                 #                 "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                 #                 "Location" :  'DConeVar10'},
                 'DConeVar15' : { "ConeAngle" : 1.5,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "Location" :  'DConeVar15'},
                 'DConeVar10DKPP' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D+ ->^K- pi+ pi+ ]CC" : "KmConeVar10"} },
                 'DConeVar10DPPK' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D+ ->K+ ^pi- pi+ ]CC" : "PimConeVar10"} },
                 'DConeVar10DKKP' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D+ -> ^K- K+ pi+]CC" : "KmConeVar10"} },
                 'DConeVar10DPPP' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D+ ->^pi- pi+ pi+ ]CC" : "PimConeVar10"} },
                 'DConeVar10DKKK' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D+ ->^K- K+ K+ ]CC" : "KmConeVar10"} },
                 'DConeVar10DPKK' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D+ -> ^pi- K+ K+]CC" : "PimConeVar10"} },

                 'DConeVar10DsPKK' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D_s+ ->K+ K+ ^pi- ]CC" : "PimConeVar10"} },
                 'DConeVar10DsPPK' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D_s+ ->K+ ^pi- pi+ ]CC" : "PimConeVar10"} },
                 'DConeVar10DsKKP' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D_s+ ->^K- K+ pi+ ]CC" : "KmConeVar10"} },
                 'DConeVar10DsKKK' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D_s+ ->^K- K+ K+ ]CC" : "KmConeVar10"} },
                 'DConeVar10DsKPP' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                  "DaughterLocations" : {"[D_s+ ->^K- pi+ pi+ ]CC" : "KmConeVar10"} },

                 'DConeVar10DsPPP' : { "ConeAngle" : 1.0,
                                  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                 "DaughterLocations" : {"[D_s+ -> ^pi- pi+ pi+]CC" : "PimConeVar10"} },

                 'VertexIsolation' : { "Location"  : "VertexIsoInfo" }
                }

    def locallines(self):
        from Stages import MassFilter, TagDecay, HHHCombiner
        from Stages import DetAsym_DpToKmPipPip
        from Stages import SharedSoftTagChild_pi
        from Stages import SharedPromptChild_K, SharedPromptChild_pi
        from Stages import SharedTighterPromptChild_p
        #
        from Stages import D2HHH_DpToKmPipPip, D2HHH_DpToKpPimPip
        from Stages import D2HHH_DpToKmKpPip, D2HHH_DpToKpKpPim
        from Stages import D2HHH_DpToPimPipPip, D2HHH_DpToKmKpKp
        #
        from Stages import D2HHH_DspToKpPimPip, D2HHH_DspToKmPipPip
        from Stages import D2HHH_DspToKpKpPim,   D2HHH_DspToKmKpPip
        from Stages import D2HHH_DspToPimPipPip, D2HHH_DspToKmKpKp
        #
        from Stages import LcXic2HHH_LcpToKmPpPip, LcXic2HHH_LcpToKmPpKp
        from Stages import LcXic2HHH_LcpToPimPpPip, LcXic2HHH_LcpToPimPpKp
        from Stages import Lc2HHH_LcpToKmPpPip, Xic2HHH_XicpToKmPpPip


        ## MassFilters for imported combiner instances.
        ## Since the D+ and D_s+ combinatorics were separated, do we still need
        ##   to apply MassFilter?

        ## The end-point of the DpToKmPipPip line is used as input to the DPS
        ##   module and must be shared.  Because it is not used anywhere else
        ##   in CharmHad, it can be defined here rather than in Stages.py
        DpToKmPipPip  = MassFilter('DpToKmPipPip'
                                , inputs=[D2HHH_DpToKmPipPip]
                                , nickname = 'Dpm2HHH'
                                , shared = True, reFitPVs = True)

        ## The other D+ -> 3h lines can have non-shared mass filters.
        DpToKpPimPip  = MassFilter('Dpm2HHH'
                                , inputs=[D2HHH_DpToKpPimPip], reFitPVs = True)
        DpToKmKpPip   = MassFilter('Dpm2HHH'
                                , inputs=[D2HHH_DpToKmKpPip], reFitPVs = True)
        DpToPimPipPip = MassFilter('Dpm2HHH'
                                , inputs=[D2HHH_DpToPimPipPip], reFitPVs = True)
        DpToKmKpKp    = MassFilter('Dpm2HHH'
                                , inputs=[D2HHH_DpToKmKpKp], reFitPVs = True)
        DpToKpKpPim   = MassFilter('Dpm2HHH'
                                , inputs=[D2HHH_DpToKpKpPim], reFitPVs = True)


        ## The end-point of the DspToKmKpPip line is used as input to the DPS
        ##   module and must be shared.  Because it is not used anywhere else
        ##   in CharmHad, it can be defined here rather than in Stages.py
        DspToKmKpPip   = MassFilter('DspToKmKpPip'
                                , inputs=[D2HHH_DspToKmKpPip]
                                , nickname = 'Ds2HHH'
                                , shared = True, reFitPVs = True)

        ## The other D_s+ -> 3h lines can have non-shared mass filters.
        DspToKpKpPim   = MassFilter('Ds2HHH'
                               , inputs=[D2HHH_DspToKpKpPim], reFitPVs = True)
        DspToKpPimPip  = MassFilter('Ds2HHH'
                               , inputs=[D2HHH_DspToKpPimPip], reFitPVs = True)
        DspToPimPipPip = MassFilter('Ds2HHH'
                               , inputs=[D2HHH_DspToPimPipPip], reFitPVs = True)
        DspToKmKpKp    = MassFilter('Ds2HHH'
                               , inputs=[D2HHH_DspToKmKpKp], reFitPVs = True)
        DspToKmPipPip    = MassFilter('Ds2HHH'
                               , inputs=[D2HHH_DspToKmPipPip], reFitPVs = True)

        ## The end-point of the LcpToPpKmPip line is used as input to the DPS
        ## module and as input the CharmSpectroscopyLines.py lines.
        ## Because it is shared within CharmHad, its definition is in Stages.py.
        LcpToKmPpKp   = MassFilter('Lc2HHH'
                            , inputs=[LcXic2HHH_LcpToKmPpKp], reFitPVs = True)
        LcpToPimPpPip = MassFilter('Lc2HHH'
                            , inputs=[LcXic2HHH_LcpToPimPpPip], reFitPVs = True)
        LcpToPimPpKp  = MassFilter('Lc2HHH'
                            , inputs=[LcXic2HHH_LcpToPimPpKp], reFitPVs = True)


        ## Maximally-biased-lifetime combiners.
        Dpm2HHH_DpToKmPipPip_LTUNB = HHHCombiner( 'Comb'
                        , decay = "[D+ -> K- pi+ pi+]cc"
                        , inputs = [ SharedPromptChild_pi, SharedPromptChild_K ]
                        , nickname = 'Dpm2HHH_LTUNB' )

        DpToKmPipPip_LTUNB = MassFilter('Dpm2HHH_LTUNB'
                        , inputs=[Dpm2HHH_DpToKmPipPip_LTUNB], reFitPVs = True)


        Ds2HHH_DspToKmKpPip_LTUNB = HHHCombiner( 'Comb'
                        , decay = "[D_s+ -> K- K+ pi+]cc"
                        , inputs = [ SharedPromptChild_pi, SharedPromptChild_K ]
                        , nickname = 'Ds2HHH_LTUNB' )

        DspToKmKpPip_LTUNB = MassFilter('Ds2HHH_LTUNB'
                        , inputs = [Ds2HHH_DspToKmKpPip_LTUNB], reFitPVs = True)


        Lc2HHH_LcpToPpKmPip_LTUNB = HHHCombiner( 'Comb'
                        , decay = "[Lambda_c+ -> K- p+ pi+]cc"
                        , inputs = [ SharedPromptChild_pi, SharedPromptChild_K
                                     , SharedTighterPromptChild_p ]
                        , nickname = 'Lc2HHH_LTUNB' )

        LcpToPpKmPip_LTUNB = MassFilter('Lc2HHH_LTUNB'
                        , inputs = [Lc2HHH_LcpToPpKmPip_LTUNB], reFitPVs = True)


        ## Selection specifically for detector asymmetry measurements
        Dpm2KPiPi_ForKPiAsym = MassFilter('Dpm2KPiPi_ForKPiAsym'
                        , inputs=[DetAsym_DpToKmPipPip], reFitPVs = True)


        ## The stages dictionary should be a clear two-column list from
        ##   which the lines defined in this module can be directly read.
        stages = {
                  ## CPV D+ -> HHH lines.
                  'DpToKmPipPipTurbo'        : [DpToKmPipPip],
                  'DpToKpPimPipTurbo'        : [DpToKpPimPip],
                  'DpToKmKpPipTurbo'         : [DpToKmKpPip],
                  'DpToPimPipPipTurbo'       : [DpToPimPipPip],
                  'DpToKmKpKpTurbo'          : [DpToKmKpKp],
                  'DpToKpKpPimTurbo'         : [DpToKpKpPim],

                  ## CPV D_s+ -> HHH lines.
                  ## Explicitly includes forbidden KmPipPip.
                  'DspToKmKpPipTurbo'        : [DspToKmKpPip],
                  'DspToKpKpPimTurbo'        : [DspToKpKpPim],
                  'DspToKpPimPipTurbo'       : [DspToKpPimPip],
                  'DspToPimPipPipTurbo'      : [DspToPimPipPip],
                  'DspToKmKpKpTurbo'         : [DspToKmKpKp],
                  'DspToKmPipPipTurbo'       : [DspToKmPipPip],

                  # CPV L_c+ -> HHH lines
                  'LcpToPpKmPipTurbo'        : [Lc2HHH_LcpToKmPpPip],
                  'LcpToPpKmKpTurbo'         : [LcpToKmPpKp],
                  'LcpToPpPimPipTurbo'       : [LcpToPimPpPip],
                  'LcpToPpKpPimTurbo'        : [LcpToPimPpKp],

                  ## Xi_c+ -> p K- pi+, DO NOT SEND TO TURBO.
                  'XicpToPpKmPip'            : [Xic2HHH_XicpToKmPpPip],

                  # Now the three CF lifetime unbiased lines
                  'DpToKmPipPip_LTUNBTurbo'  : [DpToKmPipPip_LTUNB],
                  'DspToKmKpPip_LTUNBTurbo'  : [DspToKmKpPip_LTUNB],
                  'LcpToPpKmPip_LTUNBTurbo'  : [LcpToPpKmPip_LTUNB],

                  # Now the KPi asymmetry line
                  'DpToKmPipPip_ForKPiAsym'  : [Dpm2KPiPi_ForKPiAsym]
            }

        # Create Full stream version of this line for use in
        # flavour tagging calibration.
        stages['DspToKmKpPip'] = stages['DspToKmKpPipTurbo']

        return stages

    def relatedInfo(self):
        from Hlt2Lines.Utilities.Hlt2RelatedInfo import Hlt2RelInfoConeVariables, Hlt2RelInfoVertexIsolation
        DConeVar05 = Hlt2RelInfoConeVariables('DConeVar05')
        DConeVar15 = Hlt2RelInfoConeVariables('DConeVar15')
        DConeVar10DsPPP = Hlt2RelInfoConeVariables('DConeVar10DsPPP')
        DConeVar10DKPP  = Hlt2RelInfoConeVariables('DConeVar10DKPP')
        DConeVar10DPPK  = Hlt2RelInfoConeVariables('DConeVar10DPPK')
        DConeVar10DKKP  = Hlt2RelInfoConeVariables('DConeVar10DKKP')
        DConeVar10DPPP  = Hlt2RelInfoConeVariables('DConeVar10DPPP')
        DConeVar10DKKK  = Hlt2RelInfoConeVariables('DConeVar10DKKK')
        DConeVar10DPKK  = Hlt2RelInfoConeVariables('DConeVar10DPKK')
        DConeVar10DsPKK  = Hlt2RelInfoConeVariables('DConeVar10DsPKK')
        DConeVar10DsPPK  = Hlt2RelInfoConeVariables('DConeVar10DsPPK')
        DConeVar10DsKKP  = Hlt2RelInfoConeVariables('DConeVar10DsKKP')
        DConeVar10DsKKK  = Hlt2RelInfoConeVariables('DConeVar10DsKKK')
        DConeVar10DsKPP  = Hlt2RelInfoConeVariables('DConeVar10DsKPP')

        VertexIso = Hlt2RelInfoVertexIsolation("VertexIsolation")
        relInfo = {  
                   'DpToKpPimPipTurbo' : [DConeVar10DPPK, DConeVar05,  DConeVar15,VertexIso],  
                   'DpToPimPipPipTurbo' : [DConeVar10DPPP, DConeVar05,  DConeVar15,VertexIso],  
                   'DpToKmKpKpTurbo' : [DConeVar10DKKK, DConeVar05,  DConeVar15,VertexIso], 
                   'DpToKpKpPimTurbo' : [DConeVar10DPKK, DConeVar05,  DConeVar15,VertexIso],   
                   'DspToKpKpPimTurbo' : [DConeVar10DsPKK, DConeVar05,  DConeVar15,VertexIso],  
                   'DspToKpPimPipTurbo' : [DConeVar10DsPPK, DConeVar05,  DConeVar15,VertexIso],  
                   'DspToPimPipPipTurbo' : [DConeVar10DsPPP, DConeVar05, DConeVar15, VertexIso],  
                   'DspToKmKpKpTurbo' : [DConeVar10DsKKK, DConeVar05,  DConeVar15,VertexIso],  
                   'DspToKmPipPipTurbo' : [DConeVar10DsKPP, DConeVar05,  DConeVar15,VertexIso]                  
} 
        return relInfo
