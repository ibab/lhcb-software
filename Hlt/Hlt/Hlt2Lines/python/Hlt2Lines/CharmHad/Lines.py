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
from CharmSpectroscopyLines import CharmSpectroscopyLines
from ChargedHyperonLines import ChargedHyperonLines
from D2HHHHHLines import CharmHadD2HHHHHLines
from XiccLines import CharmHadXiccLines


from Configurables import HltANNSvc
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
CharmSpectroscopyLines = CharmSpectroscopyLines()
HyperonLines = ChargedHyperonLines()
D2HHHHHLines = CharmHadD2HHHHHLines()
XiccLines = CharmHadXiccLines()



theseslots = {   'Postscale' : {'Hlt2CharmHadD02KmKpTurbo'               : 0.1,
                                'Hlt2CharmHadD02KmPipTurbo'              : 0.1,
                                'Hlt2CharmHadD02PimPipTurbo'             : 0.1,
                                'Hlt2CharmHadDpToKmPipPip_LTUNBTurbo'    : 0.05,
                                'Hlt2CharmHadDspToKmKpPip_LTUNBTurbo'    : 0.05,
                                'Hlt2CharmHadLcpToPpKmPip_LTUNBTurbo'    : 0.05,
                                'Hlt2CharmHadXic0ToPpKmKmPip_LTUNBTurbo' : 0.05,
                                'Hlt2CharmHadDpToKmPipPip_ForKPiAsym'    : 0.25  }, 
                 'TrackGEC' : { 'NTRACK_MAX'           : 10000},
                 'Common' : {'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS",
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
                 'CharmHadSharedDetachedDpmChild_TightK' : {
                                 'PID_LIM'                  :  7,
                                 'Trk_ALL_PT_MIN'           :  250 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'CharmHadSharedDetachedDpmChild_Tightpi' : {
                                 'PID_LIM'                  :  3,
                                 'Trk_ALL_PT_MIN'           :  250 * MeV,
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
                                   'PID_LIM'                  :  5.0
                                 , 'Trk_ALL_PT_MIN'           :  800.0 * MeV
                                 , 'Trk_ALL_P_MIN'            :  5.0  * GeV
                                 , 'Trk_ALL_MIPCHI2DV_MIN'    :  2.0
                                              },
                 'CharmHadSharedDetachedD0ToHHChildK'  : {
                                   'PID_LIM'                  :  5.0
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
                 # Use tighter PID and (and momentum) cuts for protons which
                 # are charmed baryon daughters
                 'TighterProtons' :  {
                                  'PIDp_MIN'              :  5.0,
                                  'DeltaPID_MIN'         :  5.0,
                                  'P_MIN'                 :  10000 * MeV
                                  },
                 'CharmHadTighterSharedPromptChild_p' :  {
                                  'PIDp_MIN'              :  5.0,
                                  'Delta_PID_MIN'         :  5.0,
                                  'P_MIN'                 :  10000 * MeV
                                  },
                 'CharmHadTighterSharedLcChild_p' :  {
                                  'PIDp_MIN'              :  5.0,
                                  'Delta_PID_MIN'         :  5.0,
                                  'P_MIN'                 :  10000 * MeV
                                  },
                 # Secondary Lambda's, used at least for building Xi- and Omega- candidates
                 'CharmHadSharedSecondaryLambdaLL' : {
                                 'DecayTime_MIN'           : 4.5 * picosecond,
                                 'VZ_MIN'                  : -100. * mm,
                                 'VZ_MAX'                  :  500. * mm,
                                             },
                 # Secondary Lambda's, used at least for building Xi- and Omega- candidates
                 'CharmHadSharedSecondaryLambdaDD' : {
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
                                 'Trk_ALL_PT_MIN'           :  100.0 * MeV,
                                           },
                 'CharmHadSharedSoftTagChild_mu' : { 
                                 'Trk_ALL_PT_MIN'           :  500.0 * MeV,
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
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  3.0,
                                          },
                 'CharmHadSharedNoPIDDetachedChild_K' : {
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  3.0,
                                          },
                 # Neutral particles for D -> HHX lines
                 # 
                 'CharmHadSharedNeutralChild_pi0' : {
                                 'Neut_ALL_PT_MIN'           : 500.0 * MeV,
                                 'Neut_ALL_ADMASS_MAX'       : 60.0,
                                          },
                 'CharmHadSharedNeutralChild_pi0R' : {
                                 'Neut_ALL_PT_MIN'           : 500.0 * MeV,
                                 'Neut_ALL_ADMASS_MAX'       : 60.0, 
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
                 'CharmHadSharedNeutralLowPtChild_pi0R' : {
                                 'Neut_ALL_PT_MIN'           : 350.0 * MeV,
                                 'Neut_ALL_ADMASS_MAX'       :  60.0,
                                          },
                 'CharmHadSharedNeutralLowPtChild_pi0M' : {
                                 'Neut_ALL_PT_MIN'           : 350.0 * MeV,
                                 'Neut_ALL_ADMASS_MAX'       :  60.0,
                                          },
                 'CharmHadSharedNeutralLowPtChild_eta' : {
                                 'Neut_ALL_PT_MIN'           : 200.0 * MeV,
                                 'Neut_ALL_ADMASS_MAX'       :  105.0,
                                          },

                ## Configuration for two-body seeds shared by inclusive lines
                'InclHc2HHX'         : {
                                  'Trk_TRCHI2DOF_MAX'         : 2.0
                                , 'Trk_PT_MIN'                : 200.0 * MeV
                                , 'Trk_MIPCHI2DV_MIN'         : 7.0
                                , 'D0_VCHI2PDOF_MAX'          : 50
                                , 'D0_BPVVDCHI2_MIN'          : 20
                                , 'D0_BPVCORRM_MAX'           : 2400.0 * MeV
                                , 'D0_comb_PT_MIN'             : 1900 * MeV
                                , 'TisTosSpec': { "Hlt1Track.*Decision%TOS":0 }
                },
                ## Configuration for Hc + pion combinatorics shared by lines
                'InclHcst2PiHc2HHX'  : {
                                  'Spi_TRCHI2DOF_MAX'         : 3.0       
                                , 'Spi_PT_MIN'                : 150.0 * MeV   
                                , 'Dst_VCHI2PDOF_MAX'         : 50.0
                                , 'Dst_PT_MIN'                : 2.0 * GeV
                                , 'Dst_M_MAX'                 : 2700.0 * MeV
                                , 'Dst_D0_DeltaM_MAX'         : 500.0 * MeV
                },
                ## Configuration for D*+ specific filtering
                'InclDst2PiD02HHX'   : {
                                  'D0_BPVVDCHI2_MIN'          : 20
                                , 'D0_BPVCORRM_MAX'           : 2100.0 * MeV
                                , 'Dst_M_MAX'                 : 2300.0 * MeV
                                , 'Dst_D0_DeltaM_MAX'         : 350.0 * MeV
                                , 'BDT_Lookup_Filename'       : "Hlt2Dst2PiD02HHX_BDTParams_v1r0.txt"
                                , 'BDT_Threshold'             : 1.33

                                , 'BDT_Lookup_VarMap' : {
                                        "D0FD"          : "CHILDFUN(BPVVD, 1)"
                                        , "SLPCOSTHETA" : "LV02"
                                        , "TRSUMPT"     : "CHILDFUN(CHILDFUN(PT, 1) + CHILDFUN(PT, 2), 1)"
                                        , "DSTFD"       : "BPVVD"
                                        , "SLPPT"       : "CHILDFUN(PT, 2)"
                                        , "D0CHI2"      : "CHILDFUN(VFASPF(VCHI2), 1)"
                                        , "DSTDOCA"     : "DOCAMAX"
                              }
                },
                ## Configuration for Sigc specific filtering
                'InclSigc2PiLc2HHX'  : {
                                  'D0_BPVVDCHI2_MIN'          : 20
                                , 'D0_BPVCORRM_MAX'           : 2400.0 * MeV
                                , 'Dst_M_MAX'                 : 2700.0 * MeV
                                , 'Dst_D0_DeltaM_MAX'         : 500.0 * MeV
                                , 'BDT_Lookup_Filename'       : "Hlt2Sigc2PiLc2HHX_BDTParams_v1r0.txt"
                                , 'BDT_Threshold'             : 1.345

                                , 'BDT_Lookup_VarMap' : {
                                        "D0FD"          : "CHILDFUN(BPVVD, 1)"
                                        , "SLPCOSTHETA" : "LV02"
                                        , "TRSUMPT"     : "CHILDFUN(CHILDFUN(PT, 1) + CHILDFUN(PT, 2), 1)"
                                        , "DSTFD"       : "BPVVD"
                                        , "SLPPT"       : "CHILDFUN(PT, 2)"
                                        , "D0CHI2"      : "CHILDFUN(VFASPF(VCHI2), 1)"
                                        , "DSTDOCA"     : "DOCAMAX"
                              }
                  }
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
theseslots.update(CharmSpectroscopyLines.localcuts())
theseslots.update(HyperonLines.localcuts())
theseslots.update(D2HHHHHLines.localcuts())
theseslots.update(XiccLines.slots())

class CharmHadLines(Hlt2LinesConfigurableUser):
    from copy import deepcopy
    __slots__ = deepcopy(theseslots)
    __lines__ = {}


    ## To avoid problems with propogating configuration changes for private
    ## tools back to CharmHadDst2PiD02HHXBDTLines, the line definitions for
    ## the inclusive lines are made here instead of in a sub-file.
    def __inclHHXStages(self) : # {
        from Stages import InclHcst2PiHc2HHX
        from Stages import InclHcst2PiHc2HHXFilter

        ## Final kinematic filtering
        InclDst2PiD02HHX = InclHcst2PiHc2HHXFilter( 'Dst2PiD0'
                , inputs = [ InclHcst2PiHc2HHX ]
                , nickname = 'InclDst2PiD02HHX' )

        InclSigc2PiLc2HHX = InclHcst2PiHc2HHXFilter( 'Sigc2PiLc'
                , inputs = [ InclHcst2PiHc2HHX ]
                , nickname = 'InclSigc2PiLc2HHX' )


        from Stages import BDTFilter
        Dstp2D0PiInclFilt = BDTFilter( 'FiltBDT', [ InclDst2PiD02HHX ], self.getProps()['InclDst2PiD02HHX'] )
        Sigc2LcPiInclFilt = BDTFilter( 'FiltBDT', [ InclSigc2PiLc2HHX ], self.getProps()['InclSigc2PiLc2HHX'] )

        inclStages = {   'InclDst2PiD02HHXBDT'   :  [ Dstp2D0PiInclFilt ]
                       , 'InclSigc2PiLc2HHXBDT'  :  [ Sigc2LcPiInclFilt ]  }

        return inclStages
    # }


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
            self.__lines__.update(CharmSpectroscopyLines.locallines())
            self.__lines__.update(HyperonLines.locallines())
            self.__lines__.update(D2HHHHHLines.locallines())
            self.__lines__.update(XiccLines.stages())
            
        return self.__lines__[nickname] if nickname else self.__lines__
            
    def __apply_configuration__(self):
        
        _stages = self.stages()

        ## Other line Hlt2LinesConfigurableUser, like that for the DPS lines,
        ## can call self.stages().  This can (but probably won't) happen
        ## before __slots__ is updated with the configuration.  In order to
        ## guarantee that the __slots__ are updated prior to the definition of
        ## the inclusive lines, either the dependence of this configurable and
        ## all others that call its stages() must be explicitly defined, or,
        ## as is done here, the inclusive lines are defined first and only
        ## here in __apply_configuration__ and not in stages().
        _stages.update(self.__inclHHXStages())


        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms( _stages ).iteritems():
            doturbo = True if (nickname.find('Turbo') > -1) else False
            Hlt2Line('CharmHad' + nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale, Turbo = doturbo)
            if "Dst_2D0Pi0_D02KPi_XSec" in nickname:
                HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadDst_2D0Pi0_D02KPi_XSecTurboDecision":  57222 } )




