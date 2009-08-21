"""

"""
__version__ = "$Id: MicroDSTWriter.py,v 1.13 2009-08-21 15:55:38 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

import GaudiPython
GaudiPython.loaddict('libEventAssocDict')

from AnalysisConf.BaseDSTWriter import BaseDSTWriter

class MicroDSTWriter(BaseDSTWriter) :
    __slots__ = { "CopyParticles"        : True
                  , "CopyODIN"             : True
                  , "CopyRecHeader"        : True
                  , "CopyPVs"              : True
                  , "CopyProtoParticles"   : True
                  , "CopyBTags"            : True
                  , "CopyPVRelations"      : {"Particle2VertexRelations":True}
                  , "CopyL0DUReport"       : False
                  , "CopyHltDecReports"    : False
                  , "CopyMCTruth"          : False
                  , "OutputPrefix"         : "MicroDST"
                  }

    _propertyDocDct =  {  "CopyParticles"        : """ """
                          , "CopyPVs"              : """Copy Primary vertices and standard Particle->PV relaitons """
                          , "CopyProtoParticles"   : """Copy the ProtoParticles stored Particles were made of. Default: True"""
                          , "CopyBTags"            : """ """
                          , "P2PVSuffixes"         : """ """
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

    def mainLocation(self, sel) :
        return 'Phys/' + sel.algName()

    def setOutputPrefix(self, alg) :
        alg.OutputPrefix = self.getProp('OutputPrefix')
    
    def _copyRecHeader(self, sel):
        from Configurables import CopyRecHeader
        cloner = CopyRecHeader(self._personaliseName(sel,
                                                     "CopyRecHeader"))
        self.setOutputPrefix(cloner)
        return [cloner]

    def _copyODIN(self, sel) :
        from Configurables import CopyODIN
        cloner = CopyODIN(self._personaliseName(sel,"CopyODIN"))
        self.setOutputPrefix(cloner)
        return [cloner]
        
    def _copyParticleTrees(self, sel) :
        from Configurables import (CopyParticles,
                                   VertexCloner,
                                   ParticleCloner,
                                   ProtoParticleCloner )
        cloner = CopyParticles(self._personaliseName(sel,
                                                     'CopyParticles'))
        cloner.InputLocation = self.mainLocation(sel)+"/Particles"
        cloner.OutputLevel=4
        if self.getProp("CopyProtoParticles") == False :
            cloner.addTool(ParticleCloner, name="ParticleCloner")
            cloner.ParticleCloner.ICloneProtoParticle="NONE"
        self.setOutputPrefix(cloner)
        return [cloner]

    def _copyP2PVRelations(self, sel, name, location) :
        from Configurables import CopyParticle2PVRelations
        cloner = CopyParticle2PVRelations(self._personaliseName(sel,name))
        cloner.InputLocation = location
        cloner.OutputLevel=4
        self.setOutputPrefix(cloner)
        return cloner

    def _copyPVs(self, sel) :
        from Configurables import CopyPrimaryVertices
        cloner=CopyPrimaryVertices(self._personaliseName(sel,
                                                         'CopyPrimaryVertices'))
        cloner.OutputLevel = 4
        self.setOutputPrefix(cloner)
        return [cloner]
    
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
        # Now copy relations table + matched MCParticles to MicroDST
        cloner = CopyParticle2MCRelations(self._personaliseName(sel,
                                                                "CopyP2MCRel"))
        cloner.addTool(MCParticleCloner)
        cloner.MCParticleCloner.addTool(MCVertexCloner,
                                             name = 'ICloneMCVertex')
        cloner.InputLocation = self.mainLocation(sel)+"/P2MCPRelations"
        self.setOutputPrefix(cloner)
        return [p2mcRelator, cloner]

    def _copyBTaggingInfo(self, sel) :
        from Configurables import BTagging, BTaggingTool
        from Configurables import CopyFlavourTag
        importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.py')
        BTagAlgo = BTagging(self._personaliseName(sel,'BTagging'))
        BTagAlgo.InputLocations=[self.mainLocation(sel)]
        BTagLocation = self.mainLocation(sel)+"/Tagging"
        BTagAlgo.TagOutputLocation = BTagLocation
        sel.sequence().Members += [BTagAlgo]
        cloner = CopyFlavourTag(self._personaliseName(sel,
                                                      "CopyFlavourTag"))
        cloner.InputLocation = BTagLocation
        self.setOutputPrefix(cloner)
        return [cloner]

    def _copyPVRelations(self, sel) :
        # loop over related PV locations and copy each table.
        # If no PV copying, keep original PV
        cloners = []
        if self.getProp('CopyParticles') :
            for loc, copyPV in self.getProp("CopyPVRelations").iteritems() :
                print "Copy PV relations ", loc
                fullLoc = self.mainLocation(sel) + "/" + loc
                cloner = self._copyP2PVRelations(sel,"CopyP2PV_"+loc, fullLoc )
                if copyPV == False :
                    cloner.ClonerType = "NONE"
                cloners += [cloner]
        return cloners
    
    def _copyL0DUReport(self, sel) :
        from Configurables import CopyL0DUReport
        cloner = CopyL0DUReport(self._personaliseName(sel,'CopyL0DUReport'))
        self.setOutputPrefix(cloner)
        return [cloner]

    def _copyHltDecReports(self, sel) :
        from Configurables import CopyHltDecReports
        cloner = CopyHltDecReports(self._personaliseName(sel,'CopyHltDecReports'))
        self.setOutputPrefix(cloner)
        return [cloner]

    def extendSequence(self, sel) :
        print self.name(), ": Extending sequence ", sel.sequence().Members
        clonerList = []
        if self.getProp("CopyODIN")          :
            clonerList+=self._copyODIN(sel)
        if self.getProp("CopyRecHeader")     :
            clonerList+=self._copyRecHeader(sel)
        if self.getProp("CopyParticles")     :
            clonerList+=self._copyParticleTrees(sel)
        if self.getProp("CopyPVs")           :
            clonerList+=self._copyPVs(sel)
        if self.getProp("CopyBTags")         :
            clonerList+=self._copyBTaggingInfo(sel)
        if self.getProp("CopyL0DUReport")    :
            clonerList+=self._copyL0DUReport(sel)
        if self.getProp("CopyHltDecReports") :
            clonerList+=self._copyHltDecReports(sel)
        if self.getProp("CopyMCTruth")       :
            clonerList+=self._copyMCInfo(sel)
        if len(self.getProp("CopyPVRelations")) > 0 :
            clonerList+=self._copyPVRelations(sel)
        sel.sequence().Members += clonerList

        print self.name(), ": Extended sequence is now", sel.sequence().Members
