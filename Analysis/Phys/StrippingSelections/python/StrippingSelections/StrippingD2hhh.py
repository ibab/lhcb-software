# Comments on prescaling and relative importance of lines:
#  (updated by Alberto and Hamish, 21/05/10)
# There are a lot of stripping lines in this file:
#   a) Some are narrow windows where we want to look for interesting physics;
#   b) Some are wider sideband windows where we want to study the backgrounds
#      underneath the interesting physics from (a);
#   c) Some are for control modes that we want to use as crosschecks to make
#      sure we understand our physics channels from (a);
#   d) Some are sidebands to the control modes from (c);
#   e) Some are for other crosschecks, e.g. testing PID efficiency.
#   f) There is also an inclusive stripping line that takes almost everything.
# 
# We've tried to choose sensible default prescales to keep the event retention rate
# reasonable -- but what counts as "reasonable" will probably change over time
# as the luminosity ramps up. So it may be that some of these prescales need to be
# tightened later. If you need to do that, please first contact at least:
#
#   Mat Charles <m.charles1@physics.ox.ac.uk>
#   Alberto Correa dos Reis <alberto@cbpf.br>
#   Hamish Gordon <h.gordon1@physics.ox.ac.uk>
#
# As a rough guide, here is our thinking on what is most important (from most
# to least):
#
#  * These are critical signal windows and should never be prescaled:
#
#      * lineD2PPP_A_NoPID_Sig
#      * lineD2PPP_B_NoPID_Sig
#      * lineD2KPP_A_NoPID_Sig
#      * lineD2KKP_B_LoosePID_Sig
#      * lineDs2KPP_B_LoosePID_Sig
#      * lineD2KKK_B_LoosePID_Sig
#
#  * This is an inclusive stripping which takes everything in a wide
#    mass window, useful for several different physics studies and
#    for control modes. It is already heavily prescaled so the rate is
#    modest; it shouldn't be necessary to prescale further.
#      * lineD2HHH
#
#  * These are high-statistics control modes for our signals ("standard
#    candles"). They are useful and are not prescaled by default, but if
#    the rate needs to be brought they could be (but not to zero!). Note
#    that the first three lines are dominated by the same signal events and so
#    are quite correlated -- this means that prescaling just one will have
#    little effect on the overall rate. It would be better to apply a
#    common prescale (i.e. run all four lines with probability x and run
#    no lines with probability 1-x, but nothing inbetween) but I don't
#    know how to implement that.
#      * lineD2KPP_B_NoPID_Sig
#      * lineD2KPP_A_LoosePID_Sig
#      * lineD2KPP_B_LoosePID_Sig
#      * lineD2KKP_A_LoosePID_Sig
#      * lineD2KKK_A_LoosePID_Sig
#      * lineD2KPP_DCS_B_LoosePID_Sig
#      * lineD2KPP_DCS_A_LoosePID_Sig
#      * lineDs2KPP_A_LoosePID_Sig
#  * These are the sidebands to our signals. They can be prescaled (and
#    are by default) but we need enough events here to study the backgrounds
#    and test the MC behaviour.
#      * lineD2PPP_A_NoPID_Bkg
#      * lineD2PPP_B_NoPID_Bkg
#      * lineD2KPP_A_NoPID_Bkg
#      * lineD2KKP_B_LoosePID_Bkg
#      * lineDs2KPP_B_LoosePID_Bkg
#      * lineD2KKK_B_LoosePID_Bkg
#  * These are the sidebands to our control channels. They are less critical,
#    but we need enough events to fit the mass spectrum and test out sideband
#    subtraction methods on the control channels. They are prescaled by default.
#     
#      * lineD2KPP_B_NoPID_Bkg
#      * lineD2KPP_A_LoosePID_Bkg
#      * lineD2KPP_B_LoosePID_Bkg
#      * lineD2KKK_A_LoosePID_Bkg
#      * lineD2KKP_A_LoosePID_Bkg
#      * lineD2KPP_DCS_B_LoosePID_Bkg
#      * lineD2KPP_DCS_A_LoosePID_Bkg
#      * lineDs2KPP_A_LoosePID_Bkg
#  * These are calibration/test channels to study PID performance. It would be
#    nice to have some events here for crosschecks, but they are not vital and
#    can be prescaled safely (even to zero if bandwidth is tight). The A lines are #    already switched off (they're not included in StreamCharm.py)
#      * lineD2KKP_B_NoPID_Sig
#      * lineD2KKP_B_NoPID_Bkg
#      * lineD2KKP_A_NoPID_Sig
#      * lineD2KKP_A_NoPID_Bkg
#
#  * Update  26/04/10 (Hamish Gordon):
# 8 new stripping lines added 26/04/10 to account for Ds2KPP (SCS) and D2KPP (DCS)
# The lines use StdLooseKaons.
# The signal mass windows are copied from the CF KPP mode so may not be
# optimal for the retention rate (they are definitely wide enough
# to accept all or almost all of the signal, but may be so wide that we accept
# events unnecessarily).
# 
#  * Update  21/05/10 (Hamish and Alberto):
# Cuts for B lines updated: DIRA cut loosened from 0.99999, FD chi2 cut loosened
# from 50, Dplus_IPchi2 cut removed. Prescales changed to 1 for all important
# background lines
# Cuts for A lines updated: see Alberto's presentation to charm WG of 20/05/09
# (Indico 77085). Changes are marked "#mod in 9/5"
# There is a significant change to A mass windows - the SIGNAl windows run
# from 1800 to 1920 (D->K-pi+pi+) or 1800 to 2040 (all other lines). This is
# almost equivalent to setting the background prescales to 1. The prescales on the
# 3H line and background lines (which now overlap heavily with the signal) are
# retained, and these lines are much less important now, so could be switched off
# if the rate becomes too high.

#The priority A lines are the PPP signal line, the KPP_NOPID signal line and the HHH line.

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, bindMembers

from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons

# Set up cuts.

useCutsA = bool(True)
useCutsB = bool(True)

# 1) Pointing of D+ (applied as MOTHERCUT)
str_cutOffline1A_asMothCut = '((BPVDIRA > 0.9999) & (BPVTRGPOINTINGWPT < 0.10))'# mod. in 9/5
str_cutOffline1HHH_asMothCut = '((BPVDIRA > 0.999) & (BPVTRGPOINTINGWPT < 0.15))'# mod. in 9/5
str_cutOffline1B_asMothCut = '(BPVDIRA > 0.9999)'

# 2) IP of D+ to PV (applied as MOTHERCUT)
str_cutOffline2A_asMothCut = '(MIPDV(PRIMARY) < 0.08*mm)'
str_cutOffline2HHH_asMothCut = '(MIPDV(PRIMARY) < 0.08*mm)'


# 3) IP of D+ daughters to PV.
# For selection 'B' this has to be a COMBINATIONCUT.
# For selection 'A' this is easiest done as a DAUGHTERSCUT but can
#   also be done as a COMBINATIONCUT. So we implement it both ways
#   (so that 'A' and 'B' cuts can be OR'd).
str_cutOffline3A_asCombCut = '(MIPCHI2DV(PRIMARY) > 4)'# mod. in 9/5
str_cutOffline3A_asDaugCut = '(MIPCHI2DV(PRIMARY) > 4)'# mod. in 9/5
str_cutOffline3HHH_asDaugCut = '(MIPCHI2DV(PRIMARY) > 2)'# mod. in 9/5
str_cutOffline3B_asCombCut = '(AHASCHILD((MIPCHI2DV(PRIMARY)) > 30.0))'

# 4) Flight distance of D+ from PV (applied as MOTHERCUT)
str_cutOffline4A_asMothCut = '((BPVVDCHI2 > 150.0) & (VFASPF(VMINVDDV(PRIMARY)) > 3.0*mm))'# mod. in 9/5
str_cutOffline4HHH_asMothCut = '((BPVVDCHI2 > 100.0) & (VFASPF(VMINVDDV(PRIMARY)) > 2.0*mm))'# mod. in 9/5
str_cutOffline4B_asMothCut = '(BPVVDCHI2 > 25.0)'

# 5) Vertex quality of D+
# For 'A' we apply both a chi^2 cut (MOTHERCUT) and a daughter pairwise DOCA cut (COMBINATIONCUT).
# For 'B' we apply just the chi^2 cut (MOTHERCUT).
str_cutOffline5AMoth = '(VFASPF(VCHI2)<12.0)' # mod. in 9/5
str_cutOffline5Moth = '(VFASPF(VCHI2)<8.0)'
str_cutOffline5HHHMoth = '(VFASPF(VCHI2)<15.0)'
str_cutOffline5AComb = "(AMAXDOCA('') < 0.20*mm)" # mod. in 9/5
str_cutOffline5Comb = "(AMAXDOCA('') < 0.12*mm)"
str_cutOffline5HHHComb = "(AMAXDOCA('') < 0.20*mm)" # mod. in 9/5

# 6) PT cuts
# There is a daughter cut (applied to every daughter).
# There are also cuts that apply to the combination or to the mother.
# For 'A' individually: use A daughter & A combination cut
# For 'B' individually: use B daughter & B mother cut & B combination cut
str_cutOffline6DaugA      = '(PT > 200*MeV)' # mod. in 9/5
str_cutOffline6DaugHHH      = '(PT > 200*MeV)'
str_cutOffline6DaugB      = '((PT > 200*MeV) & (P > 3200*MeV) & (P < 100*GeV))'
str_cutOffline6A_asCombCut = '((AHASCHILD((PT > 200*MeV))) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1400*MeV))' # mod. in 9/5
str_cutOffline6HHH_asCombCut = '((ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > 1200*MeV))' # mod. in 9/5
str_cutOffline6A_asMothCut = '((NINGENERATION((PT>200*MeV),1) >= 1) & (CHILD(PT,1)+CHILD(PT,2)+CHILD(PT,3) > 1400*MeV))' # mod. in 9/5
str_cutOffline6BComb = '((APT > 1500*MeV) & (ANUM(PT > 600.0*MeV) >= 2))' # PT cut is looser than post-fit (mother) cut to allow for modification by fit
str_cutOffline6BMoth = '(PT >1800*MeV)'

# 7) Track quality of D+ daughters
# For 'A' individually: use COMBINATIONCUT.
# For 'B' individually: use DAUGHTERSCUT.
str_cutOffline7A_asCombCut = '(ACHILD(TRPCHI2,1)*ACHILD(TRPCHI2,2)*ACHILD(TRPCHI2,3) > 1.0E-15)' # mod. in 9/5
str_cutOffline7HHH_asCombCut = '(ACHILD(TRPCHI2,1)*ACHILD(TRPCHI2,2)*ACHILD(TRPCHI2,3) > 1.0E-15)' # mod. in 9/5
str_cutOffline7B_asCombCut = '(AMINCHILD(TRPCHI2) > 0.0001)'
str_cutOffline7B_asDaugCut = '(TRPCHI2 > 0.0001)'

# 8) PID and track lists
str_OfflinePionListNoPID = 'StdNoPIDsPions'
str_OfflineKaonListNoPID = 'StdNoPIDsKaons'
str_OfflineKaonListLoosePID = 'StdLooseKaons'

# Mass cuts (big envelope; we may go back and chop out bits later)
str_cutOfflineMassPreFit = '((AM > 1580*MeV) & (AM < 2260*MeV))'
str_cutOfflineMassPostFit = '((M > 1780*MeV) & (M < 2060*MeV))'
# Other mass windows edited by Alberto:
str_cutOfflineMassPPP = '((M > 1800*MeV) & (M < 2040*MeV))'
str_cutOfflineMassKPP = '((M > 1820*MeV) & (M < 1920*MeV))'
str_cutOfflineMassKKP = '((M > 1800*MeV) & (M < 2040*MeV))'
str_cutOfflineMassDsKPP = '((M > 1920*MeV) & (M < 2040*MeV))'
str_cutOfflineMassHHH = '((M > 1100*MeV) & (M < 2100*MeV))'
str_cutOfflineMassHHHPreFit = '((AM > 900*MeV) & (AM < 2300*MeV))'

# Combined cuts for A / B:
# A: 1=Moth, 2=Moth, 3=Daug, 4=Moth, 5=Comb+Moth, 6=Comb+Daug,      7=Comb
# B: 1=Moth, 2=Moth, 3=Comb, 4=Moth, 5=Moth,      6=Comb+Moth+Daug, 7=Daug
str_cutOfflineDaugA      = '(' + str_cutOffline3A_asDaugCut + '&' + str_cutOffline6DaugA + ')'
str_cutOfflineDaugHHH    = '(' + str_cutOffline3HHH_asDaugCut + '&' + str_cutOffline6DaugHHH + ')'
str_cutOfflineDaugB      = '(' + str_cutOffline6DaugB + '&' + str_cutOffline7B_asDaugCut + ')'
str_cutOfflineMothB      = '(' + str_cutOffline1B_asMothCut + '&'  + str_cutOffline4B_asMothCut + '&' + str_cutOffline5Moth + '&' + str_cutOffline6BMoth + '&' + str_cutOfflineMassPostFit + ')'#'&' + str_cutOffline2B_asMothCut + '&'
str_cutOfflineCombA      = '(' + str_cutOffline6A_asCombCut + '&' + str_cutOffline7A_asCombCut + '&' + str_cutOfflineMassPreFit + '&' + str_cutOffline5AComb + ')'
str_cutOfflineCombB      = '(' + str_cutOfflineMassPreFit + '&' + str_cutOffline6BComb + '&' + str_cutOffline3B_asCombCut + ')'

# Make per-mode versions with Alberto's mass windows for the 'A' lines:
str_cutOfflineMothHHH    = '(' + str_cutOffline1HHH_asMothCut + '&' + str_cutOffline2HHH_asMothCut + '&' + str_cutOffline4HHH_asMothCut + '&' + str_cutOffline5HHHMoth + '&' + str_cutOfflineMassHHH + ')'
str_cutOfflineMothPPP_A  = '(' + str_cutOffline1A_asMothCut + '&' + str_cutOffline2A_asMothCut + '&' + str_cutOffline4A_asMothCut + '&' + str_cutOffline5AMoth + '&' + str_cutOfflineMassPPP + ')' 
str_cutOfflineMothKPP_A  = '(' + str_cutOffline1A_asMothCut + '&' + str_cutOffline2A_asMothCut + '&' + str_cutOffline4A_asMothCut + '&' + str_cutOffline5AMoth + '&' + str_cutOfflineMassKPP + ')' 
str_cutOfflineMothKKP_A  = '(' + str_cutOffline1A_asMothCut + '&' + str_cutOffline2A_asMothCut + '&' + str_cutOffline4A_asMothCut + '&' + str_cutOffline5AMoth + '&' + str_cutOfflineMassKKP + ')' 
str_cutOfflineMothDsKPP_A  = '(' + str_cutOffline1A_asMothCut + '&' + str_cutOffline2A_asMothCut + '&' + str_cutOffline4A_asMothCut + '&' + str_cutOffline5Moth + '&' + str_cutOfflineMassPPP + ')'
str_cutOfflineMothKKK_A  = '(' + str_cutOffline1A_asMothCut + '&' + str_cutOffline2A_asMothCut + '&' + str_cutOffline4A_asMothCut + '&' + str_cutOffline5Moth + '&' + str_cutOfflineMassPostFit + ')'
# Special for inclusive:
str_cutOfflineCombHHH    = '(' + str_cutOffline6HHH_asCombCut + '&' + str_cutOffline7HHH_asCombCut + '&' + str_cutOfflineMassHHHPreFit + '&' + str_cutOffline5HHHComb + ')'


# Create D+ -> hhh candidates
# ---------------------------

# 'A' selection lines (no PID)

combineOfflineD2PPP_A_NoPID = StrippingMember( CombineParticles, 'Combine'
                                               , InputLocations = [ str_OfflinePionListNoPID ]
                                               , DecayDescriptor = '[D+ -> pi- pi+ pi+]cc'
                                               , DaughtersCuts = { 'pi+' : str_cutOfflineDaugA }
                                               , CombinationCut = str_cutOfflineCombA
                                               , MotherCut = str_cutOfflineMothPPP_A
                                               )
combineOfflineD2KPP_A_NoPID = StrippingMember( CombineParticles
                                               , 'Combine'
                                               , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListNoPID ]
                                               , DecayDescriptor = '[D+ -> K- pi+ pi+]cc'
                                               , DaughtersCuts = { 'pi+' : str_cutOfflineDaugA, 'K+' : str_cutOfflineDaugA }
                                               , CombinationCut = str_cutOfflineCombA
                                               , MotherCut = str_cutOfflineMothKPP_A
                                               )

combineOfflineD2KKP_A_NoPID = StrippingMember( CombineParticles
                                               , 'Combine'
                                               , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListNoPID ]
                                               , DecayDescriptor = '[D+ -> K- K+ pi+]cc'
                                               , DaughtersCuts = { 'pi+' : str_cutOfflineDaugA, 'K+' : str_cutOfflineDaugA }
                                               , CombinationCut = str_cutOfflineCombA
                                               , MotherCut = str_cutOfflineMothKKP_A
                                               )
combineOfflineD2KKK_A_NoPID = StrippingMember( CombineParticles
                                               , 'Combine'
                                               , InputLocations = [ str_OfflineKaonListNoPID ]
                                               , DecayDescriptor = '[D+ -> K- K+ K+]cc'
                                               , DaughtersCuts = { 'K+' : str_cutOfflineDaugA }
                                               , CombinationCut = str_cutOfflineCombA
                                               , MotherCut = str_cutOfflineMothKKK_A
                                               )

# 'A' selection lines (loose PID)

combineOfflineD2PPP_A_LoosePID = StrippingMember( CombineParticles, 'Combine'
                                               , InputLocations = [ str_OfflinePionListNoPID ]
                                               , DecayDescriptor = '[D+ -> pi- pi+ pi+]cc'
                                               , DaughtersCuts = { 'pi+' : str_cutOfflineDaugA }
                                               , CombinationCut = str_cutOfflineCombA
                                               , MotherCut = str_cutOfflineMothPPP_A
                                               )
combineOfflineD2KPP_A_LoosePID = StrippingMember( CombineParticles
                                               , 'Combine'
                                               , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListLoosePID ]
                                               , DecayDescriptor = '[D+ -> K- pi+ pi+]cc'
                                               , DaughtersCuts = { 'pi+' : str_cutOfflineDaugA, 'K+' : str_cutOfflineDaugA }
                                               , CombinationCut = str_cutOfflineCombA
                                               , MotherCut = str_cutOfflineMothKPP_A
                                               )

combineOfflineDs2KPP_A_LoosePID = StrippingMember( CombineParticles
                                               , 'Combine'
                                               , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListLoosePID ]
                                               , DecayDescriptor = '[D_s+ -> K+ pi- pi+]cc'
                                               , DaughtersCuts = { 'pi+' : str_cutOfflineDaugA, 'K+' : str_cutOfflineDaugA }
                                               , CombinationCut = str_cutOfflineCombA
                                               , MotherCut = str_cutOfflineMothDsKPP_A
                                               )

combineOfflineD2KKP_A_LoosePID = StrippingMember( CombineParticles
                                               , 'Combine'
                                               , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListLoosePID ]
                                               , DecayDescriptor = '[D+ -> K- K+ pi+]cc'
                                               , DaughtersCuts = { 'pi+' : str_cutOfflineDaugA, 'K+' : str_cutOfflineDaugA }
                                               , CombinationCut = str_cutOfflineCombA
                                               , MotherCut = str_cutOfflineMothKKP_A
                                               )
combineOfflineD2KKK_A_LoosePID = StrippingMember( CombineParticles
                                               , 'Combine'
                                               , InputLocations = [ str_OfflineKaonListLoosePID ]
                                               , DecayDescriptor = '[D+ -> K- K+ K+]cc'
                                               , DaughtersCuts = { 'K+' : str_cutOfflineDaugA }
                                               , CombinationCut = str_cutOfflineCombA
                                               , MotherCut = str_cutOfflineMothKKK_A
                                               )

# 'B' selection lines (no PID)

combineOfflineD2PPP_B_NoPID = StrippingMember( CombineParticles, 'Combine'
                                         , InputLocations = [ str_OfflinePionListNoPID ]
                                         , DecayDescriptor = '[D+ -> pi- pi+ pi+]cc'
                                         , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB }
                                         , CombinationCut = str_cutOfflineCombB
                                         , MotherCut = str_cutOfflineMothB
                                         )
combineOfflineD2KPP_B_NoPID = StrippingMember( CombineParticles
                                         , 'Combine'
                                         , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListNoPID ]
                                         , DecayDescriptor = '[D+ -> K- pi+ pi+]cc'
                                         , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB, 'K+' : str_cutOfflineDaugB }
                                         , CombinationCut = str_cutOfflineCombB
                                         , MotherCut = str_cutOfflineMothB
                                         )

combineOfflineD2KKP_B_NoPID = StrippingMember( CombineParticles
                                       , 'Combine'
                                       , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListNoPID ]
                                       , DecayDescriptor = '[D+ -> K- K+ pi+]cc'
                                       , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB, 'K+' : str_cutOfflineDaugB }
                                       , CombinationCut = str_cutOfflineCombB
                                       , MotherCut = str_cutOfflineMothB
                                       )
combineOfflineD2KKK_B_NoPID = StrippingMember( CombineParticles
                                       , 'Combine'
                                       , InputLocations = [ str_OfflineKaonListNoPID ]
                                       , DecayDescriptor = '[D+ -> K- K+ K+]cc'
                                       , DaughtersCuts = { 'K+' : str_cutOfflineDaugB }
                                       , CombinationCut = str_cutOfflineCombB
                                       , MotherCut = str_cutOfflineMothB
                                       )

# 'B' selection lines (with PID)

combineOfflineD2PPP_B_LoosePID = StrippingMember( CombineParticles, 'Combine'
                                         , InputLocations = [ str_OfflinePionListNoPID ]
                                         , DecayDescriptor = '[D+ -> pi- pi+ pi+]cc'
                                         , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB }
                                         , CombinationCut = str_cutOfflineCombB
                                         , MotherCut = str_cutOfflineMothB
                                         )
combineOfflineD2KPP_B_LoosePID = StrippingMember( CombineParticles
                                         , 'Combine'
                                         , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListLoosePID ]
                                         , DecayDescriptor = '[D+ -> K- pi+ pi+]cc'
                                         , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB, 'K+' : str_cutOfflineDaugB }
                                         , CombinationCut = str_cutOfflineCombB
                                         , MotherCut = str_cutOfflineMothB
                                         )
combineOfflineDs2KPP_B_LoosePID = StrippingMember( CombineParticles
                                         , 'Combine'
                                         , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListLoosePID ]
                                         , DecayDescriptor = '[D_s+ -> K+ pi- pi+]cc'
                                         , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB, 'K+' : str_cutOfflineDaugB }
                                         , CombinationCut = str_cutOfflineCombB
                                         , MotherCut = str_cutOfflineMothB
                                         )
combineOfflineD2KKP_B_LoosePID = StrippingMember( CombineParticles
                                       , 'Combine'
                                       , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListLoosePID ]
                                       , DecayDescriptor = '[D+ -> K- K+ pi+]cc'
                                       , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB, 'K+' : str_cutOfflineDaugB }
                                       , CombinationCut = str_cutOfflineCombB
                                       , MotherCut = str_cutOfflineMothB
                                       )
combineOfflineD2KKK_B_LoosePID = StrippingMember( CombineParticles
                                       , 'Combine'
                                       , InputLocations = [ str_OfflineKaonListLoosePID ]
                                       , DecayDescriptor = '[D+ -> K- K+ K+]cc'
                                       , DaughtersCuts = { 'K+' : str_cutOfflineDaugB }
                                       , CombinationCut = str_cutOfflineCombB
                                       , MotherCut = str_cutOfflineMothB
                                       )

# Inclusive selection line (no PID)

combineOfflineD2HHH_A_NoPID = StrippingMember( CombineParticles
                                               , 'Combine'
                                               , InputLocations = [ str_OfflinePionListNoPID ]
                                               , DecayDescriptor = '[D+ -> pi- pi+ pi+]cc'
                                               , DaughtersCuts = { 'pi+' : str_cutOfflineDaugHHH }
                                               , CombinationCut = str_cutOfflineCombHHH
                                               , MotherCut = str_cutOfflineMothHHH
                                               )  

# Set up signal & sideband mass windows
# -------------------------------------

# Watch out: The 'A' lines may have had narrower windows applied earlier!

# These are mode-dependent.
# They have a little overlap between signal and sidebands (for book-keeping):

str_cutLowerSideband_PPP  = '((M > 1780*MeV) & (M<1856*MeV))'
str_cutSignalWindowD_PPP  = '((M > 1855*MeV) & (M<1885*MeV))'
str_cutCentreSideband_PPP = '((M > 1884*MeV) & (M<1954*MeV))'
str_cutSignalWindowDs_PPP = '((M > 1953*MeV) & (M<1984*MeV))'
str_cutUpperSideband_PPP  = '((M > 1983*MeV) & (M<2060*MeV))'

str_cutLowerSideband_KPP  = '((M > 1780*MeV) & (M<1859*MeV))'
str_cutSignalWindowD_KPP  = '((M > 1858*MeV) & (M<1882*MeV))'
str_cutCentreSideband_KPP = '((M > 1881*MeV) & (M<1957*MeV))'
str_cutSignalWindowDs_KPP = '((M > 1956*MeV) & (M<1981*MeV))'
str_cutUpperSideband_KPP  = '((M > 1980*MeV) & (M<2060*MeV))'

str_cutLowerSideband_KKP  = '((M > 1780*MeV) & (M<1861*MeV))'
str_cutSignalWindowD_KKP  = '((M > 1860*MeV) & (M<1880*MeV))'
str_cutCentreSideband_KKP = '((M > 1879*MeV) & (M<1959*MeV))'
str_cutSignalWindowDs_KKP = '((M > 1958*MeV) & (M<1979*MeV))'
str_cutUpperSideband_KKP  = '((M > 1978*MeV) & (M<2060*MeV))'

str_cutLowerSideband_KKK  = '((M > 1780*MeV) & (M<1861*MeV))'
str_cutSignalWindowD_KKK  = '((M > 1860*MeV) & (M<1880*MeV))'
str_cutCentreSideband_KKK = '((M > 1879*MeV) & (M<1959*MeV))'
str_cutSignalWindowDs_KKK = '((M > 1958*MeV) & (M<1979*MeV))'
str_cutUpperSideband_KKK  = '((M > 1978*MeV) & (M<2060*MeV))'

# In practice we usually bundle sidebands together, to reduce the number of lines.
# Likewise, we often bundle the D+ and Ds windows together. (Exception: D+ -> K- pi+ pi+)

str_cutBigSideband_PPP = '(' + str_cutLowerSideband_PPP + '|' + str_cutCentreSideband_PPP + '|' + str_cutUpperSideband_PPP + ')'
str_cutBigSideband_KPP = '(' + str_cutLowerSideband_KPP + '|' + str_cutCentreSideband_KPP + '|' + str_cutUpperSideband_KPP + ')'
str_cutBigSideband_KKP = '(' + str_cutLowerSideband_KKP + '|' + str_cutCentreSideband_KKP + '|' + str_cutUpperSideband_KKP + ')'
str_cutBigSideband_KKK = '(' + str_cutLowerSideband_KKK + '|' + str_cutCentreSideband_KKK + '|' + str_cutUpperSideband_KKK + ')'

str_cutBothSignals_PPP = '(' + str_cutSignalWindowD_PPP + '|' + str_cutSignalWindowDs_PPP + ')'
str_cutBothSignals_KPP = '(' + str_cutSignalWindowD_KPP + '|' + str_cutSignalWindowDs_KPP + ')'
str_cutBothSignals_KKP = '(' + str_cutSignalWindowD_KKP + '|' + str_cutSignalWindowDs_KKP + ')'
str_cutBothSignals_KKK = '(' + str_cutSignalWindowD_KKK + '|' + str_cutSignalWindowDs_KKK + ')'

str_cutBigSidebandAndDs_KPP = '(' + str_cutLowerSideband_KPP + '|' + str_cutCentreSideband_KPP + '|' + str_cutUpperSideband_KPP + '|' + str_cutSignalWindowDs_KPP + ')'
str_cutDsSideband = '(' + str_cutCentreSideband_KPP + '|' + str_cutUpperSideband_KPP + ')'

# We want to bundle the 'A' and 'B' selections together.
# Ideally we would have a single line which contains candidates selected by
# either algorithm. But since this is technically fiddly and the framework
# is going to change completely soon, I am doing it by brute force instead:

# Here is a useful snippet of code explaining what the '%' means:
#   From StrippingMember definition:
#          for i in  self.InputLocations :
#              if type(i) is bindMembers : i = i.outputLocation() 
#              if i[0] == '%' : i = 'Stripping' + line + i[1:]

# Define new prescales
ps_inclusive          = 0.04
ps_PPP_Bkg_A          = 0.2
ps_PPP_Bkg_B          = 1.0
ps_PPP_Sig            = 1.0
ps_KPP_Bkg_NoPID_A    = 0.1
ps_KPP_Bkg_NoPID_B    = 1.0
ps_KPP_Bkg_LoosePID_A = 0.1
ps_KPP_Bkg_LoosePID_B = 1.0
ps_KPP_Sig            = 1.0
ps_KKP_Bkg_NoPID      = 0.05
ps_KKP_Sig_NoPID      = 0.1
ps_KKP_Bkg_LoosePID   = 1.0
ps_KKP_Sig_LoosePID   = 1.0
ps_KKK_Bkg            = 1.0
ps_KKK_Sig            = 1.0
ps_DsKPP_Sig          = 1.0
ps_KPP_DCS_Sig        = 1.0
ps_DsKPP_Bkg_LoosePID_A  = 0.1
ps_DsKPP_Bkg_LoosePID_B  = 1.0
ps_KPP_DCS_Bkg_LoosePID_B = 1.0
ps_KPP_DCS_Bkg_LoosePID_A = 0.1

# Inclusive (heavily prescaled)
lineD2HHH = StrippingLine('StripD2HHH', prescale = ps_inclusive, algos = [ combineOfflineD2HHH_A_NoPID ])

# The (pi+ pi+ pi-) final state
filterD2PPP_A_NoPID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_PPP)
filterD2PPP_B_NoPID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_PPP)
lineD2PPP_A_NoPID_Bkg   = StrippingLine('StripD2PPP_A_NoPID_Bkg', prescale = ps_PPP_Bkg_A, algos = [ combineOfflineD2PPP_A_NoPID, filterD2PPP_A_NoPID_Bkg ])
lineD2PPP_B_NoPID_Bkg   = StrippingLine('StripD2PPP_B_NoPID_Bkg', prescale = ps_PPP_Bkg_B, algos = [ combineOfflineD2PPP_B_NoPID, filterD2PPP_B_NoPID_Bkg ])
lineD2PPP_A_NoPID_Sig = lineD2PPP_A_NoPID_Bkg.clone('StripD2PPP_A_NoPID_Sig', prescale = ps_PPP_Sig, FilterDesktopFilter = { "Code" : str_cutOfflineMassPPP } )  # mod. in 9/5
lineD2PPP_B_NoPID_Sig = lineD2PPP_B_NoPID_Bkg.clone('StripD2PPP_B_NoPID_Sig', prescale = ps_PPP_Sig, FilterDesktopFilter = { "Code" : str_cutBothSignals_PPP } )

# The (K- pi+ pi+) final state
filterD2KPP_A_NoPID_Bkg    = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSidebandAndDs_KPP)
filterD2KPP_B_NoPID_Bkg    = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSidebandAndDs_KPP)
filterD2KPP_A_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSidebandAndDs_KPP)
filterD2KPP_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSidebandAndDs_KPP)
lineD2KPP_A_NoPID_Bkg      = StrippingLine('StripD2KPP_A_NoPID_Bkg',    prescale = ps_KPP_Bkg_NoPID_A,    algos = [ combineOfflineD2KPP_A_NoPID,    filterD2KPP_A_NoPID_Bkg    ])
lineD2KPP_B_NoPID_Bkg      = StrippingLine('StripD2KPP_B_NoPID_Bkg',    prescale = ps_KPP_Bkg_NoPID_B,    algos = [ combineOfflineD2KPP_B_NoPID,    filterD2KPP_B_NoPID_Bkg    ])
lineD2KPP_A_LoosePID_Bkg   = StrippingLine('StripD2KPP_A_LoosePID_Bkg', prescale = ps_KPP_Bkg_LoosePID_A, algos = [ combineOfflineD2KPP_A_LoosePID, filterD2KPP_A_LoosePID_Bkg ])
lineD2KPP_B_LoosePID_Bkg   = StrippingLine('StripD2KPP_B_LoosePID_Bkg', prescale = ps_KPP_Bkg_LoosePID_B, algos = [ combineOfflineD2KPP_B_LoosePID, filterD2KPP_B_LoosePID_Bkg ])
lineD2KPP_A_NoPID_Sig      = lineD2KPP_A_NoPID_Bkg.clone('StripD2KPP_A_NoPID_Sig',       prescale = ps_KPP_Sig, FilterDesktopFilter = { "Code" : str_cutOfflineMassKPP } )  # mod. in 9/5
lineD2KPP_B_NoPID_Sig      = lineD2KPP_B_NoPID_Bkg.clone('StripD2KPP_B_NoPID_Sig',       prescale = ps_KPP_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowD_KPP } )
lineD2KPP_A_LoosePID_Sig   = lineD2KPP_A_LoosePID_Bkg.clone('StripD2KPP_A_LoosePID_Sig', prescale = ps_KPP_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowD_KPP } )
lineD2KPP_B_LoosePID_Sig   = lineD2KPP_B_LoosePID_Bkg.clone('StripD2KPP_B_LoosePID_Sig', prescale = ps_KPP_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowD_KPP } )

# The (K+ pi- pi+_ final state 

filterDs2KPP_A_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutDsSideband)
filterDs2KPP_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutDsSideband)
filterD2KPP_DCS_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutLowerSideband_KPP)
filterD2KPP_DCS_A_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutLowerSideband_KPP)
lineDs2KPP_A_LoosePID_Bkg   = StrippingLine('StripDs2KPP_A_LoosePID_Bkg', prescale = ps_DsKPP_Bkg_LoosePID_A, algos = [ combineOfflineDs2KPP_A_LoosePID, filterDs2KPP_A_LoosePID_Bkg ])
lineDs2KPP_B_LoosePID_Bkg   = StrippingLine('StripDs2KPP_B_LoosePID_Bkg', prescale = ps_DsKPP_Bkg_LoosePID_B, algos = [ combineOfflineDs2KPP_B_LoosePID, filterDs2KPP_B_LoosePID_Bkg ])
lineD2KPP_DCS_B_LoosePID_Bkg = StrippingLine('StripD2KPP_DCS_B_LoosePID_Bkg', prescale = ps_KPP_DCS_Bkg_LoosePID_B, algos = [ combineOfflineDs2KPP_B_LoosePID, filterD2KPP_DCS_B_LoosePID_Bkg ])
lineD2KPP_DCS_A_LoosePID_Bkg = StrippingLine('StripD2KPP_DCS_A_LoosePID_Bkg', prescale = ps_KPP_DCS_Bkg_LoosePID_A, algos = [ combineOfflineDs2KPP_A_LoosePID, filterD2KPP_DCS_A_LoosePID_Bkg ])
lineDs2KPP_A_LoosePID_Sig   = lineDs2KPP_A_LoosePID_Bkg.clone('StripDs2KPP_A_LoosePID_Sig', prescale = ps_DsKPP_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowDs_KPP } )
lineDs2KPP_B_LoosePID_Sig   = lineDs2KPP_B_LoosePID_Bkg.clone('StripDs2KPP_B_LoosePID_Sig', prescale = ps_DsKPP_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowDs_KPP } )
lineD2KPP_DCS_B_LoosePID_Sig   = lineD2KPP_DCS_B_LoosePID_Bkg.clone('StripD2KPP_DCS_B_LoosePID_Sig', prescale = ps_KPP_DCS_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowD_KPP } )
lineD2KPP_DCS_A_LoosePID_Sig   = lineD2KPP_DCS_A_LoosePID_Bkg.clone('StripD2KPP_DCS_A_LoosePID_Sig', prescale = ps_KPP_DCS_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowD_KPP } ) 


# The (K- K+ pi+) final state
filterD2KKP_A_NoPID_Bkg    = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_KKP)
filterD2KKP_B_NoPID_Bkg    = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_KKP)
filterD2KKP_A_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_KKP)
filterD2KKP_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_KKP)
lineD2KKP_A_NoPID_Bkg      = StrippingLine('StripD2KKP_A_NoPID_Bkg',    prescale = ps_KKP_Bkg_NoPID,     algos = [ combineOfflineD2KKP_A_NoPID,    filterD2KKP_A_NoPID_Bkg    ])
lineD2KKP_B_NoPID_Bkg      = StrippingLine('StripD2KKP_B_NoPID_Bkg',    prescale = ps_KKP_Bkg_NoPID,     algos = [ combineOfflineD2KKP_B_NoPID,    filterD2KKP_B_NoPID_Bkg    ])
lineD2KKP_A_LoosePID_Bkg   = StrippingLine('StripD2KKP_A_LoosePID_Bkg', prescale = ps_KKP_Bkg_LoosePID,  algos = [ combineOfflineD2KKP_A_LoosePID, filterD2KKP_A_LoosePID_Bkg ])
lineD2KKP_B_LoosePID_Bkg   = StrippingLine('StripD2KKP_B_LoosePID_Bkg', prescale = ps_KKP_Bkg_LoosePID,  algos = [ combineOfflineD2KKP_B_LoosePID, filterD2KKP_B_LoosePID_Bkg ])
lineD2KKP_A_NoPID_Sig      = lineD2KKP_A_NoPID_Bkg.clone('StripD2KKP_A_NoPID_Sig',       prescale = ps_KKP_Sig_NoPID,    FilterDesktopFilter = { "Code" : str_cutOfflineMassKKP } )  # mod in 9/5
lineD2KKP_B_NoPID_Sig      = lineD2KKP_B_NoPID_Bkg.clone('StripD2KKP_B_NoPID_Sig',       prescale = ps_KKP_Sig_NoPID,    FilterDesktopFilter = { "Code" : str_cutBothSignals_KKP } )
lineD2KKP_A_LoosePID_Sig   = lineD2KKP_A_LoosePID_Bkg.clone('StripD2KKP_A_LoosePID_Sig', prescale = ps_KKP_Sig_LoosePID, FilterDesktopFilter = { "Code" : str_cutBothSignals_KKP } )
lineD2KKP_B_LoosePID_Sig   = lineD2KKP_B_LoosePID_Bkg.clone('StripD2KKP_B_LoosePID_Sig', prescale = ps_KKP_Sig_LoosePID, FilterDesktopFilter = { "Code" : str_cutBothSignals_KKP } )

# The (K- K+ K+) final state
filterD2KKK_A_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_KKK)
filterD2KKK_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_KKK)
lineD2KKK_A_LoosePID_Bkg   = StrippingLine('StripD2KKK_A_LoosePID_Bkg', prescale = ps_KKK_Bkg, algos = [ combineOfflineD2KKK_A_LoosePID, filterD2KKK_A_LoosePID_Bkg ])
lineD2KKK_B_LoosePID_Bkg   = StrippingLine('StripD2KKK_B_LoosePID_Bkg', prescale = ps_KKK_Bkg, algos = [ combineOfflineD2KKK_B_LoosePID, filterD2KKK_B_LoosePID_Bkg ])
lineD2KKK_A_LoosePID_Sig   = lineD2KKK_A_LoosePID_Bkg.clone('StripD2KKK_A_LoosePID_Sig', prescale = ps_KKK_Sig, FilterDesktopFilter = { "Code" : str_cutBothSignals_KKK } )
lineD2KKK_B_LoosePID_Sig   = lineD2KKK_B_LoosePID_Bkg.clone('StripD2KKK_B_LoosePID_Sig', prescale = ps_KKK_Sig, FilterDesktopFilter = { "Code" : str_cutBothSignals_KKK } )
