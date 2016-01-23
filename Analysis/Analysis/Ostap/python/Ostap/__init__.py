#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#
#  Helper module to load various Ostap-modules
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-03-10
# =============================================================================
"""
Helper module to load various Ostap-modules

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

""" 
# =============================================================================
__author__  = "Vanya BELYAEV  Ivan.Belyaev@itep.ru"
__date__    = "2014-03-10"
__version__ = "$Revision$"
# =============================================================================
import ROOT, os 
#
try:
    import cppyy
except ImportError:
    # FIXME: backward compatibility
    print "# Ostap WARNING: using PyCintex as cppyy implementation"
    import PyCintex as cppyy
    import sys
    sys.modules['cppyy'] = cppyy
#
cpp = cppyy.makeNamespace('')
##if not hasattr ( ROOT    , 'ostream' ) :
##    print "# Ostap WARNING: force loading of <ostream>"
##    ROOT.gROOT.ProcessLine("#include <ostream>")

# =============================================================================
workdir = os.environ.get('OSTAP_DIR') or '$HOME/.ostap'
workdir = os.path.expandvars(workdir)

if not os.path.exists(workdir):
    try : 
        os.mkdir(workdir)
        os.mkdir(os.path.join(workdir, "cache"))
    except :
        pass 


# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120

# =============================================================================
# The END 
# =============================================================================
