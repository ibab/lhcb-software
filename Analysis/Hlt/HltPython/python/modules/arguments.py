#! /usr/bin/env python
"""
@brief two methods to deal with command input arguments
@author J.A. Hernando, jose.hernando@cern.ch
@ data 16-03-05
"""

import sys
import os
import string

def argument(key, comment, default = None):
    """ return the command line argument associated with a key, if not
    return the default value
    use: argument('-ifile',' input file name','input.txt')
    @author Jose A. Hernando
    """

    args = sys.argv
    i = 0
    name= default;
    if (args):
        for i in range(len(args)):
            word = args[i]
            if (word == "-"+key):
                name = args[i+1]
    print " argument ",key," \t", comment," : \t",name
    if (name == None):
        print "   use: ",comment
    return name

def argument_list(key, comment, default = []):
    """ return the command line arguments associated with a key, if not
    return the default value
    use: argument('-ifiles',' input file names',['input.txt','input2.txt'])
    @author Jose A. Hernando
    """
    args = sys.argv
    list = default
    i0 = 0
    ie = 0
    if (args):
        for i in range(len(args)):
            word = args[i]
            if (word == "-"+key):
                i0 = i+1
            for j in range(i+1,len(args)):
                if (args[j].find("-")>=0): ie = j
    if ((i0 != 0) and (ie >=i0)):
        list = args[i0,ie]
    print " argument list ",key," ", list
    if (len(list) == 0):
        print "   use: ",comment
    return list


