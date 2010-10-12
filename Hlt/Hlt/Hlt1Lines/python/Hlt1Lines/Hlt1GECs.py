
def Hlt1GEC( reject = True ):
    from Configurables import Hlt__GEC
    from HltLine.HltDecodeRaw import DecodeIT,DecodeVELO
    from HltLine.HltLine import bindMembers, Hlt1Tool
    from Configurables import LoKi__VoidFilter as VoidFilter
    
    # tool = Hlt1Tool( Hlt__GEC, "ActualGECs", MaxOTHits = 10000, MaxITHits = 3000, MaxVeloHits = 3000 )
    tool = Hlt__GEC("ActualGECs", MaxOTHits = 10000, MaxITHits = 3000, MaxVeloHits = 3000 )

    rej  = VoidFilter("Hlt1GECReject")
    rej.Code = "ACCEPT('%s')" % tool.getFullName()

    acc  = VoidFilter("Hlt1GECAccept")
    acc.Code = "~ ACCEPT('%s')" % tool.getFullName()

    return bindMembers(None, [ DecodeIT, DecodeVELO, rej if reject else acc  ] )


    
def Hlt1GECLoose( reject = True ):
    from Configurables import Hlt__GEC
    from HltLine.HltDecodeRaw import DecodeIT,DecodeVELO
    from HltLine.HltLine import bindMembers, Hlt1Tool
    from Configurables import LoKi__VoidFilter as VoidFilter
    
    #tool = Hlt1Tool( Hlt__GEC, "ActualGECsLoose", MaxOTHits = 10000, MaxITHits = 3000, MaxVeloHits = 4000 )
    Hlt__GEC( "ActualGECsLoose", MaxOTHits = 10000, MaxITHits = 3000, MaxVeloHits = 4000 )

    rej  = VoidFilter("Hlt1GECRejectLoose")
    rej.Code = "ACCEPT('%s')" % tool.getFullName()

    acc  = VoidFilter("Hlt1GECAcceptLoose")
    acc.Code = "~ ACCEPT('%s')" % tool.getFullName()

    return bindMembers(None, [ DecodeIT, DecodeVELO, rej if reject else acc  ] )
    

