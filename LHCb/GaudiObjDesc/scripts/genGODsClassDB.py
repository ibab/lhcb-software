import getopt, os, sys, string, re

argv0 = sys.argv[0]

#--------------------------------------------------------------------------------
class findHeaders :

  def __init__(self, extraExtIncludes, extraExtExcludes, extraDirExcludes):
    self.ext_include   = extraExtIncludes
    self.ext_exclude   = extraExtExcludes
    self.dir_exclude   = extraDirExcludes
    self.allfiles      = {}
    self.treefiles     = []
    self.clpatt        = re.compile('class\s+([\w]+)\s*(?:(?::[^:])|(?:{))')
    self.genfiles      = {}
    self.genfiles2     = {}

  def isHeaderFile(self, name):
    for inc in self.ext_include:
      linc = len(inc)
      if name[len(name)-linc:] == inc:
        for exc in self.ext_exclude:
          lexc = len(exc)
          if name[len(name)-lexc:] != exc:
            return 1
    return 0
    
  def fileTree(self, dir, base):
    for file in os.listdir(dir):
      if os.path.isfile(file) and self.isHeaderFile(file):
        self.treefiles.append(base + file)
      elif os.path.isdir(file) and file not in self.dir_exclude:
        base += file + os.sep
        os.chdir(file)
        self.fileTree(os.curdir, base)
        os.chdir(os.pardir)
        base = string.join(string.split(base, os.sep)[:-2],os.sep)
        if (len(base) and base[-1] != os.sep) : base += os.sep

  def searchClasses(self):
    for dir in self.allfiles.keys():
      os.chdir(dir)
      for file in self.allfiles[dir]:
        clin = open(file, 'r')
        str = clin.read()
        classes = re.findall(self.clpatt, str)
        clin.close()
        classes.sort()
        if "" in classes:
          del classes[classes.index(""):classes.count("")]
        if len(classes) and not self.genfiles.has_key(file):
          map(string.strip,classes)
          self.genfiles[file] = classes
        elif self.genfiles.has_key(file):
          print argv0 + ': WARNING: file ' + file + ' found more than once'

  def sortClasses(self):
    for fullfile in self.genfiles:
      base = os.path.dirname(fullfile)
      file = string.split(os.path.basename(fullfile),".")[0]
      if not self.genfiles2.has_key(base):
        self.genfiles2[base] = {}
      if not self.genfiles2[base].has_key(file):
        self.genfiles2[base][file] = self.genfiles[fullfile]
      else:
        print argv0 + ": WARNING: two entries for file " + file
        

#--------------------------------------------------------------------------------
class genHeaderDict :

  def __init__(self):
    return
  
  def genDictFile(self, dict):
    s =  '<?xml version="1.0" encoding="UTF-8"?>\n\n'
    s += '<!DOCTYPE GODsClassDB [\n'
    s += '  <!ELEMENT GODsClassDB (class+)>\n'
    s += '  <!ATTLIST GODsClassDB version CDATA #REQUIRED>\n'
    s += '  <!ELEMENT class EMPTY>\n'
    s += '  <!ATTLIST class  name CDATA #REQUIRED  file CDATA #REQUIRED>\n'
    s += ']>\n\n'
    s += '<GODsClassDB version="1.0">\n'
    dictkeys = dict.keys()
    dictkeys.sort()
    for base in dictkeys:
      s += '\n  <!-- classes of package %s -->\n' % base 
      for file in dict[base]:
        for cl in dict[base][file]:
          s += '  <class name="%s" file="%s"/>\n' % (cl, base+os.sep+file)
    s += '\n</GODsClassDB>\n'
    return s
    
#--------------------------------------------------------------------------------
def usage():
  print 
  print argv0, 'generates the header file data base used by GOD'
  print
  print '  Usage: python %s [options] ' % argv0
  print """
  Options:

  -h, --help  print this usage message
  -d          use default patterns for inclusion and exclusion, these are
              default file includes: *.h
              default file excludes: *_dict.h
              default dir excludes : src, test
  -I          top directories to scan for include files
  -f          pass a file containing directories to search
  -i          add extra include file patterns
  -e          add extra exclude file patterns
  -d          add extra directories to exclude

  Tip: To produce a database for the LHCb project one may pass the output of
       'cmt show macro_value includes' of the LHCbSys package to the script.

  Please report any problems, suggestions, error(s) to stefan.roiser@cern.ch
  
  """
  sys.exit()

def scanfile(file):
  includes = []
  if os.path.isfile(file):
    f = open(file,'r')
    for line in f:
      if len(line.strip()):
        includes.append(line.strip())
    f.close()
  else:
    print argv0 + ": ERROR  : file " + file + " does not exist"
    usage()
  return includes  

def main():
  startdir = os.getcwd()
  includedirs = []
  extraExtIncludesDef = ['.h']
  extraExtExcludesDef = ['_dict.h']
  extraDirExcludesDef = ['src','test']
  extraExtIncludes = []
  extraExtExcludes = []
  extraDirExcludes = []
  try:
    opts, args = getopt.getopt(sys.argv[1:], "hf:I:i:e:d", ['help'])
  except getopt.GetoptError, e:
    print argv0 + ': ERROR  :',
    print e
    usage()
  for o, a in opts:
    if o in ('-h', '--help'):
      usage()
    if o in ('-d'):
      extraExtIncludes.extend(extraExtIncludesDef)
      extraExtExcludes.extend(extraExtExcludesDef)
      extraDirExcludes.extend(extraDirExcludesDef)
    if o in ('-I'):
      includedirs.append(a)                       # directory to search
    if o in ('-f'):
      includedirs.extend(scanfile(a))             # file containing directories to search
    if o in ('-i'):
      extraExtIncludes.append(a)                  # file patterns to include 
    if o in ('-e'):
      extraExtExcludes.append(a)                  # file patterns to exclude
    if o in ('-d'):
      extraDirExcludes.append(a)
  if args:
    usage()

  fh = findHeaders(extraExtIncludes, extraExtExcludes, extraDirExcludes)

  for dir in includedirs:
    if os.path.isdir(dir):
      if dir[-1] != os.sep : dir += os.sep
      print argv0 +  ": INFO   : scanning directory " + dir
      os.chdir(dir)
      fh.treefiles = []
      fh.fileTree(dir,'')
      fh.allfiles[dir] = fh.treefiles
    else:
      print argv0 + ": WARNING: " + dir + " is not a directory"

  print argv0 + ': INFO   : found %d header matching patterns ' % sum(map(len,fh.allfiles.values()))

  print argv0 + ': INFO   : Searching for classes in files'
  fh.searchClasses()
  print argv0 + ': INFO   : Sorting classes'
  fh.sortClasses()

  print argv0 + ': INFO   : generating xml file'
  gh = genHeaderDict()

  os.chdir(startdir)

  print argv0 + ': INFO   : writing xml file'
  fp = open("GODsClassDB.xml", "w")
  fp.write(gh.genDictFile(fh.genfiles2))
  fp.close()
  print argv0 + ': INFO   : database contains %d number of classes ' % sum(map(len,fh.genfiles2.values()))

  print argv0 + ': INFO   : Done'

if __name__ == "__main__":
  main()
