#!/usr/bin/env python

from bendermodule import *


def configure() :
    """
    This is the configuration method for module Minimalistic_1.py
    """
    # get the confgigurtaion for *.opts file
    gaudi.config( files = ['$BENDERTUTOROPTS/BenderTutor.opts'] )
    return SUCCESS

if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    gaudi.run(50)
    
    # for the interactive mode it is better to comment the last line
    gaudi.exit()


