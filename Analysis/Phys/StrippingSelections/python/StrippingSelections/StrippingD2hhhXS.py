# Temporary stripping for cross-section measurements only, based on B lines in
# StrippingD2hhh.py
# Comments on prescaling and relative importance of lines:
#
# There are a lot of stripping lines in this file:
#   a) Some are narrow windows where we want to make cross-section measurements;
#   b) Some are wider sideband windows where we want to study the backgrounds
#   c) Some are for control modes that we want to use as crosschecks to make
#      sure we understand our physics channels from (a);
#   d) Some are sidebands to the control modes from (c);
#   e) Some are for other crosschecks, e.g. testing PID efficiency.
# 
# We've tried to choose sensible default prescales to keep the event retention rate
# reasonable -- but what counts as "reasonable" will probably change over time
# as the luminosity ramps up. So it may be that some of these prescales need to be
# tightened later. If you need to do that, please first contact at least:
#
#   Mat Charles <m.charles1@physics.ox.ac.uk>
#   Hamish Gordon <h.gordon1@physics.ox.ac.uk>
#
# As a rough guide, here is our thinking on what is most important (from most
# to least):
#
#  * These are critical signal windows and should never be prescaled:
#     
#      * lineD2KKK_B_LoosePID_Sig
#      * lineD2KKP_B_LoosePID_Sig
#      * lineD2KPP_B_NoPID_Sig
#      * lineDs2KPP_B_LoosePID_Sig
#
#  * These are high-statistics control modes for our signals ("standard
#    candles"). They are useful and are not prescaled by default, but if
#    the rate needs to be brought they could be (but not to zero!).
#     
#      * lineD2KPP_B_LoosePID_Sig
#      * lineD2KPP_B_LoosePID_Sig
#      * lineD2PPP_B_NoPID_Sig
#
#  * These are calibration/test channels to study PID performance. It is vital
# to have some events here for crosschecks, but they can be prescaled safely.
#      * lineD2KKP_B_NoPID_Sig
#      * lineD2KKP_B_NoPID_Bkg
#      * lineD2KKP_B_LoosePID_Sig
#      * lineD2KKP_B_LoosePID_Bkg
#
#  * These are the sidebands  to our signals. They can be prescaled (and
#    are by default) but we need enough events here to study the backgrounds
#    and test the MC behaviour.
#     
#     
#      * lineD2KPP_B_NoPID_Bkg
#      * lineD2KKP_B_LoosePID_Bkg
#      * lineDs2KPP_B_LoosePID_Bkg
#  * These are the sidebands to our control channels. They are less critical,
#    but we need enough events to fit the mass spectrum and test out sideband
#    subtraction methods on the control channels. They are prescaled by default.
#      * lineD2KPP_A_NoPID_Bkg
#      * lineD2PPP_B_NoPID_Bkg
#      * lineD2KPP_A_LoosePID_Bkg
#      * lineD2KPP_B_LoosePID_Bkg
#      * lineD2KKK_A_LoosePID_Bkg
#      * lineD2KKK_B_LoosePID_Bkg
#


from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, bindMembers

from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons

# Set up cuts (numbering corresponds to the normal 3h strip).


useCutsB = bool(True)

# 1) Pointing of D+ (applied as MOTHERCUT)
# 0.9999  is now standard in the 3h selection as well
str_cutOffline1B_asMothCut = '(BPVDIRA > 0.9999)'

# 3) IP of D+ daughters to PV.
# This has to be a COMBINATIONCUT.
str_cutOffline3B_asCombCut = '(AHASCHILD((MIPCHI2DV(PRIMARY)) > 30.0))'

# 4) Flight distance of D+ from PV (applied as MOTHERCUT)
# 25 is now standard in the 3h selection as well
str_cutOffline4B_asMothCut = '(BPVVDCHI2 > 25.0)'

# 5) Vertex quality of D+
str_cutOffline5Moth = '(VFASPF(VCHI2)<8.0)'


# 6) PT cuts
# There is a daughter cut (applied to every daughter), but no mother cut, as
# we want to measure cross-section in bins of PT.
# The combination cut is looser than the 3h selection
# There are also cuts that apply to the combination or to the mother.
# For each line individually: use B daughter & B mother cut & B combination cut

str_cutOffline6DaugB      = '((PT > 200*MeV) & (P > 3200*MeV) & (P < 100*GeV))'
str_cutOffline6BComb = '( (ANUM(PT > 400.0*MeV) >= 2))'

# 7) Track quality of D+ daughters
# For each line individually: use DAUGHTERSCUT.

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
# B: 1=Moth, 2=Moth, 3=Comb, 4=Moth, 5=Moth,      6=Comb+Moth+Daug, 7=Daug

str_cutOfflineDaugB      = '(' + str_cutOffline6DaugB + '&' + str_cutOffline7B_asDaugCut + ')'
str_cutOfflineMothB      = '(' + str_cutOffline1B_asMothCut + '&' + str_cutOffline4B_asMothCut + '&' + str_cutOffline5Moth + '&' + str_cutOfflineMassPostFit + ')'

str_cutOfflineCombB      = '(' + str_cutOfflineMassPreFit + '&' + str_cutOffline6BComb + '&' + str_cutOffline3B_asCombCut + ')'


# Create D+ -> hhh candidates
# ---------------------------
                          

#No PID

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

# With PID

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

# Set up signal & sideband mass windows
# -------------------------------------

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

##########################################################

# Here is a useful snippet of code explaining what the '%' means:
#   From StrippingMember definition:
#          for i in  self.InputLocations :
#              if type(i) is bindMembers : i = i.outputLocation() 
#              if i[0] == '%' : i = 'Stripping' + line + i[1:]

# Define prescales
ps_inclusive          = 1.0
ps_PPP_Bkg_B          = 0.2
ps_PPP_Sig            = 0.2
ps_KPP_Bkg_NoPID_B    = 1.0
ps_KPP_Bkg_LoosePID_B = 1.0
ps_KPP_Sig            = 1.0
ps_KKP_Bkg_NoPID      = 0.1
ps_KKP_Sig_NoPID      = 0.1
ps_KKP_Bkg_LoosePID   = 1.0
ps_KKP_Sig_LoosePID   = 1.0
ps_KKK_Bkg            = 1.0
ps_KKK_Sig            = 1.0
ps_DsKPP_Sig          = 1.0
ps_KPP_DCS_Sig        = 1.0
ps_DsKPP_Bkg_LoosePID_B  = 1.0
ps_KPP_DCS_Bkg_LoosePID_B = 1.0

# The (pi+ pi- pi+) final state

filterD2PPP_B_NoPID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_PPP)
lineD2PPP_B_NoPID_BkgXS   = StrippingLine('StripD2PPP_B_NoPID_BkgXS', prescale = ps_PPP_Bkg_B, algos = [ combineOfflineD2PPP_B_NoPID, filterD2PPP_B_NoPID_Bkg ])
lineD2PPP_B_NoPID_SigXS = lineD2PPP_B_NoPID_BkgXS.clone('StripD2PPP_B_NoPID_SigXS', prescale = ps_PPP_Sig, FilterDesktopFilter = { "Code" : str_cutBothSignals_PPP } )

# The (K- pi+ pi+) final state

filterD2KPP_B_NoPID_Bkg    = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSidebandAndDs_KPP)
filterD2KPP_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSidebandAndDs_KPP)
lineD2KPP_B_NoPID_BkgXS      = StrippingLine('StripD2KPP_B_NoPID_BkgXS',    prescale = ps_KPP_Bkg_NoPID_B,    algos = [ combineOfflineD2KPP_B_NoPID,    filterD2KPP_B_NoPID_Bkg    ])
lineD2KPP_B_LoosePID_BkgXS   = StrippingLine('StripD2KPP_B_LoosePID_BkgXS', prescale = ps_KPP_Bkg_LoosePID_B, algos = [ combineOfflineD2KPP_B_LoosePID, filterD2KPP_B_LoosePID_Bkg ])
lineD2KPP_B_NoPID_SigXS      = lineD2KPP_B_NoPID_BkgXS.clone('StripD2KPP_B_NoPID_SigXS',       prescale = ps_KPP_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowD_KPP } )
lineD2KPP_B_LoosePID_SigXS   = lineD2KPP_B_LoosePID_BkgXS.clone('StripD2KPP_B_LoosePID_SigXS', prescale = ps_KPP_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowD_KPP } )


# The (K+ pi- pi+) final state 

filterDs2KPP_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutDsSideband)
filterD2KPP_DCS_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutLowerSideband_KPP)
lineDs2KPP_B_LoosePID_BkgXS   = StrippingLine('StripDs2KPP_B_LoosePID_BkgXS', prescale = ps_DsKPP_Bkg_LoosePID_B, algos = [ combineOfflineDs2KPP_B_LoosePID, filterDs2KPP_B_LoosePID_Bkg ])
lineD2KPP_DCS_B_LoosePID_BkgXS = StrippingLine('StripD2KPP_DCS_B_LoosePID_BkgXS', prescale = ps_KPP_DCS_Bkg_LoosePID_B, algos = [ combineOfflineDs2KPP_B_LoosePID, filterD2KPP_DCS_B_LoosePID_Bkg ])
lineDs2KPP_B_LoosePID_SigXS   = lineDs2KPP_B_LoosePID_BkgXS.clone('StripDs2KPP_B_LoosePID_SigXS', prescale = ps_DsKPP_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowDs_KPP } )
lineD2KPP_DCS_B_LoosePID_SigXS   = lineD2KPP_DCS_B_LoosePID_BkgXS.clone('StripD2KPP_DCS_B_LoosePID_SigXS', prescale = ps_KPP_DCS_Sig, FilterDesktopFilter = { "Code" : str_cutSignalWindowD_KPP } )


# The (K- K+ pi+) final state

filterD2KKP_B_NoPID_Bkg    = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_KKP)
filterD2KKP_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_KKP)
lineD2KKP_B_NoPID_BkgXS      = StrippingLine('StripD2KKP_B_NoPID_BkgXS',    prescale = ps_KKP_Bkg_NoPID,     algos = [ combineOfflineD2KKP_B_NoPID,    filterD2KKP_B_NoPID_Bkg    ])
lineD2KKP_B_LoosePID_BkgXS   = StrippingLine('StripD2KKP_B_LoosePID_BkgXS', prescale = ps_KKP_Bkg_LoosePID,  algos = [ combineOfflineD2KKP_B_LoosePID, filterD2KKP_B_LoosePID_Bkg ])
lineD2KKP_B_NoPID_SigXS      = lineD2KKP_B_NoPID_BkgXS.clone('StripD2KKP_B_NoPID_SigXS',       prescale = ps_KKP_Sig_NoPID,    FilterDesktopFilter = { "Code" : str_cutBothSignals_KKP } )
lineD2KKP_B_LoosePID_SigXS   = lineD2KKP_B_LoosePID_BkgXS.clone('StripD2KKP_B_LoosePID_SigXS', prescale = ps_KKP_Sig_LoosePID, FilterDesktopFilter = { "Code" : str_cutBothSignals_KKP } )

# The (K- K+ K+) final state
filterD2KKK_B_LoosePID_Bkg = StrippingMember(FilterDesktop, "Filter", InputLocations = [ "%Combine" ], Code = str_cutBigSideband_KKK)
lineD2KKK_B_LoosePID_BkgXS   = StrippingLine('StripD2KKK_B_LoosePID_BkgXS', prescale = ps_KKK_Bkg, algos = [ combineOfflineD2KKK_B_LoosePID, filterD2KKK_B_LoosePID_Bkg ])
lineD2KKK_B_LoosePID_SigXS   = lineD2KKK_B_LoosePID_BkgXS.clone('StripD2KKK_B_LoosePID_SigXS', prescale = ps_KKK_Sig, FilterDesktopFilter = { "Code" : str_cutBothSignals_KKK } )
