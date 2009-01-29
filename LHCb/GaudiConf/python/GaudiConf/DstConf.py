"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: DstConf.py,v 1.8 2009-01-29 10:01:08 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration import *

class DstConf(ConfigurableUser):
    __slots__ = {
         "DstType"       : "NONE"
       , "EnableUnpack"  : False
       , "PackType"      : "TES"
       , "PackSequencer" : None
       , "Simulation"    : False
       , "Writer"        : "DstWriter"
       , "OutputName"    : ""
         }

    _propertyDocDct = { 
        'DstType'       : """ Type of dst, can be ['DST','RDST'] """
       ,'EnableUnpack'  : """ Flag to set up on demand unpacking of DST containers """
       ,'PackType'      : """ Type of packing for the DST, can be ['NONE','TES','MDF'] """
       ,'PackSequencer' : """ Sequencer in which to run the packing algorithms """
       ,'Simulation'    : """ Flag to define whether to store simulation objects """
       ,'Writer'        : """ Name of OutputStream writing the DST """
       ,'OutputName'    : """ Name of the output file, for MDF writing """ 
       }

    KnownDstTypes  = ['NONE', 'DST','RDST']
    KnownPackTypes = ['NONE', 'TES','MDF']

    def _doWrite( self ):
        """
        Define the file content and write it out
        """
        dType = self.getProp( "DstType" ).upper()
        if dType not in self.KnownDstTypes:
            raise TypeError( "Unknown DST type '%s'"%dType )
        if dType == 'NONE': return

        pType = self.getProp( "PackType" ).upper()
        if pType not in self.KnownPackTypes:
            raise TypeError( "Unknown packing type '%s'"%pType )

        items    = []
        optItems = []
        self._defineOutputData( dType, pType, items, optItems )
        
        if pType == 'MDF':
            if dType == 'DST': raise TypeError( "Only RDST are supported with MDF packing" )
            self._doWriteMDF( items )
        else:
            self._doWritePOOL( items, optItems )
            

    def _defineOutputData( self, dType, pType, items, optItems ):
        """
        Define content of the output dataset
        """
        
        # Choose whether to write packed or unpacked objects
        if pType == 'NONE':
            recDir = "Rec"
        else:
            recDir = "pRec"
            if not hasattr( self, "PackSequencer" ):
                raise TypeError( "Packing requested but PackSequencer not defined" )

        # Add depth if not MDF
        if pType == 'MDF':
            depth = ""
        else:
            depth = "#1"

        items += [ "/Event/DAQ/ODIN"                         + depth
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

        # Additional objects only on DST
        if dType == "DST":
            items += [ "/Event/" + recDir + "/Track/Muon" + depth ]

            # Additional objects not packable as MDF
            if pType != "MDF":
                items += [ "/Event/DAQ/RawEvent#1" ]

                # Add selection results if DST commes from stripping ETC
                optItems += [ "/Event/Phys/Selections#1" ]

                # Add the simulation objects (POOL DST only)
                if self.getProp( "Simulation" ):
                    items += [
                             # Links to MCParticles created in Brunel
                               "/Event/Link/Rec/Track/Best#1"

                             # Objects propagated from Gauss
                             , "/Event/Gen/Header#1"
                             , "/Event/Gen/Collisions#1"
                             , "/Event/Gen/HepMCEvents#1"
                             , "/Event/MC/Header#1"
                             , "/Event/pSim/MCParticles#1"
                             , "/Event/pSim/MCVertices#1"

                             # Objects propagated from Boole
                             , "/Event/MC/DigiHeader#1"
                             , "/Event/MC/TrackInfo#1"
                             , "/Event/MC/Rich/DigitSummaries#1"
                             , "/Event/MC/Muon/DigitsInfo#1"
                             , "/Event/Link/Raw/Velo/Clusters#1"
                             , "/Event/Link/Raw/TT/Clusters#1"
                             , "/Event/Link/Raw/IT/Clusters#1"
                             , "/Event/Link/Raw/OT/Times#1"
                             , "/Event/Link/Raw/Ecal/Digits#1"
                             , "/Event/Link/Raw/Muon/Digits#1"
                             , "/Event/Link/Trig/L0/FullCalo#1" ]

                    # Objects propagated from Boole, not always available
                    optItems += [ "/Event/Prev/MC/Header#1"
                                , "/Event/PrevPrev/MC/Header#1"
                                , "/Event/Next/MC/Header#1"
                                , "/Event/Link/Raw/Hcal/Digits#1" # From Boole v14r9
                                ]

    def _doWritePOOL( self, items, optItems ):
        """
        Write a DST (or RDST) in POOL format
        """
        writer = OutputStream( self.getProp("Writer") )
        ApplicationMgr().OutStream.append( writer )
        writer.Preload = False
        writer.ItemList += items
        writer.OptItemList += optItems
        

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
        packDST.Members = [   PackTrack()
                              , PackCaloHypo( name       = "PackElectrons",
                                              InputName  = "/Event/Rec/Calo/Electrons",
                                              OutputName = "/Event/pRec/Calo/Electrons")
                              , PackCaloHypo( name       = "PackPhotons",
                                              InputName  = "/Event/Rec/Calo/Photons",
                                              OutputName = "/Event/pRec/Calo/Photons")
                              , PackCaloHypo( name       = "PackMergedPi0s",
                                              InputName  = "/Event/Rec/Calo/MergedPi0s",
                                              OutputName = "/Event/pRec/Calo/MergedPi0s")
                              , PackCaloHypo( name       = "PackSplitPhotons",
                                              InputName  = "/Event/Rec/Calo/SplitPhotons",
                                              OutputName = "/Event/pRec/Calo/SplitPhotons")
                              , PackProtoParticle( name       = "PackCharged",
                                                   InputName  = "/Event/Rec/ProtoP/Charged",
                                                   OutputName = "/Event/pRec/ProtoP/Charged")
                              , PackProtoParticle( name       = "PackNeutrals",
                                                   InputName  = "/Event/Rec/ProtoP/Neutrals",
                                                   OutputName = "/Event/pRec/ProtoP/Neutrals")
                              , PackRecVertex()
                              , PackTwoProngVertex()
                              ]
        if self.getProp( "DstType" ).upper() == "DST":
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
        from Configurables import UnpackTrack, UnpackCaloHypo, UnpackProtoParticle, UnpackRecVertex, UnpackTwoProngVertex

        unpackTracks       = UnpackTrack()
        unpackMuons        = UnpackTrack( name       = "UnpackMuons",
                                          OutputName = "/Event/Rec/Track/Muon",
                                          InputName  = "/Event/pRec/Track/Muon")
        unpackVertex       = UnpackRecVertex()
        unpackV0           = UnpackTwoProngVertex()
        unpackElectrons    = UnpackCaloHypo( name       = "UnpackElectrons",
                                             OutputName = "/Event/Rec/Calo/Electrons",
                                             InputName  = "/Event/pRec/Calo/Electrons")
        unpackPhotons      = UnpackCaloHypo( name       = "UnpackPhotons",
                                             OutputName = "/Event/Rec/Calo/Photons",
                                             InputName  = "/Event/pRec/Calo/Photons")
        unpackMergedPi0s   = UnpackCaloHypo( name       = "UnpackMergedPi0s",
                                             OutputName = "/Event/Rec/Calo/MergedPi0s",
                                             InputName  = "/Event/pRec/Calo/MergedPi0s")
        unpackSplitPhotons = UnpackCaloHypo( name       = "UnpackSplitPhotons",
                                             OutputName = "/Event/Rec/Calo/SplitPhotons",
                                             InputName  = "/Event/pRec/Calo/SplitPhotons")
        unpackCharged  = UnpackProtoParticle(name       = "UnpackCharged",
                                             OutputName = "/Event/Rec/ProtoP/Charged",
                                             InputName  = "/Event/pRec/ProtoP/Charged")
        unpackNeutrals = UnpackProtoParticle(name       = "UnpackNeutrals",
                                             OutputName = "/Event/Rec/ProtoP/Neutrals",
                                             InputName  = "/Event/pRec/ProtoP/Neutrals")

        DataOnDemandSvc().AlgMap[ "/Event/Rec/Track/Best" ]        = unpackTracks
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Track/Muon" ]        = unpackMuons
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Calo/Electrons" ]    = unpackElectrons
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Calo/Photons" ]      = unpackPhotons
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Calo/MergedPi0s" ]   = unpackMergedPi0s
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Calo/SplitPhotons" ] = unpackSplitPhotons
        DataOnDemandSvc().AlgMap[ "/Event/Rec/ProtoP/Charged" ]    = unpackCharged
        DataOnDemandSvc().AlgMap[ "/Event/Rec/ProtoP/Neutrals" ]   = unpackNeutrals
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/Primary" ]    = unpackVertex
        DataOnDemandSvc().AlgMap[ "/Event/Rec/Vertex/V0" ]         = unpackV0

        # If simulation, set up also unpacking of MC Truth
        if self.getProp( "Simulation" ):
            from Configurables import UnpackMCParticle, UnpackMCVertex
            DataOnDemandSvc().AlgMap[ "/Event/MC/Particles" ] = UnpackMCParticle()
            DataOnDemandSvc().AlgMap[ "/Event/MC/Vertices" ]  = UnpackMCVertex()
        

    def __apply_configuration__(self):
        if self.getProp( "EnableUnpack" ) : self._doUnpack()
        if hasattr( self, "PackSequencer" ): self._doPack()
        self._doWrite()
