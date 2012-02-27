from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2CharmHadLambdaCLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'LCMassWinLow'         : 2150   # MeV
                   ,'LCMassWinHigh'       : 2430   # MeV
                   ,'TrChi2'             : 3      # 
                   ,'ChildPT'             : 500   # MeV
                   ,'ChildIPChi2'          : 9   #
                   ,'LCDIRA'               : 0.99985    # 
                   ,'LCPT'                : 2500   # MeV
                   ,'LCFDCHI2'               : 16 # 
                   ,'LCVCHI2'                  : 15   
                   ,'LCProtonPionID'        : 0
                   ,'LCProtonKaonID'        : 0  
                   ,'LCProtonP'             : 10000  
                   ## GEC
                   , 'GEC_Filter_NTRACK'        : True       # do or do not
                   , 'GEC_NTRACK_MAX'           : 180        # max number of tracks
                   , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }   
                   , 'Prescale'         : { } 
                   , 'Postscale'        : { }
                   }
    


    def __apply_configuration__(self) :
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

        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] : 
            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('GEC_Filter_NTRACK') : # { 
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()
        # } 

        filtName = 'Hlt2CharmHadLambdaCKillTooManyInTrkAlg'
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter( filtName
                                                 , Code = filtCode
                                                )   
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        _daughters_cut = "( TRCHI2DOF < %(TrChi2)s ) & ( PT > %(ChildPT)s *MeV ) & ( MIPCHI2DV(PRIMARY) > %(ChildIPChi2)s )" % self.getProps()

        _combination_cut = "(AM>%(LCMassWinLow)s*MeV) & (AM<%(LCMassWinHigh)s*MeV)" % self.getProps()

        _mother_cut = "( PT > %(LCPT)s ) & (BPVDIRA > %(LCDIRA)s ) & ( BPVVDCHI2 > %(LCFDCHI2)s ) & (VFASPF(VCHI2PDOF) < %(LCVCHI2)s)" % self.getProps()


        Hlt2CharmHadLambdaC2KPPi = Hlt2Member( CombineParticles 
                               , "Combine"     
                               , DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc"
                               , DaughtersCuts = { 'pi+' : _daughters_cut, 'K+' : _daughters_cut, 'p+' : _daughters_cut }
                               , CombinationCut = _combination_cut
                               , MotherCut = _mother_cut
                               , Inputs = [ BiKalmanFittedPions.outputSelection(), 
                                            BiKalmanFittedKaons.outputSelection(),
                                            BiKalmanFittedProtons.outputSelection() ])

        _daughters_cut_rich = _daughters_cut + " & (P > %(LCProtonP)s) & (PIDp > %(LCProtonPionID)s) & ((PIDp - PIDK)> %(LCProtonKaonID)s)" % self.getProps()

        Hlt2CharmHadLambdaC2KPPi_Rich = Hlt2Member( CombineParticles 
                               , "CombineRich"    
                               , DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc"
                               , DaughtersCuts = { 'pi+' : _daughters_cut, 'K+' : _daughters_cut, 'p+' : _daughters_cut_rich }
                               , CombinationCut = _combination_cut
                               , MotherCut = _mother_cut
                               , Inputs = [ BiKalmanFittedPions.outputSelection(), 
                                            BiKalmanFittedKaons.outputSelection(),
                                            BiKalmanFittedRichLowPTProtons.outputSelection() ])

        filterTOS_LC2KPPi = Hlt2Member( TisTosParticleTagger
                                , 'Hlt1TOSFilter'
                                , Inputs = [ Hlt2CharmHadLambdaC2KPPi_Rich ]
                                , TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
                              )   

        filterLC2KPPi = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , Inputs = [filterTOS_LC2KPPi]
                             , Code = "(ALL)")

        ###########################################################################
        # Define the Hlt2 Lines
        #
        from HltTracking.HltPVs import PV3D
        line = Hlt2Line('CharmHadLambdaC2KPPi'
                        , prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  BiKalmanFittedPions, 
                                                                        BiKalmanFittedKaons, 
                                                                        BiKalmanFittedProtons, 
                                            Hlt2CharmHadLambdaC2KPPi, 
                                            BiKalmanFittedRichLowPTProtons,
                                            Hlt2CharmHadLambdaC2KPPi_Rich,
                                            filterTOS_LC2KPPi,
                                            filterLC2KPPi ]
                        , postscale = self.postscale
                        )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadLambdaC2KPPiDecision" :55666  } )
