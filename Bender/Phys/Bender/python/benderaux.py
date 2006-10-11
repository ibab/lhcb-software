# =============================================================================
# $Id: benderaux.py,v 1.16 2006-10-11 14:45:07 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.16 $
# =============================================================================
""" Auxillary module  to keep some helper fuctions for bender """
# =============================================================================
## @file
#  Auxillary module  to keep some helper fuctions for Bender
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"

import os
import gaudimodule

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
    import sets
    _libs_ = sets.Set()
    ## get the patterns form the environment 
    for k in os.environ.keys() :
        i = k.find('DictShr')
        if 0 < i : _libs_.add( k[0:k.find('Shr')] )
    ## get the additional libraries from argument
    if type(lst) is str : lst = [lst] 
    for l in lst : _libs_.add ( l )
    _libs_.remove('LoKiDict')
    _libs_.remove('DetDict')
    _libs_.remove('BenderDict')
    _libs_.remove('RelationsDict')
    if verbose : print ' Libraries to be loaded: %s' % _libs_
    good = [] 
    for _lib_ in _libs_ :
        try    :
            if verbose : print ' Try to load dictionary: %s' % _lib_
            gaudimodule.loaddict( _lib_ )
            good += [ _lib_ ]
        except :
            print 'Error Loading the Dictionary "%s"' % _lib_
    ## return the list of properly loaded dictionaries
    if verbose : print ' Successfully loaded dictionaries: %s' % good 
    return good 

## Load list of DLLs
#  @param lst list of DLLs to be loaded
#  @param verbose verbosity flag
#  @param appMgr application manager
#  @return list of successfullyt loaded DLLs 
def _loadDll_ ( lst , verbose = True , appMgr = None ) :
    """ Load list of DLLs
    - lst list of DLLs to be loaded
    - verbose verbosity flag
    - appMgr application manager
    return list of successfully loaded DLLs
    """
    if not appMgr : appMgr = gaudimodule.AppMgr()
    good = [] 
    for lib in lst :
        if lib not in appMgr.DLLs :
            try :
                if verbose : print 'Try to load library: %s' % lib
                appMgr.DLLs += [ lib ]
                good += [lib]
            except :
                print 'Error loading the Library: %s' % lib
    if verbose : print ' Successfully loaded libraries: %s '% good 
    return good 
    
# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
# The END 
# =============================================================================
