'''Collection of functions and callable classes to perform standard MicroDST cloning.

'''

from copy import copy
from dstwriterutils import (setCloneFilteredParticlesToTrue,
                            ConfigurableList,
                            personaliseName,
                            MicroDSTElement)



class CloneRecHeader(MicroDSTElement) :
    def __call__(self, sel):
        from Configurables import CopyRecHeader
        cloner = CopyRecHeader(self.personaliseName(sel, "CopyRecHeader"))
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneODIN(MicroDSTElement) :
    def __call__(self, sel) :
        from Configurables import CopyODIN
        cloner = CopyODIN(self.personaliseName(sel,"CopyODIN"))
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneParticleTrees(MicroDSTElement) :

    def __init__(self, branch='', copyProtoParticles = True) :
        MicroDSTElement.__init__(self, branch)
        self.copyPP = copyProtoParticles

    def __call__(self, sel) :
        from Configurables import (CopyParticles,
                                   VertexCloner,
                                   ParticleCloner,
                                   ProtoParticleCloner )
        cloner = CopyParticles(self.personaliseName(sel,
                                                    'CopyParticles'))
        cloner.InputLocations = self.dataLocations(sel,"Particles")
        cloner.OutputLevel=4
        if self.copyPP == False :
            cloner.addTool(ParticleCloner, name="ParticleCloner")
            cloner.ParticleCloner.ICloneProtoParticle="NONE"
        self.setOutputPrefix(cloner)

        confList = ConfigurableList(sel)
        setCloneFilteredParticlesToTrue( confList.flatList() )

        return [cloner]

class ClonePVs(MicroDSTElement) :
    def __call__(self, sel) :
        from Configurables import CopyPrimaryVertices
        cloner=CopyPrimaryVertices(self.personaliseName(sel,
                                                        'CopyPrimaryVertices'))
        cloner.OutputLevel = 4
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneMCInfo(MicroDSTElement) :
    def __call__(self, sel) :
        """
        Copy related MC particles of candidates plus daughters
        """
        from Configurables import P2MCRelatorAlg, CopyParticle2MCRelations
        from Configurables import MCParticleCloner, MCVertexCloner
        # first, get matches MCParticles for selected candidates.
        # This will make a relations table in mainLocation+"/P2MCPRelations"
        p2mcRelator = P2MCRelatorAlg(self.personaliseName(sel,'P2MCRel'))
        p2mcRelator.ParticleLocations = self.dataLocations(sel,'Particles')
        p2mcRelator.OutputLevel=4
        # Now copy relations table + matched MCParticles to MicroDST
        cloner = CopyParticle2MCRelations(self.personaliseName(sel,
                                                               "CopyP2MCRel"))
        cloner.addTool(MCParticleCloner)
        cloner.MCParticleCloner.addTool(MCVertexCloner,
                                        name = 'ICloneMCVertex')
        cloner.InputLocations = self.dataLocations(sel,"P2MCPRelations")
        cloner.OutputLevel = 4
        self.setOutputPrefix(cloner)
        return [p2mcRelator, cloner]

class CloneBTaggingInfo(MicroDSTElement) :
    def __call__(self, sel) :
        from Configurables import BTagging
        from Configurables import CopyFlavourTag
        importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.py')
        BTagAlgo = BTagging(self.personaliseName(sel,'BTagging'))
        BTagAlgo.InputLocations=self.dataLocations(sel,"")
        BTagAlgo.OutputLevel = 4
        cloner = CopyFlavourTag(self.personaliseName(sel,
                                                     "CopyFlavourTag"))
        cloner.InputLocations = self.dataLocations(sel,"FlavourTags")
        cloner.OutputLevel=4
        self.setOutputPrefix(cloner)
        return [BTagAlgo, cloner]


class ClonePVRelations(MicroDSTElement) :
    def __init__(self, PVRelationsMap, branch='') :
        MicroDSTElement.__init__(self, branch)
        self.PVRelMap = PVRelationsMap
        
    def __call__(self, sel) :
        """
        loop over related PV locations and copy each table.
        If no PV copying, keep original PV, but only if no PV re-fitting
        has been performed. If it has, then copy the PV too.
        """
        cloners = []
        for loc, copyPV in self.PVRelMap.iteritems() :
            print "Copy PV relations ", loc
            fullLoc = self.dataLocations(sel, loc)
            cloner = _copyP2PVRelations(sel,"CopyP2PV_"+loc, fullLoc)
            clonerType = cloner.getProp('ClonerType')
            if copyPV == False :
                cloner.ClonerType = 'NONE'
                if hasattr(sel,'algorithm') :
                    alg = sel.algorithm()
                    refitPVs = False
                    if alg != None and alg.properties().has_key('ReFitPVs') :
                        refitPVs =  alg.getProp('ReFitPVs')
                    if refitPVs :
                        cloner.ClonerType = clonerType
            self.setOutputPrefix(cloner)
            cloners += [cloner]
            return cloners

class CloneL0DUReport(MicroDSTElement) :
    def __call__(self, sel) :
        from Configurables import CopyL0DUReport
        cloner = CopyL0DUReport(self.personaliseName(sel,'CopyL0DUReport'))
        self.setOutputPrefix(cloner)
        return [cloner]
    
class CloneHltDecReports(MicroDSTElement) :
    def __call__(self, sel) :
        from Configurables import CopyHltDecReports
        cloner = CopyHltDecReports(self.personaliseName(sel,'CopyHltDecReports'))
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneBackCat(MicroDSTElement) :
    def __call__(self, sel) :
        from Configurables import Particle2BackgroundCategoryRelationsAlg
        from Configurables import CopyParticle2BackgroundCategory
        backCatAlg = Particle2BackgroundCategoryRelationsAlg(self.personaliseName(sel,'BackCatAlg'))
        backCatAlg.InputLocations=self.dataLocations(sel,"Particles")
        cloner =  CopyParticle2BackgroundCategory(self.personaliseName(sel, 'CopyP2BackCat'))
        cloner.InputLocations = self.dataLocations(sel,"P2BCRelations")
        cloner.OutputLevel=4
        self.setOutputPrefix(cloner)
        return [backCatAlg, cloner]

def _copyP2PVRelations(sel, name, locations) :
    from Configurables import CopyParticle2PVRelations
    cloner = CopyParticle2PVRelations(personaliseName(sel,name))
    cloner.InputLocations = locations
    cloner.OutputLevel=4
    return cloner


