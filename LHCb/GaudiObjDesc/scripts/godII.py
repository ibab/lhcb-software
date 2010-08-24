import xparser, getopt, sys, os
import tools
import genPackage, genClasses, genClassDicts, genNamespaces, genAssocDicts

#================================================================================
class godII:
#--------------------------------------------------------------------------------
  def __init__(self,args):
    self.fullCommand = (' ').join(args)
    self.tools = tools.tools()
    self.version = 'v8r0'
    self.xmlSources = []
    self.xmlDBFile = ''
    self.xmlDBFileExtra = []
    self.srcOutput = os.curdir
    self.dictOutput = os.curdir
    self.argv0 = args[0]
    self.godRoot = ''
    self.gClasses = 1
    self.gClassDicts = 1
    self.gNamespaces = 1
    self.gAssocDicts = 1
    self.allocatorType = 'FROMXML'
    self.default_namespace = 'LHCb'
    self.parseArgs(args[1:])
#--------------------------------------------------------------------------------
  def usage(self):
    print """
%s %s
    
Usage: %s [options] xml-source(s)
Produce c++ source files and dictionary files from xml descriptions

  options:
  -h             display this help and exit
  -v             display version information and exit
  -g src|dct     produce only sources ('src') or only dictionaries ('dct') 
  -i             add additional file-package-information from './AddImports.txt'
  -s <path>|env  define possible output destination of source code
                   -s <path>   use path
                   -s env      use environment-variable 'GODDOTHOUT'
                    default    use local directory
  -d <path>|env  define possible output destination dictionary code
                   -d <path>   use path
                   -d env      use environment-variable 'GODDICTOUT'
                    default    use local directory
  -x <path>|env  define location of 'GODsClassDB.xml' which contains a
                 dictionary of classes and their corresponding include files
                   -x <path>   use path
                   -x env      use environment-variable 'GODXMLDB'
                    default    use '$(GAUDIOBJDESCROOT)/xml_files/GODsClassDB.xml'
  -r <path>      define the root path to the GOD tools
                   -r <path>   use path
                   default     use environment variable $GAUDIOBJDESCROOT
  -n <namespace> define the default namespace to use if not given in XML
                   -n <namespace> use given namespace
                   default        use namespace LHCb
  --allocator=<type>
                 chose the type of allocator to use. Allowed values are:
                   FROMXML    use what is specified in the XML (default)
                   NO         never overload operators "new" and "delete"
                   BOOST      always use Boost singleton pool
                   BOOST2     always use Boost singleton pool with a check on
                              delete (slower)
                   DEBUG      same as BOOST2 plus debug print-outs on std::cout
                   DEFAULT    alias for BOOST

  xml-source(s):
  can be either one or more directories where all xml files will be parsed
  or one or more xml-files which must have the extension .xml
  """ % (self.argv0, self.version, self.argv0)
#--------------------------------------------------------------------------------
  def parseArgs(self,args):
    try: opts,args = getopt.getopt(args, 'hvg:o:i:s:d:x:r:l:n:',['allocator='])
    except getopt.GetoptError, (e):
      print '%s: ERROR: %s' % (self.argv0, e.msg)
      self.usage()
      sys.exit(1)
    error = 0
    gen = ''
    depricatedOutput = ''
    for o, a in opts :
      if o in ('-h'):
        self.usage()
        sys.exit(0)
      if o in ('-v'):
        print self.version
        sys.exit(0)
      if o in ('-g'):
        if a == 'src':
          gen = a
          self.gClasses      = 1
          self.gNamespaces   = 1
          self.gAssocDicts   = 0
          self.gClassDicts   = 0 
        elif a == 'dct':
          gen = a
          self.gClasses      = 0
          self.gNamespaces   = 0
          self.gAssocDicts   = 1
          self.gClassDicts   = 1
        else:
          print '%s: ERROR: value %s is not allowed in combination with -g' % (self.argv0, a)
          error = 1
      if o in ('-o'):
        depricatedOutput = a
        print '%s: INFO: Option -o depricated. Use -s or -d instead' % self.argv0
      if o in ('-i'):
        self.xmlDBFileExtra.append(a)
      if o in ('-s'):
        if a != 'env' : self.srcOutput = a
        elif os.environ.has_key('GODDOTHOUT') : self.srcOutput = os.environ['GODDOTHOUT']
        else:
          print '%s: ERROR: Option "-s env" used without environment variable GODDOTHOUT declared'
          error = 1
      if o in ('-d'):
        if a != 'env' : self.dictOutput = a
        elif os.environ.has_key('GODDICTOUT') : self.dictOutput = os.environ['GODDICTOUT']
        else:
          print '%s: ERROR: Option "-d env" used without environment variable GODDICTOUT declared'
          error = 1
      if o in ('-x'):
        if a != 'env' : self.xmlDBFile = a
        elif os.environ.has_key('GODXMLDB') : self.xmlDBFile = os.environ['GODXMLDB']
        else:
          print '%s: ERROR: Option -x used without path or environment variable GODXMLDB' % self.argv0
          error = 1
      if o in ('-r'):
        self.godRoot = a + os.sep
      if o in ('--allocator'):
        if len(a) == 0:
          print '%s: ERROR: Option %s used without parameter' % (self.argv0,o)
          error = 1
        else:
          self.allocatorType = a.upper()
          if not self.allocatorType in ("FROMXML","NO","BOOST","BOOST2","DEBUG","DEFAULT"):
            print '%s: ERROR: Allocator type \'%s\' unknown' % (self.argv0,a)
            error = 1
      if o in ('-l'):
        print '%s: INFO: Option -l depricated and not used anymore' % (self.argv0)
      if o in ('-n'):
        if len(a) == 0:
          print '%s: ERROR Option %s used without parameter' % (self.argv0,o)
          error = 1
        else:
          self.default_namespace = a

    self.xmlSources = args
    
    if error:
      self.usage()
      sys.exit(1)

    if not self.godRoot:
      try:
        self.godRoot = os.environ['GAUDIOBJDESCROOT'] + os.sep
      except KeyError:
        print '%s: ERROR: Environment variable GAUDIOBJDESCROOT not set' % self.argv0
        self.usage()
        sys.exit(1)

    if depricatedOutput:
      if   gen == 'src' : self.srcOutput = depricatedOutput
      elif gen == 'dct' : self.dctOutput = depricatedOutput

    if not self.xmlDBFile :
      self.xmlDBFile = self.godRoot + 'xml_files' + os.sep + 'GODsClassDB.xml'
#--------------------------------------------------------------------------------
  def findLongestName(self,godPackage):
    lname = 0
    if self.gAssocDicts and godPackage.has_key('assoc') :
      lname = len(godPackage['attrs']['name'])+22
    if godPackage.has_key('class') :
      classLName = self.tools.getLongestName(godPackage['class'])
      if self.gClasses     : lname = max(lname, classLName+2)
      if self.gClassDicts : lname = max(lname, classLName+9)
    if self.gNamespaces and godPackage.has_key('namespace') :
      lname = max(lname, self.tools.getLongestName(godPackage['namespace'])+2)
    return lname
#--------------------------------------------------------------------------------
  def doit(self):

    #print 'GODII %s' % self.version
    #print self.fullCommand

    x = xparser.xparser()

    srcFiles = []
    for src in self.xmlSources:
      if os.path.isdir(src):
        for file in src:
          if file.split('.')[-1] == 'xml':
            srcFiles.append(file)
      elif os.path.isfile(src) and src.split('.')[-1] == 'xml':
        srcFiles.append(src)
      else :
        print '%s: ERROR: %s passed as source location is neither directory nor a .xml file' % (self.argv0, src)

    if self.gClasses : gClasses = genClasses.genClasses(self.godRoot)
    if self.gNamespaces : gNamespaces = genNamespaces.genNamespaces(self.godRoot)
    if self.gAssocDicts : gAssocDicts = genAssocDicts.genAssocDicts(self.godRoot, self.dictOutput, self.srcOutput)
    
    for srcFile in srcFiles:

      #--->PM<--- Needs to be reset each time! Otherwise it doubles the contents for 'instantiations' and 'exclusions'
      if self.gClassDicts :
        gClassDicts = genClassDicts.genClassDicts(self.godRoot, self.dictOutput, self.srcOutput)
        if not self.gClasses : gClasses = genClasses.genClasses(self.godRoot)

      gdd = x.parseSource(srcFile)
      godPackage = gdd['package'][0]

      # Set default namespace if not set in <package>
      try:
        ns = godPackage['attrs']['namespace']
      except KeyError:
        ns = self.default_namespace
        godPackage['attrs']['namespace'] = ns


      package = genPackage.genPackage(godPackage)

      print 'Processing package ' + package.dict['packagename']

      lname = self.findLongestName(godPackage)

      if godPackage.has_key('assoc') and self.gAssocDicts :
        print '  Generating Dictionaries for Associations'
        gAssocDicts.doit(godPackage)
        print '  - Done'

      if godPackage.has_key('namespace') and self.gNamespaces :
        print '  Generating Namespaces'
        gNamespaces.doit(package,godPackage['namespace'],self.srcOutput,lname)
        print '  - Done'

      if godPackage.has_key('class'):
        if self.gClasses :
          print '  Generating Header Files'
          gClasses.doit(package,godPackage['class'],self.srcOutput,lname,self.allocatorType)
          print '  - Done'
          
        if self.gClassDicts :
          print '  Generating Dictionaries'
          gClassDicts.doit(godPackage)
          print '  - Done'

      print '- Done '
#================================================================================
if __name__ == '__main__':
  g = godII(sys.argv)
  g.doit()
