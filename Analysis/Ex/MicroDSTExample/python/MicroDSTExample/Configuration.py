"""
High level configuration example for a typical physics MicroDST
"""
__version__ = "$Id: Configuration.py,v 1.5 2009-03-24 11:03:06 jpalac Exp $"
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
        "EvtMax"                 :  -1           # Number of events to analyse
        , "SkipEvents"           :   0           # Number of events to skip at beginning for file
        , "PrintFreq"            : 100           # The frequency at which to print event numbers
        , "DataType"             : 'DC06'        # Data type, can be ['DC06','2008'] Forwarded to PhysConf
        , "Simulation"           : True          # set to True to use SimCond. Forwarded to PhysConf
        # Input
        , "Input"                : []            # Input data. Can also be passed as a second option file.
        # Output
        , "HistogramFile"        : ""            # Write name of output Histogram file
        , "TupleFile"            : ""            # Write name of output Tuple file
        , "ETCFile"              : ""            # Name of ETC file
        , "MicroDSTFile"         : "MicroDST.dst"
         # DaVinci Options
        , "UserAlgorithms"       : []            # User algorithms to run.
        , "MicroDSTSelectionAlg" : ""            # Name of selection algorithm that defines data for MicroDST
        , "RedoMCLinks"        : False         # On some stripped DST one needs to redo the Track<->MC link table. Set to true if problems with association.
        # Trigger running
        , "L0"                 : False         # Run L0. 
        , "ReplaceL0BanksWithEmulated" : False # Re-run L0 
        , "HltType"            : ''            # HltType : No Hlt. Use Hlt1+Hlt2 to run Hlt
        , "HltUserAlgorithms"  : [ ]           # put here user algorithms to add
        , "Hlt2Requires"       : 'L0+Hlt1'     # Say what Hlt2 requires
        , "CopyParticles"      : True
        , "CopyPVs"            : True
        , "CopyBTags"          : True
        , "CopyReFittedPVs"    : False
        , "CopyMCTruth"        : False
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
        , "UserAlgorithms"     : """ User algorithms to run. """
        , "MicroDSTSelectionAlg"      : """            # Name of selection algorithm that defines data for MicroDST"""
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
                                       "RedoMCLinks",
                                       "L0",
                                       "ReplaceL0BanksWithEmulated",
                                       "HltType",
                                       "HltUserAlgorithms",
                                       "Hlt2Requires"] )

    def seqMicroDST(self) :
        return GaudiSequencer('SeqMicroDST')

    def getMicroDSTSelAlg(self) :        
        seqName = self.getProp("MicroDSTSelectionAlg")
        log.info("Getting MicroDSTSelectionAlg "+ seqName)
        return seqName

    def userAlgs(self) :
        userAlgs = []
        for alg in self.getProp("UserAlgorithms"):
            userAlgs.append( alg )
        mainAlgoName = self.getMicroDSTSelAlg()
        print "Adding ", mainAlgoName, " to MicroDST sequence"
        self.seqMicroDST().Members += [mainAlgoName]
        userAlgs.append(self.seqMicroDST())
        log.info("PhysMicroDST: Setting UserAlgorithms:")
        DaVinci().setProp("UserAlgorithms", userAlgs)
        
        
    def initMicroDSTStream(self) :
        log.info("Setting MicroDSTStream")
        importOptions('$MICRODSTOPTS/MicroDSTStream.py')
        stream = OutputStream('MicroDSTStream')
        dstName = self.getProp("MicroDSTFile")
        if (dstName != "" ) :
            stream.Output = "DATAFILE='"+dstName +"' TYP='POOL_ROOTTREE' OPT='REC'"
        return stream

    def mainLocation(self) :
        algoName = self.getProp("MicroDSTSelectionAlg")
        return 'Phys/' + algoName
    
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

    def copyP2PVLink(self, name, location) :

        copyP2RefitPVLink = CopyParticle2PVLink(name)
        copyP2RefitPVLink.InputLocation = location
        copyP2RefitPVLink.OutputLevel=4
        self.seqMicroDST().Members += [copyP2RefitPVLink]


    def copyPVs(self) :
        copyPV=CopyPrimaryVertices('CopyPrimaryVertices')
        copyPV.OutputLevel = 4
        self.seqMicroDST().Members += [copyPV]
        if self.getProp("CopyParticles") :
            self.copyP2PVLink("CopyP2PVLink",
                              self.mainLocation()+"/Particle2VertexRelations")
        
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
        

    def copyReFittedPVs(self) :
        self.refitPVs()
        copyReFittedPVs = CopyPrimaryVertices('CopyReFittedPVs')
        copyReFittedPVs.InputLocation = self.reFitPVLocation()
        self.seqMicroDST().Members += [copyReFittedPVs]
        if self.getProp("CopyParticles") :
            self.copyP2PVLink("CopyP2RefitPVLink",
                              self.P2ReFitPVRelationsLoc() )

    def __apply_configuration__(self) :
        """
        PhysMicroDST configuration
        """
        log.info("Applying PhysMicroDST configuration")
        log.info( self )
        self.davinci()
        self.userAlgs()
        self.copyDefaultStuff()
        if self.getProp("CopyParticles") : self.copyParticleTrees()
        if self.getProp("CopyPVs") : self.copyPVs()
        if self.getProp("CopyBTags") : self.copyBTaggingInfo()
        if self.getProp("CopyReFittedPVs") : self.copyReFittedPVs()
        if self.getProp("CopyMCTruth") : self.copyMCInfo()
        self.seqMicroDST().Members += [self.initMicroDSTStream()]
