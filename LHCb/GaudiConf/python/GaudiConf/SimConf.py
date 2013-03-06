"""
Configurable for Gauss output
"""
__version__ = "v17r0"
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
        ,"DataType"          : "" # Flag for backward compatibility with old data
        ,"SaveHepMC"         : True # If False, do not save HepMC on output file
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
                    from Configurables import DataPacking__Pack_LHCb__MCVeloHitPacker_   as MCVeloHitPacker
                    packing.Members += [ MCVeloHitPacker("MCVeloHitPacker"+slot) ]
                
                if 'PuVeto' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCPuVetoHitPacker_ as MCPuVetoHitPacker
                    packing.Members += [ MCPuVetoHitPacker("MCPuVetoHitPacker"+slot) ]

                if 'VP' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCVPHitPacker_     as MCVPHitPacker
                    packing.Members += [ MCVPHitPacker("MCVPHitPacker"+slot) ]

                if 'VL' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCVLHitPacker_     as MCVLHitPacker
                    packing.Members += [ MCVLHitPacker("MCVLHitPacker"+slot) ]

                if 'TT' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCTTHitPacker_     as MCTTHitPacker
                    packing.Members += [ MCTTHitPacker("MCTTHitPacker"+slot) ]

                if 'UT' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCUTHitPacker_     as MCUTHitPacker
                    packing.Members += [ MCUTHitPacker("MCUTHitPacker"+slot) ]

                if 'IT' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCITHitPacker_     as MCITHitPacker
                    packing.Members += [ MCITHitPacker("MCITHitPacker"+slot) ]

                if 'OT' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCOTHitPacker_     as MCOTHitPacker
                    packing.Members += [ MCOTHitPacker("MCOTHitPacker"+slot) ]

                if 'FT' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCFTHitPacker_     as MCFTHitPacker
                    packing.Members += [ MCFTHitPacker("MCFTHitPacker"+slot) ]

                if 'Muon' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCMuonHitPacker_   as MCMuonHitPacker
                    packing.Members += [ MCMuonHitPacker("MCMuonHitPacker"+slot) ]

                if 'Prs' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCPrsHitPacker_    as MCPrsHitPacker
                    packing.Members += [ MCPrsHitPacker("MCPrsHitPacker"+slot) ]

                if 'Spd' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCSpdHitPacker_    as MCSpdHitPacker
                    packing.Members += [ MCSpdHitPacker("MCSpdHitPacker"+slot) ]

                if 'Ecal' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCEcalHitPacker_   as MCEcalHitPacker
                    packing.Members += [ MCEcalHitPacker("MCEcalHitPacker"+slot) ]

                if 'Hcal' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCHcalHitPacker_   as MCHcalHitPacker
                    packing.Members += [ MCHcalHitPacker("MCHcalHitPacker"+slot) ]

                if 'Rich' in dets :
                    from Configurables import DataPacking__Pack_LHCb__MCRichHitPacker_   as MCRichHitPacker
                    from Configurables import DataPacking__Pack_LHCb__MCRichOpticalPhotonPacker_ as MCRichOpticalPhotonPacker
                    from Configurables import DataPacking__Pack_LHCb__MCRichSegmentPacker_ as MCRichSegmentPacker
                    from Configurables import DataPacking__Pack_LHCb__MCRichTrackPacker_   as MCRichTrackPacker
                    packing.Members += [ MCRichHitPacker("MCRichHitPacker"+slot) ]
                    packing.Members += [ MCRichOpticalPhotonPacker("MCRichOpPhotPacker"+slot) ]
                    packing.Members += [ MCRichSegmentPacker("MCRichSegmentPacker"+slot) ]
                    packing.Members += [ MCRichTrackPacker("MCRichTrackPacker"+slot) ]

        # print "SimConf.py: _doPacking(): packing.Members =", packing.Members


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
                
        # Define the event locations
        crossingList = self.allEventLocations()

        # Active Detectors
        dets = self.getProp("Detectors")

        ## event locations
        for slot in crossingList:

            self._makeUnpacker( UnpackMCParticle, "UnpackMCParticles", slot, 'Particles' )
            self._makeUnpacker( UnpackMCVertex,   "UnpackMCVertices",  slot, 'Vertices' )

            if 'Velo' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCVeloHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCVeloHitPacker_, "UnpackMCVeloHits", slot, 'Velo/Hits' )
            
            if 'PuVeto' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCPuVetoHitPacker_        
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCPuVetoHitPacker_, "UnpackMCPuVetoHits", slot, 'PuVeto/Hits' )

            if 'VP' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCVPHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCVPHitPacker_, "UnpackMCVPHits", slot, 'VP/Hits' )

            if 'VL' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCVLHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCVLHitPacker_, "UnpackMCVLHits", slot, 'VL/Hits' )

            if 'IT' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCITHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCITHitPacker_, "UnpackMCITHits", slot, 'IT/Hits' )

            if 'OT' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCOTHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCOTHitPacker_, "UnpackMCOTHits", slot, 'OT/Hits' )

            if 'FT' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCFTHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCFTHitPacker_, "UnpackMCFTHits", slot, 'FT/Hits' )

            if 'TT' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCTTHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCTTHitPacker_, "UnpackMCTTHits", slot, 'TT/Hits' )

            if 'UT' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCUTHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCUTHitPacker_, "UnpackMCUTHits", slot, 'UT/Hits' )

            if 'Muon' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCMuonHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCMuonHitPacker_, "UnpackMCMuonHits", slot, 'Muon/Hits' )

            if 'Spd' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCSpdHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCSpdHitPacker_, "UnpackMCSpdHits", slot, 'Spd/Hits' )

            if 'Prs' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCPrsHitPacker_                
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCPrsHitPacker_, "UnpackMCPrsHits", slot, 'Prs/Hits' )

            if 'Ecal' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCEcalHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCEcalHitPacker_, "UnpackMCEcalHits", slot, 'Ecal/Hits' )

            if 'Hcal' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCHcalHitPacker_
                self._makeUnpacker( DataPacking__Unpack_LHCb__MCHcalHitPacker_, "UnpackMCHcalHits", slot, 'Hcal/Hits' )
                
            if 'Rich' in dets :
                from Configurables import DataPacking__Unpack_LHCb__MCRichHitPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCRichOpticalPhotonPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCRichSegmentPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCRichTrackPacker_
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

            if "Simulation" in self.getProp("Phases") or "GenToMCTree" in self.getProp("Phases") :
                list += [ self.tapeLocation( slot, 'MC', 'Header' ) ]

            # main event is mandatory, spillover events optional.
            if slot != '':
                tape.OptItemList += list
            else:
                tape.ItemList    += list
            

    def addGenInfo( self, tape ):

        # Add Generator level information
        if "Generator" in self.getProp("Phases") :

            # Save HepMC and BeamParameters only for main event
            if self.getProp("DataType") not in [ '2009', '2010', 'MC09' ]:
                tape.ItemList += [ '/Event/Gen/BeamParameters#1' ]
                # OptItem so that, when used by applications other than Gauss, they are saved if saved by Gauss
                if self.getProp('SaveHepMC') : tape.OptItemList += [ '/Event/Gen/HepMCEvents#1' ]

            for slot in self.allEventLocations() :
                # main event is mandatory, spillover events optional.
                if slot != '':
                    tape.OptItemList += [ self.tapeLocation( slot , 'Gen' , 'Collisions' ) ]
                else:
                    tape.ItemList    += [ self.tapeLocation( slot , 'Gen' , 'Collisions' ) ]


    def addMCParticles( self, tape, eventLocations ) :
        
        if "Simulation" in self.getProp("Phases") or "GenToMCTree" in self.getProp("Phases") :

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
        
        if "Simulation" in self.getProp("Phases") or "GenToMCTree" in self.getProp("Phases") :

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

                if 'VP' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'VP/Hits' ) ]

                if 'VL' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'VL/Hits' ) ]

                if 'IT' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'IT/Hits' ) ]

                if 'OT' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'OT/Hits' ) ]

                if 'FT' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'FT/Hits' ) ]

                if 'TT' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'TT/Hits' ) ]

                if 'UT' in dets :
                    simList += [ self.tapeLocation( slot, mcRoot, 'UT/Hits' ) ]

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

        # print "addSubDetSimInfo(", tape, "): simList =", simList
                
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

