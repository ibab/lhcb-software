#!/usr/bin/env python
# =============================================================================
# @file Bender/DataUtils.py
#
# Helper module to deal with data
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
# @author VanyaBELYAEV Ivan.Belyaev@nikhef.nl
# @date   2010-02-12
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""
Helper module to deal with data

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
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2010-02-12"
__version__ = "$Revision$"
__all__     = ( 'extendfile1' ,
                'extendfile2' ,
                'extendfile'  ,
                'inCastor'    ) 
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
_castor = '/castor/cern.ch/grid'
_nsls   = 'nsls -l %s'
# =============================================================================
## check the presence of the file in (CERN) Castor
#
#  @code
#
#   >>> lfn = '/lhcb/Collisons11/......'
#   >>> ok = inCastor ( lfn ) ## check CERN Grid Castor storage 
#
#  @endcode
#
#  @code
#
#   >>> fname = '/castor/cern.ch/......'
#   >>> ok = inCastor ( fname , '' ) ## check the explicit location
#
#  @endcode
#
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2011-09-26
#
#  @param fname   filename
#  @param prefix  prefix to be used
#
def inCastor ( fname            ,
               prefix = _castor ,
               nsls   = _nsls   ) :
    """
    check if the file is accessible from castor:

    >>> lfn = '/lhcb/Collisons11/......'
    >>> ok = inCastor ( lfn ) ## check CERN Grid Castor storage 

    >>> fname = '/castor/cern.ch/......'
    >>> ok = inCastor ( fname , '' ) ## check the explicit location
    
    """

    if 0 != fname.find ( prefix ) : fname = prefix + fname
    
    ## use nsls command to check the file existence 
    cmd = nsls % fname
    
    try:

        ## 
        from subprocess import Popen, PIPE
        p   = Popen( cmd                 ,
                     shell     = True    ,
                     stdin     = PIPE    ,
                     stdout    = PIPE    ,
                     stderr    = PIPE    ,
                     close_fds = True    )
        stdin, stdout, stderr = p.stdin, p.stdout, p.stderr

        ## require empty stder
        for l in stderr : return False   ## RETURN 
        
        ## Require non-empty std-out: 
        for l in stdout : return True    ##  RETURN 

    except :
        logger.error('inCastor: failure to use Popen, return False')
        pass
    
    return False 


# =============================================================================
## Helper function to 'extend' the short file name
#
#  @thanks Philippe Charpentier for extremly clear explanation of
#          LHCb conventions for CASTOR pools and many CASTOR-related tricks
#
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2010-02-12
def extendfile1 ( filename , castor = False ) :
    """
    Helper function to 'extend' the short file name 

    Thanks to Philippe Charpentier for extremly clear explanation of
           LHCb conventions for CASTOR pools and many CASTOR-related tricks

    """
    ## the name is already decorated:
    if 0 <= filename.find (' ') : return filename
    ##
    import os 
    ## 
    if   0 == filename.find ( '/castor/cern.ch/grid/lhcb/user/' ) :
        filename = 'PFN:root://castorlhcb.cern.ch/%s?svcClass=lhcbuser' % filename

    elif 0 == filename.find ( '/castor/cern.ch/grid/lhcb/data/' )             or \
         0 == filename.find ( '/castor/cern.ch/grid/lhcb/LHCb/Collision10' )  or \
         0 == filename.find ( '/castor/cern.ch/grid/lhcb/LHCb/Collision11' )  or \
         0 == filename.find ( '/castor/cern.ch/grid/lhcb/LHCb/Collision12' )  :
        
        _len = len(filename) 
        if   _len - 4  == filename.find ( '.raw'  ) :
            filename = 'PFN:root://castorlhcb.cern.ch/%s?svcClass=lhcbtape' % filename
        elif _len - 5  == filename.find ( '.rdst' ) :
            filename = 'PFN:root://castorlhcb.cern.ch/%s?svcClass=lhcbtape' % filename
        else:
            filename = 'PFN:root://castorlhcb.cern.ch/%s?svcClass=lhcbdisk' % filename

    elif 0 == filename.find ( '/castor/cern.ch/grid/lhcb/MC/'        ) :
        filename = 'PFN:root://castorlhcb.cern.ch/%s?svcClass=lhcbdisk' % filename

    elif 0 == filename.find ( '/castor/cern.ch/grid/lhcb/validation/' ) :
        filename = 'PFN:root://castorlhcb.cern.ch/%s?svcClass=lhcbdisk' % filename

    elif 0 == filename.find ( '/castor/cern.ch/grid/lhcb/'      ) :
        filename = 'PFN:root://castorlhcb.cern.ch/%s'                   % filename

    elif 0 == filename.find ( '/castor/cern.ch'                 ) :
        filename = 'PFN:castor:' + filename

    elif 0 == filename.find ( '//castor'    ) :
        return extendfile1 ( filename[1:] , castor ) ## RECURSION!
    
    elif os.path.exists ( filename )          : filename = 'PFN:' + filename
    
    elif 0 == filename.find ( '/lhcb/data/'       ) or \
         0 == filename.find ( '/lhcb/LHCb/'       ) or \
         0 == filename.find ( '/lhcb/MC/'         ) or \
         0 == filename.find ( '/lhcb/user/'       ) or \
         0 == filename.find ( '/lhcb/validation/' ) :

        if castor and inCastor ( filename ) :
            filename = extendfile1 ( '/castor/cern.ch/grid' + filename , castor ) 
        else : 
            filename = 'LFN:' + filename

            
    ##
    return filename


_local_dict_ = {}
# =============================================================================
## Helper function to 'extend' the short file name
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2010-02-12
def extendfile2 ( filename , castor = False ) :
    """
    Helper function to 'extend' the short file name 
    """
    ##
    from GaudiPython.Bindings import _gaudi
    ##
    if 0 <= filename.find ( ' '         ) : return filename
    if 0 <= filename.find ( 'DATAFILE=' ) : return filename
    ##
    ## @see extendfile1 
    filename = extendfile1 ( filename , castor ) 
    ##
    ##
    #
    from GaudiConf import IOHelper
    #
    fname_upper = filename.upper()
    #
    if 0 < fname_upper.find ( '.MDF' ) or \
       0 < fname_upper.find ( '.RAW' ) :
        ioh = IOHelper ( Input = 'MDF'  , Output = 'ROOT' )
    else :
        ioh = IOHelper ( Input = 'ROOT' , Output = 'ROOT' )
        #
    iohstr = str(ioh) 
    if not _gaudi and not _local_dict_.has_key( iohstr ) : 
        logger.info('make use of IOHelper.setupServices() ')
        ioh.setupServices ()
        _local_dict_[ iohstr ] = 1
        ##
    #
    return ioh.dressFile ( filename , 'I')

# =============================================================================
## Helper function to 'extend' the short file name
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2010-02-12
extendfile  = extendfile2

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
