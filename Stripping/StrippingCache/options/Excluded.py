from Gaudi.Configuration         import log
from StrippingConf.Configuration import StrippingConf

to_be_removed =  ( "BS2KSKS" , "MUNU" , "DISPLVERTICES" ) 

sc      = StrippingConf()
streams = sc.activeStreams()
streams = sc._streams

for stream in streams:
    
    lines = [] 
    for l in stream.lines:
        lname = l.name().upper()
        if 0 <= lname.find ( 'BS2KSKS'  ) :
            log.warning( "Removing" +l.name() )
            break
        if 0 <= lname.find ( 'MUNU'     ) :
            log.warning( "Removing" +l.name() )
            break
        if 0 <= lname.find ( 'DISPLVER' ) :
            log.warning( "Removing" +l.name() )
            break
        
        lines.append ( l )    
    stream.lines = lines 
