#!/usr/bin/env python
# =============================================================================
# $Id: bendermath.py,v 1.1 2005-02-02 19:15:10 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
"""
Simple module to import matehematicla functionf valid for *fuctions* and *cuts*
"""
__author__ = "Vanya BELYAEV belyaev@lapp.in2p3.fr"
# =============================================================================
# @file
#
# @date   2005-02-02
# @author Vanya BELYAEV belyaev@lapp.in2p3.fr
# =============================================================================

import gaudimodule

gaudimodule.loaddict('BenderDict')

# import functions:
exp  = gaudimodule.gbl.Bender.Math.exp

sin   = gaudimodule.gbl.Bender.Math.sin
asin  = gaudimodule.gbl.Bender.Math.asin
sinh  = gaudimodule.gbl.Bender.Math.sinh

cos   = gaudimodule.gbl.Bender.Math.cos
acos  = gaudimodule.gbl.Bender.Math.acos
cosh  = gaudimodule.gbl.Bender.Math.cosh

tan   = gaudimodule.gbl.Bender.Math.tan
atan  = gaudimodule.gbl.Bender.Math.atan
tanh  = gaudimodule.gbl.Bender.Math.tanh

sqrt  = gaudimodule.gbl.Bender.Math.sqrt
log   = gaudimodule.gbl.Bender.Math.log
log10 = gaudimodule.gbl.Bender.Math.log10

fabs  = gaudimodule.gbl.Bender.Math.fabs

atan2 = gaudimodule.gbl.Bender.Math.atan2

# pow  = gaudimodule.gbl.Bender.Math.pow

# =============================================================================
# The END 
# =============================================================================
