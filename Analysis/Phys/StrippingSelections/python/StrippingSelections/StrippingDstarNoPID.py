## #####################################################################
# $Id: StrippingDstarNoPID.py,v 1.1 2009-10-13 13:24:16 poluekt Exp $ 
#
# A stripping selection for D*+ -> D0(Kpi) pi+
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
from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons


# Create our own D0 -> K pi candidates.
# We cannot use the common D0 reconstruction because it uses PID information.
## ############################################################
str_daugKinCuts = '(PT>0.35*GeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF<14)'
d0Combine = StrippingMember( CombineParticles
                             , 'Combine'
                             , InputLocations = [ 'StdNoPIDsPions', 'StdNoPIDsKaons' ]
                             , DecayDescriptor = '[D0 -> K- pi+]cc'
                             , DaughtersCuts = { 'pi+' : str_daugKinCuts
                                                 , 'K+'  : str_daugKinCuts }
                             , CombinationCut = "(APT>1.25*GeV) & (ADAMASS('D0')<50*MeV)"
                             , MotherCut = '(VFASPF(VCHI2/VDOF)<12) & (BPVVDCHI2>12) & (BPVDIRA>0.99975)'
                             )

seqD02KPiNoPID = bindMembers( 'D02KPiNoPID', algos = [ d0Combine ] )



# Create the D*+ -> D0 pi+ candidates
## ############################################################
dstCombine = StrippingMember( CombineParticles
                              , 'Combine'
                              , InputLocations = [ 'StdNoPIDsPions', seqD02KPiNoPID.outputSelection() ]
                              , DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
                              , DaughtersCuts = { 'pi+' : '(MIPCHI2DV(PRIMARY)>1) & (PT>0.11*GeV)'
                                                  , 'D0'  : '(ALL)' }
                              , CombinationCut = "(APT>1.25*GeV) & (ADAMASS('D*(2010)+')<50*MeV)"
                              , MotherCut = "(VFASPF(VCHI2)<16) & (M-MAXTREE('D0'==ABSID,M)<155.5)"
                              )

# Define the line
## ############################################################
line = StrippingLine('DstarNoPID'
                     , prescale = 1. #For ~ 100Hz - TBC
                     , algos = [ seqD02KPiNoPID, dstCombine ]
                     )

