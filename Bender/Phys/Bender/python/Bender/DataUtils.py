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
"""Helper module to deal with data

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    

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
__all__     = ( 'extendfile1'  ,
                'extendfile2'  ,
                'extendfile'   ,
                'inCastor'     ,
                'inEOS'        ,
                'inGrid'       ,
                'hasGridProxy' ) 
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.DataUtils' )
else                      : logger = getLogger ( __name__ )
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
    
    import os 
    from subprocess import Popen, PIPE
    try:
        p   = Popen( [ 'nsls' , '-l' , fname ] , 
                     env    = os.environ       , 
                     stdout = PIPE             ,
                     stderr = PIPE             )
        stdout, stderr = p.stdout, p.stderr
    except OSError:
        ## nsls not in the path
        return False
    ## require empty stder
    for l in stderr : return False   ## RETURN 
    
    ## Require non-empty std-out: 
    for l in stdout : return True    ##  RETURN 
    
    return False 


# =============================================================================
_eos_   = 'root://eoslhcb.cern.ch//eos'
_eos_0  = 'root://eoslhcb.cern.ch/'
_eos_p  = '/eos/lhcb/grid/prod'
_eos_u  = '/eos/lhcb/grid/user'
_eos_f  = '/eos/lhcb/grid/prod/lhcb/freezer' ## Freezer 
_eosls  = 'eos_ls %s '
# =============================================================================
## check the presence of the file in (CERN) EOS
#
#  @code
#
#   >>> lfn = '/lhcb/Collisons11/......'
#   >>> ok = inEOS ( lfn ) ## check CERN EOS storage 
#
#  @endcode
#
#  @code
#
#   >>> fname = '/eos/...'
#   >>> ok = inCastor ( fname , '' ) ## check the explicit location
#
#  @endcode
#
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2012-10-08
#
#  @param fname   filename
#  @param prefix  prefix to be used
#
# how to use this pattern? 
## ['root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/freezer/lhcb/LHCb/Collision15/RDST/00048237/0000/00048237_00008700_1.rdst'
def inEOS ( fname            ,
            prefix  = _eos_0 ,
            eosls   = _eosls ) :
    """
    check if the file is accessible from castor:
    
    >>> lfn = '/lhcb/Collisons11/......'
    >>> ok = inEOS ( lfn   ) ## check CERN Grid Castor storage 
    
    >>> fname = '/eos/...'
    >>> ok = inEOS ( fname ) ## check the explicit location
    
    """
    # remove full
    if 0 == fname.find (          prefix ) : fname = fname [   len(prefix) : ]
    if 0 == fname.find ( 'mdf:' + prefix ) : fname = fname [ 4+len(prefix) : ]
    #
    fname0 = str(fname)
    # check short prefix
    if   0 == fname.find ( '/lhcb/user/' ) :
        fname = _eos_u + fname
    elif 0 == fname.find ( '/lhcb/LHCb/' ) or 0 == fname.find ( '/lhcb/MC/' ) or 0 == fname.find ( '/lhcb/validation/' ) :
        fname = _eos_p + fname
    elif 0 == fname.find (  'eos' ) : fname = '/'  + fname 
    elif 0 != fname.find ( '/eos' ) :
        fname = '/eos' + fname 
    ##
    import os 
    from subprocess import Popen, PIPE
    try:
        p   = Popen( [ 'eos_ls' , fname ] ,
                     env    = os.environ  ,
                     stdout = PIPE        ,
                     stderr = PIPE        )
        stdout, stderr = p.stdout, p.stderr
    except OSError:
        # eos_ls not in the path
        return ''
    #
    ## require empty stder
    #
    for l in stderr :
        ## make a try to freezer 
        if 0 == fname0.find('/lhcb/' ) :
            if fname0.endswith('.rdst') or fname0.endswith('.RDST') :
                return  inEOS ( _eos_f + fname0 , prefix , eosls )
        if 0 == fname0.find('/lhcb/user') :
            # try user EOS area : 
            return inEOS ( '/eos' + fname0 , prefix , eosls )
        return ''
    #
    ## Require non-empty std-out:
    #
    for l in stdout :
        ## play a bit with extension
        for ext in ( '.raw' , '.RAW' , '.mdf' , '.MDF' ) :
            if fname.endswith ( ext ) : 
                return 'mdf:' + prefix + fname               ## RETURN
            
        ## add prefix
        return prefix + fname                                ## RETURN 

    ## 
    return ''

# =============================================================================
## check the validity of GRID proxy 
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2012-10-10
def hasGridProxy ( ) : 
    """Check GRID proxy
    >>> hasGridProxy () 
    """
    import os
    from subprocess import Popen, PIPE
    arguments = ['lhcb-proxy-info' , '--checkvalid' ] 
    logger.verbose ( 'use Popen(%s)' % arguments )
    p   = Popen ( arguments              ,
                  env       = os.environ ,
                  stdout    = PIPE       ,
                  stderr    = None       )
    (cout,cerr) = p.communicate()
    #
    return 0 == p.returncode and cout 

# =============================================================================
## check the presence of file in Grid and get the access URL
#  @attention  valid GRID proxy is required 
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2012-10-10
def inGrid ( filename , grid = [] ) :
    """Check the presence of file in Grid and get the access URL
    Result depends on supplied 'grid' argument

    Get the access URL for certain GRID site:
    >>> inGrid('/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst','RAL')
    ... 'root://clhcbdlf.ads.rl.ac.uk//castor/ads.rl.ac.uk/prod/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst?svcClass=lhcbDst'

    Get the DICTIONARY of all access URLs for all GRID sites
    >>> inGrid('/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst')
    ... {'CERN_MC-DST-EOS': 'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst',
    ...  'IN2P3_MC-DST': 'root://ccdcacli067.in2p3.fr:1094/pnfs/in2p3.fr/data/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst',
    ...  'RAL_MC-DST': 'root://clhcbdlf.ads.rl.ac.uk//castor/ads.rl.ac.uk/prod/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst?svcClass=lhcbDst'}
    
    Get the access URL from GID site according to the priority list 
    >>> inGrid('/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst', ['RAL','IN2P3'] )
    ... 'root://clhcbdlf.ads.rl.ac.uk//castor/ads.rl.ac.uk/prod/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst?svcClass=lhcbDst'

    ditto (note the order of GRID sites in the list)
    >>> inGrid('/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst', [ 'IN2P3', 'RAL'] )
    ... 'root://ccdcacli067.in2p3.fr:1094/pnfs/in2p3.fr/data/lhcb/MC/2012/ALLSTREAMS.DST/00037532/0000/00037532_00000245_1.allstreams.dst'
    """

    if    grid is None                           : return None
    elif  grid is False                          : return None 
    elif  isinstance ( grid , str ) and not grid : grid = [] 
    elif  isinstance ( grid , str )              : grid = [ grid ]
    #
    ## 1-check lhcb-proxy-info
    #
    if not hasGridProxy () : return None ## LHCb-PROXY-INFO is invalid! 
    #
    
    #
    ## 2 - get the files from storage element 
    #
    import os 
    from subprocess import Popen, PIPE
    arguments =  [ 'get-grid-url', filename ]
    if grid   : arguments += ["--Sites" ] + grid
    logger.verbose('use Popen(%s)' % arguments )
    p   = Popen ( arguments               ,
                  env       = os.environ  ,
                  stdout    = PIPE        ,
                  stderr    = None        )
    (stdout,stderr) = p.communicate()
    if 0 != p.returncode  or stderr : return None
    #
    try :
        res = eval ( stdout )
    except:
        res = {}

    #
    if not isinstance  ( res, dict ) : return None
    if not res   : return None 
    #
    ## special case: return whole  disctioanry 
    if not grid : return res
    
    ## return the first matched entry accoring to priotity list of GRID sites 
    keys = res.keys()
    keys.sort() 
    for s in grid :
        su = s.upper() 
        for k in keys :
            ku = k.upper() 
            if 0 <= ku.find(su) : return res[k]

    ## return the first entry     
    for k in res :
        return res [k]
    #
    ## nothing to return 
    return None 

# =============================================================================
##  check the existence of the file with given name
#   optionally check the prefixes 
def fileExists ( filename ) :
    """Check the existence of the file with given name
    optionally check the prefixes using BENDERDATAPATH
    environment variable 
    """
    import os
    if os.path.exists ( filename ) and os.path.isfile ( filename ) :
        return filename
    ## 
    prfx = os.getenv('BENDERDATAPATH')
    if not prfx : return None 
    #
    prfx = prfx.split( os.pathsep )
    for p in prfx :
        #
        if not p or not filename : continue
        elif '/' == p[-1] and '/' == filename[0] : name = p[0:-1] + filename
        elif '/' != p[-1] and '/' != filename[0] : name = p + '/' + filename
        else                                     : name = p       + filename
        #
        if os.path.exists ( name ) and os.path.isfile ( name ) : return name

    return None
    
# =============================================================================
## Helper function to 'extend' the short file name
#
#  @thanks Philippe Charpentier for extremly clear explanation of
#          LHCb conventions for CASTOR pools and many CASTOR-related tricks
#
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2010-02-12
def extendfile1 ( filename , castor = False , grid = None ) :
    """Helper function to 'extend' the short file name 

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
        filename = 'PFN:root:' + filename

    elif 0 == filename.find ( '//castor'    ) :
        return extendfile1 ( filename[1:] , castor ) ## RECURSION!

    ## check existence of the file 
    elif fileExists ( filename   ) : filename = 'PFN:' + fileExists ( filename )
    
    elif 0 == filename.find ( '/lhcb/data/'       ) or \
         0 == filename.find ( '/lhcb/LHCb/'       ) or \
         0 == filename.find ( '/lhcb/MC/'         ) or \
         0 == filename.find ( '/lhcb/user/'       ) or \
         0 == filename.find ( '/lhcb/validation/' ) :
        
        if grid :
            #
            ## try to get the connection string from Grid
            #
            res = inGrid ( filename , grid )
            if res :
                filename = res
                return filename                         ## RETURN
            
        #
        ## check the file in EOS
        #
        eos = inEOS ( filename ) if castor else ''

        if   castor and eos     : filename = 'PFN:' + eos
        elif castor and inCastor ( filename ) :
            filename = extendfile1 ( '/castor/cern.ch/grid' + filename , castor , grid )
        else : 
            filename = 'LFN:' + filename

    else:
        
        eos = inEOS ( filename )
        if eos : filename = 'PFN:'+ eos
        
    ##
    return filename


_local_dict_ = {}
# =============================================================================
## Helper function to 'extend' the short file name
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2010-02-12
def extendfile2 ( filename       ,
                  castor = False ,
                  grid   = ''    ) :
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
    filename = extendfile1 ( filename , castor , grid ) 
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
        logger.verbose ( 'extendfile2: make use of IOHelper.setupServices() ')
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
## get list of input files from EventSelector.Input
#  @code
#  file_list = ... 
#  inputs = evtSelInput  ( file_list )
#  @endcode 
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2016-01-28
def evtSelInput ( files ) :
    """Get list of input files from EventSelector.Input
    >>> file_list = ... 
    >>> inputs = evtSelInput  ( file_list )( 
    """

    if not files : return []
    
    import os 
    from subprocess import Popen, PIPE
    try:
        p   = Popen( [ '_get_input_.py' ] + files , 
                     env    = os.environ          , 
                     stdout = PIPE                ,
                     stderr = PIPE                )
        stdout, stderr = p.stdout, p.stderr
    except :
        logger.error("Can't get input data from conf-files: %f" % files ) 
        return []                                                  ## RETURN 
    ## require empty stder
    for l in stderr :
        logger.error("Can't get input data from conf-files: %f" % files ) 
        return []                                                  ## RETURN

    flist = []  
    ## Require non-empty std-out: 
    for line in stdout :
        
        if not line : continue
        iline = line.strip()
        if not iline  or '#' == iline[0] : continue
        
        try :
            flist = flist + eval ( iline )
        except :
            pass

    flist = set ( flist ) 
    return list ( flist )  

# =============================================================================
if __name__ == '__main__' :

    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) ) 
    logger.info ( 80*'*'  ) 
    

# =============================================================================
# The END 
# =============================================================================
