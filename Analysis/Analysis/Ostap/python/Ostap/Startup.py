#!/usr/bin/env python
# =============================================================================
# $Id: Startup.py 173921 2014-06-16 14:25:50Z ibelyaev $ 
# =============================================================================
## The trivial startup sctript for Ostap session
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
#                    $Revision: 173921 $
#  Last modification $Date: 2014-06-16 16:25:50 +0200 (Mon, 16 Jun 2014) $
#                 by $Author: ibelyaev $ 
# =============================================================================
"""This is a trivial startup script for Ostap session
 
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
__version__ = "$Revision: 173921 $"
__all__     = () 
# =============================================================================
## logging
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'Ostap.Startup' )
else                      : logger = getLogger ( __name__ )
# =============================================================================

def with_ipython() :
    try :
        __IPYTHON__
        return True
    except NameError :
        return False 

try:

    import datetime
    start_time = datetime.datetime.now()
    logger.info ( 'Ostap session started %s' % start_time.strftime('%c')  )
    
    import os
    ## define the name of the history file
    __history__ = os.path.curdir + os.sep + '.ostap_history'
    

    try :
        
        def _rename_ ( base , version = 0  ) :
            
            if version <= 0 :
                version = 0 
                fname   = '%s'    %    base 
                fnamep1 = '%s.%d' %  ( base , 1 )
            else :
                fname   = '%s.%d' %  ( base , version     )
                fnamep1 = '%s.%d' %  ( base , version + 1 )
                
            if os.path.exists ( fnamep1 ) :
                if 0 == os.path.getsize( fnamep1 ) : os.remove ( fnamep1 )
                else : _rename_ ( base , version + 1 )
                
            if os.path.exists ( fname ) :
                if 0 == os.path.getsize( fname ) : os.remove ( fname )
                else : os.rename ( fname , fnamep1 )
                
        _rename_ ( __history__  )
        
    except :
        logger.warning ( "Can't erase old history files", exc_info = True ) 
        pass



    ## write history at the end 
    def _prnt_() :
        end_time = datetime.datetime.now()   
        logger.info ( 'Ostap session   ended %s' % end_time.strftime('%c')  )
            
    ## line completer 
    import rlcompleter
    import readline
    readline.clear_history() 
    readline.parse_and_bind("tab: complete")

    def write_history ( fname ) :

        if with_ipython() :
 
            try :
                ##  history from IPython 
                import IPython
                ip  = IPython.get_ipython()
                with open ( fname , 'w' ) as f :
                    f.write( '# Ostap session started %s\n' % start_time.strftime('%c')  ) 
                    for record in ip.history_manager.get_range() :
                        f.write( record[2] + '\n' )
                    end_time = datetime.datetime.now()   
                    f.write( '# Ostap session   ended %s\n' % end_time.strftime('%c')  )
                if os.path.exists( fname ) and os.path.isfile ( fname ) and 0 != os.path.getsize ( fname ) : 
                    logger.info ( 'Ostap history file: %s' % fname )                    
                return
            
            except:
                pass
            
        ##  history from readline 
        readline.write_history_file ( fname ) 
        if os.path.exists( fname ) and os.path.isfile ( fname ) and 0 != os.path.getsize ( fname ) : 
            logger.info ( 'Ostap history file: %s' % fname )
            

    import atexit

    atexit.register ( _prnt_ )
    atexit.register ( write_history , __history__ )    

except:
    ## 
    logger.error ( 'Error in startup configuration, ignore... ' , exc_info = True ) 


# =============================================================================
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================
