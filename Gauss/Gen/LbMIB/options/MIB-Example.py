##############################################################################
#  Options to run LbMIB with Time Correlated source 
##############################################################################
#  @author M.Lieng, G.Corti
#  @date 2009-11-01
##############################################################################
# This options should be added in addition to MIB.py (tells Gauss to use
# MIBackground as generator).
##############################################################################
from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *
from Configurables import MIBackground

MIBackground = MIBackground("MIBackground")
##############################################################################
# Event Information:
##############################################################################
# EventType = GSDCTNXU, G=6 & C=5 halo on collimators
# S = particle types generated (similar as for G=5) 
#     0: all
#     2: mu
#     9: other (i.e. hadrons)
# D = special meaning of source configuration (shield and collimator)
#     TCTV = (TCTV.4L8.B1 or TCTV.4R8.B2)
#     TCTH = (TCTH.L8.B1 or TCTH.R8.B2)
#     0: staged shield - Loss on TCTV & TCTH
#     1: staged shield - Loss on TCTV
#     2: staged shield - Loss on TCTH
#     3: design shield - Loss on TCTV & TCTH
#     4: design shield - Loss on TCTV
#     5: design shield - Loss on TCTH
#     6: no shield - Loss on TCTV & TCTH
#     7: no shield - Loss on TCTH
#     8: no shield - Loss on TCTH
# T=0: both beams, T=1: beam1, T=2: beam2
# N=Num. of bunches traveling (beam1), X=Num. of bunches traveling (beam2)
MIBackground.EventType = 60451000

##############################################################################
# Luminosity to be set in header. It is NOT used by TimeCorrSourceAlg!
##############################################################################
MIBackground.Luminosity = 2.e32/cm2/s

##############################################################################
# Use only one source and configure it accordingly
# (Multiple sources can be used if wished.)
##############################################################################
from Configurables import TimeCorrSource
MIBackground.addTool(TimeCorrSource, name="TimeCorrSource")
MIBackground.MIBSources = ["TimeCorrSource"]

##############################################################################
# Input File Options:
##############################################################################
# Name of source file (Default: "" - MUST be set in options)
MIBackground.TimeCorrSource.ParticleSourceFile = "$MIBDATAROOT/data/TCTV.staged_shield.mu.root"

##############################################################################
# Mode Modifiers:
##############################################################################
# MIB event read mode:
#  -1 : Choose random particles using weight based envelope method.
# >=0 : Pick particles sequentually from file starting at this particle number
#       When end of file is reached, start at beginning.
# (Default: -1)
MIBackground.TimeCorrSource.ReadFileFromOffset = -1

# Number of MIB events in Gauss event:
#  -1 : Use weight to find number of MIB event in Gauss event. Empty events 
#       are registered in counter, but not generated.
# >=0 : Force the generation of this many MIB events in Gauss event.
# (Default: -1)
MIBackground.TimeCorrSource.ForceLossPerEvt = -1

# Size of envelopes used for particle selection:
# Low number: faster, but uses more memory.
# (Default: 1000)
MIBackground.TimeCorrSource.EnvelopeSize = 10

##############################################################################
# Source external information:
##############################################################################
# The following variables are used to choose the point of generation and 
# direction of the MIB particles. In addition particle timing is calculated.

# Z origin used in input file (Default: -1.*m)
MIBackground.TimeCorrSource.ZParticleOrigin = -1.0*m

# Z origin at generation (Default: -1.*m)
MIBackground.TimeCorrSource.ZParticleGen = -1.0*m

# Particle direction
MIBackground.TimeCorrSource.ZDirection = 1

# MIB source time offset
MIBackground.TimeCorrSource.TimeOffset = 0.0*ns

# Beam proton energy
MIBackground.TimeCorrSource.BeamEnergy = 7.0*TeV


##############################################################################
# Normalization of source:
##############################################################################
# Scaling (arbitrary and bunch frequency to find particle/bunch since
#          weights in file are given for 1 sec of running at full current).
# (Default = 1)
# The bunch frequency is that of the nominal machine, i.e. 2808 filled bunches
# with 11.25 kHz bunch revolution frequency. 
# It correspond to what was used to produce the original input file.
# The number of protons per bunch to produce the source was 1.15 x 10^11
# (Default = 31600000)
MIBackground.TimeCorrSource.ScalingFactor = 1.0
MIBackground.TimeCorrSource.BunchFrequency = 31.6*megahertz

# In tha case of input files that represent something else than 1 sec
# it is possible to indicate the amount of time it does represent (in sec).
MIBackground.TimeCorrSource.TimeOfFile = 1.0*s

##############################################################################
# Histogramming (Defalt = false)
##############################################################################
MIBackground.TimeCorrSource.GenerationHist = False

