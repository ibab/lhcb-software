__author__  = 'Grant McGregor'
__date__    = '$Date: 2011-03-15 13:53:00 $'
__version__ = '$Revision: 1.0 $'

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## decays of D0 to semi-leptonic modes
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2CharmSemilepD02HMuNuLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    __slots__ = {
                ## Cut values for D0 -> KMuNu
                  'Muon_PT_MIN'               : 600.0 * MeV
                , 'Kaon_PT_MIN'               : 800.0 * MeV  
                , 'Muon_P_MIN'                : 3.6  * GeV
                , 'Trk_MIPCHI2DV_MIN'         : 5.0        # no units
                , 'Trk_TRCHI2DOF_MAX'         : 3.0        # no units
                , 'Pair_AMAXDOCA_MAX'         : 0.08 * mm
                , 'Pair_Mass_MAX'             : 2000.0 * MeV
                , 'Pair_SumAPT_MIN'           : 2000.0 * MeV  #2800.0
                , 'D0_BPVDIRA_MIN'            : 0.9995    # neuter
                , 'D0_VCHI2PDOF_MAX'          : 10.0       # neuter
		, 'D0_MIPDV_MAX'              : 0.2 * mm  
                , 'D0_P_MIN'                  : 18.0 * GeV
                , 'D0_MCORR_MIN'              : 1400.0 * MeV
                , 'D0_MCORR_MAX'              : 2700.0 * MeV ###2600.0
                , 'D0_BPVVDZ_MIN'             : 0.0 * mm    
                ## GEC
                , 'GEC_Filter_NTRACK'        : False       # do or do not, there is no try...
                , 'GEC_NTRACK_MAX'           : 350        # max number of tracks
                , 'TisTosParticleTaggerSpecs': { "Hlt1TrackAllL0Decision%TOS":0,
                                                 "Hlt1TrackMuonDecision%TOS":0 }	
		, 'L0FILTER'                 : "Muon,Hadron"
                , 'name_prefix'              : 'CharmSemilepD02HMuNu'
                , 'Prescale'         : { }
                , 'Postscale'        : { 'Hlt2CharmSemilepD02HMuNu_D02KMuNu'     : 1.0
		                       , 'Hlt2CharmSemilepD02HMuNu_D02KMuNuWS'   : 0.1
                                       }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : { ## Signal line
                                     'Hlt2CharmSemilepD02HMuNu_D02KMuNuDecision'     : 50860  
				     ## Wrong-sign line
                                   , 'Hlt2CharmSemilepD02HMuNu_D02KMuNuWSDecision'   : 50861  
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
        l0filter = self.getProp("L0FILTER").split(',')

        from HltLine.HltLine import Hlt2Line
        line = Hlt2Line(lineName
	                , L0DU = "|".join(["L0_CHANNEL('%s')" % channel for channel in l0filter])
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

        combcuts = "(AM< %(Pair_Mass_MAX)s )" \
		   "& ((APT1+APT2)> %(Pair_SumAPT_MIN)s)" \
                   "& (AP > %(D0_P_MIN)s)" \
	           "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMAXDOCA_MAX)s )" \
		   "& (AALLSAMEBPV)" \
                   % self.getProps()

        # extracuts allows additional cuts to be applied for special cases
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = "(in_range(%(D0_MCORR_MIN)s,BPVCORRM,%(D0_MCORR_MAX)s))" \
                     "& (BPVDIRA > %(D0_BPVDIRA_MIN)s )" \
		     "& (MIPDV(PRIMARY) > %(D0_MIPDV_MAX)s )"\
                     "& (VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                     "& (BPVVDZ> %(D0_BPVVDZ_MIN)s )" \
                     % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineTwoBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , Inputs = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )

        combSeq = bindMembers( name, inputSeq + [ combineTwoBody ] )
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
                             , PreMonitor  =  Hlt2Monitor( "M","M(K#mu)",1400.,500,'M_in',nbins=101) 
                             , PostMonitor =  Hlt2Monitor( "M","M(K#mu)",1400.,500,'M_out',nbins=101)   
                           )
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }

    def __filterHlt1TOS(self, name, input) : # {
	'''Require HLT1TrackMuon or HLT1TrackAllL0'''
        from HltLine.HltLine import bindMembers
        from Configurables import TisTosParticleTagger
	filterTOS = TisTosParticleTagger('Hlt2'+name+"Hlt1TOSFilter")
	filterTOS.TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
	filterTOS.Inputs = [ input.outputSelection() ]
	return bindMembers(name, [ input, filterTOS ])
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
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter( filtName , Code = filtCode )
	Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )
        return Hlt2CharmKillTooManyInTrk
    # }


    def __inPartFilter(self, name, inputContainers, forMuon=False) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
 
        if forMuon:
            incuts = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
	             "& ISMUON & (PT> %(Muon_PT_MIN)s)" \
		     "& (P> %(Muon_P_MIN)s)" \
                     "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )" % self.getProps()
        else:
            incuts = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
	             "& (PT> %(Kaon_PT_MIN)s)" \
                     "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction 
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
    # }

    def __apply_configuration__(self) :

        ## Input particles
        ### ###############################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import (BiKalmanFittedKaons,
		                                                   BiKalmanFittedMuons)
        muonName = self.getProp('name_prefix') + 'Muons'
	kaonName = self.getProp('name_prefix') + 'Kaons'
	lclInputKaons = self.__inPartFilter(kaonName, [ BiKalmanFittedKaons], False )
        lclInputMuons = self.__inPartFilter(muonName, [ BiKalmanFittedMuons], True )

        ## D0 -> mu+ K- line and wrong-sign line
        ### ###############################################################
        decayModes = {   'D02KMuNu'   : {'descriptor' :  "[D0 -> mu+ K-]cc"
	                                 , 'inList' : [ lclInputMuons, lclInputKaons ] }
	               , 'D02KMuNuWS'  : {'descriptor' :  "[D0 -> mu+ K+]cc"
                                  	 , 'inList' : [ lclInputMuons, lclInputKaons ] }
	}

        for mode in decayModes.keys() : # {
            ## Combinatorics
	    modeName = self.getProp('name_prefix') + '_' + mode
            d02KMuNuComb = self.__combine(  name = modeName
                                , inputSeq = decayModes[mode]['inList']
                                , decayDesc = [ decayModes[mode]['descriptor'] ]
                           )
            d02KMuNuTOS = self.__filterHlt1TOS(modeName, d02KMuNuComb)

            ## Signal window filter
            d02KMuNuSigSeq = self.__filter( name = modeName
                                     , inputSeq = [ d02KMuNuTOS ]
                                     , extracode = ""
                           )

            ## Signal window line
            d02KMuNuSigLine = self.__makeLine(modeName, algos = [ d02KMuNuSigSeq ])

        # }
