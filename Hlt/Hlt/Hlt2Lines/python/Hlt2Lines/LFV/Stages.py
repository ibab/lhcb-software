from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Inputs import (Hlt2Muons, Hlt2Electrons)


class SpdCut(Hlt2VoidFilter):
    def __init__(self, name):
        Hlt2VoidFilter.__init__(self,
                                'LFV{}'.format(name),
                                "(CONTAINS('/Raw/Spd/Digits') < %(NSPD)s)",
                                [],
                                shared=True,
                                nickname='SpdCut')


class JpsiMuECombiner(Hlt2Combiner):
    def __init__(self, name):
        def daughter_cut(particle_short, particle_long):
            return """(TRCHI2DOF < %({l}TrChi2DoF)s)
                    & (TRGHOSTPROB < %({l}TrGhostProb)s)
                    & (PROBNN{s} > %({l}ProbNn)s)""".format(s=particle_short,
                                                            l=particle_long)

        def asymmetric_mass_cut(functor, varname):
            return "in_range(%({v}Low)s, {f}, %({v}High)s)".format(f=functor,
                                                                     v=varname)

        dc = {'mu+': daughter_cut('mu', 'Muon'),
              'e-': daughter_cut('e', 'Electron')}
        cc = asymmetric_mass_cut('AM', 'CombMass')
        mc = ' & '.join((asymmetric_mass_cut('M', 'Mass'), '(CHI2VXNDOF < %(VertexChi2DoF)s)'))

        Hlt2Combiner.__init__(self,
                              'JpsiMuE',
                              '[J/psi(1S) -> mu+ e-]cc',
                              inputs=[Hlt2Muons, Hlt2Electrons],
                              dependencies=[SpdCut('JpsiMuESpdCut')],
                              DaughtersCuts=dc,
                              CombinationCut=cc,
                              MotherCut=mc,
                              Preambulo=[])


SelJpsiMuE = JpsiMuECombiner('JpsiMuE')
