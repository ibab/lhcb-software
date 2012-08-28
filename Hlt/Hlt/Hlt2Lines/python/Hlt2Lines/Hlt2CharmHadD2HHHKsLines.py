__author__  = 'Mat Charles, Patrick Spradlin, Mike Sokoloff, Hamish Gordon'
__date__    = '$Date: $'
__version__ = '$Revision: $'

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## hadronic decays of charm hadrons.
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, mm

class Hlt2CharmHadD2HHHKsLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    __slots__ = {
                ## Slots for K_S h h' lines
                  'KshhhTFHHHTrkPLL'          : 3000.0 * MeV
                , 'KshhhTFHHHTrkPtLL'          : 300.0 * MeV
                , 'KshhhTFHHHTrkMIPChi2'      : 5.0
                , 'KshhhTFHHHTrkChi2UL'       : 5.0        # unitless -- 14 Jan: Mat changed from 20.0 to 5.0
                , 'KshhhTFHHHVtxChi2UL'       : 11.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                , 'KshhhTFHHHPtLL'            : 1300.0 * MeV
                , 'KshhhTFHHHFDLL'            : 2.0 * mm        # -- 26 Feb: Mat changed from 1mm to 2mm
                , 'KshhhTFHHHTightTrkChi2UL'  : 2.0        # unitless -- 27 Feb: Mat adds for use in DD filter
                , 'KshhhTFHHHTightVtxChi2UL'  : 9.0        # unitless -- 27 Feb: Mat adds for use in DD filter
                , 'KshhhTFHHHTightFDLL'       : 4.0 * mm       # -- 27 Feb: Mat adds for use in DD filter
                , 'KshhhTFHHHTightPtLL'       : 1300.0 * MeV # -- 27 Feb: Mat adds for use in DD filter
                , 'KshhhTFHHHTightTrkPVIPChi2LL' : 12.5     # unitless -- 28 Feb: Mat adds for use in DD filter

                , 'KshhhTFKsLLTrkPtLL'       : 200.0  * MeV
                , 'KshhhTFKsLLTrkPVIPChi2LL' : 9.0        # unitless
                , 'KshhhTFKsLLTrkChi2UL'     : 2.5       # unitless;  changed to 10 from 20 111219 mds  to 2.5 120121
                , 'KshhhTFKsLLMothSymMassWin': 30.0 * MeV  #  mds, increased to 30 MeV from 15 120315
                , 'KshhhTFKsLLVtxChi2UL'     : 20.0       # unitless change from 30 to 20 120116
                , 'KshhhTFKsLLVtxPVDispZLL'  : -1000.0 * mm
                , 'KshhhTFKsLLVtxPVDispZUL'  : 650.0 * mm
                , 'KshhhTFKsLLVtxPVDispChi2LL' : 400.0    # unitlessfrom 100 to 300 120114 to 400 120116
                , 'KshhhTFKsLLBpvlTimeLL'    : 4.0        # initial value 120327 is 2.0 ps -----------------Hamish change from 2 to 4 as D+ lives longer than D0
                , 'KshhhTFKsLLDiraLL'        : 0.9997     # unitless
                , 'KshhhTFKsDDTrkPLL'        : 2000.0 * MeV
                , 'KshhhTFKsDDTrkPtLL'       : 200.0  * MeV
                , 'KshhhTFKsDDTrkPVIPChi2LL' : 9.0        # unitless changed from 4.0 to 9.0 120328 mds
                , 'KshhhTFKsDDTrkChi2UL'     : 10.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                , 'KshhhTFKsDDSymMassWin'    : 40.0 * MeV # changed from 80 * MeV 111219 mds
                , 'KshhhTFKsDDVtxChi2UL'     : 20.0       # unitless -- 14 Jan: Mat changed from 30 to 20
                , 'KshhhTFKsDDVtxPVDispZLL'  : 400.0 * mm
                , 'KshhhTFKsDDVtxPVDispZUL'  : 2300.0 * mm
                , 'KshhhTFKsDDVtxPVDispChi2LL' : 100.0    # unitless
                , 'KshhhTFKsDDDiraLL'        : 0.9999     # unitless -- 14 Jan: Mat changed from 0.9997 to 0.9999
                , 'KshhhTFD0VtxChi2UL'        : 20.0       # unitless
                , 'KshhhTFD0PtLL'             : 2000.0 * MeV    # -- 14 Jan: Mat changed from 500 to 2000
                , 'KshhhTFD0DiraLL'           : 0.999        # unitless -- 14 Jan: Mat changed from 0.999 to 0.0; and back to 0.999 120115 mds
                , 'KshhhTFDwKsLLSymMassWin'  : 100.0 * MeV  # restored to 100 MeV 120315 mds
                , 'KshhhTFDwKsDDSymMassWin'  : 120.0 * MeV  # restored to 120 MeV 120315 mds
                , 'KshhhTFD0MassLL'          : 1795.0 * MeV # 1865 - 70
                , 'KshhhTFD0MassUL'          : 2035.0 * MeV # 1965 + 70
                , 'KshhhTFD0BpvlTimeLL'      : 0.3        # initial value 120329 is 0.2 ps
                , 'KshhhTFD0PVIPCHI2UL'      : 20        # inital value 120329 20
                , 'ComRobUseGEC'            : True       # do or do not 
                , 'ComRobGEC'               : 180        # max number of tracks
                , 'HLT1FILTER'               : ''
                , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                , 'name_prefix'              : 'CharmHadD2HHHKs'
                , 'Prescale'                : { }
                , 'Postscale'               : { }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : {
                                   ## Kshhh' lines
                                     'Hlt2CharmHadD2HHHKsLLDecision' : 50935
                                   , 'Hlt2CharmHadD2HHHKsDDDecision' : 50936
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


    def __KshhhTFHHHInPartFilter(self, name, inputContainers) : # {
        """
        # Function to configure a filter for the input particles of the
        #   of the 3-body reconstruction for Kshhh.  It lashes the new
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
        incuts = "(TRCHI2DOF< %(KshhhTFHHHTrkChi2UL)s )" \
                 "& (PT> %(KshhhTFHHHTrkPtLL)s)" \
                 "& (MIPCHI2DV(PRIMARY) > %(KshhhTFHHHTrkMIPChi2)s)" \
                 "& (P> %(KshhhTFHHHTrkPLL)s)" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Remember to require the PV3D reconstruction before cutting on IP.
        filterSeq = bindMembers( name, inputContainers + [PV3D(), filter] )

        return filterSeq
    # }


    def __KshhhTFDCombine(self, name, inputSeq, decayDesc = [ "[D+ -> K*(892)+ KS0]cc" ], extracuts = None) : # {
        """
        # Function to configure the D+ -> K_S hhh combinations
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

        parentcuts = "(VFASPF(VCHI2PDOF)< %(KshhhTFD0VtxChi2UL)s )" \
                     "& (PT> %(KshhhTFD0PtLL)s)" \
                     "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(KshhhTFD0BpvlTimeLL)s * ps)" \
                     "& (BPVIPCHI2() < %(KshhhTFD0PVIPCHI2UL)s )"\
                     "& (MM < %(KshhhTFD0MassUL)s )"\
                     "& (MM > %(KshhhTFD0MassLL)s )"\
                     "& (BPVDIRA > %(KshhhTFD0DiraLL)s )" % self.getProps()

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
        lclKshhhTFInputKaons = self.__KshhhTFHHHInPartFilter(inKaonsName, [ BiKalmanFittedKaons] )
        lclKshhhTFInputPions = self.__KshhhTFHHHInPartFilter(inPionsName, [ BiKalmanFittedPions] )


        ###################################################################

        KshhhHHHcombcuts = """(AM<1530*MeV) 
                            & (APT > %(KshhhTFHHHPtLL)s)
                            & (AALLSAMEBPV)""" % self.getProps()
        KshhhHHHparentcuts = """(VFASPF(VCHI2PDOF)< %(KshhhTFHHHVtxChi2UL)s )
                              & (BPVVD > %(KshhhTFHHHFDLL)s)""" % self.getProps()

        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        combineKshhhTF3Body = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = [ "[K*(892)+ -> pi- pi+ pi-]cc", "[K*(892)+ -> K+ K- pi+]cc", "[K*(892)+ -> K- pi+ pi+]cc", "[K*(892)+ -> K+ pi- pi+]cc" ]
                                       , Inputs = [ lclKshhhTFInputKaons, lclKshhhTFInputPions ]
                                       , CombinationCut = KshhhHHHcombcuts
                                       , MotherCut = KshhhHHHparentcuts
                                     )

        from HltTracking.HltPVs import PV3D
        threeBodyName = self.getProp('name_prefix') + '3Body'
        charmKshhhTF3Body = bindMembers( threeBodyName,  [PV3D(),lclKshhhTFInputKaons,  lclKshhhTFInputPions, combineKshhhTF3Body ] )

        ## Special for down-down: Require that the three-body combination pass more cuts:
        strTighterCuts = """( (CHILDCUT((TRCHI2DOF < %(KshhhTFHHHTightTrkChi2UL)s ),1)) 
                              & (CHILDCUT((TRCHI2DOF < %(KshhhTFHHHTightTrkChi2UL)s),2))
                              & (CHILDCUT((TRCHI2DOF < %(KshhhTFHHHTightTrkChi2UL)s),3)) 
                              & (VFASPF(VCHI2PDOF) < %(KshhhTFHHHTightVtxChi2UL)s) 
                              & (PT > %(KshhhTFHHHTightPtLL)s)
                              & (  (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhhTFHHHTightTrkPVIPChi2LL)s ),1)) 
                              or  (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhhTFHHHTightTrkPVIPChi2LL)s ),2))
                              or  (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhhTFHHHTightTrkPVIPChi2LL)s ),3)))
                              & (BPVVD > %(KshhhTFHHHTightFDLL)s) )""" % self.getProps()
        threeBodyTighterName = self.getProp('name_prefix') + '3BodyTighter'
        charmKshhhTF3BodyTighter = self.__quickFilter(threeBodyTighterName, strTighterCuts, [ charmKshhhTF3Body ] )
        ## Special for down-down: Require that the three-body combination be TOS on Hlt1Track:
        threeBodyTighterTOSName = threeBodyTighterName
        charmKshhhTF3BodyTighterReqTOS = self.__filterRequiringTOS(threeBodyTighterTOSName, charmKshhhTF3BodyTighter )

        ###################################################################

        ksLLName = self.getProp('name_prefix') + 'LL'
        ksDDName = self.getProp('name_prefix') + 'DD'

        ########
        # KsLL  
        #######
        KSLL_FilterCuts= """(ADMASS('KS0')<%(KshhhTFKsLLMothSymMassWin)s) 
               & (BPVVDCHI2 > %(KshhhTFKsLLVtxPVDispChi2LL)s) 
               & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(KshhhTFKsLLBpvlTimeLL)s * ps)
               & (VFASPF(VCHI2)<%(KshhhTFKsLLVtxChi2UL)s) 
               & (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhhTFKsLLTrkPVIPChi2LL)s ),1))
               & (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhhTFKsLLTrkPVIPChi2LL)s ),2))
               & ((CHILDCUT((PT> %(KshhhTFKsLLTrkPtLL)s ) ,1)) or (CHILDCUT((PT> %(KshhhTFKsLLTrkPtLL) s ),2)))
               & (CHILDCUT((TRCHI2DOF < %(KshhhTFKsLLTrkChi2UL)s),1)) 
               & (CHILDCUT((TRCHI2DOF < %(KshhhTFKsLLTrkChi2UL)s),2))"""% self.getProps()
        from Hlt2SharedParticles.Ks import KsLLTF
        KSLLforD02HHKs = Hlt2Member( FilterDesktop # type
                               , "KsLLforD02HHHKs"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code =KSLL_FilterCuts
                               , Inputs = [KsLLTF]
                      )
        charmKshhhTFKsLL = bindMembers( "charmKshhhTFKsLL", [ PV3D(), KsLLTF, KSLLforD02HHKs ] )

        ########
        # KsDD  
        #######
        KSDD_FilterCuts= """(ADMASS('KS0')<%(KshhhTFKsDDSymMassWin)s) 
                     & (BPVVDZ > %(KshhhTFKsDDVtxPVDispZLL)s )
                     & ((CHILDCUT((PT> %(KshhhTFKsDDTrkPtLL)s ) ,1)) or (CHILDCUT((PT> %(KshhhTFKsDDTrkPtLL) s ),2)))
                     & (VFASPF(VCHI2)<%(KshhhTFKsDDVtxChi2UL)s) """% self.getProps()

        from Hlt2SharedParticles.Ks import KsDD
        KsDDforD02HHKs = Hlt2Member( FilterDesktop # type
                               , "KsDDforD02HHHKs"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = KSDD_FilterCuts
                               , Inputs = [KsDD]
                      )
        charmKshhhTFKsDD = bindMembers( "D02HHHKsDD", [ PV3D(), KsDD, KsDDforD02HHKs ] )

        combineKshhhTFD2HHKsLL = self.__KshhhTFDCombine(ksLLName
                                        , [charmKshhhTFKsLL, charmKshhhTF3Body]
                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhhTFDwKsLLSymMassWin)s )" % self.getProps() }
                                        )

        ksLLTOSName = ksLLName
        combineKshhhTFD2HHKsLLToS = self.__filterRequiringTOS(ksLLTOSName, combineKshhhTFD2HHKsLL )

        combineKshhhTFD2HHKsDD = self.__KshhhTFDCombine(ksDDName
                                        , [charmKshhhTFKsDD, charmKshhhTF3BodyTighterReqTOS]
                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhhTFDwKsDDSymMassWin)s )" % self.getProps() }
                                        )


        ## Make the lines
        ###################################################################
        self.__makeLine(ksLLName, algos = [ combineKshhhTFD2HHKsLLToS ])
        self.__makeLine(ksDDName, algos = [ combineKshhhTFD2HHKsDD ])


    # }
