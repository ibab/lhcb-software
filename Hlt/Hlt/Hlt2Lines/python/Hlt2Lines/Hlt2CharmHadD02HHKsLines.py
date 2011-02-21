__author__  = 'Mat Charles, Patrick Spradlin'
__date__    = '$Date: $'
__version__ = '$Revision: $'

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## hadronic decays of charm hadrons.
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2CharmHadD02HHKsLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    __slots__ = {
                ## Slots for K_S h h' lines
                  'KshhTFHHTrkPLL'          : 1500.0     # in MeV
                , 'KshhTFHHTrkChi2UL'       : 5.0        # unitless -- 14 Jan: Mat changed from 20.0 to 5.0
                , 'KshhTFHHTrkMaxPVIPLL'    : 0.1        # in mm
                , 'KshhTFHHVtxChi2UL'       : 10.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                , 'KshhTFHHVtxPVDispLL'     : -1.0       # in mm
                , 'KshhTFHHPtLL'            : 500.0      # in MeV -- bumping up to even 1000 doesn't really help
                , 'KshhTFKsLLTrkPLL'        : 2000.0     # in MeV
                , 'KshhTFKsLLTrkPVIPChi2LL' : 9.0        # unitless
                , 'KshhTFKsLLTrkChi2UL'     : 20.0       # unitless
                , 'KshhTFKsLLCombSymMassWin': 50.0       # in MeV
                , 'KshhTFKsLLMothSymMassWin': 11.4       # in MeV
                , 'KshhTFKsLLVtxChi2UL'     : 30.0       # unitless
                , 'KshhTFKsLLVtxPVDispZLL'  : -1000.0    # in mm
                , 'KshhTFKsLLVtxPVDispZUL'  : 650.0      # in mm
                , 'KshhTFKsLLVtxPVDispChi2LL' : 100.0    # unitless
                , 'KshhTFKsLLDiraLL'        : 0.9997     # unitless
                , 'KshhTFKsDDTrkPLL'        : 2000.0     # in MeV
                , 'KshhTFKsDDTrkPVIPChi2LL' : 4.0        # unitless
                , 'KshhTFKsDDTrkChi2UL'     : 10.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                , 'KshhTFKsDDCombSymMassWin': 80.0       # in MeV
                , 'KshhTFKsDDMothSymMassWin': 24.9       # in MeV
                , 'KshhTFKsDDVtxChi2UL'     : 20.0       # unitless -- 14 Jan: Mat changed from 30 to 20
                , 'KshhTFKsDDVtxPVDispZLL'  : 0.0        # in mm
                , 'KshhTFKsDDVtxPVDispZUL'  : 2300.0     # in mm
                , 'KshhTFKsDDVtxPVDispChi2LL' : 100.0    # unitless
                , 'KshhTFKsDDDiraLL'        : 0.9999     # unitless -- 14 Jan: Mat changed from 0.9997 to 0.9999
                , 'KshhTFDVtxChi2UL'        : 20.0       # unitless
                , 'KshhTFDVtxPVDispLL'      : -1.0       # in mm
                , 'KshhTFDPtLL'             : 2000.0     # in MeV -- 14 Jan: Mat changed from 500 to 2000
                , 'KshhTFDDiraLL'           : 0.0        # unitless -- 14 Jan: Mat changed from 0.999 to 0.0
                , 'KshhTFDwKsLLSymMassWin'  : 100.0      # in MeV
                , 'KshhTFDwKsDDSymMassWin'  : 120.0      # in MeV
                , 'ComRobUseGEC'            : True       # do or do not 
                , 'ComRobGEC'               : 120        # max number of tracks
                , 'HLT1FILTER'               : None
                , 'Prescale'                : { }
                , 'Postscale'               : { }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : {
                                   ## Kshh' lines
                                     'Hlt2CharmHadD02HHKsLLDecision' : 50920
                                   , 'Hlt2CharmHadD02HHKsDDDecision' : 50921
                                   }
                  }


    def __updateHltANNSvc(self,line) : # {
        """
        Wrapper for updating the HltANNSvc after a new line has been
        constructed.  This should eventually become obsolete.
        """
        from Configurables import HltANNSvc
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { lineName : id } )
    # }


    def __makeLine(self, lineName, algos) : # {
        """
        Wrapper for line construction that also registers it to the HltANNSvc.
        """
        from HltLine.HltLine import Hlt2Line

        lclAlgos = []
        ## Prepend a filter on the number of tracks, if required.
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()
        lclAlgos = [ Hlt2CharmKillTooManyInTrk ]
        lclAlgos.extend(algos)

        line = Hlt2Line(lineName
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = lclAlgos
                        , HLT = self.getProp("HLT1FILTER")
                       )
        self.__updateHltANNSvc(lineName)
    # }


    def __seqGEC(self) : # {
        """
        # Defines a Global Event Cut (mild badness) on all events with more
        # than a configurable upper limit of tracks.
        #  
        # This is a temporary fix only and will be removed once proper
        # GEC lines are implemented
        """
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        from HltLine.HltLine import bindMembers

        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('ComRobUseGEC') : # {
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(ComRobGEC)s" % self.getProps()
        # }
            
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter('Hlt2CharmKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __KshhTFHHInPartFilter(self, name, inputContainers) : # {
        """
        # Function to configure a filter for the input particles of the
        #   of the 2-body reconstruction for Kshh.  It lashes the new
        #   FilterDesktop to a bindMembers with its antecedents.
        # The argument inputContainer should be 
        #   a list of bindMember sequences that produces the particles
        #   to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        # Cut on:
        #  * Momenta of the track
        #  * Chi2/NDF of the track

        ## 14 Jan: Mat removes hard-coded cut on track p(chi2)
        incuts = """(P> %(KshhTFHHTrkPLL)s *MeV)
                    & (TRCHI2DOF< %(KshhTFHHTrkChi2UL)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )

        ## Remember to require the PV3D reconstruction before cutting on IP.
        filterSeq = bindMembers( name, inputContainers + [PV3D(), filter] )

        return filterSeq
    # }


    def __KshhTFDCombine(self, name, inputSeq, decayDesc = [ "D0 -> K*(892)0 KS0" ], extracuts = None) : # {
        """
        # Function to configure the D0 -> K_S K*0 combinations
        #   It lashes the new CombineParticles to a bindMembers with its
        #   antecedents.
        # Its arguments:
        #     name      : string name
        #     inputSeq  : list of input particle sequences
        #     decayDesc : list of string decay descriptors
        #     extracuts : dictionary of extra cuts to be applied.
        #                 Can include cuts at the CombinationCut or at
        #                 the MotherCut level.
        #                 e.g. : { 'CombinationCut' : '(AM>4*GeV)'
        #                        , 'MotherCut'      : '(BPVDIRA>0.5)' }
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        ## Do cut on:
        ##  * Mass (varies by mode)
        ##  * PT
        ##  * TAU (currently hard-coded; move to configurable slot)
        ##  * DIRA > 0.0 (just to remove backwards-going D0)
        ##  * Vertex chi2
        ###################################################################

        # Construct a cut string for the combination.
        combcuts = ""

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(VFASPF(VCHI2PDOF)< %(KshhTFDVtxChi2UL)s )
                          & (BPVLTIME() > 0.2*ps)
                          & (PT> %(KshhTFDPtLL)s *MeV)
                          & (BPVDIRA > %(KshhTFDDiraLL)s )""" % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineNBody = Hlt2Member( CombineParticles
                                   , 'Combine'
                                   , DecayDescriptors = decayDesc
                                   , InputLocations = inputSeq
                                   , CombinationCut = combcuts
                                   , MotherCut = parentcuts
                                 )

        charmNBody = bindMembers( name, inputSeq + [ PV3D(), combineNBody ] )
        return charmNBody
    # }


    def __apply_configuration__(self) : # {

        ## Input particles for 2-body HH part of KsHH
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        lclKshhTFInputKaons = self.__KshhTFHHInPartFilter('CharmKshhTFHHInputKaons', [ BiKalmanFittedKaons] )
        lclKshhTFInputPions = self.__KshhTFHHInPartFilter('CharmKshhTFHHInputPions', [ BiKalmanFittedPions] )


        ## 2-body construction for KsHH
        ## Cut on:
        ##   * Upper bound on 2-body invariant mass: m(pipi) < m(D0) - m(KS) = 1367 MeV (plus wiggle room)
        ##   * Both tracks have same best PV
        ##   * Vertex fit chi2/NDF < KshhTFHHVtxChi2UL = 10.0
        ##   * PT of 2-body combination > KshhTFHHPtLL = 500
        ## Don't cut on:
        ##   * IPchi2-to-PV of tracks
        ## Use complicated cut on estimated lifetime of D0
        ##   * As a temporary placeholder, flight distance cut on two-body combination. (hard-coded)
        ##     But we can be smarter -- needs some tinkering and perhaps a
        ##     dedicated LoKi functor.
        ###################################################################

        KshhHHcombcuts = """(AM<1450*MeV) 
                            & (AALLSAMEBPV)
                            & (APT > %(KshhTFHHPtLL)s *MeV)""" % self.getProps()
        KshhHHparentcuts = """(VFASPF(VCHI2PDOF)< %(KshhTFHHVtxChi2UL)s )
                              & (BPVVD > 1.0*mm)""" % self.getProps()

        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        combineKshhTF2Body = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = [ "K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-" ]
                                       , InputLocations = [ lclKshhTFInputKaons, lclKshhTFInputPions ]
                                       , CombinationCut = KshhHHcombcuts
                                       , MotherCut = KshhHHparentcuts
                                     )

        from HltTracking.HltPVs import PV3D
        charmKshhTF2Body = bindMembers( 'CharmKshhTF2Body',  [PV3D(), lclKshhTFInputKaons, lclKshhTFInputPions, combineKshhTF2Body ] )


        ## Ks reconstruction
        ## These should probably be moved to SharedParticles

        ###################################################################
        ## These cuts are used by the long-long routine and are probably OK:
        ##   Tracks:
        ##    * P > KshhTFKsLLTrkPLL = 2000 MeV
        ##    * MIPCHI2DV(PRIMARY) > KshhTFKsLLTrkPVIPChi2LL = 9.0
        ##    * TRCHI2DOF < KshhTFKsLLTrkChi2UL = 20
        ##   Combination:
        ##    * Mass within KshhTFKsLLCombSymMassWin = 50.0
        ##   Mother:
        ##    * Vertex chi2/NDF < KshhTFKsLLVtxChi2UL = 30.0
        ##    * KshhTFKsLLVtxPVDispZLL in range (KshhTFKsLLVtxPVDispZLL,KshhTFKsLLVtxPVDispZUL) = (-1000.0, 650)
        ##    * BPVDIRA > KshhTFKsLLDiraLL = 0.9997
        ##    * Mass within KshhTFKsLLMothSymMassWin = 11.4
        ##    * BPVVDCHI2 > KshhTFKsLLVtxPVDispChi2LL = 100.0

        KshhKsLLdaugcuts = { "pi+" : "(P> %(KshhTFKsLLTrkPLL)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KshhTFKsLLTrkPVIPChi2LL)s ) & (TRCHI2DOF< %(KshhTFKsLLTrkChi2UL)s)" % self.getProps() }
        KshhKsLLcombcuts = "ADAMASS('KS0')< %(KshhTFKsLLCombSymMassWin)s *MeV" % self.getProps()
        KshhKsLLparentcuts = """(VFASPF(VCHI2PDOF)< %(KshhTFKsLLVtxChi2UL)s ) 
                        & in_range( %(KshhTFKsLLVtxPVDispZLL)s *mm, BPVVDZ, %(KshhTFKsLLVtxPVDispZUL)s *mm) 
                        & (BPVDIRA > %(KshhTFKsLLDiraLL)s ) 
                        & (ADMASS('KS0')< %(KshhTFKsLLMothSymMassWin)s *MeV) 
                        & (BPVVDCHI2> %(KshhTFKsLLVtxPVDispChi2LL)s )""" % self.getProps()
        combineKshhTFKsLL = Hlt2Member( CombineParticles
                                      , "KsLL"
                                      , DecayDescriptor = "KS0 -> pi+ pi-"
                                      , DaughtersCuts   = KshhKsLLdaugcuts
                                      , CombinationCut  = KshhKsLLcombcuts
                                      , MotherCut       = KshhKsLLparentcuts
                                      , InputLocations  = [ BiKalmanFittedPions ]
                                      )
        charmKshhTFKsLL = bindMembers( "CharmKshhTFKsLL", [ PV3D(), BiKalmanFittedPions, combineKshhTFKsLL ] )


        ## Repeat for KsDD with independent cuts.
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions

        ###################################################################
        ## These cuts are used by the down-down routine and may be too loose:
        ##   Tracks:
        ##    * P > KshhTFKsDDTrkPLL = 2000 MeV
        ##    * MIPCHI2DV(PRIMARY) > KshhTFKsDDTrkPVIPChi2LL = 4.0
        ##    * TRCHI2DOF < KshhTFKsDDTrkChi2UL = 10.0 
        ##   Combination:
        ##    * Mass within KshhTFKsDDCombSymMassWin = 80.0
        ##   Mother:
        ##    * Vertex chi2/NDF < KshhTFKsDDVtxChi2UL = 20.0
        ##    * BPVVDZ in range (KshhTFKsDDVtxPVDispZLL,KshhTFKsDDVtxPVDispZUL = (0, 2300)
        ##    * BPVDIRA > KshhTFKsDDDiraLL = 0.9999
        ##    * ADMASS within KshhTFKsDDMothSymMassWin = 24.9
        ##    * BPVVDCHI2 > KshhTFKsDDVtxPVDispChi2LL = 100.0

        KshhKsDDdaugcuts = { "pi+" : "(P> %(KshhTFKsDDTrkPLL)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KshhTFKsDDTrkPVIPChi2LL)s ) & (TRCHI2DOF< %(KshhTFKsDDTrkChi2UL)s)" % self.getProps() }
        KshhKsDDcombcuts = "ADAMASS('KS0')< %(KshhTFKsDDCombSymMassWin)s *MeV" % self.getProps()
        KshhKsDDparentcuts = """(VFASPF(VCHI2PDOF)< %(KshhTFKsDDVtxChi2UL)s ) 
                        & in_range(%(KshhTFKsDDVtxPVDispZLL)s *mm, BPVVDZ, %(KshhTFKsDDVtxPVDispZUL)s *mm) 
                        & (BPVDIRA > %(KshhTFKsDDDiraLL)s ) 
                        & (ADMASS('KS0')< %(KshhTFKsDDMothSymMassWin)s *MeV) 
                        & (BPVVDCHI2> %(KshhTFKsDDVtxPVDispChi2LL)s )""" % self.getProps()
        combineKshhTFKsDD = Hlt2Member( CombineParticles
                                      , "KsDD"
                                      , DecayDescriptor = "KS0 -> pi+ pi-"
                                      , DaughtersCuts   = KshhKsDDdaugcuts
                                      , CombinationCut  = KshhKsDDcombcuts
                                      , MotherCut       = KshhKsDDparentcuts
                                      , InputLocations  = [ BiKalmanFittedDownPions ]
                                      )
        charmKshhTFKsDD = bindMembers( "CharmKshhTFKsDD", [ PV3D(), BiKalmanFittedDownPions, combineKshhTFKsDD ] )


        ## D0 -> K_S K* construction sequences.
        ###################################################################
        combineKshhTFD2HHKsLL = self.__KshhTFDCombine('CharmKshhTFD2HHKsLL'
                                        , [charmKshhTF2Body, charmKshhTFKsLL]
                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhTFDwKsLLSymMassWin)s *MeV)" % self.getProps() } 
                                        )
        combineKshhTFD2HHKsDD = self.__KshhTFDCombine('CharmKshhTFD2HHKsDD'
                                        , [charmKshhTF2Body, charmKshhTFKsDD]
                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhTFDwKsDDSymMassWin)s *MeV)" % self.getProps() } 
                                        )

        ## Make the lines
        ###################################################################
        self.__makeLine('CharmHadD02HHKsLL' , algos = [ combineKshhTFD2HHKsLL ])
        self.__makeLine('CharmHadD02HHKsDD' , algos = [ combineKshhTFD2HHKsDD ])


    # }
