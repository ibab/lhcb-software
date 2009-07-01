#! /usr/bin/env python
# =============================================================================
""" @namespace extpickdesktop
@brief Makes easier use of pickle
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@author Hugo Ruiz hugo.ruiz@cern.ch
@date 2007-11-28
"""
# =============================================================================


import cPickle

#---------------------------------------------------
def pickleSave( myObject, fileName):
    """ A shortcut to save an object with pickle.
    @param myObject Object to save
    @param fileName File to use
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    print commands.getoutput('mv '+fileName+' '+fileName+'.old')
    f = open(fileName, 'w')
    cPickle.dump(myObject, f)
    f.close()
    return

#---------------------------------------------------
def pickleLoad(fileName):
    """ A shortcut to load an object from a pickle file.
    @param fileName File to use
    @returns The object in the file
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    f = open(fileName, 'r')
    return cPickle.load(f)

