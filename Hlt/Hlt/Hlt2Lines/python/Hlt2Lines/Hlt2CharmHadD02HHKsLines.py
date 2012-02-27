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
                , 'KshhTFKsLLTrkPLL'        : 2000.0 * MeV
                , 'KshhTFKsLLTrkPVIPChi2LL' : 9.0        # unitless
                , 'KshhTFKsLLTrkChi2UL'     : 2.5       # unitless;  changed to 10 from 20 111219 mds  to 2.5 120121
                , 'KshhTFKsLLCombSymMassWin': 20.0 * MeV # changed from 50. * MeV 111219 mds - may be used
                                                         # for a different purpose now?
                , 'KshhTFKsLLMothSymMassWin': 15.0 * MeV  #  mds, add some sidebands, 120122, up from 11.4 * MeV
                , 'KshhTFKsLLVtxChi2UL'     : 20.0       # unitless change from 30 to 20 120116
                , 'KshhTFKsLLVtxPVDispZLL'  : -1000.0 * mm
                , 'KshhTFKsLLVtxPVDispZUL'  : 650.0 * mm
                , 'KshhTFKsLLVtxPVDispChi2LL' : 400.0    # unitlessfrom 100 to 300 120114 to 400 120116
                , 'KshhTFKsLLDiraLL'        : 0.9997     # unitless
                , 'KshhTFKsDDTrkPLL'        : 2000.0 * MeV
                , 'KshhTFKsDDTrkPVIPChi2LL' : 4.0        # unitless
                , 'KshhTFKsDDTrkChi2UL'     : 10.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                , 'KshhTFKsDDCombSymMassWin': 40.0 * MeV # changed from 80 * MeV 111219 mds
                , 'KshhTFKsDDMothSymMassWin': 24.9 * MeV
                , 'KshhTFKsDDVtxChi2UL'     : 20.0       # unitless -- 14 Jan: Mat changed from 30 to 20
                , 'KshhTFKsDDVtxPVDispZLL'  : 0.0 * mm
                , 'KshhTFKsDDVtxPVDispZUL'  : 2300.0 * mm
                , 'KshhTFKsDDVtxPVDispChi2LL' : 100.0    # unitless
                , 'KshhTFKsDDDiraLL'        : 0.9999     # unitless -- 14 Jan: Mat changed from 0.9997 to 0.9999
                , 'KshhTFDVtxChi2UL'        : 20.0       # unitless
                , 'KshhTFDPtLL'             : 2000.0 * MeV    # -- 14 Jan: Mat changed from 500 to 2000
                , 'KshhTFDDiraLL'           : 0.999        # unitless -- 14 Jan: Mat changed from 0.999 to 0.0; and back to 0.999 120115 mds
                , 'KshhTFDwKsLLSymMassWin'  : 65.0 * MeV  # changed from 100 MeV to 65 MeV 120116 mds
                , 'KshhTFDwKsDDSymMassWin'  : 70.0 * MeV  # changed from 120 MeV to 70 MeV 120116 mds
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
## mds 20120216                     "& (ADMASS('D0') < 70*MeV)"\
## mds 20120216 make the mass window asymmetric for Kshort,pi,pi to account 
## mds 20120216 for Kshort,K,K (and Kshort,Kpi)
        parentcuts = "(VFASPF(VCHI2PDOF)< %(KshhTFDVtxChi2UL)s )" \
                     "& (PT> %(KshhTFDPtLL)s)" \
                     "& (BPVLTIME() > 0.2*ps)" \
                     "& (BPVIPCHI2() < 20)"\
                     "& (MM < 1935.0 * MeV)"\
                     "& (MM > 1665.0 * MeV)"\
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

## copy the following lines from Hlt2B2KsHHLines.py
## to allow use of whatever with imported Kshort candidates, etc.
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from HltTracking.HltPVs import PV3D

        ## Input particles for 2-body HH part of KsHH
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        inPionsName = self.getProp('name_prefix') + 'InputPions'
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
                                       , DecayDescriptors = [ "K*(892)0 -> pi+ pi-" ]
                                       , Inputs = [ lclKshhTFInputPions ]
                                       , CombinationCut = KshhHHcombcuts
                                       , MotherCut = KshhHHparentcuts
                                     )

        from HltTracking.HltPVs import PV3D
        twoBodyName = self.getProp('name_prefix') + '2Body'
        charmKshhTF2Body = bindMembers( twoBodyName,  [PV3D(), lclKshhTFInputPions, combineKshhTF2Body ] )

        ## Special for down-down: Require that the two-body combination pass more cuts:
        strTighterCuts = """( (CHILDCUT((TRCHI2DOF < %(KshhTFHHTightTrkChi2UL)s ),1)) 
                              & (CHILDCUT((TRCHI2DOF < %(KshhTFHHTightTrkChi2UL)s),2)) 
                              & (VFASPF(VCHI2PDOF) < %(KshhTFHHTightVtxChi2UL)s) 
                              & (PT > %(KshhTFHHTightPtLL)s)
                              & (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFHHTightTrkPVIPChi2LL)s ),1)) 
                                or  (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFHHTightTrkPVIPChi2LL)s ),2))
                              & (BPVVD > %(KshhTFHHTightFDLL)s) )""" % self.getProps()
        twoBodyTighterName = self.getProp('name_prefix') + '2BodyTighter'
        charmKshhTF2BodyTighter = self.__quickFilter(twoBodyTighterName, strTighterCuts, [ charmKshhTF2Body ] )
        ## Special for down-down: Require that the two-body combination be TOS on Hlt1Track:
        twoBodyTighterTOSName = twoBodyTighterName
        charmKshhTF2BodyTighterReqTOS = self.__filterRequiringTOS(twoBodyTighterTOSName, charmKshhTF2BodyTighter )

        ###################################################################

        ksLLName = self.getProp('name_prefix') + 'LL'
        ksDDName = self.getProp('name_prefix') + 'DD'

        print "mds:  ksLLName and ksDDName  =  ", ksLLName, "  ", ksDDName


        ########
        # KsLL  
        #######
## mds        KSLL_FilterCuts= """(ADMASS('KS0')<%(KshhTFKsLLCombSymMassWin)s) 
## mds               & (CHILDCUT((PT>500 * MeV),1))    
## mds               & (CHILDCUT((PT>500 * MeV),2))
        KSLL_FilterCuts= """(ADMASS('KS0')<%(KshhTFKsLLMothSymMassWin)s) 
               & (BPVVDCHI2 > %(KshhTFKsLLVtxPVDispChi2LL)s) 
               & (BPVLTIME() > 2.0*ps)
               & (VFASPF(VCHI2)<%(KshhTFKsLLVtxChi2UL)s) 
               & (CHILDCUT((MIPCHI2DV(PRIMARY) > 9. ),1))
               & (CHILDCUT((MIPCHI2DV(PRIMARY) > 9. ),2))
               & ((CHILDCUT((PT>200 * MeV),1)) or (CHILDCUT((PT>200 * MeV),2)))
               & (CHILDCUT((TRCHI2DOF < %(KshhTFKsLLTrkChi2UL)s),1)) 
               & (CHILDCUT((TRCHI2DOF < %(KshhTFKsLLTrkChi2UL)s),2)) """% self.getProps()
        from Hlt2SharedParticles.Ks import KsLLTF
        KSLLforD02HHKs = Hlt2Member( FilterDesktop # type
                               , "KsLLforD02HHKs"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code =KSLL_FilterCuts
                               , Inputs = [KsLLTF]
                      )
        charmKshhTFKsLL = bindMembers( "charmKshhTFKsLL", [ PV3D(), KsLLTF, KSLLforD02HHKs ] )
        print "mds:   charmKshhTFKsLL = ",  charmKshhTFKsLL

        ########
        # KsDD  
        #######
## mds               & (CHILDCUT((PT>300 * MeV),1))    
## mds               & (CHILDCUT((PT>300 * MeV),2))
## mds                       & (BPVVDCHI2 > %(KshhTFKsDDVtxPVDispChi2LL)s)  ## not appropriate for DD
        KSDD_FilterCuts= """(ADMASS('KS0')<%(KshhTFKsDDCombSymMassWin)s) 
                     & (BPVVDZ > 400)
                     & (CHILDCUT((MIPCHI2DV(PRIMARY) > 9. ),1))
                     & (CHILDCUT((MIPCHI2DV(PRIMARY) > 9. ),2))
                     & ((CHILDCUT((PT>200 * MeV),1)) or (CHILDCUT((PT>200 * MeV),2)))
                     & (VFASPF(VCHI2)<%(KshhTFKsDDVtxChi2UL)s) """% self.getProps()

        from Hlt2SharedParticles.Ks import KsDD
        KsDDforD02HHKs = Hlt2Member( FilterDesktop # type
                               , "KsDDforD02HHKs"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = KSDD_FilterCuts
                               , Inputs = [KsDD]
                      )
        charmKshhTFKsDD = bindMembers( "D02HHKsKsDD", [ PV3D(), KsDD, KsDDforD02HHKs ] )
        print "mds:   charmKshhTFKsDD = ",  charmKshhTFKsDD

##  code modified to use an asymmetric window for th KsPiPi hypothesis to accept
##  KsKK, KsKPi, KsPiK, an KsKK candidates. Numerical values here correspond to
##  (1865 +- 100) MeV for th KsPiPi hypothesis and another 130 MeV down to
##  include the KsKK lower limit.  The corresponding MotherCut values are
##  (1865 +- 70) MeV for KsPiPi and another 130 MeV  lower to accept KsKK.

## mds 20120216                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhTFDwKsLLSymMassWin)s)" % self.getProps() } 
        combineKshhTFD2HHKsLL = self.__KshhTFDCombine(ksLLName
                                        , [charmKshhTFKsLL, charmKshhTF2Body]
                                        , extracuts = { 'CombinationCut' : "(AM < 1965.0*MeV) & (AM > 1635.0*MeV)" }
                                        )

        ksLLTOSName = ksLLName
        combineKshhTFD2HHKsLLToS = self.__filterRequiringTOS(ksLLTOSName, combineKshhTFD2HHKsLL )

##  code modified to use an asymmetric window for th KsPiPi hypothesis to accept
##  KsKK, KsKPi, KsPiK, an KsKK candidates. Numerical values here correspond to
##  (1865 +- 120) MeV for th KsPiPi hypothesis and another 130 MeV down to
##  include the KsKK lower limit.  The corresponding MotherCut values are
##  (1865 +- 70) MeV for KsPiPi and another 130 MeV  lower to accept KsKK.

## mds 20120216                                        , extracuts = { 'CombinationCut' : "(ADAMASS('D0')< %(KshhTFDwKsDDSymMassWin)s)" % self.getProps() } 
        combineKshhTFD2HHKsDD = self.__KshhTFDCombine(ksDDName
                                        , [charmKshhTF2BodyTighterReqTOS, charmKshhTFKsDD]
                                        , extracuts = { 'CombinationCut' : "(AM < 1985.0*MeV) & (AM > 1615.0*MeV)" }
                                        )


        ## Make the lines
        ###################################################################
        print "mds: about to __makeLine, ksLLName and ksDDName  =  ", ksLLName,  "  ", ksDDName
        self.__makeLine(ksLLName, algos = [ combineKshhTFD2HHKsLLToS ])
        self.__makeLine(ksDDName, algos = [ combineKshhTFD2HHKsDD ])


    # }
