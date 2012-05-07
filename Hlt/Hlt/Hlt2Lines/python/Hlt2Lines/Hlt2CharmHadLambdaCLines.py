from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, GeV, mm, picosecond


class Hlt2CharmHadLambdaCLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'Lc_AM_MIN'                  : 2136 * MeV
                   , 'Lc_AM_MAX'                : 2436 * MeV
                   , 'Trk_TRCHI2DOF_MAX'        :    3.0
                   , 'Trk_PT_MIN'               :  500 * MeV
                   , 'Trk_MIPCHI2DV_MIN'        :    9.0
                   , 'Trk_1of3_MIPCHI2DV_MIN'   :   15.0
                   , 'Lc_ASUMPT_MIN'            : 2500 * MeV
                   , 'Lc_BPVDIRA_MIN'           :    0.99985
                   , 'Lc_BPVVDCHI2_MIN'         :   49.0
                   , 'Lc_BPVVDR_MAX'            :    4 * mm
                   , 'Lc_VCHI2PDOF_MAX'         :   15.0
                   , 'Lc_BPVLTIME_MAX'          :   20 * picosecond
                   , 'P_PIDp_MIN'               :    0.0
                   #, 'P_PIDpPIDK_MIN'           :    0.0
                   , 'P_PT_MIN'                 : 1500 * MeV
                   , 'P_P_MIN'                  :   10 * GeV
                   , 'SigMass_M_MIN'            : 2211 * MeV
                   , 'SigMass_M_MAX'            : 2361 * MeV
                   ## GEC
                   , 'GEC_Filter_NTRACK'        : True       # do or do not
                   , 'GEC_NTRACK_MAX'           : 180        # max number of tracks
                   , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }   
                   , 'name_prefix'              : 'CharmHadLambdaC'
                   , 'Prescale'         : { } 
                   , 'Postscale'        : { 'Hlt2CharmHadLambdaC2KPPiWideMass'    : 0.10
                                            , 'Hlt2CharmHadLambdaC2KPKWideMass'   : 0.10
                                            , 'Hlt2CharmHadLambdaC2PiPPiWideMass' : 0.10
                                            , 'Hlt2CharmHadLambdaC2PiPKWideMass'  : 0.10
                   }
                   , 'HltANNSvcID'      : {
                                        'Hlt2CharmHadLambdaC2KPPiDecision'    : 55666
                                        , 'Hlt2CharmHadLambdaC2KPKDecision'   : 55667
                                        , 'Hlt2CharmHadLambdaC2PiPPiDecision' : 55668
                                        , 'Hlt2CharmHadLambdaC2PiPKDecision'  : 55669
                                        , 'Hlt2CharmHadLambdaC2KPPiWideMassDecision'  : 55662
                                        , 'Hlt2CharmHadLambdaC2KPKWideMassDecision'   : 55663
                                        , 'Hlt2CharmHadLambdaC2PiPPiWideMassDecision' : 55664
                                        , 'Hlt2CharmHadLambdaC2PiPKWideMassDecision'  : 55665
                   }
                }
    


    def __apply_configuration__(self) : # {
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions,BiKalmanFittedKaons,BiKalmanFittedProtons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichLowPTProtons
        from Configurables import TisTosParticleTagger      
 
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        from HltLine.HltLine import bindMembers
        from HltTracking.HltPVs import PV3D


        ## GEC cut on number of tracks in the event.
        ## ####################################################################
        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] : 
            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('GEC_Filter_NTRACK') : # { 
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()
        # } 

        filtName = 'Hlt2' + self.getProp('name_prefix')  + 'KillTooManyInTrkAlg'
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter( filtName
                                                 , Code = filtCode
                                                )   
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )


        ## Candidate selection without proton PID.
        ## ####################################################################
        _daughters_cut = "(TRCHI2DOF < %(Trk_TRCHI2DOF_MAX)s )" \
                         "& (PT > %(Trk_PT_MIN)s)" \
                         "& (MIPCHI2DV(PRIMARY) > %(Trk_MIPCHI2DV_MIN)s)" % self.getProps()

        _proton_cut = "(TRCHI2DOF < %(Trk_TRCHI2DOF_MAX)s )" \
                      "& (PT > %(P_PT_MIN)s)" \
                      "& (P > %(P_P_MIN)s)" \
                      "& (MIPCHI2DV(PRIMARY) > %(Trk_MIPCHI2DV_MIN)s)" % self.getProps()

        _combination_cut = "(in_range( %(Lc_AM_MIN)s, AM, %(Lc_AM_MAX)s ))" \
                           "& ((APT1+APT2+APT3) > %(Lc_ASUMPT_MIN)s )" \
                           "& (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1of3_MIPCHI2DV_MIN)s))" % self.getProps()

        _mother_cut = "(VFASPF(VCHI2PDOF) < %(Lc_VCHI2PDOF_MAX)s)" \
                      "& (BPVDIRA > %(Lc_BPVDIRA_MIN)s )" \
                      "& (BPVVDR < %(Lc_BPVVDR_MAX)s)" \
                      "& (BPVVDCHI2 > %(Lc_BPVVDCHI2_MIN)s )" \
                      "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') < %(Lc_BPVLTIME_MAX)s )" % self.getProps()


        Hlt2CharmHadLambdaC2KPPi = Hlt2Member( CombineParticles 
                               , "Combine"     
                               , DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc"
                               , DaughtersCuts = { 'pi+' : _daughters_cut, 'K+' : _daughters_cut, 'p+' : _proton_cut }
                               , CombinationCut = _combination_cut
                               , MotherCut = _mother_cut
                               , Inputs = [ BiKalmanFittedPions.outputSelection(), 
                                            BiKalmanFittedKaons.outputSelection(),
                                            BiKalmanFittedProtons.outputSelection() ])

        ## Other decay modes.
        Hlt2CharmHadLambdaC2KPK = Hlt2CharmHadLambdaC2KPPi.clone( "Combine",
                                        DecayDescriptor = "[Lambda_c+ -> K- p+ K+]cc",
                                        Inputs = [ BiKalmanFittedKaons.outputSelection(),
                                                   BiKalmanFittedProtons.outputSelection() ] )
        Hlt2CharmHadLambdaC2PiPPi = Hlt2CharmHadLambdaC2KPPi.clone( "Combine",
                                        DecayDescriptor = "[Lambda_c+ -> pi- p+ pi+]cc",
                                         Inputs = [ BiKalmanFittedPions.outputSelection(), 
                                                    BiKalmanFittedProtons.outputSelection() ])
        Hlt2CharmHadLambdaC2PiPK = Hlt2CharmHadLambdaC2KPPi.clone( "Combine",
                                        DecayDescriptor = "[Lambda_c+ -> pi- p+ K+]cc" )


        ## Candidate selection with proton PID.
        ## Only proton selection criteria change.
        ## Proton PIDK not working yet.
        ## ####################################################################
        #_proton_cut_rich = _proton_cut + "& (PIDp > %(P_PIDp_MIN)s)" \
        #                   "& ((PIDp - PIDK)> %(P_PIDpPIDK_MIN)s)" % self.getProps()
        _proton_cut_rich = _proton_cut + "& (PIDp > %(P_PIDp_MIN)s)" % self.getProps()

        Hlt2CharmHadLambdaC2KPPi_Rich = Hlt2CharmHadLambdaC2KPPi.clone( "CombineRich"
                , DaughtersCuts = { 'pi+' : _daughters_cut,
                                    'K+' : _daughters_cut,
                                    'p+' : _proton_cut_rich }
                , Inputs = [ BiKalmanFittedPions.outputSelection(), 
                             BiKalmanFittedKaons.outputSelection(),
                             BiKalmanFittedRichLowPTProtons.outputSelection() ]
        )

        ## Other decay modes.
        Hlt2CharmHadLambdaC2KPK_Rich = Hlt2CharmHadLambdaC2KPPi_Rich.clone( "CombineRich",
                                        DecayDescriptor = "[Lambda_c+ -> K- p+ K+]cc",
                                        Inputs = [ BiKalmanFittedKaons.outputSelection(),
                                                   BiKalmanFittedRichLowPTProtons.outputSelection() ] )
        Hlt2CharmHadLambdaC2PiPPi_Rich = Hlt2CharmHadLambdaC2KPPi_Rich.clone( "CombineRich",
                                        DecayDescriptor = "[Lambda_c+ -> pi- p+ pi+]cc",
                                        Inputs = [ BiKalmanFittedPions.outputSelection(), 
                                                   BiKalmanFittedRichLowPTProtons.outputSelection() ] )
        Hlt2CharmHadLambdaC2PiPK_Rich = Hlt2CharmHadLambdaC2KPPi_Rich.clone( "CombineRich",
                                        DecayDescriptor = "[Lambda_c+ -> pi- p+ K+]cc" )


        ## TOS to the Hlt1Track lines.
        ## ####################################################################
        filterTOS_LC2KPPi = Hlt2Member( TisTosParticleTagger
                                , 'Hlt1TOSFilter'
                                , Inputs = [ Hlt2CharmHadLambdaC2KPPi_Rich ]
                                , TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
                              )   

        ## Other decay modes.
        filterTOS_LC2KPK = filterTOS_LC2KPPi.clone( 'Hlt1TOSFilter', Inputs = [ Hlt2CharmHadLambdaC2KPK_Rich ] )
        filterTOS_LC2PiPPi = filterTOS_LC2KPPi.clone( 'Hlt1TOSFilter', Inputs = [ Hlt2CharmHadLambdaC2PiPPi_Rich ] )
        filterTOS_LC2PiPK = filterTOS_LC2KPPi.clone( 'Hlt1TOSFilter', Inputs = [ Hlt2CharmHadLambdaC2PiPK_Rich ] )


        ## Create bindMembers to prevent dupclication of algorithms
        ## for the two line windows.
        ## ####################################################################
        modeNameKPPi = self.getProp('name_prefix') + '2KPPi'
        modeNameKPK = self.getProp('name_prefix') + '2KPK'
        modeNamePiPPi = self.getProp('name_prefix') + '2PiPPi'
        modeNamePiPK = self.getProp('name_prefix') + '2PiPK'
        seqKPPi = bindMembers( modeNameKPPi, [ PV3D()
                                               , BiKalmanFittedPions
                                               , BiKalmanFittedKaons
                                               , BiKalmanFittedProtons
                                               , Hlt2CharmHadLambdaC2KPPi
                                               , BiKalmanFittedRichLowPTProtons
                                               , Hlt2CharmHadLambdaC2KPPi_Rich
                                               , filterTOS_LC2KPPi ] )
        seqKPK = bindMembers( modeNameKPK, [ PV3D()
                                               , BiKalmanFittedKaons
                                               , BiKalmanFittedProtons
                                               , Hlt2CharmHadLambdaC2KPK
                                               , BiKalmanFittedRichLowPTProtons
                                               , Hlt2CharmHadLambdaC2KPK_Rich
                                               , filterTOS_LC2KPK ] )
        seqPiPPi = bindMembers( modeNamePiPPi, [ PV3D()
                                               , BiKalmanFittedPions
                                               , BiKalmanFittedProtons
                                               , Hlt2CharmHadLambdaC2PiPPi
                                               , BiKalmanFittedRichLowPTProtons
                                               , Hlt2CharmHadLambdaC2PiPPi_Rich
                                               , filterTOS_LC2PiPPi ] )
        seqPiPK = bindMembers( modeNamePiPK, [ PV3D()
                                               , BiKalmanFittedPions
                                               , BiKalmanFittedKaons
                                               , BiKalmanFittedProtons
                                               , Hlt2CharmHadLambdaC2PiPK
                                               , BiKalmanFittedRichLowPTProtons
                                               , Hlt2CharmHadLambdaC2PiPK_Rich
                                               , filterTOS_LC2PiPK ] )

        _sigMassWinCut = "in_range( %(SigMass_M_MIN)s, M, %(SigMass_M_MAX)s )" % self.getProps()
        filterLC2KPPi = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , Inputs = [ seqKPPi ]
                             , Code = _sigMassWinCut )
        filterLC2KPK   = filterLC2KPPi.clone( 'Filter', Inputs = [ seqKPK ] )
        filterLC2PiPPi = filterLC2KPPi.clone( 'Filter', Inputs = [ seqPiPPi ] )
        filterLC2PiPK  = filterLC2KPPi.clone( 'Filter', Inputs = [ seqPiPK ] )


        ## Define the Hlt2 Lines
        ## ####################################################################
        line = Hlt2Line( modeNameKPPi
                        , prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, seqKPPi,
                                            filterLC2KPPi ]
                        , postscale = self.postscale
                        )

        decname = 'Hlt2' + modeNameKPPi + 'Decision'
        id = self._scale(decname, 'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decname : id  } )


        lineWM = line.clone( modeNameKPPi + 'WideMass'
                             , prescale = self.prescale
                             , Filter = { "Code" : "(ALL)" }
                             , postscale = self.postscale )

        decnameWM = 'Hlt2' + modeNameKPPi + 'WideMassDecision'
        idWM = self._scale(decnameWM, 'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decnameWM : idWM  } )


        ## other decay modes
        ## ####################################################################
        lineKPK = Hlt2Line( modeNameKPK
                        , prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, seqKPK,
                                            filterLC2KPK ]
                        , postscale = self.postscale
                        )

        decnameKKP = 'Hlt2' + modeNameKPK + 'Decision'
        idKKP = self._scale(decnameKKP, 'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decnameKKP : idKKP  } )

        lineKPKWM = lineKPK.clone( modeNameKPK + 'WideMass'
                                   , prescale = self.prescale
                                   , Filter = { "Code" : "(ALL)" }
                                   , postscale = self.postscale )

        decnameKKPWM = 'Hlt2' + modeNameKPK + 'WideMassDecision'
        idKKPWM = self._scale(decnameKKPWM, 'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decnameKKPWM : idKKPWM  } )


        linePiPiP = Hlt2Line( modeNamePiPPi
                        , prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, seqPiPPi,
                                            filterLC2PiPPi ]
                        , postscale = self.postscale
                        )

        decnamePiPiP = 'Hlt2' + modeNamePiPPi + 'Decision'
        idPiPiP = self._scale(decnamePiPiP, 'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decnamePiPiP : idPiPiP  } )

        linePiPiPWM = linePiPiP.clone( modeNamePiPPi + 'WideMass'
                                     , prescale = self.prescale
                                     , Filter = { "Code" : "(ALL)" }
                                     , postscale = self.postscale )
        decnamePiPiPWM = 'Hlt2' + modeNamePiPPi + 'WideMassDecision'
        idPiPiPWM = self._scale(decnamePiPiPWM, 'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decnamePiPiPWM : idPiPiPWM  } )


        linePiKP = Hlt2Line( modeNamePiPK
                        , prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk, seqPiPK,
                                            filterLC2PiPK ]
                        , postscale = self.postscale
                        )

        decnamePiKP = 'Hlt2' + modeNamePiPK + 'Decision'
        idPiKP = self._scale(decnamePiKP, 'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decnamePiKP : idPiKP  } )

        linePiKPWM = linePiKP.clone( modeNamePiPK + 'WideMass'
                                     , prescale = self.prescale
                                     , Filter = { "Code" : "(ALL)" }
                                     , postscale = self.postscale )

        decnamePiKPWM = 'Hlt2' + modeNamePiPK + 'WideMassDecision'
        idPiKPWM = self._scale(decnamePiKPWM, 'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decnamePiKPWM : idPiKPWM  } )

    # }
