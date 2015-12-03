#!/usr/bin/env python
# =============================================================================
# $Id:$
# =============================================================================
# @file DaVinciTools/momentum_scale.py
#
# Helper script to convert input data (histograms + offsets) to XML-format
#
# Input data are specified close to the start of the script:
#
#  - input ROOT file with historams
#     the file should have two 2D-histograms, named as 'idp-plus' and 'idp-minus'
#     
#  - global delta
#  - list of run-dependent offsets as list of triplets:  [ (start,stop,offset) ],
#     where (start,stop) represent the run range (both edges are inclusive) 
#
#  As the output xml-file MomentumScale.xml is generated in cwd
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2013-04-20
# @see TrackScaleState
#
#                   $Revision:$
# Last modification $Date:$
#                by $Author:$
#
# ============================================================================
"""
Helper script to convert input data (histograms + offsets) to XML-format

Input data are specified close to the start of the script

  - input ROOT file with historams
     the file should have two 2D-histograms, named as 'idp-plus' and 'idp-minus'
     
  - global delta
  - list of run-dependent offsets as list of triplets:  [ (start,stop,offset) ],
     where (start,stop) represent the run range (both edges are inclusive) 

  As the output xml-file MomentumScale.xml is generated in cwd
  

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
from   Ostap.PyRoUts import h1_axis,VE


# =============================================================================
## get the data 
# =============================================================================
#
year      = '2k+11'
reco      = 'Reco14'
#
directory = '/afs/cern.ch/user/m/mneedham/public/2011-repro-Calib/'
filename  = 'BFitResults-2011repro.root'
the_file = ROOT.TFile( directory + filename )
print 100*'*'
print 'HISTOS are read from ' , directory + filename
print the_file.ls()
print 100*'*'

delta = 1.0e-4 
print 'global DELTA is defined as %s ' % delta 

#
# get histograms
# 
idp_plus  = the_file.Get('idp-plus' )
idp_minus = the_file.Get('idp-minus')

# =============================================================================
## CalScale.h: 
##   if (run >= 87219  && run <=  89777 ) alpha =0.0; 
##   else if (run >= 89333  && run <=  90256 ) alpha = 6.36699e-05  ;
##   else if (run >= 90257  && run <=   90763  ) alpha = 7.11719e-05 ;
##   else if (run >= 91556 && run <= 93282)  alpha = 1.53674e-05 ;
##   else if (run >= 93398  && run <=  94386 ) alpha = 0.000144135 ; 
##   else if (run >= 95948  && run <=  97028 ) alpha = 0.000214408  ;
##   else if (run >= 97114  && run <=  98882 ) alpha = 3.41493e-05 ;
##   else if (run >= 98900  && run <=  101862 ) alpha =  0.000137622;
##   else if (run >= 101891  && run <=  102452 ) alpha = 3.73981e-05 ;
##   else if (run >= 102499  && run <=  102907 ) alpha = 0.000169023 ;
##   else if (run >= 103049  && run <=  103687 ) alpha =  3.20303e-05 ;
##   else if (run >= 103954  && run <=  104414 ) alpha =  0.00017584 ;

offsets = [
    ## start   stop     offset 
    (  87219 ,  89777 , 0.0         ) ,
    (  89333 ,  90256 , 6.36699e-05 ) ,
    (  90257 ,  90763 , 7.11719e-05 ) , 
    (  91556 ,  93282 , 1.53674e-05 ) , 
    (  93398 ,  94386 , 0.000144135 ) , 
    (  95948 ,  97028 , 0.000214408 ) , 
    (  97114 ,  98882 , 3.41493e-05 ) , 
    (  98900 , 101862 , 0.000137622 ) , 
    ( 101891 , 102452 , 3.73981e-05 ) , 
    ( 102499 , 102907 , 0.000169023 ) ,
    ( 103049 , 103687 , 3.20303e-05 ) , 
    ( 103954 , 104414 , 0.00017584  ) ,
    ]


# ============================================================================
## convert run offsets into the histogram
# @param offset the list of run-dependes offsets: [ (start,stop,offset) ]
# @param title (optional) the histogram title
# @code 
#   offsets_2011 = [
#     ## start   stop     offset 
#     ( 89333  , 90256  , 9.97629e-05 ) ,
#     ( 90257  , 90763  , 6.01683e-06 ) , 
#     ( 91556  , 93282  , 5.33486e-05 ) , 
#     ( 93398  , 94386  , 0.000183575 ) , 
#     ]   
#   histo = offsets_to_histo ( offets_2011 )
# @endcode
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2013-04-20
#
def offsets_to_histo ( offsets , title =  '' ) :
    """
    Helper function to convert the offset list into histogram
    
    offsets_2011 = [
    ## start   stop     offset 
    ( 89333  , 90256  , 9.97629e-05 ) ,
    ( 90257  , 90763  , 6.01683e-06 ) , 
    ( 91556  , 93282  , 5.33486e-05 ) , 
    ( 93398  , 94386  , 0.000183575 ) , 
    ]
    
    histo = offsets_to_histo ( offets_2011 ) 
    
    """
    #
    # make the axis ticks 
    ticks = set()
    for i in offsets :
        ticks.add ( i[0]     )
        ticks.add ( i[1] + 1 ) ## take care about equality 
        
    ## create the histogram with appropriate biunings 
    h = h1_axis ( list ( ticks )  , double = True , title = title )
        
    for i in offsets :
        mid = 0.5*( i[0] + i[1] )
        bin = h.findBin ( mid ) 
        h [ bin ] = VE( i[2] , 0 )

    return h 


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

xml_delta     = PARAM (
    str ( delta ),
    name    = 'Delta'              ,
    type    = 'double'             ,
    comment = 'Global delta scale         %s %s' % ( reco , year )
    )

xml_idplus    = PARAM (
    idp_plus.toString() , 
    name    = "IdpPlus" ,
    type    = "Histo2D" ,
    comment = "Calibration for IdpPlus    %s %s" % ( reco , year )
    )

xml_idpminus = PARAM (
    idp_minus.toString () ,
    name    = "IdpMinus" ,
    type    = "Histo2D"  ,
    comment = "Calibration for IdpMinus   %s %s" % ( reco , year )
    )

xml_offsets = PARAM (
    offsets_to_histo ( offsets , 'Run-dependent offsets' ).toString() , 
    name    = "Offsets" ,
    type    = "Histo1D" ,
    comment = "Run-dependent offsets     %s %s" % ( reco , year )
    )

document = DDDB(
    CONDITION (
    xml_delta    ,
    xml_idplus   ,
    xml_idpminus ,
    xml_offsets  ,
    ## comment = "Momentum scale            %s %s" % ( reco , year ) ,
    name    = 'MomentumScale' 
    ),
    )   


xml_name = 'MomentumScale.xml'
xml_file = open ( xml_name ,'w')

print >> xml_file,  line1+'\n'+line2+'\n\n\n'+lxml.etree.tostring( document , pretty_print=True)

xml_file.close()
print 100*'*'
print 'XML file "%s" is generated in CWD ' % xml_name 
print 100*'*'


# =============================================================================
# The END 
# =============================================================================
