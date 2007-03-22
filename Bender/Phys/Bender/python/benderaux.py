# =============================================================================
# $Id: benderaux.py,v 1.24 2007-03-22 18:50:47 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.24 $
# =============================================================================
## Auxillary module  to keep some helper fuctions for Bender
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" Auxillary module  to keep some helper fuctions for bender """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

import os,sys,sets
import gaudimodule

# =============================================================================
## Load all defined dictionary libraries
#  @param  lst list of additional dictionaries to be load
#  @param  verbose flag for verbose execution 
#  @return list of properly loaded dictionaries 
def _loadDict_ ( lst = [] , verbose = True ) :
    """
    Load all defined dictionary libraries
    Parameters:
    
    - lst      list of additional dictionaries to be load
    - verbose  flag for verbose execution 

    return list of properly loaded dictionaries 
    """
    if type(lst) is str : lst = [lst] 
    ## get the libraries from argument
    _libs_ = sets.Set( lst )
    
    ## get the patterns form the environment 
##     for k in os.environ :
##         i = k.find('DictShr')
##         if 0 < i : _libs_.add( k[0:k.find('Shr')] )
    
##     _libs_.add ( 'LHCbKernelDict'    )
##     _libs_.add ( 'RelationsDict'     )
##     _libs_.add ( 'GenEventDict'      )
##     _libs_.add ( 'DAQEventDict'      )
##     _libs_.add ( 'DigiEventDict'     )
##     _libs_.add ( 'TrackFitEventDict' )
##     _libs_.add ( 'TrackEventDict'    )
##     _libs_.add ( 'PackedEventDict'   )
##     _libs_.add ( 'MCEventDict'       )
##     _libs_.add ( 'RecEventDict'      )
##     _libs_.add ( 'PhysEventDict'     )
##     _libs_.add ( 'HltEventDict'      )
##     _libs_.add ( 'EventAssocDict'    )
##     _libs_.add ( 'LinkerEventDict'   )
##     _libs_.add ( 'DetDict'           )
##     _libs_.add ( 'LoKiDict'          )
##     _libs_.add ( 'LoKiCutsDict'      )
##     _libs_.add ( 'BenderDict'        )
    
    if verbose : print ' Libraries to be loaded: %s' % list(_libs_)
    
    ## list of properly loaded dictionaries 
    good = sets.Set()
    for _lib_ in _libs_ :
        try    :
            ## load dictionary:
            gaudimodule.loaddict( _lib_ )
            good.add(_lib_)
        except :
            print 'Error Loading the Dictionary "%s"' % _lib_
    ## return the list of properly loaded dictionaries
    good = list(good) 
    if verbose: print ' Successfully loaded dictionaries: %s' % good
    return good 

# =============================================================================
## Load list of DLLs
#  @param lst list of DLLs to be loaded or library name 
#  @param appMgr application manager
#  @return list of successfully loaded libraries 
def _loadDll_ ( lst , appMgr = None ) :
    """
    Load list of DLLs
    
    - lst list of DLLs to be loaded or library name 
    - appMgr application manager
    
    return list of successfully loaded DLLs
    
    """
    ## library or list of libraries? 
    if type(lst) == str : lst= [lst] 
    ## create application manager if needed 
    if not appMgr : appMgr = gaudimodule.AppMgr()
    ## get the loaded libraries from ApplicationManager
    _DLLs = sets.Set( appMgr.DLLs )    
    ## get the libraries from argument, except already loaded libraries 
    libs = [ l for l in lst if not l in _DLLs ]
    ## remove duplicates (if any) 
    libs = list ( sets.Set ( libs ) )
    ## load libraries (if needed) 
    if libs : appMgr.DLLs += libs
    ## return list of successfully loaded libraries 
    return libs

# =============================================================================
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END 
# =============================================================================
