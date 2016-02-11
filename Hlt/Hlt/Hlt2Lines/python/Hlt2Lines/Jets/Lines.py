from GaudiKernel.SystemOfUnits import MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class JetsLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale' : {},
                 'Postscale' : {},
                 'Common' : {
                     'GHOSTPROB' : 0.2,
                     'DPHI'      : 1.5,
                     'SV_VCHI2'  : 10,
                     'SV_TRK_PT' : 500*MeV,
                     'SV_TRK_IPCHI2' : 16,
                     'SV_FDCHI2' : 25,
                     'MU_PT'     : 1000*MeV,
                     'MU_PROBNNMU'  : 0.5}
             }
    
    def stages(self, nickname = ''):
        if hasattr(self, '_stages') and self._stages:
            if nickname: return self._stages[nickname]
            else: return self._stages
        else: self._stages = {}

        from Hlt2Lines.Topo.Lines import TopoLines
        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage

        topo_lines = TopoLines()
        topo_sv = Hlt2ExternalStage(topo_lines, topo_lines.stages('Topo2BodyCombos')[0])
     

        from Stages import FilterSV, FilterMuon, SVSVCombiner, SVMuCombiner, MuMuCombiner

        sv = [FilterSV([topo_sv])]
        muon = [FilterMuon()]

        svsv = [SVSVCombiner(sv)]
        svmu = [SVMuCombiner(sv+muon)]
        mumu = [MuMuCombiner(muon)]

        # TO-DO: 
        # once jet code is in, make jets and add SV and Mu tags to extrainfo;
        # make appropriate svsv, etc, required inputs so PF is only run when needed;
        # replace, eg svsv, below with the proper jets;
        # add prescaled pt > 7 GeV lines.

        self._stages['DiJetSV'] = sv
        self._stages['DiJetMu'] = muon
        self._stages['DiJetSVSV'] = svsv
        self._stages['DiJetSVMu'] = svmu
        self._stages['DiJetMuMu'] = mumu


        # Return the stages.
        if nickname: return self._stages[nickname]
        else: return self._stages

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc
        stages = self.stages()
        for (nickname, algos) in self.algorithms(stages):
            linename = nickname
            print '--> Adding', linename 
            Hlt2Line(linename, prescale = self.prescale, algos = algos, postscale = self.postscale)


        
        
        
