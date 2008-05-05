#!/usr/bin/env python

############################ mibplotter ######################################
# Simple plotting tool to look at different distributions of the MIB file    #
##############################################################################

# Local
from LbMIB import plotter

##############################################################################
# User options                                                               #
##############################################################################
type = '2D' # 1D or 2D
variables = ['X','Y','W'] # What to plot. 1D: [X,weight]. 2D: [X,Y,weight]
axisrange = {'min':-300,'max':300} # Axis range
update = 10000 # Draw plot for every N entry

##############################################################################
# Particle info as given in MIB file                                         #
##############################################################################
# I: PID of particle in question                                             #
# W: Weight of particle                                                      #
# E: Kinetic energy (GeV)                                                    #
# X: X position of particle at scoring plane (-1m or +19.9m)                 #
# Y: Y position of particle at scoring plane (-1m or +19.9m)                 #
# DX: Cosine of X direction of particle at scoring plane                     #
# DY: Cosine of Y direction of particle at scoring plane                     #
###############################################################################
dct = {'I':0,'W':1,'E':2,'X':3,'Y':4,'DX':5,'DY':6}

# Create plots
# MIB files can be found on Castor
# /castor/cern.ch/lhcb/background/LHC/2006_Collimators and 2008_BeamGas
def main():
  f = open('/afs/cern.ch/lhcb/group/background/vol2/mars/vh_lowb_TCTV.4L8.B1_staged_shield.mu','r')
  plot = plotter.plotter(type,100,axisrange['min'],axisrange['max'])
  plot.setupdate(update)
  for line in f:
    values = []
    entries = map(float,line.split())
    values.append(entries[dct[variables[0]]])
    if len(variables)>1:
      values.append(entries[dct[variables[1]]])
    else:
      values.append(1)
    if len(variables)>2:
      values.append(entries[dct[variables[2]]])
    else:
      values.append(1)
    plot.fill(values)
  plot.save()
  plot.rootsave()

if __name__ == '__main__':main()
