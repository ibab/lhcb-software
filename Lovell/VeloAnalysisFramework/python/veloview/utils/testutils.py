# coding=utf-8
from ROOT import TH1D


def read_as(rdir, name, newname):
    """Read ROOT object from file, and rename it."""
    return rdir.Get(name).Clone(newname)

def get_tell1s(r_or_p=None):
    """Get list of TELL1s."""
    if r_or_p == 'r':
        tell1s = [i for i in range(42)]
    elif r_or_p == 'p':
        tell1s = [i for i in range(64, 106)]
    else:
        tell1s = [i for i in range(42)]
        tell1s += [i for i in range(64, 106)]
    return tell1s

def get_avg_hist(rdir, hname, newname=None, r_or_p=None):
    """Get average histograms for all TELL1s.

    rdir   -- ROOT directory with histograms for all TELL1s
    hname  -- Histogram name
    r_or_p -- R or φ histograms (string: 'r' or 'p', all when neither)

    """
    if not newname:
        newname = 'avg_hist'
    avg_hist = TH1D(newname, '', 50, 0, 5)
    for tell1 in get_tell1s(r_or_p):
        h = rdir.Get('TELL1_{:0>3}/{}'.format(tell1, hname))
        binc = 0
        for b in range(1, h.GetNbinsX()+1):
            binc += h.GetBinContent(b)
        avg_hist.Fill(binc/h.GetNbinsX())
    return avg_hist

def get_avg_trend(rdir, hname, newname=None, r_or_p=None):
    """Get trend of average."""
    if not newname:
        newname = 'trend_hist'
    trend_hist = TH1D(newname, '', 106, -0.5, 105.5)
    for tell1 in get_tell1s(r_or_p):
        h = rdir.Get('TELL1_{:0>3}/{}'.format(tell1, hname))
        binc = 0
        for b in range(1, h.GetNbinsX()+1):
            binc += h.GetBinContent(b)
        trend_hist.SetBinContent(tell1, binc/h.GetNbinsX())
    return trend_hist
