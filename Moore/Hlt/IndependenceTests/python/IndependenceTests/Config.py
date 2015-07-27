from Gaudi.Configuration import ApplicationMgr
from Configurables import GaudiSequencer as Sequence
from Configurables import ( HltConf,
                            NTupleSvc,
                            TupleHltDecReports )
import Configurables

class ConfigOutputLevel( object ):
    def __init__( self, classes, level ):
        self._classes = classes
        self._level = level

    def setOutputLevel( self ):
        """ Recursively set OutputLevel for configurables """
        for cl in self._classes:
            s = cl.split( "/" )
            if len( s ) == 1:
                typ, name = s, s
            else:
                typ, name = s
            conf = getattr( Configurables, typ )( name )
            self.setLevel( conf )

    def setLevel( self, conf ):
        seq = type( Sequence() )
        conf.OutputLevel = self._level
        if type( conf ) == seq:
            for member in conf.Members:
                self.setLevel( member )

class ConfigTupleAlg( object ):
    def __init__( self, name = None, filename = 'tuples.root' ):
        if name == None:
            self._algName = "TupleHltDecReports"
        else:
            self._algName = name
        self._filename = filename

    def addTupleAlg( self ):
        """ Add TupleHltDecReports to TopAlg """
        alg = TupleHltDecReports( self._algName )
        seq = Sequence( "Hlt" )
        algSeq = Sequence( "TupleSequence" )
        algSeq.Members = [ alg ]
        seq.Members += [ algSeq ]
        ApplicationMgr().HistogramPersistency = "ROOT"
        tupleFile = self._filename
        ApplicationMgr().ExtSvc +=  [ NTupleSvc() ]
        tup = "FILE1 DATAFILE='" + tupleFile + "' TYP='ROOT' OPT='NEW'"
        NTupleSvc().Output = [ tup ]
        NTupleSvc().OutputLevel = 1

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
        from HltConf.HltMonitoring import HltMonitoringConf
        HltMonitoringConf().configureHltMonitoring(lines1, lines2)
  
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
