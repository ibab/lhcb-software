from Configurables import GaudiSequencer as Sequence
from Configurables import HltConf
import Configurables

class ConfigOutputLevel( object ):
    def __init__( self, configurables, level ):
        self._configurables = configurables
        self._level = level

    def setOutputLevel( self ):
        """ Recursively set OutputLevel for configurables """
        seq = type( Sequence() )
        for configurable in self._configurables:
            if type( configurable ) != seq:
                typ = configurable.getType().replace( "::", "__" )
                name = configurable.name()
                conf = getattr( Configurables, typ )
                conf( name ).OutputLevel = self._level
            else:
                for member in configurable.Members:
                    if type( member ) == seq:
                        self.setOutputLevel( member, self._level )
                    else:
                        typ = member.getType().replace( "::", "__" )
                        name = member.name()
                        conf = getattr( Configurables, typ )
                        conf( name ).OutputLevel = self._level

class ConfigLFC( object ):
    def __init__( self, site ):
        self._site = site

    def setLFCSite( self ):
        from Configurables import COOLConfSvc
        COOLConfSvc().UseLFCReplicaSvc = True
        COOLConfSvc().LocalSite = self._site
            
class Config( object ):
    """ A class to reconfigure to run with specified Hlt lines """
    def __init__( self, hlt1Lines, hlt2Lines ):

        # Always run the Globas lines to avoid crashes and monitoring problems
        if not "Hlt1Global" in hlt1Lines:
            hlt1Lines.append( "Hlt1Global" )

        if not "Hlt2Global" in hlt2Lines:
            hlt2Lines.append( "Hlt2Global" )

        self.hlt1Lines = hlt1Lines
        self.hlt2Lines = hlt2Lines

    def postConfigAction( self ):
        from HltLine.HltLine import hlt1Lines
        from HltLine.HltLine import hlt2Lines

        # Reconfigure the Hlt sequences
        lines1 = [ i for i in hlt1Lines() if  i.name() in self.hlt1Lines ]
        print "HLT1LINES"
        for hltLine in lines1:
            print hltLine.name()
        Sequence( "Hlt1" ).Members = [ i.configurable() for i in lines1 ]
        lines2 = [ i for i in hlt2Lines() if  i.name() in self.hlt2Lines ]
        print "HLT2LINES"
        for hltLine in lines2:
            print hltLine.name()
        print "ENDLINES"
        Sequence( "Hlt2" ).Members = [ i.configurable() for i in lines2 ]

        # Reconfigure the monitoring to expect the correct lines
        HltConf().configureHltMonitoring( lines1, lines2 )

        for hltLine in Sequence( "Hlt1" ).Members + Sequence( "Hlt2" ).Members:
            try:
                prescale = getattr( hltLine, "Prescale" )
                if hasattr( prescale, "AcceptFraction" ):
                    prescale.AcceptFraction = 1
                elif hasattr( prescale, "Code" ):
                    prescale.Code = "FALL"
                postscale = getattr( hltLine, "Postscale" )
                if hasattr( postscale, "AcceptFraction" ):
                    postscale.AcceptFraction = 1
                elif hasattr( postscale, "Code" ):
                    postscale.Code = "FALL"
            except AttributeError:
                pass

        # This is rather nasty, but required. It is possible because we don't need
        # an output file.
        Sequence( "HltEndSequence" ).Members = []
