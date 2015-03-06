from D2V0HLines import CharmHadD2V0HLines
from D2HHHLines import CharmHadD2HHHLines
from D2V0V0Lines import CharmHadD2V0V0Lines
from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

D2HHHLines = CharmHadD2HHHLines()
D2V0HLines = CharmHadD2V0HLines()
D2V0V0Lines = CharmHadD2V0V0Lines()

theseslots = {   'Prescale' : {}, 
                 'TrackGEC' : {'NTRACK_MAX'           : 10000},
                 'Common' : {'TisTosSpec'               : "Hlt1Track.*Decision%TOS",
                             'Trk_ALL_TRCHI2DOF_MAX'    :  3.0,    
                             'Trk_ALL_P_MIN'            :  1000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                            },  
                 # Particles for the "Detached" CPV lines
                 'SharedDetachedDpmChild_K' : { 
                                 'Kaon_ALL_PIDK_MIN'        :  0,  
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'SharedDetachedDpmChild_pi' : { 
                                 'Pion_ALL_PIDK_MAX'        :  10, 
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'SharedDetachedLcChild_K' : { 
                                 'Kaon_ALL_PIDK_MIN'        :  0,  
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  2.0,
                                              },  
                 'SharedDetachedLcChild_pi' : { 
                                 'Pion_ALL_PIDK_MAX'        :  10, 
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  2.0,
                                              },  
                 'SharedDetachedLcChild_p' : {
                                 'Proton_ALL_PIDp_MIN'      :  0,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  2.0,
                                              },
                 # Particles for the 'Prompt' CF lifetime unbiased lines for the 
                 # lifetime measurements
                 'SharedPromptChild_K' : {
                                 'Kaon_ALL_PIDK_MIN'        :  5,
                                 'Trk_ALL_PT_MIN'           :  500.0,
                                              },
                 'SharedPromptChild_pi' : {
                                 'Pion_ALL_PIDK_MAX'        :  0,
                                 'Trk_ALL_PT_MIN'           :  500.0,
                                              },
                 'SharedPromptChild_p' : {
                                 'Proton_ALL_PIDp_MIN'      :  5,
                                 'Trk_ALL_PT_MIN'           :  500.0,
                                              },
                 # Soft particles for tagging decays
                 'SharedSoftTagChild_pi' : {
                                 'Trk_ALL_PT_MIN'           :  100.0,
                                           },
                 # NoPID protons for the Sigma_c tagged Lambda_c line for PID calibration
                 # the IPCHI2 cut is tight because the PT/PT cuts have to be loose to give
                 # a useful sample, and we need to reduce the combinatorics
                 'SharedNoPIDLcChild_p' : {
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  9.0,
                                          },
                }

theseslots.update(D2HHHLines.localcuts())
theseslots.update(D2V0HLines.localcuts())
theseslots.update(D2V0V0Lines.localcuts())

class CharmHadLines(Hlt2LinesConfigurableUser):
    __slots__ = theseslots
    __lines__ = {}

    def __apply_configuration__(self):
        self.__lines__.update(D2HHHLines.locallines())
        self.__lines__.update(D2V0HLines.locallines())
        self.__lines__.update(D2V0V0Lines.locallines())

        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(self.__lines__).iteritems():
            Hlt2Line(nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
