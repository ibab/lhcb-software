# $Id: StrippingBsDsMuNu.py,v 1.1 2009-12-07 15:49:09 poluekt Exp $

__author__ = 'Kim Vervink'
__date__ = '06/10/2009'
__version__ = '$Revision: 1.1 $'

'''
Bs->Ds Mu  stripping selection using LoKi::Hybrid and python
configurables.
'''
from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from Configurables import CombineParticles
import GaudiKernel.SystemOfUnits as Units

########################################################################

MuonCut     = "((ISLONG) & (PT > 600.*MeV) & (PIDmu-PIDpi > -5.) & (PIDmu-PIDK > -5.) & (PIDmu-PIDp > -5.) & (MIPCHI2DV(PRIMARY) > 1.))"

KaPiCut     = "(PT >300.*MeV) & (P > 1.8*GeV) & (MIPCHI2DV(PRIMARY) > 4.)"

KaonCut     = "( 2 == NINTREE( (ABSID == 321) & (PIDK-PIDpi > -5.) & (PIDK-PIDp > -5.) & (PIDK-PIDmu > -5.) & (" + KaPiCut + ")))"

PionCut     = "( 1 == NINTREE( (ABSID == 211) & (PIDpi-PIDK > -5.) & (" + KaPiCut + ")))"

DsCut       = "((M > 1768.*MeV) & (M < 2168.*MeV) & (MIPCHI2DV(PRIMARY) > 4.)  & (PT > 1500.*MeV) & (BPVVDZ>0.8*mm) & (VFASPF(VCHI2/VDOF)<15) & (BPVDIRA>0.995))"

DsKKPiCut   = DsCut + " & " + PionCut + " & " + KaonCut

BCut        = "((M > 2900.*MeV) & (VFASPF(VCHI2/VDOF)<10) )"

BCombCut    = "((AM > 2900.*MeV))"

Bs2DMuX = CombineParticles("Bs2DMuX")

Bs2DMuX.DecayDescriptors = ["[B_s0 -> D- mu+]cc", "[B_s0 -> D+ mu+]cc"] #includes wrong charges

Bs2DMuX.InputLocations = ["StdLooseDplus2KKPi", "StdLooseMuons"]

Bs2DMuX.DaughtersCuts  = { "mu+"  : MuonCut, "D+" : DsKKPiCut}

Bs2DMuX.CombinationCut = BCombCut

Bs2DMuX.MotherCut      = BCut

Bs2DMuXSeq = GaudiSequencer("Bs2DMuXSeq")
Bs2DMuXSeq.Members += [ Bs2DMuX ]

line = StrippingLine('Bs2DMuXLine'
               , prescale = 1.0
               , algos = [Bs2DMuX]
               )
