__author__  = 'Mat Charles, Patrick Spradlin'
__date__    = '$Date: $'
__version__ = '$Revision: $'

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## hadronic decays of charm hadrons.
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, mm

class Hlt2CharmHadD02HHKsLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    __slots__ = {
                ## Slots for K_S h h' lines
                  'KshhTFHHTrkPLL'          : 1500.0 * MeV
                , 'KshhTFHHTrkChi2UL'       : 5.0        # unitless -- 14 Jan: Mat changed from 20.0 to 5.0
                , 'KshhTFHHVtxChi2UL'       : 10.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                , 'KshhTFHHPtLL'            : 1000.0 * MeV
                , 'KshhTFHHFDLL'            : 2.0 * mm        # -- 26 Feb: Mat changed from 1mm to 2mm
                , 'KshhTFHHTightTrkChi2UL'  : 2.0        # unitless -- 27 Feb: Mat adds for use in DD filter
                , 'KshhTFHHTightVtxChi2UL'  : 5.0        # unitless -- 27 Feb: Mat adds for use in DD filter
                , 'KshhTFHHTightFDLL'       : 4.0 * mm       # -- 27 Feb: Mat adds for use in DD filter
                , 'KshhTFHHTightPtLL'       : 1000.0 * MeV     # -- 27 Feb: Mat adds for use in DD filter
                , 'KshhTFHHTightTrkPVIPChi2LL' : 12.5     # unitless -- 28 Feb: Mat adds for use in DD filter
                , 'KshhTFKsLLTrkPLL'        : 2000.0 * MeV
                , 'KshhTFKsLLTrkPVIPChi2LL' : 9.0        # unitless
                , 'KshhTFKsLLTrkChi2UL'     : 20.0       # unitless
                , 'KshhTFKsLLCombSymMassWin': 50.0 * MeV
                , 'KshhTFKsLLMothSymMassWin': 11.4 * MeV
                , 'KshhTFKsLLVtxChi2UL'     : 30.0       # unitless
                , 'KshhTFKsLLVtxPVDispZLL'  : -1000.0 * mm
                , 'KshhTFKsLLVtxPVDispZUL'  : 650.0 * mm
                , 'KshhTFKsLLVtxPVDispChi2LL' : 100.0    # unitless
                , 'KshhTFKsLLDiraLL'        : 0.9997     # unitless
                , 'KshhTFKsDDTrkPLL'        : 2000.0 * MeV
                , 'KshhTFKsDDTrkPVIPChi2LL' : 4.0        # unitless
                , 'KshhTFKsDDTrkChi2UL'     : 10.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                , 'KshhTFKsDDCombSymMassWin': 80.0 * MeV
                , 'KshhTFKsDDMothSymMassWin': 24.9 * MeV
                , 'KshhTFKsDDVtxChi2UL'     : 20.0       # unitless -- 14 Jan: Mat changed from 30 to 20
                , 'KshhTFKsDDVtxPVDispZLL'  : 0.0 * mm
                , 'KshhTFKsDDVtxPVDispZUL'  : 2300.0 * mm
                , 'KshhTFKsDDVtxPVDispChi2LL' : 100.0    # unitless
                , 'KshhTFKsDDDiraLL'        : 0.9999     # unitless -- 14 Jan: Mat changed from 0.9997 to 0.9999
                , 'KshhTFDVtxChi2UL'        : 20.0       # unitless
                , 'KshhTFDPtLL'             : 2000.0 * MeV    # -- 14 Jan: Mat changed from 500 to 2000
                , 'KshhTFDDiraLL'           : 0.0        # unitless -- 14 Jan: Mat changed from 0.999 to 0.0
                , 'KshhTFDwKsLLSymMassWin'  : 100.0 * MeV
                , 'KshhTFDwKsDDSymMassWin'  : 120.0 * MeV
                , 'ComRobUseGEC'            : True       # do or do not 
                , 'ComRobGEC'               : 120        # max number of tracks
                , 'HLT1FILTER'               : ''
                , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                , 'name_prefix'              : 'CharmHadD02HHKs'
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

        hlt = self.getProp("HLT1FILTER")
        if not hlt : hlt = None
        line = Hlt2Line(lineName
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = lclAlgos
                        , HLT = hlt
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
            
        filtName = 'Hlt2' + self.getProp('name_prefix')  + 'KillTooManyInTrkAlg'
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter(filtName
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
        incuts = "(TRCHI2DOF< %(KshhTFHHTrkChi2UL)s )" \
                 "& (P> %(KshhTFHHTrkPLL)s)" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
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
        parentcuts = "(VFASPF(VCHI2PDOF)< %(KshhTFDVtxChi2UL)s )" \
                     "& (PT> %(KshhTFDPtLL)s)" \
                     "& (BPVLTIME() > 0.2*ps)" \
                     "& (BPVDIRA > %(KshhTFDDiraLL)s )" % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineNBody = Hlt2Member( CombineParticles
                                   , 'Combine'
                                   , DecayDescriptors = decayDesc
                                   , Inputs = inputSeq
                                   , CombinationCut = combcuts
                                   , MotherCut = parentcuts
                                 )

        charmNBody = bindMembers( name, inputSeq + [ PV3D(), combineNBody ] )
        return charmNBody
    # }

    ###################################################################
    ## KS -> long tracks reconstruction by hand.
    ## FIXME: Should instead start from Hlt2SharedParticles and filter.
    ##
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
    ##
    def __makeKsLL(self, name) :
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        from HltTracking.HltPVs import PV3D
        KshhKsLLdaugcuts = { "pi+" : "(TRCHI2DOF< %(KshhTFKsLLTrkChi2UL)s)" \
                                     "& (P> %(KshhTFKsLLTrkPLL)s)" \
                                     "& (MIPCHI2DV(PRIMARY)> %(KshhTFKsLLTrkPVIPChi2LL)s )" % self.getProps() }
        KshhKsLLcombcuts = "ADAMASS('KS0')< %(KshhTFKsLLCombSymMassWin)s" % self.getProps()
        KshhKsLLparentcuts = "(ADMASS('KS0')< %(KshhTFKsLLMothSymMassWin)s)" \
                        "& (VFASPF(VCHI2PDOF)< %(KshhTFKsLLVtxChi2UL)s ) " \
                        "& in_range( %(KshhTFKsLLVtxPVDispZLL)s, BPVVDZ, %(KshhTFKsLLVtxPVDispZUL)s)" \
                        "& (BPVDIRA > %(KshhTFKsLLDiraLL)s )" \
                        "& (BPVVDCHI2> %(KshhTFKsLLVtxPVDispChi2LL)s )" % self.getProps()
        combineKshhTFKsLL = Hlt2Member( CombineParticles
                                      , "CombKsLL"
                                      , DecayDescriptor = "KS0 -> pi+ pi-"
                                      , DaughtersCuts   = KshhKsLLdaugcuts
                                      , CombinationCut  = KshhKsLLcombcuts
                                      , MotherCut       = KshhKsLLparentcuts
                                      , Inputs  = [ BiKalmanFittedPions ]
                                      )
        charmKshhTFKsLL = bindMembers( name, [ PV3D(), BiKalmanFittedPions, combineKshhTFKsLL ] )
        return charmKshhTFKsLL

    ###################################################################
    ## KS -> downstream tracks reconstruction by hand.
    ## FIXME: Should instead start from Hlt2SharedParticles and filter.
    ##
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
    ##
    def __makeKsDD(self, name) :
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        from HltTracking.HltPVs import PV3D
        KshhKsDDdaugcuts = { "pi+" : "(TRCHI2DOF< %(KshhTFKsDDTrkChi2UL)s)"
                                     "& (P> %(KshhTFKsDDTrkPLL)s)" \
                                     "& (MIPCHI2DV(PRIMARY)> %(KshhTFKsDDTrkPVIPChi2LL)s )" % self.getProps() }
        KshhKsDDcombcuts = "ADAMASS('KS0')< %(KshhTFKsDDCombSymMassWin)s" % self.getProps()
        KshhKsDDparentcuts = "(ADMASS('KS0')< %(KshhTFKsDDMothSymMassWin)s)" \
                        "& (VFASPF(VCHI2PDOF)< %(KshhTFKsDDVtxChi2UL)s )" \
                        "& in_range(%(KshhTFKsDDVtxPVDispZLL)s, BPVVDZ, %(KshhTFKsDDVtxPVDispZUL)s)" \
                        "& (BPVDIRA > %(KshhTFKsDDDiraLL)s )" \
                        "& (BPVVDCHI2> %(KshhTFKsDDVtxPVDispChi2LL)s )" % self.getProps()
        combineKshhTFKsDD = Hlt2Member( CombineParticles
                                      , "CombKsDD"
                                      , DecayDescriptor = "KS0 -> pi+ pi-"
                                      , DaughtersCuts   = KshhKsDDdaugcuts
                                      , CombinationCut  = KshhKsDDcombcuts
                                      , MotherCut       = KshhKsDDparentcuts
                                      , Inputs  = [ BiKalmanFittedDownPions ]
                                      )
        charmKshhTFKsDD = bindMembers( name, [ PV3D(), BiKalmanFittedDownPions, combineKshhTFKsDD ] )
        return charmKshhTFKsDD

    ###################################################################
    ## Filter the input, requiring that they be TOS on Hlt1Track*
    ##
    ## DANGER: How do we enforce that the name of the filter is globally unique?
    ##         This is easy for an Hlt2Member, but TisTosParticleTagger is not an Hlt2Member...
    ##
    def __filterRequiringTOS(self,name,input) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import TisTosParticleTagger

        filterTOS = TisTosParticleTagger('Hlt2'+name+"TOSFilter")
        filterTOS.TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
        filterTOS.Inputs = [ input.outputSelection() ]
        return bindMembers(name, [ input, filterTOS ])


    ###################################################################
    ##
    ## Utility routine to do a quick Filterdesktop
    ##
    def __quickFilter(self,name,cuts,inputContainers) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop
        filterPlus = Hlt2Member( FilterDesktop
                                 , 'FilterPlus'
                                 , Inputs = inputContainers
                                 , Code = cuts
                                 )
        return bindMembers(name, inputContainers + [filterPlus])

    ###################################################################
    ##
    ## Main configuration routine
    ##
    def __apply_configuration__(self) : # {

        ## Input particles for 2-body HH part of KsHH
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        inPionsName = self.getProp('name_prefix') + 'InputPions'
        inKaonsName = self.getProp('name_prefix') + 'InputKaons'
        lclKshhTFInputKaons = self.__KshhTFHHInPartFilter(inKaonsName, [ BiKalmanFittedKaons] )
        lclKshhTFInputPions = self.__KshhTFHHInPartFilter(inPionsName, [ BiKalmanFittedPions] )


        ## 2-body construction for KsHH
        ## Cut on:
        ##   * Upper bound on 2-body invariant mass: m(pipi) < m(D0) - m(KS) = 1367 MeV (plus wiggle room)
        ##   * Both tracks have same best PV
        ##   * Vertex fit chi2/NDF < KshhTFHHVtxChi2UL = 10.0
        ##   * PT of 2-body combination > KshhTFHHPtLL = 1000 MeV
        ## Don't cut on:
        ##   * IPchi2-to-PV of tracks
        ## Use complicated cut on estimated lifetime of D0
        ##   * As a temporary placeholder, flight distance cut on two-body combination.
        ##     But we can be smarter -- needs some tinkering and perhaps a
        ##     dedicated LoKi functor.
        ###################################################################

        KshhHHcombcuts = """(AM<1450*MeV) 
                            & (APT > %(KshhTFHHPtLL)s)
                            & (AALLSAMEBPV)""" % self.getProps()
        KshhHHparentcuts = """(VFASPF(VCHI2PDOF)< %(KshhTFHHVtxChi2UL)s )
                              & (BPVVD > %(KshhTFHHFDLL)s)""" % self.getProps()

        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        combineKshhTF2Body = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = [ "K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-" ]
                                       , Inputs = [ lclKshhTFInputKaons, lclKshhTFInputPions ]
                                       , CombinationCut = KshhHHcombcuts
                                       , MotherCut = KshhHHparentcuts
                                     )

        from HltTracking.HltPVs import PV3D
        twoBodyName = self.getProp('name_prefix') + '2Body'
        charmKshhTF2Body = bindMembers( twoBodyName,  [PV3D(), lclKshhTFInputKaons, lclKshhTFInputPions, combineKshhTF2Body ] )

        ## Special for down-down: Require that the two-body combination pass more cuts:
        strTighterCuts = """( (CHILDCUT((TRCHI2DOF < %(KshhTFHHTightTrkChi2UL)s ),1)) 
                              & (CHILDCUT((TRCHI2DOF < %(KshhTFHHTightTrkChi2UL)s),2)) 
                              & (VFASPF(VCHI2PDOF) < %(KshhTFHHTightVtxChi2UL)s) 
                              & (PT > %(KshhTFHHTightPtLL)s)
                              & (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFHHTightTrkPVIPChi2LL)s ),1))
                              & (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFHHTightTrkPVIPChi2LL)s ),2))
                              & (BPVVD > %(KshhTFHHTightFDLL)s) )""" % self.getProps()
        twoBodyTighterName = self.getProp('name_prefix') + '2BodyTighter'
        charmKshhTF2BodyTighter = self.__quickFilter(twoBodyTighterName, strTighterCuts, [ charmKshhTF2Body ] )
        ## Special for down-down: Require that the two-body combination be TOS on Hlt1Track:
        twoBodyTighterTOSName = twoBodyTighterName
        charmKshhTF2BodyTighterReqTOS = self.__filterRequiringTOS(twoBodyTighterTOSName, charmKshhTF2BodyTighter )

        ## Ks reconstruction
        ksLLName = self.getProp('name_prefix') + 'LL'
        ksDDName = self.getProp('name_prefix') + 'DD'
        charmKshhTFKsLL = self.__makeKsLL(ksLLName)
        charmKshhTFKsDD = self.__makeKsDD(ksDDName)

        ## D0 -> K_S K* construction sequences.
        ###################################################################
        combineKshhTFD2HHKsLL = self.__KshhTFDCombine(ksLLName
                                        , [charmKshhTF2Body, charmKshhTFKsLL]
                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhTFDwKsLLSymMassWin)s)" % self.getProps() } 
                                        )

        ksLLTOSName = ksLLName
        combineKshhTFD2HHKsLLToS = self.__filterRequiringTOS(ksLLTOSName, combineKshhTFD2HHKsLL )

        combineKshhTFD2HHKsDD = self.__KshhTFDCombine(ksDDName
                                        , [charmKshhTF2BodyTighterReqTOS, charmKshhTFKsDD]
                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhTFDwKsDDSymMassWin)s)" % self.getProps() } 
                                        )

        ## Make the lines
        ###################################################################
        self.__makeLine(ksLLName, algos = [ combineKshhTFD2HHKsLLToS ])
        self.__makeLine(ksDDName, algos = [ combineKshhTFD2HHKsDD ])


    # }
