#!/usr/bin/env python2.4 
# =============================================================================
# $Id: data_tutorial.py,v 1.2 2007-05-31 11:29:45 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.2 $
# =============================================================================
"""
//-- GAUDI data cards generated on Nov 9, 2006 11:49:47 AM
//-- For Event Type = 13144000 / Data type = DST 1
//--     Configuration = DC06 - v1-lumi2
//--     DST 1 datasets output 
//--     From DIGI 1 datasets produced by ANY
//--     From SIM 1 datasets produced by ANY
//--     Database version = ANY
//--     Cards content = physical
//--     
"""
# =============================================================================
## @file
#  Auxillary module  to define input dta for Bender tutorial
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

## use castor tapes at CERN 
prefix = "PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/v1-lumi2/00001395/DST/0000/"

## for Bender tutorial in Dortnund University use local data 
import os
if 'UNI_DORTMUND' == os.environ.get('CMTSITE','') :
    prefix = '/software/lhcb/BenderData/Bs2PsiPhi_mm/'
    
## list of files     
_FILES = [
    '00001395_00000005_5.dst'  ,
    '00001395_00000007_5.dst'  ,
    '00001395_00000008_5.dst'  ,
    '00001395_00000010_5.dst'  ,
    #
    '00001395_00000012_5.dst'  ,
    '00001395_00000013_5.dst'  ,
    '00001395_00000014_5.dst'  ,
    '00001395_00000015_5.dst'  ,
    #
    '00001395_00000017_5.dst'  ,
    '00001395_00000018_5.dst'  ,
    '00001395_00000019_5.dst'  ,
    '00001395_00000020_5.dst'  ,
    #
    '00001395_00000021_5.dst'  ,
    '00001395_00000022_5.dst'  ,
    '00001395_00000023_5.dst'  ,
    '00001395_00000025_5.dst'  ,
    #
    '00001395_00000026_5.dst'  ,
    '00001395_00000027_5.dst'  ,
    '00001395_00000029_5.dst'  ,
    '00001395_00000030_5.dst'  ]

## the actual list of files
FILES = []
for file in _FILES :
    import os,sys
    #
    if 'win32' == sys.platform : local = 'C:\\Data\\JpsiPhi\\'    # my Windows laptop
    else: local = '/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/' # "prestage" at CERN AFS
    #
    if os.path.exists( local + file ) : FILES += [ local  + file ]
    else : FILES += [ prefix + file ] # use CASTOR

if __name__ == '__main__' :
    print '  #FILES to be used for BENDER Tutotial : %s'%len(FILES)
    for file in FILES : print file 


# =============================================================================
# The END 
# =============================================================================
