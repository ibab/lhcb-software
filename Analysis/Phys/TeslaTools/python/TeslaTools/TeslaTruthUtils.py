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
    base = "/Event/"
    protos = prefix + "/Protos"
    tracks = prefix + "/Tracks"
    protocont = base + protos
    trackcont = base + tracks
    relloc = "Relations/" + protos
    return relloc

def associateSequence(prefix,debug):
    from Gaudi.Configuration import * 
    from Configurables import TrackAssociator, ChargedPP2MC
    base = "/Event/"
    protos = prefix + "/Protos"
    tracks = prefix + "/Tracks"
    protocont = base + protos
    trackcont = base + tracks
    relloc = "Relations/" + protos
    assoctr = TrackAssociator(prefix+"AssocTr")
    assoctr.TracksInContainer = trackcont
    assocpp=ChargedPP2MC(prefix+"ProtoAssocPP")
    assocpp.TrackLocations = [ trackcont ]
    assocpp.InputData = [ protocont ]
    assocpp.OutputTable = relloc
    if debug == True:
        assocpp.OutputLevel = DEBUG
    # Add it to a selection sequence
    seq = GaudiSequencer(prefix+'SeqP2MC')
    seq.Members += [assoctr,assocpp]
    return seq

def makeTruth(input,rel,toollist):
    from Gaudi.Configuration import * 
    from Configurables import TupleToolMCTruth, DaVinciSmartAssociator, P2MCPFromProtoP
    from Configurables import MCMatchObjP2MCRelator, TupleToolMCBackgroundInfo, BackgroundCategory
    MCTruth = TupleToolMCTruth() 
    MCTruth.ToolList = toollist
    input.addTool(MCTruth )
    rellocations=[ rel ]
    input.TupleToolMCTruth.addTool(DaVinciSmartAssociator)   
    input.TupleToolMCTruth.DaVinciSmartAssociator.addTool(P2MCPFromProtoP)   
    input.TupleToolMCTruth.DaVinciSmartAssociator.P2MCPFromProtoP.Locations = [rel]
    input.TupleToolMCTruth.DaVinciSmartAssociator.addTool(BackgroundCategory)   
    input.TupleToolMCTruth.DaVinciSmartAssociator.BackgroundCategory.addTool(P2MCPFromProtoP)
    input.TupleToolMCTruth.DaVinciSmartAssociator.BackgroundCategory.P2MCPFromProtoP.Locations = [rel]
    input.TupleToolMCTruth.addTool(MCMatchObjP2MCRelator)
    input.TupleToolMCTruth.MCMatchObjP2MCRelator.RelTableLocations = [rel]
    input.addTool(TupleToolMCBackgroundInfo)
    input.TupleToolMCBackgroundInfo.addTool(BackgroundCategory)
    input.TupleToolMCBackgroundInfo.BackgroundCategory.addTool(P2MCPFromProtoP)
    input.TupleToolMCBackgroundInfo.BackgroundCategory.P2MCPFromProtoP.Locations=rellocations  
