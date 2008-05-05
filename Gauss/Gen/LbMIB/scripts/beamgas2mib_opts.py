#!/usr/bin/env python

##############################################################################
# This is the beam gas tool option file                                      #
##############################################################################

##############################################################################
# Files                                                                      #
##############################################################################
# Gas densities (must progress from -z to +z)
# Files contain: from_Z H2_dens CH4_dens CO_dens CO2_dens H2eq_dens
# start- and end-maps contain best guess values between end of main file and
# default region (DS/Arc)

# 2006 estimates. Scrubbed.
gasmapfiles = ['../data/GasEstimates-ARossi-2006-07-28-scrubbed-startmap.csv',\
               '../data/GasEstimates-ARossi-2006-07-28-scrubbed.csv',\
               '../data/GasEstimates-ARossi-2006-07-28-scrubbed-endmap.csv']

# 2006 estimates. Startup.
#gasmapfiles = ['gasmaps/GasEstimates-ARossi-2006-07-28-startup-startmap.csv',\
#               'gasmaps/GasEstimates-ARossi-2006-07-28-startup.csv',\
#               'gasmaps/GasEstimates-ARossi-2006-07-28-startup-endmap.csv']

# 1996 estimates used to generate original files.
oldgasmapfile = '../data/GasEstimates-1996.csv'

# Particles:
# Particle files can be found on Castor at
# /castor/cern.ch/lhcb/background/LHC/mars
particlefile = ''
#particlefile = '../data/ring.one.beta.10.m'
#particlefile = '../data/ring.two.beta.10.m'
#particlefile = '../data/ring.one.beta.01.m'
#particlefile = '../data/ring.two.beta.01.m'

# Output (If empty: Don't produce file):
outfile = ''


##############################################################################
# Default beam gas distribrution to be used where the gasmap is not valid    #
# (molecules/m3)                                                             #
##############################################################################
# H2:
defh2 = 1.6e14
# CH4:
defch4 = 3.3e12 
# CO:
defco = 2.1e13
# CO2:
defco2 = 5.8e13

##############################################################################
# Production options                                                         #
##############################################################################
# Min and max position of primary interactioni:
prodzmin = -100000.0
prodzmax = 100000.0

# Particle types to include (PIDs) ( [] means all )
#pids = [] # all
pids = [13, -13] # muons
#pids = [2212, -2212, -211, 211, -321, 321] # charged hadrons
#pids = [-2112, 2112] # neutrons
#pids = [2212, -2212, -211, 211, -321, 321, -2112, 2112] # hadrons

##############################################################################
# Scaling                                                                    #
##############################################################################
# Beam intensity (ex: protons/sec)
scale = 1.1e11 * 11.25e3 * 2808.0 # p/bunch * mach_freq * num_bunches

##############################################################################
# Plotting                                                                   #
##############################################################################
# Show plots while processing (True or False)
plot = True



