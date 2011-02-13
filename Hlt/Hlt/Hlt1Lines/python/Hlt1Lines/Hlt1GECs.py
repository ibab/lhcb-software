
def Hlt1GEC( reject = True ):
    from Configurables import Hlt__GEC
    from HltLine.HltDecodeRaw import DecodeIT,DecodeVELO
    from HltLine.HltLine import bindMembers, Hlt1Tool
    from Configurables import LoKi__VoidFilter as VoidFilter
    
    tool = Hlt__GEC("ActualGECs", MaxOTHits = 10000, MaxITHits = 3000, MaxVeloHits = 3000 )

    rej  = VoidFilter("Hlt1GECReject")
    rej.Code = "ACCEPT('%s')" % tool.getFullName()
    rej.Preambulo = [ 'ACCEPT = LoKi.Status.Accept' ]

    acc  = VoidFilter("Hlt1GECAccept")
    acc.Code = "~ ACCEPT('%s')" % tool.getFullName()
    acc.Preambulo = [ 'ACCEPT = LoKi.Status.Accept' ]

    return bindMembers(None, [ DecodeIT, DecodeVELO, rej if reject else acc  ] )

def HLT1GECStreamer( reject = True ) :
    bm = Hlt1GEC(reject)
    # TODO: automate....
    d1 = bm.members()[0].getFullName()
    d2 = bm.members()[1].getFullName()
    code = bm.members()[2].Code.replace('ACCEPT','LoKi.Status.Accept')
    return "GEC%s  = ( execute([ '%s', '%s']) & %s ) " % ( { True : 'Reject', False : 'Accept'}[reject] , d1, d2, code )
    
def Hlt1GECLoose( reject = True ):
    from Configurables import Hlt__GEC
    from HltLine.HltDecodeRaw import DecodeIT,DecodeVELO
    from HltLine.HltLine import bindMembers, Hlt1Tool
    from Configurables import LoKi__VoidFilter as VoidFilter
    
    tool = Hlt__GEC( "ActualGECsLoose", MaxOTHits = 10000, MaxITHits = 3000, MaxVeloHits = 10000 )

    rej  = VoidFilter("Hlt1GECRejectLoose")
    rej.Code = "ACCEPT('%s')" % tool.getFullName()
    rej.Preambulo = [ 'ACCEPT = LoKi.Status.Accept' ]

    acc  = VoidFilter("Hlt1GECAcceptLoose")
    acc.Code = "~ ACCEPT('%s')" % tool.getFullName()
    acc.Preambulo = [ 'ACCEPT = LoKi.Status.Accept' ]

    return bindMembers(None, [ DecodeIT, DecodeVELO, rej if reject else acc  ] )
    

def Hlt1GECLooseStreamer( reject = True ) :
    bm = Hlt1GECLoose(reject)
    # TODO: automate....
    d1 = bm.members()[0].getFullName()
    d2 = bm.members()[1].getFullName()
    code = bm.members()[2].Code.replace('ACCEPT','LoKi.Status.Accept')
    return "GEC%sLoose  = ( execute([ '%s', '%s']) & %s ) " % ( { True : 'Reject', False : 'Accept'}[reject] , d1, d2, code )
