##################################################################################
# Utility to dump the sequence in the appmgr
##################################################################################
def printsequence( appMgr ):
    def printAlgo( algName, appMgr, prefix = ' ') :
        HEADER = '\033[95m'
        OKBLUE = '\033[94m'
        OKGREEN = '\033[32m'
        WARNING = '\033[93m'
        FAIL = '\033[91m'
        ENDC = '\033[0m'
        alg = appMgr.algorithm( algName.split( "/" )[ -1 ] )
        prop = alg.properties()
      # if the algorithm is not enabled, print a '#'
        if prop.has_key( "Enable" ) and not prop[ "Enable" ].value():
            print FAIL + prefix + algName + ' (disabled)' + ENDC
        else:
         #print prefix + algName
            if prop.has_key( "Members" ) :
                print OKBLUE + prefix + algName + ENDC
                subs = prop[ "Members" ].value()
                for i in subs : printAlgo( i.strip( '"' ), appMgr, prefix + "     " )
            elif prop.has_key( "DetectorList" ) :
                print OKBLUE + prefix + algName + ENDC
                subs = prop[ "DetectorList" ].value()
                for i in subs : printAlgo( algName.split( "/" )[ -1 ] + i.strip( '"' ) + "Seq", appMgr, prefix + "     ")
            else:
                print OKGREEN + prefix + algName + ENDC

    mp = appMgr.properties()
    print "\n ****************************** Algorithm Sequence **************************** \n"
    for i in mp["TopAlg"].value(): printAlgo( i, appMgr )
    print "\n ****************************************************************************** \n"


##################################################################################
# Utility to stage files on a local disk
##################################################################################
def stagelocally():
    print "Warning: staging files locally"
    # first create the target directory, if it doesn't exist yet
    import re, os, getpass
    username = getpass.getuser()
    targetdir = '/tmp/%s/stagedfiles/' % username
    if os.path.isdir( '/pool/spool/' ) :
        targetdir = '/pool/spool/%s/stagedfiles/' % username
    if not os.path.isdir( targetdir ):
        os.makedirs( targetdir )
         
    # remove any old files in that directory
    os.system("find %s -atime +7 -exec rm '{}' \;" % targetdir)
    
    from Configurables import EventSelector
    eventinput = EventSelector().Input
    neweventinput = []
    for i in eventinput:
        # need some pattern matching ...
        x = re.match("DATAFILE='([^']*)'",i)
        if not x: trouble
      #for j in x.groups() : print j
        fullpath = x.group(1)
      #print 'fullfilename:', fullpath
       #extract the filename itself, without the path
        filename = fullpath[fullpath.rfind("/")+1:]
        targetfilename = targetdir + filename
        issuccess = True
        if not os.path.exists( targetfilename ):
          #if filename contains castor, use rfcp
            if fullpath.find('castor')>=0:
                castorfile = fullpath[fullpath.find("/"):]
                print "copying file %s from castor\n" % castorfile
                os.system('rfcp %s %s' % (castorfile,targetfilename))
            elif fullpath.find('LFN:')>=0:
                lfnfile = fullpath.replace("LFN","lfn")
                print "copying file %s from grid\n" % lfnfile
                os.system("lcg-cp % s %s " % (lfnfile,targetfilename))
            else:
                print 'cannot cache file locally: %s\n' % fullpath
                issuccess = False
        
        # now fix the input file
        if issuccess:
            i = i.replace(fullpath,targetfilename)
        neweventinput.append(i)
    EventSelector().Input = neweventinput
