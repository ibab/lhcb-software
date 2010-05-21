# Comments on prescaling and relative importance of lines:
#
#   These stripping lines are cross-checks for the D->3h cross-section
#   measurement. They should help in determining tracking efficiencies
#   The no-PID lines are cross-checks to the PID efficiency.
# We've tried to choose sensible default prescales to keep the event retention rate
# reasonable -- but what counts as "reasonable" will probably change over time
# as the luminosity ramps up. So it may be that some of these prescales need to be
# tightened later. If you need to do that, please first contact at least:
#
#  Hamish Gordon <h.gordon1@physics.ox.ac.uk>
#  Mat Charles <m.charles1@physics.ox.ac.uk>
#
# If it becomes necessary to prescale these lines please start with the no-PID
# background (sideband) lines, then the no-PID signal lines, then the with-PID
# sideband lines, and finally the with-PID signal. The modes have low branching
# fractions so we'd rather you remove the no-PID lines altogether before you
# touch the signal lines.

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, bindMembers

from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons

# Set up cuts (numbering corresponds to the 3h strip).

useCutsB = bool(True)

# 1) Pointing of D+ (applied as MOTHERCUT)
str_cutOffline1B_asMothCut = '(BPVDIRA > 0.9999)'

# 3) IP of D+ daughters to PV.
# This has to be a COMBINATIONCUT.
str_cutOffline3B_asCombCut = '(AHASCHILD((MIPCHI2DV(PRIMARY)) > 30.0))'

# 4) Flight distance of D+ from PV (applied as MOTHERCUT)
str_cutOffline4B_asMothCut = '(BPVVDCHI2 > 25.0)'

# 5) Vertex quality of D+
# We apply just the chi^2 cut (MOTHERCUT).
str_cutOffline5Moth = '(VFASPF(VCHI2)<8.0)'

# 6) PT cuts
# There is a daughter cut (applied to every daughter).
# There are also cuts that apply to the combination or to the mother.
# For each cut individually: use B daughter & B mother cut & B combination cut
str_cutOffline6DaugB      = '((PT > 200*MeV) & (P > 3200*MeV) & (P < 100*GeV))'
str_cutOffline6BComb = '((ANUM(PT > 400.0*MeV) >= 2))' #(APT > 1200*MeV) &  PT cut is looser than post-fit (mother) cut to allow for modification by fit
#str_cutOffline6BMoth = '(PT >1800*MeV)'

# 7) Track quality of D+ daughters
# For each cut individually: use DAUGHTERSCUT.
str_cutOffline7B_asCombCut = '(AMINCHILD(TRPCHI2) > 0.0001)'
str_cutOffline7B_asDaugCut = '(TRPCHI2 > 0.0001)'

# 8) PID and track lists
str_OfflinePionListNoPID = 'StdNoPIDsPions'
str_OfflineKaonListNoPID = 'StdNoPIDsKaons'
str_OfflineKaonListLoosePID = 'StdLooseKaons'

# Mass cuts (big envelope; we may go back and chop out bits later)
str_cutOfflineMassPreFit = '((AM > 1580*MeV) & (AM < 2260*MeV))'
str_cutOfflineMassPostFit = '((M > 1780*MeV) & (M < 2060*MeV))'


# Combined cuts:
# 1=Moth, 2=Moth, 3=Comb, 4=Moth, 5=Moth,      6=Comb+Moth+Daug, 7=Daug

str_cutOfflineDaugB      = '(' + str_cutOffline6DaugB + '&' + str_cutOffline7B_asDaugCut + ')'
str_cutOfflineMothB      = '(' + str_cutOffline1B_asMothCut +  '&' + str_cutOffline4B_asMothCut + '&' + str_cutOffline5Moth + '&' + str_cutOfflineMassPostFit + ')'
str_cutOfflineCombB      = '(' + str_cutOfflineMassPreFit + '&' + str_cutOffline6BComb + '&' + str_cutOffline3B_asCombCut + ')'

# Create D+ -> hhh candidates
# ---------------------------

# No PID

combineOfflineD2KKPPP_B_NoPID = StrippingMember( CombineParticles, 'Combine'
                                         , InputLocations = [ str_OfflinePionListNoPID,  str_OfflineKaonListNoPID ]
                                         , DecayDescriptor = '[D+ -> K+ K- pi+ pi+ pi-]cc'
                                         , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB,  'K+' : str_cutOfflineDaugB   }
                                         , CombinationCut = str_cutOfflineCombB
                                         , MotherCut = str_cutOfflineMothB
                                         )
combineOfflineD2KPPPP_B_NoPID = StrippingMember( CombineParticles
                                         , 'Combine'
                                         , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListNoPID ]
                                         , DecayDescriptor = '[D+ -> K- pi+ pi+ pi+ pi-]cc'
                                         , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB, 'K+' : str_cutOfflineDaugB }
                                         , CombinationCut = str_cutOfflineCombB
                                         , MotherCut = str_cutOfflineMothB
                                         )

# With PID

combineOfflineD2KKPPP_B_LoosePID = StrippingMember( CombineParticles, 'Combine'
                                         , InputLocations = [ str_OfflinePionListNoPID,  str_OfflineKaonListLoosePID ]
                                         , DecayDescriptor = '[D+ -> K+ K- pi+ pi+ pi-]cc'
                                         , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB ,  'K+' : str_cutOfflineDaugB }
                                         , CombinationCut = str_cutOfflineCombB
                                         , MotherCut = str_cutOfflineMothB
                                         )
combineOfflineD2KPPPP_B_LoosePID = StrippingMember( CombineParticles
                                         , 'Combine'
                                         , InputLocations = [ str_OfflinePionListNoPID, str_OfflineKaonListLoosePID ]
                                         , DecayDescriptor = '[D+ -> K- pi+ pi+ pi+ pi-]cc'
                                         , DaughtersCuts = { 'pi+' : str_cutOfflineDaugB, 'K+' : str_cutOfflineDaugB }
                                         , CombinationCut = str_cutOfflineCombB
                                         , MotherCut = str_cutOfflineMothB
                                         )
# Set up signal & sideband mass windows
# -------------------------------------

# These are mode-dependent.
# They have a little overlap between signal and sidebands (for book-keeping):

str_cutLowerSideband  = '((M > 1780*MeV) & (M<1859*MeV))'
str_cutSignalWindowD  = '((M > 1858*MeV) & (M<1882*MeV))'
str_cutCentreSideband = '((M > 1881*MeV) & (M<1957*MeV))'
str_cutSignalWindowDs = '((M > 1956*MeV) & (M<1981*MeV))'
str_cutUpperSideband  = '((M > 1980*MeV) & (M<2060*MeV))'

# Likewise, we often bundle the D+ and Ds windows together. (Exception: D+ -> K- pi+ pi+)

str_cutBigSideband = '(' + str_cutLowerSideband + '|' + str_cutCentreSideband + '|' + str_cutUpperSideband + ')'

str_cutBothSignals = '(' + str_cutSignalWindowD + '|' + str_cutSignalWindowDs + ')'

# Here is a useful snippet of code explaining what the '%' means:
#   From StrippingMember definition:
#          for i in  self.InputLocations :
#              if type(i) is bindMembers : i = i.outputLocation() 
#              if i[0] == '%' : i = 'Stripping' + line + i[1:]

# Define prescales
ps_KPPPP_Sig_NoPID            = 0.2
ps_KPPPP_Sig_LoosePID         = 1.0
ps_KKPPP_Sig_NoPID            = 0.2
ps_KKPPP_Sig_LoosePID         = 1.0
ps_KPPPP_Bkg_NoPID            = 0.2
ps_KPPPP_Bkg_LoosePID         = 1.0
ps_KKPPP_Bkg_NoPID            = 0.2
ps_KKPPP_Bkg_LoosePID         = 1.0


filterD2KKPPP_B_NoPID_Bkg    = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband)
filterD2KKPPP_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband)
lineD2KKPPP_B_NoPID_Bkg      = StrippingLine('StripD2KKPPP_B_NoPID_Bkg',    prescale = ps_KKPPP_Bkg_NoPID,    algos = [ combineOfflineD2KKPPP_B_NoPID,    filterD2KKPPP_B_NoPID_Bkg    ])
lineD2KKPPP_B_LoosePID_Bkg   = StrippingLine('StripD2KKPPP_B_LoosePID_Bkg', prescale = ps_KKPPP_Bkg_LoosePID, algos = [ combineOfflineD2KKPPP_B_LoosePID, filterD2KKPPP_B_LoosePID_Bkg ])
lineD2KKPPP_B_NoPID_Sig      = lineD2KKPPP_B_NoPID_Bkg.clone('StripD2KKPPP_B_NoPID_Sig',       prescale = ps_KKPPP_Sig_NoPID, FilterDesktopFilter = { "Code" : str_cutBothSignals } )
lineD2KKPPP_B_LoosePID_Sig   = lineD2KKPPP_B_LoosePID_Bkg.clone('StripD2KKPPP_B_LoosePID_Sig', prescale = ps_KKPPP_Sig_LoosePID, FilterDesktopFilter = { "Code" : str_cutBothSignals } )

filterD2KPPPP_B_NoPID_Bkg    = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband)
filterD2KPPPP_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband)
lineD2KPPPP_B_NoPID_Bkg      = StrippingLine('StripD2KPPPP_B_NoPID_Bkg',    prescale = ps_KPPPP_Bkg_NoPID,    algos = [ combineOfflineD2KPPPP_B_NoPID,    filterD2KPPPP_B_NoPID_Bkg    ])
lineD2KPPPP_B_LoosePID_Bkg   = StrippingLine('StripD2KPPPP_B_LoosePID_Bkg', prescale = ps_KPPPP_Bkg_LoosePID, algos = [ combineOfflineD2KPPPP_B_LoosePID, filterD2KPPPP_B_LoosePID_Bkg ])
lineD2KPPPP_B_NoPID_Sig      = lineD2KPPPP_B_NoPID_Bkg.clone('StripD2KPPPP_B_NoPID_Sig',       prescale = ps_KPPPP_Sig_NoPID, FilterDesktopFilter = { "Code" : str_cutBothSignals } )
lineD2KPPPP_B_LoosePID_Sig   = lineD2KPPPP_B_LoosePID_Bkg.clone('StripD2KPPPP_B_LoosePID_Sig', prescale = ps_KPPPP_Sig_LoosePID, FilterDesktopFilter = { "Code" : str_cutBothSignals } )

