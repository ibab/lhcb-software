## #####################################################################
# A stripping selection for D*+ -> D0(Kpi) pi+
#
# This is a relatively tight, high purity selection that does not use PID
# information.  It is intended for RICH calibration.
#
# @author Andrew Powell and Patrick Spradlin
# @date 2009-June-24
# 
## #####################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, bindMembers
#from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
#from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons



# Create our own D0 -> K pi candidates.
# We cannot use the common D0 reconstruction because it uses PID information.
## ############################################################
str_daugKinCuts = '(PT>0.25*GeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>6) & (TRCHI2DOF<10)'
d0Combine = StrippingMember( CombineParticles
                           , 'Combine'
                           , InputLocations = [ 'StdNoPIDsPions', 'StdNoPIDsKaons' ]
                           , DecayDescriptor = '[D0 -> K- pi+]cc'
                           , DaughtersCuts = { 'pi+' : str_daugKinCuts
                                             , 'K+'  : str_daugKinCuts }
                           , CombinationCut = "(APT>1.5*GeV) & (ADAMASS('D0')<50*MeV)"
                           , MotherCut = '(VFASPF(VCHI2/VDOF)<10)'
                          )

seqD02KPiNoPID = bindMembers( 'D02KPiNoPID', algos = [ d0Combine ] )



# Create the D*+ -> D0 pi+ candidates
## ############################################################
dstCombine = StrippingMember( CombineParticles
                            , 'Combine'
                            , InputLocations = [ 'StdNoPIDsPions', seqD02KPiNoPID.outputSelection() ]
                            , DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
                            , DaughtersCuts = { 'pi+' : '(MIPCHI2DV(PRIMARY)>1) & (TRCHI2DOF<10)'
                                              , 'D0'  : '(ALL)' }
                            , CombinationCut = "(APT>1.25*GeV) & (ADAMASS('D*(2010)+')<50*MeV)"
                            , MotherCut = "(VFASPF(VCHI2)<15) & (M-MAXTREE('D0'==ABSID,M)<155.5)"
                           )

# Define the line
## ############################################################
line = StrippingLine('DstarNoPID'
                    , prescale = 1.
                    , algos = [ seqD02KPiNoPID, dstCombine ]
                    , stream = 'Dstar'
                    )

