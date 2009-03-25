"""
High level configuration example for a typical physics MicroDST
"""
__version__ = "$Id: Configuration.py,v 1.8 2009-03-25 11:07:57 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"


from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from Configurables import LHCbConfigurableUser

class PhysMicroDST(LHCbConfigurableUser) :

    __slots__ = {
        "MicroDSTFile"           : "MicroDST.dst"
        , "MicroDSTSelectionAlg" : ""
        , "OutputPrefix"         : "microDST"
        , "CopyParticles"        : True
        , "CopyPVs"              : True
        , "CopyBTags"            : True
        , "CopyReFittedPVs"      : False
        , "CopyMCTruth"          : False
        }

    _propertyDocDct = {  
        "MicroDSTFile"           : """ Write name of output MicroDST file. Default 'MicroDST.dst'"""
        , "MicroDSTSelectionAlg" : """            # Name of selection algorithm that defines data for MicroDST"""
        , "OutputPrefix"         : """ """
        , "CopyParticles"        : """ """
        , "CopyPVs"              : """ """
        , "CopyBTags"            : """ """
        , "CopyReFittedPVs"      : """ """
        , "CopyMCTruth"          : """ """
        }

    def seqMicroDST(self) :
        from Configurables import GaudiSequencer
        return GaudiSequencer('SeqMicroDST')

    def getMicroDSTSelAlg(self) :        
        seqName = self.getProp("MicroDSTSelectionAlg")
        log.info("Getting MicroDSTSelectionAlg "+ seqName)
        return seqName

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
        algoName = self.getProp("MicroDSTSelectionAlg")
        return 'Phys/' + algoName

    def outputPrefix(self) :
        return self.getProp("OutputPrefix")

    def setOutputPrefix(self, alg) :
        alg.OutputPrefix = self.outputPrefix()
    
    def copyDefaultStuff(self):
        from Configurables import CopyRecHeader
        copyRecHeader = CopyRecHeader()
        self.setOutputPrefix(copyRecHeader)
        self.seqMicroDST().Members += [copyRecHeader]

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

    def copyP2PVLink(self, name, location) :
        from Configurables import CopyParticle2PVLink
        copyP2RefitPVLink = CopyParticle2PVLink(name)
        copyP2RefitPVLink.InputLocation = location
        copyP2RefitPVLink.OutputLevel=4
        self.setOutputPrefix(copyP2RefitPVLink)
        self.seqMicroDST().Members += [copyP2RefitPVLink]


    def copyPVs(self) :
        from Configurables import CopyPrimaryVertices
        copyPV=CopyPrimaryVertices('CopyPrimaryVertices')
        copyPV.OutputLevel = 4
        self.setOutputPrefix(copyPV)
        self.seqMicroDST().Members += [copyPV]
        if self.getProp("CopyParticles") :
            self.copyP2PVLink("CopyP2PVLink",
                              self.mainLocation()+"/Particle2VertexRelations")
        
    def copyMCInfo(self) :
        """
        Copy related MC particles of candidates plus daughters
        """
        from Configurables import CopyRelatedMCParticles
        from Configurables import MCParticleCloner, MCVertexCloner
        copyMC = CopyRelatedMCParticles()
        copyMC.InputLocation = self.mainLocation()+'/Particles'
        copyMC.addTool(MCParticleCloner(), name= 'MCParticleCloner')
        copyMC.MCParticleCloner.addTool(MCVertexCloner(), name = 'ICloneMCVertex')
        copyMC.OutputLevel=4;
        self.setOutputPrefix(copyMC)
        self.seqMicroDST().Members += [copyMC]

    def copyBTaggingInfo(self) :
        from Configurables import BTagging, BTaggingTool
        from Configurables import CopyFlavourTag
        from Configurables import PhysDesktop
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
        self.setOutputPrefix(copyFlavTag)
        self.seqMicroDST().Members += [copyFlavTag]

    def reFitPVLocation(self) :
        return self.mainLocation()+"/RefittedVertices"
        
    def P2ReFitPVRelationsLoc(self) :
        return self.mainLocation()+"/Particle2ReFittedVertexRelations"

    
    def refitPVs(self) :
        from Configurables import PVReFitterAlg
        PVReFitter = PVReFitterAlg("PVReFitterAlg")
        PVReFitter.ParticleInputLocation = self.mainLocation()+"/Particles"
        PVReFitter.VertexOutputLocation = self.reFitPVLocation()
        PVReFitter.P2VRelationsOutputLocation = self.P2ReFitPVRelationsLoc()
        PVReFitter.OutputLevel=4
        self.seqMicroDST().Members += [PVReFitter]
        

    def copyReFittedPVs(self) :
        from Configurables import CopyPrimaryVertices
        self.refitPVs()
        copyReFittedPVs = CopyPrimaryVertices('CopyReFittedPVs')
        copyReFittedPVs.InputLocation = self.reFitPVLocation()
        self.setOutputPrefix(copyReFittedPVs)
        self.seqMicroDST().Members += [copyReFittedPVs]
        if self.getProp("CopyParticles") :
            self.copyP2PVLink("CopyP2RefitPVLink",
                              self.P2ReFitPVRelationsLoc() )

    def __apply_configuration__(self) :
        """
        PhysMicroDST configuration
        """
        from Configurables import LoKi__VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory
        from Configurables import FilterDesktop, PhysDesktop
        LoKi__Hybrid__CoreFactory('CoreFactory:').Modules += ['LoKiHlt.algorithms','LoKiHlt.decorators']
        log.info("Applying PhysMicroDST configuration")
        log.info( self )
        mdstSeq = self.seqMicroDST()
#        mdstSeq.IgnoreFilterPassed = True
        ApplicationMgr().OutStream += [mdstSeq]
        selAlg = self.getMicroDSTSelAlg()
        if type(selAlg) == str :
            mdstSeq.Members = [LoKi__VoidFilter("MicroDSTSel",
                                                Code = "ALG_PASSED('"+selAlg+"')")]
        else :
            mdstSeq.Members = [selAgl]
        filterHack = FilterDesktop('FilterHack')
        filterHack.Code = "ALL"
        filterHack.addTool(PhysDesktop)
        filterHack.PhysDesktop.InputLocations = [self.mainLocation()]
        mdstSeq.Members += [filterHack]
        self.copyDefaultStuff()
        if self.getProp("CopyParticles") : self.copyParticleTrees()
        if self.getProp("CopyPVs") : self.copyPVs()
        if self.getProp("CopyBTags") : self.copyBTaggingInfo()
        if self.getProp("CopyReFittedPVs") : self.copyReFittedPVs()
        if self.getProp("CopyMCTruth") : self.copyMCInfo()
        mdstSeq.Members += [self.initMicroDSTStream()]
