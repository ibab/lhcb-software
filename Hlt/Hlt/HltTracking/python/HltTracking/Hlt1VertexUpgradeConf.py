# ========================================================================#
# vertex-version of track upgrade strings defined in Hlt1Tracking         #
# for Hlt1 displaced vertex line
# ========================================================================#

def LooseForward(allowFail=False):
    if allowFail:
        return "LooseForward  = ( execute(decodeIT) * TC_UPGRADE_VX ( '', HltTracking.Hlt1StreamerConf.LooseForward , True ) )"
    else:
        return "LooseForward  = ( execute(decodeIT) * TC_UPGRADE_VX ( '', HltTracking.Hlt1StreamerConf.LooseForward  ) )"

def FitTracks(allowFail=False):
    if allowFail:
        return "FitTracks     = TC_UPGRADE_VX ( '', HltTracking.Hlt1StreamerConf.FitTrack , True )"
    else:
        return "FitTracks     = TC_UPGRADE_VX ( '', HltTracking.Hlt1StreamerConf.FitTrack )"

def VeloOnlyFitTracks(allowFail=False):
    if allowFail:
        return "VeloOnlyFitTracks = TC_UPGRADE_VX ( '', HltTracking.Hlt1StreamerConf.VeloOnlyFitTrack , True )"
    else:
        return "VeloOnlyFitTracks = TC_UPGRADE_VX ( '', HltTracking.Hlt1StreamerConf.VeloOnlyFitTrack )"
