from Gaudi.Configuration import *
from Gauss.Configuration import *
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
   


def createTemplateOptionFile(path,models,particlesTodo,energies,materialsTodo,thicks) :

    opt_file = open(path+"/options.txt","w")

    opt_file.write('\n\n#Set path where data tuples are stored')
    opt_file.write('\n\ninputPath: ' + path)
    opt_file.write('\n\n#Set path where you want your output')
    opt_file.write('\n\noutputPath: ./')
    opt_file.write('\n\n#Select with plot you want to produce: TOTAL (total cross section vs energy), INEL (inelastic cross section vs energy), MULTI (multiplicity vs energy)')
    opt_file.write('\n\nfinalPlot: TOTAL')
    opt_file.write('\n\n#Set "plotdata" to 1 if you want to plot on your graphs COMPASS data, where available (p, pbar in Al and Be total cross section), in caso of ratios this will plot the PDG ratios ')
    opt_file.write('\n\nplotData: 0')
    opt_file.write('\n\n#Set "Ratios" to 1 if you want the ratio between two particle guns, useful to make ratios particle/antiparticle\n#NB: ATTENTION!! If doRatio = 1 in order for this to work you have to list the pguns in sequence "particle, antiparticle, particle, antiparticle, ecc"\n#NB: Ratio will be done only for first material and thickness')
    opt_file.write('\n\ndoRatios: 0')
    opt_file.write('\n\n#Set "print" to 1 if you want to print histograms with PDG ID od daughters, 0 if not (all histograms are saved in rootfiles anyway)')
    opt_file.write('\n\nprint: 0')

    opt_file.write("\n\n#############################################################      GENERATED OPTIONS      ##########################################")	
    opt_file.write('\n\n#Set the models to use\n#N.B.:No more than 2 models per analysis if you put more the rest will be ignored')
    opt_file.write('\n\nModels: ')
    for model in models:
       opt_file.write( model + " ")

    opt_file.write('\n\n#Set the particle guns you want to use: "Piplus", "Piminus", "Kplus", "Kminus", "p", "pbar" are available')
    opt_file.write('\n\nParticleGuns: ')
    for particle in particlesTodo:
       opt_file.write( particle + " ")

    opt_file.write('\n\n#Set the energies of your particles in GeV: any energy available, write just number without units')
    opt_file.write('\n\nEnergies: ')
    for energy in energies:
       opt_file.write( str(energy) + " ")

    opt_file.write('\n\n#Set the materials in which you want to fire your particles: "Al", "Be", "Cu", "Si", "Pb" are available')
    opt_file.write('\n\nMaterials: ')
    for material in materialsTodo:
       opt_file.write( material + " ")
   
    opt_file.write('\n\n#Set the thicknesses of this material in mm, write just number without units')
    opt_file.write('\n\nThicknesses: ')
    for thick in thicks:
       opt_file.write( str(thick) + " ")

    opt_file.close()

   
