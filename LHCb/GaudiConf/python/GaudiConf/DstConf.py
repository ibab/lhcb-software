"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: DstConf.py,v 1.4 2009-01-13 09:39:26 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration import *

class DstConf(ConfigurableUser):
    __slots__ = {
         "DstType"       : "NONE"
       , "EnableUnpack"  : False
       , "EnablePack"    : True
       , "PackSequencer" : None
       , "Simulation"    : False
       , "Writer"        : "DstWriter"
        }

    _propertyDocDct = { 
        'DstType'       : """ Type of dst, can be ['DST','RDST'] """
       ,'EnableUnpack'  : """ Flag to set up on demand unpacking of DST containers """
       ,'EnablePack'    : """ Flag to define whether to pack the output containers """
       ,'PackSequencer' : """ Sequencer in which to run the packing algorithms """
       ,'Simulation'    : """ Flag to define whether to store simulation objects """
       ,'Writer'        : """ Name of OutputStream writing the DST """
       }

    KnownTypes = ['NONE', 'DST','RDST']

    def _doWrite( self ):
        """
        Define the file content and write it out
        """
        type = self.getProp( "DstType" ).upper()
        if type not in self.KnownTypes:
            raise TypeError( "Unknown DST type '%s'"%type )
        if type == 'NONE': return
    
        writer = OutputStream( self.getProp("Writer") )
        ApplicationMgr().OutStream.append( writer )
        writer.Preload = False

        # Choose whether to write packed or unpacked objects
        if self.getProp( "EnablePack" ):
            recDir = "pRec"
        else:
            recDir = "Rec"

        # Objects written to all DST types
        writer.ItemList += [ "/Event/DAQ/ODIN#1"
                           , "/Event/Rec/Header#1"
                           , "/Event/Rec/Status#1"
                           , "/Event/" + recDir + "/Track/Best#1"
                           , "/Event/" + recDir + "/Calo/Electrons#1"
                           , "/Event/" + recDir + "/Calo/Photons#1"
                           , "/Event/" + recDir + "/Calo/MergedPi0s#1"
                           , "/Event/" + recDir + "/Calo/SplitPhotons#1"
                           , "/Event/" + recDir + "/ProtoP/Charged#1"
                           , "/Event/" + recDir + "/ProtoP/Neutrals#1"
                           , "/Event/" + recDir + "/Vertex/Primary#1"
                           , "/Event/Rec/Vertex/V0#1" ]

        # Additional objects only on DST
        if type == "DST":
            writer.ItemList += [ "/Event/DAQ/RawEvent#1"
                               , "/Event/" + recDir + "/Track/Muon#1" ]
            # Add selection results if DST commes from stripping ETC
            writer.OptItemList += [ "/Event/Phys/Selections#1" ]

            # Add the simulation objects (DST only)
            if self.getProp( "Simulation" ):
                writer.ItemList += [
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

                writer.OptItemList += [ "/Event/Prev/MC/Header#1"
                                      , "/Event/PrevPrev/MC/Header#1"
                                      , "/Event/Next/MC/Header#1"

                    # Links for extended Rich info if linked objects exist on input file
                                      , "/Event/Link/MC/Particles2MCRichTracks#1"
                                      , "/Event/Link/MC/Rich/Hits2MCRichOpticalPhotons#1"

                    # Objects propagated from Boole, not always available
                                      , "/Event/Link/Raw/Hcal/Digits#1" # From Boole v14r9
                                      ]

    def _doPack( self ):
        """
        Set up the sequence to create the packed containers
        """
        packDST = self.getProp("PackSequencer")
        from Configurables import PackTrack, PackCaloHypo, PackProtoParticle, PackRecVertex
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
                              , PackRecVertex() ]
        if self.getProp( "DstType" ).upper() == "DST":
            packDST.Members += [ PackTrack( name       = "PackMuons",
                                            InputName  = "/Event/Rec/Track/Muon",
                                            OutputName = "/Event/pRec/Track/Muon") ]

    def _doUnpack( self ):
        """
        Set up DataOnDemandSvc to unpack a packed (r)DST
        """
        from Configurables import UnpackTrack, UnpackCaloHypo, UnpackProtoParticle, UnpackRecVertex

        unpackTracks       = UnpackTrack()
        unpackMuons        = UnpackTrack( name       = "UnpackMuons",
                                          OutputName = "/Event/Rec/Track/Muon",
                                          InputName  = "/Event/pRec/Track/Muon")
        unpackVertex       = UnpackRecVertex()
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

        # If simulation, set up also unpacking of MC Truth
        if self.getProp( "Simulation" ):
            from Configurables import UnpackMCParticle, UnpackMCVertex
            DataOnDemandSvc().AlgMap[ "/Event/MC/Particles" ] = UnpackMCParticle()
            DataOnDemandSvc().AlgMap[ "/Event/MC/Vertices" ]  = UnpackMCVertex()
        

    def __apply_configuration__(self):
        if self.getProp( "EnableUnpack" ) : self._doUnpack()
        if hasattr( self, "PackSequencer" ) : self._doPack()
        self._doWrite()
