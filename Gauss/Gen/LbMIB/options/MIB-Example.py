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
MIBackground.TimeCorrSource.ParticleSourceFile = "$MIBDATAROOT/data/Beam1/7TeV/FullShield/BetatronCleaningTCT/PartSource.root"

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

# Z origin used in input file (Set by source file. Uncomment to override)
#MIBackground.TimeCorrSource.ZParticleOrigin = -1.0*m

# Particle direction (Set by source file. Uncomment to override)
#MIBackground.TimeCorrSource.ZDirection = 1

# Beam proton energy (Set by source file. Uncomment to override)
#MIBackground.TimeCorrSource.BeamEnergy = 7.0*TeV

# Z origin offset at generation (Default: 0.0*m)
MIBackground.TimeCorrSource.ZParticleOffset = 0.0*m

# MIB global time offset (Default: 0.0*ns)
MIBackground.TimeCorrSource.TimeOffset = 0.0*ns

##############################################################################
# Normalization of source:
##############################################################################
# Num protons represented by file (Set by source file. Uncomment to override)
#MIBackground.TimeCorrSource.ProtonsInFile = 1.0e11

# Num protons per bunch simulated
MIBackground.TimeCorrSource.BunchSize = 1.15e11

# Scaling factor for MIB event production probability
MIBackground.TimeCorrSource.ScalingFactor = 1.0

##############################################################################
# Histogramming (Defalt = false)
##############################################################################
MIBackground.TimeCorrSource.GenerationHist = False

