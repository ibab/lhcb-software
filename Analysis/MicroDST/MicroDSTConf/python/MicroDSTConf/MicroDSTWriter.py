"""

"""
__version__ = "$Id: MicroDSTWriter.py,v 1.1 2009-08-09 16:28:23 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from GaudiConf.Configuration import *

class MicroDSTWriter(LHCbConfigurableUser) :

    __slots__ = {
        "MicroDSTFile"           : "MicroDST.mdst"
        , "SelectionSequence"    : ""
        , "CopyParticles"        : True
        , "CopyPVs"              : True
        , "CopyBTags"            : True
        , "CopyRelatedPVs"       : False
        , "P2PVRelationsSuffix"  : ""
        , "CopyL0DUReport"       : False
        , "CopyHltDecReports"    : False
        , "CopyMCTruth"          : False
        }

    _propertyDocDct = {  
        "MicroDSTFile"           : """ Write name of output MicroDST file. Default 'MicroDST.dst'"""
        , "SelectionSequence"    : """            # Name of selection sequence that defines data for MicroDST"""
        , "CopyParticles"        : """ """
        , "CopyPVs"              : """Copy Primary vertices and standard Particle->PV relaitons """
        , "CopyBTags"            : """ """
        , "CopyRelatedPVs"      : """ """
        , "CopyMCTruth"          : """ """
        }

    def selectionSeq(self) :
        return self.getProp('SelectionSequence')
    
    def _initMicroDSTStream(self) :
        from Configurables import OutputStream
        name = self.name()
        outputFileName = self.getProp('MicroDSTFile')
        streamName = 'MicroDSTStream'+ name
        stream = OutputStream(streamName)
        stream.OptItemList = ["/Event/"+ name +"#99"]
        dstName = outputFileName
        if (dstName != "" ) :
            stream.Output = "DATAFILE='"+dstName +"' TYP='POOL_ROOTTREE' OPT='REC'"
        
    def outputStream(self) :
        return OutputStream('MicroDSTStream'+ self.name())

    def _personaliseName(self, name) :
        return name + "_" + self.name()

    def sequence(self) :
        return self.selectionSeq().sequence()

    def algorithm(self) :
        return self.selectionSeq().algorithm()

    def algName(self) :        
        return self.algorithm().name()

    def mainLocation(self) :
        return 'Phys/' + self.algName()

    def setOutputPrefix(self, alg) :
        alg.OutputPrefix = self.name()
    
    def copyDefaultStuff(self):
        from Configurables import CopyRecHeader, CopyODIN
        copyRecHeader = CopyRecHeader(self._personaliseName("CopyRecHeader"))
        self.setOutputPrefix(copyRecHeader)
        self.sequence().Members += [copyRecHeader]
        copyODIN = CopyODIN(self._personaliseName("CopyODIN"))
        self.setOutputPrefix(copyODIN)
        self.sequence().Members += [copyODIN]

        
    def copyParticleTrees(self) :
        from Configurables import CopyParticles, VertexCloner, ProtoParticleCloner
        copyParticles = CopyParticles(self._personaliseName('CopyParticles'))
        copyParticles.InputLocation = self.mainLocation()+"/Particles"
        copyParticles.addTool(VertexCloner(), name='VertexCloner')
        copyParticles.addTool(ProtoParticleCloner(),
                              name='ProtoParticleCloner')
        copyParticles.OutputLevel=4
        self.setOutputPrefix(copyParticles)
        self.sequence().Members += [copyParticles]  

    def copyP2PVRelations(self, name, location) :
        from Configurables import CopyParticle2PVRelations
        copyP2PVRel = CopyParticle2PVRelations(self._personaliseName(name))
        copyP2PVRel.InputLocation = location
        copyP2PVRel.OutputLevel=4
        self.setOutputPrefix(copyP2PVRel)
        self.sequence().Members += [copyP2PVRel]


    def copyPVs(self) :
        from Configurables import CopyPrimaryVertices
        copyPV=CopyPrimaryVertices(self._personaliseName('CopyPrimaryVertices'))
        copyPV.OutputLevel = 4
        self.setOutputPrefix(copyPV)
        self.sequence().Members += [copyPV]
        if self.getProp('CopyParticles') :
            self.copyP2PVRelations("CopyP2PVRelations",
                                   self.mainLocation()+"/Particle2VertexRelations")
        
    def copyMCInfo(self) :
        """
        Copy related MC particles of candidates plus daughters
        """
        from Configurables import P2MCRelatorAlg, CopyParticle2MCRelations
        from Configurables import MCParticleCloner, MCVertexCloner
        # first, get matches MCParticles for selected candidates.
        # This will make a relations table in mainLocation+"/P2MCPRelations"
        p2mcRelator = P2MCRelatorAlg(self._personaliseName('P2MCRel'))
        p2mcRelator.ParticleLocation = self.mainLocation()+'/Particles'
        p2mcRelator.OutputLevel=4
        self.sequence().Members += [p2mcRelator]
        # Now copy relations table + matched MCParticles to MicroDST
        copyP2MCRel = CopyParticle2MCRelations(self._personaliseName("CopyP2MCRel"))
        copyP2MCRel.addTool(MCParticleCloner)
        copyP2MCRel.MCParticleCloner.addTool(MCVertexCloner,
                                             name = 'ICloneMCVertex')
        copyP2MCRel.InputLocation = self.mainLocation()+"/P2MCPRelations"
        self.setOutputPrefix(copyP2MCRel)
        self.sequence().Members += [copyP2MCRel]

    def copyBTaggingInfo(self) :
        from Configurables import BTagging, BTaggingTool
        from Configurables import CopyFlavourTag
        importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.py')
        BTagAlgo = BTagging(self._personaliseName('BTagging'))
        BTagAlgo.InputLocations=[self.mainLocation()]
        BTagLocation = self.mainLocation()+"/Tagging"
        BTagAlgo.TagOutputLocation = BTagLocation
        self.sequence().Members += [BTagAlgo]
        copyFlavTag = CopyFlavourTag(self._personaliseName("CopyFlavourTag"))
        copyFlavTag.InputLocation = BTagLocation
        self.setOutputPrefix(copyFlavTag)
        self.sequence().Members += [copyFlavTag]

    def P2PVLocation(self) :
        return self.mainLocation()+"/"+self.P2PVRelationsSuffix

    def copyRelatedPVs(self) :
        if self.getProp('CopyParticles') :
            self.copyP2PVRelations("CopyUserP2PVRelations",
                                   self.P2PVLocation() )

    def copyL0DUReport(self) :
        from Configurables import CopyL0DUReport
        copyL0 = CopyL0DUReport(self._personaliseName('CopyL0DUReport'))
        self.setOutputPrefix(copyL0)
        self.sequence().Members += [copyL0]

    def copyHltDecReports(self) :
        from Configurables import CopyHltDecReports
        copyHlt = CopyHltDecReports(self._personaliseName('CopyHltDecReports'))
        self.setOutputPrefix(copyHlt)
        self.sequence().Members += [copyHlt]


    def __apply_configuration__(self) :
        """
        PhysMicroDST configuration
        """
        self._initMicroDSTStream()
        self.copyDefaultStuff()
        if self.getProp("CopyParticles")     : self.copyParticleTrees()
        if self.getProp("CopyPVs")           : self.copyPVs()
        if self.getProp("CopyBTags")         : self.copyBTaggingInfo()
        if self.getProp("CopyL0DUReport")    : self.copyL0DUReport()
        if self.getProp("CopyHltDecReports") : self.copyHltDecReports()
        if self.getProp("CopyRelatedPVs")    : self.copyRelatedPVs()
        if self.getProp("CopyMCTruth")       : self.copyMCInfo()
        self.sequence().Members += [self.outputStream()]
