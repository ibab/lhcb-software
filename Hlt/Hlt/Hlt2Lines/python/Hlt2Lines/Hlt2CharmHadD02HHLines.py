__author__  = 'Patrick Spradlin'
__date__    = '$Date: 2010-09-09 15:20:09 $'
__version__ = '$Revision: 1.22 $'

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## decays of D0 to two final state hadrons.
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, GeV, mm



class Hlt2CharmHadD02HHLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    # Don't touch my variables!
    __slots__ = {
                ## Cut values for basic D0 -> hh signal lines
                  'Trk_PT_MIN'               : 800.0 * MeV
                , 'Trk_P_MIN'                : 5.0  * GeV
                , 'Trk_MIPCHI2DV_MIN'        : 2.0        # neuter
                , 'Trk_TRCHI2DOF_MAX'        : 3.0        # neuter
                , 'Pair_AMINDOCA_MAX'        : 0.10 * mm
                , 'Trk_Max_APT_MIN'          : 1500.0 * MeV
                , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                , 'D0_BPVDIRA_MIN'           : 0.99985    # neuter
                , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                , 'D0_PT_MIN'                : 2000.0 * MeV
                , 'Sig_M_MIN'                : 1815.0 * MeV
                , 'Sig_M_MAX'                : 1915.0 * MeV
                , 'WideMass_M_MIN'           : 1715.0 * MeV
                , 'WideMass_M_MAX'           : 2015.0 * MeV
                ## 2-body inclusive!
                , 'Inc_Trk_PT_MIN'           : 500.0 * MeV
                , 'Inc_Trk_P_MIN'            : 5.0 * GeV
                , 'Inc_Trk_MIPCHI2DV_MIN'    : 16.0       # neuter
                , 'Inc_Trk_TRCHI2DOF_MAX'    : 3.0        # neuter
                , 'Inc_Pair_AMINDOCA_MAX'    : 0.15 * mm
                , 'Inc_D0_BPVVDCHI2_MIN'     : 100.0      # neuter
                , 'Inc_Trk_Max_APT_MIN'      : 1000.0 * MeV
                , 'Inc_D0_APT_MIN'           : 1500.0 * MeV
                , 'Inc_D0_SumAPT_MIN'        : 1800.0 * MeV
                , 'Inc_D0_BPVIPCHI2_MIN'     : 2.0        # neuter
                , 'Inc_D0_BPVCORRM_MIN'      : 300.0 * MeV
                , 'Inc_D0_BPVCORRM_MAX'      : 3200.0 * MeV
                ## GEC
                , 'GEC_Filter_NTRACK'        : True       # do or do not
                , 'GEC_NTRACK_MAX'           : 180        # max number of tracks
                , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                , 'name_prefix'              : 'CharmHadD02HH'
                , 'Prescale'         : { }
                , 'Postscale'        : { 'Hlt2CharmHadD02HH_D02KKWideMass'     : 0.10
                                         , 'Hlt2CharmHadD02HH_D02KPiWideMass'  : 0.10
                                         , 'Hlt2CharmHadD02HH_D02PiPiWideMass' : 0.10
                                       }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : { ## Signal lines
                                     'Hlt2CharmHadD02HH_D02KKDecision'     : 50880
                                   , 'Hlt2CharmHadD02HH_D02KPiDecision'    : 50881
                                   , 'Hlt2CharmHadD02HH_D02PiPiDecision'   : 50882
                                   ## Inclusive lines
                                   , 'Hlt2CharmHadD02HH2BodyIncDecision'  : 50870 
                                   ## Wide mass lines
                                   , 'Hlt2CharmHadD02HH_D02KKWideMassDecision'   : 50890
                                   , 'Hlt2CharmHadD02HH_D02KPiWideMassDecision'  : 50891
                                   , 'Hlt2CharmHadD02HH_D02PiPiWideMassDecision' : 50892
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
        lclAlgos = []
        ## Prepend a filter on the number of tracks, if required.
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()
        lclAlgos = [ Hlt2CharmKillTooManyInTrk ]
        lclAlgos.extend(algos)

        from HltLine.HltLine import Hlt2Line
        line = Hlt2Line(lineName
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = lclAlgos
                       )
        self.__updateHltANNSvc(lineName)
    # }


    def __combine(self, name, inputSeq, decayDesc, extracuts = None) : # {
        "Configure combinatorics.  Returns a bindMembers."
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        ## Mass window for shared combinatorics
        massmin = min(self.getProp('Sig_M_MIN'), self.getProp('WideMass_M_MIN'))
        massmax = max(self.getProp('Sig_M_MAX'), self.getProp('WideMass_M_MAX'))

        masscut = "in_range(%s,  AM, %s)" % (massmin, massmax)

        combcuts = masscut + \
                   "& ((APT1 > %(Trk_Max_APT_MIN)s) " \
                       "| (APT2 > %(Trk_Max_APT_MIN)s))" \
                   "& (APT > %(D0_PT_MIN)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') " \
                       "< %(Pair_AMINDOCA_MAX)s )" \
                   % self.getProps()


        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                     "& (BPVVDCHI2> %(D0_BPVVDCHI2_MIN)s )" \
                     "& (BPVDIRA > %(D0_BPVDIRA_MIN)s )" \
                     % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineTopoNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , Inputs = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )

        combSeq = bindMembers( name, inputSeq + [ combineTopoNBody ] )
        return combSeq
    # }


    def __filter(self, name, inputSeq, extracode = None) : # {
        "Filter combinatorics.  Returns a bindMembers."
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltLine.Hlt2Monitoring import Hlt2Monitor

        codestr = "(ALL)"
        
        if extracode :
          codestr = extracode + '&' + codestr
        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , Inputs = inputSeq
                             , Code = codestr
                             , PreMonitor  =  Hlt2Monitor( "M","M(K#pi)",1865.,50,'M_in',nbins=101) 
                             , PostMonitor =  Hlt2Monitor( "M","M(K#pi)",1865.,50,'M_out',nbins=101)   
                           )
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }


    def __filterHlt1TOS(self, name, input) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import TisTosParticleTagger

        filterTOS = Hlt2Member( TisTosParticleTagger
                                , 'Hlt1TOSFilter'
                                , Inputs = [ input.outputSelection() ]
                                , TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
                              )

        return bindMembers(name, [ input, filterTOS ])
    # }


    def __combineInc(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        Configure combinatorics for inclusive selection.  
        Returns a bindMembers.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        ## Daughter cuts
        daugCuts = "(TRCHI2DOF< %(Inc_Trk_TRCHI2DOF_MAX)s)" \
                   "& (PT> %(Inc_Trk_PT_MIN)s)" \
                   "& (P> %(Inc_Trk_P_MIN)s)" \
                   "& (MIPCHI2DV(PRIMARY)> %(Inc_Trk_MIPCHI2DV_MIN)s)" % self.getProps()

        # Construct a cut string for the combination.
        combcuts = "(AM<2000*MeV)" \
                   "& ((APT1 > %(Inc_Trk_Max_APT_MIN)s) " \
                       "| (APT2 > %(Inc_Trk_Max_APT_MIN)s))" \
                   "& (APT > %(Inc_D0_APT_MIN)s)" \
                   "& (APT1 + APT2 > %(Inc_D0_SumAPT_MIN)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') " \
                        "< %(Inc_Pair_AMINDOCA_MAX)s )" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = "(BPVVDCHI2 > %(Inc_D0_BPVVDCHI2_MIN)s )" \
                     "& (in_range(%(Inc_D0_BPVCORRM_MIN)s,BPVCORRM,%(Inc_D0_BPVCORRM_MAX)s))" \
                     "& (BPVIPCHI2() > %(Inc_D0_BPVIPCHI2_MIN)s)" % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineTopoNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , Inputs = inputSeq
                                       , DaughtersCuts = { "pi+" : daugCuts }
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )    

        combSeq = bindMembers( name, inputSeq + [ combineTopoNBody ] )
        return combSeq
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
        if self.getProp('GEC_Filter_NTRACK') : # {
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()
        # }

        filtName = 'Hlt2' + self.getProp('name_prefix')  + 'KillTooManyInTrkAlg'
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter( filtName
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __inPartFilter(self, name, inputContainers) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
                 "& (PT> %(Trk_PT_MIN)s)" \
                 "& (P> %(Trk_P_MIN)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
    # }



    def __apply_configuration__(self) :

        ## Input particles
        ### ###############################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        pionName = self.getProp('name_prefix') + 'Pions'
        kaonName = self.getProp('name_prefix') + 'Kaons'
        lclInputKaons = self.__inPartFilter(kaonName, [ BiKalmanFittedKaons] )
        lclInputPions = self.__inPartFilter(pionName, [ BiKalmanFittedPions] )


        ## D0 -> h- h+ lines
        ### ###############################################################
        decayModes = {   'D02KK'   : { 'descriptor' : "D0 -> K- K+"
                                        , 'inList' : [ lclInputKaons ] }
                       , 'D02KPi'  : { 'descriptor' : "[D0 -> K- pi+]cc"
                                        , 'inList' : [ lclInputKaons, lclInputPions ] }
                       , 'D02PiPi' : { 'descriptor' : "D0 -> pi- pi+"
                                        , 'inList' : [lclInputPions] }
                     }

        sigMassCut  = "in_range(%s, M, %s)" \
                      % (self.getProp('Sig_M_MIN'), self.getProp('Sig_M_MAX'))

        wideMassCut = "in_range(%s, M, %s)" \
                      % (self.getProp('WideMass_M_MIN'), \
                         self.getProp('WideMass_M_MAX'))


        for mode in decayModes.keys() : # {
            ## Combinatorics
            modeName = self.getProp('name_prefix') + '_' + mode
            d02hhComb = self.__combine(  name = modeName
                                , inputSeq = decayModes[mode]['inList']
                                , decayDesc = [ decayModes[mode]['descriptor'] ]
                           )

            d02hhTOS = self.__filterHlt1TOS(modeName, d02hhComb)

            ## Signal window filter
            d02hhSigSeq = self.__filter( name = modeName
                                     , inputSeq = [ d02hhTOS ]
                                     , extracode = sigMassCut
                           )

            ## Signal window line
            d02hhSigLine = self.__makeLine(modeName, algos = [ d02hhSigSeq ])


            ## Wide mass window filter
            wmModeName = modeName + 'WideMass'
            d02hhWMSeq = self.__filter( name = wmModeName
                                     , inputSeq = [ d02hhTOS ]
                                     , extracode = wideMassCut
                           )

            ## Wide mass window line
            d02hhSigLine = self.__makeLine(wmModeName, algos = [ d02hhWMSeq ])

        # }






        ### ###############################################################
        ## 2-body inclusive line
        ### ###############################################################
        inclName = self.getProp('name_prefix') + '2BodyInc'

        charmIncSeq = self.__combineInc(  name = inclName
                                , inputSeq = [ BiKalmanFittedPions ]
                                , decayDesc = [  "D0 -> pi+ pi-"
                                               , "D0 -> pi+ pi+"
                                               , "D0 -> pi- pi-"]
                               )

        ## Make the inclusive line!
        self.__makeLine(inclName, algos=[charmIncSeq])

