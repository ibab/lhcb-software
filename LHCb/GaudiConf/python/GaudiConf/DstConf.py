"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: DstConf.py,v 1.34 2009-12-17 15:16:11 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

__all__ = [
    'DstConf'  ## the configurable, configures DST writing/packing/unpacking   
    ]

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from CaloPackingConf import CaloDstPackConf, CaloDstUnPackConf
from Configurables   import LHCbConfigurableUser
from SimConf  import SimConf
from DigiConf import DigiConf

class DummyWriter(LHCbConfigurableUser):
    __slots__ = { "ItemList" : [], "OptItemList" : [] }

class DstConf(LHCbConfigurableUser):
    
    __slots__ = {
         "DstType"        : "NONE"
       , "SimType"        : "None"
       , "EnableUnpack"   : False
       , "EnablePackingChecks" : False
       , "PackType"       : "TES"
       , "PackSequencer"  : None
       , "AlwaysCreate"   : False
       , "Writer"         : "DstWriter"
       , "OutputName"     : ""
       , "SpilloverPaths" : [ "Prev", "PrevPrev", "Next" ]
       , "DataType"       : ""
         }

    _propertyDocDct = { 
        'DstType'       : """ Type of dst, can be ['DST','RDST','XDST'] """
       ,'SimType'       : """ Type of simulation output, can be ['None','Minimal','Full'] """
       ,'EnableUnpack'  : """ Flag to set up on demand unpacking of DST containers """
       ,'EnablePackingChecks' : """ Flag to turn on the running of various unpacking checks, to test the quality of the data packing """
       ,'PackType'      : """ Type of packing for the DST, can be ['NONE','TES','MDF'] """
       ,'PackSequencer' : """ Sequencer in which to run the packing algorithms """
       ,'AlwaysCreate'  : """ Flags whether to create output packed objects even if input missing """
       ,'Writer'        : """ Name of OutputStream writing the DST """
       ,'OutputName'    : """ Name of the output file, for MDF writing """
       ,'SpilloverPaths': """ Paths to write to XDST if available on input file """
       ,'DataType'      : """ Flag for backward compatibility with old data """
       }

    __used_configurables__ = [
        CaloDstPackConf    , 
        CaloDstUnPackConf  ,
        SimConf            ,
        DigiConf
        ]
    
    KnownSimTypes  = ['None','Minimal','Full']
    KnownDstTypes  = ['NONE','DST','RDST','XDST']
    KnownPackTypes = ['NONE','TES','MDF']

    def _doWrite( self, dType, pType, sType ):
        """
        Define the file content and write it out
        """

        writer = DummyWriter()
        self._defineOutputData( dType, pType, sType, writer )
        
        if pType == 'MDF':
            if dType == 'DST': raise TypeError( "Only RDST are supported with MDF packing" )
            self._doWriteMDF( writer.ItemList )
        else:
            self._doWritePOOL( writer.ItemList, writer.OptItemList )
            

    def _defineOutputData( self, dType, pType, sType, writer ):
        """
        Define content of the output dataset
        """
        
        # Choose whether to write packed or unpacked objects
        if pType == 'NONE':
            if dType == 'RDST': raise TypeError( "RDST should always be in a packed format" )
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
                             , "/Event/Rec/Status"                       + depth 
                             , "/Event/" + recDir + "/Track/Best"        + depth
                             , "/Event/" + recDir + "/Rich/PIDs"         + depth
                             , "/Event/" + recDir + "/Muon/MuonPID"      + depth
                             , "/Event/" + recDir + "/Calo/Electrons"    + depth
                             , "/Event/" + recDir + "/Calo/Photons"      + depth
                             , "/Event/" + recDir + "/Calo/MergedPi0s"   + depth
                             , "/Event/" + recDir + "/Calo/SplitPhotons" + depth
                             , "/Event/" + recDir + "/ProtoP/Charged"    + depth
                             , "/Event/" + recDir + "/ProtoP/Neutrals"   + depth
                             , "/Event/" + recDir + "/Vertex/Primary"    + depth
                             , "/Event/" + recDir + "/Vertex/V0"         + depth ]

        # Copy of HLT results, only on RDST
        if dType == "RDST":
            writer.ItemList += [ "/Event/pRec/RawEvent" + depth ]

        # Additional objects not on RDST
        else:
            writer.ItemList += [ "/Event/" + recDir + "/Track/Muon" + depth ]

            # Additional objects not packable as MDF
            if pType != "MDF":
                writer.ItemList += [ "/Event/DAQ/RawEvent#1" ]

                # Add selection results if DST commes from stripping ETC
                writer.OptItemList += [ "/Event/Phys/Selections#1" ]

                # Add the simulation objects (POOL DST only)
                if sType != "None":
                    
                    eventLocations = ['']
                    if dType == "XDST":
                        eventLocations = SimConf().allEventLocations()

                    # Minimal MC output.
                    SimConf().addHeaders(writer)
                    SimConf().addMCVertices(writer,eventLocations)

                    if sType == "Full":
                        
                        writer.ItemList += [
                            # Links to MCParticles created in Brunel
                            "/Event/Link/Rec/Track/Best#1"
                            ]

                        # Objects propagated from Gauss
                        # Generation information
                        SimConf().addGenInfo(writer)
                        # MCparticles
                        SimConf().addMCParticles(writer,eventLocations)

                        # Objects propagated from Boole
                        # Digi headers
                        DigiConf().addHeaders(writer)
                        # Digitisation summaries
                        DigiConf().addMCDigitSummaries(writer)
                        # Links to MCParticles
                        DigiConf().addMCParticleLinks(writer)

                        if dType == "XDST":
                            # Add the MCHits (from Gauss) and links to them (from Boole)
                            SimConf().addSubDetSimInfo(writer)
                            DigiConf().addMCHitLinks(writer)

    def _doWritePOOL( self, items, optItems ):
        """
        Write a DST (or RDST, XDST) in POOL format
        """
        writer = OutputStream( self.getProp("Writer") )
        ApplicationMgr().OutStream.append( writer )
        writer.Preload = False
        writer.ItemList += items
        writer.OptItemList += optItems
        log.info( "%s.ItemList=%s"%(self.getProp("Writer"),writer.ItemList) )
        log.info( "%s.OptItemList=%s"%(self.getProp("Writer"),writer.OptItemList) )
        

    def _doWriteMDF( self, items ):
        """
        Write an RDST in MDF format
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

        from Configurables import PackTrack, PackCaloHypo, PackProtoParticle, PackRecVertex, PackTwoProngVertex
        from Configurables import DataPacking__Pack_LHCb__RichPIDPacker_
        from Configurables import DataPacking__Pack_LHCb__MuonPIDPacker_

        alwaysCreate = self.getProp("AlwaysCreate")
        packDST.Members = [ PackTrack( name = "PackTracks", AlwaysCreateOutput = alwaysCreate) ]

        richpidpack = DataPacking__Pack_LHCb__RichPIDPacker_( name               = "PackRichPIDs",
                                                              AlwaysCreateOutput = alwaysCreate )
        richpidpack.PackingVersion = 1 # For want of a better place, put here for the moment ...
        muonpidpack = DataPacking__Pack_LHCb__MuonPIDPacker_( name               = "PackMuonPIDs",
                                                              AlwaysCreateOutput = alwaysCreate )
        packDST.Members += [ richpidpack, muonpidpack ]

        caloPackSeq = GaudiSequencer("CaloPacking")
        packDST.Members += [caloPackSeq]
        CaloDstPackConf (
            Enable       = True    ,
            Sequence     = caloPackSeq ,
            AlwaysCreate = alwaysCreate
            )

        packChargedPs = PackProtoParticle( name               = "PackChargedProtos",
                                           AlwaysCreateOutput = alwaysCreate,
                                           InputName          = "/Event/Rec/ProtoP/Charged",
                                           OutputName         = "/Event/pRec/ProtoP/Charged" )
        packNeutralPs = PackProtoParticle( name               = "PackNeutralProtos",
                                           AlwaysCreateOutput = alwaysCreate,
                                           InputName          = "/Event/Rec/ProtoP/Neutrals",
                                           OutputName         = "/Event/pRec/ProtoP/Neutrals" )
        #packChargedPs.OutputLevel = 2
        #packNeutralPs.OutputLevel = 2
        packDST.Members += [
            packChargedPs
            , packNeutralPs
            , PackRecVertex(AlwaysCreateOutput = alwaysCreate)
            , PackTwoProngVertex(AlwaysCreateOutput = alwaysCreate)
            ]
        
        if self.getProp( "DstType" ).upper() == "RDST":
            # Copy the ODIN and HLT results from RawEvent to put them on the RDST
            from Configurables import RawEventSelectiveCopy
            rawEventSelectiveCopy = RawEventSelectiveCopy()
            rawEventSelectiveCopy.RawBanksToCopy = [ "HltDecReports" , "L0DU", "ODIN" ]
            packDST.Members += [ rawEventSelectiveCopy ]
        else:
            packDST.Members += [ PackTrack( name               = "PackMuonTracks",
                                            AlwaysCreateOutput = alwaysCreate,
                                            InputName          = "/Event/Rec/Track/Muon",
                                            OutputName         = "/Event/pRec/Track/Muon" ) ]

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

        from Configurables import ( UnpackTrack, UnpackCaloHypo, UnpackProtoParticle,
                                    UnpackRecVertex, UnpackTwoProngVertex )
        from Configurables import DataPacking__Unpack_LHCb__RichPIDPacker_
        from Configurables import DataPacking__Unpack_LHCb__MuonPIDPacker_
        
        from Configurables import ( CompareTrack, CompareRecVertex, CompareTwoProngVertex,
                                    CompareProtoParticle )
        from Configurables import DataPacking__Check_LHCb__RichPIDPacker_
        from Configurables import DataPacking__Check_LHCb__MuonPIDPacker_

        # Unpack to temporary locations
        tempLoc = "Test"
        unpackTracks       = UnpackTrack("UnpackTracksTest")
        unpackRichPIDs     = DataPacking__Unpack_LHCb__RichPIDPacker_("UnpackRichPIDsTest")
        unpackMuonPIDs     = DataPacking__Unpack_LHCb__MuonPIDPacker_("UnpackMuonPIDsTest")
        unpackChargedPs    = UnpackProtoParticle("UnpackChargedProtosTest")
        unpackNeutralPs    = UnpackProtoParticle("UnpackNeutralProtosTest")
        unpackVertex       = UnpackRecVertex("UnpackVertexTest")
        unpackV0           = UnpackTwoProngVertex("UnpackV0Test")
        unpackTracks.OutputName = unpackTracks.getProp("OutputName")+tempLoc
        unpackVertex.OutputName = unpackVertex.getProp("OutputName")+tempLoc
        unpackV0.OutputName     = unpackV0.getProp("OutputName")+tempLoc
        unpackRichPIDs.OutputName = unpackRichPIDs.getProp("OutputName")+tempLoc     
        unpackMuonPIDs.OutputName = unpackMuonPIDs.getProp("OutputName")+tempLoc
        unpackChargedPs.InputName  = "/Event/pRec/ProtoP/Charged"
        unpackNeutralPs.InputName  = "/Event/pRec/ProtoP/Neutrals"
        unpackChargedPs.OutputName = "/Event/Rec/ProtoP/Charged"+tempLoc
        unpackNeutralPs.OutputName = "/Event/Rec/ProtoP/Neutrals"+tempLoc

        checks.Members += [ unpackTracks, unpackRichPIDs, unpackMuonPIDs,
                            unpackChargedPs, unpackNeutralPs, unpackVertex, unpackV0 ]
        
        # Comparisons
        checkTracks    = CompareTrack("CheckPackedTracks")
        checkVertex    = CompareRecVertex("CheckPackedVertices")
        checkV0        = CompareTwoProngVertex("CheckPackedV0s")
        checkRichPID   = DataPacking__Check_LHCb__RichPIDPacker_("CheckPackedRichPIDs")
        checkMuonPID   = DataPacking__Check_LHCb__MuonPIDPacker_("CheckPackedMuonPIDs")
        checkChargedPs = CompareProtoParticle("CheckChargedProtos")
        checkNeutralPs = CompareProtoParticle("CheckNeutralProtos")
        checkChargedPs.InputName = "/Event/Rec/ProtoP/Charged"
        checkChargedPs.TestName  = unpackChargedPs.OutputName
        checkNeutralPs.InputName = "/Event/Rec/ProtoP/Neutrals"
        checkNeutralPs.TestName  = unpackNeutralPs.OutputName
        
        checks.Members += [ checkTracks, checkRichPID, checkMuonPID,
                            checkChargedPs, checkNeutralPs, checkVertex, checkV0 ]

        if self.getProp( "DstType" ).upper() != "RDST" :

            unpackMuTracks = UnpackTrack("UnpackMuonTracksTest")
            unpackMuTracks.InputName  = "/Event/pRec/Track/Muon"
            unpackMuTracks.OutputName = "/Event/Rec/Track/Muon"+tempLoc

            checkMuTracks = CompareTrack("CheckPackedMuonTracks")
            checkMuTracks.InputName = "/Event/Rec/Track/Muon"
            checkMuTracks.TestName  = unpackMuTracks.OutputName

            checks.Members += [ unpackMuTracks, checkMuTracks ]
            
    def _doUnpack( self ):
        """
        Set up DataOnDemandSvc to unpack a packed (r)DST
        """
        from Configurables import ( UnpackTrack, UnpackCaloHypo, UnpackProtoParticle,
                                    UnpackRecVertex, UnpackTwoProngVertex )

        unpackTracks       = UnpackTrack()
        unpackVertex       = UnpackRecVertex()
        unpackV0           = UnpackTwoProngVertex()

        CaloDstUnPackConf ( Enable = True )

        unpackCharged  = UnpackProtoParticle(name       = "UnpackCharged",
                                             OutputName = "/Event/Rec/ProtoP/Charged",
                                             InputName  = "/Event/pRec/ProtoP/Charged")
        unpackNeutrals = UnpackProtoParticle(name       = "UnpackNeutrals",
                                             OutputName = "/Event/Rec/ProtoP/Neutrals",
                                             InputName  = "/Event/pRec/ProtoP/Neutrals")

        DataOnDemandSvc().AlgMap[ "/Event/Rec/Track/Best" ]        = unpackTracks
        DataOnDemandSvc().AlgMap[ "/Event/Rec/ProtoP/Charged" ]    = unpackCharged
        DataOnDemandSvc().AlgMap[ "/Event/Rec/ProtoP/Neutrals" ]   = unpackNeutrals
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/Primary" ]    = unpackVertex
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/V0" ]         = unpackV0

        from Configurables import DataPacking__Unpack_LHCb__RichPIDPacker_
        from Configurables import DataPacking__Unpack_LHCb__MuonPIDPacker_
        
        unpackrichpid = DataPacking__Unpack_LHCb__RichPIDPacker_("UnpackRichPIDs")
        unpackmuonpid = DataPacking__Unpack_LHCb__MuonPIDPacker_("UnpackMuonPIDs")

        DataOnDemandSvc().AlgMap[ "/Event/Rec/Rich/PIDs"    ] = unpackrichpid
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Muon/MuonPID" ] = unpackmuonpid

        # Muon tracks do not exist on RDST, do not try to unpack them
        if self.getProp( "DstType" ).upper() != "RDST":
            unpackMuons = UnpackTrack( name       = "UnpackMuonTracks",
                                       OutputName = "/Event/Rec/Track/Muon",
                                       InputName  = "/Event/pRec/Track/Muon" )
            DataOnDemandSvc().AlgMap[ "/Event/Rec/Track/Muon" ] = unpackMuons

    def __apply_configuration__(self):

        log.info(self)

        sType = self.getProp( "SimType" ).capitalize()
        if sType not in self.KnownSimTypes:
            raise TypeError( "Unknown SimType '%s'"%sType )
        if sType != "None":
            DigiConf().setProp("EnableUnpack",True)
            SimConf().setProp("EnableUnpack", DigiConf().getProp("EnableUnpack") )

        dType = self.getProp( "DstType" ).upper()
        if dType not in self.KnownDstTypes:
            raise TypeError( "Unknown DstType '%s'"%dType )

        # Propagate SpilloverPaths and DataType to DigiConf and to SimConf via DigiConf
        self.setOtherProps(DigiConf(),["SpilloverPaths","DataType"])
        DigiConf().setOtherProps(SimConf(),["SpilloverPaths","DataType"])

        pType = self.getProp( "PackType" ).upper()
        if pType not in self.KnownPackTypes:
            raise TypeError( "Unknown PackType '%s'"%pType )

        if self.getProp( "EnableUnpack" ) : self._doUnpack()

        if dType != 'NONE':
            if hasattr( self, "PackSequencer" ): self._doPack()
            GaudiKernel.ProcessJobOptions.PrintOn()
            self._doWrite(dType, pType, sType)
            GaudiKernel.ProcessJobOptions.PrintOff()
