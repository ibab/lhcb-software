## #####################################################################
# $Id: StrippingRICHLambdaNoPID.py,v 1.2 2009-09-04 16:02:55 poluekt Exp $
#
# A stripping selection for Lambda0 -> p+ pi-
#
# This is a relatively tight selection, except for all mass requirements.
# It is intended for RICH calibration and, consquently, does not use PID.
#
# @author Andrew Powell and Patrick Spradlin
# @date 2009-August-02
#
## #####################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, bindMembers

from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
from CommonParticles.StdNoPIDsProtons import StdNoPIDsProtons

# Create Lambda -> ppi candidates
## ############################################################
RICHLambdaCombine = StrippingMember( CombineParticles
                                     , 'Combine'
                                     , InputLocations = [ "StdNoPIDsPions","StdNoPIDsProtons" ]
                                     , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                     , DaughtersCuts = { "p+"  :  "(PT > 0.1*GeV) & (MIPCHI2DV(PRIMARY) > 9) & (TRCHI2DOF < 3) & (ISLONG)",
                                                         "pi-" :  "(PT > 0.4*GeV) & (MIPCHI2DV(PRIMARY) > 9) & (TRCHI2DOF < 3) & (ISLONG)" }
                                     , CombinationCut = "(ADAMASS('Lambda0')<30*MeV) & (AMAXDOCA('') < 0.2*mm)"
                                     , MotherCut = "(PT > 0.5*GeV) & (VFASPF(VCHI2/VDOF) < 6.0) & (MIPDV(PRIMARY) < 0.5) & (BPVVDCHI2 > 750) & (MIPCHI2DV(PRIMARY) < 200) "
                                     )

# Define the line
# ############################################################
lm_line = StrippingLine('RICHLambdaNoPID'
                        , prescale = 0.05 #For ~100 Hz -TBC
                        , algos = [ RICHLambdaCombine ]
                        , stream = 'Lambda'
                        )
