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



        }

    def locallines(self):
        ## DplusFilter makes additional selections based on mass, PT, and decay time
        from Stages import DplusFilter
        ## Dch2XXXX include both the D+ an Ds mass range
        from Stages import Dch2KmPimPipPipPip, Dch2KmKpPimPipPip, Dch2KmKmKpPipPip
        

        stages = { 'Dp2KmPimPipPipPipTurbo'   :  [DplusFilter('Dp2KmPimPipPipPip',
                                                  inputs=[Dch2KmPimPipPipPip],
                                                  nickname='Dplus')],
                   'Dp2KmKpPimPipPipTurbo'   :  [DplusFilter('Dp2KmKpPimPipPip',
                                                  inputs=[Dch2KmKpPimPipPip],
                                                  nickname='Dplus')],
                   'Dp2KmKmKpPipPipTurbo'   :  [DplusFilter('Dp2KmKmKpPipPip',
                                                  inputs=[Dch2KmKmKpPipPip],
                                                  nickname='Dplus')],
                   'Dsp2KmPimPipPipPipTurbo'   :  [DplusFilter('Dsp2KmPimPipPipPip',
                                                  inputs=[Dch2KmPimPipPipPip],
                                                  nickname='Ds')],
                   'Dsp2KmKpPimPipPipTurbo'   :  [DplusFilter('Dsp2KmKpPimPipPip',
                                                  inputs=[Dch2KmKpPimPipPip],
                                                  nickname='Ds')],
                   'Dsp2KmKmKpPipPipTurbo'   :  [DplusFilter('Dsp2KmKmKpPipPip',
                                                  inputs=[Dch2KmKmKpPipPip],
                                                  nickname='Ds')]
                 }
        

        return stages
