"""
High level configuration tools for Gauss
"""
__version__ = "$Id: Configuration.py,v 1.8 2009-04-07 18:01:47 gcorti Exp $"
__author__  = "Gloria Corti <Gloria.Corti@cern.ch>"

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from GaudiKernel import SystemOfUnits
from Configurables import ( CondDBCnvSvc, EventClockSvc, FakeEventTime,
                            CondDBEntityResolver )
from Configurables import ( RootHistCnv__PersSvc,
                            HepRndm__Engine_CLHEP__RanluxEngine_ ) 
from Configurables import ( GenInit, Generation, MinimumBias, Inclusive,
                            SignalPlain, SignalRepeatedHadronization,
                            SignalForcedFragmentation, StandAloneDecayTool,
                            Special,
                            PythiaProduction, HijingProduction,
                            CollidingBeams, FixedTarget,
                            BeamSpotSmearVertex, FlatZSmearVertex,
                            EvtGenDecay,
                            FixedLuminosity )
from Configurables import ( SimInit, GiGaGeo, GiGaInputStream, GiGa,
                            GiGaDataStoreAlgorithm,
                            GiGaPhysListModular, GiGaRunActionSequence,
                            TrCutsRunAction, GiGaRunActionCommand,
                            GiGaEventActionSequence, GiGaMagFieldGlobal,
                            GiGaTrackActionSequence, GaussPostTrackAction,
                            GiGaStepActionSequence, SimulationSvc,
                            GiGaFieldMgr, GiGaRunManager, GiGaSetSimAttributes,
                            GiGaPhysConstructorOp, 
                            SpdPrsSensDet, EcalSensDet, HcalSensDet,
                            GaussSensPlaneDet )
from Configurables import ( GenerationToSimulation, GiGaFlushAlgorithm,
                            GiGaCheckEventStatus, SimulationToMCTruth,
                            GiGaGetEventAlg, GiGaGetHitsAlg,
                            GetTrackerHitsAlg, GetCaloHitsAlg, 
                            GetMCRichHitsAlg, GetMCRichOpticalPhotonsAlg,
                            GetMCRichSegmentsAlg, GetMCRichTracksAlg,
                            Rich__MC__MCPartToMCRichTrackAlg,
                            Rich__MC__MCRichHitToMCRichOpPhotAlg)
from Configurables import ( GenMonitorAlg, MuonHitChecker, MCTruthMonitor,
                            VeloGaussMoni, MCHitMonitor, MCCaloMonitor,
                            DumpHepMC )
from Configurables import ( PackMCParticle, PackMCVertex,
                            UnpackMCParticle, UnpackMCVertex,
                            CompareMCParticle, CompareMCVertex )


from Configurables import LHCbApp


##
## Set value of crossing angle for all generation methods 
def setCrossingAngle(angle):

    # All event methods with Pythia as production engine
    # Minimum bias
    Generation().addTool(MinimumBias,name="MinimumBias")
    Generation().MinimumBias.addTool(PythiaProduction,name="PythiaProduction")
    Generation().MinimumBias.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().MinimumBias.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Inclusive events
    Generation().addTool(Inclusive,name="Inclusive")
    Generation().Inclusive.addTool(PythiaProduction,name="PythiaProduction")
    Generation().Inclusive.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().Inclusive.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Signal plane (e.g. J/Psi)
    Generation().addTool(SignalPlain,"SignalPlain")
    Generation().SignalPlain.addTool(PythiaProduction,name="PythiaProduction")
    Generation().SignalPlain.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().SignalPlain.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Signal repeated hadronization (e.g. Bd, Bu, Bs, Lambda_b)
    Generation().addTool(SignalRepeatedHadronization,"SignalRepeatedHadronization")
    Generation().SignalRepeatedHadronization.addTool(PythiaProduction,name="PythiaProduction")
    Generation().SignalRepeatedHadronization.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().SignalRepeatedHadronization.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Signal forced fragmentation (e.g. Bc)
    Generation().addTool(SignalForcedFragmentation,"SignalForcedFragmentation")
    Generation().SignalForcedFragmentation.addTool(PythiaProduction,name="PythiaProduction")
    Generation().SignalForcedFragmentation.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().SignalForcedFragmentation.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Special signal (Bc with BcVegPy, Higgs, HiddenValley...)
    Generation().addTool(Special,"Special")
    Generation().Special.addTool(PythiaProduction,name="PythiaProduction")
    Generation().Special.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().Special.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Only signal events
    Generation().addTool(StandAloneDecayTool,"StandAloneDecayTool")
    Generation().StandAloneDecayTool.addTool(PythiaProduction,name="PythiaProduction")
    Generation().StandAloneDecayTool.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().StandAloneDecayTool.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle


## end setCrossingAngle

##
## Helper functions for spill-over
def setTrackersHitsProperties( alg , det , slot , dd ):
    if slot != '' : 
	slot = slot + '/'
    alg.MCHitsLocation = '/Event/' + slot + 'MC/' + det + '/Hits'
    if det == 'PuVeto':
	det = 'VeloPu'
    alg.CollectionName = det + 'SDet/Hits'
    alg.Detectors = ['/dd/Structure/LHCb/'+dd]
    
def tapeLocation( slot , root , item ):
    if slot != '':
	return '/Event/' + slot + '/' + root + '/' + item + '#1'
    return '/Event/' + root + '/' + item + '#1'

def slot_( slot ):
    if slot != '':
	return slot + '/'
    return slot


##
## Function to define what to write on output
## takes as input list of spill over slots ('' for main event)
def setOutputs(spillOverSlots):

    tape = OutputStream("GaussTape" , Preload = False , OutputLevel = 3 )
    ApplicationMgr().OutStream += [ tape ]

    # output content
    for slot in spillOverSlots:
        tape.ItemList += [ tapeLocation( slot , 'Gen' , 'Header' ) , 
                           tapeLocation( slot , 'MC' , 'Header' ) ]
		       
        generatorList = [ tapeLocation( slot , 'Gen' , 'Collisions' ) ,
                          tapeLocation( slot , 'Gen' , 'HepMCEvents' ) ]
        if slot != '':
            tape.OptItemList += generatorList
        else:
            tape.ItemList += generatorList
    
            simulationList = [ tapeLocation( slot , 'pSim' , 'MCParticles' ) ,
                               tapeLocation( slot , 'pSim' , 'MCVertices' ) ,
                               tapeLocation( slot , 'MC' , 'Velo/Hits' ) ,
                               tapeLocation( slot , 'MC' , 'PuVeto/Hits' ) , 
                               tapeLocation( slot , 'MC' , 'TT/Hits' ) ,
                               tapeLocation( slot , 'MC' , 'IT/Hits' ) ,
                               tapeLocation( slot , 'MC' , 'OT/Hits' ) ,
                               tapeLocation( slot , 'MC' , 'Rich/Hits' ) ,
                               tapeLocation( slot , 'MC' , 'Spd/Hits' ) ,
                               tapeLocation( slot , 'MC' , 'Prs/Hits' ) ,
                               tapeLocation( slot , 'MC' , 'Ecal/Hits' ) ,
                               tapeLocation( slot , 'MC' , 'Hcal/Hits' ) ,
                               tapeLocation( slot , 'MC' , 'Muon/Hits' ) ]
    
            if slot != '':
                tape.OptItemList += simulationList
            else: 
                tape.ItemList += simulationList
                
                tape.OptItemList += [ tapeLocation( slot , 'MC' , 'Rich/OpticalPhotons' ) ,
                                      tapeLocation( slot , 'MC' , 'Rich/Tracks' ) , 
                                      tapeLocation( slot , 'MC' , 'Rich/Segments' ) ,
                                      tapeLocation( slot , 'Link/MC' , 'Particles2MCRichTracks' ) ,
                                      tapeLocation( slot , 'Link/MC' , 'Rich/Hits2MCRichOpticalPhotons' ) ]


    
    tape.RequireAlgs = [ 'Gauss' ]
    tape.Output = "DATAFILE='PFN:Gauss.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"

    FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

    HistogramPersistencySvc().OutputFile = "GaussHistos.root"


## end of output function


    
    
    
