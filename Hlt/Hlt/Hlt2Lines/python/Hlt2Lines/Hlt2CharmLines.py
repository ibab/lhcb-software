## $Id: Hlt2CharmLines.py,v 1.19 2010-08-18 16:30:31 spradlin Exp $
__author__  = 'Patrick Spradlin'
__date__    = '$Date: 2010-08-18 16:30:31 $'
__version__ = '$Revision: 1.19 $'

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## hadronic decays of charm hadrons.
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2CharmLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    # Don't touch my variables!
    __slots__ = { 'ComRobAllTrkPtLL'        : 300.0      # in MeV
                , 'ComRobAllTrkPLL'         : 2000.0     # in MeV
                , 'ComRobAllTrkPVIPLL'      : 0.025      # in mm
                , 'ComRobPairMinDocaUL'     : 0.20       # in mm
                , 'ComRobPairMaxDocaUL'     : 1.0        # in mm
                , 'ComRobTrkMaxPtLL'        : 1500.0     # in MeV
                , 'ComRobVtxPVDispLL'       : 2.0        # in mm
                , 'ComRobVtxPVRDispLL'      : 0.2        # in mm
                , 'RobustPointingUL'        : 0.20       # unitless
                , 'ComRobUseGEC'            : True       # do or do not 
                , 'ComRobGEC'               : 120        # max number of tracks
                , 'ComTFAllTrkPtLL'         : 300.0      # in MeV
                , 'ComTFAllTrkPLL'          : 2000.0     # in MeV
                , 'ComTFAllTrkPVIPChi2LL'   : 9.0        # unitless
                , 'ComTFAllTrkChi2UL'       : 10.0       # unitless
                , 'ComTFPairMinDocaUL'      : 0.20       # in mm
                , 'ComTFPairMaxDocaUL'      : 1.0        # in mm
                , 'ComTFTrkMaxPtLL'         : 1500.0     # in MeV
                , 'ComTFVtxPVDispChi2LL'    : 25.0       # unitless
                , 'TFPointUL'               : 0.20       # unitless
                ## Cut values for one stage track-fit (OSTF) lines.
                , 'OSTFAllTrkPtLL'          : 300.0      # in MeV
                , 'OSTFAllTrkPLL'           : 2000.0     # in MeV
                , 'OSTFAllTrkPVIPChi2LL'    : 9.0        # unitless
                , 'OSTFAllTrkChi2UL'        : 10.0       # unitless
                , 'OSTFPairMinDocaUL'       : 0.20       # in mm
                , 'OSTFPairMaxDocaUL'       : 1.0        # in mm
                , 'OSTFTrkMaxPtLL'          : 1500.0     # in MeV
                , 'OSTFVtxPVDispChi2LL'     : 100.0      # unitless
                , 'OSTFPointUL'             : 0.20       # unitless
                ## Slots for K_S h h' lines
                , 'KshhTFHHTrkPLL'          : 1500.0     # in MeV
                , 'KshhTFHHTrkChi2UL'       : 20.0       # unitless
                , 'KshhTFHHTrkMaxPVIPLL'    : 0.1        # in mm
                , 'KshhTFHHVtxChi2UL'       : 20.0       # in mm
                , 'KshhTFHHVtxPVDispLL'     : -1.0       # in mm
                , 'KshhTFHHPtLL'            : 500.0      # in mm
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
                , 'KshhTFKsDDTrkChi2UL'     : 20.0       # unitless
                , 'KshhTFKsDDCombSymMassWin': 80.0       # in MeV
                , 'KshhTFKsDDMothSymMassWin': 24.9       # in MeV
                , 'KshhTFKsDDVtxChi2UL'     : 30.0       # unitless
                , 'KshhTFKsDDVtxPVDispZLL'  : 0.0        # in mm
                , 'KshhTFKsDDVtxPVDispZUL'  : 2300.0     # in mm
                , 'KshhTFKsDDVtxPVDispChi2LL' : 100.0    # unitless
                , 'KshhTFKsDDDiraLL'        : 0.9997     # unitless
                , 'KshhTFDVtxChi2UL'        : 20.0       # unitless
                , 'KshhTFDVtxPVDispLL'      : -1.0       # in mm
                , 'KshhTFDPtLL'             : 500.0      # in MeV
                , 'KshhTFDDiraLL'           : 0.999      # unitless
                , 'KshhTFDwKsLLSymMassWin'  : 100.0      # in MeV
                , 'KshhTFDwKsDDSymMassWin'  : 120.0      # in MeV
                , 'D02MuMuMinDaughterPt'      : 1          # in GeV
                , 'D02MuMuMinLifeTime'        : 0.1        # in ps
                , 'Prescale'                : {
                                                 'Hlt2CharmTF2BodySA' : 0.001
                                               , 'Hlt2CharmTF3BodySA' : 0.001
                                               , 'Hlt2CharmTF4BodySA' : 0.001
                                               , 'Hlt2CharmTF2BodyWideMass' : 0.05
                                               , 'Hlt2CharmTF3BodyWideMass' : 0.05
                                               , 'Hlt2CharmTF4BodyWideMass' : 0.05
                                               , 'Hlt2CharmTF2BodyWideMassSA' : 0.001
                                               , 'Hlt2CharmTF3BodyWideMassSA' : 0.001
                                               , 'Hlt2CharmTF4BodyWideMassSA' : 0.001
                                               , 'Hlt2CharmOSTF2BodyWideMass' : 0.05
                                               , 'Hlt2CharmOSTF3BodyWideMass' : 0.05
                                               , 'Hlt2CharmOSTF4BodyWideMass' : 0.05
                                               , 'Hlt2CharmD02PiPiForD02MuMu' : 0.01
                                               , 'Hlt2CharmD02KPiForD02MuMu'  : 0.01
                                               , 'Hlt2CharmD02KKForD02MuMu'   : 0.01
                                              }
                , 'Postscale'               : {'Hlt2Charm2BodySA' : 0.002
                                               , 'Hlt2Charm3BodySA' : 0.0005
                                               , 'Hlt2Charm4BodySA' : 0.0005
                                              }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : { ## Main two stage lines
                                     'Hlt2CharmTF2BodySignalDecision' : 50880
                                   , 'Hlt2CharmTF3BodySignalDecision' : 50910
                                   , 'Hlt2CharmTF4BodySignalDecision' : 50950
                                   ## Robust stage monitoring lines
                                   , 'Hlt2Charm2BodySADecision'   : 50850
                                   , 'Hlt2Charm3BodySADecision'   : 50860
                                   , 'Hlt2Charm4BodySADecision'   : 50870
                                   ## Post-track fit stage monitoring lines
                                   , 'Hlt2CharmTF2BodySADecision' : 50882
                                   , 'Hlt2CharmTF3BodySADecision' : 50912
                                   , 'Hlt2CharmTF4BodySADecision' : 50952
                                   ## Two stage wide mass sideband lines
                                   , 'Hlt2CharmTF2BodyWideMassDecision' : 50890
                                   , 'Hlt2CharmTF3BodyWideMassDecision' : 50920
                                   , 'Hlt2CharmTF4BodyWideMassDecision' : 50960
                                   ## Post-TF monitoring for wide mass
                                   , 'Hlt2CharmTF2BodyWideMassSADecision' : 50893
                                   , 'Hlt2CharmTF3BodyWideMassSADecision' : 50923
                                   , 'Hlt2CharmTF4BodyWideMassSADecision' : 50963
                                   ## Main One stage lines
                                   , 'Hlt2CharmOSTF2BodyDecision' : 50883
                                   , 'Hlt2CharmOSTF3BodyDecision' : 50913
                                   , 'Hlt2CharmOSTF4BodyDecision' : 50953
                                   ## One stage wide mass lines
                                   , 'Hlt2CharmOSTF2BodyWideMassDecision' : 50885
                                   , 'Hlt2CharmOSTF3BodyWideMassDecision' : 50915
                                   , 'Hlt2CharmOSTF4BodyWideMassDecision' : 50955
                                   ## Kshh' lines
                                   , 'Hlt2CharmTFD2HHKsLLDecision' : 50967
                                   , 'Hlt2CharmTFD2HHKsDDDecision' : 50968
                                   , 'Hlt2CharmD02MuMuDecision' : 50971
                                   , 'Hlt2CharmD02PiPiForD02MuMuDecision' : 50972
                                   , 'Hlt2CharmD02KPiForD02MuMuDecision' : 50973
                                   , 'Hlt2CharmD02KKForD02MuMuDecision' : 50974
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
                       )
        self.__updateHltANNSvc(lineName)
    # }


    def __robustCombine(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        # Function to configure common particle combinations used by the
        #   robust stages of the charm lines.  It lashes the new
        #   CombineParticles to a bindMembers with its antecedents.
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

        # Construct a cut string for the combination.
        combcuts = "(AM<2100*MeV) & (AMAXDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """
            (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(ComRobTrkMaxPtLL)s *MeV)
            & (BPVVD> %(ComRobVtxPVDispLL)s )
            & (BPVVDR> %(ComRobVtxPVRDispLL)s )""" % self.getProps()
        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , InputLocations = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )

        charmNBody = bindMembers( name, inputSeq + [ combineNBody ] )
        return charmNBody
    # }


    def __robustFilter(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a robust filter for the charm.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        codestr = "(M>1700*MeV) & (M<2100*MeV) & (BPVTRGPOINTINGWPT< %(RobustPointingUL)s )" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                            , 'CharmRobustFilter'
                            , InputLocations = inputSeq
                            , Code = codestr
                           )

        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }


    def __tfCombine(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        # Function to configure post-track-fit particle combinations
        #   used by the charm lines.  It lashes the new
        #   CombineParticles to a bindMembers with its antecedents.
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

        # Construct a cut string for the combination.
        combcuts = "(AM<2100*MeV) & (AMAXDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )
            & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(ComTFTrkMaxPtLL)s *MeV)""" % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , InputLocations = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )

        charmNBody = bindMembers( name, inputSeq + [ combineNBody ] )
        return charmNBody
    # }


    def __tfFilter(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a post-track-fit filter for the charm.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        codestr = "(BPVTRGPOINTINGWPT< %(TFPointUL)s )" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                            , 'TFFilter'
                            , InputLocations = inputSeq
                            , Code = codestr
                           )

        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }


    def __ostfCombine(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        # Function to configure track fit particle combinations
        #   used by the one stage topological lines.  It lashes the new
        #   CombineParticles to a bindMembers with its antecedents.
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

        # Construct a cut string for the combination.
        combcuts = "(AM<2100*MeV) & (AMAXDOCA('LoKi::TrgDistanceCalculator')< %(OSTFPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(BPVVDCHI2> %(OSTFVtxPVDispChi2LL)s )
            & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(OSTFTrkMaxPtLL)s *MeV)""" % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineTopoNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , InputLocations = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )

        topoNBody = bindMembers( name, inputSeq + [ combineTopoNBody ] )
        return topoNBody
    # }


    def __ostfFilter(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a filter for the one stage track-fit
        #   topological lines.  It lashes the new FilterDesktop to a
        #   bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        codestr = "(BPVTRGPOINTINGWPT< %(OSTFPointUL)s )" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'TFFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
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
        if self.getProp('ComRobUseGEC') : # {
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(ComRobGEC)s" % self.getProps()
        # }
            
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter('Hlt2CharmKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __robInPartFilter(self, name, inputSeq) : # {
        """
        # Function to configure a filter for the input particles of the
        #   robust stages of the charm lines.  It lashes the new FilterDesktop
        #   to a bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        daugcuts = """(PT> %(ComRobAllTrkPtLL)s *MeV)
                      & (P> %(ComRobAllTrkPLL)s *MeV)
                      & (MIPDV(PRIMARY)> %(ComRobAllTrkPVIPLL)s )""" % self.getProps()
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputSeq
                            , Code = daugcuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D() ] + inputSeq + [ filter ] )

        return filterSeq
    # }


    def __tfInPartFilter(self, name, inputContainers) : # {
        """
        # Function to configure a filter for the input particles of the
        #   post-track-fit stages of the charm lines.  It lashes the new
        #   FilterDesktop to a bindMembers with its antecedents.
        # The argument inputContainer should be 
        #   a list of bindMember sequences that produces the particles
        #   to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """(PT> %(ComTFAllTrkPtLL)s *MeV)
                    & (P> %(ComTFAllTrkPLL)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(ComTFAllTrkPVIPChi2LL)s )
                    & (TRCHI2DOF< %(ComTFAllTrkChi2UL)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        ## Require tracking before attempts to fit the tracks.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter] )

        return filterSeq
    # }


    def __ostfInPartFilter(self, name, inputContainers) : # {
        """
        # Function to configure a filter for the input particles of the
        #   one stage track fit lines.  It lashes the new FilterDesktop
        #   to a bindMembers with its antecedents.
        # The argument inputContainer should be 
        #   a list of bindMember sequences that produces the particles
        #   to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """(PT> %(OSTFAllTrkPtLL)s *MeV)
                    & (P> %(OSTFAllTrkPLL)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(OSTFAllTrkPVIPChi2LL)s )
                    & (TRCHI2DOF< %(OSTFAllTrkChi2UL)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
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

        incuts = """(P> %(KshhTFHHTrkPLL)s *MeV)
                    & (TRCHI2DOF< %(KshhTFHHTrkChi2UL)s )
                    & (TRPCHI2>0.00001)""" % self.getProps()

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

        # Construct a cut string for the combination.
        combcuts = ""

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(VFASPF(VCHI2/VDOF)< %(KshhTFDVtxChi2UL)s )
                          & (BPVVD> %(KshhTFDVtxPVDispLL)s *mm)
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



    def __apply_configuration__(self) :
        ###################################################################
        # Decay descriptors
        ###################################################################
        decayDesc2Body = [  "K*(892)0 -> pi+ pi+" , "K*(892)0 -> pi+ pi-"
                          , "K*(892)0 -> pi- pi-" , "K*(892)0 -> K+ K+"
                          , "K*(892)0 -> K+ K-"   , "K*(892)0 -> K- K-"
                          , "K*(892)0 -> K+ pi-"  , "K*(892)0 -> pi+ K-"
                          , "K*(892)0 -> K+ pi+"  , "K*(892)0 -> K- pi-" ]
        decayDesc3Body = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
        decayDesc4Body = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
        ###################################################################
        ## Filter the input particles.
        ###################################################################
        from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
        lclRobInputKaons = self.__robInPartFilter('CharmInputKaons', [ GoodKaons ])
        lclRobInputPions = self.__robInPartFilter('CharmInputPions', [ GoodPions ])


        ###################################################################
        # Create sequences for the shared combinatorics of the robust stages.
        ###################################################################

        # CombineParticles for the robust 2-body combinations.
        ###################################################################
        charm2Body = self.__robustCombine(  name = 'Charm2Body'
                                  , inputSeq = [ lclRobInputKaons, lclRobInputPions ]
                                  , decayDesc = decayDesc2Body
                                  , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMinDocaUL)s )" % self.getProps() }
                                  )

        # CombineParticles for the robust 3-body combinations.
        ###################################################################
        charm3Body = self.__robustCombine(  name = 'Charm3Body'
                                  , inputSeq = [ lclRobInputPions, charm2Body ]
                                  , decayDesc = decayDesc3Body )


        # Construct a bindMember for the charm robust 4-body decision
        # CombineParticles for the 4-body combinations.
        ###################################################################
        charmRob4Body = self.__robustCombine(  name = 'CharmRobust4Body'
                                  , inputSeq = [lclRobInputPions, charm3Body ]
                                  , decayDesc = decayDesc4Body
                                  , extracuts = {'CombinationCut' : '(AM>1700*MeV) & (AM<2100*MeV)'
                                                , 'MotherCut'     : "(BPVTRGPOINTINGWPT< %(RobustPointingUL)s )" % self.getProps()
                                                }
                                  )


        ###################################################################
        # Construct a bindMember for the charm robust 2-body decision
        ###################################################################
        charmRobust2BodySeq = self.__robustFilter('CharmRobust2Body', [charm2Body], extracode = '(SUMQ==0)')


        # Construct a bindMember for the charm robust 3-body decision
        ###################################################################
        charmRobust3BodySeq = self.__robustFilter('CharmRobust3Body', [charm3Body])


        # Construct a bindMember for the charm robust 4-body decision
        ###################################################################
        charmRobust4BodySeq = self.__robustFilter('CharmRobust4Body', [charmRob4Body])



        ###################################################################
        ## Filter post-track fit input particles.
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        lclTFInputKaons = self.__tfInPartFilter('CharmTFInputKaons', [ BiKalmanFittedKaons] )
        lclTFInputPions = self.__tfInPartFilter('CharmTFInputPions', [ BiKalmanFittedPions] )


        ###################################################################
        # Create sequences for the shared combinatorics of the post-track-fit
        #   stages.
        ###################################################################

        # post-track-fit 2-body combinations
        ###################################################################
        charmTF2Body = self.__tfCombine(  name = 'CharmTF2Body'
                                , inputSeq = [ lclTFInputKaons, lclTFInputPions ]
                                #, decayDesc = ["K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-"]
                                , decayDesc = decayDesc2Body 
                                , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMinDocaUL)s )" % self.getProps() }
                               )

        # post-track-fit 3-body combinations
        ###################################################################
        charmTF3Body = self.__tfCombine(  name = 'CharmTF3Body'
                                , inputSeq = [ lclTFInputPions, charmTF2Body ]
                                , decayDesc = decayDesc3Body
                               )

        # post-track-fit 4-body combinations
        ###################################################################
        charmTF4Body = self.__tfCombine(  name = 'CharmTF4Body'
                                , inputSeq = [ lclTFInputPions, charmTF3Body ]
                                , decayDesc = decayDesc4Body
                                , extracuts = { 'CombinationCut' : '(AM>1839*MeV) & (AM<1889*MeV)'
                                              , 'MotherCut' : '(BPVTRGPOINTINGWPT< %(TFPointUL)s )' % self.getProps()
                                              }
                                 )
        ###################################################################
        # Main 2-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF2BodySeq = self.__tfFilter('CharmPostTF2Body'
                                     , [charmTF2Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV) & (SUMQ == 0)')


        # Main 3-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF3BodySeq = self.__tfFilter('CharmPostTF3Body'
                                     , [charmTF3Body]
                                     , extracode = '(((M>1844*MeV) & (M<1894*MeV)) | ((M>1943*MeV) & (M<1993*MeV))) & (abs(CHILD(1,SUMQ) + CHILD(2,Q))==1)')


        # Main 4-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF4BodySeq = self.__tfFilter('CharmPostTF4Body'
                                     , [charmTF4Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV)')



        ##
        #
        # Charm lines
        #
        ##
        # Map charm robust sequences to monitoring line names
        ###################################################################
        charmNBodySeq = {  '2Body' : {  'Rob' : charmRobust2BodySeq
                                      , 'TF'  : charmTF2BodySeq }
                         , '3Body' : {  'Rob' : charmRobust3BodySeq
                                      , 'TF'  : charmTF3BodySeq }
                         , '4Body' : {  'Rob' : charmRobust4BodySeq
                                      , 'TF'  : charmTF4BodySeq }
                        }

        ###################################################################
        # Create the main charm lines and the standalone monitoring
        # lines.
        ###################################################################
        for charmSeq in charmNBodySeq.keys() :
            robSALineName = 'Charm' + charmSeq + 'SA'
            tfSALineName = 'CharmTF' + charmSeq + 'SA'
            sigLineName = 'CharmTF' + charmSeq + 'Signal'
            self.__makeLine(robSALineName, algos = [ charmNBodySeq[charmSeq]['Rob'] ] )
            self.__makeLine(tfSALineName, algos = [ charmNBodySeq[charmSeq]['TF'] ] )
            self.__makeLine(sigLineName, algos = [ charmNBodySeq[charmSeq]['Rob'], charmNBodySeq[charmSeq]['TF'] ] )



        ##
        #
        # Define additional charm lines for mass sidebands.  Heavily prescaled.
        #
        ##
        # Line for 2-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTF2BodySBSeq = self.__tfFilter('CharmPostTF2BodyWideMass'
                                     , [charmTF2Body]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')


        # Line for 3-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTF3BodySBSeq = self.__tfFilter('CharmPostTF3BodyWideMass'
                                     , [charmTF3Body]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV) & (abs(CHILD(1,SUMQ) + CHILD(2,Q))==1)')


        # Line for 4-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTF4BodySB = self.__tfCombine(  name = 'CharmWMTF4Body'
                                , inputSeq = [ lclTFInputPions, charmTF3Body ]
                                , decayDesc = decayDesc4Body
                                , extracuts = { 'CombinationCut' : '(AM>1700*MeV) & (AM<2100*MeV)' 
                                              , 'MotherCut' : '(BPVTRGPOINTINGWPT< %(TFPointUL)s )' % self.getProps()
                                              }
                                 )

        charmTF4BodySBSeq = self.__tfFilter('CharmPostTF4BodyWideMass'
                                     , [charmTF4BodySB]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')

        charmNBodyWideMassSeq = {  '2Body' : charmTF2BodySBSeq
                                 , '3Body' : charmTF3BodySBSeq
                                 , '4Body' : charmTF4BodySBSeq
                                }


        ###################################################################
        # Create the main prescaled wide mass window lines for mass
        # sidebands, and monitoring lines for the post-track-fit stage of
        # the wide mass lines.
        ###################################################################
        for charmSeq in charmNBodySeq.keys() :
            wmLineName = 'CharmTF' + charmSeq + 'WideMass'
            wmTFLineName = 'CharmTF' + charmSeq + 'WideMassSA'
            self.__makeLine(wmLineName
                 , algos = [ charmNBodySeq[charmSeq]['Rob'], charmNBodyWideMassSeq[charmSeq] ])
            self.__makeLine(wmTFLineName
                 , algos = [ charmNBodyWideMassSeq[charmSeq] ])



        ###################################################################
        # Combinations for one stage lines that run on fitted tracks, for
        # low rate running.
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        lclOSTFInputKaons = self.__ostfInPartFilter('CharmOSTFInputKaons', [ BiKalmanFittedKaons] )
        lclOSTFInputPions = self.__ostfInPartFilter('CharmOSTFInputPions', [ BiKalmanFittedPions] )

        # 2-body combinations
        ###################################################################
        charmOSTF2Body = self.__ostfCombine(  name = 'CharmOSTF2Body'
                                , inputSeq = [ lclOSTFInputKaons, lclOSTFInputPions ]
                                #, decayDesc = ["K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-"]
                                , decayDesc = decayDesc2Body 
                                , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(OSTFPairMinDocaUL)s )" % self.getProps() }
                               )

        # 3-body combinations
        ###################################################################
        charmOSTF3Body = self.__ostfCombine(  name = 'CharmOSTF3Body'
                                , inputSeq = [ lclOSTFInputPions, charmOSTF2Body ]
                                , decayDesc = decayDesc3Body
                               )

        # 4-body combinations
        ###################################################################
        charmOSTF4Body = self.__ostfCombine(  name = 'CharmOSTF4Body'
                                , inputSeq = [ lclOSTFInputPions, charmOSTF3Body ]
                                , decayDesc = decayDesc4Body
                                , extracuts = { 'CombinationCut' : '(AM>1839*MeV) & (AM<1889*MeV)'
                                              , 'MotherCut' : '(BPVTRGPOINTINGWPT< %(OSTFPointUL)s )' % self.getProps()
                                              }
                                 )
        ###################################################################
        # Decision sequences for the one stage lines
        ###################################################################

        # 2-body decision
        ###################################################################
        charmOSTF2BodySeq = self.__ostfFilter('CharmOSTF2Body'
                                     , [charmOSTF2Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV) & (SUMQ == 0)')


        # 3-body decision
        ###################################################################
        charmOSTF3BodySeq = self.__ostfFilter('CharmOSTF3Body'
                                     , [charmOSTF3Body]
                                     , extracode = '(((M>1844*MeV) & (M<1894*MeV)) | ((M>1943*MeV) & (M<1993*MeV))) & (abs(CHILD(1,SUMQ) + CHILD(2,Q))==1)')


        # 4-body decision
        ###################################################################
        charmOSTF4BodySeq = self.__ostfFilter('CharmOSTF4Body'
                                     , [charmOSTF4Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV)')


        # Line for 2-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmOSTF2BodySBSeq = self.__ostfFilter('CharmPostOSTF2BodyWideMass'
                                     , [charmOSTF2Body]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')


        # Line for 3-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmOSTF3BodySBSeq = self.__ostfFilter('CharmPostOSTF3BodyWideMass'
                                     , [charmOSTF3Body]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV) & (abs(CHILD(1,SUMQ) + CHILD(2,Q))==1)')


        # Line for 4-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmOSTF4BodySB = self.__ostfCombine(  name = 'CharmWMOSTF4Body'
                                , inputSeq = [ lclOSTFInputPions, charmOSTF3Body ]
                                , decayDesc = decayDesc4Body
                                , extracuts = { 'CombinationCut' : '(AM>1700*MeV) & (AM<2100*MeV)' 
                                              , 'MotherCut' : '(BPVTRGPOINTINGWPT< %(OSTFPointUL)s )' % self.getProps()
                                              }
                                 )

        charmOSTF4BodySBSeq = self.__ostfFilter('CharmPostOSTF4BodyWideMass'
                                     , [charmOSTF4BodySB]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')

        # Line creation for one stage track fit lines.
        ###################################################################
        charmOSNBodySeq = {  '2Body' : {  'Std'  : charmOSTF2BodySeq
                                        , 'Wide' : charmOSTF2BodySBSeq }
                           , '3Body' : {  'Std'  : charmOSTF3BodySeq
                                        , 'Wide' : charmOSTF3BodySBSeq }
                           , '4Body' : {  'Std'  : charmOSTF4BodySeq
                                        , 'Wide' : charmOSTF4BodySBSeq }
                          }

        for charmSeq in charmOSNBodySeq.keys() :
            osLineName   = 'CharmOSTF' + charmSeq
            oswmTFLineName = 'CharmOSTF' + charmSeq + 'WideMass'
            self.__makeLine(osLineName, algos = [ charmOSNBodySeq[charmSeq]['Std'] ])
            self.__makeLine(oswmTFLineName, algos = [ charmOSNBodySeq[charmSeq]['Wide'] ])



        ###################################################################
        # Lines for D decays with K_S
        ###################################################################


        ## Input particles for 2-body HH part of KsHH
        ###################################################################
        lclKshhTFInputKaons = self.__KshhTFHHInPartFilter('CharmKshhTFHHInputKaons', [ BiKalmanFittedKaons] )
        lclKshhTFInputPions = self.__KshhTFHHInPartFilter('CharmKshhTFHHInputPions', [ BiKalmanFittedPions] )


        ## 2-body construction for KsHH
        ###################################################################
        KshhHHcombcuts = """(AM<2100*MeV) 
                            & (AMAXCHILD(MIPDV(PRIMARY), (('pi+'==ABSID) | ('K+'==ABSID)))> %(KshhTFHHTrkMaxPVIPLL)s *mm)
                            & (AALLSAMEBPV)""" % self.getProps()
        KshhHHparentcuts = """(VFASPF(VCHI2/VDOF)< %(KshhTFHHVtxChi2UL)s )
                              & (BPVVD> %(KshhTFHHVtxPVDispLL)s *mm)
                              & (PT> %(KshhTFHHPtLL)s *MeV)""" % self.getProps()

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
        charmKshhTF2Body = bindMembers( 'CharmKshhTF2Body',  [lclKshhTFInputKaons, lclKshhTFInputPions, PV3D(), combineKshhTF2Body ] )


        ## Ks reconstruction
        ## These should probably be moved to SharedParticles
        ###################################################################
        KshhKsLLdaugcuts = { "pi+" : "(P> %(KshhTFKsLLTrkPLL)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KshhTFKsLLTrkPVIPChi2LL)s ) & (TRCHI2DOF< %(KshhTFKsLLTrkChi2UL)s)" % self.getProps() }
        KshhKsLLcombcuts = "ADAMASS('KS0')< %(KshhTFKsLLCombSymMassWin)s *MeV" % self.getProps()
        KshhKsLLparentcuts = """(VFASPF(VCHI2/VDOF)< %(KshhTFKsLLVtxChi2UL)s ) 
                        & (BPVVDZ> %(KshhTFKsLLVtxPVDispZLL)s *mm) 
                        & (BPVVDZ< %(KshhTFKsLLVtxPVDispZUL)s *mm) 
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
        charmKshhTFKsLL = bindMembers( "CharmKshhTFKsLL", [ BiKalmanFittedPions, PV3D(), combineKshhTFKsLL ] )


        ## Repeat for KsDD with independent cuts.
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions

        KshhKsDDdaugcuts = { "pi+" : "(P> %(KshhTFKsDDTrkPLL)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KshhTFKsDDTrkPVIPChi2LL)s ) & (TRCHI2DOF< %(KshhTFKsDDTrkChi2UL)s)" % self.getProps() }
        KshhKsDDcombcuts = "ADAMASS('KS0')< %(KshhTFKsDDCombSymMassWin)s *MeV" % self.getProps()
        KshhKsDDparentcuts = """(VFASPF(VCHI2/VDOF)< %(KshhTFKsDDVtxChi2UL)s ) 
                        & (BPVVDZ> %(KshhTFKsDDVtxPVDispZLL)s *mm) 
                        & (BPVVDZ< %(KshhTFKsDDVtxPVDispZUL)s *mm) 
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
        charmKshhTFKsDD = bindMembers( "CharmKshhTFKsDD", [ BiKalmanFittedDownPions, PV3D(), combineKshhTFKsDD ] )


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
        self.__makeLine('CharmTFD2HHKsLL' , algos = [ combineKshhTFD2HHKsLL ])
        self.__makeLine('CharmTFD2HHKsDD' , algos = [ combineKshhTFD2HHKsDD ])


        ## D[0 -> mu+ mu-] with pt cut on daughters and life time cut on D0;
        ## prescaled calibration channels: pi+ pi-, K- pi+, K+ K-
        ###################################################################

#        D0DaughterCut = "(PT > 1 * GeV)"
        D0DaughterCut = "(PT > %(D02MuMuMinDaughterPt)s * GeV)" % self.getProps()
        D0CombinationCut = "(ADAMASS('D0') < 100 * MeV)"
#        D0MotherCut = "(VFASPF(VCHI2/VDOF) < 100) & (BPVLTFITCHI2() < 100) & (BPVLTIME() > 0.1 * ps)"
        D0MotherCut = "(VFASPF(VCHI2/VDOF) < 100) & (BPVLTFITCHI2() < 100) & (BPVLTIME() > %(D02MuMuMinLifeTime)s * ps)" % self.getProps()

        from Hlt2SharedParticles.BasicParticles import Muons, NoCutsPions, NoCutsKaons

        D02MuMuCombine = Hlt2Member(CombineParticles
                                    , 'D02MuMuCombine'
                                    , DecayDescriptor = '[ D0 -> mu+ mu- ]cc'
                                    , InputLocations = [ Muons ]
                                    , DaughtersCuts = { "mu+" : D0DaughterCut }
                                    , CombinationCut = D0CombinationCut
                                    , MotherCut = D0MotherCut)
        self.__makeLine('CharmD02MuMu'
                        , algos = [ PV3D(), Muons, D02MuMuCombine ])

        D02PiPiForD02MuMuCombine = Hlt2Member(CombineParticles
                                              , 'D02PiPiForD02MuMuCombine'
                                              , DecayDescriptor = '[ D0 -> pi+ pi- ]cc'
                                              , InputLocations = [ NoCutsPions ]
                                              , DaughtersCuts = { "pi+" : D0DaughterCut }
                                              , CombinationCut = D0CombinationCut
                                              , MotherCut = D0MotherCut)
        self.__makeLine('CharmD02PiPiForD02MuMu'
                        , algos = [ PV3D(), NoCutsPions, D02PiPiForD02MuMuCombine ])

        D02KPiForD02MuMuCombine = Hlt2Member(CombineParticles
                                             , 'D02KPiForD02MuMuCombine'
                                             , DecayDescriptor = '[ D0 -> K+ pi- ]cc'
                                             , InputLocations = [ NoCutsKaons, NoCutsPions ]
                                             , DaughtersCuts = { "K+" : D0DaughterCut, "pi+" : D0DaughterCut }
                                             , CombinationCut = D0CombinationCut
                                             , MotherCut = D0MotherCut)
        self.__makeLine('CharmD02KPiForD02MuMu'
                        , algos = [ PV3D(), NoCutsKaons, NoCutsPions, D02KPiForD02MuMuCombine ])

        D02KKForD02MuMuCombine = Hlt2Member(CombineParticles
                                            , 'D02KKForD02MuMuCombine'
                                            , DecayDescriptor = '[ D0 -> K+ K- ]cc'
                                            , InputLocations = [ NoCutsKaons ]
                                            , DaughtersCuts = { "K+" : D0DaughterCut }
                                            , CombinationCut = D0CombinationCut
                                            , MotherCut = D0MotherCut)
        self.__makeLine('CharmD02KKForD02MuMu'
                        , algos = [ PV3D(), NoCutsKaons, D02KKForD02MuMuCombine ])
