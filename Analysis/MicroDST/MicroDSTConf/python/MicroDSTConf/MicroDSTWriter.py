"""

"""
__version__ = "$Id: MicroDSTWriter.py,v 1.9 2009-08-11 11:44:51 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from AnalysisConf.BaseDSTWriter import BaseDSTWriter

class MicroDSTWriter(BaseDSTWriter) :
    __slots__ = { "CopyParticles"        : True
                  , "CopyODIN"             : True
                  , "CopyRecHeader"        : True
                  , "CopyPVs"              : True
                  , "CopyBTags"            : True
                  , "CopyRelatedPVs"       : False
                  , "P2PVRelationsSuffix"  : ""
                  , "CopyL0DUReport"       : False
                  , "CopyHltDecReports"    : False
                  , "CopyMCTruth"          : False
                  , "OutputPrefix"         : "MicroDST"
                  }

    _propertyDocDct =  {  "CopyParticles"        : """ """
                          , "CopyPVs"              : """Copy Primary vertices and standard Particle->PV relaitons """
                          , "CopyBTags"            : """ """
                          , "P2PVRelationsSuffix"  : """ """
                          , "CopyL0DUReport"       : """ """
                          , "CopyHltDecReports"    : """ """
                          , "CopyRelatedPVs"       : """ """
                          , "CopyMCTruth"          : """ """
                          , "OutputPrefix"         : """ TES location of output. Default: /Event/MicroDST."""
                          }

    def outputStreamType(self) :
        from Configurables import OutputStream
        return OutputStream
    
    def extendStream(self, stream) :
        location = self.getProp("OutputPrefix")
        stream.OptItemList = ["/Event/"+ location +"#99"]

    def fileExtension(self) :
        return ".mdst"
    
    def _personaliseName(self, sel, name) :
        return name + "_" + sel.name()

    def algName(self, sel) :        
        return sel.algorithm().name()

    def mainLocation(self, sel) :
        return 'Phys/' + self.algName(sel)

    def setOutputPrefix(self, alg) :
        alg.OutputPrefix = self.getProp('OutputPrefix')
    
    def _copyRecHeader(self, sel):
        from Configurables import CopyRecHeader
        copyRecHeader = CopyRecHeader(self._personaliseName(sel,
                                                            "CopyRecHeader"))
        self.setOutputPrefix(copyRecHeader)
        sel.sequence().Members += [copyRecHeader]

    def _copyODIN(self, sel) :
        from Configurables import CopyODIN
        copyODIN = CopyODIN(self._personaliseName(sel,"CopyODIN"))
        self.setOutputPrefix(copyODIN)
        sel.sequence().Members += [copyODIN]

        
    def _copyParticleTrees(self, sel) :
        from Configurables import (CopyParticles,
                                   VertexCloner,
                                   ParticleCloner,
                                   ProtoParticleCloner )
        copyParticles = CopyParticles(self._personaliseName(sel,
                                                            'CopyParticles'))
        copyParticles.InputLocation = self.mainLocation(sel)+"/Particles"
        copyParticles.OutputLevel=4
        copyParticles.addTool(ParticleCloner, name="ParticleCloner")
        copyParticles.ParticleCloner.ICloneProtoParticle="NONE"
        self.setOutputPrefix(copyParticles)
        sel.sequence().Members += [copyParticles]  

    def _copyP2PVRelations(self, sel, name, location) :
        from Configurables import CopyParticle2PVRelations
        copyP2PVRel = CopyParticle2PVRelations(self._personaliseName(sel,name))
        copyP2PVRel.InputLocation = location
        copyP2PVRel.OutputLevel=4
        self.setOutputPrefix(copyP2PVRel)
        sel.sequence().Members += [copyP2PVRel]

    def _copyPVs(self, sel) :
        from Configurables import CopyPrimaryVertices
        copyPV=CopyPrimaryVertices(self._personaliseName(sel,'CopyPrimaryVertices'))
        copyPV.OutputLevel = 4
        self.setOutputPrefix(copyPV)
        sel.sequence().Members += [copyPV]
        if self.getProp('CopyParticles') :
            self._copyP2PVRelations(sel,"CopyP2PVRelations",
                                    self.mainLocation(sel)+"/Particle2VertexRelations")
        
    def _copyMCInfo(self, sel) :
        """
        Copy related MC particles of candidates plus daughters
        """
        from Configurables import P2MCRelatorAlg, CopyParticle2MCRelations
        from Configurables import MCParticleCloner, MCVertexCloner
        # first, get matches MCParticles for selected candidates.
        # This will make a relations table in mainLocation+"/P2MCPRelations"
        p2mcRelator = P2MCRelatorAlg(self._personaliseName(sel,'P2MCRel'))
        p2mcRelator.ParticleLocation = self.mainLocation(sel)+'/Particles'
        p2mcRelator.OutputLevel=4
        sel.sequence().Members += [p2mcRelator]
        # Now copy relations table + matched MCParticles to MicroDST
        copyP2MCRel = CopyParticle2MCRelations(self._personaliseName(sel,
                                                                     "CopyP2MCRel"))
        copyP2MCRel.addTool(MCParticleCloner)
        copyP2MCRel.MCParticleCloner.addTool(MCVertexCloner,
                                             name = 'ICloneMCVertex')
        copyP2MCRel.InputLocation = self.mainLocation(sel)+"/P2MCPRelations"
        self.setOutputPrefix(copyP2MCRel)
        sel.sequence().Members += [copyP2MCRel]

    def _copyBTaggingInfo(self, sel) :
        from Configurables import BTagging, BTaggingTool
        from Configurables import CopyFlavourTag
        importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.py')
        BTagAlgo = BTagging(self._personaliseName(sel,'BTagging'))
        BTagAlgo.InputLocations=[self.mainLocation(sel)]
        BTagLocation = self.mainLocation(sel)+"/Tagging"
        BTagAlgo.TagOutputLocation = BTagLocation
        sel.sequence().Members += [BTagAlgo]
        copyFlavTag = CopyFlavourTag(self._personaliseName(sel,
                                                           "CopyFlavourTag"))
        copyFlavTag.InputLocation = BTagLocation
        self.setOutputPrefix(copyFlavTag)
        sel.sequence().Members += [copyFlavTag]

    def _P2PVLocation(self, sel) :
        return self.mainLocation(sel)+"/"+self.P2PVRelationsSuffix

    def _copyRelatedPVs(self, sel) :
        if self.getProp('CopyParticles') :
            self._copyP2PVRelations(sel,"CopyUserP2PVRelations",
                                   self._P2PVLocation(sel) )

    def _copyL0DUReport(self, sel) :
        from Configurables import CopyL0DUReport
        copyL0 = CopyL0DUReport(self._personaliseName(sel,'CopyL0DUReport'))
        self.setOutputPrefix(copyL0)
        sel.sequence().Members += [copyL0]

    def _copyHltDecReports(self, sel) :
        from Configurables import CopyHltDecReports
        copyHlt = CopyHltDecReports(self._personaliseName(sel,'CopyHltDecReports'))
        self.setOutputPrefix(copyHlt)
        sel.sequence().Members += [copyHlt]

    def extendSequence(self, sel) :
        if self.getProp("CopyODIN")          : self._copyODIN(sel)
        if self.getProp("CopyRecHeader")     : self._copyRecHeader(sel)
        if self.getProp("CopyParticles")     : self._copyParticleTrees(sel)
        if self.getProp("CopyPVs")           : self._copyPVs(sel)
        if self.getProp("CopyBTags")         : self._copyBTaggingInfo(sel)
        if self.getProp("CopyL0DUReport")    : self._copyL0DUReport(sel)
        if self.getProp("CopyHltDecReports") : self._copyHltDecReports(sel)
        if self.getProp("CopyRelatedPVs")    : self._copyRelatedPVs(sel)
        if self.getProp("CopyMCTruth")       : self._copyMCInfo(sel)
