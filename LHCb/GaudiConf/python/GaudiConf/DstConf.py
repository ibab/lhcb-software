"""
High level configuration tools for LHCb applications
"""
__version__ = "v17r0"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

__all__ = [
    'DstConf'  ## the configurable, configures DST writing/packing/unpacking
    ]

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from CaloPackingConf import CaloDstPackConf, CaloDstUnPackConf
from Configurables   import LHCbConfigurableUser
from Configurables   import RawEventFormatConf
from SimConf  import SimConf
from DigiConf import DigiConf

class DummyWriter(LHCbConfigurableUser):
    __slots__ = { "ItemList" : [], "OptItemList" : [] }

class DstConf(LHCbConfigurableUser):

    __slots__ = {
         "DstType"         : "NONE"
       , "SimType"         : "None"
       , "EnableUnpack"    : [ ]
       , "EnablePackingChecks" : False
       , "PackType"        : "TES"
       , "PackSequencer"   : None
       , "Detectors"       : ['Velo','TT','IT','OT','Rich','Tr','Calo','Muon']
       , "AlwaysCreate"    : False
       , "Turbo"           : False
       , "Writer"          : "DstWriter"
       , "OutputName"      : ""
       , "SpilloverPaths"  : [ "Prev", "PrevPrev", "Next", "NextNext" ]
       , "DataType"        : ""
       , "WriteFSR"        : True
       , "SplitRawEventOutput" : 2.0 #split raw event from Stripping 20
       , "PVPersistence"   : "RecVertex"
         }

    _propertyDocDct = {
        'DstType'         : """ Type of dst, can be ['DST', 'RDST', 'XDST','MDST', 'LDST' ] """
       ,'SimType'         : """ Type of simulation output, can be ['None','Minimal','Full'] """
       ,'EnableUnpack'    : """ List of DST container types to be set up for on demand unpacking. See KnownUnpackingTypes """
       ,'EnablePackingChecks' : """ Flag to turn on the running of various unpacking checks, to test the quality of the data packing """
       ,'PackType'        : """ Type of packing for the DST, can be ['NONE','TES','MDF'] """
       ,'PackSequencer'   : """ Sequencer in which to run the packing algorithms """
       ,'AlwaysCreate'    : """ Flags whether to create output packed objects even if input missing """
       ,'Turbo'           : """ Propagate Turbo stream locations """
       ,'Writer'          : """ Name of OutputStream writing the DST """
       ,'OutputName'      : """ Name of the output file, for MDF writing """
       ,'SpilloverPaths'  : """ Paths to write to XDST if available on input file """
       ,'DataType'        : """ Flag for backward compatibility with old data """
       ,'WriteFSR'        : """ Flags whether to write out an FSR """
       ,"SplitRawEventOutput" : "Which version fo the split raw event to write out, default 2.0, same as Reco 14/Stripping 20 DSTs"
       }

    __used_configurables__ = [
        CaloDstPackConf    ,
        CaloDstUnPackConf  ,
        SimConf            ,
        DigiConf
        ]

    KnownSimTypes       = ['None','Minimal','Full']
    KnownDstTypes       = ['NONE','DST','RDST', 'XDST','SDST','MDST', 'LDST']
    KnownPackTypes      = ['NONE','TES','MDF']
    KnownUnpackingTypes = ["Reconstruction","Stripping","Tracking"]

    def _doWrite( self, dType, pType, sType ):
        """
        Define the file content and write it out
        """

        writer = DummyWriter()
        self._defineOutputData( dType, pType, sType, writer )

        if pType == 'MDF':
            if sType != 'None' :
                raise TypeError( "MDF packing not supported for Simulation" )
            self._doWriteMDF( writer.ItemList )
        else:
            self._doWriteROOT( writer.ItemList, writer.OptItemList )


    def _defineOutputData( self, dType, pType, sType, writer ):
        """
        Define content of the output dataset
        """

        # Choose whether to write packed or unpacked objects
        if pType == 'NONE':
            recDir = "Rec"
            if sType != "None":
                DigiConf().setProp("EnablePack", False)
                SimConf().setProp("EnablePack", DigiConf().getProp("EnablePack") )
        else:
            if not hasattr( self, "PackSequencer" ):
                raise TypeError( "Packing requested but PackSequencer not defined" )
            recDir = "pRec"
            if sType != "None":
                DigiConf().setProp("EnablePack", True)
                self.setOtherProps(DigiConf(),["PackSequencer"])
                SimConf().setProp("EnablePack", DigiConf().getProp("EnablePack") )

        # Add depth if not MDF
        if pType == 'MDF':
            depth = ""
        else:
            depth = "#1"

        writer.ItemList += [   "/Event/Rec/Header"                       + depth
                             , "/Event/Rec/Status"                       + depth]
        from Configurables import LHCbApp

        writer.ItemList += [ "/Event/Rec/Summary"                      + depth]

        writer.ItemList += [ "/Event/" + recDir + "/Track/Best"        + depth]

        if( self.getProp("PVPersistence") is "RecVertex" ) :
            writer.ItemList += [ "/Event/" + recDir + "/Vertex/Primary"    + depth ]
            # for 2015, fitted velo tracks are stored on DST to make PV refit possible offline
            if( self.getProp("DataType") is "2015" ):
                writer.ItemList += [ "/Event/" + recDir + "/Track/FittedHLT1VeloTracks" + depth]
        elif( self.getProp("PVPersistence") is "PrimaryVertex" ) :
            writer.ItemList += [ "/Event/" + recDir + "/Vertex/PrimaryVertices" + depth ]
        else:
             raise RuntimeError("Unknown PVPersistence type: '%s'" % self.getProp("PVPersistence") )
                                
        if "Rich" in self.getProp("Detectors"):
            writer.ItemList += [ "/Event/" + recDir + "/Rich/PIDs"         + depth]
        if "Muon" in self.getProp("Detectors"):
            writer.ItemList += [ "/Event/" + recDir + "/Muon/MuonPID"      + depth]
        if "Calo" in self.getProp("Detectors"):
            writer.ItemList += [ "/Event/" + recDir + "/Calo/Electrons"    + depth
                                 , "/Event/" + recDir + "/Calo/Photons"      + depth
                                 , "/Event/" + recDir + "/Calo/MergedPi0s"   + depth
                                 , "/Event/" + recDir + "/Calo/SplitPhotons" + depth]

        writer.ItemList += [ "/Event/" + recDir + "/ProtoP/Charged"    + depth
                             , "/Event/" + recDir + "/ProtoP/Neutrals"   + depth
                             , "/Event/" + recDir + "/Vertex/V0"         + depth]

        if "Muon" in self.getProp("Detectors"):
            writer.ItemList += [ "/Event/" + recDir + "/Track/Muon"        + depth ]

        # Additional objects not packable as MDF
        if pType != "MDF":
            #Add the RawEvent, except for RDST. get RawEventLocations from the RawEventFormat database
            if dType != "RDST":
                #copied code from the RawEventJuggler
                RawEventFormatConf().loadIfRequired()
                from RawEventCompat.Configuration import _replaceWrap, _getDict, ReverseDict
                locs,rec=_getDict()
                output_locations=ReverseDict(self.getProp("SplitRawEventOutput"),locs,rec)
                for loc in output_locations:
                    if not loc.startswith("/Event/"):
                        loc=("/Event/"+loc).replace("//","/")
                    writer.ItemList += [ _replaceWrap(loc)+"#1" ]
            
            # Add the simulation objects if simulation DST
            if sType != "None":
                
                eventLocations = ['']
                if dType == "XDST":
                    eventLocations = SimConf().allEventLocations()

                # Minimal MC output.
                SimConf().addHeaders(writer)
                SimConf().addMCVertices(writer,eventLocations)

                if self.getProp('Turbo'):
                    writer.OptItemList += [
                            "/Event/Turbo/pPhys/Particles#99"
                            ,"/Event/Turbo/pPhys/Vertices#99"
                            ,"/Event/Turbo/pPhys/RecVertices#99"
                            ,"/Event/Turbo/pPhys/Relations#99"
                            ,"/Event/Turbo/pPhys/PP2MCPRelations#99"
                            ,"/Event/Turbo/pRec/Track/Custom#99"
                            ,"/Event/Turbo/pRec/Muon/CustomPIDs#99"
                            ,"/Event/Turbo/pRec/Rich/CustomPIDs#99"
                            ,"/Event/Turbo/pRec/ProtoP/Custom#99"
                            ]

                if sType == "Full":

                    writer.ItemList += [
                        # Links to from reconstructed objects to MCParticles
                        "/Event/Link/Rec/Track/Best#1"]
                    if "Calo" in self.getProp("Detectors"):
                        writer.ItemList += ["/Event/Link/Rec/Calo/Photons#1",
                                            "/Event/Link/Rec/Calo/Electrons#1",
                                            "/Event/Link/Rec/Calo/MergedPi0s#1",
                                            "/Event/Link/Rec/Calo/SplitPhotons#1"
                                            ]

                    # Objects propagated from Gauss
                    # Generation information
                    SimConf().addGenInfo(writer)
                    # MCparticles
                    SimConf().addMCParticles(writer,eventLocations)

                    # Objects propagated from Boole
                    # Digi headers
                    DigiConf().addHeaders(writer)
                    DigiConf().addMCTrackInfo(writer)
                    DigiConf().addMCCaloInfo(writer)

                    if dType == "XDST":
                        # Digitisation summaries
                        DigiConf().addMCDigitSummaries(writer)
                        # Add the MCHits (from Gauss) and links to them (from Boole)
                        SimConf().addSubDetSimInfo(writer)
                        DigiConf().addMCHitLinks(writer)
                    if dType in ["XDST", "LDST"]:
                        # Links from Digits to MCParticles
                        DigiConf().addMCParticleLinks(writer)


    def _doWriteROOT( self, items, optItems ):
        """
        Write a DST (or XDST) in ROOT format
        """
        writer = OutputStream( self.getProp("Writer") )
        writer.Preload = False
        writer.ItemList += items
        writer.OptItemList += optItems
        log.info( "%s.ItemList=%s"%(self.getProp("Writer"),writer.ItemList) )
        log.info( "%s.OptItemList=%s"%(self.getProp("Writer"),writer.OptItemList) )
        from GaudiConf.IOHelper import IOHelper
        # Set a default file name if not already set
        if not writer.isPropertySet("Output") :
            outputFile = self.getProp("OutputName")
            outputFile = "PFN:"+outputFile + '.' + self.getProp("DstType").lower()
        else:
            outputFile = IOHelper().undressFile( writer.getProp("Output") )
        # Add to the ApplicationMgr with corresct output persistency
        IOHelper().outStream( outputFile, "OutputStream/"+self.getProp("Writer"), self.getProp("WriteFSR") )

    def _doWriteMDF( self, items ):
        """
        Write an DST in MDF format
        """
        from Configurables import  WritePackedDst, LHCb__MDFWriter

        MDFpacker = WritePackedDst('MdfPacker')
        MDFpacker.Containers += items

        MDFwr = LHCb__MDFWriter('MdfWriter')
        MDFwr.Connection = self.getProp( "OutputName" ) + '.mdf'
        MDFwr.Compress = 2
        MDFwr.GenerateMD5 = True
        MDFwr.BankLocation = '/Event/DAQ/DstEvent'

        GaudiSequencer("WriteMDFSeq").Members += [ MDFpacker, MDFwr ]

    def _doPack( self ):
        """
        Set up the sequence to create the packed containers
        """
        packDST = self.getProp("PackSequencer")

        from Configurables import ( PackTrack, PackCaloHypo, PackProtoParticle,
                                    PackRecVertex, PackTwoProngVertex,
                                    DataPacking__Pack_LHCb__RichPIDPacker_,
                                    DataPacking__Pack_LHCb__MuonPIDPacker_,
                                    PackPrimaryVertices,
                                    ChargedProtoParticleRemovePIDInfo )

        alwaysCreate = self.getProp("AlwaysCreate")
        doChecks     = self.getProp("EnablePackingChecks")

        # Pack tracks if they were not read from the input file...
        if "Tracking" not in self.getProp("EnableUnpack"): 
            packDST.Members += [ PackTrack( name               = "PackTracks",
                                            AlwaysCreateOutput = alwaysCreate,
                                            EnableCheck        = doChecks ) ]
            # for 2015, add packed Velo tracks
            if( (self.getProp("DataType") is "2015") and (self.getProp("PVPersistence") is "RecVertex") ):
                packFittedVelo = PackTrack( name               = "PackTracksFittedVelo",
                                            InputName          = "Rec/Track/FittedHLT1VeloTracks",
                                            OutputName         = "pRec/Track/FittedHLT1VeloTracks",
                                            AlwaysCreateOutput = alwaysCreate ,
                                            EnableCheck        = doChecks )
                packDST.Members += [ packFittedVelo ]
            ###
        richpidpack = DataPacking__Pack_LHCb__RichPIDPacker_( name               = "PackRichPIDs",
                                                              AlwaysCreateOutput = alwaysCreate,
                                                              EnableCheck        = doChecks )
        muonpidpack = DataPacking__Pack_LHCb__MuonPIDPacker_( name               = "PackMuonPIDs",
                                                              AlwaysCreateOutput = alwaysCreate,
                                                              EnableCheck        = doChecks )

        if "Rich" in self.getProp("Detectors"):
            packDST.Members += [ richpidpack ]
        if "Muon" in self.getProp("Detectors"):
            packDST.Members += [ muonpidpack ]

        caloPackSeq = GaudiSequencer("CaloPacking")
        if "Calo" in self.getProp("Detectors"):
            packDST.Members += [caloPackSeq]

            caloPack = CaloDstPackConf()
            if not caloPack.isPropertySet('Enable') :
                CaloDstPackConf ( Enable = True )
            caloPack.Sequence     = caloPackSeq
            caloPack.AlwaysCreate = alwaysCreate
            caloPack.EnableChecks = doChecks

        # Clean the PID information in the Charged ProtoParticles
        protoPidClean = ChargedProtoParticleRemovePIDInfo("ProtoParticlePIDClean")
        packDST.Members += [protoPidClean]

        # packed the charged protos
        packChargedPs = PackProtoParticle( name               = "PackChargedProtos",
                                           AlwaysCreateOutput = alwaysCreate,
                                           InputName          = "/Event/Rec/ProtoP/Charged",
                                           OutputName         = "/Event/pRec/ProtoP/Charged",
                                           EnableCheck        = doChecks )
        packDST.Members += [packChargedPs]

        # pack the neutral protos
        packNeutralPs = PackProtoParticle( name               = "PackNeutralProtos",
                                           AlwaysCreateOutput = alwaysCreate,
                                           InputName          = "/Event/Rec/ProtoP/Neutrals",
                                           OutputName         = "/Event/pRec/ProtoP/Neutrals",
                                           EnableCheck        = doChecks )
        packDST.Members += [packNeutralPs]

        # Pack vertices if they were not read from the input file...
        if "Tracking" not in self.getProp("EnableUnpack"): 
            if self.getProp("PVPersistence") is "RecVertex":
                packDST.Members += [ PackRecVertex(AlwaysCreateOutput = alwaysCreate) ]
            elif self.getProp("PVPersistence") is "PrimaryVertex":
                packDST.Members += [ PackPrimaryVertices() ]                
            packDST.Members += [ PackTwoProngVertex(AlwaysCreateOutput = alwaysCreate) ]

        if "Muon" in self.getProp("Detectors"):
            packDST.Members += [ PackTrack( name               = "PackMuonTracks",
                                            AlwaysCreateOutput = alwaysCreate,
                                            InputName          = "/Event/Rec/Track/Muon",
                                            OutputName         = "/Event/pRec/Track/Muon",
                                            EnableCheck        = doChecks ) ]
        
        # In MDF case, add a sub sequence for the MDF writing
        if self.getProp( "PackType" ).upper() == "MDF":
            packDST.Members += [ GaudiSequencer("WriteMDFSeq") ]

        # Tests
        if self.getProp("EnablePackingChecks") : self._doPackChecks()

        
    def _doPackChecks(self):

        packDST = self.getProp("PackSequencer")
        from Configurables import GaudiSequencer
        checks = GaudiSequencer("PackingChecks")
        packDST.Members += [checks]

        from Configurables import ( UnpackRecVertex, UnpackTwoProngVertex )
        from Configurables import ( CompareRecVertex, CompareTwoProngVertex )

        # Unpack to temporary locations
        tempLoc = "Test"
        unpackVertex       = UnpackRecVertex("UnpackVertexTest")
        unpackV0           = UnpackTwoProngVertex("UnpackV0Test")
        unpackVertex.OutputName = unpackVertex.getProp("OutputName")+tempLoc
        unpackV0.OutputName     = unpackV0.getProp("OutputName")+tempLoc

        checks.Members += [ unpackVertex, unpackV0 ]

        # Comparisons
        checkVertex    = CompareRecVertex("CheckPackedVertices")
        checkV0        = CompareTwoProngVertex("CheckPackedV0s")

        checks.Members += [ checkVertex, checkV0 ]

    def _doUnpack( self ):
        """
        Set up DataOnDemandSvc to unpack reconstruction information
        """
        from Configurables import ( UnpackTrack, UnpackCaloHypo, UnpackProtoParticle,
                                    UnpackRecVertex, UnpackTwoProngVertex,UnpackPrimaryVertices,
                                    DataPacking__Unpack_LHCb__WeightsVectorPacker_,
                                    RecToPVConverter, PVToRecConverter)

        log.debug("In DstConf._doUnpack")

        DataOnDemandSvc().AlgMap[ "/Event/Rec/Track/Best" ]     = UnpackTrack()
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/V0" ]      = UnpackTwoProngVertex()
       
        if( self.getProp("PVPersistence") is "RecVertex" ):
            DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/Primary" ] = UnpackRecVertex()
            pvWunpack = DataPacking__Unpack_LHCb__WeightsVectorPacker_("UnpackPVWeights")
            DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/Weights" ] = pvWunpack
            DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/PrimaryVertices"] = RecToPVConverter()
        elif( self.getProp("PVPersistence") is "PrimaryVertex" ): 
            DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/PrimaryVertices" ] = UnpackPrimaryVertices()       
            DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/Primary" ] = PVToRecConverter()
        else:
             raise RuntimeError("Unknown PVPersistence type: '%s'" % self.getProp("PVPersistence") )

        if( self.getProp("DataType") is "2015" ):
            unpackFittedVeloTracks = UnpackTrack("unpackFittedVeloTracks")
            unpackFittedVeloTracks.InputName = "pRec/Track/FittedHLT1VeloTracks"
            unpackFittedVeloTracks.OutputName = "Rec/Track/FittedHLT1VeloTracks"
            DataOnDemandSvc().AlgMap[ "/Event/Rec/Track/FittedHLT1VeloTracks" ] = unpackFittedVeloTracks


        if "Tracking" in self.getProp("EnableUnpack") : return # skip the rest

        # Calo
        if "Calo" in self.getProp("Detectors"):
            caloUnpack = CaloDstUnPackConf ()
            if not caloUnpack.isPropertySet('Enable') :
                log.debug( "Setting caloUnpack.Enable = True" )
                caloUnpack.Enable = True
            else :
                log.debug( "Not setting caloUnpack.Enable. Current value = %s", caloUnpack.Enable )

        # RichPIDs
        if "Rich" in self.getProp("Detectors"):
            self._unpackRichPIDs()

        # MuonPIDs
        if "Muon" in self.getProp("Detectors"):
            self._unpackMuonPIDs()

        # ProtoParticles
        self._unpackProtoParticles()

        # Muon Tracks
        if "Muon" in self.getProp("Detectors"):
            unpackMuons = UnpackTrack( name       = "UnpackMuonTracks",
                                       OutputName = "/Event/Rec/Track/Muon",
                                       InputName  = "/Event/pRec/Track/Muon",
                                       AncestorFor= "/Event/pRec/Track/Muon" )
            DataOnDemandSvc().AlgMap[ "/Event/Rec/Track/Muon" ] = unpackMuons

    def _unpackMuonPIDs(self):

        from Configurables import ( GaudiSequencer,
                                    DataPacking__Unpack_LHCb__MuonPIDPacker_ )

        mpidLoc = "/Event/Rec/Muon/MuonPID"

        mpidSeq = GaudiSequencer("UnpackMuonPIDSeq")
        DataOnDemandSvc().AlgMap[ mpidLoc ] = mpidSeq

        # Unpacking alg
        unpackmuonpid = DataPacking__Unpack_LHCb__MuonPIDPacker_("UnpackMuonPIDs")
        mpidSeq.Members += [unpackmuonpid]

        # Additional processing, not for MDST
        inputtype = self.getProp('DstType').upper()
        if inputtype != 'MDST':

            # For backwards compat, also run an alg that if need be recreates MuonPIDs
            # from ProtoParticles. Can eventually be removed.
            from Configurables import MuonPIDsFromProtoParticlesAlg
            mpidSeq.Members += [ MuonPIDsFromProtoParticlesAlg("CheckMuonPIDs") ]

    def _unpackRichPIDs(self):

        from Configurables import ( GaudiSequencer,
                                    DataPacking__Unpack_LHCb__RichPIDPacker_ )

        rpidLoc = "/Event/Rec/Rich/PIDs"

        rpidSeq = GaudiSequencer("UnpackRichPIDSeq")
        DataOnDemandSvc().AlgMap[ rpidLoc ] = rpidSeq

        # Alg that unpacks data from packed data
        unpackrichpid = DataPacking__Unpack_LHCb__RichPIDPacker_("UnpackRichPIDs")
        rpidSeq.Members += [ unpackrichpid ]

        # Additional processing, not for MDST
        inputtype = self.getProp('DstType').upper()
        if inputtype != 'MDST'  :

            # For backwards compat, also run an alg that if need be recreates RichPIDs
            # from ProtoParticles. Can eventually be removed.
            from Configurables import RichPIDsFromProtoParticlesAlg
            rpidSeq.Members += [ RichPIDsFromProtoParticlesAlg("CheckRichPIDs") ]

    def _unpackProtoParticles(self):

        from Configurables import ( GaudiSequencer, UnpackProtoParticle )

        # Neutrals
        # --------

        neutralLoc = "/Event/Rec/ProtoP/Neutrals"
        unpackNeutrals = UnpackProtoParticle(name       = "UnpackNeutralProtos",
                                             OutputName = neutralLoc,
                                             InputName  = "/Event/pRec/ProtoP/Neutrals")
        DataOnDemandSvc().AlgMap[neutralLoc] = unpackNeutrals

        # Charged
        # -------

        chargedLoc = "/Event/Rec/ProtoP/Charged"
        chargedSeq = GaudiSequencer("UnpackChargedProtosSeq")
        DataOnDemandSvc().AlgMap[chargedLoc] = chargedSeq

        # Unpacker
        unpackCharged = UnpackProtoParticle(name       = "UnpackChargedProtos",
                                            OutputName = chargedLoc,
                                            InputName  = "/Event/pRec/ProtoP/Charged")
        chargedSeq.Members += [unpackCharged]

        # Additional processing, not for MDST
        inputtype = self.getProp('DstType').upper()
        if inputtype != 'MDST'  :

            # PID calibration
            from Configurables import ( ChargedProtoParticleAddRichInfo,
                                        ChargedProtoParticleAddMuonInfo,
                                        ChargedProtoCombineDLLsAlg,
                                        TESCheck )
            recalib = GaudiSequencer("ProtoParticleCombDLLs")
            recalib.IgnoreFilterPassed = False
            chargedSeq.Members += [ recalib ]
            # Filter to check in Protos exist
            recalib.Members += [ TESCheck( name        = "CheckChargedProtosExist",
                                           Inputs      = [chargedLoc],
                                           OutputLevel = 5,
                                           Stop        = False ) ]
            # Add Rich and Muon PID results to protoparticles
            recalib.Members += [ChargedProtoParticleAddMuonInfo("ChargedProtoPAddMuon")]
            recalib.Members += [ChargedProtoParticleAddRichInfo("ChargedProtoPAddRich")]
            # Combined DLLs
            recalib.Members += [ChargedProtoCombineDLLsAlg("ChargedProtoPCombDLL")]

    def _unpackStripping(self):

        from Configurables import ( ConversionDODMapper,
                                    ParticlesAndVerticesMapper,
                                    TrackClustersMapper )

        # General unpacking mapping
        mapper     = ConversionDODMapper("UnpackRecPhysMapper")
        # The input <-> output mappings
        mapper.Transformations = [ ( '(.*)/Rec(.*)',  '$1/pRec$2'  ),
                                   ( '(.*)/Phys(.*)', '$1/pPhys$2' ),
                                   ( '(.*)/MC(.*)',   '$1/pMC$2'   ) ]
        # algorithm types from source ClassIDs
        mapper.Algorithms[1550] = "UnpackTrack"
        mapper.Algorithms[1552] = "UnpackProtoParticle"
        if "Calo" in self.getProp("Detectors"):
            mapper.Algorithms[1551] = "UnpackCaloHypo"
        if "Rich" in self.getProp("Detectors"):
            mapper.Algorithms[1561] = "DataPacking::Unpack<LHCb::RichPIDPacker>"
        if "Muon" in self.getProp("Detectors"):
            mapper.Algorithms[1571] = "DataPacking::Unpack<LHCb::MuonPIDPacker>"
        mapper.Algorithms[1553] = "UnpackRecVertex"
        mapper.Algorithms[1555] = "DataPacking::Unpack<LHCb::WeightsVectorPacker>"
        mapper.Algorithms[1581] = "UnpackParticlesAndVertices"
        mapper.Algorithms[1559] = "UnpackDecReport"
        if "Calo" in self.getProp("Detectors"):
            mapper.Algorithms[1541] = "DataPacking::Unpack<LHCb::CaloClusterPacker>"
        mapper.Algorithms[1510] = "UnpackMCParticle"
        mapper.Algorithms[1511] = "UnpackMCVertex"

        # Packed Particles and Vertices
        pvmapper = ParticlesAndVerticesMapper("UnpackPsAndVsMapper")

        # Cluster upacking
        clusmapper = TrackClustersMapper("UnpackTkClustersMapper")
        # Lite cluster creation
        lclusmapper = ConversionDODMapper("LiteClusterMapper")
        lclusmapper.InputOptionName  = "inputLocation"
        lclusmapper.OutputOptionName = "outputLocation"
        lclusmapper.Transformations = [ ('(.*)/Raw/(.*)/LiteClusters','$1/Raw/$2/Clusters') ]
        if "Velo" in self.getProp("Detectors"):
            lclusmapper.Algorithms[397222] = "VeloClustersToLite"
        lclusmapper.Algorithms[402220] = "STClustersToLite"
       
        # Add the tools to the DOD service tools lists
        tools = [clusmapper,lclusmapper,pvmapper,mapper]
        DataOnDemandSvc().NodeMappingTools += tools
        DataOnDemandSvc().AlgMappingTools  += tools

    def __apply_configuration__(self):

        log.info(self)

        sType = self.getProp( "SimType" ).capitalize()
        dType = self.getProp( "DstType" ).upper()
        
        if sType not in self.KnownSimTypes:
            raise TypeError( "Unknown SimType '%s'"%sType )
        if sType != "None":
            if dType == 'RDST':
                raise TypeError( "Cannot have RDST for simulation!" )
            DigiConf().setProp("EnableUnpack",True )
            SimConf().setProp("EnableUnpack",DigiConf().getProp("EnableUnpack"))

        if dType not in self.KnownDstTypes:
            raise TypeError( "Unknown DstType '%s'"%dType )
        if dType == "SDST":
            log.warning("SDST output type is obsolete, setting it to DST")
            dtype = "DST"

        # Propagate SpilloverPaths and DataType to DigiConf and to SimConf via DigiConf
        self.setOtherProps(DigiConf(),["SpilloverPaths","DataType", "Detectors"])
        DigiConf().setOtherProps(SimConf(),["SpilloverPaths","DataType"])

        pType = self.getProp( "PackType" ).upper()
        if pType not in self.KnownPackTypes:
            raise TypeError( "Unknown PackType '%s'"%pType )

        # Unpacking options
        for unpackT in self.getProp("EnableUnpack") :
            if unpackT not in self.KnownUnpackingTypes :
                raise TypeError( "Unknown Unpacking type '%s'"%unpackT )
        if "Reconstruction" in self.getProp("EnableUnpack") or "Tracking" in self.getProp("EnableUnpack") : self._doUnpack()
        if "Stripping"      in self.getProp("EnableUnpack") : self._unpackStripping()

        if dType != 'NONE':
            if hasattr( self, "PackSequencer" ): self._doPack()
            GaudiKernel.ProcessJobOptions.PrintOn()
            self._doWrite(dType, pType, sType)
            GaudiKernel.ProcessJobOptions.PrintOff()


        
