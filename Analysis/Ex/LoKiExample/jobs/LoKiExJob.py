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
#BSUB -J LoKiJob
#  define the queue
#BSUB -q 8nm
#
###############################################################################
"""
==============================================================================
Simple script to submit/run jobs through LSF
==============================================================================
   The script sets a proper environment ( 'LoKiExample' )
   and run a command, constructed from arguments 
==============================================================================
Usage:

# run inetractively: 
    >                 LoKiExJob.py -o My.opts -n 1000 
# submit the job through LSF
    > bsub -J LoKiJob LoKiExJob.py -o My.opts -n 1000
    
"""

import sys , os

script = __name__ 
jobexe = 'job.exe'
jobopt = 'job.opts'

verbose = (1<2)
# =============================================================================
# Set the generic CMT environment ( Project, etc ...)
# =============================================================================
def setLHCBenv () :
    """
    Set the generic LHCb environment: LHCB*
    """
    patterns = ( 'ROOT' , 'SITE' , 'DIR' , 'PATH' , 'HOST' , 'TAPES' , 'LHCB' ) 
    command  = '. $LHCBHOME/scripts/lhcbenv.sh ; '
    for pattern in patterns  :
        command += ' env | grep ' + pattern + ' ; ' 
    cint,cout,cerr = os.popen3( command )
    for line in cout :
        line  = line.replace('\n','')
        items = line.split(' ')
        for item in items :
            for pattern in patterns : 
                if -1 == item.find(pattern)     : continue 
                index  = item.find('=')
                if -1 == index                  : continue 
                variable = item[:index]
                if -1 == variable.find(pattern) : continue 
                value    = item[index+1:]
                if not value                    : continue 
                if verbose  : print ' LHCB: variable/value ' + `variable` + ' ' + `value`
                os.environ[variable] = value
    for line in cerr : print 'STDERR: '+ `line`.replace('\n','')
        
# =============================================================================
# Set the generic CMT environment ( Project, etc ...)
# =============================================================================
def setCMTenv ( ) :
    """
    Set the generic CMT environment: CMT* 
    """
    setLHCBenv()
    
    variables = ('CMTROOT','CMTBIN','CMTPATH','CMTCONFIG','CMTDEB')
    command   = '( . $LHCBHOME/scripts/lhcbenv.sh ; '
    command  += '  . $LHCBHOME/scripts/newCMT.sh  ; '
    command  += ' env | grep CMT '
    command  += ' ) ' 
    cin,cout,cerr = os.popen3( command )
    for line in cout :
        line  = line.replace('\n','')
        print 'Line = ' + `line`
        items = line.split(' ')
        for item in items :
            for variable in variables :
                pattern = variable + '='
                index = item.find(pattern)
                if -1 == index     : continue 
                var   = item[:index]
                if var != variable : continue 
                value = item[index + len(pattern):]
                if not value   : continue 
                print ' CMT: variable/value ' + `variable` + ' ' + `value`
                os.environ[variable] = value 
    # errors ? 
    for line in cerr : print 'STDERR' + `line`.replace('\n','')
    # final check 
    if 'CMTROOT'   not in os.environ.keys() :
        raise RuntimeError, " CMTROOT   is not defined "
    if 'CMTBIN'    not in os.environ.keys() :
        raise RuntimeError, " CMTBIN    is not defined "
    if 'CMTPATH'   not in os.environ.keys() :
        raise RuntimeError, " CMTPATH   is not defined "
    if 'CMTCONFIG' not in os.environ.keys() :
        raise RuntimeError, " CMTCONFIG is not defined "
    if 'CMTDEB'    not in os.environ.keys() :
        raise RuntimeError, " CMTDEV    is not defined "
    
# =============================================================================
# Set the generic environment ( Project, etc ...)
# =============================================================================
def setEnvironment() :
    """
    Set the generic job environment ( CMT, project, etc ...)
    """
    if 'LHCBHOME' not in os.environ.keys() :
        print "WARNING: redefine LHCBHOME " 
        environ['LHCBHOME'] = '/afs/cern.ch/lhcb'
    if 'LHCBDEV'  not in os.environ.keys() :
        print "WARNING: redefine LHCBDEV  " 
        environ['LHCBHOME'] = os.environ['LHCBHOME']+'/DEV'
        
    # set the  generic LHCB+CMT environment 
    setCMTenv()

    # add Pere's CMT package
    # package is placed now at $LHCBHOME/scripts 
    pycmt = os.environ['LHCBHOME'] + os.sep + 'scripts'
    if pycmt not in sys.path : sys.path.extend( [ pycmt ] )
    import cmt

    # set the project environment 
    print cmt.project( 'DaVinci'  , 'v12r0' , os.environ['LHCBDEV'] )
        
    # show CMT paths 
    for p in cmt.showpath() : print  ' CMT path: ' + `p`
        
    # configure the job     
    cmt.use( package = 'Ex/LoKiExample' )
    cmt.use( package = 'Phys/DaVinci'   , version = 'v12r0' )

    # show all used packages a
    for p in cmt.showuses() : print `p`
    
    sys.stdout.flush()
    sys.stderr.flush()
    # execute the setup 
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

# =============================================================================
# copy useful files into initial directory
# =============================================================================
def  copyResults( pattern = '*.*' ) :
    import glob,os
    # remove core*
    cores = glob.glob('core*')
    for core in cores : os.path.remove( core )
    # try to remove the executable
    try :   os.remove( jobexe )
    except: pass
    # nothing to be copied for non-batch mode 
    if not inBatch() : return 
    # copy all files 
    files = glob.glob( pattern )
    ret   = open('RETURN','w')
    for file in files : ret.write( file+'\n')
    # list of copied files 
    return files
# =============================================================================

# =============================================================================
# get all redefineable joboptions 
# =============================================================================
def getOpts( options ) :
    """
    parse the job options
    """
    import getopt
    try:
        opts , args = getopt.getopt ( options , "o:n:h:t:e:d:m:f:p:" ,
                                      [ 'options='     , 
                                        'nevents='     ,
                                        'histos='      , 
                                        'histograms='  , 
                                        'tuples="'     ,
                                        'evtcols='     ,
                                        'dst='         ,
                                        'minidst='     ,
                                        'first='       ,
                                        'persistency=' ] )
        optdict = {} 
        for opt,value in opts :
            if    opt in ( '-o'  , '--options='  ) :
                optdict['options']     =      value
            elif  opt in ( '-n'  , '--nevents='      ) :
                optdict['nevents']     = int( value )
            elif  opt in ( '-h'  , '--histos=' , '--histograms=' ) :
                optdict['histos']      =      value
            elif  opt in ( '-t'  , '--tuples='       ) :
                optdict['tuples']      =      value
            elif  opt in ( '-e'  , '--evtcols='      ) :
                optdict['evtcols']     =      value
            elif  opt in ( '-d'  , '--dst='          ) :
                optdict['dst']         =      value
            elif  opt in ( '-m'  , '--minidst='      ) :
                optdict['minidst']     =      value                
            elif  opt in ( '-f'  , '--first='        ) :
                optdict['first']       = int( value ) 
            elif  opt in ( '-p'  , '--persistency='  ) :
                optdict['persistency'] =      value  
        return optdict
    except:
        raise 
# =============================================================================
    

# =============================================================================
# configuration of redefineable options 
# =============================================================================
def configure() :
    import os 
    # get all options 
    opts = getOpts( sys.argv[1:] )
    # check mandatory option
    if not opts.has_key('options') :
        raise RuntimeError, "DaVinci job-options file (*.opts) is not specified!"
    options = opts['options']
    if not os.path.exists( options ) :
        raise RuntimeError, "DaVinci job-options file '"+options+"' does not exist"
    # read all options 
    optread = open( options ).readlines()
    # optread = []
    endl = ' ; \n\n'
    if opts.has_key('nevents') :
        v        = `opts['nevents']`
        optread += [ '// ' + `script` + ': redefine number of events  '        + endl ]
        optread += [ 'ApplicationMgr.EvtMax    = ' + v                         + endl ]
    if opts.has_key('first')   :
        v        = `opts['first']`
        optread += [ '// ' + `script` + ': skip events '                       + endl ]
        optread += [ 'EventSelector.FirstEvent = ' + v                         + endl ]
    if opts.has_key('histos')  :
        v        = opts['histos'] 
        optread += [ '// ' + `script` + ': output file for histos '            + endl ]
        optread += [ 'HistogramPersistencySvc.OutputFile = "'+ v + '"'         + endl ]
    if opts.has_key('tuples')  :
        v        = opts['tuples']
        v = v.split(',')
        if len(v) < 2 : v = [ "TUPLES" ] + v  
        line = 'NTupleSvc.Output += { '
        l1 = '"'+ v[0] + "  DATAFILE='" + v[1] + "' TYP='HBOOK' OPT='NEW'" + '"' 
        line += l1 + ' } ; '
        optread += [ line + endl ]
    if opts.has_key('tuples')  :
        v        = opts['tuples']
        v = v.split(',')
        if len(v) < 2 : v = [ "TUPLES" ] + v  
        optread += [ '// ' + `script` + ': output file for N-tuples '      + endl ]
        line = 'NTupleSvc.Output += { '
        l1 = '"'+ v[0] + "  DATAFILE='" + v[1] + "' TYP='HBOOK' OPT='NEW'" + '"' 
        line += l1 + ' } ; '
        optread += [ line + endl ]
    if opts.has_key('evtcols')  :
        v        = opts['evtcols']
        v = v.split(',')
        if len(v) < 2 : v = [ "EVTTAGS" ] + v  
        optread += [ '// ' + `script` + ': output file for Event Tag Collections ' + endl ]
        line = 'EvtTupleSvc.Output += { '
        l1 = '"'+ v[0] + "  DATAFILE='PFN:" + v[1] + "' TYP='POOL_ROOTTREE' OPT='RECREAT'" + '"' 
        line += l1 + ' } ; '
        optread += [ line + endl ]
    if opts.has_key('dst' ) :
        v        = opts['dst'] 
        optread += [ '// ' + `script` + ': output file for DST '            + endl ]        
        line     = '"' + "DATAFILE='PFN:" + v+ "' TYP='POOL_ROOTTREE' OPT='RECREAT'" + '"' 
        optread += [ 'DstWriter.Output = ' + line + endl ]
    if opts.has_key('minidst' ) :
        v        = opts['minidst'] 
        optread += [ '// ' + `script` + ': output file for (mini)DST '            + endl ]        
        line     = '"' + "DATAFILE='PFN:" + v+ "' TYP='POOL_ROOTTREE' OPT='RECREAT'" + '"' 
        optread += [ 'MiniDstWriter.Output = ' + line + endl ]
        
    try    : os.remove( jobopt )
    except : pass 
    file = open( jobopt , 'w' )
    file.writelines(optread)
    file.close()
    #
    executable = os.environ['DAVINCIROOT']+'/'+os.environ['CMTCONFIG']+'/DaVinci.exe'
    if not os.path.exists( executable  ) :
        raise RuntimeError, "DaVinci executable file '"+executable+"' does not exist"
    try    : os.remove( jobexe )
    except : pass 
    if inBatch() :  os.system(' cp ' + executable + '  ' + jobexe )
    else         :  os.symlink( executable , jobexe )
    if not os.path.exists( jobexe  ) :
        raise RuntimeError, "DaVinci executable file/link '"+jobexe+"' does not exist"    
# =============================================================================

# =============================================================================
# job execution itself 
# =============================================================================
if __name__ == '__main__' :
    script = 'LoKiExJob'
    import string 
    # set CMT/Project/context global environments
    setEnvironment()
    configure() 
    # the command to be executed
    command = jobexe + ' ' + jobopt
    # the actual execution 
    res     = os.system( command )
    # copy the result (in BATCH only)  
    copyResults()

# =============================================================================
# The END 
# =============================================================================
