#!/usr/bin/env python
# =============================================================================
# $Id: benderROOT.py,v 1.1 2004-02-13 08:50:17 ibelyaev Exp $
# =================================================================================
# CVS tag $Name: not supported by cvs2svn $
# =================================================================================
# @file   simple helper python script to simplify teh usage or SEAL's 
# @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
# @date   2004-02-11
# =============================================================================

import PyLCGDict
### initialize root -------------------------------------------------------------
print 'Loading ROOT dictionaries.....'
PyLCGDict.loadDict('SealROOTDict')
print 'Starting ROOT application.....'
gApplication = PyLCGDict.makeClass('ROOTApplication')('ROOTApp')
TGlobals = PyLCGDict.makeClass('TGlobals')
gSystem = TGlobals.System()
gROOT = TGlobals.ROOT();
gSystem.Load('libPhysics')
gSystem.Load('libTree')
### root thread -----------------------------------------------------------------
import thread, time
def _processRootEvents():
  while True:
    gSystem.ProcessEvents()
    time.sleep(0.1)
print 'Starting ROOT thread.....'
thread.start_new_thread( _processRootEvents, () )

TCanvas  = PyLCGDict.makeClass ( 'TCanvas' )
TH1D     = PyLCGDict.makeClass ( 'TH1D'    )
TH2D     = PyLCGDict.makeClass ( 'TH2D'    )
TH3D     = PyLCGDict.makeClass ( 'TH3D'    )

gROOT.Reset()


def _plot_( self , obj ) :
    if not hasattr( obj ,'Draw' ) :
        raise RuntimeError, "TCanvas: 'obj' has not attribute 'Draw' "
    obj.Draw() 
    self.Modified()
    self.Update()
    
TCanvas.plot = _plot_
    
plotter = TCanvas()
canvas  = plotter
