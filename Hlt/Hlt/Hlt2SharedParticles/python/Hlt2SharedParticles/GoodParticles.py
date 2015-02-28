### @file
#
#  Standard good Particles with IP and Pt.
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers, Hlt2Member
from HltTracking.HltPVs import PV3D
from Configurables import FilterDesktop
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions, BiKalmanFittedProtons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichKaons, BiKalmanFittedRichPions, BiKalmanFittedRichProtons
#
__all__ = ( 'GoodKaons','GoodPions','ChargedParticles' )
#################################################################################
# NOTE : GoodKaons and GoodPions are obsolete, will be deleted once lines relying
#        on them are rewritten
##########################################################################
# Prepare the Kaons -- pt cuts
#
Kaons = Hlt2Member( FilterDesktop, "Kaons", Inputs = [ BiKalmanFittedKaons ], Code = "(PT > 300.*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)" )
##########################################################################
# Prepare the Pions -- pt cuts
#
Pions = Hlt2Member( FilterDesktop, "Pions", Inputs = [ BiKalmanFittedPions ], Code = "(PT > 300.*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)" )
##########################################################################
GoodKaons = bindMembers( 'Good', [ PV3D('Hlt2'), BiKalmanFittedKaons, Kaons ] )
GoodPions = bindMembers( 'Good', [ PV3D('Hlt2'), BiKalmanFittedPions, Pions ] )
#################################################################################
# END OF OBSOLETE BLOCK 
#################################################################################
# NEW GOOD PARTICLES BLOK
# DEFINE SOME GENERALLY SENSIBLE THRESHOLDS, EACH FOLLOWS THE PREVIOUS
#################################################################################
'''
ParticlesToBuild = {"NoPIDsKaons"   : {"Inputs" : [BiKalmanFittedKaons],   "ValidPID" : []},
                    "NoPIDsPions"   : {"Inputs" : [BiKalmanFittedPions],   "ValidPID" : []},
                    "NoPIDsProtons" : {"Inputs" : [BiKalmanFittedProtons], "ValidPID" : []}
                    "Kaons"         : {"Inputs" : [BiKalmanFittedRichKaons],   "ValidPID" : ["K"]},
                    "Pions"         : {"Inputs" : [BiKalmanFittedRichPions],   "ValidPID" : ["Pi"]},
                    "Protons"       : {"Inputs" : [BiKalmanFittedRichProtons], "ValidPID" : ["p","pK"]}}

Precedence = {"Strength" : ["Loose","Med","Tight"],
              "CutType"  : ["PT","PID","IPC2"]}

CutStrings = {"PT"  : "(PT>",
              "PID" : { "K"  : "(PIDK>",
                        "Pi" : "(PIDK<",
                        "p"  : "(PIDp>",
                        "pK" : "((PIDp-PIDK)>"},
              "IPC2" : "(MIPCHI2DV(PRIMARY)>"}

SelectionThresholdsDict = { "IPC2" : { "Loose" : "1." , "Med" : "4.", "Tight" : "9."},
                            "PT"   : { "Loose" : "100.*MeV", "Med" : "250.*MeV", "Tight" : "500.*MeV"},
                            "PID"  : { "Loose" : "-2." , "Med" : "2.", "Tight" : "5."}}

ChargedParticles = {}

# Build the dictionary
for particle in ParticlesToBuild :
  for cuttype in Precedence["CutType"] :
    for cutstrength in Precedence["Strength"] :
      ChargedParticles[particle+cutstrength+cuttype] = {}
      ChargedParticles[particle+cutstrength+cuttype]["Member"] = Hlt2Member( FilterDesktop, particle+cutstrength+cuttype,
                                                                             Inputs = ParticlesToBuild[particle]["Inputs"],
                                                                             Code = CutStrings[cuttype]+SelectionThresholdsDict[cuttype][cutstrength]+")" ) 
      ChargedParticles[particle+cutstrength+cuttype]["BM"]     = bindMembers( 'Good', [ PV3D('Hlt2') ] +\
                                                                                      ParticlesToBuild[particle]["Inputs"]+\
                                                                                      ChargedParticles[particle+cutstrength+cuttype]["Member"] ] )
'''
