'''Collection of MicroDSTElement implementations to perform standard MicroDST cloning.

Each element is in charge of generating a consistently configured set of Configurables
responsible for copying some data to a MicroDST partition. 

'''

__author__  = 'Juan Palacios juan.palacios@nikhef.nl'

__all__ = ( 'CloneRecHeader',
            'CloneRecSummary',
            'CloneODIN',
            'GlobalEventCounters',
            'CloneParticleTrees',
            'ClonePVs',
            'CloneSwimmingReports',
            'CloneMCInfo',
            'CloneBTaggingInfo',
            'ClonePVRelations',
            'CloneTPRelations',
            'ReFitAndClonePVs',
            'CloneL0DUReport',
            'CloneHltDecReports',
            'CloneBackCat',
            'CloneRawBanks',
            'CloneLHCbIDs',
            'CloneTisTosInfo',
            'MoveObjects')

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

class CloneRecSummary(MicroDSTElement) :
    def __call__(self, sel):
        from Configurables import CopyRecSummary
        cloner = CopyRecSummary(self.personaliseName(sel, "CopyRecSummary"))
        self.setOutputPrefix(cloner)
        return [cloner]


class CloneODIN(MicroDSTElement) :
    def __call__(self, sel) :
        from Configurables import CopyODIN
        cloner = CopyODIN(self.personaliseName(sel,"CopyODIN"))
        self.setOutputPrefix(cloner)
        return [cloner]

class GlobalEventCounters(MicroDSTElement) :

    def __init__(self, branch='', configGenerator=None) :
        MicroDSTElement.__init__(self, branch)
        if not configGenerator :
            raise Exception('GlobalEventCounters configGenerator NoneType')
        self.configGenerator = configGenerator

    def __call__(self, sel) :
        cloner = self.configGenerator(self.personaliseName(sel,
                                                           'GlobalEventCounters'))
        cloner.Location = self.branch + "/GlobalEventCounters"
        return [cloner]

class CloneSwimmingReports(MicroDSTElement):
    def __init__(self, inputSwimmingReports = 'Swimming/Reports', branch='') :
        MicroDSTElement.__init__(self, branch)
        self._inputSwimmingReports = inputSwimmingReports
    def __call__(self, sel) :
        from Configurables import CopySwimmingReports
        cloner=CopySwimmingReports(self.personaliseName(sel, 'CopySwimmingReports'))
        cloner.InputLocation = self._inputSwimmingReports
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
                                   ProtoParticleCloner)
        cloner = CopyParticles(self.personaliseName(sel,
                                                    'CopyParticles'))
        cloner.InputLocations = self.dataLocations(sel,"Particles")
        if self.copyPP == False :
            cloner.addTool(ParticleCloner, name="ParticleCloner")
            cloner.ParticleCloner.ICloneProtoParticle="NONE"
        self.setOutputPrefix(cloner)

        confList = ConfigurableList(sel)
        setCloneFilteredParticlesToTrue( confList.flatList() )

        return [cloner]

class ClonePVs(MicroDSTElement) :
    def __init__(self, branch='', copyTracks=False) :
        MicroDSTElement.__init__(self, branch)
        self._copyTracks = copyTracks
    def __call__(self, sel) :
        from Configurables import CopyPrimaryVertices, CopyPVWeights
        clonePV=CopyPrimaryVertices(self.personaliseName(sel,
                                                         'CopyPrimaryVertices'))
        if self._copyTracks :
            clonePV.ClonerType = 'RecVertexClonerWithTracks'
        self.setOutputPrefix(clonePV)
        cloneWeights = CopyPVWeights(self.personaliseName(sel,
                                                          'CopyPVWeights'))
        self.setOutputPrefix(cloneWeights)
        return [clonePV, cloneWeights]

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
        # Now copy relations table + matched MCParticles to MicroDST
        cloner = CopyParticle2MCRelations(self.personaliseName(sel,
                                                               "CopyP2MCRel"))
        cloner.addTool(MCParticleCloner)
        cloner.MCParticleCloner.addTool(MCVertexCloner,
                                        name = 'ICloneMCVertex')
        cloner.InputLocations = self.dataLocations(sel,"P2MCPRelations")
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
        BTagAlgo.Inputs=self.dataLocations(sel,"")
        cloner = CopyFlavourTag(self.personaliseName(sel,
                                                     "CopyFlavourTag"))
        cloner.InputLocations = self.dataLocations(sel,"FlavourTags")
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

class CloneTPRelations(MicroDSTElement) :
    """
    Prepares a CopyParticle2TPRelations generator.
    Constructor arguments:
    location : name of the TES leaf where the Particle->TP relations are stored.
    clonePVs : Clone the target swimming reports?
    branch   : TES branch for output relations table, appended to '/Event'.
    """
    def __init__(self, location, cloneReports=True, branch = '') :
        MicroDSTElement.__init__(self, branch)
        self.location = location
        self.cloneReports = cloneReports
    def __call__(self, sel) :
        from Configurables import CopyParticle2TPRelations
        cloner = CopyParticle2TPRelations(self.personaliseName(sel, "CopyP2TP_"+self.location))
        cloner.InputLocations = self.dataLocations(sel, self.location)
        if self.cloneReports == False:
            cloner.ClonerType = 'NONE'
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
        refitPVs = PVReFitterAlg(self.personaliseName(sel, 'ReFitPvs'))
        refitPVs.ParticleInputLocations = self.dataLocations(sel, 'Particles')
        bestPV = BestPVAlg(self.personaliseName(sel, 'BestPV'))
        bestPV.P2PVRelationsInputLocations = self.dataLocations(sel, refitPVs.name()+'_P2PV')
        cloner = CopyParticle2PVRelations(self.personaliseName(sel, 'CopyReFitP2PV'))
        cloner.InputLocations = self.dataLocations(sel, bestPV.name()+'_P2PV')
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
    def __init__(self, branch = '', banks = [], inputRawEvent = "/DAQ/RawEvent"):
        MicroDSTElement.__init__(self, branch)
        self.banks = list(banks)
        self.inputRawEvent = inputRawEvent
    def __call__(self, sel) :
        from Configurables import RawEventSelectiveCopy
        rawBankCopy = RawEventSelectiveCopy(self.personaliseName(sel, 'CloneRawBank'))
        rawBankCopy.InputRawEventLocation = self.inputRawEvent
        rawBankCopy.RawBanksToCopy = self.banks 
        rawBankCopy.OutputRawEventLocation = self.branch + "/DAQ/RawEvent"
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
        backCatAlg.Inputs=self.dataLocations(sel,"Particles")
        cloner =  CopyParticle2BackgroundCategory(self.personaliseName(sel, 'CopyP2BackCat'))
        cloner.InputLocations = self.dataLocations(sel,"P2BCRelations")
        self.setOutputPrefix(cloner)
        return [backCatAlg, cloner]

class CloneLHCbIDs(MicroDSTElement) :
    def __init__(self, branch='', fullDecayTree=False) :
        MicroDSTElement.__init__(self, branch)
        self._fullDecay = fullDecayTree
    def __call__(self, sel) :
        from Configurables import CopyParticle2LHCbIDs
        cloner =  CopyParticle2LHCbIDs(self.personaliseName(sel,
                                                            'CopyLHCbIDs'),
                                       FullDecayTree = self._fullDecay)
        cloner.InputLocations = self.dataLocations(sel,"Particles")
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneTisTosInfo(MicroDSTElement) :
    def __call__(self, sel) :
        from Configurables import CopyParticle2TisTosDecisions
        cloner =  CopyParticle2TisTosDecisions(self.personaliseName(sel,
                                                            'CopyTisTos'))
        cloner.InputLocations = self.dataLocations(sel,"Particles")
        self.setOutputPrefix(cloner)
        return [cloner]

class MoveObjects(MicroDSTElement) :
    """
    Configurable to move DataObject from one location to another.
    Arguments:

    objects: list of TES locations DataObjects to copy (strings)
    branch: TES branch of output.
    """
    def __init__(self, branch = '', objects = []) :
        MicroDSTElement.__init__(self, branch)
        self.objects = list(objects)

    def __call__(self, sel):
        from Configurables import MoveDataObject
        cloner = MoveDataObject(self.personaliseName(sel, "MoveContainer"))
        cloner.InputLocations = self.objects
        self.setOutputPrefix(cloner)
        return [cloner]
