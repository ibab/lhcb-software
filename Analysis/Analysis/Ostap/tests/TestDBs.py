#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file TestDBs.py
#
#  tests for data storages 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2014-06-06
# 
#                    $Revision: 173870 $
#  Last modification $Date: 2014-06-14 13:44:41 +0200 (Sat, 14 Jun 2014) $
#                 by $Author: ibelyaev $
# =============================================================================
"""Tests for various data storages 
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-06-21"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT, random 
from   Ostap.PyRoUts import *
from   Ostap.Utils   import rooSilent, timing 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__ or '__builtin__' == __name__ : 
    logger = getLogger( 'Ostap.TestDBs' )
else : 
    logger = getLogger( __name__ )
# =============================================================================
import os,tempfile
#
db_sql_name = tempfile.mktemp ( suffix = '.sqldb' ) 
db_zip_name = tempfile.mktemp ( suffix = '.zipdb' )
#
# =============================================================================
import Ostap.ZipShelve    as ZipShelve
import Ostap.SQLiteShelve as SQLiteShelve
#
bins = 500
data    = {}
h1      = ROOT.TH1D('h1','1D-histogram',bins,-5,5) ; h1.Sumw2() 
m1      = VE(1,2)
for i in range ( 0, 100000) : h1.Fill( m1.gauss() )
h2      = ROOT.TH2D('h2','2D-histogram',bins,-5,5,bins,-5,5) ; h2.Sumw2() 
for i in range ( 0, 100000) : h2.Fill( m1.gauss() , m1.gauss() )

data [ 'histo-1D' ] = h1
data [ 'histo-2D' ] = h2

db_sql  = SQLiteShelve.open    ( db_sql_name , 'c' )
db_zip  = ZipShelve.open       ( db_zip_name , 'c' )
#
#
for k in data :
    db_sql [ k ] = data[k]
    db_zip [ k ] = data[k]

logger.info('SQLiteShelve keys: %s' % db_sql.keys() )
logger.info('ZipShelve    keys: %s' % db_zip.keys() )

db_sql.close() 
db_zip.close()

logger.info('SQLiteShelve size: %d ' % os.path.getsize( db_sql_name ) )
logger.info('ZipShelve    size: %d ' % os.path.getsize( db_zip_name ) )

db_sql  = SQLiteShelve.open    ( db_sql_name , 'r' )
db_zip  = ZipShelve.open       ( db_zip_name , 'r' )

with timing ( 'h1-read/SQL' ) : h1_sql = db_sql [ 'histo-1D']
with timing ( 'h1-read/ZIP' ) : h1_zip = db_sql [ 'histo-1D']
with timing ( 'h2-read/SQL' ) : h2_sql = db_sql [ 'histo-2D']
with timing ( 'h2_read/ZIP' ) : h2_zip = db_sql [ 'histo-2D']

for i in h1_sql : 
    v = h1_sql[i] - h1_zip[i] 
    if not iszero ( v.value() ) :
        logger.error('Large difference for 1D histogram(1)!')
    v = h1_sql[i] - h1    [i] 
    if not iszero ( v.value() ) :
        logger.error('Large difference for 1D histogram(2)!')
        
for i in h2_sql : 
    v = h2_sql[i] - h2_zip[i] 
    if not iszero ( v.value() ) :
        logger.error('Large difference for 2D histogram(1)!')
    v = h2_sql[i] - h2    [i] 
    if not iszero ( v.value() ) :
        logger.error('Large difference for 2D histogram(2)!')

with timing('Close SQL') : db_sql.close() 
with timing('Close ZIP') : db_zip.close()

with timing('Remove SQL') : os.remove ( db_sql_name )
with timing('Remove ZIP') : os.remove ( db_zip_name )


    
# =============================================================================
# The END 
# =============================================================================
