__author__  = 'Patrick Spradlin'
__date__    = '$Date: 2010-09-09 15:20:09 $'
__version__ = '$Revision: 1.22 $'

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## decays of D0 to two final state hadrons.
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from HltLine.HltLine import Hlt2Line


class Hlt2CharmHadD02HHLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    # Don't touch my variables!
    __slots__ = {
                ## Cut values for basic D0 -> hh signal lines
                  'Trk_PT_MIN'               : 800.0      # MeV
                , 'Trk_P_MIN'                : 5000.0     # MeV
                , 'Trk_MIPCHI2DV_MIN'        : 2.0        # neuter
                , 'Trk_TRCHI2DOF_MAX'        : 5.0        # neuter
                , 'Pair_AMINDOCA_MAX'        : 0.10       # mm
                , 'Trk_Max_APT_MIN'          : 1500.0     # MeV
                , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                , 'D0_BPVDIRA_MIN'           : 0.99985    # neuter
                , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                , 'D0_PT_MIN'                : 2000.0     # MeV
                , 'Sig_M_MIN'                : 1839.0     # MeV
                , 'Sig_M_MAX'                : 1889.0     # MeV
                , 'WideMass_M_MIN'           : 1700.0     # MeV
                , 'WideMass_M_MAX'           : 2100.0     # MeV
                ## 2-body inclusive!
                , 'Inc_Trk_PT_MIN'           : 500.0      # MeV
                , 'Inc_Trk_P_MIN'            : 5000.0     # MeV
                , 'Inc_Trk_MIPCHI2DV_MIN'    : 16.0       # neuter
                , 'Inc_Trk_TRCHI2DOF_MAX'    : 3.0        # neuter
                , 'Inc_Pair_AMINDOCA_MAX'    : 0.15       # in mm
                , 'Inc_D0_BPVVDCHI2_MIN'     : 100.0      # neuter
                , 'Inc_Trk_Max_APT_MIN'      : 1000.0     # MeV
                , 'Inc_D0_PT_MIN'            : 1500.0     # MeV
                , 'Inc_D0_SUMPT_MIN'         : 1800.0     # MeV
                , 'Inc_D0_BPVIPCHI2_MIN'     : 2.0        # neuter
                , 'Inc_D0_MCOR_MIN'          : 300.       # MeV
                , 'Inc_D0_MCOR_MAX'          : 3200.      # MeV
                ## GEC
                , 'GEC_Filter_NTRACK'        : True       # do or do not
                , 'GEC_NTRACK_MAX'           : 120        # max number of tracks
                , 'Prescale'         : { }
                , 'Postscale'        : { 'Hlt2CharmHadD02KKWideMass'     : 0.05
                                         , 'Hlt2CharmHadD02KPiWideMass'  : 0.05
                                         , 'Hlt2CharmHadD02PiPiWideMass' : 0.05
                                       }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : { ## Signal lines
                                     'Hlt2CharmHadD02KKDecision'     : 50880
                                   , 'Hlt2CharmHadD02KPiDecision'    : 50881
                                   , 'Hlt2CharmHadD02PiPiDecision'   : 50882
                                   ## Inclusive lines
                                   , 'Hlt2CharmHad2BodyIncDecision'  : 50870 
                                   ## Wide mass lines
                                   , 'Hlt2CharmHadD02KKWideMassDecision'   : 50890
                                   , 'Hlt2CharmHadD02KPiWideMassDecision'  : 50891
                                   , 'Hlt2CharmHadD02PiPiWideMassDecision' : 50892
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

        masscut = "(%s*MeV < AM) & (AM < %s*MeV)" % (massmin, massmax)

        combcuts = masscut + \
                   "& ((APT1 > %(Trk_Max_APT_MIN)s) " \
                       "| (APT2 > %(Trk_Max_APT_MIN)s))" \
                   "& (APT > %(D0_PT_MIN)s)" \
                   "& (AALLSAMEBPV)" \
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
                                       , InputLocations = inputSeq
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
                             , InputLocations = inputSeq
                             , Code = codestr
                             , PreMonitor  =  Hlt2Monitor( "M","M(K#pi)",1865.,50,'M_in',nbins=101) 
                             , PostMonitor =  Hlt2Monitor( "M","M(K#pi)",1865.,50,'M_out',nbins=101)   
                           )
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }


    def __combineInc(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        Configure combinatorics for inclusive selection.  
        Returns a bindMembers.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Construct a cut string for the combination.
        combcuts = "(AM<2000*MeV) & (AALLSAMEBPV) " \
                   "& ((APT1 > %(Inc_Trk_Max_APT_MIN)s) " \
                       "| (APT2 > %(Inc_Trk_Max_APT_MIN)s))" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') " \
                        "< %(Inc_Pair_AMINDOCA_MAX)s )" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = "(BPVVDCHI2> %(Inc_D0_BPVVDCHI2_MIN)s )" % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineTopoNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , InputLocations = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )    

        combSeq = bindMembers( name, inputSeq + [ combineTopoNBody ] )
        return combSeq
    # }

    def __filterInc(self, name, inputSeq, extracode = None) : # {
        """
        Filter combinatorics for inclusive selection.  
        Returns a bindMembers.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                     "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]

        codestr = "(PT > %(Inc_D0_PT_MIN)s)" \
                  "& (SUMTREE(PT,('pi+'==ABSID),0.0) > %(Inc_D0_SUMPT_MIN)s)" \
                  "& (in_range(%(Inc_D0_MCOR_MIN)s*MeV,MCOR,%(Inc_D0_MCOR_MAX)s*MeV))" \
                  "& (BPVIPCHI2() > %(Inc_D0_BPVIPCHI2_MIN)s)" \
                  % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = inputSeq
                             , Code = codestr
                             , Preambulo=preambulo 
                           )    
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
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

        from HltTracking.Hlt2TrackingConfigurations import Hlt2UnfittedForwardTracking
        tracks = Hlt2UnfittedForwardTracking().hlt2PrepareTracks()

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('GEC_Filter_NTRACK') : # {
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()
        # }
            
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter('Hlt2CharmKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __inPartFilter(self, name, inputContainers) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(PT> %(Trk_PT_MIN)s *MeV)" \
                 "& (P> %(Trk_P_MIN)s *MeV)" \
                 "& (TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
                 "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )" \
                 % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
    # }

    def __inPartFilterInc(self, name, inputContainers) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D 

        incuts = """(PT> %(Inc_Trk_PT_MIN)s *MeV)
                    & (P> %(Inc_Trk_P_MIN)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(Inc_Trk_MIPCHI2DV_MIN)s )
                    & (TRCHI2DOF< %(Inc_Trk_TRCHI2DOF_MAX)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
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
        lclInputKaons = self.__inPartFilter('CharmHadD02HHKaons', [ BiKalmanFittedKaons] )
        lclInputPions = self.__inPartFilter('CharmHadD02HHPions', [ BiKalmanFittedPions] )


        ## D0 -> h- h+ lines
        ### ###############################################################
        decayModes = {   'CharmHadD02KK'   : "D0 -> K- K+"
                       , 'CharmHadD02KPi'  : "[D0 -> K- pi+]cc"
                       , 'CharmHadD02PiPi' : "D0 -> pi- pi+"
                     }

        sigMassCut  = "in_range(%s*MeV, M, %s*MeV)" \
                      % (self.getProp('Sig_M_MIN'), self.getProp('Sig_M_MAX'))

        wideMassCut = "in_range(%s*MeV, M, %s*MeV)" \
                      % (self.getProp('WideMass_M_MIN'), \
                         self.getProp('WideMass_M_MAX'))


        for modeName in decayModes.keys() : # {
            ## Combinatorics
            d02hhComb = self.__combine(  name = modeName
                                , inputSeq = [ lclInputKaons, 
                                               lclInputPions ]
                                , decayDesc = [ decayModes[modeName] ]
                           )

            ## Signal window filter
            d02hhSigSeq = self.__filter( name = modeName
                                     , inputSeq = [ d02hhComb ]
                                     , extracode = sigMassCut
                           )

            ## Signal window line
            d02hhSigLine = self.__makeLine(modeName, algos = [ d02hhSigSeq ])


            ## Wide mass window filter
            wmModeName = modeName + 'WideMass'
            d02hhWMSeq = self.__filter( name = wmModeName
                                     , inputSeq = [ d02hhComb ]
                                     , extracode = wideMassCut
                           )

            ## Wide mass window line
            d02hhSigLine = self.__makeLine(wmModeName, algos = [ d02hhWMSeq ])

        # }






        ### ###############################################################
        ## 2-body inclusive line
        ### ###############################################################
        lclInputPionsInc = self.__inPartFilterInc('CharmHadD02HHIncPions',
                                                  [ BiKalmanFittedPions] )

        charmInc = self.__combineInc(  name = 'CharmHad2BodyInc'
                                , inputSeq = [ lclInputPionsInc ]
                                , decayDesc = [  "D0 -> pi+ pi-"
                                               , "D0 -> pi+ pi+"
                                               , "D0 -> pi- pi-"]
                               )

        charmIncSeq = self.__filterInc('CharmHad2BodyInc'
                                     , [charmInc]
                                    )

        ## Make the inclusive line!
        self.__makeLine("CharmHad2BodyInc",algos=[charmIncSeq])

