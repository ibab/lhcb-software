__author__  = 'Mika Vesterinen'
__date__    = '$Date: $'
__version__ = '$Revision: $'
## based on the HHKs lines

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## hadronic decays of charm hadrons.
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, mm
from HltLine.Hlt2Monitoring import Hlt2Monitor

class Hlt2CharmHadD02HHPi0LinesConf(HltLinesConfigurableUser) :

    __slots__ = {
        "UseGEC" : True
        ,"GEC" : 150 
        ,"HH_DaugTrChi2Max" : 3.0
        ,"HH_DaugPtMin" : 300*MeV
        ,"HH_DaugMinIPChi2" : 6.0
        ,"HH_MassMax" : 1865 *MeV
        ,"HH_MaxDoca" : 0.08 *mm
        ,"HH_MaxVtxChi2"  : 20.0
        ,"HH_MinVVDChi2"  : 25.0
        ,"HH_PT_MIN"      : 1000.0 * MeV
        ,"D0_PT_MIN"      : 2500.0 * MeV
        ,"D0_DMASS_MAX"   : 120 * MeV
        ,"D0_ADMASS_MAX"  : 130 * MeV
        ,"D0_MaxIPChi2"   : 50.0 
        ,"D0_MinDIRA"     : 0.9995
        ,"D0_TAU"         : 0.2  #ps
        ,"Pi0R_DMASS_MAX" : 30.0  * MeV
        ,"Pi0R_PT_MIN"    : 500.0 * MeV
        ,"HLT1FILTER"     : ""
        ,"name_prefix"              : "CharmHadD02HHPi0"
        ,"Prescale"             : { "Hlt2CharmHadD02HHPi0_ResolvedDecision"     : 1.0}
        ,"Postscale"            : { "Hlt2CharmHadD02HHPi0_ResolvedDecision"     : 1.0}
        ,"HltANNSvcID"          : { "Hlt2CharmHadD02HHPi0_ResolvedDecision"     : 50930}
        }


    def __updateHltANNSvc(self,line) : 
        """
        Wrapper for updating the HltANNSvc after a new line has been
        constructed.  This should eventually become obsolete.
        """
        from Configurables import HltANNSvc
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { lineName : id } )

    def __makeLine(self, lineName, algos) : 
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


    def __seqGEC(self) :
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
        if self.getProp('UseGEC') : 
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC)s" % self.getProps()
            
        filtName = 'Hlt2' + self.getProp('name_prefix')  + 'KillTooManyInTrkAlg'
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter(filtName
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk

    def __Pi0hhTFHHInPartFilter(self, name, inputContainers) : 
        """
        # Function to configure a filter for the input particles of the
        #   of the 2-body reconstruction for Pi0hh.  It lashes the new
        #   FilterDesktop to a bindMembers with its antecedents.
        # The argument inputContainer should be 
        #   a list of bindMember sequences that produces the particles
        #   to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF < %(HH_DaugTrChi2Max)s ) & (PT> %(HH_DaugPtMin)s) & (MIPCHI2DV(PRIMARY)>%(HH_DaugMinIPChi2)s)" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Remember to require the PV3D reconstruction before cutting on IP.
        filterSeq = bindMembers( name, inputContainers + [PV3D(), filter] )

        return filterSeq


    def __Pi0hhTFDCombine(self, name, inputSeq, decayDesc = [ "D0 -> K*(892)0 pi0" ], extracuts = None) : 
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

        HHPi0_combcuts =  "(ADAMASS('D0') < %(D0_ADMASS_MAX)s) & (APT > %(D0_PT_MIN)s)" %self.getProps()
        HHPi0_combcuts += "& (((ACHILD(PT,1) > 1.7*GeV) & (ACHILD(BPVIPCHI2(),1) > 36)) | ((ACHILD(PT,2) > 1.7*GeV) & (ACHILD(BPVIPCHI2(),2) > 36)))"
        HHPi0_parentcuts = "(DMASS('D0') < %(D0_DMASS_MAX)s)"\
            "& (BPVIPCHI2()<%(D0_MaxIPChi2)s) & (BPVDIRA > %(D0_MinDIRA)s)"\
            "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(D0_TAU)s *ps)" %self.getProps()


        combineNBody = Hlt2Member( CombineParticles
                                   , 'Combine'
                                   , DecayDescriptors = decayDesc
                                   , Inputs = inputSeq
                                   , CombinationCut = HHPi0_combcuts
                                   , MotherCut = HHPi0_parentcuts
                                   , ParticleCombiners = {'' : 'TrgVertexFitter'}
                                   )

        
        
        charmNBody = bindMembers( name, inputSeq + [ PV3D(), combineNBody ] )
        return charmNBody

    def __filter(self, name, inputSeq, extracode = None) : 
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
                             , PreMonitor  =  Hlt2Monitor( "M","M(K#pi)",1865.,150,'M_in',nbins=50)
                             , PostMonitor =  Hlt2Monitor( "M","M(K#pi)",1865.,150,'M_out',nbins=50)
                             )
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
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
    def __apply_configuration__(self) : 
        
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from HltTracking.HltPVs import PV3D

        ## Input particles for 2-body HH part of Pi0HH
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons
        inPionsName = self.getProp('name_prefix') + 'InputPions'
        inKaonsName = self.getProp('name_prefix') + 'InputKaons'
        lclPi0hhTFInputKaons = self.__Pi0hhTFHHInPartFilter(inKaonsName, [ BiKalmanFittedKaons] )
        lclPi0hhTFInputPions = self.__Pi0hhTFHHInPartFilter(inPionsName, [ BiKalmanFittedPions] )

        ## 2-track vertex
        HH_combcuts = "(AM < %(HH_MassMax)s) & (APT > %(HH_PT_MIN)s) & (AMAXDOCA('LoKi::DistanceCalculator')<%(HH_MaxDoca)s) & (AALLSAMEBPV)" % self.getProps()
        HH_parentcuts = "(VFASPF(VCHI2)<%(HH_MaxVtxChi2)s) & (BPVVDCHI2>%(HH_MinVVDChi2)s)" % self.getProps()
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        combinePi0hhTF2Body = Hlt2Member( CombineParticles
                                          , 'Combine'
                                          #, DecayDescriptors = [ "K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-" ]
                                          , DecayDescriptors = [ "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-" ]
                                          , Inputs = [ lclPi0hhTFInputKaons, lclPi0hhTFInputPions ]
                                          , CombinationCut = HH_combcuts
                                          , MotherCut = HH_parentcuts
                                          )

        from HltTracking.HltPVs import PV3D
        twoBodyName = self.getProp('name_prefix') + '2Body'
        charmPi0hhTF2Body = bindMembers( twoBodyName,  [PV3D(),lclPi0hhTFInputKaons,  lclPi0hhTFInputPions, combinePi0hhTF2Body ] )

        ## Filter the pi0s
        from Hlt2SharedParticles.Pi0 import MergedPi0s,ResolvedPi0s
        Pi0R_FilterCuts= "(ADMASS('pi0')<%(Pi0R_DMASS_MAX)s) & (PT > %(Pi0R_PT_MIN)s)" %self.getProps()
        #Pi0R_FilterCuts= "(PT > %(Pi0R_PT_MIN)s)" %self.getProps()
        Pi0R_forD02HHPi0 = Hlt2Member( FilterDesktop # type
                                       , "Pi0RforD02HHPi0"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                       , Code =Pi0R_FilterCuts
                                       , Inputs = [ResolvedPi0s]
                                       )
        charmPi0hhTFPi0R = bindMembers( "charmPi0hhTFPi0R", [ PV3D(), ResolvedPi0s, Pi0R_forD02HHPi0 ] )
        
        ## make the D0->hh pi0 combination
        Pi0R_Name = self.getProp('name_prefix') + '_Resolved'
        combinePi0hhTFD2HHPi0 = self.__Pi0hhTFDCombine(Pi0R_Name
                                                       , [charmPi0hhTFPi0R, charmPi0hhTF2Body]
                                                       , extracuts = {}
                                                       )
        
        d02hhSigSeq = self.__filter( name = "FILTER_"+Pi0R_Name
                                     , inputSeq = [ combinePi0hhTFD2HHPi0 ]
                                     , extracode = "(DMASS('D0') < 200*MeV)"
                                     )

        self.__makeLine(Pi0R_Name, algos = [ d02hhSigSeq])
    ## Make the lines
        ###################################################################
        #self.__makeLine(Pi0R_Name, algos = [ combinePi0hhTFD2HHPi0,filter ])


