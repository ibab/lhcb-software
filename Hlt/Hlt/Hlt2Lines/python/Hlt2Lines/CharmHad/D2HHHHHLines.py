from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHHHHLines(Hlt2LinesConfigurableUser):
    def localcuts(self) :
        return {
                 ##  dictionary entries for decays instantiated in Stages.py
                 'Dch2KmPimPipPipPip' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'Trk_ALL_PT_MIN'           :  250 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                             'AM_MIN'                   :  1780. * MeV,
                             'AM_MAX'                   :  2080. * MeV,
                             'AM_3'                     :  139.570 * MeV,
                             'AM_4'                     :  139.570 * MeV,
                             'AM_5'                     :  139.570 * MeV,
                             'ACHI2DOCA_MAX'            :  15.0,
                             'ASUMPT_MIN'               :  2000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'BPVDIRA_MIN'              : 0.9994,
                             'BPVLTIME_MIN'             : 0.3 * picosecond,
                             'PT_MIN'                   :  2500 * MeV,
                             'IPCHI2_MAX'               : 15.0
                                          }

                ,'Dch2KmKpPimPipPip' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'Trk_ALL_PT_MIN'           :  250 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                             'AM_MIN'                   :  1780. * MeV,
                             'AM_MAX'                   :  2060. * MeV,
                             'AM_3'                     :  139.570 * MeV,
                             'AM_4'                     :  139.570 * MeV,
                             'AM_5'                     :  139.570 * MeV,
                             'ACHI2DOCA_MAX'            :  15.0,
                             'ASUMPT_MIN'               :  2000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'BPVDIRA_MIN'              : 0.9994,
                             'BPVLTIME_MIN'             : 0.3 * picosecond,
                             'PT_MIN'                   :  2500 * MeV,
                             'IPCHI2_MAX'               : 15.0
                                          }
                            
                ,'Dch2KmKmKpPipPip' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'Trk_ALL_PT_MIN'           :  250 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                             'AM_MIN'                   :  1780. * MeV,
                             'AM_MAX'                   :  2060. * MeV,
                             'AM_3'                     :  493.677 * MeV,
                             'AM_4'                     :  139.570 * MeV,
                             'AM_5'                     :  139.570 * MeV,
                             'ACHI2DOCA_MAX'            :  15.0,
                             'ASUMPT_MIN'               :  2000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'BPVDIRA_MIN'              : 0.9994,
                             'BPVLTIME_MIN'             : 0.3 * picosecond,
                             'PT_MIN'                   :  2500 * MeV,
                             'IPCHI2_MAX'               : 15.0
                                          }
##  designed for both Lambda_c and Xi_c^+
                ,'LcpToPpKmKpPimPip' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'Trk_ALL_PT_MIN'           :  250 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                             'AM_MIN'                   :  2186. * MeV,
                             'AM_MAX'                   :  2586. * MeV,
                             'AM_3'                     :  493.677 * MeV,
                             'AM_4'                     :  139.570 * MeV,
                             'AM_5'                     :  139.570 * MeV,
                             'ACHI2DOCA_MAX'            :  15.0,
                             'ASUMPT_MIN'               :  2000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'BPVDIRA_MIN'              : 0.9994,
                             'BPVLTIME_MIN'             : 0.1 * picosecond,
                             'PT_MIN'                   :  2500 * MeV,
                             'IPCHI2_MAX'               : 25.0
                                          }
                ,'LcpToPpKmPimPipPip' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'Trk_ALL_PT_MIN'           :  250 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                             'AM_MIN'                   :  2186. * MeV,
                             'AM_MAX'                   :  2586. * MeV,
                             'AM_3'                     :  139.570 * MeV,
                             'AM_4'                     :  139.570 * MeV,
                             'AM_5'                     :  139.570 * MeV,
                             'ACHI2DOCA_MAX'            :  15.0,
                             'ASUMPT_MIN'               :  2000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'BPVDIRA_MIN'              : 0.9994,
                             'BPVLTIME_MIN'             : 0.1 * picosecond,
                             'PT_MIN'                   :  2500 * MeV,
                             'IPCHI2_MAX'               : 25.0
                                          }
                ,'LcpToPpPimPimPipPip' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'Trk_ALL_PT_MIN'           :  250 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                             'AM_MIN'                   :  2186. * MeV,
                             'AM_MAX'                   :  2586. * MeV,
                             'AM_3'                     :  139.570 * MeV,
                             'AM_4'                     :  139.570 * MeV,
                             'AM_5'                     :  139.570 * MeV,
                             'ACHI2DOCA_MAX'            :  15.0,
                             'ASUMPT_MIN'               :  2000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'BPVDIRA_MIN'              : 0.9994,
                             'BPVLTIME_MIN'             : 0.1 * picosecond,
                             'PT_MIN'                   :  2500 * MeV,
                             'IPCHI2_MAX'               : 25.0
                                          }
                ,'LcpToPpKpPimPimPip' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'Trk_ALL_PT_MIN'           :  250 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                             'AM_MIN'                   :  2186. * MeV,
                             'AM_MAX'                   :  2586. * MeV,
                             'AM_3'                     :  139.570 * MeV,
                             'AM_4'                     :  139.570 * MeV,
                             'AM_5'                     :  139.570 * MeV,
                             'ACHI2DOCA_MAX'            :  15.0,
                             'ASUMPT_MIN'               :  2000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'BPVDIRA_MIN'              : 0.9994,
                             'BPVLTIME_MIN'             : 0.1 * picosecond,
                             'PT_MIN'                   :  2500 * MeV,
                             'IPCHI2_MAX'               : 25.0
                                          }
                ,'LcpToPpKpKpPimPim' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'Trk_ALL_PT_MIN'           :  250 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                             'AM_MIN'                   :  2186. * MeV,
                             'AM_MAX'                   :  2586. * MeV,
                             'AM_3'                     :  493.677 * MeV,
                             'AM_4'                     :  139.570 * MeV,
                             'AM_5'                     :  139.570 * MeV,
                             'ACHI2DOCA_MAX'            :  15.0,
                             'ASUMPT_MIN'               :  2000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'BPVDIRA_MIN'              : 0.9994,
                             'BPVLTIME_MIN'             : 0.1 * picosecond,
                             'PT_MIN'                   :  2500 * MeV,
                             'IPCHI2_MAX'               : 25.0
                                          }
                ,'LcpToPpKmKmPipPip' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'Trk_ALL_PT_MIN'           :  250 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                             'AM_MIN'                   :  2186. * MeV,
                             'AM_MAX'                   :  2586. * MeV,
                             'AM_3'                     :  493.677 * MeV,
                             'AM_4'                     :  139.570 * MeV,
                             'AM_5'                     :  139.570 * MeV,
                             'ACHI2DOCA_MAX'            :  15.0,
                             'ASUMPT_MIN'               :  2000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'BPVDIRA_MIN'              : 0.9994,
                             'BPVLTIME_MIN'             : 0.1 * picosecond,
                             'PT_MIN'                   :  2500 * MeV,
                             'IPCHI2_MAX'               : 25.0
                                          }

               ##  cuts dictionary entries for DplusFilter selections
                ,'Dplus':  {
                             'DMASS_MIN'              :  1800 * MeV,
                             'DMASS_MAX'              :  1940 * MeV,
                             'PT_MIN'                 :  2500 * MeV,
                             'D_BPVLTIME_MIN'         :  0.4 * picosecond,
                           }
                ,'Ds':     {
                             'DMASS_MIN'              :  1900 * MeV,
                             'DMASS_MAX'              :  2040 * MeV,
                             'PT_MIN'                 :  2500 * MeV,
                             'D_BPVLTIME_MIN'         :  0.3 * picosecond,
                           }
## "Lcp" should accept the full mass range for both Lambda_c and Xi_c^+
                ,'Lcp':     {
                             'DMASS_MIN'              :  2206 * MeV,
                             'DMASS_MAX'              :  2548 * MeV,
                             'PT_MIN'                 :  2500 * MeV,
                             'D_BPVLTIME_MIN'         :  0.1 * picosecond,
                           }



        }

    def locallines(self):
        ## DplusFilter makes additional selections based on mass, PT, and decay time
        from Stages import DplusFilter
        ## Dch2XXXX include both the D+ an Ds mass range
        from Stages import Dch2KmPimPipPipPip, Dch2KmKpPimPipPip, Dch2KmKmKpPipPip
        from Stages import LcpToPpKmKpPimPip, LcpToPpKmPimPipPip
        from Stages import LcpToPpPimPimPipPip, LcpToPpKpPimPimPip
        from Stages import LcpToPpKpKpPimPim, LcpToPpKmKmPipPip
        

        stages = { 'Dp2KmPimPipPipPipTurbo'   :  [DplusFilter('Dp2KmPimPipPipPip',
                                                  inputs=[Dch2KmPimPipPipPip],
                                                  nickname='Dplus')],
                   'Dp2KmKpPimPipPipTurbo'    :  [DplusFilter('Dp2KmKpPimPipPip',
                                                  inputs=[Dch2KmKpPimPipPip],
                                                  nickname='Dplus')],
                   'Dp2KmKmKpPipPipTurbo'     :  [DplusFilter('Dp2KmKmKpPipPip',
                                                  inputs=[Dch2KmKmKpPipPip],
                                                  nickname='Dplus')],
                   'Dsp2KmPimPipPipPipTurbo'  :  [DplusFilter('Dsp2KmPimPipPipPip',
                                                  inputs=[Dch2KmPimPipPipPip],
                                                  nickname='Ds')],
                   'Dsp2KmKpPimPipPipTurbo'   :  [DplusFilter('Dsp2KmKpPimPipPip',
                                                  inputs=[Dch2KmKpPimPipPip],
                                                  nickname='Ds')],
                   'Dsp2KmKmKpPipPipTurbo'    :  [DplusFilter('Dsp2KmKmKpPipPip',
                                                  inputs=[Dch2KmKmKpPipPip],
                                                  nickname='Ds')],
                   'LcpToPpKmKpPimPipTurbo'   :  [DplusFilter('LcpToPpKmKpPimPip',
                                                  inputs=[LcpToPpKmKpPimPip],
                                                  nickname='Lcp')],
                   'LcpToPpKmPimPipPipTurbo'  :  [DplusFilter('LcpToPpKmPimPipPip',
                                                  inputs=[LcpToPpKmPimPipPip],
                                                  nickname='Lcp')],
                   'LcpToPpPimPimPipPipTurbo' :  [DplusFilter('LcpToPpPimPimPipPip',
                                                  inputs=[LcpToPpPimPimPipPip],
                                                  nickname='Lcp')],
                   'LcpToPpKpPimPimPipTurbo'  :  [DplusFilter('LcpToPpKpPimPimPip',
                                                  inputs=[LcpToPpKpPimPimPip],
                                                  nickname='Lcp')],
                   'LcpToPpKpKpPimPimTurbo'   :  [DplusFilter('LcpToPpKpKpPimPim',
                                                  inputs=[LcpToPpKpKpPimPim],
                                                  nickname='Lcp')],
                   'LcpToPpKmKmPipPipTurbo'   :  [DplusFilter('LcpToPpKmKmPipPip',
                                                  inputs=[LcpToPpKmKmPipPip],
                                                  nickname='Lcp')],
                 }
        

        return stages
