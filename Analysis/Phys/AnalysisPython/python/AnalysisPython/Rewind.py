# -*- coding: utf-8 -*-
from GaudiPython.Bindings import iHistogramSvc, AppMgr, SUCCESS, FAILURE
from AnalysisPython import Dir

"""
Module containing add-on methods for re-setting hostograms and rewinding the
event data store. Adds methods
  - AppMgr.rewind
  - iHistogramSvc.reset
"""
__author__ = "Juan Palacios juan.palacios@nikhef.nl"

def _histSvc_reset_(self) :
    """
    Simple tool to re-set all the histograms in the HistogramSvc.
    @author Juan Palacios juan.palacios@nikhef.nl
    Usage:
    >>> from AnalysisPython import Rewind
    >>> appMgr = AppMgr(....)
    >>> histSvc = appMgr.histSvc
    >>> ... # book and fill some histograms
    >>> histSvc.reset()
    """
    histos = self.nodes()
    for h in histos :
        print "iHistogramSvc: Re-setting histogram ", h
        hh = self[h] 
        if   hasattr ( hh , 'reset' ) : hh.reset()
        elif hasattr ( hh , 'Reset' ) : hh.Reset()

iHistogramSvc.reset = _histSvc_reset_

def _appMgr_rewind_ (self, histos=True) :
    """
    Simple tool to rewind the event selector and
    re-set all the histograms in the HistogramSvc.
    @author Juan Palacios juan.palacios@nikhef.nl
    Usage:
    >>> from AnalysisPython import Rewind
    >>> appMgr = AppMgr(....)
    >>> histSvc = appMgr.histSvc
    >>> ... # book and fill some histograms
    >>>  # loop over some events
    >>> appMgr.rewind()
    To avoid re-setting of histograms,
    >>> appMgr.rewind(histos=False)
    """
    self.evtSel().rewind()
    if histos : 
        self.histSvc().reset()

AppMgr.rewind = _appMgr_rewind_
