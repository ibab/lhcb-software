# Helper functions to configure complete MVAclassifier tool chains

# TMVA Value (to be used for cutting) 
def addTMVAclassifierValue(Component, XMLFile, Variables, Name) :
    from Configurables import LoKi__Hybrid__DictValue as DictValue
    Component.addTool(DictValue,Name)
    MVAResponse = getattr(Component,Name)
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
def addTMVAclassifierTuple(Component, XMLFile, Variables, Name) :
    from Configurables import LoKi__Hybrid__Dict2Tuple as Dict2Tuple
    Component.addTool(Dict2Tuple,Name)
    MVAResponse = getattr(Component,Name)
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
