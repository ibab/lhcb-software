'''Collection of MicroDSTElement implementations to perform standard MicroDST cloning.

Each element is in charge of generating a consistently configured set of Configurables
responsible for copying some data to a MicroDST partition. 

'''

from dstwriterutils import ( setCloneFilteredParticlesToTrue,
                             ConfigurableList,
                             MicroDSTElement )

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
            'MoveObjects' )

class CloneRecHeader(MicroDSTElement) :

    def __call__(self, sel):
        from Configurables import CopyRecHeader
        cloner = CopyRecHeader(self.personaliseName(sel,"CopyRecHeader"))
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneRecSummary(MicroDSTElement) :

    def __call__(self, sel):
        from Configurables import CopyRecSummary
        cloner = CopyRecSummary(self.personaliseName(sel,"CopyRecSummary"))
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

    def __init__( self,
                  branch='',
                  #ProtoParticleCloner = "ProtoParticleCloner",
                  TESVetoList = [ ] ) :
        MicroDSTElement.__init__(self, branch)
        #self.ppCloner    = "ProtoParticleCloner"
        self.tesVetoList = TESVetoList

    def __call__(self, sel) :
        
        from Configurables import ( CopyParticles,
                                    VertexCloner,
                                    ParticleCloner,
                                    ProtoParticleCloner,
                                    TrackCloner,
                                    CaloHypoCloner,
                                    CaloClusterCloner )
        
        cloner = CopyParticles( name = self.personaliseName(sel,'CopyParticles'),
                                InputLocations = self.dataLocations(sel,"Particles") )
        
        cloner.addTool(ParticleCloner, name="ParticleCloner")
        cloner.ParticleCloner.TESVetoList = self.tesVetoList
        cloner.ParticleCloner.ICloneProtoParticle = "ProtoParticleCloner"
                
        cloner.addTool(ProtoParticleCloner,name="ProtoParticleCloner")
        cloner.ProtoParticleCloner.TESVetoList = self.tesVetoList

        cloner.addTool(TrackCloner,name="TrackCloner")
        cloner.TrackCloner.TESVetoList = self.tesVetoList

        cloner.addTool(CaloHypoCloner,name="CaloHypoCloner")
        cloner.CaloHypoCloner.TESVetoList = self.tesVetoList

        cloner.addTool(CaloClusterCloner,name="CaloClusterCloner")
        cloner.CaloClusterCloner.TESVetoList = self.tesVetoList
        
        self.setOutputPrefix(cloner)

        confList = ConfigurableList(sel)
        setCloneFilteredParticlesToTrue( confList.flatList() )
        
        return [cloner]

class ClonePVs(MicroDSTElement) :
    
    def __init__( self,
                  branch          = '',
                  RecVertexCloner = "RecVertexCloner",
                  ClonePVWeights  = True ) :
        MicroDSTElement.__init__(self, branch)
        self.clonerType = RecVertexCloner
        self.clonePVWeights = ClonePVWeights
        
    def __call__(self, sel) :
        algs = [ ]
        from Configurables import CopyPrimaryVertices
        clonePV = CopyPrimaryVertices( name = self.personaliseName(sel,'CopyPrimaryVertices'),
                                       ClonerType = self.clonerType )
        self.setOutputPrefix(clonePV)
        algs += [clonePV]
        if self.clonePVWeights :
            from Configurables import CopyPVWeights
            cloneWeights = CopyPVWeights( name = self.personaliseName(sel,'CopyPVWeights') )
            self.setOutputPrefix(cloneWeights)
            algs += [cloneWeights]
        return algs

class CloneMCInfo(MicroDSTElement) :
    """
    Generator returning list of P2MCRelatorAlg and CopyParticle2MCRelations.
    Copies related MC particles and relations table to '/Event/<branch>/...'
    """
    def __call__(self, sel) :
        """
        Copy related MC particles of candidates plus daughters
        """
        from Configurables import ( P2MCRelatorAlg, CopyParticle2MCRelations,
                                    MCParticleCloner, MCVertexCloner,
                                    CopyMCHeader )
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
        # MC Header
        #copyMCHeader = 
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
    RecVertexCloner : The vertex cloner to use
    """
    def __init__( self,
                  location,
                  clonePVs = True,
                  branch = '',
                  RecVertexCloner = "VertexBaseFromRecVertexCloner" ) :
        MicroDSTElement.__init__(self, branch)
        self.location = location
        self.clonePVs = clonePVs
        self.clonerType = RecVertexCloner
    def __call__(self, sel) :
        from Configurables import CopyParticle2PVRelations
        cloner = CopyParticle2PVRelations(self.personaliseName(sel,"CopyP2PV_"+self.location))
        cloner.InputLocations = self.dataLocations(sel, self.location)
        #clonerType = cloner.getProp('ClonerType')
        cloner.ClonerType = self.clonerType
        if self.clonePVs == False :
            cloner.ClonerType = 'NONE'
            if hasattr(sel,'algorithm') :
                alg = sel.algorithm()
                refitPVs = False
                if alg != None and alg.properties().has_key('ReFitPVs') :
                    refitPVs = alg.getProp('ReFitPVs')
                if refitPVs :
                    cloner.ClonerType = self.clonerType
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
    
    def __init__( self,
                  branch = '',
                  RecVertexCloner = "VertexBaseFromRecVertexCloner" ) :
        MicroDSTElement.__init__(self, branch)
        self.clonerType = RecVertexCloner
        
    def __call__(self, sel) :
        from Configurables import CopyParticle2PVRelations, PVReFitterAlg, BestPVAlg

        # Reftter
        refitPVs = PVReFitterAlg(self.personaliseName(sel, 'ReFitPvs'))
        refitPVs.ParticleInputLocations = self.dataLocations(sel, 'Particles')

        # Pick the best
        bestPV = BestPVAlg(self.personaliseName(sel, 'BestPV'))
        bestPV.P2PVRelationsInputLocations = self.dataLocations(sel, refitPVs.name()+'_P2PV')

        # Clone to stream
        cloner = CopyParticle2PVRelations(self.personaliseName(sel, 'CopyReFitP2PV'))
        cloner.ClonerType = self.clonerType
        cloner.InputLocations = self.dataLocations(sel, bestPV.name()+'_P2PV')
        self.setOutputPrefix(cloner)

        # Return
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
        from Configurables import ( Particle2BackgroundCategoryRelationsAlg,
                                    CopyParticle2BackgroundCategory )
        backCatAlg = Particle2BackgroundCategoryRelationsAlg(self.personaliseName(sel,'BackCatAlg'))
        backCatAlg.Inputs = self.dataLocations(sel,"Particles",True)
        backCatAlg.FullTree = True
        cloner = CopyParticle2BackgroundCategory(self.personaliseName(sel,'CopyP2BackCat'))
        cloner.InputLocations = self.dataLocations(sel,"P2BCRelations")
        self.setOutputPrefix(cloner)
        return [backCatAlg,cloner]

class CloneLHCbIDs(MicroDSTElement) :
    def __init__(self, branch='', fullDecayTree=False) :
        MicroDSTElement.__init__(self, branch)
        self._fullDecay = fullDecayTree
    def __call__(self, sel) :
        from Configurables import CopyParticle2LHCbIDs
        cloner = CopyParticle2LHCbIDs( name = self.personaliseName(sel,'CopyLHCbIDs'),
                                       FullDecayTree  = self._fullDecay,
                                       InputLocations = self.dataLocations(sel,"Particles") )
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneTisTosInfo(MicroDSTElement) :
    def __call__(self, sel) :
        from Configurables import CopyParticle2TisTosDecisions
        cloner = CopyParticle2TisTosDecisions(self.personaliseName(sel,
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
        cloner = MoveDataObject(self.personaliseName(sel,"MoveContainer"))
        cloner.InputLocations = self.objects
        self.setOutputPrefix(cloner)
        return [cloner]

class PackStrippingReports(MicroDSTElement) :
    """
    Configurable to pack Stripping reports
    """
    def __call__(self, sel):
        from Configurables import PackDecReport
        # Packer for the full object, at /Event/Strip/
        fpacker = PackDecReport( name = "PackFullStripReps",
                                 InputName   = "Strip/Phys/DecReports",
                                 OutputName  = "Strip/pPhys/DecReports",
                                 DeleteInput = True )
        # Packer for stream dependant location, if present
        # Probably will not be used, so could be removed eventually,
        # but keep for the moment
##         spacker = PackDecReport( name = self.personaliseName(sel,"PackStripReps"),
##                                  InputName   = self.branch + "/Phys/DecReports",
##                                  OutputName  = self.branch + "/pPhys/DecReports",
##                                  DeleteInput = True )
        return [fpacker]

class PackParticlesAndVertices(MicroDSTElement) :
    """
    Configurable to pack Particles and Vertices into a single location
    """
    def __call__(self, sel):
        from Configurables import PackParticlesAndVertices as PackPsVs
        packer = PackPsVs( name = self.personaliseName(sel,"PackPsAndVs"),
                           InputStream        = self.branch,
                           DeleteInput        = True,
                           EnableCheck        = True,
                           AlwaysCreateOutput = False,
                           VetoedContainers = ["/Event/"+self.branch+"/Rec/Vertex/Primary"] )
        return [packer]

class PackTrackingClusters(MicroDSTElement):
    """
    Configurable to pack Tracking Clusters
    """
    def __call__(self, sel):
        from Configurables import PackCluster
        packer = PackCluster( name       = self.personaliseName(sel,"PackTkClusters"),
                              InputName  = self.branch + "/Rec/Track/Best",
                              OutputName = self.branch + "/pRec/Track/Clusters",
                              AlwaysCreateOutput = False )
        return [packer]

class PackRecObjects(MicroDSTElement) :
    """
    Configurable to pack Rec objects
    """
    def __call__(self, sel):

        deleteInput = True

        testPacking = True

        algs = [ ]

        # NOTE : The order is important here.

        # ProtoParticles
        from Configurables import PackProtoParticle
        algs += [ PackProtoParticle( name = self.personaliseName(sel,"PackChargedProtos"),
                                     AlwaysCreateOutput = False,
                                     DeleteInput        = deleteInput,
                                     EnableCheck        = testPacking,
                                     InputName          = self.branch + "/Rec/ProtoP/Charged",
                                     OutputName         = self.branch + "/pRec/ProtoP/Charged" ),
                  PackProtoParticle( name = self.personaliseName(sel,"PackNeutralProtos"),
                                     AlwaysCreateOutput = False,
                                     DeleteInput        = deleteInput,
                                     EnableCheck        = testPacking,
                                     InputName          = self.branch + "/Rec/ProtoP/Neutrals",
                                     OutputName         = self.branch + "/pRec/ProtoP/Neutrals" )
                  ]

        # CALO Hypos
        from Configurables import PackCaloHypo
        for hypo in [ 'Electrons','Photons','MergedPi0s','SplitPhotons' ] :
            algs += [ PackCaloHypo( name = self.personaliseName(sel,"PackCalo"+hypo),
                                    AlwaysCreateOutput = False,
                                    DeleteInput        = deleteInput,
                                    EnableCheck        = testPacking,
                                    InputName          = self.branch + "/Rec/Calo/"  + hypo,
                                    OutputName         = self.branch + "/pRec/Calo/" + hypo )
                      ]

        # CALO Clusters
        from Configurables import DataPacking__Pack_LHCb__CaloClusterPacker_ as PackCaloClusters
        for type in [ 'EcalClusters','HcalClusters','EcalSplitClusters' ] :
            algs += [ PackCaloClusters( name = self.personaliseName(sel,"Pack"+type),
                                        AlwaysCreateOutput = False,
                                        DeleteInput        = deleteInput,
                                        EnableCheck        = testPacking,
                                        InputName          = self.branch + "/Rec/Calo/"  + type,
                                        OutputName         = self.branch + "/pRec/Calo/" + type )
                      ]

        # RICH PIDs
        from Configurables import DataPacking__Pack_LHCb__RichPIDPacker_ as PackRichPIDs
        algs += [ PackRichPIDs( name = self.personaliseName(sel,"PackRichPIDs"),
                                AlwaysCreateOutput = False,
                                DeleteInput        = deleteInput,
                                EnableCheck        = testPacking,
                                InputName          = self.branch + "/Rec/Rich/PIDs",
                                OutputName         = self.branch + "/pRec/Rich/PIDs" )
                  ]
        
        # MUON PIDs
        from Configurables import DataPacking__Pack_LHCb__MuonPIDPacker_ as PackMuonPIDs
        algs += [ PackMuonPIDs( name = self.personaliseName(sel,"PackMuonPIDs"),
                                AlwaysCreateOutput = False,
                                DeleteInput        = deleteInput,
                                EnableCheck        = testPacking,
                                InputName          = self.branch + "/Rec/Muon/MuonPID",
                                OutputName         = self.branch + "/pRec/Muon/MuonPID" )
                  ]

        # PVs
        from Configurables import PackRecVertex
        #from Configurables import DataPacking__Pack_LHCb__WeightsVectorPacker_ as PackPVWeights
        algs += [ PackRecVertex( name = self.personaliseName(sel,"PackPVs"),
                                 AlwaysCreateOutput = False,
                                 DeleteInput        = deleteInput,
                                 #EnableCheck        = testPacking, # To Do
                                 InputName          = self.branch + "/Rec/Vertex/Primary",
                                 OutputName         = self.branch + "/pRec/Vertex/Primary" )
##                   PackPVWeights( name = self.personaliseName(sel,"PackPVWeights"),
##                                  AlwaysCreateOutput = False,
##                                  DeleteInput        = deleteInput,
##                                  InputName          = self.branch + "/Rec/Vertex/Weights",
##                                  OutputName         = self.branch + "/pRec/Vertex/Weights" )
                  ]

        # Tracks
        from Configurables import PackTrack
        algs += [ PackTrack( name = self.personaliseName(sel,"PackBestTracks"),
                             AlwaysCreateOutput = False,
                             DeleteInput        = deleteInput,
                             EnableCheck        = testPacking,
                             InputName          = self.branch + "/Rec/Track/Best",
                             OutputName         = self.branch + "/pRec/Track/Best" ),
                  PackTrack( name = self.personaliseName(sel,"PackMuonTracks"),
                             AlwaysCreateOutput = False,
                             DeleteInput        = deleteInput,
                             EnableCheck        = testPacking,
                             InputName          = self.branch + "/Rec/Track/Muon",
                             OutputName         = self.branch + "/pRec/Track/Muon" )
                  ]
                
        return algs

class CleanEmptyEventNodes(MicroDSTElement) :
    """
    Configurable to pack Rec objects
    """
    def __call__(self, sel):
        from Configurables import EmptyEventNodeCleaner
        cleaner = EmptyEventNodeCleaner( name = self.personaliseName(sel,"EmptyNodeCleaner"),
                                         InputStream = self.branch )
        return [cleaner]
