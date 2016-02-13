#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## The trivial startup sctript for python Bender session
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
#  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
#  @date   2006-10-08
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""This is a trivial startup script for python Bender session

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
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2006-10-08"
__version__ = "$Revision$"
__all__     = () 
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'Bender.Startup' )
else                      : logger = getLogger ( __name__ )
# =============================================================================

def with_ipython() :
    try :
        __IPYTHON__
        return True
    except NameError :
        return False
    
## check if the file is actually "empty"
def _empty_ ( fname ) :
    
    import os 
    if not os.path.exists ( fname ) or 0 == os.path.getsize( fname ) : return True

    try :
        
        ## find at least one non-empty line not starting from '#'
        with open ( fname , 'r' ) as f :
            for l in f :
                if not l  or  '#' == l [0] : continue
                l1 = l.strip()
                if     l1 and '#' != l1[0] : return False 
            
        return True
        
    except IOError :
        pass
     

import datetime
start_time = datetime.datetime.now()
logger.info ( 'Bender session started %s' % start_time.strftime('%c')  )

import os,sys
## define the name of the history file
__history__ = os.path.curdir + os.sep + '.bender_history'

def _rename_ ( base , version = 0  ) :
    
    if version <= 0 :
        version = 0 
        fname   = '%s'    %    base 
        fnamep1 = '%s.%d' %  ( base , 1 )
    else :
        fname   = '%s.%d' %  ( base , version     )
        fnamep1 = '%s.%d' %  ( base , version + 1 )
        
    if os.path.exists ( fname ) and _empty_ ( fname ) :
        os.remove ( fname )
        return
    
    if os.path.exists ( fnamep1 ) :
        if  _empty_   ( fnamep1 ) : os.remove ( fnamep1 )
        else : _rename_ ( base , version + 1 )
        
    if os.path.exists ( fname ) :
        if _empty_    ( fname ) : os.remove ( fname )
        else : os.rename ( fname , fnamep1 )
        
                
## write history at the end 
def _prnt_() :
    end_time = datetime.datetime.now()   
    logger.info ( 'Bender session   ended %s' % end_time.strftime('%c')  )

    
## line completer 
import rlcompleter
import readline
readline.clear_history() 
readline.parse_and_bind("tab: complete")

## write history
def write_history ( fname ) :
    """Write history file 
    """
    ## first, delete old history file
    try :        
        _rename_ ( __history__  )
    except :
        logger.warning ( "Can't erase old history file(s)", exc_info = True ) 

    end_time = datetime.datetime.now()   
    command  = [ a for a in sys.argv ]
    if command : command[0] = os.path.basename( command[0] )
    command  = ' '.join(command)
    curdit   = os.path.curdir
    
    if with_ipython() :
        
        try :
            
            import IPython, getpass
            ip  = IPython.get_ipython()
            me  = getpass.getuser() 
            with open ( fname , 'w' ) as f :
                f.write( '# Bender session by %s started at %s\n' % ( me , start_time.strftime('%c' ) ) ) 
                f.write( '# Command from CWD=%s \n# %s\n' % ( curdir , command  ) ) 
                for record in ip.history_manager.get_range() :
                    f.write( record[2] + '\n' )
                f.write( '# Bender session by %s   ended at %s\n' % ( me ,   end_time.strftime('%c' ) ) ) 
                
            if os.path.exists( fname ) and os.path.isfile ( fname ) and not _empty_ ( fname ) : 
                logger.info ( 'Bender history file: %s' % __history__ )
                return
            
        except:
            pass
            
    ## use 'old-style' history 
    readline.write_history_file ( fname ) 
    if os.path.exists( fname ) and os.path.isfile ( fname ) and not _empty_ ( fname ) : 
        logger.info ( 'Bender history file: %s' % __history__ )
        


import atexit
atexit.register ( _prnt_ )
atexit.register ( write_history , __history__ )    

    
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
