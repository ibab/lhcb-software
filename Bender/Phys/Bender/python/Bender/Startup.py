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
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV ibelyaev@physcis.syr.edu
#  @date   2006-10-08
# =============================================================================
"""
This is a trivial startup script for python Bender session

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = 'Vanya BELYAEV belyaev@physics.syr.edu'
__date__    = "2006-10-08"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, version $Revision$"
__all__     = () 
# =============================================================================

try:

    import os
    ## define the name of the history file
    __history__ = os.path.curdir + os.sep + '.BenderHistory'
    
    def _rename_ ( file , app ) :
        if os.path.exists ( file + app ) :
            if 0 == os.path.getsize( file + app ) : os.remove( file + app )
            else :   _rename_ ( file + app ,        app )
        if os.path.exists ( file       ) :
            if 0 == os.path.getsize( file ) : os.remove( file )
            else : os.rename  ( file       , file + app )

    ## remove/backup the previous history file
    _rename_ ( __history__ , '.OLD' )

    ## write history at the end 
    def _prnt_() :
        print 'BENDER history file: %s' % __history__
        
    ## line completer 
    import rlcompleter
    import readline
    readline.parse_and_bind("tab: complete")


    import atexit
    atexit.register ( readline.write_history_file , __history__ )
    atexit.register ( _prnt_ )

    _prnt_() 
    
except:
    pass 


    
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
