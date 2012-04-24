#!/usr/bin/env python
########################################################################
# $Id: onlineInstallSoft.py,v 1.13 2008/04/28 12:26:22 joel Exp $
########################################################################
""" LHCb Online installation script
"""
__RCSID__ = "$Id: onlineInstallSoft.py,v 1.13 2008/04/28 12:26:22 joel Exp $"


import sys, re, os, commands, urllib, urlparse, time, string, socket, shutil

DEBUG = 0
SWTAGS = 1
FORCE_LOCK_REMOVAL = 0
CLEANUP_OLD_SOFTWARE = 0
DEFAULT_URL = 'http://lhcbproject.web.cern.ch/lhcbproject/dist/'
if os.environ.has_key( 'VO_LHCB_SW_DIR' ):
  sharedArea = os.environ['VO_LHCB_SW_DIR']
else:
  sharedArea = '/sw'

mysiteroot = os.path.join( sharedArea, 'lib' )
os.environ['MYSITEROOT'] = mysiteroot
filelock = os.path.join( mysiteroot, 'SAME_lock' )

startTime = time.time()
filescript = 'install_project.py'
filedist = 'software_online.txt'
logsdir = '/scratch/everyone/lhcbsgm'
logfile = 'online-install-' + str( startTime ) + '.log'
errorfile = 'online-install-' + str( startTime ) + '-error.log'
architecture = ['x86_64-slc5-gcc43-opt', 'x86_64-slc5-gcc43-dbg', 'x86_64-slc5-gcc46-opt', 'x86_64-slc5-gcc46-dbg']

sys.path.insert( 0, mysiteroot )

start = os.getcwd()
catch = os.path.join( logsdir, logfile )
catcherr = os.path.join( logsdir, errorfile )

def printlog( mess, typefile = 'log' ):
  if typefile == 'error':
    fd = open( catcherr, 'a' )
  else:
    fd = open( catch, 'a' )

  fd.write( mess )
  fd.write( '\n' )
  fd.close()

def get_script( scriptname, url = DEFAULT_URL ):
  try:
    result = urllib.urlretrieve( url + scriptname, scriptname )
    if not result:
      printlog( 'Error downloading ' + scriptname + ' ...', 'error' )
      printlog( result, 'error' )
  except Exception, x:
    printlog( 'Exception while downloading', scriptname, str( x ), 'error' )

def removeLock():
    if os.path.exists( filelock ):
      cmd = 'rm -fv ' + filelock
      status, output = commands.getstatusoutput( cmd )
      if DEBUG:
        return printlog( str( status ) + ' ' + output )
    else:
      return printlog( 'No SAM lock file to remove (OK)' )

def installPackage( pname, pversion, pbinary = 'x86_64-slc5-gcc46-opt', sam = 'no', url = DEFAULT_URL ):
  """ Function to check if the package is present in
      the current installation, installs the package if
      not yet there
  """

  start = os.getcwd()
  sc = False
  root = sharedArea
  os.environ['CMTCONFIG'] = pbinary
  cmd = 'mkdir -p ' + root
  result, output = commands.getstatusoutput( cmd )
  os.chdir( os.environ['MYSITEROOT'] )
  get_script( filescript )
  printlog( filescript + ' ' + pname + '' + pversion + ' ' + pbinary + '\n' )
  instCommand = 'python ' + filescript + '  -p ' + pname + ' -v ' + pversion + ' --binary=' + pbinary + ' --latest-data-link -d -s ' + mysiteroot + '/scripts/Setup' + pname + '_' + pversion + '.sh'
  printlog( 'SW Installation command will be:' + instCommand )
  result, output = commands.getstatusoutput( instCommand )
  if result == 0: #orig 1
    printlog( 'Package ' + pname + ' ' + pversion + ' successfully installed' )
    sc = True #all ok
  else:
    printlog( 'Status from ' + filescript + ': ' + str( result ) )
    printlog( 'Output from ' + filescript + ': ' + output )
    sc = False

  os.chdir( start )
  return sc



CONTINUE = 1

while CONTINUE == 1:
  printlog( 50 * '*' )
  printlog( '*  %29s                 *' % "Online  Install v2.0" )
  printlog( '*  Date: %30s          *' % time.asctime() )
  printlog( '*  Host: %30s          *' % socket.gethostname() )
  printlog( 50 * '*' )

  if FORCE_LOCK_REMOVAL:
    printlog( '------------------------------------------------------------------------------------' )
    printlog( 'Deliberately Removing SAM lock file' )
    removeLock()

  printlog( '------------------------------------------------------------------------------------' )
  printlog( 'Current Directory is:' )
  printlog( start )
  printlog( 'Setting PREINSTALLED_SW_INSTALLATION to:' )
  printlog( sharedArea )
  os.environ['PREINSTALLED_SW_INSTALLATION'] = sharedArea
  printlog( '------------------------------------------------------------------------------------' )
  printlog( 'Checking SAM lock file' )
  if os.path.exists( filelock ):
    printlog( 'WARNING: There is another SAM job running at this site', 'error' )
    cmd = 'ls -al ' + filelock
    status, output = commands.getstatusoutput( cmd )
    printlog( output, 'error' )
    curtime = time.time()
    filetime = os.stat( filelock )[8]
    strstat = os.stat( filelock )
    printlog( str( strstat ), 'error' )
    printlog( 'diff time is' + str( filetime ) + ' ' + str( curtime ), 'error' )
    if curtime - filetime > 24 * 60 * 60:
      printlog( 'SAM Lock File present for > 1 day' )
      printlog( 'Removing SAM lock file' )
      removeLock()
    else:
      printlog( 'SAM job running < 1 day, exiting', 'error' )

      CONTINUE = 0
      break

  printlog( '------------------------------------------------------------------------------------' )
  status, output = commands.getstatusoutput( 'whoami' )
  whoami = output.strip()

  printlog( 'Creating new SAM lock file' )
  cmd = 'touch ' + filelock
  status, output = commands.getstatusoutput( cmd )
  printlog( output + ' ' + str( status ) )
  cmd = 'ls -al ' + filelock
  status, output = commands.getstatusoutput( cmd )
  printlog( output )


  printlog( '------------------------------------------------------------------------------------' )
  os.environ['http_proxy'] = 'http://netgw01:8080'
  url = DEFAULT_URL
  result = urllib.urlretrieve( url + filedist, filedist )


  printlog( 'MYSITEROOT: ' + mysiteroot )

  for binary in architecture:
     printlog( '------------------------------------------------------------------------------------' )
     printlog( 'Software checking/installation phase in ' + mysiteroot + ' for ' + binary )
     if os.path.exists( mysiteroot ):
       os.chdir( mysiteroot )
     else:
       os.mkdir( mysiteroot ) # may have been removed by an earlier test
       os.chdir( mysiteroot )

     #filedist = open(start+'/software_online.txt')
     filedistname = open( os.path.join( start, filedist ) )
     fd = filedistname.readlines()
     failedSW = {}
     for line in fd:
       pname = line.split()[0]
       pversion = line.split()[1]
       printlog( '----------------------------' + pname + ' ' + pversion + ' ' + binary )
       #result = checkPackage(pname,pversion,binary)
       result = False
       if result:
         printlog( "%s %s found on system" % ( pname, pversion ) )
       else:
         printlog( 'Attempting to install ' + pname + ' ' + pversion )
         install = installPackage( pname, pversion, binary, 'yes' )
         if not install:
           printlog( pname + ' ' + pversion + ' installation failed.' )
           failedSW[pname] = pversion + '+' + binary
           printlog( '>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>' )
           cmd = "cat " + mysiteroot + "/log/" + pname + "_" + pversion + ".log"
           status, output = commands.getstatusoutput( cmd )
           if DEBUG:
             printlog( cmd )
             printlog( str( status ) )
           printlog( output )
           printlog( '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<' )
         else:
           printlog( "%s %s successfully installed" % ( pname, pversion ) )


     filedistname.close()
     os.chdir( start )

  printlog( '------------------------------------------------------------------------------------' )
  printlog( 'Final Listing of :' + sharedArea )
  cmd = 'ls -al ' + sharedArea
  status, output = commands.getstatusoutput( cmd )
  printlog( output )
  printlog( '------------------------------------------------------------------------------------' )
  printlog( 'Final Listing of ' + mysiteroot + '/log:' )
  cmd = 'ls -al ' + os.path.join( mysiteroot, 'log' )
  status, output = commands.getstatusoutput( cmd )
  printlog( output )

  printlog( '------------------------------------------------------------------------------------' )
  printlog( 'Removing SAM lock file' )
  removeLock()

  printlog( '------------------------------------------------------------------------------------' )
  if failedSW:
    printlog( 'WARNING: Software installation failed for the following packages', 'error' )
    for n, v in failedSW.items():
      t = v.split( '-' )[0]
      printlog( '>>>>>>>> ' + n + ' ' + v.split( '+' )[0] + ' for ' + v.split( '+' )[1], 'error' )


  printlog( '------------------------------------------------------------------------------------' )
  curTime = time.time()
  elapsedTime = curTime - startTime
  printlog( 'Test took %.0f minutes.' % ( elapsedTime / 60 ) )

  printlog( '------------------------------------------------------------------------------------' )
  printlog( 'End of LHCb software installation test.' )
  printlog( '------------------------------------------------------------------------------------' )

  CONTINUE = 0
  break

print "If there is some error, they will be printed here\n"
if os.path.exists( catcherr ):
  fc = open( catcherr )
  for line in fc.readlines():
    print line
  fc.close()

print "Normal log\n"
print startTime

sys.exit( 0 )
