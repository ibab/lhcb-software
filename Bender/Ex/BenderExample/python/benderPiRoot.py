#!/usr/bin/env python
# =============================================================================
# $Id: benderPiRoot.py,v 1.1 2004-02-13 08:50:17 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file   simple helper python script to load rootPlotter from PI
# @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
# @date   2004-02-11
# =============================================================================

print  'import rootPlotter from PI'
import rootPlotter

# create the plotter/canvas  
plotter = rootPlotter.RootPlotter('ROOT/PI Plotter for Bender') 
canvas  = plotter 

# =============================================================================
# The END 
# =============================================================================
