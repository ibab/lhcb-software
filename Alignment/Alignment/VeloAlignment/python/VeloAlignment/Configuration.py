## @package VeloAlignment
#  Configurable for VeloAlignment package
#  @author Johan Blouw <johan.blouw@physi.uni-heidelberg.de>
#  @date   29/01/2009

from os      import environ
from os.path import expandvars

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from DetCond.Configuration import *
from TrackSys.Configuration import TrackSys
from Configurables import ( LHCbConfigurableUser, LHCbApp, GaudiSequencer, AlignTrTools )

class VeloAlignment( LHCbConfigurableUser ):
    INFO=3

    __version__="$Id: Configuration.py"
    __author__= "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"

    __slots__ = {
          "Sequencer" : GaudiSequencer("VeloAlignmentSequencer")          # the sequencer to add algorithms to
        
        , "WriteCondSubDetList"          : []                          # List of sub-detectors for which to write out the conditions
        , "ElementsToAlign"              : []                          # Elements to align

        }


    __used_configurables__ = [ AlignTrTools ]

    def __apply_configuration__(self):
        print "******* calling ", self.name()
        mainseq = self.getProp("Sequencer")
        mainseq.MeasureTime = True
        mainseq.getProperties()
        if  mainseq.name() == "" :
            mainseq = GaudiSequencer("Align")
            
        import  VeloAlignment.VeloAlignmentConf 
        if 'Module'  in self.getProp("ElementsToAlign") :
            va = VeloAlignment.VeloAlignmentConf.VAlign()
            print "Adding ", va.name(), " to sequence ", mainseq.name()
            mainseq.Members += [va]
            importOptions("$VELOALIGNMENTOPTS/VeloAlign.py")
            self.sequencers()




    def sequencers( self ) :
        print "**** setting the main Millepede Velo aligment sequence!"
        ## The main sequence
        mainSeq = self.getProp("Sequencer")
        mainSeq.MeasureTime = True

        # Different sequencers depending on whether we use pat or not
#        mainSeq.Members.append( self.alignmentSeq( self.getProp( "OutputLevel" ) ) )
#        mainSeq.Members.append( va )
#        mainSeq.Members.append( self.monitorSeq() )
#        if self.getProp( "NumIterations" ) > 1 :
#            mainSeq.Members.append( self.postMonitorSeq() )
            
        listOfCondToWrite = self.getProp( "WriteCondSubDetList" )
        if listOfCondToWrite:
            mainSeq.Members.append( self.writeSeq( listOfCondToWrite ) )

    
