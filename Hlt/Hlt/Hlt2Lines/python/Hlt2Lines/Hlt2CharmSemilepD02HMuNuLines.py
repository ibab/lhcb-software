__author__  = 'Grant McGregor'
__date__    = '$Date: 2011-03-15 13:53:00 $'
__version__ = '$Revision: 1.4 $'

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
                ## Cut values for D0 -> KMuNu, PiMuNu, adapted
                  'Muon_PT_MIN'               : 800.0 * MeV
                , 'Hadron_PT_MIN'             : 600.0 * MeV  
                , 'Trk_TRCHI2DOF_MAX'         : 3.0        # neuter
                , 'Pair_AMAXDOCA_MAX'         : 0.07 * mm
                , 'Pair_Mass_MAX'             : 1900.0 * MeV
                , 'Pair_SumAPT_MIN'           : 2800.0 * MeV  
                , 'Pair_SumAPT_TIGHT_MIN'     : 1500.0 * MeV  
                , 'D0_VCHI2PDOF_MAX'          : 10.0       # neuter
		, 'D0_FD_MIN'                 :  4.0 * mm # for Suzanne to change
		, 'D0_FD_TIGHT_MIN'           : 10.0 * mm
                , 'D0_P_MIN'                  : 20.0 * GeV
                , 'D0_MCORR_MIN'              : 1400.0 * MeV
                , 'D0_MCORR_MAX'              : 2700.0 * MeV 
                , 'D0_BPVVDZ_MIN'             : 0.0 * mm    

                , 'TrkPt_SlowPion'            : 300.0 * MeV
                , 'TrkP_SlowPion'             : 3000.0 * MeV
                , 'TrkChi2_SlowPion'          : 3.0
		, 'PairMaxDoca_Dstar'	      : 120.0 * mm
                , 'DeltaM_MIN'                : 0.0 * MeV
                , 'DeltaM_MAX'                : 250.0 *MeV
                

                ## GEC
                , 'GEC_Filter_NTRACK'        : False       # do or do not, there is no try...
                , 'GEC_NTRACK_MAX'           : 350        # max number of tracks
                , 'TisTosParticleTaggerSpecs': {"Hlt1TrackMuonDecision%TOS":0 }	
		, 'L0FILTER'                 : "Muon,Hadron"
                , 'name_prefix'              : 'CharmSemilepD02HMuNu'
                , 'Prescale'         : { 'Hlt2CharmSemilepD02HMuNu_D02KMuNu'      : 1.0
		                       , 'Hlt2CharmSemilepD02HMuNu_D02KMuNuWS'    : 0.1
		                       , 'Hlt2CharmSemilepD02HMuNu_D02PiMuNu'     : 1.0
		                       , 'Hlt2CharmSemilepD02HMuNu_D02PiMuNuWS'   : 0.1
				       , 'Hlt2CharmSemilepD02HMuNu_D02KMuNuTight' : 1.0
		                       }
                , 'Postscale'        : { 'Hlt2CharmSemilepD02HMuNu_D02KMuNu'      : 1.0
		                       , 'Hlt2CharmSemilepD02HMuNu_D02KMuNuWS'    : 1.0
		                       , 'Hlt2CharmSemilepD02HMuNu_D02PiMuNu'     : 1.0
		                       , 'Hlt2CharmSemilepD02HMuNu_D02PiMuNuWS'   : 1.0
				       , 'Hlt2CharmSemilepD02HMuNu_D02KMuNuTight' : 1.0
                                       }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : { ## Signal line KMuNu
                                     'Hlt2CharmSemilepD02HMuNu_D02KMuNuDecision'       : 50860  
				     ## Wrong-sign line
                                   , 'Hlt2CharmSemilepD02HMuNu_D02KMuNuWSDecision'     : 50861  
				     ## Signal line PiMuNu
                                   , 'Hlt2CharmSemilepD02HMuNu_D02PiMuNuDecision'      : 50862  
				     ## Wrong-sign line
                                   , 'Hlt2CharmSemilepD02HMuNu_D02PiMuNuWSDecision'    : 50863
				     ## Tight KMuNu line
                                   , 'Hlt2CharmSemilepD02HMuNu_D02KMuNuTightDecision'  : 50864
                                     ## Signal line KMuNu with slow pion
                                   , 'Hlt2CharmSemilepD02HMuNu_D02KMuNu_SlowPionDecision'       : 50865  
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
	## Prepend a filter on the number of tracks, if required.
	Hlt2CharmKillTooManyInTrk = self.__seqGEC()
	lclAlgos = [ Hlt2CharmKillTooManyInTrk ]
	lclAlgos.extend(algos)
	l0filter = self.getProp("L0FILTER").split(',')
	#  move PV3D upfront in case it is present
	#  note that any duplication gets automatically removed, so we
	#  keep the original 'as is'
	from HltTracking.HltPVs import PV3D
	pv = PV3D()
	if set(pv.members()).issubset(set([ j for i in algos for j in i.members() ])) :
	    lclAlgos.insert( 0, pv )

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
		     "& (BPVVD > %(D0_FD_MIN)s )" \
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

    def __combineTight(self, name, inputSeq, decayDesc, extracuts = None) : # {
        "Configure combinatorics.  Returns a bindMembers."
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        combcuts = "(AM< %(Pair_Mass_MAX)s )" \
		   "& ((APT1+APT2)> %(Pair_SumAPT_TIGHT_MIN)s)" \
                   "& (AP > %(D0_P_MIN)s)" \
	           "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMAXDOCA_MAX)s )" \
		   "& (AALLSAMEBPV)" \
                   % self.getProps()

        # extracuts allows additional cuts to be applied for special cases
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = "(in_range(%(D0_MCORR_MIN)s,BPVCORRM,%(D0_MCORR_MAX)s))" \
		     "& (BPVVD > %(D0_FD_TIGHT_MIN)s )" \
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

    def __filterHlt1TOS(self, name, input) : # {
	'''Require HLT1TrackMuon or HLT1TrackAllL0'''
        from HltLine.HltLine import bindMembers
        from Configurables import TisTosParticleTagger
	filterTOS = TisTosParticleTagger('Hlt2'+name+"Hlt1TOSFilter")
	filterTOS.TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
	filterTOS.Inputs = [ input.outputSelection() ]
	filterTOS.Output = "/Event/Hlt2/Hlt2"+name+"Hlt1TOSFilter/Particles"
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
	             "& (HASMUON & ISMUON) & (PT> %(Muon_PT_MIN)s)"  % self.getProps()
        else:
            incuts = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
	             "& (PT> %(Hadron_PT_MIN)s)" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction 
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
    # }

    # slow pion filter
    def __SlowPionFilter(self, name, inputContainers) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
       
        incuts = "(TRCHI2DOF< %(TrkChi2_SlowPion)s )" \
                 "& (PT> %(TrkPt_SlowPion)s )" \
                 "& (P> %(TrkP_SlowPion)s )" % self.getProps()
       
        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , Inputs = inputContainers
                             , Code = incuts
                           )

        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )
       
        return filterSeq
    # }

    def __DstarCombine(self, name, inputSeq, decayDesc, masscut) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Dstar)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = masscut
        
        combineDstar = Hlt2Member( CombineParticles
                                   , "Combine_Dstar"
                                   , DecayDescriptors = decayDesc
                                   , Inputs = inputSeq 
                                   , CombinationCut = combcuts
                                   , MotherCut = mothercuts
                                   )
        return bindMembers(name, [PV3D()] + inputSeq + [combineDstar])

    def __apply_configuration__(self) :

        ## Input particles
        ### ###############################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import (BiKalmanFittedKaons,
	                                                           BiKalmanFittedPions,
		                                                   BiKalmanFittedMuons)
        muonName = self.getProp('name_prefix') + 'Muons'
	kaonName = self.getProp('name_prefix') + 'Kaons'
	pionName = self.getProp('name_prefix') + 'Pions'
	lclInputKaons      = self.__inPartFilter(kaonName, [ BiKalmanFittedKaons], False )
        lclInputMuons      = self.__inPartFilter(muonName, [ BiKalmanFittedMuons], True )
	lclInputPions      = self.__inPartFilter(pionName, [ BiKalmanFittedPions], False )

        ## D0 -> mu+ K-/pi- lines and wrong-sign line
        ### ###############################################################
        decayModes = {   'D02KMuNu'   : {'descriptor' :  "[D0 -> mu+ K-]cc"
	                                 , 'inList' : [ lclInputMuons, lclInputKaons ] }
	               , 'D02KMuNuWS'  : {'descriptor' :  "[D0 -> mu+ K+]cc"
                                  	 , 'inList' : [ lclInputMuons, lclInputKaons ] }
	               , 'D02PiMuNu'   : {'descriptor' :  "[D0 -> mu+ pi-]cc"
                                  	 , 'inList' : [ lclInputMuons, lclInputPions ] }
	               , 'D02PiMuNuWS'  : {'descriptor' :  "[D0 -> mu+ pi+]cc"
                                  	 , 'inList' : [ lclInputMuons, lclInputPions ] }
	               , 'D02KMuNuTight'  : {'descriptor' :  "[D0 -> mu+ K-]cc"
                                  	 , 'inList' : [ lclInputMuons, lclInputKaons ] }
	}

        #for mode in decayModes.keys() : # {
        #    ## Combinatorics
	#    modeName = self.getProp('name_prefix') + '_' + mode
        #    
	#    if ( "Tight" in modeName):
	#	    d02HMuNuComb = self.__combineTight(  name = modeName
	#		    , inputSeq = decayModes[mode]['inList']
	#		    , decayDesc = [ decayModes[mode]['descriptor'] ]
	#		    )
	#    else:
        #            d02HMuNuComb = self.__combine(  name = modeName
        #                    , inputSeq = decayModes[mode]['inList']
        #                    , decayDesc = [ decayModes[mode]['descriptor'] ]
        #                    )
        #    d02HMuNuSigSeq  = self.__filterHlt1TOS( modeName, d02HMuNuComb )
        #    d02HMuNuSigLine = self.__makeLine(modeName, algos = [ d02HMuNuSigSeq ])

        # }

	# added for the slow pion filter
        pionsForDstar = self.__SlowPionFilter( name = self.getProp('name_prefix')+'SlowPion'
                                               , inputContainers = [ BiKalmanFittedPions ]
                                             )

        DeltaMSigMassCut = "(M-MAXTREE('D0'==ABSID,M)<%s )" \
                           "& (M-MAXTREE('D0'==ABSID,M)>%s )" \
                           % (self.getProp('DeltaM_MAX'), self.getProp('DeltaM_MIN'))

        for mode in decayModes.keys() : # {
            ## Combinatorics
	    modeName = self.getProp('name_prefix') + '_' + mode
            
	    if ( "Tight" in modeName):
		    d02HMuNuComb = self.__combineTight(  name = modeName
			    , inputSeq = decayModes[mode]['inList']
			    , decayDesc = [ decayModes[mode]['descriptor'] ]
			    )
	    else:
                    d02HMuNuComb = self.__combine(  name = modeName
                            , inputSeq = decayModes[mode]['inList']
                            , decayDesc = [ decayModes[mode]['descriptor'] ]
                            )
            d02HMuNuSigSeq  = self.__filterHlt1TOS( modeName, d02HMuNuComb )
	    if not ( "Tight" in modeName):
              d02HMuNuSigLine = self.__makeLine(modeName, algos = [ d02HMuNuSigSeq ])
            else:
              Hlt2CharmSlowPions = self.__DstarCombine( name = modeName + '_SlowPion'
                                                  , inputSeq = [ d02HMuNuSigSeq , pionsForDstar ]
                                                  , decayDesc = [ "[D*(2010)+ -> D0 pi+]cc", "[D*(2010)- -> D0 pi-]cc" ]
                                                  , masscut = DeltaMSigMassCut
                                                )
              d02HMuNuSigLineSP = self.__makeLine(modeName, algos = [ Hlt2CharmSlowPions ])

        # }
