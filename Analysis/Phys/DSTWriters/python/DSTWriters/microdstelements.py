'''Collection of MicroDSTElement implementations to perform standard MicroDST cloning.

Each element is in charge of generating a consistently configured set of Configurables
responsible for copying some data to a MicroDST partition. 

'''

__author__  = 'Juan Palacios juan.palacios@nikhef.nl'

__all__ = ( 'CloneRecHeader',
            'CloneODIN',
            'CloneParticleTrees',
            'ClonePVs',
            'CloneMCInfo',
            'CloneBTaggingInfo',
            'ClonePVRelations',
            'ReFitAndClonePVs',
            'CloneL0DUReport',
            'CloneHltDecReports',
            'CloneBackCat',
            'CloneRawBanks')

from dstwriterutils import (setCloneFilteredParticlesToTrue,
                            ConfigurableList,
                            MicroDSTElement)

from GaudiConf.Configuration import *

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
    """
    Generator returning list of P2MCRelatorAlg and CopyParticle2MCRelations.
    Copies related MC particles and relations table to '/Event/<branch>/...'
    """
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
    """
    Generator returning consistently configured list of BTagging and CopyFlavourTags
    Configurables.
    """
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
    """
    Prepares a CopyParticle2PVRelations generator.
    Constructor arguments:
    location : name of the TES leaf where the Particle->PV relations are stored.
    clonePVs : Clone the target primary vertices?
    branch   : TES branch for output relations table, appended to '/Event'.
    """
    def __init__(self, location, clonePVs=True, branch = '') :
        MicroDSTElement.__init__(self, branch)
        self.location = location
        self.clonePVs = clonePVs
    def __call__(self, sel) :
        from Configurables import CopyParticle2PVRelations
        cloner = CopyParticle2PVRelations(self.personaliseName(sel, "CopyP2PV_"+self.location))
        cloner.InputLocations = self.dataLocations(sel, self.location)
        cloner.OutputLevel=4
        clonerType = cloner.getProp('ClonerType')
        if self.clonePVs == False :
            cloner.ClonerType = 'NONE'
            if hasattr(sel,'algorithm') :
                alg = sel.algorithm()
                refitPVs = False
                if alg != None and alg.properties().has_key('ReFitPVs') :
                    refitPVs =  alg.getProp('ReFitPVs')
                if refitPVs :
                    cloner.ClonerType = clonerType
        self.setOutputPrefix(cloner)
        return [cloner]


class ReFitAndClonePVs(MicroDSTElement) :
    '''
    Return a list with the Configurables necessary to re-fit PVs and find
    the best one for each particle in a SelectionSequence.
    Store Particle->PV table and re-fitted PVs in MicroDST.
    Returns a sequence of PVReFitterAlg, BestPVAlg and CopyParticle2PVRelations.
    Useage:
    sel = ... # some SelectionSequence
    p2pv = ReFitAndClonePVs(branch = "Test")
    necessary_configurables = p2pv(sel)
    Relations tables go to locations "<branch>/<location>/BestPV_<sel.name()>P2PV", where
    <location> is each of the entries in sel.outputLocations().
    '''

    def __call__(self, sel) :
        from Configurables import CopyParticle2PVRelations, PVReFitterAlg, BestPVAlg
        refitPVs = PVReFitterAlg(self.personaliseName(sel, 'ReFitPvs'),
                                 ParticleInputLocations = self.dataLocations(sel, 'Particles') )
        bestPV = BestPVAlg(self.personaliseName(sel, 'BestPV'),
                           P2PVRelationsInputLocations = self.dataLocations(sel, refitPVs.name()+'_P2PV') )
        cloner = CopyParticle2PVRelations(self.personaliseName(sel, 'CopyReFitP2PV'),
                                          InputLocations = self.dataLocations(sel, bestPV.name()+'_P2PV') )
        self.setOutputPrefix(cloner)
        return [refitPVs, bestPV, cloner]
        
class CloneL0DUReport(MicroDSTElement) :
    """
    Configurables necessary to copy LODUReport from standard location.
    """
    def __call__(self, sel) :
        from Configurables import CopyL0DUReport
        cloner = CopyL0DUReport(self.personaliseName(sel,'CopyL0DUReport'))
        self.setOutputPrefix(cloner)
        return [cloner]
    
class CloneHltDecReports(MicroDSTElement) :
    """
    Configurables necessary to copy HltDecReports from standard location.
    """
    def __init__(self, branch = '', locations = ['Hlt/DecReports']) :
        MicroDSTElement.__init__(self, branch)
        self.locations = list(locations)
    def __call__(self, sel) :
        from Configurables import CopyHltDecReports
        cloner = CopyHltDecReports(self.personaliseName(sel,
                                                        'CopyHltDecReports'))
        cloner.InputLocations = self.locations
        self.setOutputPrefix(cloner)
        return [cloner]


class CloneRawBanks(MicroDSTElement) :
    """
    Configurables to copy selected raw banks
    to '<branch>/DAW/RawEvent'.
    Arguments:

    banks: list of raw banks to copy (strings)
    branch: TES branch of output.
    
    """
    def __init__(self, branch = '', banks = []) :
        MicroDSTElement.__init__(self, branch)
        self.banks = list(banks)
    def __call__(self, sel) :
        from Configurables import RawEventSelectiveCopy
        rawBankCopy = RawEventSelectiveCopy(self.personaliseName(sel, 'CloneRawBank'))
        rawBankCopy.RawBanksToCopy = self.banks 
        rawBankCopy.OutputRawEventLocation = self.branch + "/DAQ/RawEvent"
        rawBankCopy.OutputLevel=4
        return [rawBankCopy]

class CloneBackCat(MicroDSTElement) :
    """
    Generator for list of Particle2BackgroundCategoryRelationsAlg and CopyParticle2BackgroundCategory.
    Used for 
    """
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
