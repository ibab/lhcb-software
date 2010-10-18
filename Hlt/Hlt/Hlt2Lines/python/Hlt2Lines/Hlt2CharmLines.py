## $Id: Hlt2CharmLines.py,v 1.22 2010-09-09 15:20:09 graven Exp $
__author__  = 'Patrick Spradlin'
__date__    = '$Date: 2010-09-09 15:20:09 $'
__version__ = '$Revision: 1.22 $'

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
                ## 2-body!
                , 'OSTFAllTrkPtLL_2Body'          : 500.0      # in MeV
                , 'OSTFAllTrkPLL_2Body'           : 5000.0     # in MeV
                , 'OSTFAllTrkPVIPChi2LL_2Body'    : 2.0        # unitless
                , 'OSTFAllTrkChi2UL_2Body'        : 5.0        # unitless
                , 'OSTFPairMinDocaUL_2Body'       : 0.10       # in mm
                , 'OSTFTrkMaxPtLL_2Body'          : 1500.0     # in MeV
                , 'OSTFVtxPVDispChi2LL_2Body'     : 25.0       # unitless
                , 'OSTFPointLL_2Body'             : 0.99985     # unitless
                , 'OSTFVtxChi2UL_2Body'           : 10.0       # unitless
                , 'OSTFDPtLL_2Body'               : 2000.0     # in MeV
                ## 3-body!
                , 'OSTFAllTrkPtLL_3Body'          : 500.0      # in MeV
                , 'OSTFAllTrkPLL_3Body'           : 5000.0     # in MeV
                , 'OSTFAllTrkPVIPChi2LL_3Body'    : 4.0       # unitless
                , 'OSTFAllTrkChi2UL_3Body'        : 5.0        # unitless
                , 'OSTFPairMinDocaUL_3Body'       : 0.20       # in mm
                , 'OSTFPairMaxDocaUL_3Body'       : 1.00       # in mm
                , 'OSTFTrkMaxPtLL_3Body'          : 1500.0     # in MeV
                , 'OSTFVtxPVDispChi2LL_3Body'     : 100.0      # unitless
                , 'OSTFDIPChi2UL_3Body'           : 9.0       # unitless
                , 'OSTFVtxChi2UL_3Body'           : 15.0       # unitless
                , 'OSTFDSumPtLL_3Body'            : 1800.0     # in MeV
                , 'OSTFDPtLL_3Body'               : 2000.0     # in MeV 
                ## 2-body inclusive!
                , 'OSTFAllTrkPtLL_2BodyInc'          : 500.0      # in MeV
                , 'OSTFAllTrkPLL_2BodyInc'           : 5000.0     # in MeV
                , 'OSTFAllTrkPVIPChi2LL_2BodyInc'    : 16.0        # unitless
                , 'OSTFAllTrkChi2UL_2BodyInc'        : 3.0        # unitless
                , 'OSTFPairMinDocaUL_2BodyInc'       : 0.15       # in mm
                , 'OSTFVtxPVDispChi2LL_2BodyInc'     : 100.0       # unitless
                , 'OSTFTrkMaxPtLL_2BodyInc'          : 1000.0     # in MeV
                , 'OSTFDPtLL_2BodyInc'               : 1500.0     # in MeV
                , 'OSTFDSumPtLL_2BodyInc'            : 1800.0     # in MeV
                , 'OSTFDIPChi2LL_2BodyInc'           : 2.0       # unitless
                , 'OSTFMCOR_MIN_2BodyInc'            : 300.        # MeV
                , 'OSTFMCOR_MAX_2BodyInc'            : 3200.        # MeV
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
                                               , 'Hlt2CharmTF2BodyWideMass' : 0.05
                                               , 'Hlt2CharmTF3BodyWideMass' : 0.05
                                               , 'Hlt2CharmTF2BodyWideMassSA' : 0.001
                                               , 'Hlt2CharmTF3BodyWideMassSA' : 0.001
                                               , 'Hlt2CharmOSTF2BodyWideMass' : 0.05
                                               , 'Hlt2CharmOSTF3BodyWideMass' : 0.05
                                               , 'Hlt2CharmD02PiPiForD02MuMu' : 0.01
                                               , 'Hlt2CharmD02KPiForD02MuMu'  : 0.01
                                               , 'Hlt2CharmD02KKForD02MuMu'   : 0.01
                                              }
                , 'Postscale'               : {'Hlt2Charm2BodySA' : 0.002
                                               , 'Hlt2Charm3BodySA' : 0.0005
                                              }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : { ## Main two stage lines
                                     'Hlt2CharmTF2BodySignalDecision' : 50880
                                   , 'Hlt2CharmTF3BodySignalDecision' : 50910
                                   ## Robust stage monitoring lines
                                   , 'Hlt2Charm2BodySADecision'   : 50850
                                   , 'Hlt2Charm3BodySADecision'   : 50860
                                   ## Post-track fit stage monitoring lines
                                   , 'Hlt2CharmTF2BodySADecision' : 50882
                                   , 'Hlt2CharmTF3BodySADecision' : 50912
                                   ## Two stage wide mass sideband lines
                                   , 'Hlt2CharmTF2BodyWideMassDecision' : 50890
                                   , 'Hlt2CharmTF3BodyWideMassDecision' : 50920
                                   ## Post-TF monitoring for wide mass
                                   , 'Hlt2CharmTF2BodyWideMassSADecision' : 50893
                                   , 'Hlt2CharmTF3BodyWideMassSADecision' : 50923
                                   ## Main One stage lines
                                   , 'Hlt2CharmOSTF2BodyDecision' : 50883
                                   , 'Hlt2CharmOSTF2BodyIncDecision' : 50884 
                                   , 'Hlt2CharmOSTF3BodyDecision' : 50913
                                   ## One stage wide mass lines
                                   , 'Hlt2CharmOSTF2BodyWideMassDecision' : 50885
                                   , 'Hlt2CharmOSTF3BodyWideMassDecision' : 50915
                                   ## Kshh' lines
                                   , 'Hlt2CharmTFD2HHKsLLDecision' : 50967
                                   , 'Hlt2CharmTFD2HHKsDDDecision' : 50968
                                   , 'Hlt2CharmD02MuMuDecision' : 50971
                                   , 'Hlt2CharmD02PiPiForD02MuMuDecision' : 50972
                                   , 'Hlt2CharmD02KPiForD02MuMuDecision' : 50973
                                   , 'Hlt2CharmD02KKForD02MuMuDecision' : 50974
                                   ## KS0h lines
                                   , 'Hlt2CharmD2KS0PiLLDecision' : 50969
                                   , 'Hlt2CharmD2KS0PiDDDecision' : 50970
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

        codestr = "in_range(1700*MeV,M, 2100*MeV) & (BPVTRGPOINTINGWPT< %(RobustPointingUL)s )" % self.getProps()
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


    def __ostfCombine2Body(self, name, inputSeq, decayDesc, extracuts = None) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Construct a cut string for the combination.
        combcuts = "(AM<2100*MeV) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(BPVVDCHI2> %(OSTFVtxPVDispChi2LL_2Body)s )
            & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(OSTFTrkMaxPtLL_2Body)s *MeV)""" % self.getProps()

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

    def __ostfCombine2BodyInc(self, name, inputSeq, decayDesc, extracuts = None) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Construct a cut string for the combination.
        combcuts = "(AM<2000*MeV) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(BPVVDCHI2> %(OSTFVtxPVDispChi2LL_2BodyInc)s )
            & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(OSTFTrkMaxPtLL_2BodyInc)s *MeV)""" % self.getProps()

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

    def __ostfCombine3Body(self, name, inputSeq, decayDesc, extracuts = None) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Construct a cut string for the combination.
        combcuts = "(AM<2100*MeV)&(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(OSTFPairMaxDocaUL_3Body)s )&(AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(BPVVDCHI2> %(OSTFVtxPVDispChi2LL_3Body)s )
            & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)), PT)> %(OSTFTrkMaxPtLL_3Body)s *MeV)""" % self.getProps()

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

    def __ostfFilter2Body(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a filter for the one stage track-fit
        #   topological lines.  It lashes the new FilterDesktop to a
        #   bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltLine.Hlt2Monitoring import Hlt2Monitor

        codestr = "(BPVDIRA > %(OSTFPointLL_2Body)s )" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'TFFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
                             , PreMonitor  =  Hlt2Monitor( "M","M(K#pi)",1865.,50,'M_in',nbins=101) 
                             , PostMonitor =  Hlt2Monitor( "M","M(K#pi)",1865.,50,'M_out',nbins=101)   
                           )
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }

    def __ostfFilter2BodyInc(self, name, inputSeq, extracode = None) : # {
        """  
        # Function to configure a filter for the one stage track-fit
        #   topological lines.  It lashes the new FilterDesktop to a
        #   bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        props = self.getProps()
        preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                     "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
        codestr = '(in_range(%s*MeV,MCOR,%s*MeV))' % (props['OSTFMCOR_MIN_2BodyInc'],props['OSTFMCOR_MAX_2BodyInc'])
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'TFFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
                             , Preambulo=preambulo 
                           )    
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }

    def __ostfFilter3Body(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a filter for the one stage track-fit
        #   topological lines.  It lashes the new FilterDesktop to a
        #   bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltLine.Hlt2Monitoring import Hlt2Monitor       
 
        codestr = "( BPVIPCHI2() < %(OSTFDIPChi2UL_3Body)s )" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'TFFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
                             , PreMonitor  =  Hlt2Monitor( "M","M(K#pi#pi)",1910.,100,'M_in',nbins=101) 
                             , PostMonitor =  Hlt2Monitor( "M","M(K#pi#pi)",1910.,100,'M_out',nbins=101)
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


    def __ostfInPartFilter2Body(self, name, inputContainers) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """(PT> %(OSTFAllTrkPtLL_2Body)s *MeV)
                    & (P> %(OSTFAllTrkPLL_2Body)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(OSTFAllTrkPVIPChi2LL_2Body)s )
                    & (TRCHI2DOF< %(OSTFAllTrkChi2UL_2Body)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
    # }

    def __ostfInPartFilter2BodyInc(self, name, inputContainers) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D 

        incuts = """(PT> %(OSTFAllTrkPtLL_2BodyInc)s *MeV)
                    & (P> %(OSTFAllTrkPLL_2BodyInc)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(OSTFAllTrkPVIPChi2LL_2BodyInc)s )
                    & (TRCHI2DOF< %(OSTFAllTrkChi2UL_2BodyInc)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )    

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
    # }    

   
    def __ostfInPartFilter3Body(self, name, inputContainers) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """(PT> %(OSTFAllTrkPtLL_3Body)s *MeV)
                    & (P> %(OSTFAllTrkPLL_3Body)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(OSTFAllTrkPVIPChi2LL_3Body)s )
                    & (TRCHI2DOF< %(OSTFAllTrkChi2UL_3Body)s )""" % self.getProps()

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
        parentcuts = """(VFASPF(VCHI2PDOF)< %(KshhTFDVtxChi2UL)s )
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

    def __DMesonCombine(self, name, inputSeq, decayDesc = ["[D+ -> KS0 pi+]cc"]  , extracuts = None) : # {
        """
        # Function to configure the D -> KS0 Pi combinations
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
        DCombCuts = "in_range( 1380*MeV, AM, 2220*MeV) & (AMAXDOCA('LoKi::DistanceCalculator') < 1.0*mm) "
        
        # Construct a cut string for the daughter pions - allowed???
        DDaughterCuts =  { "pi+" : "(P > 2*GeV) & (TRCHI2DOF < 10) & (MIPCHI2DV(PRIMARY) > 49)",
                           "KS0" : "CHILDCUT(MIPCHI2DV(PRIMARY)>100,1) & CHILDCUT(MIPCHI2DV(PRIMARY)>100,2) & (MIPCHI2DV(PRIMARY) > 9)"
                         }
        
        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        #if extracuts and extracuts.has_key('CombinationCut') :
         #  DCombCuts  = DCombCuts + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        DMotherCuts = """(VFASPF(VCHI2PDOF)< 20 )
                          & in_range(1400*MeV, MM, 2200*MeV)
                          & (MIPCHI2DV(PRIMARY) < 4) """ 
            

        #if extracuts and extracuts.has_key('MotherCut') :
          # DMotherCuts  = DMotherCuts  + '&' + extracuts['MotherCut']
                                   
        combineD = Hlt2Member( CombineParticles
                                   , 'CombineD'
                                   , DecayDescriptors = decayDesc
                                   , InputLocations = inputSeq
                                   , DaughtersCuts =  DDaughterCuts
                                   , CombinationCut = DCombCuts
                                   , MotherCut = DMotherCuts
                                 )

        charmD = bindMembers( name, inputSeq + [ PV3D(), combineD ] )
        return charmD


    def __apply_configuration__(self) :
        ###################################################################
        # Decay descriptors
        ###################################################################
        decayDesc2Body = [  "D0 -> pi+ pi-"
                          , "D0 -> K+ K-"   
                          , "D0 -> K+ pi-"  , "D0 -> pi+ K-"]
        decayDesc2BodyInc = [  "D0 -> pi+ pi-"
                          , "D0 -> pi+ pi+"   
                          , "D0 -> pi- pi-"]
        decayDesc3Body = ["[D+ -> pi+ pi+ K-]cc", "[D+ -> pi+ K+ K-]cc", "[D+ -> K+ K+ K-]cc", 
                          "[D+ -> K+ K+ pi-]cc" , "[D+ -> pi+ pi+ pi-]cc", "[D+ -> pi+ pi- K+]cc"]
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
                                  , inputSeq = [ charm2Body, lclRobInputKaons ]
                                  , decayDesc = decayDesc3Body
                                  )

        ###################################################################
        # Construct a bindMember for the charm robust 2-body decision
        ###################################################################
        charmRobust2BodySeq = self.__robustFilter('CharmRobust2Body', [charm2Body], extracode = '(SUMQ==0)')


        # Construct a bindMember for the charm robust 3-body decision
        ###################################################################
        charmRobust3BodySeq = self.__robustFilter('CharmRobust3Body', [charm3Body])

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

        ###################################################################
        # Main 2-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF2BodySeq = self.__tfFilter('CharmPostTF2Body'
                                     , [charmTF2Body]
                                     , extracode = 'in_range( 1839*MeV, M, 1889*MeV) & (SUMQ == 0)')


        # Main 3-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF3BodySeq = self.__tfFilter('CharmPostTF3Body'
                                     , [charmTF3Body]
                                     , extracode = '(in_range(1844*MeV, M, 1894*MeV) | in_range(1943*MeV, M, 1993*MeV)) & (abs(CHILD(1,SUMQ) + CHILD(2,Q))==1)')

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
                                     , extracode = 'in_range(1700*MeV, M, 2100*MeV)')


        # Line for 3-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTF3BodySBSeq = self.__tfFilter('CharmPostTF3BodyWideMass'
                                     , [charmTF3Body]
                                     , extracode = 'in_range(1700*MeV, M, 2100*MeV) & (abs(CHILD(1,SUMQ) + CHILD(2,Q))==1)')


        charmNBodyWideMassSeq = {  '2Body' : charmTF2BodySBSeq
                                 , '3Body' : charmTF3BodySBSeq
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
        lclOSTFInputKaons2Body = self.__ostfInPartFilter2Body('CharmOSTFInputKaons2Body', [ BiKalmanFittedKaons] )
        lclOSTFInputPions2Body = self.__ostfInPartFilter2Body('CharmOSTFInputPions2Body', [ BiKalmanFittedPions] )
        lclOSTFInputKaons2BodyInc = self.__ostfInPartFilter2BodyInc('CharmOSTFInputKaons2BodyInc', [ BiKalmanFittedKaons] )
        lclOSTFInputPions2BodyInc = self.__ostfInPartFilter2BodyInc('CharmOSTFInputPions2BodyInc', [ BiKalmanFittedPions] )
        lclOSTFInputKaons3Body = self.__ostfInPartFilter3Body('CharmOSTFInputKaons3Body', [ BiKalmanFittedKaons] )
        lclOSTFInputPions3Body = self.__ostfInPartFilter3Body('CharmOSTFInputPions3Body', [ BiKalmanFittedPions] )

        # 2-body combinations
        ###################################################################
        charmOSTF2Body = self.__ostfCombine2Body(  name = 'CharmOSTF2Body'
                                , inputSeq = [ lclOSTFInputKaons2Body, lclOSTFInputPions2Body ]
                                , decayDesc = decayDesc2Body 
                                , extracuts = { 'CombinationCut' : 
                                                "(AMINDOCA('LoKi::TrgDistanceCalculator') < %(OSTFPairMinDocaUL_2Body)s )" % self.getProps() 
                                              }
                               )

        # 2-body inclusive combinations
        ###################################################################
        charmOSTF2BodyInc = self.__ostfCombine2BodyInc(  name = 'CharmOSTF2BodyInc'
                                , inputSeq = [ lclOSTFInputKaons2BodyInc, lclOSTFInputPions2BodyInc ]
                                , decayDesc = decayDesc2BodyInc 
                                , extracuts = { 'CombinationCut' : 
                                                "(AMINDOCA('LoKi::TrgDistanceCalculator') < %(OSTFPairMinDocaUL_2BodyInc)s )" % self.getProps() 
                                              }    
                               )

        # 3-body combinations
        ###################################################################
        charmOSTF3Body = self.__ostfCombine3Body(  name = 'CharmOSTF3Body'
                                , inputSeq = [ lclOSTFInputPions3Body,lclOSTFInputKaons3Body]
                                , decayDesc = decayDesc3Body
                                , extracuts = { 'CombinationCut' : 
                                                "(AMINDOCA('LoKi::TrgDistanceCalculator') < %(OSTFPairMinDocaUL_3Body)s)" % self.getProps() 
                                              }
                               )

        ###################################################################
        # Decision sequences for the one stage lines
        ###################################################################

        # 2-body decision
        ###################################################################
        charmOSTF2BodySeq = self.__ostfFilter2Body('CharmOSTF2Body'
                                     , [charmOSTF2Body]
                                     , extracode = 'in_range(1839*MeV, M, 1889*MeV) & (PT > %(OSTFDPtLL_2Body)s) & (VFASPF(VCHI2PDOF) < %(OSTFVtxChi2UL_2Body)s)' % self.getProps()) 

        # 2-body inclusive decision
        ###################################################################
        charmOSTF2BodyIncSeq = self.__ostfFilter2BodyInc('CharmOSTF2BodyInc'
                                     , [charmOSTF2BodyInc]
                                     , extracode = "(SUMTREE(PT,('pi+'==ABSID),0.0) > %(OSTFDSumPtLL_2BodyInc)s) & (PT > %(OSTFDPtLL_2BodyInc)s) & (BPVIPCHI2() > %(OSTFDIPChi2LL_2BodyInc)s)" % self.getProps())

        # 3-body decision
        ###################################################################
        charmOSTF3BodySeq = self.__ostfFilter3Body('CharmOSTF3Body'
                                     , [charmOSTF3Body]
                                     , extracode = "(in_range(1800*MeV, M, 2040*MeV) | in_range(1943*MeV, M, 1993*MeV)) & (VFASPF(VCHI2PDOF) < %(OSTFVtxChi2UL_3Body)s) & ( SUMTREE(PT,(('pi+'==ABSID) | ('K+'==ABSID)),0.0) > %(OSTFDSumPtLL_3Body)s)" % self.getProps() )

        # Line for 2-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmOSTF2BodySBSeq = self.__ostfFilter2Body('CharmPostOSTF2BodyWideMass'
                                     , [charmOSTF2Body]
                                     , extracode = 'in_range(1700*MeV, M, 2100*MeV)')


        # Line for 3-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmOSTF3BodySBSeq = self.__ostfFilter3Body('CharmPostOSTF3BodyWideMass'
                                     , [charmOSTF3Body]
                                     , extracode = 'in_range(1700*MeV, M, 2100*MeV)')


        # Line creation for one stage track fit lines.
        ###################################################################
        charmOSNBodySeq = {  '2Body' : {  'Std'  : charmOSTF2BodySeq
                                        , 'Wide' : charmOSTF2BodySBSeq }
                           , '3Body' : {  'Std'  : charmOSTF3BodySeq
                                        , 'Wide' : charmOSTF3BodySBSeq }
                          }

        for charmSeq in charmOSNBodySeq.keys() :
            osLineName   = 'CharmOSTF' + charmSeq
            oswmTFLineName = 'CharmOSTF' + charmSeq + 'WideMass'
            self.__makeLine(osLineName, algos = [ charmOSNBodySeq[charmSeq]['Std'] ])
            self.__makeLine(oswmTFLineName, algos = [ charmOSNBodySeq[charmSeq]['Wide'] ])

        #Make the inclusive line!
        self.__makeLine("CharmOSTF2BodyInc",algos=[charmOSTF2BodyIncSeq])

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
        KshhHHparentcuts = """(VFASPF(VCHI2PDOF)< %(KshhTFHHVtxChi2UL)s )
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
        charmKshhTF2Body = bindMembers( 'CharmKshhTF2Body',  [PV3D(), lclKshhTFInputKaons, lclKshhTFInputPions, combineKshhTF2Body ] )


        ## Ks reconstruction
        ## These should probably be moved to SharedParticles
        ###################################################################
        KshhKsLLdaugcuts = { "pi+" : "(P> %(KshhTFKsLLTrkPLL)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KshhTFKsLLTrkPVIPChi2LL)s ) & (TRCHI2DOF< %(KshhTFKsLLTrkChi2UL)s)" % self.getProps() }
        KshhKsLLcombcuts = "ADAMASS('KS0')< %(KshhTFKsLLCombSymMassWin)s *MeV" % self.getProps()
        KshhKsLLparentcuts = """(VFASPF(VCHI2PDOF)< %(KshhTFKsLLVtxChi2UL)s ) 
                        & in_range( %(KshhTFKsLLVtxPVDispZLL)s *mm, BPVVDZ, %(KshhTFKsLLVtxPVDispZUL)s *mm) 
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
        charmKshhTFKsLL = bindMembers( "CharmKshhTFKsLL", [ PV3D(), BiKalmanFittedPions, combineKshhTFKsLL ] )


        ## Repeat for KsDD with independent cuts.
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions

        KshhKsDDdaugcuts = { "pi+" : "(P> %(KshhTFKsDDTrkPLL)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KshhTFKsDDTrkPVIPChi2LL)s ) & (TRCHI2DOF< %(KshhTFKsDDTrkChi2UL)s)" % self.getProps() }
        KshhKsDDcombcuts = "ADAMASS('KS0')< %(KshhTFKsDDCombSymMassWin)s *MeV" % self.getProps()
        KshhKsDDparentcuts = """(VFASPF(VCHI2PDOF)< %(KshhTFKsDDVtxChi2UL)s ) 
                        & in_range(%(KshhTFKsDDVtxPVDispZLL)s *mm, BPVVDZ, %(KshhTFKsDDVtxPVDispZUL)s *mm) 
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
        charmKshhTFKsDD = bindMembers( "CharmKshhTFKsDD", [ PV3D(), BiKalmanFittedDownPions, combineKshhTFKsDD ] )


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
#        D0MotherCut = "(VFASPF(VCHI2PDOF) < 100) & (BPVLTFITCHI2() < 100) & (BPVLTIME() > 0.1 * ps)"
        D0MotherCut = "(VFASPF(VCHI2PDOF) < 100) & (BPVLTFITCHI2() < 100) & (BPVLTIME() > %(D02MuMuMinLifeTime)s * ps)" % self.getProps()

        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons, BiKalmanFittedPions, BiKalmanFittedKaons

        D02MuMuCombine = Hlt2Member(CombineParticles
                                    , 'D02MuMuCombine'
                                    , DecayDescriptor = '[ D0 -> mu+ mu- ]cc'
                                    , InputLocations = [ BiKalmanFittedMuons ]
                                    , DaughtersCuts = { "mu+" : D0DaughterCut }
                                    , CombinationCut = D0CombinationCut
                                    , MotherCut = D0MotherCut)
        self.__makeLine('CharmD02MuMu'
                        , algos = [ PV3D(), BiKalmanFittedMuons, D02MuMuCombine ])

        D02PiPiForD02MuMuCombine = Hlt2Member(CombineParticles
                                              , 'D02PiPiForD02MuMuCombine'
                                              , DecayDescriptor = '[ D0 -> pi+ pi- ]cc'
                                              , InputLocations = [ BiKalmanFittedPions ]
                                              , DaughtersCuts = { "pi+" : D0DaughterCut }
                                              , CombinationCut = D0CombinationCut
                                              , MotherCut = D0MotherCut)
        self.__makeLine('CharmD02PiPiForD02MuMu'
                        , algos = [ PV3D(), BiKalmanFittedPions, D02PiPiForD02MuMuCombine ])

        D02KPiForD02MuMuCombine = Hlt2Member(CombineParticles
                                             , 'D02KPiForD02MuMuCombine'
                                             , DecayDescriptor = '[ D0 -> K+ pi- ]cc'
                                             , InputLocations = [ BiKalmanFittedKaons, BiKalmanFittedPions ]
                                             , DaughtersCuts = { "K+" : D0DaughterCut, "pi+" : D0DaughterCut }
                                             , CombinationCut = D0CombinationCut
                                             , MotherCut = D0MotherCut)
        self.__makeLine('CharmD02KPiForD02MuMu'
                        , algos = [ PV3D(), BiKalmanFittedKaons, BiKalmanFittedPions, D02KPiForD02MuMuCombine ])

        D02KKForD02MuMuCombine = Hlt2Member(CombineParticles
                                            , 'D02KKForD02MuMuCombine'
                                            , DecayDescriptor = '[ D0 -> K+ K- ]cc'
                                            , InputLocations = [ BiKalmanFittedKaons ]
                                            , DaughtersCuts = { "K+" : D0DaughterCut }
                                            , CombinationCut = D0CombinationCut
                                            , MotherCut = D0MotherCut)
        self.__makeLine('CharmD02KKForD02MuMu'
                        , algos = [ PV3D(), BiKalmanFittedKaons, D02KKForD02MuMuCombine ])

        from Hlt2SharedParticles.Ks import KsLLTF

        ## D -> KS0 Pi construction sequences.
        ###################################################################

        combineD2KS0PiLL = self.__DMesonCombine('CharmD2KS0PiLL'
                                        , [KsLLTF, BiKalmanFittedPions])

        ## Make the lines
        ###################################################################

        self.__makeLine('CharmD2KS0PiLL' , algos = [ PV3D(),combineD2KS0PiLL ])
