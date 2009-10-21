### @file $Id: CheckerSeq.py,v 1.1 2009-10-21 16:40:33 pkoppenb Exp $
#
#  Create Hlt Checking sequence
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2009-10-21
#
##
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Id: CheckerSeq.py,v 1.1 2009-10-21 16:40:33 pkoppenb Exp $, $Revision: 1.1 $"
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
    def __init__(self,dv = None,s='Undefined', d='Undefined',ip=[],dt ='MC09'):
        """
        Defaults
        """
        self.DV = dv    # DaVinci configurable
        self.Signal = s
        self.Decay = d
        self.Input = ip
        self.DataType = dt

###############################################################################
    def configureDV(self,seq):
        """
        DaVinci settings
        """
        DV = self.DV
        DV.EvtMax = -1 
        DV.Hlt = True
        DV.Hlt2Requires = 'L0'
        DV.DataType = self.DataType
        DV.Simulation = True
        DV.TupleFile = "HLT-"+self.Signal+".root"
        DV.HistogramFile = "DVHlt2-"+self.Signal+".root"
        DV.MoniSequence += [ seq ]
        DV.Input = self.Input
###############################################################################
    def sequence(self):
        """
        The monitoring sequence
        """
        from Configurables import GaudiSequencer
        seq = GaudiSequencer("HltCheck"+self.Signal)
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
        ftt.MCDecayFinder.Decay = self.Decay
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
        if (self.Signal=='Undefined'):
            print "Error: Undefined Signal"
            return 

        seq = self.sequence()
        self.configureDV(seq)

        from Gaudi.Configuration import *
        MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
