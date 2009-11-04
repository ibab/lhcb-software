"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: DstConf.py,v 1.20 2009-11-04 08:20:29 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

__all__ = [
    'DstConf'  ## the configurable, configures DST writing/packing/unpacking   
    ]

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from CaloPackingConf import CaloDstPackConf, CaloDstUnPackConf
from Configurables   import LHCbConfigurableUser
import SimConf, DigiConf

class DummyWriter(LHCbConfigurableUser):
    __slots__ = { "ItemList" : [], "OptItemList" : [] }

class DstConf(LHCbConfigurableUser):
    
    __slots__ = {
         "DstType"        : "NONE"
       , "SimType"        : "None"
       , "EnableUnpack"   : False
       , "PackType"       : "TES"
       , "PackSequencer"  : None
       , "Writer"         : "DstWriter"
       , "OutputName"     : ""
       , "SpilloverPaths" : []
         }

    _propertyDocDct = { 
        'DstType'       : """ Type of dst, can be ['DST','RDST','XDST'] """
       ,'SimType'       : """ Type of simulation output, can be ['None','Minimal','Full'] """
       ,'EnableUnpack'  : """ Flag to set up on demand unpacking of DST containers """
       ,'PackType'      : """ Type of packing for the DST, can be ['NONE','TES','MDF'] """
       ,'PackSequencer' : """ Sequencer in which to run the packing algorithms """
       ,'Writer'        : """ Name of OutputStream writing the DST """
       ,'OutputName'    : """ Name of the output file, for MDF writing """
       ,'SpilloverPaths': """ Paths to write out when spillover is enabled """
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

    def _doWrite( self ):
        """
        Define the file content and write it out
        """
        sType = self.getProp( "SimType" ).capitalize()
        if sType not in self.KnownSimTypes:
            raise TypeError( "Unknown SimType '%s'"%sType )

        dType = self.getProp( "DstType" ).upper()
        if dType not in self.KnownDstTypes:
            raise TypeError( "Unknown DstType '%s'"%dType )
        if dType == 'NONE': return

        pType = self.getProp( "PackType" ).upper()
        if pType not in self.KnownPackTypes:
            raise TypeError( "Unknown PackType '%s'"%pType )

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
            if sType != "NONE":
                DigiConf().EnablePack = False
                SimConf().EnablePack  = False
        else:
            recDir = "pRec"
            if sType != "NONE":
                DigiConf().EnablePack = True
                SimConf().EnablePack  = True
            if not hasattr( self, "PackSequencer" ):
                raise TypeError( "Packing requested but PackSequencer not defined" )

        # Add depth if not MDF
        if pType == 'MDF':
            depth = ""
        else:
            depth = "#1"

        writer.ItemList += [   "/Event/DAQ/ODIN"                         + depth
                             , "/Event/Rec/Header"                       + depth
                             , "/Event/Rec/Status"                       + depth 
                             , "/Event/" + recDir + "/Track/Best"        + depth
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
                if sType != "NONE":
                    
                    # Minimal MC output.
                    SimConf().addHeaders(writer)
                    SimConf().addMCVertices(writer)

                    if sType == "Full":
                        
                        writer.ItemList += [
                            # Links to MCParticles created in Brunel
                            "/Event/Link/Rec/Track/Best#1"
                            ]

                        # Objects propagated from Gauss
                        # Generation information
                        SimConf().addGenInfo(writer)
                        # MCparticles
                        SimConf().addMCParticles(writer)

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
        packDST.Members = [   PackTrack() ]

        CaloDstPackConf (
            Enable   = True    ,
            Sequence = packDST 
            )

        packDST.Members += [
            PackProtoParticle( name       = "PackCharged",
                               InputName  = "/Event/Rec/ProtoP/Charged",
                               OutputName = "/Event/pRec/ProtoP/Charged")
            , PackProtoParticle( name       = "PackNeutrals",
                                 InputName  = "/Event/Rec/ProtoP/Neutrals",
                                 OutputName = "/Event/pRec/ProtoP/Neutrals")
            , PackRecVertex()
            , PackTwoProngVertex()
            ]
        
        if self.getProp( "DstType" ).upper() == "RDST":
            # Copy the HLT results from RawEvent to put them on the RDST
            from Configurables import RawEventSelectiveCopy
            rawEventSelectiveCopy = RawEventSelectiveCopy()
            rawEventSelectiveCopy.RawBanksToCopy = [ "HltDecReports" , "L0DU" ]
            packDST.Members += [ rawEventSelectiveCopy ]
        else:
            packDST.Members += [ PackTrack( name       = "PackMuons",
                                            InputName  = "/Event/Rec/Track/Muon",
                                            OutputName = "/Event/pRec/Track/Muon") ]

        # In MDF case, add a sub sequence for the MDF writing
        if self.getProp( "PackType" ).upper() == "MDF":
            packDST.Members += [ GaudiSequencer("WriteMDFSeq") ]

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

        # Muon tracks do not exist on RDST, do not try to unpack them
        if self.getProp( "DstType" ).upper() != "RDST":
            unpackMuons = UnpackTrack( name       = "UnpackMuons",
                                       OutputName = "/Event/Rec/Track/Muon",
                                       InputName  = "/Event/pRec/Track/Muon")
            DataOnDemandSvc().AlgMap[ "/Event/Rec/Track/Muon" ] = unpackMuons

    def __apply_configuration__(self):

        log.info(self)
        if self.getProp("SimType").capitalize() != "NONE":
            SimConf().setProp("EnableUnpack",True)
            DigiConf().setProp("EnableUnpack",True)
            self.setOtherProps(DigiConf(),["SpilloverPaths"])
            self.setOtherProps(SimConf(), ["SpilloverPaths"])
        if self.getProp( "EnableUnpack" ) : self._doUnpack()
        if hasattr( self, "PackSequencer" ): self._doPack()
        GaudiKernel.ProcessJobOptions.PrintOn()
        self._doWrite()
        GaudiKernel.ProcessJobOptions.PrintOff()
