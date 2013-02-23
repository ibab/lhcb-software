#!/usr/bin/env python 
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderTools/Parser.py
#
#  Trivial Parser for Bender-based scripts
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  <b>``C++ ToolKit for Smart and Friendly Physics Analysis''</b>
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
#
#  @date   2010-09-10
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Trivial Parser for Bender-based scripts

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''


                  $Revision$
Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = "2010-09-10"
__version__ = '$Revision$'
__all__     = ( 'makeParser' ) ## nothing to import 
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
## create the parser
def makeParser ( usage = None ,
                 vers  = None ) :
    """
    Create the parser 
    """
    #
    if not usage : usage = __usage__
    if not vers  : vers  = __version__
    #
    from optparse import OptionParser as OptParser
    parser = OptParser( usage   = usage            ,
                        version = ' %prog ' + vers )
    
    ##
    parser.add_option (
        '-t'                  ,
        '--datatype'          ,
        dest    = 'DataType'  ,
        type    = 'str'       , 
        help    = "``DataType''    attribute to be specified for DaVinci [default : %default]" ,
        default = '2011'  
        )
    ## 
    parser.add_option (
        '-s'                          ,
        '--simulation'                ,
        action  = "store_true"        ,
        dest    = 'Simulation'        ,
        help    = "``Simulation''  flag to be propagated to DaVinci" ,
        default = False   
        )
    ## 
    parser.add_option (
        '-g'                       ,
        '--grid'                   ,
        type    = 'str'            , 
        dest    = 'Grid'           ,
        help    = "Grid-site to access LFN-files (has precedence over -c, but grid proxy is needed)" ,
        default = 'CERN'
        )
    ## 
    parser.add_option (
        '-c'                          ,
        '--castor'                    ,
        action  = "store_true"        ,
        dest    = 'Castor'            ,
        help    = "Enable direct access to Castor/EOS Grid Storage to access LFN-files" ,
        default = True   
        )
    ##
    parser.add_option (
        '-p'                          ,
        '--print'                     ,
        type    = 'int'               , 
        dest    = 'OutputLevel'       ,
        help    = "``OutputLevel'' attribute for ApplicationMgr/MessageSvc [default : %default]" ,
        default = 3                  
        )
    ## 
    parser.add_option (
        '-m'                          ,
        '--micro'                     ,
        action  = "store_true"        ,
        dest    = 'MicroDST'          ,
        help    = "MicroDST format?"  ,
        default = False   
        )
    ##
    parser.add_option (
        '-x'                           ,
        '--xml'                        ,
        dest    = 'XmlCatalogue'       ,
        help    = "``XmlCatalog'' to be transferred to setData-function [default : %default]" ,
        default = ''                  
        )
    ## 
    parser.add_option (
        '-q'                          ,
        '--quiet'                     ,
        action  = "store_true"        ,
        dest    = 'Quiet'             ,
        help    = "``Quiet'' processing"  ,
        default = False   
        )
    ## 
    parser.add_option (
        '-r'                     ,
        '--root'                 ,
        type    = 'str'          ,
        dest    = 'RootInTES'    ,
        help    = 'Root-In-TES'  ,
        default = ''           
        )
    ##
    parser.add_option (
        '-u'                       ,
        '--useoracle'              ,
        action  = "store_true"     ,
        dest    = 'UseOracle'      ,
        help    = "Use Oracle-DB"  ,
        default = False   
        )
    ##
    parser.add_option (
        '-l'                       ,
        '--lumi'                   ,
        action  = "store_true"     ,
        dest    = 'Lumi'           ,
        help    = "Use Lumi?"      ,
        default = False   
        )
    ##
    return parser

# =============================================================================
## try to extract the data type, simulation and input type  
def dataType ( files ) :
    """
    extract the data type,
    simulaton type and input type from file name 
    """
    #
    if isinstance ( files  , str ) : files = [ files ]
    #
    dtype = ''
    simu  = False
    ext   = '' 
    for f in files  :
        if   0 <= f.find ( 'Collision09' ) : dtype = '2009'
        elif 0 <= f.find ( 'Collision10' ) : dtype = '2010'
        elif 0 <= f.find ( 'Collision11' ) : dtype = '2011'
        elif 0 <= f.find ( 'Collision12' ) : dtype = '2012'
        elif 0 <= f.find ( 'MC09' ) :
            dtype = '2009'
            simu  = True 
        elif 0 <= f.find ( 'MC10' ) :
            dtype = '2010'
            simu  = True 
        elif 0 <= f.find ( 'MC11' ) :
            dtype = '2011'
            simu  = True 
        elif 0 <= f.find ( 'MC12' ) :
            dtype = '2012'
            simu = True

        p   = f.rfind ( '.' )
        if 0 <= p :
            ## allow up to 5 symbols for exension 
            if len ( f[p+1:] ) <= 5 : ext = f[p+1:]
            
    return  dtype,simu,ext.upper() 

# =============================================================================
## try to extract more information from file names 
def hasInFile ( files , pattern ) :
    """
    extract the data type,
    simulaton type and input type from file name 
    """
    #
    if isinstance ( files  , str ) : files = [ files ]
    #
    for f in files  :
        if   0 <= f.find ( pattern ) : return True
    return False

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
