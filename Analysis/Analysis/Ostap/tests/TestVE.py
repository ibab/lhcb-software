#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Test for ROOT6 problem with copy-constructor and cast-operators 
#  @see https://sft.its.cern.ch/jira/browse/ROOT-6627
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-08-27
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Test for ROOT6 problem with copy-constructor and cast-operators 

see https://sft.its.cern.ch/jira/browse/ROOT-6627

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
# =============================================================================
import ROOT
from   AnalysisPython.Logger import getLogger
logger = getLogger('TestVE')


VE = ROOT.Gaudi.Math.ValueWithError
a  = VE(1,1)

logger.info ( 'a = %s +- %s ' % (  a.value() , a.error() ) ) 


b  = VE(a)
logger.info (  'b = %s +- %s ' % (  b.value() , b.error() ) ) 

jira = 'https://sft.its.cern.ch/jira/browse/ROOT-6627'
if a.error() != b.error () :
    logger.error('Wrong result, the problem %s is not solved yet ' % jira )
    logger.error('ROOT Version %s' % ROOT.gROOT.GetVersion() ) 
    raise TypeError('Values are not equal: %s != %s' % ( a.error () , b.error () ) )



# =============================================================================
# The END 
# =============================================================================
