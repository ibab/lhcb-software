### @file
#
#  Hlt2 B->llK selection
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2009-07-21
#
##
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import Hlt2Line
from HltLine.HltLine import Hlt2Member

class Hlt2B2LLXLinesConf(HltLinesConfigurableUser) :
    """
    Definition for B->LLK lines.
    There are 4 Mondrian-type B->eeK lines (signal, J/psi, low mass, high mass)
    and as many for B->mumuK. All cuts are the same by construction.

    The Bu -> llK selection is an example of the 'mondrian' plot method
    for selecting several areas of parameter/phase space, each of which  has
    a distinct prescale factor corresponding to the fraction of events in
    this region which ought to be kept. This allows 'signal' areas to be
    selected with 100% efficiency, while 'background' areas are selected at x%
    efficiency in order to keep the output rate reasonable.

    The selectron strategy is to look for the presence of a dielectron or a dimuon,
    which has a rate a a few 100 Hz. Then to make a full track fit using the settings of the
    topological trigger and to rebuild a B->eeK candidate using the fitted tracks.
    """

    __slots__ = { 'Prescale'       : { 'Hlt2Bu2eeK'           :  1 # 0.01 # full box
                                     , 'Hlt2Bu2eeKJpsi'       :  1 # 0.1  # J/psi peak
                                     , 'Hlt2Bu2eeKHighMass'   :  1 # 0.2  # high mass band
                                     , 'Hlt2Bu2MuMuK'         :  1 # 0.01 # full box
                                     , 'Hlt2Bu2MuMuKJpsi'     :  1 # 0.1  # J/psi peak
                                     , 'Hlt2Bu2MuMuKHighMass' :  1 # 0.2  # high mass band
                                     }
                ,  'BFlightCHI2'        : 1   # 50        # adimentional 
                ,  'BDIRA'              : 0.5 # 0.9998    # adimentional
                ,  'BIPCHI2'            : 100 # 25        # adimentional  
                ,  'BVertexCHI2'        : 100 # 3         # adimentional
                ,  'LeptonPT'           : 0   # 1000      # MeV 
                ,  'KaonIPCHI2'         : 0   # 4         # adimentional
                ,  'KaonPT'             : 0   # 1400      # MeV 
                ,  'JpsiMassWindow'     : 500       # MeV (J/psi box mass window)
                ,  'HighMassBLowerMass' : 5400      # MeV (Lower bound of high-mass box)
                ,  'SignalBUpperMass'   : 5500      # MeV (Higher bound of signal box)
                   }
    


    def __apply_configuration__(self) :
        """
        The thing that defines it all
        """
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.GoodParticles import GoodKaons
        from Hlt2SharedParticles.DiMuon import DiMuon
        from Hlt2SharedParticles.DiElectron import DiElectron
                
        ###################################################################
        # CombineBu cuts
        #
        ### Kaon cut : (PT>1400*MeV) & (MIPCHI2DV(PRIMARY)>4)
        kaoncut = "(PT> %(KaonPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KaonIPCHI2)s )" % self.getProps()
        #
        ### lepton cut :(PT>1000*MeV)
        leptoncut = "(PT> %(LeptonPT)s *MeV)" % self.getProps()
        #
        ### dilepton mass cut 
        llmasscut   = "(AM < %(SignalBUpperMass)s *MeV)" % self.getProps()
        #
        ### dilepton cut (VFASPF(VCHI2/VDOF)<9)
        llcut   = "(VFASPF(VCHI2/VDOF)<9)" 
        # 
        ### B mass cuts : Hard-coded as we _need_ the full B mass window for the final fit. Nobody dare touch that!
        combcut = "(ADAMASS('B0')<600*MeV)"
        # 
        ### B candidate cuts : ((VFASPF(VCHI2/VDOF)<3) & (BPVIPCHI2()<25) & (BPVDIRA>0.9998) & (BPVVDCHI2>50))
        bcut   = "((VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s ) & (BPVIPCHI2()< %(BIPCHI2)s ) & (BPVDIRA> %(BDIRA)s ) & (BPVVDCHI2> %(BFlightCHI2)s ))" % self.getProps()

        ###################################################################
        #
        # CombineEE : make fitted dielectron
        #
        from Hlt2SharedParticles.TFBasicParticles import TFElectrons
        #
        combineEE = Hlt2Member( CombineParticles                               # type
                              , "EE"                                           # name -- to be bound to the line name: Hlt2LineNameFilter
                              , DecayDescriptor = "J/psi(1S) -> e+ e-"
                              , DaughtersCuts = { "e-" : leptoncut }
                              , CombinationCut = llmasscut
                              , MotherCut = llcut
                              , InputLocations = [ TFElectrons ]
                              )
        
        ###################################################################
        #
        # CombineMM : make fitted dimuon
        #
        from Hlt2SharedParticles.TFBasicParticles import TFMuons
        #
        combineMM = Hlt2Member( CombineParticles                               # type
                              , "MM"                                           # name -- to be bound to the line name: Hlt2LineNameFilter
                              , DecayDescriptor = "J/psi(1S) -> mu+ mu-"
                              , DaughtersCuts = { "mu-" : leptoncut }
                              , CombinationCut = llmasscut
                              , MotherCut = llcut
                              , InputLocations = [ TFMuons ]
                              )
        
        ###################################################################
        #
        # CombineBu : apply hard cuts
        #
        from Hlt2SharedParticles.TFBasicParticles import TFKaons
        combineB = Hlt2Member( CombineParticles                               # type
                             , "Bu"                                           # name -- to be bound to the line name: Hlt2LineNameFilter
                             , DecayDescriptor = "[ B+ -> J/psi(1S) K+ ]cc"
                             , DaughtersCuts = { "K+" : kaoncut }
                             , CombinationCut = combcut
                             , MotherCut = bcut
                             , InputLocations = [ TFKaons, combineEE ]
                             )
#        from Configurables import PrintHeader
#        DEBUGPrintHeader1 = PrintHeader("DEBUGPrintHeader1")
#        DEBUGPrintHeader2 = PrintHeader("DEBUGPrintHeader2")
        
        ###########################################################################
        # Define the Hlt2 Line
        #
        # 1) The 'whole' eeK line, precaled
        #
        line_M = Hlt2Line('Bu2eeK'
                          , prescale = self.prescale
# OLD                         ,  algos = [ DiElectron, GoodKaons , combineB ]
                          ,  algos = [ DiElectron,
                                       TFElectrons,
                                       combineEE,
                                       combineB ]
                          , postscale = self.postscale
                          , PV = True
                          )
        ###########################################################################
        #
        # 2) Only the signal box. No precale.
        #    That's ll mass < 3 GeV and B mass within 600 MeV
        #
        ###########################################################################
        line_S = line_M.clone(   'Bu2eeKSignal'
                                 , prescale = self.prescale
#                                 , EE = { "CombinationCut" : "(AM<3*GeV)" } # don't change that (tightens combcut and hence overwrites)
                                 , EE = { "CombinationCut" : "(AM<4*GeV)" } # don't change that (tightens combcut and hence overwrites)
                                 , Bu = { "CombinationCut" : combcut+" & (AM< %(SignalBUpperMass)s *MeV)" % self.getProps() }
                                 )
        ###########################################################################
        #
        # 3) Only the J/psi box. Prescale by 10%
        #    That's ll mass around the J/psi
        #    @todo One might consider applying the SignalBUpperMass cut as well.
        #
        ###########################################################################
        line_J = line_M.clone(   'Bu2eeKJpsi'
                                 , prescale = self.prescale
                                 , EE = {  "CombinationCut" : "(ADAMASS('J/psi(1S)')< %(JpsiMassWindow)s *MeV)" % self.getProps() } 
                                 )
        ###########################################################################
        #
        # 4) High mass combinatorics
        #    That's the high-mass side-band for any ll mass
        #
        ###########################################################################
        line_H = line_M.clone(   'Bu2eeKHighMass'
                                 , prescale = self.prescale
                                 , Bu = { "CombinationCut":"(AM> %(HighMassBLowerMass)s *MeV)" % self.getProps() }
                                 )
        ###########################################################################
        #
        # 5 ) Now do MuMu. There are 4 lines as well.
        #
        ###########################################################################
        #
        # The lines
        #
        ### TODO: allow reference to entries in Hlt2Line._algos 'by name'...
        ### with the above  the only real substitution would be  combineB --> 'Bu'
        ### where the 'object' is the original defined above, and the name refers to the named
        ### entry (potentially modified by clone) of 'self._algos' where self is the instance of the
        ### line...
        #
        line_M.clone('Bu2MuMuK'
                     , prescale = self.prescale
                     , algos = [ DiMuon, TFMuons, combineMM, line_M._algos[-1] ]  # 'Bu' ]
                     , Bu = { 'InputLocations' :  [ TFKaons, combineMM ]} 
                     )
        line_S.clone('Bu2MuMuKSignal'
                     , prescale = 1
                     , algos = [ DiMuon, TFMuons, combineMM, line_S._algos[-1] ] # 'Bu' ]
                     , MM = { "CombinationCut" : "(AM<3*GeV)" } # don't change that (tightens combcut and hence overwrites
                     , Bu = { 'InputLocations' :  [ TFKaons, combineMM ]} 
                     )
        line_J.clone('Bu2MuMuKJpsi'
                     , prescale = self.prescale
                     , algos = [ DiMuon, TFMuons, combineMM, line_J._algos[-1] ]# 'Bu' ]
                     , MM = {  "CombinationCut" : "(ADAMASS('J/psi(1S)')< %(JpsiMassWindow)s *MeV)" % self.getProps() } 
                     , Bu = {'InputLocations' :  [ TFKaons, combineMM ]} 
                     )
        line_H.clone('Bu2MuMuKHighMass'
                     , prescale = self.prescale
                     , algos = [ DiMuon, TFMuons, combineMM, line_H._algos[-1] ]# 'Bu' ]
                     , Bu = {'InputLocations' :  [ TFKaons, combineMM ]} 
                     )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2eeKDecision" : 50060 } )   # whole prescaled box
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2eeKSignalDecision" : 50061 } ) # signal box
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2eeKJpsiDecision" : 50062 } ) # j/psi box
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2eeKHighMassDecision" : 50063 } ) # high mass box
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2MuMuKDecision" : 50071 } )   # whole prescaled box
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2MuMuKSignalDecision" : 50072 } ) # signal box
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2MuMuKJpsiDecision" : 50073 } ) # j/psi box
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2MuMuKHighMassDecision" : 50074 } ) # high mass box
        
