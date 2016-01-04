"""
Module importing stripping selection line builder modules
for Calib WG.
"""

_selections = [
    #"StrippingElectronID", #now part of StrippingPIDCalib
    "StrippingMuIDCalib",#now part of StrippingPIDCalib
    "StrippingNoPIDDstarWithD02RSKPi", #deactivated, might run in Run 1 restripping
    #"StrippingPIDCalib", #deactivated, might run in Run 1 restripping
    "StrippingV0ForPID",#now part of StrippingPIDCalib
    #"StrippingCharmFromBSemiForHadronAsy",
    #"StrippingCharmFromBSemiForProtonPID",
    #"StrippingProtonAsym",
    #"StrippingTrackEffD0ToKPi",
    #"StrippingTrackEffD0ToK3Pi",
    #"StrippingTrackEffMuonTT",
    #"StrippingTrackEffVeloMuon",
    #"StrippingTrackEffDownMuon",
    #"StrippingD02KPiPi0",
    #"StrippingSigmacForPID",
    #"StrippingDstarD02KShhForTrackingEff",
    #"StrippingJPsiForSL",
    "StrippingLowPtMuID"
 ]

for _sel in _selections :
    try :
        __import__( '%s.%s'  % ( __name__, _sel ) )
    except Exception, x:
        print '[WARNING] Submodule %s.%s raises the exception "%s" and will be skipped !' % ( __name__,_sel,x )

from sys import modules as _modules
_this = _modules[__name__]

_strippingKeys = filter ( lambda x : x[:9]=='Stripping',
                          locals().keys())

_strippingModules = [getattr(_this, _k) for _k in _strippingKeys]

