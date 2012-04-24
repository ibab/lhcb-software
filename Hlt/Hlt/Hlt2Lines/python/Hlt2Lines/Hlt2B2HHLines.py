from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2B2HHLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'BMassWinLow'         : 4700   # MeV
                   ,'BMassWinHigh'       : 5900   # MeV
                   ,'TrChi2'             : 3.      # 
                   ,'PionPT'             : 1000   # MeV
                   ,'PionIP'             : 0.12   # mm
                   ,'doca'               : 0.1    # mm
                   ,'BIP'                : 0.12   # mm
                   ,'BPT'                : 1200   # MeV/c
                   ,'BTAU'               : 0.0006 # mm
                   }
    


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions        
        
        _daughters_cut = "( TRCHI2DOF < %(TrChi2)s ) & ( PT > %(PionPT)s *MeV ) & ( MIPDV(PRIMARY) > %(PionIP)s )" % self.getProps()

        _combination_cut = "(AM>%(BMassWinLow)s*MeV) & (AM<%(BMassWinHigh)s*MeV) & ( AMAXDOCA('') < %(doca)s )" % self.getProps()

        _mother_cut = "( PT > %(BPT)s ) & (BPVIP() < %(BIP)s ) & ( BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(BTAU)s )" % self.getProps()


        Hlt2B2HH = Hlt2Member( CombineParticles 
                               , "Combine"     
                               , DecayDescriptor = "B0 -> pi+ pi-"
                               , DaughtersCuts = { 'pi+' : _daughters_cut }
                               , CombinationCut = _combination_cut
                               , MotherCut = _mother_cut
                               , Inputs = [ BiKalmanFittedPions.outputSelection() ])
        ###########################################################################
        # Define the Hlt2 Line
        #
        from HltTracking.HltPVs import PV3D
        line = Hlt2Line('B2HH'
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedPions, Hlt2B2HH ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHDecision" : 50080 } )
