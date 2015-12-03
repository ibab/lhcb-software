#!/usr/bin/env python
# =============================================================================
# @file DaVinciTools/covariance_scale.py
#
# Helper script to convert input data histogram to XML-format
#
# Input data are specified close to the start of the script
#
#  - input ROOT file with histogram 'CovScale'
#
#  As the output xml-file CovarianceScale.xml is generated in cwd
#  
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2013-04-20
#
# ============================================================================
"""

 Input data are specified close to the start of the script

  - input ROOT file with histogram 'CovScale'

  As the output xml-file CovarianceScale.xml is generated in cwd
  
"""
# ============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2013-04-20"
__version__ = "$Revision:$"
# ============================================================================
print 100*'*'
print __doc__
print 100*'*'
# ============================================================================
import ROOT 
from   Ostap.PyRoUts import hID,h1_axis 

# =============================================================================
## get the data 
# =============================================================================
#
year      = '2k+12'
reco      = '????'
sim       = '????'
#
directory = '/afs/cern.ch/user/i/ibelyaev/public/'
filename  = 'covscale.root'
the_file  = ROOT.TFile( directory + filename )
print 100*'*'
print 'HISTOS are read from ' , directory + filename
print the_file.ls()
print 100*'*'

#
histo  = the_file.Get('CovScale')
#

# =============================================================================
# Build XML-document 
# =============================================================================

## 
line1 = '<?xml version="1.0" encoding="ISO-8859-1"?>'
line2 = '<!DOCTYPE DDDB SYSTEM "../../../DTD/structure.dtd">'
##

## import lxml.etree
import lxml.builder    

E         = lxml.builder.ElementMaker()
DDDB      = E.DDDB
CONDITION = E.condition
PARAM     = E.param


smear     = PARAM (
    histo. toString() , 
    name    = 'Scale'   ,
    type    = 'Histo1D' ,
    comment = 'Covariance scale %s %s %s ' % ( sim , reco , year ) ,
    )

document = DDDB(
    CONDITION (
    smear    ,
    ## comment = 'Covariance scale %s %s %s ' % ( sim , reco , year ) ,
    name    = 'CovarianceScale' ,
    ),
    )   


xml_name = 'CovarianceScale.xml'
xml_file = open ( xml_name ,'w')

print >> xml_file,  line1+'\n'+line2+'\n\n\n'+lxml.etree.tostring(document, pretty_print=True)

xml_file.close()
print 100*'*'
print 'XML file "%s" is generated in CWD ' % xml_name 
print 100*'*'


# =============================================================================
# The END 
# =============================================================================
