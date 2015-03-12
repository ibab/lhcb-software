from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class InPartFilter(Hlt2ParticleFilter):
    """
    Filter for the input particles for the topo lines.
    """
    def __init__(self, name, inputs):
        pc = ("(PT > %(ALL_PT_MIN)s) & (P > %(ALL_P_MIN)s) "
              "& (MIPCHI2DV(PRIMARY) > %(ALL_MIPCHI2DV_MIN)s) "
              "& (((TRCHI2DOF < %(MU_TRCHI2DOF_MAX)s) & (HASMUON & ISMUON)) "
              "| (TRCHI2DOF < %(HAD_TRCHI2DOF_MAX)s))")
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, pc, inputs, shared = True, 
                                    dependencies = [PV3D('Hlt2')])

from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
class AllNBody(Hlt2Combiner):
    """
    Combiner for all n-body combinations.
    """
    def __init__(self, tag, n, inputs):
        name = 'NewTopo%dBodyComb%s' % (n, tag)
        decays = [
            ["K*(892)0 -> K+ K+","K*(892)0 -> K+ K-","K*(892)0 -> K- K-",
             "K*(892)0 -> K+ KS0","K*(892)0 -> K- KS0",
             "K*(892)0 -> K+ Lambda0","K*(892)0 -> K- Lambda0",
             "K*(892)0 -> K+ Lambda~0","K*(892)0 -> K- Lambda~0",
             "K*(892)0 -> KS0 KS0"],
            ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-",
             "D*(2010)+ -> K*(892)0 KS0", "D*(2010)+ -> K*(892)0 KS0",
             "D*(2010)+ -> K*(892)0 Lambda0",
             "D*(2010)+ -> K*(892)0 Lambda~0"],
            ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+",
             "B0 -> D*(2010)+ KS0",
             "B0 -> D*(2010)+ Lambda0","B0 -> D*(2010)+ Lambda~0"]
            ]
        cc = ("(AM < 7000*MeV) & ((ANUM((ID=='KS0')|(ABSID=='Lambda0'))>0) "
              "| ((AALLSAMEBPV | (AMINCHILD(MIPCHI2DV(PRIMARY)) > 16)) "
              "& (AMAXDOCA('LoKi::DistanceCalculator') < %(AMAXDOCA_MAX)s)))")
        mc = "(BPVDIRA > 0) & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s)"
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decays[n - 2], inputs, shared = True,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc, MotherCut = mc)

class FilterNforN(Hlt2ParticleFilter):
    """
    Filter an n-body combination for an n-body line.
    """
    def __init__(self, tag, n, inputs):
        from GaudiKernel.SystemOfUnits import MeV
        name = 'NewTopo%d%sFilterNforN' % (n, tag)
        pids = "(('K+' == ABSID) | ('KS0' == ID) | ('Lambda0' == ABSID))"
        pt = 4000 * MeV if n > 2 else 3000 * MeV
        pc = ("(SUMTREE(PT, %s, 0.0) > %f) & (NINTREE(('KS0' == ID) "
              "| ('Lambda0' == ABSID)) <= %%(NV0_%iBody_MAX)s) "
              "& (INTREE(ISBASIC "
              "& (MIPCHI2DV(PRIMARY) > %%(ONETRACK_IPCHI2_MIN)s) "
              "& ((PT > %%(ONETRACK_PT_MIN)s) | ((HASMUON & ISMUON) "
              "& (PT > %%(MU_ONETRACK_PT_MIN)s))))) & (MINTREE(HASTRACK "
              "& %s, TRCHI2DOF) < %%(MIN_TRCHI2DOF_MAX)s)"
              % (pids, pt, n, pids))
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, pc, inputs, shared = True, 
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
    def __init__(self, tag, n, inputs, varmap):
        from Configurables import BBDTSimpleTool
        from Configurables import LoKi__Hybrid__DictOfFunctors
        from HltLine.HltLine import Hlt1Tool
        name    = 'NewTopo%d%sFilterSimple' % (n, tag)
        varmap  = PrepVarMap(['K+'], varmap)
        handler = Hlt1Tool(type = LoKi__Hybrid__DictOfFunctors, 
                           name = 'VarHandler', Variables = varmap)
        bdttool = Hlt1Tool(type = BBDTSimpleTool, name = 'TrgSimple',
                           Cuts = [], ParticleDictTool = 
                           'LoKi::Hybrid::DictOfFunctors/' + handler.Name,
                           tools = [handler])
        pc = "FILTER('BBDTSimpleTool/TrgSimple')"
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, pc, inputs, shared = False, 
                                    dependencies = [PV3D('Hlt2')],
                                    tools = [bdttool])

class FilterBDT(Hlt2ParticleFilter):
    """
    Filter for the BDTlines.
    """
    def __init__(self,tag, n, inputs, varmap, params, threshold):
        from Configurables import BBDecTreeTool
        from Configurables import LoKi__Hybrid__DictOfFunctors
        from HltLine.HltLine import Hlt1Tool
        params  = 'Hlt2Topo%dBody_BDTParams_%s.txt' % (n, params)
        varmap  = PrepVarMap(['K+', 'KS0', 'Lambda0', 'Lambda~0'], varmap)

        classifierTool = self.__classifierTool("$PARAMFILESROOT/data/" + params, varmap, "TrgBBDT")
        pc = "(VALUE('{0}/{1}') > %({2})s)".format(classifierTool.Type.getType(), classifierTool.Name, threshold)
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, 'BDT', pc, inputs, shared = False, 
                                    dependencies = [PV3D('Hlt2')],
                                    tools = [classifierTool])

    def __classifierTool(self, BBDecTreeFile, Variables, ToolName, Preambulo = []) :
        from HltLine.HltLine import Hlt1Tool as Tool
        from Configurables import LoKi__Hybrid__DictValue as DictValue

        from Configurables import LoKi__Hybrid__DictOfFunctors
        source = Tool(type = LoKi__Hybrid__DictOfFunctors, name = "MVAdict",
                      Preambulo = Preambulo, Variables = Variables)

        Key = "BDT"
        Options = {
            "BBDecTreeFile"    : BBDecTreeFile,
            "Name"       : Key,
            "KeepVars"   : "0"}

        from Configurables import LoKi__Hybrid__DictTransform_BBDecTreeTransform_ as BBDecTreetransform
        bbdtTool = Tool(type = BBDecTreetransform, name = "BBDecTree", tools = [source],
                        Options = Options, Source = "LoKi::Hybrid::DictOfFunctors/MVAdict")
            
        tool = Tool(type = DictValue, name = ToolName, tools = [bbdtTool], Key = Key, 
                     Source = "LoKi::Hybrid::DictTransform<BBDecTreeTransform>/BBDecTree")
        return tool
