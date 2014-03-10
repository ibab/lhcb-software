#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with some commono stuff for AnalysisPython package 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-02-10
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Module with some common stuff for AnalysisPython package 
"""
# =============================================================================
import ROOT
#
try:
    import cppyy
except ImportError:
    # FIXME: backward compatibility
    print "# AnalysisPython WARNING: using PyCintex as cppyy implementation"
    import PyCintex as cppyy
    import sys
    sys.modules['cppyy'] = cppyy
#
cpp = cppyy.makeNamespace('')
if not hasattr ( ROOT    , 'ostream' ) :
    print "# AnalysisPython WARNING: force loading of <ostream>"
    ROOT.gROOT.ProcessLine("#include <ostream>")

# =============================================================================
# The END 
# =============================================================================
