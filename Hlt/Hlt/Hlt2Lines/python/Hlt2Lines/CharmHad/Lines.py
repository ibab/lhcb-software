from D2V0HLines import CharmHadD2V0HLines
from D2HHHLines import CharmHadD2HHHLines
from D2V0V0Lines import CharmHadD2V0V0Lines
from D02HHLines import CharmHadD02HHLines
from D2HHPi0Lines import CharmHadD2HHPi0Lines
from D02HHHHLines import CharmHadD02HHHHLines
from D2HHHKsLines import CharmHadD2HHHKsLines
from Hc2HHHHLines import CharmHadHc2HHHHLines
from XSecLines import CharmHadXSecLines 

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

D2HHHLines = CharmHadD2HHHLines()
D2V0HLines = CharmHadD2V0HLines()
D2V0V0Lines = CharmHadD2V0V0Lines()
D02HHLines = CharmHadD02HHLines()
D2HHPi0Lines = CharmHadD2HHPi0Lines()
D02HHHHLines = CharmHadD02HHHHLines()
D2HHHKsLines = CharmHadD2HHHKsLines()
Hc2HHHHLines = CharmHadHc2HHHHLines()
XSecLines = CharmHadXSecLines()

theseslots = {   'Prescale' : {}, 
                 'TrackGEC' : {'NTRACK_MAX'           : 10000},
                 'Common' : {'TisTosSpec'               : "Hlt1Track.*Decision%TOS",
                             'Trk_ALL_TRCHI2DOF_MAX'    :  3.0,    
                             'Trk_ALL_P_MIN'            :  1000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                            },  
                 # Particles for the "Detached" CPV lines
                 'SharedDetachedDpmChild_K' : { 
                                 'PID_LIM'                  :  2,  
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'SharedDetachedDpmChild_pi' : { 
                                 'PID_LIM'                  :  5, 
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'SharedDetachedLcChild_K' : { 
                                 'PID_LIM'                  :  2,  
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'SharedDetachedLcChild_pi' : { 
                                 'PID_LIM'                  :  5, 
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'SharedDetachedLcChild_p' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 # Particles for the 'Prompt' CF lifetime unbiased lines for the 
                 # lifetime measurements
                 'SharedPromptChild_K' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  500.0 * MeV,
                                              },
                 'SharedPromptChild_pi' : {
                                 'PID_LIM'                  :  0,
                                 'Trk_ALL_PT_MIN'           :  500.0 * MeV,
                                              },
                 'SharedPromptChild_p' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  500.0 * MeV,
                                              },
                 # Soft particles for tagging decays
                 'SharedSoftTagChild_pi' : {
                                 'Trk_ALL_PT_MIN'           :  100.0,
                                           },
                 # NoPID protons for the Sigma_c tagged Lambda_c line for PID calibration
                 # the IPCHI2 cut is tight because the PT/PT cuts have to be loose to give
                 # a useful sample, and we need to reduce the combinatorics
                 'SharedNoPIDDetachedChild_p' : {
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                          },
                 # NoPID particles for other charm lines 
                 'SharedNoPIDDetachedChild_pi' : {
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                          },
                 'SharedNoPIDDetachedChild_K' : {
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                          },
                 # Neutral particles for D -> HHX lines
                 # 
                 # MAYBE BETTER TO RENAME Trk_ALL_PT_MIN HERE AS THIS IS
                 # NOT A TRACK BUT A PI0?
                 'SharedNeutralChild_pi0R' : {
                                 'Trk_ALL_PT_MIN'           : 500.0 * MeV,
                                 'Pi0_ALL_DMASS_MAX'        : 30.0,
                                          },
                }

theseslots.update(D2HHHLines.localcuts())
theseslots.update(D2V0HLines.localcuts())
theseslots.update(D2V0V0Lines.localcuts())
theseslots.update(D02HHLines.slots())
theseslots.update(D2HHPi0Lines.localcuts())
theseslots.update(D02HHHHLines.localcuts())
theseslots.update(D2HHHKsLines.localcuts())
theseslots.update(Hc2HHHHLines.localcuts())
theseslots.update(XSecLines.localcuts())

class CharmHadLines(Hlt2LinesConfigurableUser):
    __slots__ = theseslots
    __lines__ = {}

    def stages ( self , nickname = '' ) :

        if not self.__lines__ :
            self.__lines__.update(D2HHHLines.locallines())
            self.__lines__.update(D2V0HLines.locallines())
            self.__lines__.update(D2V0V0Lines.locallines())
            self.__lines__.update(D02HHLines.stages())
            self.__lines__.update(D2HHPi0Lines.locallines())
            self.__lines__.update(D02HHHHLines.locallines())
            self.__lines__.update(D2HHHKsLines.locallines())
            self.__lines__.update(Hc2HHHHLines.locallines())
            self.__lines__.update(XSecLines.locallines())
    
        return self.__lines__[nickname] if nickname else self.__lines__
            
    def __apply_configuration__(self):
        
        _stages = self.stages()
        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms( _stages ).iteritems():
            Hlt2Line(nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
