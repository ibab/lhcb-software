"""Simple functions to steer raw event locations
"""

def setTriggerRawEventLocation(location="/Event/Trigger/RawEvent"):
    """
    After Brunel v41r0, there is a new smaller raw event with the
    trigger information, '/Event/Trigger/RawEvent'
    see task #217176
    This will be there on SDST and MicroDST
    """
    from Configurables import ( OdinTimeDecoder, ODINDecodeTool, HltDecReportsDecoder,
                                HltSelReportsDecoder, L0CaloCandidatesFromRaw,
                                L0DUFromRawAlg, L0DUFromRawTool, L0MuonCandidatesFromRaw,
                                L0MuonOutputs, DecodePileUpData )
    
    OdinTimeDecoder().addTool( ODINDecodeTool, 'ODINDecodeTool' )
    OdinTimeDecoder().ODINDecodeTool.RawEventLocation = location
    
    HltDecReportsDecoder().InputRawEventLocation = location
    HltSelReportsDecoder().InputRawEventLocation = location
    L0CaloCandidatesFromRaw().InputRawEventLocation = location
    
    L0DUFromRawAlg().addTool( L0DUFromRawTool, 'L0DUFromRawTool' )
    L0DUFromRawAlg().L0DUFromRawTool.RawLocation = location
    
    L0MuonCandidatesFromRaw().addTool( L0MuonOutputs, 'L0MuonOutputs' )
    L0MuonCandidatesFromRaw().L0MuonOutputs.InputRawEventLocation = location
    
    DecodePileUpData().RawEventLocation = location

