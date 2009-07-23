"""
High level configuration example for a typical physics MicroDST
"""
__version__ = "$Id: PhysMicroDST.py,v 1.1 2009-07-23 13:25:54 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"


from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from Configurables import LHCbConfigurableUser

class PhysMicroDST(LHCbConfigurableUser) :

    __slots__ = {
        "MicroDSTFile"           : "MicroDST.mdst"
        , "MicroDSTSelectionAlg" : ""
        , "OutputPrefix"         : "microDST"
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
        , "MicroDSTSelectionAlg" : """            # Name of selection algorithm that defines data for MicroDST"""
        , "OutputPrefix"         : """ """
        , "CopyParticles"        : """ """
        , "CopyPVs"              : """Copy Primary vertices and standard Particle->PV relaitons """
        , "CopyBTags"            : """ """
        , "CopyRelatedPVs"      : """ """
        , "CopyMCTruth"          : """ """
        }

    def seqMicroDST(self) :
        from Configurables import GaudiSequencer
        return GaudiSequencer('SeqMicroDST')

    def getMicroDSTSelAlg(self) :        
        seqName = self.getProp("MicroDSTSelectionAlg")
        if type(seqName) == str :
            log.info("Getting MicroDSTSelectionAlg "+ seqName)
        else :
            log.info("Getting MicroDSTSelectionAlg "+ seqName.name())
        return seqName

    def getMicroDSTSelAlgName(self) :        
        seq = self.getMicroDSTSelAlg()
        if type(seq) == str :
            return seq
        else :
            return seq.name()

    def initMicroDSTStream(self) :
        from Configurables import OutputStream
        log.info("Setting MicroDSTStream")
        importOptions('$MICRODSTOPTS/MicroDSTStream.py')
        stream = OutputStream('MicroDSTStream')
        stream.OptItemList = ["/Event/"+self.outputPrefix()+"#99"]
        dstName = self.getProp("MicroDSTFile")
        if (dstName != "" ) :
            stream.Output = "DATAFILE='"+dstName +"' TYP='POOL_ROOTTREE' OPT='REC'"
        return stream

    def mainLocation(self) :
        algoName = self.getMicroDSTSelAlgName()
        return 'Phys/' + algoName

    def outputPrefix(self) :
        return self.getProp("OutputPrefix")

    def setOutputPrefix(self, alg) :
        alg.OutputPrefix = self.outputPrefix()
    
    def copyDefaultStuff(self):
        from Configurables import CopyRecHeader, CopyODIN
        copyRecHeader = CopyRecHeader()
        self.setOutputPrefix(copyRecHeader)
        self.seqMicroDST().Members += [copyRecHeader]
        copyODIN = CopyODIN()
        self.setOutputPrefix(copyODIN)
        self.seqMicroDST().Members += [copyODIN]

        
    def copyParticleTrees(self) :
        from Configurables import CopyParticles, VertexCloner, ProtoParticleCloner
        copyParticles = CopyParticles('CopyParticles')
        copyParticles.InputLocation = self.mainLocation()+"/Particles"
        copyParticles.addTool(VertexCloner(), name='VertexCloner')
        copyParticles.addTool(ProtoParticleCloner(),
                              name='ProtoParticleCloner')
        copyParticles.OutputLevel=4
        self.setOutputPrefix(copyParticles)
        self.seqMicroDST().Members += [copyParticles]  

    def copyP2PVRelations(self, name, location) :
        from Configurables import CopyParticle2PVRelations
        copyP2PVRel = CopyParticle2PVRelations(name)
        copyP2PVRel.InputLocation = location
        copyP2PVRel.OutputLevel=4
        self.setOutputPrefix(copyP2PVRel)
        self.seqMicroDST().Members += [copyP2PVRel]


    def copyPVs(self) :
        from Configurables import CopyPrimaryVertices
        copyPV=CopyPrimaryVertices('CopyPrimaryVertices')
        copyPV.OutputLevel = 4
        self.setOutputPrefix(copyPV)
        self.seqMicroDST().Members += [copyPV]
        if self.getProp("CopyParticles") :
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
        p2mcRelator = P2MCRelatorAlg()
        p2mcRelator.ParticleLocation = self.mainLocation()+'/Particles'
        p2mcRelator.OutputLevel=4
        self.seqMicroDST().Members += [p2mcRelator]
        # Now copy relations table + matched MCParticles to MicroDST
        copyP2MCRel = CopyParticle2MCRelations()
        copyP2MCRel.addTool(MCParticleCloner)
        copyP2MCRel.MCParticleCloner.addTool(MCVertexCloner,
                                             name = 'ICloneMCVertex')
        copyP2MCRel.InputLocation = self.mainLocation()+"/P2MCPRelations"
        self.setOutputPrefix(copyP2MCRel)
        self.seqMicroDST().Members += [copyP2MCRel]

    def copyBTaggingInfo(self) :
        from Configurables import BTagging, BTaggingTool
        from Configurables import CopyFlavourTag
        importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.py')
        BTagAlgo = BTagging('BTagging')
        BTaggingTool("BTaggingTool").OutputLevel=4
        BTagAlgo.InputLocations=[self.mainLocation()]
        BTagLocation = self.mainLocation()+"/Tagging"
        BTagAlgo.TagOutputLocation = BTagLocation
        self.seqMicroDST().Members += [BTagAlgo]
        copyFlavTag = CopyFlavourTag()
        copyFlavTag.InputLocation = BTagLocation
        self.setOutputPrefix(copyFlavTag)
        self.seqMicroDST().Members += [copyFlavTag]

    def P2PVLocation(self) :
        return self.mainLocation()+"/"+self.getProp("P2PVRelationsSuffix")

    def copyRelatedPVs(self) :
        if self.getProp("CopyParticles") :
            self.copyP2PVRelations("CopyUserP2PVRelations",
                                   self.P2PVLocation() )

    def copyL0DUReport(self) :
        from Configurables import CopyL0DUReport
        copyL0 = CopyL0DUReport()
        self.setOutputPrefix(copyL0)
        self.seqMicroDST().Members += [copyL0]

    def copyHltDecReports(self) :
        from Configurables import CopyHltDecReports
        copyHlt = CopyHltDecReports()
        self.setOutputPrefix(copyHlt)
        self.seqMicroDST().Members += [copyHlt]


    def __apply_configuration__(self) :
        """
        PhysMicroDST configuration
        """
        from Configurables import LoKi__VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory

        LoKi__Hybrid__CoreFactory('CoreFactory').Modules += ['LoKiHlt.algorithms','LoKiHlt.decorators']
        log.info("Applying PhysMicroDST configuration")
        log.info( self )
        mdstSeq = self.seqMicroDST()
        ApplicationMgr().OutStream += [mdstSeq]
        selAlg = self.getMicroDSTSelAlgName()
        if type(selAlg) == str :
            mdstSeq.Members = [LoKi__VoidFilter("MicroDSTSel",
                                                Code = "ALG_PASSED('"+selAlg+"')&ALG_EXECUTED('"+selAlg+"')")]
        else :
            mdstSeq.Members = [selAlg]
        self.copyDefaultStuff()
        if self.getProp("CopyParticles") : self.copyParticleTrees()
        if self.getProp("CopyPVs") : self.copyPVs()
        if self.getProp("CopyBTags") : self.copyBTaggingInfo()
        if self.getProp("CopyL0DUReport") : self.copyL0DUReport()
        if self.getProp("CopyHltDecReports") : self.copyHltDecReports()
        if self.getProp("CopyRelatedPVs") : self.copyRelatedPVs()
        if self.getProp("CopyMCTruth") : self.copyMCInfo()
        mdstSeq.Members += [self.initMicroDSTStream()]
