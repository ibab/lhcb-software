def Hlt1GECStreamer( gec = 'Loose', accept = True ):
    gecs = { 'Tight' : { 'MaxOTHits'   : 10000,
                         'MaxITHits'   :  3000,
                         'MaxVeloHits' :  3000 },
             'Loose' : { 'MaxOTHits'   : 10000,
                         'MaxITHits'   :  3000,
                         'MaxVeloHits' : 10000 } }
    from Configurables import Hlt__GEC
    from HltLine.HltDecodeRaw import DecodeIT,DecodeVELO
    from HltLine.HltLine import bindMembers
    cuts = gecs[ gec ]
    tool = Hlt__GEC("ActualGECs", **cuts )
    code = "ACCEPT( '%s' )" % tool.getFullName()
    if not accept: code = '~' + code
    bm = bindMembers( None, [ DecodeIT, DecodeVELO ] )
    return "GEC%s%s  = ( execute( %s ) & %s ) " % ( { True : 'Accept', False : 'Reject' }[ accept ], gec,
                                                    [ m.getFullName() for m in bm.members() ], code )

def Hlt1GECUnit( gec = 'Loose', accept = True ):
        from Configurables import LoKi__HltUnit as HltUnit
        mode = { True : 'Accept', False : 'Reject' }[ accept ]
        code = """
        GEC%s%s
        """ % ( mode, gec )
        return HltUnit( "GEC%sUnit" % gec,
                        Preambulo = [ Hlt1GECStreamer( gec, accept ) ],
                        Code = code )


