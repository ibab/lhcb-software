## @file $Id: CheckerSeq.py,v 1.6 2010-05-18 11:48:45 pkoppenb Exp $
#
#  Create Hlt Checking sequence
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2009-10-21
#
##
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Id: CheckerSeq.py,v 1.6 2010-05-18 11:48:45 pkoppenb Exp $, $Revision: 1.6 $"
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
                 Decay    ='',
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
        DV.Hlt2Requires = 'L0+Hlt1'
        DV.DataType = self._DataType
        if ( self._DataType == 'MC09' ):
            DV.CondDBtag = 'MC09-20100430-vc-md100'
        DV.ReplaceL0BanksWithEmulated = True 
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

        if (self._Decay):
            ftt = self.trueTracks(self._Decay)
            seq.Members.append(ftt)
            seq.Members.append( self.correlations( [ ftt.name() ] ))
        else :
            seq.Members.append( self.correlations( [  ] ))
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
    def trueTracks(self,decay):
        """
        True Tracks
        """
        from Configurables import FilterTrueTracks, MCDecayFinder
        ftt = FilterTrueTracks()
        ftt.TracksPath = [ "Hlt2/Track/Unfitted/Forward" ]
        ftt.OutputPath = "Hlt2/Track/Unfitted/Signal"
        ftt.addTool(MCDecayFinder)
        ftt.MCDecayFinder.Decay = decay
#        ftt.OutputLevel = 2 
        return ftt
    
###############################################################################
    def correlations(self,FTT):
        """
        Correlations
        """
        from Configurables import HltCorrelations
        h = HltCorrelations("Hlt2SelectionsCorrs", Algorithms = FTT ) 
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

        import Gaudi.Configuration
        from Configurables import MessageSvc
        MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
