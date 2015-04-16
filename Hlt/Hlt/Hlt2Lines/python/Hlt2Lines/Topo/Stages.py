from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
class FilterEvent(Hlt2VoidFilter):
    """
    Filter for the topo events.
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
    Filter for the topo input particles.
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

class FilterCombos(Hlt2ParticleFilter):
    """
    Filter for the BDT lines.
    """
    def __init__(self, n, inputs, props):
        params = props.get('BDT_%iBODY_PARAMS' % n)
        if params == None: params = props['BDT_PARAMS']
        params = '$PARAMFILESROOT/data/' + params
        bdtcut = props.get('BDT_%iBODY_MIN' % n)
        if bdtcut == None: bdtcut = props['BDT_MIN']
        varmap = props.get('BDT_%iBODY_VARMAP' % n)
        if varmap == None: varmap = props['BDT_VARMAP']
        bdttool = self.__classifier(params, varmap, "TrgBBDT")
        pc = ("(VALUE('%s/%s') > %s)" % 
              (bdttool.Type.getType(), bdttool.Name, bdtcut))
        if n == 2:
            pc = ("(PT > %(CMB_PRT_PT_MIN)s) "
                  "& (VFASPF(VCHI2) < %(CMB_VRT_CHI2_MAX)s) "
                  "& (BPVVDCHI2 > %(CMB_VRT_VDCHI2_MIN)s) "
                  "& (in_range(%(CMB_VRT_ETA_MIN)s, BPVETA,"
                  " %(CMB_VRT_ETA_MAX)s)) "
                  "& (in_range(%(CMB_VRT_MCOR_MIN)s, BPVCORRM,"
                  " %(CMB_VRT_MCOR_MAX)s)) "
                  "& (BPVDIRA > %(CMB_VRT_DIRA_MIN)s) "
                  "& (NINTREE(ISBASIC & (MIPCHI2DV(PRIMARY) < 16)) <"
                  " %(CMB_TRK_NLT16_MAX)s) "
                  "& " + pc)
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'BDT', pc, inputs, shared = False, 
                                    tools = [bdttool], 
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
    Combiner for all 2-body HLT1 TOS combos.
    """
    def __init__(self, inputs):
        pids = ['K+', 'K-', 'KS0', 'Lambda0', 'Lambda~0']
        combos = list(combinations_with_replacement(pids, 2))
        decays = ['K*(892)0 -> ' + ' '.join(combo) for combo in combos]
        cc = ("(ACUTDOCACHI2(%(CMB_VRT_CHI2_MAX)s, '')) "
              "& (AALLSAMEBPV | (AMINCHILD(MIPCHI2DV(PRIMARY)) > 16))")
        mc = ("(HASVERTEX)")
        from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
        merged = [Hlt2MergedStage('Topo2BodyTos', inputs, shared = True)]
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'Topo2BodyTos', decays,
                              merged, shared = True, tistos = 'TOS',
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc, MotherCut = mc)

class CombineN(Hlt2Combiner):
    """
    Combiner for all n-body combos.
    """
    def __init__(self, n, inputs):
        pids = ['K+', 'K-', 'KS0', 'Lambda0', 'Lambda~0']
        combos = list(combinations_with_replacement(pids, n - 2))
        decays = ['B*0 -> K*(892)0 ' + ' '.join(combo) for combo in combos]
        cc = ("(APT > %(CMB_PRT_PT_MIN)s) "
              "& (AM < %(CMB_VRT_MCOR_MAX)s) "
              "& (ACUTDOCACHI2(%(CMB_VRT_CHI2_MAX)s, '')) "
              "& (AALLSAMEBPV | (AMINCHILD(MIPCHI2DV(PRIMARY)) > 16)) "
              "& (ANUM(ISBASIC & (MIPCHI2DV(PRIMARY) < 16)) <"
              " %(CMB_TRK_NLT16_MAX)s)")
        mc = ("(HASVERTEX) "
              "& (VFASPF(VCHI2) < %(CMB_VRT_CHI2_MAX)s) "
              "& (in_range(%(CMB_VRT_MCOR_MIN)s, BPVCORRM,"
              " %(CMB_VRT_MCOR_MAX)s)) "
              "& (in_range(%(CMB_VRT_ETA_MIN)s, BPVETA,"
              " %(CMB_VRT_ETA_MAX)s)) "
              "& (BPVVDCHI2 > %(CMB_VRT_VDCHI2_MIN)s) "
              "& (BPVDIRA > %(CMB_VRT_DIRA_MIN)s)")
        from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
        merged = [Hlt2MergedStage('Topo%iBody' % n, inputs, shared = True)]
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'Topo%iBody' % n, decays, merged,
                              shared = True, dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc, MotherCut = mc)
