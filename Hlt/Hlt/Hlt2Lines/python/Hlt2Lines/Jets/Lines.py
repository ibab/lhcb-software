from GaudiKernel.SystemOfUnits import MeV, GeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

# Jet Lines written by Phil Ilten and Mike Williams (mwill@mit.edu),
# with Roel Aaij's significant assistance.

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
                     'MU_PROBNNMU'  : 0.5,
                     'JET_PT': 17*GeV},
                 
                 'JetBuilder' : {
                     'JetPtMin' : 5 * GeV,
                     'JetInfo' : False,
                     'JetEcPath' : ""
                     }
                 }

    def stages(self, nickname = ''):
        if hasattr(self, '_stages') and self._stages:
            if nickname: return self._stages[nickname]
            else: return self._stages
        else: self._stages = {}

        # Import the topo lines for the 2-body SVs.
        from Hlt2Lines.Topo.Lines import TopoLines
        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage
        from Hlt2Lines.Utilities.Hlt2JetBuilder import Hlt2JetBuilder

        topo_lines = TopoLines()
        topo_sv = Hlt2ExternalStage(topo_lines,
                                    topo_lines.stages('Topo2BodyCombos')[0])


        from Stages import (FilterSV, FilterMuon, SVSVCombiner, SVMuCombiner, 
                            MuMuCombiner, DiJetCombiner)

        # Create the stage filters.
        sv = [FilterSV([topo_sv])]
        mu = [FilterMuon()]
        svsv = [SVSVCombiner(sv)]
        svmu = [SVMuCombiner(sv + mu)]
        mumu = [MuMuCombiner(mu)]

        # The input tags correspond to 9600 + index (9600 SV, 9601 mu).
        jets = [Hlt2JetBuilder('DiJetsJetBuilder', sv + mu, shared = True,
                               nickname = 'JetBuilder')]

        # Create the stages.
        self._stages['JetsDiJetSVSV'] = [DiJetCombiner(svsv + jets, 9600, 9600)]
        self._stages['JetsDiJetSVMu'] = [DiJetCombiner(svmu + jets, 9600, 9601)]
        self._stages['JetsDiJetMuMu'] = [DiJetCombiner(mumu + jets, 9601, 9601)]
        self._stages['JetsDiJetSV']   = [DiJetCombiner(sv + jets, 9600)]
        self._stages['JetsDiJet']     = [DiJetCombiner(jets)]

        # Return the stages.
        if nickname: return self._stages[nickname]
        else: return {k : v for k, v in self._stages.iteritems()}

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc
        stages = self.stages()
        for (nickname, algos) in self.algorithms(stages):
            linename = nickname
            Hlt2Line(linename, prescale = self.prescale, algos = algos,
                     postscale = self.postscale)
