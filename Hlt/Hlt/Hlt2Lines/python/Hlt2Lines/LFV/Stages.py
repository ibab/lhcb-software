from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from HltTracking.HltPVs import PV3D
from Inputs import (Hlt2Muons, Hlt2Electrons)


class JpsiMuECombiner(Hlt2Combiner):
    def __init__(self, name):
        def daughter_cut(particle_short, particle_long):
            return ("(TRCHI2DOF < %({l}TrChi2DoF)s)" +
                    " & (TRGHOSTPROB < %({l}TrGhostProb)s)" +
                    " & (VFASPF(VCHI2PDOF) < %({l}VertexChi2DoF)s)" +
                    " & (PROBNN{s} > %({l}ProbNn)s)").format(s=particle_short,
                                                             l=particle_long)

        def asymmetric_mass_cut(functor, varname):
            return ("({f} > %({v}Low)s) & ({f} < %({v}High)s)".format(
                f=functor, v=varname))

        def spd_cut():
            return "(%(NSPD)s < CONTAINS('/Raw/Spd/Digis'))"

        dc = {'mu+': daughter_cut('mu', 'Muon'),
              'e-': daughter_cut('e', 'Electron')}
        # cc = ' & '.join((asymmetric_mass_cut('AM', 'CombMass'), spd_cut()))
        cc = asymmetric_mass_cut('AM', 'CombMass')
        mc = asymmetric_mass_cut('M', 'Mass')

        Hlt2Combiner.__init__(self,
                              'JpsiMuE',
                              '[J/psi(1S) -> mu+ e-]cc',
                              inputs=[Hlt2Muons, Hlt2Electrons],
                              DaughtersCuts=dc,
                              CombinationCut=cc,
                              MotherCut=mc,
                              Preambulo=[])


SelJpsiMuE = JpsiMuECombiner('JpsiMuE')
