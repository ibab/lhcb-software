"""
Configurable for Gauss output
"""
__version__ = "$Id: SimConf.py,v 1.6 2009-11-11 16:58:24 cattanem Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

__all__ = [
    'SimConf'  ## the configurable, configures SIM writing/packing/unpacking   
    ]

from Gaudi.Configuration import *
from Configurables import LHCbConfigurableUser
import GaudiKernel.ProcessJobOptions

class SimConf(LHCbConfigurableUser) :

    __slots__ = {
         "Writer"            : "NONE" # Name of the SIM Writer
        ,"Phases"            : ["Generator","Simulation"] # The simulation phases to include in the SIM file
        ,"SpilloverPaths"    : []   # Paths to write out when spillover is enabled
        ,"EnablePack"        : True # Turn on/off packing of the SIM data
        ,"EnableUnpack"      : True # Configure the SIM unpacking via the Data On Demand Service
        ,"DataUnpackingSeq"  : None # If set, the data unpacking will be run explicitly in the given sequence
        ,"Detectors"         : ['Velo','PuVeto','TT','IT','OT','Rich','Muon','Spd','Prs','Ecal','Hcal'] # Active sub-detectors
        ,"PackingSequencers" : { } # The packing sequence to fill for each spillover event
        }

    def allEventLocations(self):
        crossingList = [ '' ]
        spillOverList = self.getProp("SpilloverPaths")
        if '' in spillOverList : spillOverList.remove('')
        crossingList += spillOverList
        return crossingList

    def _doWrite( self ):
        """
        Define the file content and write it out
        """
        if not self.isPropertySet("Writer") :
            log.debug( "No Writer defined for SIM" )
            return
        self._defineOutputData()

    def slot_( self, slot ):
        if slot != '' :  return slot + '/'
        return slot

    def tapeLocation( self, slot , root , item ):
        return '/Event/' + self.slot_(slot) + root + '/' + item + '#1'

    def dodLocation( self, slot , root , item ):
        return '/Event/' + self.slot_(slot) + root + '/' + item

    def mcTESRoot(self):
        mcRoot = 'MC'
        if self.getProp('EnablePack') : mcRoot = 'pSim'
        return mcRoot

    def writer(self):
        tape = OutputStream( self.getProp("Writer"), Preload=False, OutputLevel=3 )
        return tape

    def _doPacking(self):

        from Configurables import ( PackMCParticle, PackMCVertex )
        from Configurables import DataPacking__Pack_LHCb__MCVeloHitPacker_   as MCVeloHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCPuVetoHitPacker_ as MCPuVetoHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCVeloPixHitPacker_   as MCVeloPixHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCTTHitPacker_     as MCTTHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCITHitPacker_     as MCITHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCOTHitPacker_     as MCOTHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCMuonHitPacker_   as MCMuonHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCPrsHitPacker_    as MCPrsHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCSpdHitPacker_    as MCSpdHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCEcalHitPacker_   as MCEcalHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCHcalHitPacker_   as MCHcalHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCRichHitPacker_   as MCRichHitPacker
        from Configurables import DataPacking__Pack_LHCb__MCRichOpticalPhotonPacker_ as MCRichOpticalPhotonPacker
        from Configurables import DataPacking__Pack_LHCb__MCRichSegmentPacker_ as MCRichSegmentPacker
        from Configurables import DataPacking__Pack_LHCb__MCRichTrackPacker_   as MCRichTrackPacker

        # Active Detectors
        dets = self.getProp("Detectors")

        # Packing sequences
        packingSeqs = self.getProp("PackingSequencers")

        # output content
        for slot in self.allEventLocations():

            if slot in packingSeqs :

                packing = packingSeqs[slot]

                if slot != '' : packing.RootInTES = slot

                packMCP = PackMCParticle( "PackMCParticle"+slot )
                packMCV = PackMCVertex( "PackMCVertex"+slot )
                packing.Members += [ packMCP, packMCV ]

                if 'Velo' in dets :
                    packing.Members += [ MCVeloHitPacker("MCVeloHitPacker"+slot) ]
                
                if 'PuVeto' in dets :
                    packing.Members += [ MCPuVetoHitPacker("MCPuVetoHitPacker"+slot) ]

                if 'VeloPix' in dets :
                    packing.Members += [ MCVeloPixHitPacker("MCVeloPixHitPacker"+slot) ]

                if 'TT' in dets :
                    packing.Members += [ MCTTHitPacker("MCTTHitPacker"+slot) ]

                if 'IT' in dets :
                    packing.Members += [ MCITHitPacker("MCITHitPacker"+slot) ]

                if 'OT' in dets :
                    packing.Members += [ MCOTHitPacker("MCOTHitPacker"+slot) ]

                if 'Muon' in dets :
                    packing.Members += [ MCMuonHitPacker("MCMuonHitPacker"+slot) ]

                if 'Prs' in dets :
                    packing.Members += [ MCPrsHitPacker("MCPrsHitPacker"+slot) ]

                if 'Spd' in dets :
                    packing.Members += [ MCSpdHitPacker("MCSpdHitPacker"+slot) ]

                if 'Ecal' in dets :
                    packing.Members += [ MCEcalHitPacker("MCEcalHitPacker"+slot) ]

                if 'Hcal' in dets :
                    packing.Members += [ MCHcalHitPacker("MCHcalHitPacker"+slot) ]

                if 'Rich' in dets :
                    packing.Members += [ MCRichHitPacker("MCRichHitPacker"+slot) ]
                    packing.Members += [ MCRichOpticalPhotonPacker("MCRichOpPhotPacker"+slot) ]
                    packing.Members += [ MCRichSegmentPacker("MCRichSegmentPacker"+slot) ]
                    packing.Members += [ MCRichTrackPacker("MCRichTrackPacker"+slot) ]

    def _makeUnpacker(self, type, name, slot, tesLoc):
        
        unp = type(name+slot)
        if slot != '' : unp.RootInTES = slot
        if self.isPropertySet("DataUnpackingSeq"):
            self.getProp("DataUnpackingSeq").Members += [unp]
        else:
            output = self.dodLocation( slot, 'MC', tesLoc )
            DataOnDemandSvc().AlgMap[ output ] = unp

    def _doUnpacking(self):

        from Configurables import DataOnDemandSvc
        from Configurables import UnpackMCParticle, UnpackMCVertex
        from Configurables import DataPacking__Unpack_LHCb__MCVeloHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCPuVetoHitPacker_        
        from Configurables import DataPacking__Unpack_LHCb__MCVeloPixHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCTTHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCITHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCOTHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCMuonHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCPrsHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCSpdHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCEcalHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCHcalHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCRichHitPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCRichOpticalPhotonPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCRichSegmentPacker_
        from Configurables import DataPacking__Unpack_LHCb__MCRichTrackPacker_
                
        # Define the event locations
        crossingList = self.allEventLocations()

        # Active Detectors
        dets = self.getProp("Detectors")

        ## event locations
        for slot in crossingList:

            self._makeUnpacker( UnpackMCParticle, "UnpackMCParticles", slot, 'Particles' )
            self._makeUnpacker( UnpackMCVertex,   "UnpackMCVertices",  slot, 'Vertices' )

            if 'Velo' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCVeloHitPacker_, "UnpackMCVeloHits", slot, 'Velo/Hits' )
            
            if 'PuVeto' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCPuVetoHitPacker_, "UnpackMCPuVetoHits", slot, 'PuVeto/Hits' )

            if 'VeloPix' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCVeloPixHitPacker_, "UnpackMCVeloPixHits", slot, 'VeloPix/Hits' )

            if 'IT' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCITHitPacker_, "UnpackMCITHits", slot, 'IT/Hits' )

            if 'OT' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCOTHitPacker_, "UnpackMCOTHits", slot, 'OT/Hits' )

            if 'TT' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCTTHitPacker_, "UnpackMCTTHits", slot, 'TT/Hits' )

            if 'Muon' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCMuonHitPacker_, "UnpackMCMuonHits", slot, 'Muon/Hits' )

            if 'Spd' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCSpdHitPacker_, "UnpackMCSpdHits", slot, 'Spd/Hits' )

            if 'Prs' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCPrsHitPacker_, "UnpackMCPrsHits", slot, 'Prs/Hits' )

            if 'Ecal' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCEcalHitPacker_, "UnpackMCEcalHits", slot, 'Ecal/Hits' )

            if 'Hcal' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCHcalHitPacker_, "UnpackMCHcalHits", slot, 'Hcal/Hits' )
                
            if 'Rich' in dets :
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCRichHitPacker_,
                                    "UnpackMCRichHits", slot, 'Rich/Hits' )
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCRichOpticalPhotonPacker_,
                                    "UnpackMCRichPhots", slot, 'Rich/OpticalPhotons' )
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCRichSegmentPacker_,
                                    "UnpackMCRichSegments", slot, 'Rich/Segments' )
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCRichTrackPacker_,
                                    "UnpackMCRichTracks", slot, 'Rich/Tracks' )

    def addHeaders( self, tape ):

        # Event locations
        for slot in self.allEventLocations() :

            list = []

            if "Generator" in self.getProp("Phases") :
                list += [ self.tapeLocation( slot, 'Gen', 'Header' ) ]

            if "Simulation" in self.getProp("Phases") :
                list += [ self.tapeLocation( slot, 'MC', 'Header' ) ]

            # main event is manditory, spillover events optional.
            if slot != '':
                tape.OptItemList += list
            else:
                tape.ItemList    += list
            

    def addGenInfo( self, tape ):

        # Add Generator level information
        if "Generator" in self.getProp("Phases") :

            # Event locations
            for slot in self.allEventLocations() :

                generatorList = [ self.tapeLocation( slot, 'Gen', 'Collisions' ),
                                  self.tapeLocation( slot, 'Gen', 'HepMCEvents' ) ]

                # main event is manditory, spillover events optional.
                if slot != '':
                    tape.OptItemList += generatorList
                else:
                    tape.ItemList    += generatorList

    def addMCParticles( self, tape, eventLocations ) :
        
        if "Simulation" in self.getProp("Phases") :

            # Event locations
            for slot in eventLocations :

                # Annoyingly (MC)Particles and (MC)Vertices change their names when packed ...
                if not self.getProp('EnablePack') :
                    simList = [ self.tapeLocation( slot, 'MC', 'Particles' ) ]
                else:
                    simList = [ self.tapeLocation( slot, 'pSim', 'MCParticles' ) ]

                # main event is manditory, spillover events optional.
                if slot != '':
                    tape.OptItemList += simList
                else:
                    tape.ItemList    += simList

    def addMCVertices( self, tape, eventLocations ) :
        
        if "Simulation" in self.getProp("Phases") :

            # Event locations
            for slot in eventLocations :

                # Annoyingly (MC)Particles and (MC)Vertices change their names when packed ...
                if not self.getProp('EnablePack') :
                    simList = [ self.tapeLocation( slot, 'MC', 'Vertices' ) ]
                else:
                    simList = [ self.tapeLocation( slot, 'pSim', 'MCVertices' ) ]

                # main event is manditory, spillover events optional.
                if slot != '':
                    tape.OptItemList += simList
                else:
                    tape.ItemList    += simList
                    
    def addGeneralSimInfo( self, tape, eventLocations) :

        self.addMCVertices(tape, eventLocations)
        self.addMCParticles(tape, eventLocations)

    def addSubDetSimInfo( self, tape ) :

        if "Simulation" in self.getProp("Phases") :

            # Active Detectors
            dets = self.getProp("Detectors")

            # Event locations
            for slot in self.allEventLocations() :

                mcRoot = self.mcTESRoot()

                simList = [ ]

                if 'Velo' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'Velo/Hits' ) ]

                if 'PuVeto' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'PuVeto/Hits' ) ]

                if 'VeloPix' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'VeloPix/Hits' ) ]

                if 'IT' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'IT/Hits' ) ]

                if 'OT' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'OT/Hits' ) ]

                if 'TT' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'TT/Hits' ) ]

                if 'Muon' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'Muon/Hits' ) ]

                if 'Spd' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'Spd/Hits' ) ]

                if 'Prs' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'Prs/Hits' ) ]

                if 'Ecal' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'Ecal/Hits' ) ]

                if 'Hcal' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'Hcal/Hits' ) ]
                   
                if 'Rich' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'Rich/Hits' ),
                                 self.tapeLocation( slot, mcRoot, 'Rich/OpticalPhotons' ),
                                 self.tapeLocation( slot, mcRoot, 'Rich/Tracks' ), 
                                 self.tapeLocation( slot, mcRoot, 'Rich/Segments' ) ]
                    # Linkers annoyingly put things in slightly different places ...
                    if slot != '' :
                        simList += ['/Event/Link/'+slot+'/MC/Particles2MCRichTracks#1',
                                    '/Event/Link/'+slot+'/MC/Rich/Hits2MCRichOpticalPhotons#1' ]
                    else:
                        simList += ['/Event/Link/MC/Particles2MCRichTracks#1',
                                    '/Event/Link/MC/Rich/Hits2MCRichOpticalPhotons#1' ]

                # main event is manditory, spillover events optional.
                if slot != '':
                    tape.OptItemList += simList
                else:
                    tape.ItemList    += simList
                
    def _defineOutputData( self ):
        """
        Define content of the output dataset
        """

        # Get the Writer instance
        tape = self.writer()

        # Empty lists to start
        tape.ItemList    = [ ]
        tape.OptItemList = [ ]

        # Headers
        self.addHeaders( tape )

        # Add generator level information
        self.addGenInfo( tape )

        # Add general simulation information
        self.addGeneralSimInfo( tape, self.allEventLocations() )

        # Add Sub-detector information
        self.addSubDetSimInfo( tape )

        # Some printout ...
        log.info( "%s.ItemList    = %s"%(self.getProp("Writer"),tape.ItemList) )
        log.info( "%s.OptItemList = %s"%(self.getProp("Writer"),tape.OptItemList) )
    
    def __apply_configuration__(self):
        
        GaudiKernel.ProcessJobOptions.PrintOn()
        
        self._doWrite()

        if self.getProp("EnablePack")   : self._doPacking()
        if self.getProp("EnableUnpack") : self._doUnpacking()
        
        GaudiKernel.ProcessJobOptions.PrintOff()
