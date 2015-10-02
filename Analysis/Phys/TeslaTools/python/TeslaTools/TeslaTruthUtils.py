#    Functions to help with truth matching on Tesla output
#    NOTE: ONLY WORKS ON XDSTS !!!!!
#    Usage: 
# 
# from TeslaTools import TeslaTruthUtils 
# seq = TeslaTruthUtils.associateSequence("Tesla",False)
# relations = TeslaTruthUtils.getRelLoc("Tesla")
# TeslaTruthUtils.makeTruth(tuple, relations, [ "MCTupleToolKinematic" , "MCTupleToolHierarchy" , "MCTupleToolPID" ])
#    
#    where "Tesla" was the prefix used on the TES output location
#    when Tesla was ran

def getRelLoc(prefix):
    protos = prefix + "/Protos"
    relloc = "/Event/Turbo/Relations/Turbo/" + protos
    return relloc

def getNeutralRelLoc(line):
    return "/Event/Turbo/" + line + "/Relations/Turbo/NeutralPP2MC"

def associateSequence(prefix,debug):
    from Gaudi.Configuration import GaudiSequencer 
    from Configurables import TrackAssociator, ChargedPP2MC
    base = "/Event/Turbo/"
    protos = prefix + "/Protos"
    tracks = prefix + "/Tracks"
    protocont = base + protos
    trackcont = base + tracks
    relloc = "Relations/Turbo/" + protos
    assoctr = TrackAssociator(prefix+"AssocTr")
    assoctr.TracksInContainer = trackcont
    assocpp=ChargedPP2MC(prefix+"ProtoAssocPP")
    assocpp.RootInTES=base
    assocpp.TrackLocations = [ trackcont ]
    assocpp.InputData = [ protocont ]
    assocpp.OutputTable = relloc
    if debug == True:
        assocpp.OutputLevel = 2
        assoctr.OutputLevel = 2
    # Add it to a selection sequence
    seq = GaudiSequencer(prefix+'SeqP2MC')
    seq.Members += [assoctr,assocpp]
    return seq

def makeTruth(input,rels,toollist):
    from Configurables import TupleToolMCTruth, DaVinciSmartAssociator, P2MCPFromProtoP
    from Configurables import MCMatchObjP2MCRelator, TupleToolMCBackgroundInfo, BackgroundCategory
    MCTruth = TupleToolMCTruth() 
    MCTruth.ToolList = toollist
    
    #MCTruth.OutputLevel = 1
    
    input.addTool(MCTruth )
    input.TupleToolMCTruth.addTool(DaVinciSmartAssociator)   
    input.TupleToolMCTruth.DaVinciSmartAssociator.RedoNeutral=False 
    input.TupleToolMCTruth.DaVinciSmartAssociator.addTool(P2MCPFromProtoP)   
    input.TupleToolMCTruth.DaVinciSmartAssociator.P2MCPFromProtoP.Locations = rels
    input.TupleToolMCTruth.addTool(MCMatchObjP2MCRelator)
    input.TupleToolMCTruth.MCMatchObjP2MCRelator.RelTableLocations = rels
    
    input.TupleToolMCTruth.DaVinciSmartAssociator.addTool(BackgroundCategory)   
    input.TupleToolMCTruth.DaVinciSmartAssociator.BackgroundCategory.addTool(P2MCPFromProtoP)
    input.TupleToolMCTruth.DaVinciSmartAssociator.BackgroundCategory.vetoNeutralRedo=True
    input.TupleToolMCTruth.DaVinciSmartAssociator.BackgroundCategory.P2MCPFromProtoP.Locations = rels
    
    input.addTool(TupleToolMCBackgroundInfo)
    input.TupleToolMCBackgroundInfo.addTool(BackgroundCategory)
    input.TupleToolMCBackgroundInfo.BackgroundCategory.vetoNeutralRedo=True
    input.TupleToolMCBackgroundInfo.BackgroundCategory.addTool(P2MCPFromProtoP)
    input.TupleToolMCBackgroundInfo.BackgroundCategory.P2MCPFromProtoP.Locations= rels
