
def Hlt1GEC( reject = True ):
    from Configurables import Hlt__GEC
    from HltLine.HltDecodeRaw import DecodeIT,DecodeVELO
    from HltLine.HltLine import bindMembers, Hlt1Tool
    from Configurables import LoKi__VoidFilter as VoidFilter
    
    tool = Hlt1Tool( Hlt__GEC, "ActualGECs", MaxOTHits = 10000, MaxITHits = 3000, MaxVeloHits = 3000 )

    rej  = VoidFilter("Hlt1GECReject")
    rej.Code = "ACCEPT('%s')" % tool.createConfigurable(rej).getFullName()

    acc  = VoidFilter("Hlt1GECAccept")
    acc.Code = "~ ACCEPT('%s')" % tool.createConfigurable(acc).getFullName()

    return bindMembers(None, [ DecodeIT, DecodeVELO, rej if reject else acc  ] )
    

    
