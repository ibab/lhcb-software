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

try:

    import datetime
    start_time = datetime.datetime.now()
    logger.info ( 'Bender session started %s' % start_time.strftime('%c')  )
    
    import os
    ## define the name of the history file
    __history__ = os.path.curdir + os.sep + '.bender_history'
    

    try :
        
        def _rename_ ( base , version = 0  ) :
            
            if version <= 0 :
                version = 0 
                fname   = '%s'    %    base 
                fnamep1 = '%s.%d' %  ( base , 1 )
            else :
                fname   = '%s.%d' %  ( base , version     )
                fnamep1 = '%s.%d' %  ( base , version + 1 )
                
            if os.path.exists ( fname ) and  0 == os.path.getsize( fname ) :
                os.remove ( fname )
                return

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
        logger.info ( 'Bender session   ended %s' % end_time.strftime('%c')  )
            
    ## line completer 
    import rlcompleter
    import readline
    readline.clear_history() 
    readline.parse_and_bind("tab: complete")


    def write_history ( fname ) :
        
        if with_ipython() :

            try :
                
                import IPython
                ip  = IPython.get_ipython()
                with open ( fname , 'w' ) as f :
                    f.write( '# Bender session started %s\n' % start_time.strftime('%c')  ) 
                    for record in ip.history_manager.get_range() :
                        f.write( record[2] + '\n' )
                    end_time = datetime.datetime.now()   
                    f.write( '# Bender session   ended %s\n' % end_time.strftime('%c')  )
                
                if os.path.exists( fname ) and os.path.isfile ( fname ) and 0 != os.path.getsize ( fname ) : 
                    logger.info ( 'Bender history file: %s' % __history__ )
                return
            
            except:
                pass
            
        ## use 'old-style' history 
        readline.write_history_file ( fname ) 
        if os.path.exists( fname ) and os.path.isfile ( fname ) and 0 != os.path.getsize ( fname ) : 
            logger.info ( 'Bender history file: %s' % __history__ )
        

    import atexit

    atexit.register ( _prnt_ )
    atexit.register ( write_history , __history__ )    

except:
    ## 
    logger.error ( 'Error in startup configuration, ignore... ' , exc_info = True ) 


    
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
