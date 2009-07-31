"""
High level configuration example for a typical physics MicroDST
"""
__version__ = "$Id: PhysMicroDST.py,v 1.2 2009-07-31 11:35:33 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"


from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from Configurables import LHCbConfigurableUser

class PhysMicroDST(LHCbConfigurableUser) :

    __slots__ = {
        "MicroDSTFile"           : "MicroDST.mdst"
        , "MicroDSTSelectionAlg" : ""
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
        , "CopyParticles"        : """ """
        , "CopyPVs"              : """Copy Primary vertices and standard Particle->PV relaitons """
        , "CopyBTags"            : """ """
        , "CopyRelatedPVs"      : """ """
        , "CopyMCTruth"          : """ """
        }

    def personaliseName(self, name) :
        return name + "_" + self.name()

    def seqMicroDST(self) :
        from Configurables import GaudiSequencer
        return GaudiSequencer('SeqMicroDST_'+ self.name() )

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
        streamName = 'MicroDSTStream'+ self.name()
        log.info("Setting OutpurStream "+ streamName)
        #importOptions('$MICRODSTOPTS/MicroDSTStream.py')
        stream = OutputStream(streamName)
        log.info(self.name()+ ": Will save everything under "+ "/Event/"+self.name())
        stream.OptItemList = ["/Event/"+self.name()+"#99"]
        dstName = self.getProp("MicroDSTFile")
        if (dstName != "" ) :
            stream.Output = "DATAFILE='"+dstName +"' TYP='POOL_ROOTTREE' OPT='REC'"
        return stream

    def mainLocation(self) :
        algoName = self.getMicroDSTSelAlgName()
        return 'Phys/' + algoName

    def setOutputPrefix(self, alg) :
        alg.OutputPrefix = self.name()
    
    def copyDefaultStuff(self):
        from Configurables import CopyRecHeader, CopyODIN
        copyRecHeader = CopyRecHeader(self.personaliseName("CopyRecHeader"))
        self.setOutputPrefix(copyRecHeader)
        self.seqMicroDST().Members += [copyRecHeader]
        copyODIN = CopyODIN(self.personaliseName("CopyODIN"))
        self.setOutputPrefix(copyODIN)
        self.seqMicroDST().Members += [copyODIN]

        
    def copyParticleTrees(self) :
        from Configurables import CopyParticles, VertexCloner, ProtoParticleCloner
        copyParticles = CopyParticles(self.personaliseName('CopyParticles'))
        copyParticles.InputLocation = self.mainLocation()+"/Particles"
        copyParticles.addTool(VertexCloner(), name='VertexCloner')
        copyParticles.addTool(ProtoParticleCloner(),
                              name='ProtoParticleCloner')
        copyParticles.OutputLevel=4
        self.setOutputPrefix(copyParticles)
        self.seqMicroDST().Members += [copyParticles]  

    def copyP2PVRelations(self, name, location) :
        from Configurables import CopyParticle2PVRelations
        copyP2PVRel = CopyParticle2PVRelations(self.personaliseName(name))
        copyP2PVRel.InputLocation = location
        copyP2PVRel.OutputLevel=4
        self.setOutputPrefix(copyP2PVRel)
        self.seqMicroDST().Members += [copyP2PVRel]


    def copyPVs(self) :
        from Configurables import CopyPrimaryVertices
        copyPV=CopyPrimaryVertices(self.personaliseName('CopyPrimaryVertices'))
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
        p2mcRelator = P2MCRelatorAlg(self.personaliseName('P2MCRel'))
        p2mcRelator.ParticleLocation = self.mainLocation()+'/Particles'
        p2mcRelator.OutputLevel=4
        self.seqMicroDST().Members += [p2mcRelator]
        # Now copy relations table + matched MCParticles to MicroDST
        copyP2MCRel = CopyParticle2MCRelations(self.personaliseName("CopyP2MCRel"))
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
        BTagAlgo = BTagging(self.personaliseName('BTagging'))
        BTagAlgo.InputLocations=[self.mainLocation()]
        BTagLocation = self.mainLocation()+"/Tagging"
        BTagAlgo.TagOutputLocation = BTagLocation
        self.seqMicroDST().Members += [BTagAlgo]
        copyFlavTag = CopyFlavourTag(self.personaliseName("CopyFlavourTag"))
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
        copyL0 = CopyL0DUReport(self.personaliseName('CopyL0DUReport'))
        self.setOutputPrefix(copyL0)
        self.seqMicroDST().Members += [copyL0]

    def copyHltDecReports(self) :
        from Configurables import CopyHltDecReports
        copyHlt = CopyHltDecReports(self.personaliseName('CopyHltDecReports'))
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
            mdstSeq.Members = [LoKi__VoidFilter("MicroDSTSel_"+self.name(),
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
