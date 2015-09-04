from Gaudi.Configuration import *
from Gauss.Configuration import *

particles = {'Piminus': -211, 'Piplus': 211, 'Kminus': -321, 'Kplus': 321, 'p': 2212, 'pbar': -2212}
Zplane = {'Al': {1: 200, 5: 400, 10: 600}, 'Be': {1: 800, 5: 1000, 10: 1200}, 'Si': {1: 1400, 5: 1600, 10: 1800}}
Zorig = {'Al': {1: 100, 5: 300, 10: 500}, 'Be': {1: 700, 5: 900, 10: 1100}, 'Si': {1: 1300, 5: 1500, 10: 1700}}

def targetGeo():
    from Configurables import GiGaInputStream
    geo = GiGaInputStream('Geo')
    #   geo.StreamItems      = ["/dd/Structure/TargetDet/"+target] #Adds only the target you are currently looking at
    geo.StreamItems      = ["/dd/Structure/TargetDet"]          #Adds all targets at once
       
    from Configurables import SimulationSvc
    SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHesOff.xml" 
   
def addMyTool():
    from Configurables import GiGa, GiGaTrackActionSequence
    giga = GiGa()
    giga.addTool( GiGaTrackActionSequence("TrackSeq") , name = "TrackSeq" )
    giga.TrackSeq.Members.append( "GaussTargetMultiplicity" )
   

def setup_Target_GaussJob(physList,targetThick,targetMat,projEng,projID,nEvts = 10000) :
   
   from Configurables import LHCbApp
   from Configurables import DDDBConf
   from Configurables import CondDB
   from Configurables import Gauss

   target = 'Target_'+str(targetThick)+'mm'+targetMat
   
   Gauss()

   DDDBConf().DbRoot="conddb:/TargetsDet.xml"
   CondDB().Upgrade = True
   LHCbApp().DDDBtag   = "dddb-20140120"
   LHCbApp().CondDBtag = "sim-20131108-vc-md100"
   LHCbApp().Simulation = True
   
   # For testing only
   #DDDBConf(DbRoot = "/afs/cern.ch/user/s/seaso/public/Simulation/
   #upgrade/Gauss_Target/DB/myDDDB-Upgrade-TargetGeom-January2014/TargetsDet.xml")
   
   Gauss.DetectorGeo = { "Detectors": [ ] }
   Gauss.DetectorSim = { "Detectors": [ ] }
   Gauss.DetectorMoni ={ "Detectors": [ ] }
   
   Gauss.DataType = "Upgrade"
   Gauss.PhysicsList = {"Em":'NoCuts', "Hadron": physList, "GeneralPhys":True, "LHCbPhys":True}
   
   # --- activate special targets geometry
   appendPostConfigAction(targetGeo)
   
   # --- Switch off delta rays
   Gauss.DeltaRays = False
   
   # --- activate GaussTargetMultiplicity tool
   appendPostConfigAction(addMyTool)
   
   # --- Configure the tool
   from Configurables import GiGa, GiGaTrackActionSequence, GaussTargetMultiplicity
   giga = GiGa()
   giga.addTool( GiGaTrackActionSequence("TrackSeq") , name = "TrackSeq" )
   giga.TrackSeq.addTool( GaussTargetMultiplicity )
   giga.TrackSeq.GaussTargetMultiplicity.InteractionVolumeName = [ "/dd/Structure/TargetDet/"+target+"#pv" + target.replace('Target','Targ') ]
   giga.TrackSeq.GaussTargetMultiplicity.InteractionVolumeString = [target]
   giga.TrackSeq.GaussTargetMultiplicity.TargetMaterial = [targetMat]
   giga.TrackSeq.GaussTargetMultiplicity.TargetThickness = [targetThick]
   giga.TrackSeq.GaussTargetMultiplicity.PhysicsList = [physList]
   giga.TrackSeq.GaussTargetMultiplicity.ProjectileEnergy = [projEng]
   giga.TrackSeq.GaussTargetMultiplicity.ProjectilePdgID  = [particles[projID]]
   
   #giga.TrackSeq.GaussTargetMultiplicity.OutputLevel = DEBUG
   
   
   
   from Configurables import CondDB, LHCbApp, DDDBConf, CondDBAccessSvc
   from Configurables import Gauss
   
   Gauss().Production = 'PGUN'
    
   #--Generator phase, set random numbers
   GaussGen = GenInit("GaussGen")
   GaussGen.FirstEventNumber = 1
   GaussGen.RunNumber        = 1082
   
   #--Number of events
   LHCbApp().EvtMax = nEvts
   
   Gauss().Production='PGUN'
   
   Gauss().OutputType = 'NONE'
   Gauss().Histograms = 'NONE'
   
   #--- Save ntuple with hadronic cross section information
   ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
   NTupleSvc().Output = ["FILE1 DATAFILE='Multi_{particle}_in{material}.root' TYP='ROOT' OPT='NEW'".format(particle=projID,material=targetMat)] 
   

def setup_Targets_pguns(projID, projEng, targMat, targThick) :

    from Configurables import ParticleGun
    from GaudiKernel.SystemOfUnits import mm, GeV, rad
    
    # Set up ParticleGun
    ParticleGun = ParticleGun("ParticleGun")
    
    # Control of the ParticleGun
    # Event type is set as described in LHCb-2005-034 : G=5! (Def = 0)
    ParticleGun.EventType = 53210205
    
    # Pick the type of particle gun generator
    from Configurables import MaterialEval
    ParticleGun.addTool(MaterialEval, name="MaterialEval")
    ParticleGun.ParticleGunTool = "MaterialEval"
    
    # Set fixed number of particles to produce in an event (default = 10) and
    # their PDG code (default = 2221111)
    from Configurables import FlatNParticles
    ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
    ParticleGun.NumberOfParticlesTool = "FlatNParticles"
    ParticleGun.FlatNParticles.MinNParticles = 1
    ParticleGun.FlatNParticles.MaxNParticles = 1
    ParticleGun.MaterialEval.PdgCode = particles[projID]
    
    # The vertex of each particle gun is produced in a single point of given
    # coordinates. Default is 0.0*mm for all. 
    # distribution within the given limits, when min=max the vertex is in the
    # given point. 
    #ParticleGun.MaterialEval.Xorig = 0*mm
    #ParticleGun.MaterialEval.Yorig = 0*mm
    ParticleGun.MaterialEval.Zorig = Zplane[targMat][targThick]*mm
    
    # The abs(P) of the particle if fixed at the given value (default = 500 GeV)
    ParticleGun.MaterialEval.ModP = projEng * GeV
    
    # The particle can be generated uniformly (randomly or in a grid) in eta-phi
    # rather than x-y (default is false, i.e. x-y)
    ParticleGun.MaterialEval.EtaPhi = True
    
    # The boundary limits of the x-y or eta-phi planes have to given.
    # 
    # The following options are for the x-y plane, to specify the position
    # and size of the "target rectangle". The default plane is at z = 10*m, 
    # with -3.2*m < x < 3.2*m and -2.6*m < y < -2.6*m, i.e. a little more
    # than the LHCb acceptance
    ParticleGun.MaterialEval.ZPlane = 400*mm
    #ParticleGun.MaterialEval.Xmin   = -15*m
    #ParticleGun.MaterialEval.Ymin   = -15*m
    #ParticleGun.MaterialEval.Xmax   =  15*m
    #ParticleGun.MaterialEval.Ymax   =  15*m
    
    # The follwing control the parameters for the generation in eta-phi.
    # The defaults are 2.1 < eta < 4.9 and phi over 2pi, i.e. a little more
    # than scanning the LHCb acceptance
    ParticleGun.MaterialEval.MinEta = 10.
    ParticleGun.MaterialEval.MaxEta = 10.
    ParticleGun.MaterialEval.MinPhi = 0.0*rad
    ParticleGun.MaterialEval.MaxPhi = 0.0*rad 
    
    # A regular grid of given steps can be used instead of a randomly uniform
    # distribution (default = false)
    ParticleGun.MaterialEval.UseGrid = False
   

  
