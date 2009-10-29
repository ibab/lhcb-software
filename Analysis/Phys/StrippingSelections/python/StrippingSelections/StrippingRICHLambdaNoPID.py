## #####################################################################
# $Id: StrippingRICHLambdaNoPID.py,v 1.2 2009-10-29 14:52:08 powell Exp $
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
                                     , DaughtersCuts = { "p+"  :  "(PT > 0.4*GeV) & (MIPCHI2DV(PRIMARY) > 9) & (TRCHI2DOF < 3) & (ISLONG)",
                                                         "pi-" :  "(PT > 0.1*GeV) & (MIPCHI2DV(PRIMARY) > 9) & (TRCHI2DOF < 3) & (ISLONG)" }
                                     , CombinationCut = "(ADAMASS('Lambda0')<30*MeV) & (AMAXDOCA('') < 0.2*mm)"
                                     , MotherCut = "(PT > 0.5*GeV) & (VFASPF(VCHI2/VDOF) < 6.0) & (MIPDV(PRIMARY) < 0.5) & (BPVVDCHI2 > 750) & (MIPCHI2DV(PRIMARY) < 200) & (ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 15*MeV)"
                                     )

# Define the line
# ############################################################
line = StrippingLine('RICHLambdaNoPID'
                     , prescale = 1.0
                     , algos = [ RICHLambdaCombine ]
                     )
