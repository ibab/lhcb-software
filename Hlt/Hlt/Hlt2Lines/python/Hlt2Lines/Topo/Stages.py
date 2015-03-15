from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class FilterParts(Hlt2ParticleFilter):
    """
    Filter for the topo input particles.
    """
    def __init__(self, name, pid, inputs):
        pc = ("(PT > %%(ALL_PT_MIN)s) & (P > %%(ALL_P_MIN)s) "
              "& (MIPCHI2DV(PRIMARY) > %%(ALL_MIPCHI2DV_MIN)s) "
              "& ((TRCHI2DOF < %%(HAD_TRCHI2DOF_MAX)s) "
              "| ((TRCHI2DOF < %%(%s_TRCHI2DOF_MAX)s)" % pid)
        if   pid == 'MU': pc += " & (HASMUON & ISMUON)))"
        elif pid == 'E':  pc += " & (PIDe > %(PIDE_MIN)s)))"
        else:             pc += '))'
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'TopoInput' + name + pid, pc,
                                    inputs, shared = True, 
                                    dependencies = [PV3D('Hlt2')])

class FilterMforN(Hlt2ParticleFilter):
    """
    Filter m-body combos for an n-body line.
    """
    def __init__(self, tag, m, n, inputs):
        from GaudiKernel.SystemOfUnits import MeV
        if m == 2: pc = ("(M < %f) & (VFASPF(VCHI2) < %%(V2BODYCHI2_MAX)s)" %
                         (5000 * MeV if n == 4 else 6000 * MeV))
        else:      pc = '(M < %f)' % (6000 * MeV)
        Hlt2ParticleFilter.__init__(self, 'Topo%s%iFor%i' % (tag, m, n),
                                    pc, inputs, shared = True)

class FilterNforN(Hlt2ParticleFilter):
    """
    Filter n-body combos for an n-body line.
    """
    def __init__(self, tag, n, inputs):
        from GaudiKernel.SystemOfUnits import MeV
        pids = "(('K+' == ABSID) | ('KS0' == ID) | ('Lambda0' == ABSID))"
        pt = 4000.0 * MeV if n > 2 else 3000.0 * MeV
        pc = ("(SUMTREE(PT, %s, 0.0) > %f) & (NINTREE(('KS0' == ID) "
              "| ('Lambda0' == ABSID)) <= %%(NV0_%iBody_MAX)s) "
              "& (INTREE(ISBASIC "
              "& (MIPCHI2DV(PRIMARY) > %%(ONETRACK_IPCHI2_MIN)s) "
              "& ((PT > %%(ONETRACK_PT_MIN)s) | ((HASMUON & ISMUON) "
              "& (PT > %%(MU_ONETRACK_PT_MIN)s))))) & (MINTREE(HASTRACK "
              "& %s, TRCHI2DOF) < %%(MIN_TRCHI2DOF_MAX)s)"
              % (pids, pt, n, pids))
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'Topo%s%ifor%i' % (tag, n, n),
                                    pc, inputs, shared = True,
                                    tistos = 'TisTosSpec',
                                    dependencies = [PV3D('Hlt2')])

def PrepVarMap(inputs, varmap):
    """
    Format the variable map for the BBDecTreeTool.
    """
    from copy import deepcopy
    varmap = deepcopy(varmap)
    pids   = "(ABSID == '%s')" % inputs[0]
    for pid in inputs[1:]: pids += " | (ABSID == '%s')" % pid
    ptsum = "SUMTREE(PT, %s, 0.0)/MeV" % pids
    ptmin = "MINTREE(%s, PT)/MeV" % pids
    varmap["PTSUM"] = ptsum
    varmap["PTMIN"] = ptmin
    return varmap

class FilterSimple(Hlt2ParticleFilter):
    """
    Filter for the simple lines.
    """
    def __init__(self, n, inputs, varmap):
        from Configurables import BBDTSimpleTool
        from Configurables import LoKi__Hybrid__DictOfFunctors
        from HltLine.HltLine import Hlt1Tool
        varmap  = PrepVarMap(['K+'], varmap)
        handler = Hlt1Tool(type = LoKi__Hybrid__DictOfFunctors, 
                           name = 'VarHandler', Variables = varmap)
        bdttool = Hlt1Tool(type = BBDTSimpleTool, name = 'TrgSimple',
                           Cuts = [], ParticleDictTool = 
                           'LoKi::Hybrid::DictOfFunctors/' + handler.Name,
                           tools = [handler])
        pc = "FILTER('BBDTSimpleTool/TrgSimple')"
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'Simple', pc, inputs, shared = False, 
                                    dependencies = [PV3D('Hlt2')],
                                    tools = [bdttool])

class FilterBDT(Hlt2ParticleFilter):
    """
    Filter for the BDT lines.
    """
    def __init__(self, tag, n, inputs, varmap, params):
        pids = ['K+', 'KS0', 'Lambda0', 'Lambda~0']
        if tag == 'RAD' and n == 3: pids.append('gamma')
        varmap  = PrepVarMap(pids, varmap)
        params  = ('$PARAMFILESROOT/data/Hlt2Topo%dBody_BDTParams_%s.txt' % 
                   (n, params))
        tool    = self.__classifier(params, varmap, "TrgBBDT")
        pc = ("(VALUE('%s/%s') > %%(BDT_%iBODY%s_MIN)s)" % 
              (tool.Type.getType(), tool.Name, n, tag))
        if   tag == 'MU': pc += " & INTREE(HASPROTO & HASMUON & ISMUON)"
        elif tag == 'E':  pc += " & INTREE(HASPROTO & (PIDe > %(PIDE_MIN)s))"
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'BDT', pc, inputs,
                                    shared = False, tools = [tool],
                                    dependencies = [PV3D('Hlt2')])

    def __classifier(self, params, varmap, name, preambulo = []) :
        from HltLine.HltLine import Hlt1Tool as Tool
        from Configurables import LoKi__Hybrid__DictOfFunctors as DictOfFunctors
        from Configurables import LoKi__Hybrid__DictValue as DictValue
        from Configurables import \
            LoKi__Hybrid__DictTransform_BBDecTreeTransform_ as Transform
        key        = 'BDT'
        options    = {'BBDecTreeFile': params, 'Name': key, 'KeepVars': '0'}
        funcdict   = Tool(type = DictOfFunctors, name = 'MVAdict',
                        Preambulo = preambulo, Variables = varmap)
        transform  = Tool(type = Transform, name = 'BBDecTree',
                          tools = [funcdict], Options = options, 
                          Source = "LoKi::Hybrid::DictOfFunctors/MVAdict")
        classifier = Tool(type = DictValue, name = name, tools = [transform],
                          Key = key, Source = 'LoKi::Hybrid::DictTransform'
                          '<BBDecTreeTransform>/BBDecTree')
        return classifier

from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
class CombineN(Hlt2Combiner):
    """
    Combiner for all n-body combos.
    """
    def __init__(self, tag, n, inputs):
        if n == 3: decays = [
            "D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-",
            "D*(2010)+ -> K*(892)0 KS0", "D*(2010)+ -> K*(892)0 KS0",
            "D*(2010)+ -> K*(892)0 Lambda0", "D*(2010)+ -> K*(892)0 Lambda~0"]
        elif n == 4: decays = [
            "B0 -> D*(2010)+ K-", "B0 -> D*(2010)+ K+", "B0 -> D*(2010)+ KS0",
            "B0 -> D*(2010)+ Lambda0", "B0 -> D*(2010)+ Lambda~0"]
        else: decays = [
            "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-",
            "K*(892)0 -> K+ KS0", "K*(892)0 -> K- KS0",
            "K*(892)0 -> K+ Lambda0", "K*(892)0 -> K- Lambda0",
            "K*(892)0 -> K+ Lambda~0", "K*(892)0 -> K- Lambda~0",
            "K*(892)0 -> KS0 KS0"]
        cc = ("(AM < 7000*MeV) & ((ANUM((ID=='KS0')|(ABSID=='Lambda0'))>0) "
              "| ((AALLSAMEBPV | (AMINCHILD(MIPCHI2DV(PRIMARY)) > 16)) "
              "& (AMAXDOCA('LoKi::DistanceCalculator') < %(AMAXDOCA_MAX)s)))")
        mc = "(BPVDIRA > 0) & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s)"
        if tag == 'RAD': decays = ['B+ -> K*(892)0  gamma']; cc = "AM > 0"
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, 'Topo%s%iBody' % (tag, n), decays,
                              inputs, shared = True, 
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc, MotherCut = mc)
