#!/usr/bin/env python
# =============================================================================
# @file DaVinciTools/momentum_smear.py
#
# Helper script to convert input data (TGraphi) to XML-format
#
# Input data are specified close to the start of the script
#
#  - input ROOT file with graph 'res'
#
#  As the output xml-file MomentumSmear.xml is generated in cwd
#  
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2013-04-20
#
# ============================================================================
"""

 Input data are specified close to the start of the script

  - input ROOT file with graph 'res'

  As the output xml-file MomentumSmear.xml is generated in cwd
  
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
filename  = 'smear12.root'
the_file = ROOT.TFile( directory + filename )
print 100*'*'
print 'HISTOS are read from ' , directory + filename
print the_file.ls()
print 100*'*'

#
# get the grah 
#
graph   = the_file.Get('res')

#
## convert graph to histo:
histo   = graph.GetHistogram () ## histogram layout
histo   = h1_axis ( histo.GetXaxis() , 'Momentum smear' , double = True ) 
histo  += graph                 ## histogram content 

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
    name    = 'Smear'  ,
    type    = 'Histo1D' ,
    comment = 'Momentum Smearing %s %s %s ' % ( sim , reco , year )
    )

document = DDDB(
    CONDITION (
    smear    ,
    ## comment = 'Momentum Smearing %s %s %s ' % ( sim , reco , year ),
    name    = 'MomentumSmear'
    ),
    )   

xml_name = 'MomentumSmear.xml'
xml_file = open ( xml_name ,'w')

print >> xml_file,  line1+'\n'+line2+'\n\n\n'+lxml.etree.tostring(document, pretty_print=True)

xml_file.close()
print 100*'*'
print 'XML file "%s" is generated in CWD ' % xml_name 
print 100*'*'


# =============================================================================
# The END 
# =============================================================================
