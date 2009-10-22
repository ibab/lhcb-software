## @file $Id: CheckerSeq.py,v 1.2 2009-10-22 16:21:33 pkoppenb Exp $
#
#  Create Hlt Checking sequence
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2009-10-21
#
##
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Id: CheckerSeq.py,v 1.2 2009-10-22 16:21:33 pkoppenb Exp $, $Revision: 1.2 $"
# =============================================================================
###############################################################################
#
# CheckerSeq
#
class CheckerSeq :
    """
    Configures a DaVinci job with all the needed stuff for
    Hlt Checking
    """

###############################################################################
    def __init__(self,
                 DV       = None,
                 Signal   ='Undefined',
                 Decay    ='Undefined',
                 Input    =[],
                 DataType ='MC09'):
        """
        Defaults
        """
        self._DV = DV    # DaVinci configurable
        self._Signal = Signal
        self._Decay = Decay
        self._Input = Input
        self._DataType = DataType

###############################################################################
    def configureDV(self,seq):
        """
        DaVinci settings
        """
        DV = self._DV
        DV.EvtMax = 1000 
        DV.Hlt = True
        DV.Hlt2Requires = 'L0'
        DV.DataType = self._DataType
        DV.Simulation = True
        DV.TupleFile = "HLT-"+self._Signal+".root"
        DV.HistogramFile = "DVHlt2-"+self._Signal+".root"
        DV.MoniSequence += [ seq ]
        DV.Input = self._Input
###############################################################################
    def sequence(self):
        """
        The monitoring sequence
        """
        from Configurables import GaudiSequencer
        seq = GaudiSequencer("HltCheck"+self._Signal)
        seq.Context = "HLT"
        seq.IgnoreFilterPassed = True

        ftt = self.trueTracks()
        seq.Members.append(ftt)
        seq.Members.append( self.correlations( ftt.name() ))
        seq.Members.append( self.eventTuple() )
        return seq
    
###############################################################################
    def eventTuple(self):
        """
        Event Tuple
        """
        from Configurables import EventTuple, TupleToolGeneration, TupleToolTrigger
        tuple = EventTuple("TriggerTuple")
        tuple.ToolList = [ "TupleToolEventInfo",   #  "TupleToolGeneration",
                           "TupleToolTrigger", "MCTupleToolEventType",
                           "TupleToolTriggerRecoStats" ]
        tuple.addTool( TupleToolTrigger )
        tuple.TupleToolTrigger.VerboseHlt1 = True
        tuple.TupleToolTrigger.VerboseHlt2 = True
        return tuple
        
###############################################################################
    def trueTracks(self):
        """
        True Tracks
        """
        from Configurables import FilterTrueTracks, MCDecayFinder
        ftt = FilterTrueTracks()
        ftt.TracksPath = [ "Hlt/Track/Long" ]
        ftt.OutputPath = "Hlt/Track/Signal"
        ftt.addTool(MCDecayFinder)
        ftt.MCDecayFinder.Decay = self._Decay
        return ftt
    
###############################################################################
    def correlations(self,FTT):
        """
        Correlations
        """
        from Configurables import HltCorrelations
        h = HltCorrelations("Hlt2SelectionsCorrs", Algorithms = [ FTT ]) 
        return h

###############################################################################
    def configure(self):
        """
        Does everything
        """
        if (self._Signal=='Undefined'):
            print "Error: Undefined Signal"
            return 

        seq = self.sequence()
        self.configureDV(seq)

        from Gaudi.Configuration import *
        MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
