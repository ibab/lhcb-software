import string, sys, os, shutil

#================================================================================
version = 'v1r2'
#================================================================================
class builder :

#--------------------------------------------------------------------------------
  def __init__(self):

    self.argv0        = "makeDoxygen"
    self.info         = 0
    self.test         = 0
    self.gaudiRoot    = ""
    self.cfgFile      = "DoxyFile.cfg"
    self.dotPath      = "$(LHCBHOME)/bin"
    self.doxyPath     = "$LHCBHOME/bin/doxygen"

#--------------------------------------------------------------------------------
  def usage(self):

    print self.argv0, version
    print
    print self.argv0, 'Builds doxygen web for LHCb software'
    print
    print 'Prerequisites for the script to run properly are:'
    print ' - Python 2.2 or greater'
    print ' - doxygen 1.4 or greater'
    print ' - Correctly set CMT environment for project to be documented'
    print '   (correct CMTPATH, setup.csh correctly sourced)'
    print ' - DoxyFile.cfg in current directory'
    print
    print 'Arguments:'
    print '  -i, --info       print more information while processing'
    print '  -h, --help       print this usage statement'
    print '  -t, --test       use externally defined project xxROOT directories'
    print '                   default is to build directories from CMTPATH'
    print '  -v, --version    print the version of this script'
    print
    print 'Usage example:'
    print '  cd $GAUDISOFT/GAUDI/GAUDI_v15r3/Gaudi/v15r3p3/cmt'
    print '  unsetenv CMTPATH'
    print '  GaudiEnv v15r3'
    print '  source setup.csh'
    print '  cd ../doc'
    print '  python $LHCBHOME/scripts/makedoc.py'
    print
    print 'Please send requests and bug reports to marco.cattaneo@cern.ch'
    print
    sys.exit(0)

#--------------------------------------------------------------------------------
  def argChecker(self, arguments):

    for arg in arguments:
      if arg == '-i' or arg == '--info':
        self.info = 1
      elif arg == '-h' or arg == '--help':
        self.usage()
      elif arg == '-t' or arg == '--test':
        self.test = 1
      elif arg == '-v' or arg == '--version':
        print version
        sys.exit(0)

#--------------------------------------------------------------------------------
  def envChecker(self):
    if not os.path.exists( self.cfgFile ):
      print self.argv0 + ': ' + self.cfgFile + ' does not exist in current directory'
      sys.exit(0)

    if not os.environ.has_key('CMTPATH'):
      print self.argv0 + ': CMTPATH not defined'
      sys.exit(0)
 
#--------------------------------------------------------------------------------
  def getExternals(self):
    temp = os.getcwd()
    gaudiDir = self.gaudiRoot + '/cmt'
    os.chdir(gaudiDir)
    for line in os.popen('cmt show macros native_version').readlines():
      pack = line[0:line.find('_native')]+'VERS'
      vers = line[line.find('=')+2:len(line)-2]
      if self.info : print self.argv0 + ': setting $' + pack + '=' + vers
      os.putenv(pack,vers)

    os.chdir(temp)

# If externaldocs page already exists, remove it to avoid appending to it
    if os.path.exists('html/externaldocs.html'):
      os.remove('html/externaldocs.html')

#--------------------------------------------------------------------------------
  def getProjects(self):
    lhcb = os.getenv('LHCb_release_area')
    if lhcb == None:
      print self.argv0 + ' LHCb_release_area not defined'
      return

    paths = os.getenv('CMTPATH').split(':')
    for path in paths:
      dirs = path.split(os.sep)
      for dir in dirs:
        if dir.find('_') != -1:
          pack = dir[0:dir.find('_')]
          vers = dir[dir.find('_')+1:]
          if self.info : print self.argv0 + ': setting $' + pack + 'VERS=' + vers
          os.putenv(pack+'VERS',vers)

          if pack == 'LCGCMT' : break

          if pack == 'GAUDI'  :
            self.setGaudiRoot(path,vers)
            break

          sysRoot = pack + 'SYSROOT'
          if os.environ.has_key( sysRoot ) and self.test:
            if self.info : print self.argv0 + ': $' + sysRoot + ' already defined as ' + os.getenv(( sysRoot ))

          else:
            if pack == 'LHCB':
              packSys = 'LHCbSys'
            elif pack == 'DAVINCI':
              packSys = 'DaVinciSys'
            else:
              packSys = pack.capitalize() + 'Sys'
            sysPath = lhcb + '/' + pack + '/' + pack + '_' + vers + '/' + packSys + '/' + vers
            if self.info : print self.argv0 + ': setting $' + sysRoot + '=' + sysPath
            os.putenv( sysRoot, sysPath )
          break

#--------------------------------------------------------------------------------
  def setGaudiRoot(self,path,vers):
    temp = os.getenv('GAUDIROOT')
    if temp != None and self.test :
      self.gaudiRoot = temp
      if self.info : print self.argv0 + ': $GAUDIROOT already defined as ' + temp
    else:
      self.gaudiRoot = path + '/Gaudi/' + vers
      if self.info : print self.argv0 + ': setting $GAUDIROOT=' + self.gaudiRoot
      os.putenv('GAUDIROOT',self.gaudiRoot)

#--------------------------------------------------------------------------------
  def docBuilder(self):

    if self.info : print self.argv0 + ': building list of projects'
    self.getProjects()
    self.getExternals()
    if self.info : print

# append the commands to use dot tool    
    shutil.copy( self.cfgFile, 'temp.cfg' )
    tempFile = file( 'temp.cfg', 'a+' )
    tempFile.write('HAVE_DOT = YES\n')
    tempFile.write('DOT_PATH = ' + self.dotPath + '\n' )
    tempFile.write('COLLABORATION_GRAPH = YES\n')
    tempFile.write('GRAPHICAL_HIERARCHY = YES\n')
    tempFile.write('CLASS_GRAPH         = YES\n')
    tempFile.write('INCLUDE_GRAPH       = YES\n')
    tempFile.write('INCLUDED_BY_GRAPH   = YES\n')
# append the commands to document also private and static members    
    tempFile.write('EXTRACT_PRIVATE       = YES\n')
    tempFile.write('EXTRACT_STATIC        = YES\n')
    tempFile.write('EXTRACT_LOCAL_CLASSES = YES\n')
    tempFile.close()
    
    if self.info : print self.argv0 + ': making the doc'
    os.system( self.doxyPath + ' temp.cfg')
    os.remove('temp.cfg')
    if self.info : print

#================================================================================
if ( __name__ == '__main__'):

  arguments = tuple(sys.argv[1:])

  chk = builder()
  chk.argChecker(arguments)
  chk.envChecker()

  chk.docBuilder()
