from D2V0HLines import CharmHadD2V0HLines
from D2HHHLines import CharmHadD2HHHLines
from D2V0V0Lines import CharmHadD2V0V0Lines
from D02HHLines import CharmHadD02HHLines
from D2HHPi0Lines import CharmHadD2HHPi0Lines
from D02HHHHLines import CharmHadD02HHHHLines
from D2HHHKsLines import CharmHadD2HHHKsLines
from Hc2HHHHLines import CharmHadHc2HHHHLines
from XSecLines import CharmHadXSecLines 
from D2HHHPi0Lines import CharmHadD2HHHPi0Lines
from D2HHLines import CharmHadD2HHLines
from D2HHKsLines import CharmHadD2HHKshLines
from Dst2PiD02HHXBDTLines import CharmHadDst2PiD02HHXBDTLines
from D2EtaHLines import CharmHadD2EtaHLines
from ChargedHyperonLines import ChargedHyperonLines

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
D2HHHPi0Lines = CharmHadD2HHHPi0Lines()
D2HHLines = CharmHadD2HHLines()
D2HHKsLines = CharmHadD2HHKshLines()
Dst2PiD02HHXBDTLines = CharmHadDst2PiD02HHXBDTLines()
D2EtaHLines = CharmHadD2EtaHLines()
HyperonLines = ChargedHyperonLines()


theseslots = {   'Prescale' : {}, 
                 'TrackGEC' : {'NTRACK_MAX'           : 10000},
                 'Common' : {'TisTosSpec'               : "Hlt1Track.*Decision%TOS",
                             'Trk_ALL_TRCHI2DOF_MAX'    :  3.0,    
                             'Trk_ALL_P_MIN'            :  1000 * MeV,
                             'VCHI2PDOF_MAX'            :  10.0,
                            },  
                 # Particles for the "Detached" CPV lines
                 'CharmHadSharedDetachedDpmChild_K' : { 
                                 'PID_LIM'                  :  5,  
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'CharmHadSharedDetachedDpmChild_pi' : { 
                                 'PID_LIM'                  :  5, 
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'CharmHadSharedDetachedLcChild_K' : { 
                                 'PID_LIM'                  :  5,  
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'CharmHadSharedDetachedLcChild_pi' : { 
                                 'PID_LIM'                  :  5, 
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },  
                 'CharmHadSharedDetachedLcChild_p' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'CharmHadSharedDetachedD0ToHHChildPi' : {
                                 'Trk_ALL_PT_MIN'           :  800.0 * MeV
                                 , 'Trk_ALL_P_MIN'            :  5.0  * GeV
                                 , 'Trk_ALL_MIPCHI2DV_MIN'    :  2.0
                                              },
                 'CharmHadSharedDetachedD0ToHHChildK'  : {
                                   'PID_LIM'                  :  0.0
                                 , 'Trk_ALL_PT_MIN'           :  800.0 * MeV
                                 , 'Trk_ALL_P_MIN'            :  5.0  * GeV
                                 , 'Trk_ALL_MIPCHI2DV_MIN'    :  2.0
                                              },
                 'CharmHadSharedDetachedD0ToHHHHChild_pi' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  3.0,
                                              },
                 'CharmHadSharedDetachedD0ToHHHHChild_K' : {
                                 'PID_LIM'                  :  5,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  3.0,
                                              },
                 # Secondary Lambda's, used at least for building Xi- and Omega- candidates
                 'SharedSecondaryLambdaLL' : {
                                 'DecayTime_MIN'           : 4.5 * picosecond,
                                 'VZ_MIN'                  : -100. * mm,
                                 'VZ_MAX'                  :  500. * mm,
                                             },
                 # Secondary Lambda's, used at least for building Xi- and Omega- candidates
                 'SharedSecondaryLambdaDD' : {
                                 'DecayTime_MIN'           : 4.5 * picosecond,
                                 'VZ_MIN'                  : 300. * mm,
                                 'VZ_MAX'                  : 2275. * mm,
                                              },
                 # long Kshorts, at least for  D0 --> HHKs
                 'CharmHadSharedKsLL' : {
                                 'DecayTime_MIN'           : 0.5 * picosecond,
                                 'VZ_MIN'                  : -100. * mm,
                                 'VZ_MAX'                  : 500. * mm,
                                             },
                 # downstream Kshorts, at least for D0 --> HHKs
                 'CharmHadSharedKsDD' : {
                                 'DecayTime_MIN'           : 0.5 * picosecond,
                                 'VZ_MIN'                  : 300. * mm,
                                 'VZ_MAX'                  : 2275. * mm,
                                             },
                 # generic D0 --> HHKs cuts
                 'D02HHKsh' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
##                           'TisTosSpec'               : "Hlt1TwoTrackMVADecision%TOS",
                             'AM_MIN'                   :  1740. * MeV,
                             'AM_MAX'                   :  1990. * MeV,
                             'ASUMPT_MIN'               :  1500 * MeV,
                             'D_PT_MIN'                 :  1800 * MeV,
                             'D_BPVDIRA_MIN'            :  0.9994,
                             'D_BPVVDCHI2_MIN'          :  20.0,
                             'D_VCHI2PDOF_MAX'          :  20.0,
                             'D_BPVLTIME_MIN'           :  0.1 * picosecond,
                             'HHMaxDOCA'                :  0.200 * mm,
                             'HHMass_MAX'               :  1500. * MeV,
                                          },
                 # Particles for the 'Prompt' CF lifetime unbiased lines for the 
                 # lifetime measurements
                 'CharmHadSharedPromptChild_K' : {
                                 'PID_LIM'                  :  10,
                                 'Trk_ALL_PT_MIN'           :  500.0 * MeV,
                                              },
                 'CharmHadSharedPromptChild_pi' : {
                                 'PID_LIM'                  :  0,
                                 'Trk_ALL_PT_MIN'           :  500.0 * MeV,
                                              },
                 'CharmHadSharedPromptChild_p' : {
                                 'PID_LIM'                  :  10,
                                 'Trk_ALL_PT_MIN'           :  500.0 * MeV,
                                              },
                 # Soft particles for tagging decays
                 'CharmHadSharedSoftTagChild_pi' : {
                                 'Trk_ALL_PT_MIN'           :  100.0,
                                           },
                 # NoPID protons for the Sigma_c tagged Lambda_c line for PID calibration
                 # the IPCHI2 cut is tight because the PT/PT cuts have to be loose to give
                 # a useful sample, and we need to reduce the combinatorics
                 'CharmHadSharedNoPIDDetachedChild_p' : {
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                          },
                 # NoPID particles for other charm lines 
                 'CharmHadSharedNoPIDDetachedChild_pi' : {
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                          },
                 'CharmHadSharedNoPIDDetachedChild_K' : {
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                          },
                 # Neutral particles for D -> HHX lines
                 # 
                 'CharmHadSharedNeutralChild_pi0' : {
                                 'Neut_ALL_PT_MIN'           : 500.0 * MeV,
                                 'Neut_ALL_ADMASS_MAX'       : 60.0,
                                          },
                 'CharmHadSharedNeutralChild_pi0R' : {
                                 'Neut_ALL_PT_MIN'           : 500.0 * MeV,
                                 'Neut_ALL_ADMASS_MAX'       :  30.0, 
                                          },
                 'CharmHadSharedNeutralChild_pi0M' : {
                                 'Neut_ALL_PT_MIN'           : 500.0 * MeV,
                                 'Neut_ALL_ADMASS_MAX'       : 60.0,
                                          },
                 'CharmHadSharedNeutralChild_gamma' : {
                                 'Neut_ALL_PT_MIN'           : 500.0 * MeV,
                                          },
                 #
                 'CharmHadSharedNeutralLowPtChild_gamma' : {
                                 'Neut_ALL_PT_MIN'           : 350.0 * MeV,
                                          },
                 'CharmHadSharedNeutralLowPtChild_pi0' : {
                                 'Neut_ALL_PT_MIN'           : 350.0 * MeV,
                                 'Neut_ALL_ADMASS_MAX'       :  60.0,
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
theseslots.update(D2HHHPi0Lines.localcuts())
theseslots.update(D2HHLines.localcuts())
theseslots.update(D2HHKsLines.localcuts())
theseslots.update(Dst2PiD02HHXBDTLines.slots())
theseslots.update(D2EtaHLines.localcuts())
theseslots.update(HyperonLines.localcuts())

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
            self.__lines__.update(D2HHHPi0Lines.locallines())
            self.__lines__.update(D2HHLines.locallines())
            self.__lines__.update(D2HHKsLines.locallines())
            self.__lines__.update(Dst2PiD02HHXBDTLines.stages())
            self.__lines__.update(D2EtaHLines.locallines())
            self.__lines__.update(HyperonLines.locallines())
            
        return self.__lines__[nickname] if nickname else self.__lines__
            
    def __apply_configuration__(self):
        
        _stages = self.stages()
        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms( _stages ).iteritems():
            if nickname.find('Turbo') > -1 : 
                Hlt2Line(nickname, prescale = self.prescale,
                         algos = algos, postscale = self.postscale, Turbo = True)
            else :
                Hlt2Line(nickname, prescale = self.prescale,
                         algos = algos, postscale = self.postscale)
