__author__  = 'Mat Charles, Patrick Spradlin, Mike Sokoloff'
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
                , 'KshhTFHHTightPtLL'       : 1000.0 * MeV # -- 27 Feb: Mat adds for use in DD filter
                , 'KshhTFHHTightTrkPVIPChi2LL' : 12.5     # unitless -- 28 Feb: Mat adds for use in DD filter
                , 'KshhTFKsLLTrkPtLL'       : 200.0  * MeV
                , 'KshhTFKsLLTrkPVIPChi2LL' : 9.0        # unitless
                , 'KshhTFKsLLTrkChi2UL'     : 2.5       # unitless;  changed to 10 from 20 111219 mds  to 2.5 120121
                , 'KshhTFKsLLMothSymMassWin': 30.0 * MeV  #  mds, increased to 30 MeV from 15 120315
                , 'KshhTFKsLLVtxChi2UL'     : 20.0       # unitless change from 30 to 20 120116
                , 'KshhTFKsLLVtxPVDispZLL'  : -1000.0 * mm
                , 'KshhTFKsLLVtxPVDispZUL'  : 650.0 * mm
                , 'KshhTFKsLLVtxPVDispChi2LL' : 400.0    # unitlessfrom 100 to 300 120114 to 400 120116
                , 'KshhTFKsLLBpvlTimeLL'    : 2.0        # initial value 120327 is 2.0 ps
                , 'KshhTFKsLLDiraLL'        : 0.9997     # unitless
                , 'KshhTFKsDDTrkPLL'        : 2000.0 * MeV
                , 'KshhTFKsDDTrkPtLL'       : 200.0  * MeV
                , 'KshhTFKsDDTrkPVIPChi2LL' : 9.0        # unitless changed from 4.0 to 9.0 120328 mds
                , 'KshhTFKsDDTrkChi2UL'     : 10.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                , 'KshhTFKsDDSymMassWin'    : 40.0 * MeV # changed from 80 * MeV 111219 mds
                , 'KshhTFKsDDVtxChi2UL'     : 20.0       # unitless -- 14 Jan: Mat changed from 30 to 20
                , 'KshhTFKsDDVtxPVDispZLL'  : 400.0 * mm
                , 'KshhTFKsDDVtxPVDispZUL'  : 2300.0 * mm
                , 'KshhTFKsDDVtxPVDispChi2LL' : 100.0    # unitless
                , 'KshhTFKsDDDiraLL'        : 0.9999     # unitless -- 14 Jan: Mat changed from 0.9997 to 0.9999
                , 'KshhTFD0VtxChi2UL'        : 20.0       # unitless
                , 'KshhTFD0PtLL'             : 2000.0 * MeV    # -- 14 Jan: Mat changed from 500 to 2000
                , 'KshhTFD0DiraLL'           : 0.999        # unitless -- 14 Jan: Mat changed from 0.999 to 0.0; and back to 0.999 120115 mds
                , 'KshhTFDwKsLLSymMassWin'  : 100.0 * MeV  # restored to 100 MeV 120315 mds
                , 'KshhTFDwKsDDSymMassWin'  : 120.0 * MeV  # restored to 120 MeV 120315 mds
                , 'KshhTFD0MassLL'          : 1795.0 * MeV # 1865 - 70
                , 'KshhTFD0MassUL'          : 1935.0 * MeV # 1865 + 70
                , 'KshhTFD0BpvlTimeLL'      : 0.3        # initial value 120329 is 0.2 ps
                , 'KshhTFD0PVIPCHI2UL'      : 20        # inital value 120329 20
                , 'ComRobUseGEC'            : True       # do or do not 
                , 'ComRobGEC'               : 180        # max number of tracks
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
        #  move PV3D upfront in case it is present
        #  note that any duplication gets automatically removed, so we 
        #  keep the original 'as is'
        from HltTracking.HltPVs import PV3D
        pv = PV3D()
        if set(pv.members()).issubset(set([ j for i in algos for j in i.members() ])) : 
            lclAlgos.insert( 0, pv )

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

        ###################################################################

        # Construct a cut string for the combination.
        combcuts = ""

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + extracuts['CombinationCut']

        parentcuts = "(VFASPF(VCHI2PDOF)< %(KshhTFD0VtxChi2UL)s )" \
                     "& (PT> %(KshhTFD0PtLL)s)" \
                     "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(KshhTFD0BpvlTimeLL)s * ps)" \
                     "& (BPVIPCHI2() < %(KshhTFD0PVIPCHI2UL)s )"\
                     "& (MM < %(KshhTFD0MassUL)s )"\
                     "& (MM > %(KshhTFD0MassLL)s )"\
                     "& (BPVDIRA > %(KshhTFD0DiraLL)s )" % self.getProps()

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
    ## Filter the input, requiring that they be TOS on Hlt1Track*
    ##
    def __filterRequiringTOS(self,name,input) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import TisTosParticleTagger

        filterTOS = Hlt2Member( TisTosParticleTagger
                                , 'TOSFilter'

                                , Inputs = [ input.outputSelection() ]
                                , TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
                              )
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

        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from HltTracking.HltPVs import PV3D

        ## Input particles for 2-body HH part of KsHH
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons
        inPionsName = self.getProp('name_prefix') + 'InputPions'
        inKaonsName = self.getProp('name_prefix') + 'InputKaons'
        lclKshhTFInputKaons = self.__KshhTFHHInPartFilter(inKaonsName, [ BiKalmanFittedKaons] )
        lclKshhTFInputPions = self.__KshhTFHHInPartFilter(inPionsName, [ BiKalmanFittedPions] )


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
        charmKshhTF2Body = bindMembers( twoBodyName,  [PV3D(),lclKshhTFInputKaons,  lclKshhTFInputPions, combineKshhTF2Body ] )

        ## Special for down-down: Require that the two-body combination pass more cuts:
        strTighterCuts = """( (CHILDCUT((TRCHI2DOF < %(KshhTFHHTightTrkChi2UL)s ),1)) 
                              & (CHILDCUT((TRCHI2DOF < %(KshhTFHHTightTrkChi2UL)s),2)) 
                              & (VFASPF(VCHI2PDOF) < %(KshhTFHHTightVtxChi2UL)s) 
                              & (PT > %(KshhTFHHTightPtLL)s)
                              & (  (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFHHTightTrkPVIPChi2LL)s ),1)) 
                              or  (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFHHTightTrkPVIPChi2LL)s ),2))  )
                              & (BPVVD > %(KshhTFHHTightFDLL)s) )""" % self.getProps()
        twoBodyTighterName = self.getProp('name_prefix') + '2BodyTighter'
        charmKshhTF2BodyTighter = self.__quickFilter(twoBodyTighterName, strTighterCuts, [ charmKshhTF2Body ] )
        ## Special for down-down: Require that the two-body combination be TOS on Hlt1Track:
        twoBodyTighterTOSName = twoBodyTighterName
        charmKshhTF2BodyTighterReqTOS = self.__filterRequiringTOS(twoBodyTighterTOSName, charmKshhTF2BodyTighter )

        ###################################################################

        ksLLName = self.getProp('name_prefix') + 'LL'
        ksDDName = self.getProp('name_prefix') + 'DD'

        ########
        # KsLL  
        #######
        KSLL_FilterCuts= """(ADMASS('KS0')<%(KshhTFKsLLMothSymMassWin)s) 
               & (BPVVDCHI2 > %(KshhTFKsLLVtxPVDispChi2LL)s) 
               & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(KshhTFKsLLBpvlTimeLL)s * ps)
               & (VFASPF(VCHI2)<%(KshhTFKsLLVtxChi2UL)s) 
               & (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFKsLLTrkPVIPChi2LL)s ),1))
               & (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFKsLLTrkPVIPChi2LL)s ),2))
               & ((CHILDCUT((PT> %(KshhTFKsLLTrkPtLL)s ) ,1)) or (CHILDCUT((PT> %(KshhTFKsLLTrkPtLL) s ),2)))
               & (CHILDCUT((TRCHI2DOF < %(KshhTFKsLLTrkChi2UL)s),1)) 
               & (CHILDCUT((TRCHI2DOF < %(KshhTFKsLLTrkChi2UL)s),2)) """% self.getProps()
        from Hlt2SharedParticles.Ks import KsLLTF
        KSLLforD02HHKs = Hlt2Member( FilterDesktop # type
                               , "KsLLforD02HHKs"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code =KSLL_FilterCuts
                               , Inputs = [KsLLTF]
                      )
        charmKshhTFKsLL = bindMembers( "charmKshhTFKsLL", [ PV3D(), KsLLTF, KSLLforD02HHKs ] )

        ########
        # KsDD  
        #######
        KSDD_FilterCuts= """(ADMASS('KS0')<%(KshhTFKsDDSymMassWin)s) 
                     & (BPVVDZ > %(KshhTFKsDDVtxPVDispZLL)s )
                     & ((CHILDCUT((PT> %(KshhTFKsDDTrkPtLL)s ) ,1)) or (CHILDCUT((PT> %(KshhTFKsDDTrkPtLL) s ),2)))
                     & (VFASPF(VCHI2)<%(KshhTFKsDDVtxChi2UL)s) """% self.getProps()

        from Hlt2SharedParticles.Ks import KsDD
        KsDDforD02HHKs = Hlt2Member( FilterDesktop # type
                               , "KsDDforD02HHKs"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = KSDD_FilterCuts
                               , Inputs = [KsDD]
                      )
        charmKshhTFKsDD = bindMembers( "D02HHKsKsDD", [ PV3D(), KsDD, KsDDforD02HHKs ] )

        combineKshhTFD2HHKsLL = self.__KshhTFDCombine(ksLLName
                                        , [charmKshhTFKsLL, charmKshhTF2Body]
                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhTFDwKsLLSymMassWin)s )" % self.getProps() }
                                        )

        ksLLTOSName = ksLLName
        combineKshhTFD2HHKsLLToS = self.__filterRequiringTOS(ksLLTOSName, combineKshhTFD2HHKsLL )

        combineKshhTFD2HHKsDD = self.__KshhTFDCombine(ksDDName
                                        , [charmKshhTFKsDD, charmKshhTF2BodyTighterReqTOS]
                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhTFDwKsDDSymMassWin)s )" % self.getProps() }
                                        )


        ## Make the lines
        ###################################################################
        self.__makeLine(ksLLName, algos = [ combineKshhTFD2HHKsLLToS ])
        self.__makeLine(ksDDName, algos = [ combineKshhTFD2HHKsDD ])


    # }
