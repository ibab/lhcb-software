#! /usr/bin/env python
# =============================================================================
""" @namespace loadModule 
@brief Makes easier use of pickle
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@date 2009-07-27
"""
# =============================================================================



#---------------------------------------------------
def loadFromModule(file):
    """ A shortcut to load from a module
    @param fileName File to be loaded
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """
    
    myf=str(file)
    exec("from MuonID import "+myf)
    exec("outp="+myf+"."+myf)
    return outp
    
