import getopt, os, sys, string, re

argv0 = ""

#--------------------------------------------------------------------------------
class findHeaders :

  def __init__(self, extraincludes, extraexcludes):
    self.ext_include   = [".h"]
    self.ext_include.extend(extraincludes)
    self.ext_exclude   = ["_dict.h"]
    self.ext_exclude.extend(extraexcludes)
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
      elif os.path.isdir(file):
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
          print argv0 + " :ERROR: file " + file + " found more than once"

  def sortClasses(self):
    for fullfile in self.genfiles:
      base = os.path.dirname(fullfile)
      file = string.split(os.path.basename(fullfile),".")[0]
      if not self.genfiles2.has_key(base):
        self.genfiles2[base] = {}
      if not self.genfiles2[base].has_key(file):
        self.genfiles2[base][file] = self.genfiles[fullfile]
      else:
        print argV0 + ": ERROR: two entries for file " + file
        

#--------------------------------------------------------------------------------
class genHeaderDict :

  def __init__(self):
    return
  
  def genDictFile(self, dict):
    s = '<?xml version="1.0" encoding="UTF-8"?>\n'
    s += '<!DOCTYPE gdd SYSTEM "gdd.dtd">\n'
    s += '<gdd version="1.0">\n\n'
    dictkeys = dict.keys()
    dictkeys.sort()
    for base in dictkeys:
      s += '  <package name="%s">\n' % base 
      for file in dict[base]:
        for cl in dict[base][file]:
          s += '    <class author="-" desc="-"'
          if cl != file:
            s += ' filename="%s"' % file
          s += ' name="%s"/>\n' % cl
      s += '  </package>\n\n'
    s += '</gdd>\n'
    return s
    
#--------------------------------------------------------------------------------
def usage():
  print "Something went wrong"
  print "No usage message implemented for the time being"
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
    print argV0 + ": ERROR: file " + file + "does not exist"
  return includes  

def main():
  argv0 = sys.argv[0]
  startdir = os.getcwd()
  includedirs = []
  extraincludes = []
  extraexcludes = []
  try:
    opts, args = getopt.getopt(sys.argv[1:], "hf:I:i:e:", ['help'])
  except getopt.GetoptError:
    usage()
  for o, a in opts:
    if o in ("-h", "--help"):
      usage()
    if o in ("-I"):
      includedirs.append(a)
    if o in ("-f"):
      includedirs.extend(scanfile(a))
    if o in ("-i"):
      extraincludes.append(a)
    if o in ("-e"):
      extraexcludes.append(a)
  if args:
    usage()

  fh = findHeaders(extraincludes, extraexcludes)

  for dir in includedirs:
    if os.path.isdir(dir):
      os.chdir(dir)
      fh.treefiles = []
      fh.fileTree(dir, "")
      fh.allfiles[dir] = fh.treefiles
    else:
      print argv0 + ": ERROR: " + dir + " is not a directory"
      usage()

  fh.searchClasses()
  fh.sortClasses()

  gh = genHeaderDict()

  os.chdir(startdir)

  fp = open("GaudiCppExport.xml", "w")
  fp.write(gh.genDictFile(fh.genfiles2))
  fp.close()


if __name__ == "__main__":
  main()
