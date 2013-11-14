# Helper functions to configure complete MVAclassifier tool chains

# TMVA Value (to be used for cutting)
def addTMVAclassifierValue(Component, XMLFile, Variables, ToolName) :
    from Configurables import LoKi__Hybrid__DictValue as DictValue
    Component.addTool(DictValue,ToolName)
    MVAResponse = getattr(Component,ToolName)
    Key = "BDT"
    MVAResponse.Key = Key
    MVAResponse.Source = "LoKi__Hybrid__DictTransform_TMVATransform_/TMVA"
    Options = {
        "XMLFile"    : XMLFile,
        "Name"       : Key,
        "KeepVars"   : "0"}

    from Configurables import LoKi__Hybrid__DictTransform_TMVATransform_ as TMVAtransform
    from Configurables import LoKi__Hybrid__DictOfFunctors

    MVAResponse.addTool(TMVAtransform,"TMVA")
    MVAResponse.TMVA.Options = Options
    MVAResponse.TMVA.Source = "LoKi__Hybrid__DictOfFunctors/MVAdict"
    MVAResponse.TMVA.addTool(LoKi__Hybrid__DictOfFunctors,"MVAdict")
    MVAResponse.TMVA.MVAdict.Variables = Variables
    # end of function

# TMVA tuple (to be added to the HybridTupleTool)
def addTMVAclassifierTuple(Branch, XMLFile, Variables,
                           Name="BDT", Keep=False, Preambulo=[]) :
    from Configurables import LoKi__Hybrid__Dict2Tuple as Dict2Tuple
    Branch.addTupleTool(Dict2Tuple,Name)
    MVAResponse = getattr(Branch,Name)
    #Key = "BDT"
    #MVAResponse.Key = Key

    MVAResponse.Source = "LoKi__Hybrid__DictTransform_TMVATransform_/TMVA"
    Options = {
        "XMLFile"    : XMLFile,
        "Name"       : Name,
        "KeepVars"   : "1" if Keep else "0"}

    from Configurables import LoKi__Hybrid__DictTransform_TMVATransform_ as TMVAtransform
    from Configurables import LoKi__Hybrid__DictOfFunctors

    MVAResponse.addTool(TMVAtransform,"TMVA")
    MVAResponse.TMVA.Options = Options
    MVAResponse.TMVA.Source = "LoKi__Hybrid__DictOfFunctors/MVAdict"
    MVAResponse.TMVA.addTool(LoKi__Hybrid__DictOfFunctors,"MVAdict")
    MVAResponse.TMVA.MVAdict.Preambulo = Preambulo
    MVAResponse.TMVA.MVAdict.Variables = Variables
    # end of function
