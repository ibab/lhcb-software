"""
@file ZooPy.py

module which checks where libZooROOT.so and pyROOT might be located and
loads libZooROOT.so into ROOT

@author Manuel Schiller <manuel.schiller@nikhef.nl>
@date 2011-11-24

in client code, the following shold be enough:
    @code
    import ZooPy
    from ROOT import *
    @endcode
"""

#############################################################################
# BEGIN setup phase: make sure ROOT/Zoo components are accessible
#############################################################################
# get Zoo libs into path
import os
if not 'ZooPyInitialised' in vars():
    ZooPyInitialised = False
if not ZooPyInitialised:
    import sys, re
    if not 'ZOOWRITERROOT' in os.environ:
        # ZOOWRITERROOT environment variable unset, try to guess correct
        # location of libZooROOT.so from location of python script
        if os.path.exists(re.sub('(/|\\\\)[^/\\\\]+$', '/Zoo/libZooROOT.so', \
                re.sub('^([^/]+)$', os.getcwd()+'/\1', sys.argv[0]))):
            os.environ['ZOOWRITERROOT'] = re.sub('(/|\\\\)[^/\\\\]+$', '', \
                    re.sub('^([^/]+)$', os.getcwd()+'/\1', sys.argv[0]))
        else:
	    # try to guess
	    if os.path.exists('libZooROOT.so'):
		os.environ['ZOOWRITERROOT'] = os.getcwd()
	    elif os.path.exists('Zoo/libZooROOT.so'):
		os.environ['ZOOWRITERROOT'] = os.getcwd() + '/Zoo'
	    else:
                # try to ask user by reading from stdin
                print 'Please enter location to be used for $ZOOWRITERROOT'
                os.environ['ZOOWRITERROOT'] = re.sub('(\n|\r)+$', '', \
			sys.stdin.readline())
    # convert relative path to absolute
    os.environ['ZOOWRITERROOT'] = \
	    re.sub('^\\./', os.getcwd() + '/', os.environ['ZOOWRITERROOT'])
    os.environ['ZOOWRITERROOT'] = \
	    re.sub('^\\.$', os.getcwd(), os.environ['ZOOWRITERROOT'])
    print 'ZOOWRITERROOT is ' + os.environ['ZOOWRITERROOT']
    if not 'LD_LIBRARY_PATH' in os.environ or \
	    '' == os.environ['LD_LIBRARY_PATH']:
        os.environ['LD_LIBRARY_PATH'] = os.environ['ZOOWRITERROOT'] + '/Zoo'
    else:
        os.environ['LD_LIBRARY_PATH'] = \
            os.environ['ZOOWRITERROOT'] + '/Zoo:' + \
            os.environ['LD_LIBRARY_PATH']
    print 'LD_LIBRARY_PATH is ' + os.environ['LD_LIBRARY_PATH']
    # ok, use root-config to get ROOT library path and set up python search
    # paths accordingly (on most installations, python does not know about
    # ROOT)
    tmp = os.popen('root-config --libdir')
    sys.path.append(re.sub('(\n|\r)+$', '', tmp.readline()))
    tmp.close()
    print 'sys.path is ' + str(sys.path)
    # get ROOT stuff
    import ROOT
    from ROOT import *
    # load Zoo library (including needed dictionaries)
    ROOT.gSystem.Load('libZooROOT.so')
    ZooPyInitialised = True
#############################################################################
# END setup phase
#############################################################################

# vim: sw=4:tw=78:ft=python
