"""
High level configuration example for a typical physics MicroDST
"""
__version__ = "$Id: Configuration.py,v 1.2 2009-03-18 18:42:22 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"


from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
from Configurables import OutputStream
from Configurables import ( LHCbConfigurableUser, LHCbApp, PhysConf, DaVinci )
from Configurables import MCParticleCloner
from Configurables import MCVertexCloner
from Configurables import VertexCloner
from Configurables import ProtoParticleCloner
from Configurables import PrintHeader
from Configurables import OutputStream
from Configurables import BTagging, BTaggingTool
from Configurables import PhysDesktop
from Configurables import PVReFitterAlg
from Configurables import CopyRecHeader
from Configurables import CopyMCParticles
from Configurables import CopyParticles
from Configurables import CopyPrimaryVertices
from Configurables import CopyParticle2PVLink
from Configurables import CopyRelatedMCParticles
from Configurables import CopyFlavourTag
import GaudiKernel.ProcessJobOptions
    
class PhysMicroDST(LHCbConfigurableUser) :

    Sequence = None

    __slots__ = {
        # Application Configuration : sent to LHCbApp and Gaudi
        "EvtMax"             :  -1           # Number of events to analyse
        , "SkipEvents"         :   0           # Number of events to skip at beginning for file
        , "PrintFreq"          : 100           # The frequency at which to print event numbers
        , "DataType"           : 'DC06'        # Data type, can be ['DC06','2008'] Forwarded to PhysConf
        , "Simulation"         : True          # set to True to use SimCond. Forwarded to PhysConf
        # Input
        , "Input"              : []            # Input data. Can also be passed as a second option file.
        # Output
        , "HistogramFile"      : ""            # Write name of output Histogram file
        , "TupleFile"          : ""            # Write name of output Tuple file
        , "ETCFile"            : ""            # Name of ETC file
        , "MicroDSTFile"       : "MicroDST.dst"
         # DaVinci Options
        , "MainOptions"        : ""            # Main option file to execute
        , "UserAlgorithms"     : []            # User algorithms to run.
        , "MainSequence"       : ""            # Name of sequence from which to take data for MicroDST
        , "MainLocation"       : ""            # Name of TES location of selected particles
        , "RedoMCLinks"        : False         # On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association.
        # Trigger running
        , "L0"                 : False         # Run L0. 
        , "ReplaceL0BanksWithEmulated" : False # Re-run L0 
        , "HltType"            : ''            # HltType : No Hlt. Use Hlt1+Hlt2 to run Hlt
        , "HltUserAlgorithms"  : [ ]           # put here user algorithms to add
        , "Hlt2Requires"       : 'L0+Hlt1'     # Say what Hlt2 requires
        }

    _propertyDocDct = {  
        "EvtMax"             : """ Number of events to analyse """
        , "SkipEvents"         : """ Number of events to skip at beginning for file """
        , "PrintFreq"          : """ The frequency at which to print event numbers """
        , "DataType"           : """ Data type, can be ['DC06','2008'] Forwarded to PhysConf """
        , "Simulation"         : """ set to True to use SimCond. Forwarded to PhysConf """
        , "Input"              : """ Input data. Can also be passed as a second option file. """
        , "HistogramFile"      : """ Write name of output Histogram file """
        , "TupleFile"          : """ Write name of output Tuple file """
        , "ETCFile"            : """ Write name of output ETC file."""
        , "MicroDSTFile"     : """ Write name of output MicroDST file. Default 'MicroDST.dst'"""
        , "MainOptions"        : """ Main option file to execute """
        , "UserAlgorithms"     : """ User algorithms to run. """
        , "MainSequence"       : """ Name of GaudiSequence from which to take data for MicroDST"""
        , "MainLocation"       : """ Name of TES location of selected particles"""
        , "MainSequence"       : """ Name of GaudiSequence from which to take data for MicroDST"""         
        , "RedoMCLinks"        : """ On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association. """
        , "InputType"          : """ 'DST' or 'DIGI' or 'ETC' or 'RDST' or 'DST'. Nothing means the input type is compatible with being a DST.  """
        , "L0"                 : """ Re-Run L0 """
        , "ReplaceL0BanksWithEmulated" : """ Re-run L0 and replace all data with emulation  """
        , "HltType"            : """ HltType : No Hlt by default. Use Hlt1+Hlt2 to run Hlt """
        , "HltUserAlgorithms"  : """ Put here user algorithms to add to Hlt """
        , "Hlt2Requires"       : """ Definition of what Hlt2 requires to run. Default is 'L0+Hlt1'.
        'L0' will require only L0, '' (empty string) will run on all events. 'Hlt1' without L0 does not make any sense.
                                    """
        }

    __used_configurables__ = [ DaVinci ]



    def davinci(self) :
        print "configuring DaVinci"
        """
        Pass on common parameters to DaVinci configurable
        """
        self.setOtherProps( DaVinci(),["EvtMax",
                                       "SkipEvents",
                                       "PrintFreq",
                                       "DataType",
                                       "Simulation",
                                       "Input",
                                       "HistogramFile",
                                       "TupleFile",
                                       "ETCFile",
                                       "MicroDSTFile",
                                       "MainOptions",
                                       "RedoMCLinks",
                                       "L0",
                                       "ReplaceL0BanksWithEmulated",
                                       "HltType",
                                       "HltUserAlgorithms",
                                       "Hlt2Requires"] )

    def seqMicroDST(self) :
        return GaudiSequencer('SeqMicroDST')

    def getSequencer(self) :
        opts = self.getProp( "MainOptions" )
        if not (opts == '') :
            importOptions("$STDOPTS/PreloadUnits.opts") # to get units in .opts files
            importOptions( self.getProp( "MainOptions" ) )
        else :
            log.info("No MainOptions specified. DaVinci() will import no options file!")
        seqName = self.getProp("MainSequence")
        log.info("Getting MainSequence to "+seqName)
        seq = GaudiSequencer(seqName)
#        seq.Members += [ self.seqMicroDST() ]
        return seq


    def userAlgs(self) :
        userAlgs = []
        for alg in self.getProp("UserAlgorithms"):
            userAlgs.append( alg )
        seq = self.getSequencer()
        seq.Members += [ self.seqMicroDST() ]
        userAlgs.append(seq)
        log.info("PhysMicroDST: Setting UserAlgorithms:")
        DaVinci().setProp("UserAlgorithms", userAlgs)
        
        
    def initStream(self) :
        log.info("Setting MicroDSTStream")
        importOptions('$MICRODSTOPTS/MicroDSTStream.py')
        stream = OutputStream('MicroDSTStream')
        dstName = self.getProp("MicroDSTFile")
        if (dstName != "" ) :
            stream.Output = "DATAFILE='"+dstName +"' TYP='POOL_ROOTTREE' OPT='REC'"
#        seqName = self.getSequencer().name()
        seqName = self.getProp("MainSequence")
        log.info("Adding sequence "+ seqName + " to MicroDSTSTream")
        stream.AcceptAlgs.append( seqName )

    def mainLocation(self) :
        return self.getProp("MainLocation")

    def copyDefaultStuff(self):
        self.seqMicroDST().Members += [CopyRecHeader()]

    def copyParticleTrees(self) :
        copyParticles = CopyParticles('CopyParticles')
        copyParticles.InputLocation = self.mainLocation()+"/Particles"
        copyParticles.addTool(VertexCloner(), name='VertexCloner')
        copyParticles.addTool(ProtoParticleCloner(),
                              name='ProtoParticleCloner')
        copyParticles.OutputLevel=4
        self.seqMicroDST().Members += [copyParticles]  

    def copyPVs(self) :
        copyPV=CopyPrimaryVertices('CopyPrimaryVertices')
        copyPV.OutputLevel = 4
        self.seqMicroDST().Members += [copyPV]

    def copyP2PVLinks(self) :
        copyP2PVLink = CopyParticle2PVLink()
        copyP2PVLink.InputLocation = self.mainLocation()+"/Particle2VertexRelations"
        copyP2PVLink.OutputLevel=4;
        self.seqMicroDST().Members += [copyP2PVLink]



    def copyMCInfo(self) :
        """
        Copy related MC particles of candidates plus daughters
        """
        copyMC = CopyRelatedMCParticles()
        copyMC.InputLocation = self.mainLocation()+'/Particles'
        copyMC.addTool(MCParticleCloner(), name= 'MCParticleCloner')
        copyMC.MCParticleCloner.addTool(MCVertexCloner(), name = 'ICloneMCVertex')
        copyMC.OutputLevel=4;
        self.seqMicroDST().Members += [copyMC]

    def copyBTaggingInfo(self) :

        importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.py')
        BTagAlgo = BTagging('BTagging')
        BTagAlgo.addTool(PhysDesktop)
        BTaggingTool("BTaggingTool").OutputLevel=4
        BTagAlgo.PhysDesktop.InputLocations=[self.mainLocation()]
        BTagLocation = self.mainLocation()+"/Tagging"
        BTagAlgo.TagOutputLocation = BTagLocation
        self.seqMicroDST().Members += [BTagAlgo]
        copyFlavTag = CopyFlavourTag()
        copyFlavTag.InputLocation = BTagLocation
        self.seqMicroDST().Members += [copyFlavTag]

    def reFitPVLocation(self) :
        return self.mainLocation()+"/RefittedVertices"
        
    def P2ReFitPVRelationsLoc(self) :
        return self.mainLocation()+"/Particle2ReFittedVertexRelations"

    
    def refitPVs(self) :
        PVReFitter = PVReFitterAlg("PVReFitterAlg")
        PVReFitter.ParticleInputLocation = self.mainLocation()+"/Particles"
        PVReFitter.VertexOutputLocation = self.reFitPVLocation()
        PVReFitter.P2VRelationsOutputLocation = self.P2ReFitPVRelationsLoc()
        PVReFitter.OutputLevel=4
        self.seqMicroDST().Members += [PVReFitter]

    def copyP2PVLink(self) :

        copyP2RefitPVLink = CopyParticle2PVLink("CopyP2RefitPVLink")
        copyP2RefitPVLink.InputLocation = self.P2ReFitPVRelationsLoc()
        copyP2RefitPVLink.OutputLevel=4
        self.seqMicroDST().Members += [copyP2RefitPVLink]

    def copyReFittedPVs(self) :
        self.refitPVs()
        copyReFittedPVs = CopyPrimaryVertices('CopyReFittedPVs')
        copyReFittedPVs.InputLocation = self.reFitPVLocation()
        self.seqMicroDST().Members += [copyReFittedPVs]
        self.copyP2PVLink()


    def __apply_configuration__(self) :
        """
        PhysMicroDST configuration
        """
        log.info("Applying PhysMicroDST configuration")
        log.info( self )
        self.davinci()
        self.userAlgs()
        self.initStream()
        self.copyDefaultStuff()
        self.copyParticleTrees()
        self.copyPVs()
        self.copyP2PVLinks()
        self.copyBTaggingInfo()
        self.copyMCInfo()
