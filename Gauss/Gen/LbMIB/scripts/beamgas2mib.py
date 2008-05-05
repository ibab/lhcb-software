#!/usr/bin/env python

############################ beamgas2mib #####################################
# Tool to import beam gas background files, rescale them with new pressure   #
# distributions and translate them into the format used by LbMIB.            #
##############################################################################

# Local
from LbMIB import gasmap, plotter
import beamgas2mib_opts

# Particle info as given in input file
#
# S: distance from IP where primary interaction occured (cm)
# N: Type of atom involved in intercation
# I: PID of particle in question
# E: Kinetic energy (GeV)
# X: X position of particle at scoring plane (-1m or +19.9m)
# Y: Y position of particle at scoring plane (-1m or +19.9m)
# DX: Cosine of X direction of particle at scoring plane
# DY: Cosine of Y direction of particle at scoring plane
partdict = {'S':0,'N':1,'I':2,'W':3,'E':4,'X':5,'Y':6,'DX':7,'DY':8}

# Codes used for H, C and O atoms in files (i.e. the entry 'N')
atypedict = {1:'H',2:'C',3:'O'}

# List of cross sections used
atomcs = {'H':38.0*1e-31,'C':249.0*1e-31,'O':315.0*1e-31}



# Scale with the new gas map
def scale(loc, type, map, oldmap):
  newdens = map.getclosest(loc)
  olddens = oldmap.getclosest(loc)

  # Get full cs of X in new map
  ncs = atomcs[atypedict[type]]*newdens[atypedict[type]]
  # Get full cs of X in old map
  ocs = atomcs[atypedict[type]]*olddens[atypedict[type]]
  # Get total cs in old map
  otcs = atomcs['H']*olddens['H'] + atomcs['C']*olddens['C'] + atomcs['O']*olddens['O']
  # Weight is change in cs of X over fraction cs of X to total cs for old map
  # multiplied by the old cs of X
  return (ncs/ocs)/(ocs/otcs)*ocs
  

# Check if this particle type should be used
def selparttype(particle):
  if len(beamgas2mib_opts.pids) == 0:
    return True
  else:
    for pid in beamgas2mib_opts.pids:
      if particle == pid: 
        return True
  return False


# Check if primary interaction is in the area of interest
def selfpartorigin(origin):
  return origin >= beamgas2mib_opts.prodzmin and origin <= beamgas2mib_opts.prodzmax
    

def main():
  # Get the gasmap
  map = gasmap.gasmap()
  # Read gasmap files
  for f in beamgas2mib_opts.gasmapfiles:
    map.parse(f)
  # Set the defaults for gasmap
  map.setdefault(beamgas2mib_opts.defh2, beamgas2mib_opts.defch4, beamgas2mib_opts.defco, beamgas2mib_opts.defco2)


  # Get the old map used to generate the particles
  oldmap = gasmap.gasmap()
  oldmap.parse(beamgas2mib_opts.oldgasmapfile)


  if beamgas2mib_opts.plot:
    # plotter to look at generated position
    plotter = plotter.plotter('1D', 1000, -50000, 50000)
    plotter.setupdate(10000)
    # plotter to look at resulting map
    plotter2 = plotter.plotter('2D', 100, -300, 300)
    plotter2.setupdate(100000)


  # Outputfile
  outfile = None
  if beamgas2mib_opts.outfile != '':
    outfile = open(beamgas2mib_opts.outfile, 'w')

  # Weight of all the particles generated
  sumweights = 0

  #Open the particle file and start read
  partfile = open(beamgas2mib_opts.particlefile, 'r')
  for line in partfile:
    values = line.split()

    # Save the particles of interest
    if selparttype(int(values[partdict['I']])) and selfpartorigin(float(values[partdict['S']])):
      weight = beamgas2mib_opts.scale * float(values[partdict['W']]) * scale(float(values[partdict['S']])/100.0, int(values[partdict['N']]), map, oldmap) 
      sumweights = sumweights + weight
      
      #plotting
      if beamgas2mib_opts.plot:
        plotter.fill([float(values[partdict['S']]), weight])
        plotter2.fill([float(values[partdict['X']]), float(values[partdict['Y']]), weight])

      #writing to mibfile (pid weight Ekin x y dx dy)
      if outfile != None:
        line = '%5d'%int(values[partdict['I']]) +' '+ '%13.6E'%weight +' '+ '%13.6E'%float(values[partdict['E']]) +' '+ \
          '%9.3f'%float(values[partdict['X']]) +' '+ '%9.3f'%float(values[partdict['Y']]) +' '+ \
          '%13.6E'%float(values[partdict['DX']]) +' '+ '%13.6E'%float(values[partdict['DY']]) +'\n'
        outfile.write(line)
  
  # save plots
  if beamgas2mib_opts.plot:
    plotter.save()
    plotter2.save()

  print 'The sum of the particle weights is '+str(sumweights)
  partfile.close()
  if outfile != None:
    outfile.close()


if __name__ == '__main__':main()
