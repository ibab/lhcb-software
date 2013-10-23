# Helper functions to configure an MVAclassifier tool chains

def TMVAclassifier(Options, Variables) :
    from Configurables import LoKi__Hybrid__DictTransform_TMVATransform_ as TMVAtransform
    from Configurables import LoKi__Hybrid__DictOfFunctors

    TMVA = TMVAtransform()
    TMVA.Options = Options
    TMVA.Source = "LoKi__Hybrid__DictOfFunctors/MVAdict"

    TMVA.addTool(LoKi__Hybrid__DictOfFunctors,"MVAdict")
    TMVA.MVAdict.Variables = Variables

    return TMVA

def TMVAclassifierValue(XMLFile, Variables) :
    from Configurables import LoKi__Hybrid__DictValue as DictValue
    MVAResponse = DictValue()
    Key = "BDT"
    MVAResponse.Key = Key
    MVAResponse.Source = "LoKi__Hybrid__DictTransform_TMVATransform_/TMVA"
    Options = {
        "XMLFile"    : XMLFile,
        "Name"       : Key, 
        "KeepVars"   : "0"}
    MVAResponse.addTool(TMVAclassifier(Options, Variables),"TMVA")
    return MVAResponse
