#!/usr/bin/env python2.2
###############################################################################
#   Initialize LSF queue parameters
#
# "-q" gives the queue (1nh by default)
# "-c" gives the CPU time in minutes
#
# -q 8nm  = about 30 min HP735     express
# -q 1nh           3 hours         short
# -q 8nh           1 day           medium
# -q 1nd           3 days          long
# -q 1nw          10 days HP K260  verylong
#
#
#  define the job name
#BSUB -J BenderJob
#  define the queue
#BSUB -q 8nm
#
###############################################################################
"""
==============================================================================
Simple script to submit/run jobs through LSF
==============================================================================
   The script sets a proper environment ( 'BenderExample' )
   and run a command, constructed from arguments 
==============================================================================
Usage:

# run inetractively: 
    >                   BenderExJob.py Dstar.py -n 1000
# submit the job through LSF
    > bsub -J BenderJob BenderExJob.py Dstar.py -n 1000
    
"""

import sys , os

# =============================================================================
# Set the generic environment ( CMT, Project, etc ...)
# =============================================================================
def setEnvironment() :
    """
    Set the generic job environment ( CMT, project, etc ...)
    """
    if 'LHCBHOME' not in os.environ.keys() :
        print "WARNING: redefine LHCBHOME " 
        environ['LHCBHOME'] = '/afs/cern.ch/lhcb'
    if 'LHCBDEV'  not in os.environ.keys() :
        print "WARNINGL redefine LHCBDEV  " 
        environ['LHCBHOME'] = os.environ['LHCBHOME']+'/DEV'

    # some initial standard environment ( do we need it?) 
    os.system('. $LHCBHOME/scripts/oldCMT.sh')

    # check for CMT settings
    if 'CMTBIN'    not in os.environ.keys() :
        raise RuntimeError, " CMTBIN    is not defined "
    if 'CMTROOT'   not in os.environ.keys() :
        raise RuntimeError, " CMTROOT   is not defined "
    if 'CMTCONFIG' not in os.environ.keys() :
        raise RuntimeError, " CMTCONFIG is not defined "
        
    # add Pere's CMT package 
    pycmt = os.environ['LHCBHOME']+'/scripts'
    if pycmt not in sys.path : sys.path += [ pycmt ]
    import cmt

    # set the project environment 
    cmt.project( 'Bender'  , 'v3r7'   )

    # show CMT paths 
    for p in cmt.showpath() : print  ' CMT path: ' + p
    
    # configure the job     
    cmt.use( package = 'Ex/BenderExample' )
    
    # show all used packages 
    for p in cmt.showuses() : print 'CMT use ', p
    
    cmt.config()

    sys.stdout.flush()
    sys.stderr.flush()
    return cmt.setup()
# =============================================================================

# =============================================================================
# check the batch versus interactive work 
# =============================================================================
def  inBatch() :
    """
    Check the batch versus interactive work 
    """
    if os.environ.has_key('LS_SUBCWD') : return (1<2)
    return (1>2) 

# =============================================================================
# copy useful files into initial directory
# =============================================================================
def  copyResults( pattern = '*.*' ) :
    """
    copy results (in Batch mode)
    Usage:
          copyResults( pattern = '*.*' )
    """
    if not inBatch() : return
    import glob,os
    # remove core*
    cores = glob.glob('core*')
    for core in cores : os.path.remove( core )
    # copy all files 
    files = glob.glob( pattern )
    ret   = open('RETURN','w')
    for file in files : ret.write( file+'\n')
    # list of copied files 
    return files 

# =============================================================================
# job execution itself 
# =============================================================================
if __name__ == '__main__' :
    import string 
    # set CMT/Project/context global environments
    setEnvironment()
    # the command to be executed 
    command = string.join( sys.argv[1:] , ' ' )
    # the actual execution 
    res     = os.system( command )
    # copy the result (in batch)  
    print copyResults()

# =============================================================================
# The END 
# =============================================================================
