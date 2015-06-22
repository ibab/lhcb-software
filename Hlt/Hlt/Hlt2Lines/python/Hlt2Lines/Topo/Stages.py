from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
class FilterEvent(Hlt2VoidFilter):
    """
    Global event filter.
    """
    def __init__(self):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules = Factory().Modules
        for module in ['LoKiTrigger.decorators']:
            if module not in modules: modules.append(module)
        from Inputs import Hlt2BiKalmanFittedForwardTracking
        inputs = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
        ec = "CONTAINS('%s') < %%(GEC_MAX)s" % inputs.outputSelection()
        Hlt2VoidFilter.__init__(self, 'TopoEvent', ec, [inputs],
                                shared = True)

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class FilterParts(Hlt2ParticleFilter):
    """
    Filter input particles.
    """
    def __init__(self, pid, inputs, gec):
        pc = ("(PT > %(TRK_PT_MIN)s) & (P > %(TRK_P_MIN)s) "
              "& (TRCHI2DOF < %(TRK_CHI2_MAX)s) "
              "& (MIPCHI2DV(PRIMARY) > %(TRK_IPCHI2_MIN)s)")
        if pid != 'Kaon': pc = ("(BPVLTIME('') > %(TRK_LT_MIN)s)")
        from HltTracking.HltPVs import PV3D
        deps = [FilterEvent(), PV3D('Hlt2')] if gec else [PV3D('Hlt2')]
        Hlt2ParticleFilter.__init__(self, 'TopoInput' + pid, pc, inputs,
                                    shared = True, dependencies = deps)

class FilterParts4(Hlt2ParticleFilter):
    """
    Filter for the topo4 input particles.
    """
    def __init__(self, inputs, gec):
        pc = ("(PT > %(TRK_PT_MIN)s) & (P > %(TRK_P_MIN)s) "
              "& (TRCHI2DOF < %(TRK_CHI2_MAX)s) "
              "& (MIPCHI2DV(PRIMARY) > 16)")
        from HltTracking.HltPVs import PV3D
        deps = [FilterEvent(), PV3D('Hlt2')] if gec else [PV3D('Hlt2')]
        Hlt2ParticleFilter.__init__(self, 'TopoInput4', pc, inputs,
                                    shared = True, dependencies = deps)

class FilterMVA(Hlt2ParticleFilter):
    """
    Apply the MVA filter to an n-body combo (also applies 1 < MCOR < 10 GeV)
    """
    def __init__(self, n, inputs, props, bdtcut_override=None, mu=False,
                 nickname=None, preambulo=None):
        params = props.get('BDT_%iBODY_PARAMS' % n)
        if params == None: params = props['BDT_PARAMS']
        params = '$PARAMFILESROOT/data/' + params
        bdtcut = props.get('BDT_%iBODY_MIN' % n)
        if bdtcut == None: bdtcut = props['BDT_MIN']
        if bdtcut_override: bdtcut = bdtcut_override
        varmap = props.get('BDT_%iBODY_VARMAP' % n)
        if varmap == None: varmap = props['BDT_VARMAP']
        if preambulo == None: preambulo = []
        bdt_name = "TrgBBDT"
        if nickname is not None:
            bdt_name += nickname
        bdttool = self.__classifier(params, varmap, bdt_name, preambulo)
        bdt = ("(VALUE('%s/%s') > %s)" %
               (bdttool.Type.getType(), bdttool.Name, bdtcut))
        pc = bdt
        if bdtcut_override == None:
            pc = ("(in_range(%(CMB_VRT_MCOR_MIN)s, BPVCORRM,"
                  " %(CMB_VRT_MCOR_MAX)s)) & " + bdt)
        if mu:
            pc = ("(NINTREE(HASPROTO & HASMUON & ISMUON) > 0)"
                  "& (in_range(%(CMB_VRT_MCOR_MIN)s, BPVCORRM,"
                  " %(CMB_VRT_MCOR_MAX)s)) & " + bdt)
        from HltTracking.HltPVs import PV3D
        name = 'TopoBDTFilter%d' % n
        if bdtcut_override: name += 'PreFilter'
        if mu:  name = 'TopoMuBDTFilter%d' % n
        if nickname == None: nickname = name
        if mu is False and bdtcut_override is not None:
            Hlt2ParticleFilter.__init__(self, name, pc, inputs, shared = True,
                                        tools = [bdttool],
                                        dependencies = [PV3D('Hlt2')])
        else:
            tos = 'HTOS'
            if mu: tos = 'MUTOS'
            Hlt2ParticleFilter.__init__(self, name, pc, inputs, shared = False,
                                        tools = [bdttool], tistos = tos,
                                        nickname = nickname,
                                        dependencies = [PV3D('Hlt2')])


    def __classifier(self, params, varmap, name, preambulo = []):
        from HltLine.HltLine import Hlt1Tool as Tool
        from Configurables import LoKi__Hybrid__DictOfFunctors as DictOfFunctors
        from Configurables import LoKi__Hybrid__DictValue as DictValue
        if params.endswith('.mx'): from Configurables import \
                LoKi__Hybrid__DictTransform_MatrixnetTransform_ as Transform
        else: from Configurables import \
                LoKi__Hybrid__DictTransform_BBDecTreeTransform_ as Transform
        tname      = 'Matrixnet' if params.endswith('.mx') else 'BBDecTree'
        key        = 'BDT'
        options    = {tname + 'File': params, 'Name': key, 'KeepVars': '0'}
        funcdict   = Tool(type = DictOfFunctors, name = 'TopoMVAdict',
                        Preambulo = preambulo, Variables = varmap)
        transform  = Tool(type = Transform, name = tname,
                          tools = [funcdict], Options = options,
                          Source = "LoKi::Hybrid::DictOfFunctors/TopoMVAdict")
        classifier = Tool(type = DictValue, name = name, tools = [transform],
                          Key = key, Source = 'LoKi::Hybrid::DictTransform'
                          '<' + tname + 'Transform>/' + tname)
        return classifier

from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from itertools import combinations_with_replacement
class CombineTos(Hlt2Combiner):
    """
    Creates all 2-body TOS (one-track or two-track) combos.
    """
    def __init__(self, inputs):
        from Hlt1Lines.Hlt1MVALines import Hlt1MVALinesConf
        props  = Hlt1MVALinesConf().getProps()
        pids   = ['K+', 'K-', 'KS0', 'Lambda0', 'Lambda~0']
        basic  = "(ABSID=='K+')"
        combos = list(combinations_with_replacement(pids, 2))
        decays = ['K*(892)0 -> ' + ' '.join(combo) for combo in combos]
        cc = ("(APT > %(CMB_PRT_PT_MIN)s) "
              "& (ANUM((ID=='KS0')|(ABSID=='Lambda0')) < 2) "
              "& (ACUTDOCACHI2(%(CMB_VRT_CHI2_MAX)s, '')) "
              "& ((AALLSAMEBPV | (AMINCHILD(MIPCHI2DV(PRIMARY)) > 16)) "
              "| (ANUM((ID == 'KS0') | (ABSID == 'Lambda0')) > 0)) "
              "& (AM < %(CMB_VRT_MCOR_MAX)s) "
              "& (ANUM(" + basic + " & (MIPCHI2DV(PRIMARY) < 16)) <"
              " %(CMB_TRK_NLT16_MAX)s) ")
        mc = ("(HASVERTEX)"
              "& (VFASPF(VCHI2) < %(CMB_VRT_CHI2_MAX)s) "
              "& (BPVVDCHI2 > %(CMB_VRT_VDCHI2_MIN)s) "
              "& (in_range(%(CMB_VRT_ETA_MIN)s, BPVETA,"
              " %(CMB_VRT_ETA_MAX)s)) "
              )
        from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
        merged = [Hlt2MergedStage('Topo2BodyTos', inputs, shared = True)]
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'Topo2BodyTos', decays,
                              merged, shared = True, tistos = 'TOS',
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc, MotherCut = mc)

class Combine3(Hlt2Combiner):
    def __init__(self, inputs):
        pids = ['K+', 'K-', 'KS0', 'Lambda0', 'Lambda~0']
        basic = "((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0'))"
        combos = list(combinations_with_replacement(pids, 1))
        decays = ['D*(2010)+  -> K*(892)0 ' + ' '.join(combo)
                  for combo in combos]
        cc = ("(APT > %(CMB_PRT_PT_MIN)s) "
              "& (ANUM((ID=='KS0')|(ABSID=='Lambda0')) < 2) "
              "& (ACUTDOCACHI2(%(CMB_VRT_CHI2_MAX)s, '')) "
              "& (AALLSAMEBPV | (AMINCHILD(MIPCHI2DV(PRIMARY)) > 16)) "
              "& (AM < %(CMB_VRT_MCOR_MAX)s) "
              "& ((ACHILD(NINTREE(" + basic + 
              " & (MIPCHI2DV(PRIMARY) < 16)),1)+ANUM(" + basic + 
              " & (MIPCHI2DV(PRIMARY) < 16))) < 2)")
        mc = ("(HASVERTEX) "
              "& (VFASPF(VCHI2) < %(CMB_VRT_CHI2_MAX)s) "
              "& (BPVVDCHI2 > %(CMB_VRT_VDCHI2_MIN)s) "
              "& (in_range(%(CMB_VRT_ETA_MIN)s, BPVETA,"
              " %(CMB_VRT_ETA_MAX)s)) "
              "& (BPVDIRA > %(CMB_VRT_DIRA_MIN)s)")
        from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
        merged = [Hlt2MergedStage('Topo3Body', inputs, shared = True)]
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'Topo3Body', decays, merged,
                              shared = True, dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc, MotherCut = mc)


class Combine4(Hlt2Combiner):
    """
    Combiner for all 4-body combos.
    """
    def __init__(self, inputs):
        pids = ['K+', 'K-']
        combos = list(combinations_with_replacement(pids, 1))
        basic = "((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0'))"
        decays = ['B0 -> D*(2010)+ ' + ' '.join(combo) for combo in combos]
        cc = ("(AM < %(CMB_VRT_MCOR_MAX)s) "
              "& (ACUTDOCACHI2(%(CMB_VRT_CHI2_MAX)s, '')) "
              "& (AALLSAMEBPV | (AMINCHILD(MIPCHI2DV(PRIMARY)) > 16)) "
              "& (APT > %(CMB_PRT_PT_MIN)s) ")
        mc = ("(HASVERTEX) "
              "& (VFASPF(VCHI2) < %(CMB_VRT_CHI2_MAX)s) "
              "& (BPVVDCHI2 > %(CMB_VRT_VDCHI2_MIN)s) "
              "& (in_range(%(CMB_VRT_ETA_MIN)s, BPVETA,"
              " %(CMB_VRT_ETA_MAX)s)) "
              "& (BPVDIRA > %(CMB_VRT_DIRA_MIN)s)")
        from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
        merged = [Hlt2MergedStage('Topo4Body', inputs, shared = True)]
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'Topo4Body', decays, merged,
                              shared = True, dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc, MotherCut = mc)
