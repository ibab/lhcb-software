# $Id: Strippingb2DMuX.py,v 1.4 2010-04-25 04:49:37 lzhang Exp $
# The stripping selection for inclusive b->DMuX.
# The D includes D0 -> K-pi+, D+ -> K-pi+pi+, Ds+ -> K+K-pi+ and 
# Lambda_c+ -> p+K-pi+.
# The stripping has two lines, one called robust cut (RB) and 
# the other called tight cut. The robust 
# cut requires very loose PID DLL(no RICH requirement), and no 
# MIPCHI2/BPVVDCHI2, but absolute MIP&BPVVD for D's. The tight 
# cut bases on MIPCHI2& BPVVDCHI2 for D's and tighter DLL.
# 

"""
Configuration file for 'Stripping inclusive DMuX events'
"""
__author__  = "Liming Zhang"
__date__ = '04/21/2010'
__version__ = "version $Revision: 1.4 $"

from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import  PhysDesktop, CombineParticles, FilterDesktop
from CommonParticles.StdVeryLooseD02HH import StdVeryLooseD02KPi
from CommonParticles.StdVeryLooseDplus import StdVeryLooseDplus2KPiPi, StdVeryLooseDplus2KKPi
from CommonParticles.StdVeryLooseLambdac import StdVeryLooseLambdac2PKPi
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
# the robust cut
RobustKaonCut = "(ABSID=='K+') & (PIDK>-5) & (PT > 350.*MeV) & (MIPDV(PRIMARY)>0.05*mm)"
RobustPionCut = "(ABSID=='pi+') & (PIDK<10) & (PT > 350.*MeV) & (MIPDV(PRIMARY)>0.05*mm)"
RobustProtonCut = "(ABSID=='p+') & (PIDp>-5) & (PT > 350.*MeV) & (MIPDV(PRIMARY)>0.05*mm)"
RobustMuonCut = "(MIPDV(PRIMARY)>0.05*mm) & (TRCHI2DOF < 10.0) & (PT>500 * MeV) & (HASMUON) & (PIDmu>-5)"
RobustDcut = "(BPVVD > 2.5*mm)"

D02KPi_forDMuXRB = FilterDesktop( 'D02KPi_forDMuXRB' )
D02KPi_forDMuXRB.InputLocations = ['StdVeryLooseD02KPi']
D02KPi_forDMuXRB.Code = ( "(INTREE(" + RobustKaonCut + ")) & " +
                          "(INTREE(" + RobustPionCut + ")) & " +
                          RobustDcut + " & " +
                          "(ADMASS('D0')<100.*MeV) & " +
                          "(VFASPF(VCHI2/VDOF)<7.5) & " +
                          "(SUMTREE( PT,  ISBASIC )>1200.*MeV)" )
Dp2KPiPi_forDMuXRB = FilterDesktop( 'Dp2KPiPi_forDMuXRB' )
Dp2KPiPi_forDMuXRB.InputLocations = [ 'StdVeryLooseDplus2KPiPi' ]
Dp2KPiPi_forDMuXRB.Code = ( "(INTREE(" + RobustKaonCut + ")) & " +
                            "(2==NINTREE(" + RobustPionCut + ")) & " +
                            RobustDcut + " & " +
                            "(ADMASS('D+')<100.*MeV) & " +
                            "(VFASPF(VCHI2/VDOF)<4.0) & " +
                            "(SUMTREE( PT,  ISBASIC )>1800.*MeV)" )
Ds2KKPi_forDMuXRB = FilterDesktop( 'Ds2KKPi_forDMuXRB' )
Ds2KKPi_forDMuXRB.InputLocations = [ 'StdVeryLooseDplus2KKPi' ]
Ds2KKPi_forDMuXRB.Code = ( "(2==NINTREE(" + RobustKaonCut + ")) & " +
                            "(INTREE(" + RobustPionCut + ")) & " +
                            RobustDcut + " & " +
                            "(DMASS('D_s+')<100.*MeV) & " +
                           "(DMASS('D_s+')>-220.*MeV) & " +
                            "(VFASPF(VCHI2/VDOF)<4.0) & " +
                            "(SUMTREE( PT,  ISBASIC )>1800.*MeV)" )
Lc2PKPi_forDMuXRB = FilterDesktop( 'Lc2PKPi_forDMuXRB' )
Lc2PKPi_forDMuXRB.InputLocations = [ 'StdVeryLooseLambdac2PKPi' ]
Lc2PKPi_forDMuXRB.Code = ( "(INTREE(" + RobustKaonCut + ")) & " +
                           "(INTREE(" + RobustPionCut + ")) & " +
                           "(INTREE(" + RobustProtonCut + ")) & " +
                           RobustDcut + " & " +
                           "(ADMASS('Lambda_c+')<100.*MeV) & " +
                           "(VFASPF(VCHI2/VDOF)<4.0) & " +
                           "(SUMTREE( PT,  ISBASIC )>1800.*MeV)" )

SeqD_forDMuXRB = GaudiSequencer( 'SeqD_forDMuXRB' )
SeqD_forDMuXRB.ModeOR = 1
SeqD_forDMuXRB.IgnoreFilterPassed = True
SeqD_forDMuXRB.Members += [ D02KPi_forDMuXRB, Dp2KPiPi_forDMuXRB, Ds2KKPi_forDMuXRB, Lc2PKPi_forDMuXRB ]

Selb2DMuXRB = CombineParticles( 'Selb2DMuXRB' )
Selb2DMuXRB.DecayDescriptors = ['[Lambda_b0 -> Lambda_c+ mu-]cc', '[B0 -> D- mu+]cc', '[B- -> D0 mu-]cc']
Selb2DMuXRB.DaughtersCuts = {'mu+': RobustMuonCut }
Selb2DMuXRB.InputLocations = [ 'D02KPi_forDMuXRB', 'Dp2KPiPi_forDMuXRB', 'Ds2KKPi_forDMuXRB', 'Lc2PKPi_forDMuXRB', 'StdVeryLooseMuons' ]
Selb2DMuXRB.CombinationCut = "(AM<6.5*GeV) & (AM>2*GeV)"
Selb2DMuXRB.MotherCut = "(MM<6.0*GeV) & (MM>2.7*GeV) & (VFASPF(VCHI2/VDOF)<10.0) & (BPVDIRA>0.998) & (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > -1.0*mm )"
Selb2DMuXRB.ReFitPVs = True

# the tigher cut
TightKaonCut = "(ABSID=='K+') & (PIDK>1e-10) & (PT > 350.*MeV) & (MIPCHI2DV(PRIMARY)>4.0)"
TightPionCut = "(ABSID=='pi+') & (PIDK<10) & (PT > 350.*MeV) & (MIPCHI2DV(PRIMARY)>4.0)"
TightProtonCut = "(ABSID=='p+') & (PIDp>10) & (PIDp-PIDK>3) & (PT > 350.*MeV) & (MIPCHI2DV(PRIMARY)>4.0)"
TightMuonCut = "(TRCHI2DOF < 10.0) & (PT>500 * MeV) & (HASMUON) & (PIDmu>-5) & (MIPCHI2DV(PRIMARY)>1.0)"
TightDcut = "(BPVVDCHI2 > 49)"

D02KPi_forDMuX = FilterDesktop( 'D02KPi_forDMuX' )
D02KPi_forDMuX.InputLocations = ['StdVeryLooseD02KPi']
D02KPi_forDMuX.Code = ( "(INTREE(" + TightKaonCut + ")) & " +
                          "(INTREE(" + TightPionCut + ")) & " +
                          TightDcut + " & " +
                          "(ADMASS('D0')<100.*MeV) & " +
                          "(VFASPF(VCHI2/VDOF)<7.5) & " +
                          "(SUMTREE( PT,  ISBASIC )>1200.*MeV)" )
Dp2KPiPi_forDMuX = FilterDesktop( 'Dp2KPiPi_forDMuX' )
Dp2KPiPi_forDMuX.InputLocations = [ 'StdVeryLooseDplus2KPiPi' ]
Dp2KPiPi_forDMuX.Code = ( "(INTREE(" + TightKaonCut + ")) & " +
                            "(2==NINTREE(" + TightPionCut + ")) & " +
                            TightDcut + " & " +
                            "(ADMASS('D+')<100.*MeV) & " +
                            "(VFASPF(VCHI2/VDOF)<4.0) & " +
                            "(SUMTREE( PT,  ISBASIC )>1800.*MeV)" )
Ds2KKPi_forDMuX = FilterDesktop( 'Ds2KKPi_forDMuX' )
Ds2KKPi_forDMuX.InputLocations = [ 'StdVeryLooseDplus2KKPi' ]
Ds2KKPi_forDMuX.Code = ( "(2==NINTREE(" + TightKaonCut + ")) & " +
                            "(INTREE(" + TightPionCut + ")) & " +
                            TightDcut + " & " +
                            "(DMASS('D_s+')<100.*MeV) & " +
                           "(DMASS('D_s+')>-220.*MeV) & " +
                            "(VFASPF(VCHI2/VDOF)<4.0) & " +
                            "(SUMTREE( PT,  ISBASIC )>1800.*MeV)" )
Lc2PKPi_forDMuX = FilterDesktop( 'Lc2PKPi_forDMuX' )
Lc2PKPi_forDMuX.InputLocations = [ 'StdVeryLooseLambdac2PKPi' ]
Lc2PKPi_forDMuX.Code = ( "(INTREE(" + TightKaonCut + ")) & " +
                           "(INTREE(" + TightPionCut + ")) & " +
                           "(INTREE(" + TightProtonCut + ")) & " +
                           TightDcut + " & " +
                           "(ADMASS('Lambda_c+')<100.*MeV) & " +
                           "(VFASPF(VCHI2/VDOF)<4.0) & " +
                           "(SUMTREE( PT,  ISBASIC )>1800.*MeV)" )

SeqD_forDMuX = GaudiSequencer( 'SeqD_forDMuX' )
SeqD_forDMuX.ModeOR = 1
SeqD_forDMuX.IgnoreFilterPassed = True
SeqD_forDMuX.Members += [ D02KPi_forDMuX, Dp2KPiPi_forDMuX, Ds2KKPi_forDMuX, Lc2PKPi_forDMuX ]

Selb2DMuX = CombineParticles( 'Selb2DMuX' )
Selb2DMuX.DecayDescriptors = ['[Lambda_b0 -> Lambda_c+ mu-]cc', '[Lambda_b0 -> Lambda_c+ mu+]cc','[B0 -> D- mu+]cc', '[B0 -> D- mu-]cc', '[B- -> D0 mu-]cc', '[B+ -> D0 mu+]cc']
Selb2DMuX.DaughtersCuts = {'mu+': TightMuonCut}
Selb2DMuX.InputLocations = [ 'D02KPi_forDMuX', 'Dp2KPiPi_forDMuX', 'Ds2KKPi_forDMuX', 'Lc2PKPi_forDMuX', 'StdVeryLooseMuons' ]
Selb2DMuX.CombinationCut = "(AM<6.5*GeV) & (AM>2*GeV)"
Selb2DMuX.MotherCut = "(MM<6.0*GeV) & (MM>2.7*GeV) & (VFASPF(VCHI2/VDOF)<10.0) & (BPVDIRA>0.998) & (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > -1.0*mm )"
Selb2DMuX.ReFitPVs = True

line1 = StrippingLine('b2DMuXRB'
                     , prescale = 1.0
                     , algos = [ SeqD_forDMuXRB, Selb2DMuXRB ]
                     )

line = StrippingLine('b2DMuX'
                     , prescale = 1.0
                     , algos = [ SeqD_forDMuX, Selb2DMuX ]
                     )


