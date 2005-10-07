# Copyright CERN, CH-1211 Geneva 23, 2004-2005, All rights reserved.
#
# Permission to use, copy, modify, and distribute this software for any
# purpose is hereby granted without fee, provided that this copyright and
# permissions notice appear in all copies and derivatives.
#
# This software is provided "as is" without express or implied warranty.

import sys, os, gendict, selclass, gencapa, string, getopt, gendict2

class lcgdict:
#----------------------------------------------------------------------------------
  def __init__(self,
               files = [],
               cppopt='',
               slctor=None,
               dictVersion='reflection'):
    self.files           = files
    self.output          = None
    self.outputDir       = None
    self.outputFile      = None
    self.capabilities    = None
    self.select          = None
    self.cppopt          = ''
    self.deep            = False
    self.split           = False
    self.opts            = {}
    self.dictVersion     = dictVersion
    self.gccxmlpath      = None
    self.selector        = slctor
    self.gccxml          = ''
#----------------------------------------------------------------------------------
  def usage(self) :
    print 'Usage:'
    print '  lcgdict headerfile1.h [headerfile2.h] [options] [preprocesor options]'
    print 'Try "lcgdict --help" for more information.'
    sys.exit()
#----------------------------------------------------------------------------------
  def help(self) :
    print """Generates the LCG dictionary file for each header file\n
    Usage:
      lcgdict headerfile1.h [headerfile2.h] [options] [preprocesor options]\n    
    Options:
      -s <file>, --selection_file=<file>
         Class selection file to specify for which classes the dictionary
         will be generated
         Format (XML):
           <lcgdict>
           [<selection>]
             <class [name="classname"] [pattern="wildname"] 
                    [file_name="filename"] [file_pattern="wildname"] 
                    [id="xxxx"] [type="vector"]/>
             <class name="classname" >
               <field name="m_transient" transient="true"/>
               <field name="m_anothertransient" transient="true"/>
               <properties prop1="value1" [prop2="value2"]/>
             </class>
             <function [name="funcname"] [pattern="wildname"] />
           [</selection>]
           <exclusion>
             <class [name="classname"] [pattern="wildname"] />
               <method name="unwanted" />
             </class>
           ...
           </lcgdict>\n
      -o <file>, --output <file>
         Output file name. If an existing directory is specified instead of a file,
         then a filename will be build using the name of the input file and will
         be placed in the given directory. <headerfile>_dict.cpp \n
      --reflex
         Generate dictionary code for compliant to Reflex\n
      --pool
         Generate minimal dictionary required for POOL persistency\n
      --deep
         Generate dictionary for all dependend classes\n
      --split
         Generate separate file for stub functions. Option sometimes needed on Windows.\n
      --comments
         Add end-of-line comments in data and functions members as a property called "comment" \n
      --no_membertypedefs
         Disable the definition of class member typedefs \n
      --gccxmlpath=<path>
         Path path where the gccxml tool is installed.
         If not defined the standard PATH environ variable is used\n
      -c <file>, --capabilities=<file>
         Generate the capabilities file to be used by the SEAL Plugin Manager. This file
         lists the names of all classes for which the reflection is formation is provided.\n
      -h, --help
         Print this help\n
     """ 
    sys.exit()
#----------------------------------------------------------------------------------
  def parse_args(self) :
    options = []
    #----Ontain the list of files to process------------
    for a in sys.argv[1:] :
      if a[0] != '-' :
        self.files.append(a)
      else :
        options = sys.argv[sys.argv.index(a):]
        break
    #----Process options--------------------------------
    try:
      opts, args = getopt.getopt(options, 'ho:s:c:I:U:D:PC', \
      ['help','output=','selection_file=','pool','deep','split','gccxmlpath=',
       'capabilities=','reflex','comments','no_membertypedefs'])
    except getopt.GetoptError:
      self.usage()
      sys.exit(2)
    self.output = '.'
    self.select = None
    self.gccxmlpath = None
    self.cppopt = ''
    self.pool   = 0
    for o, a in opts:
      if o in ('-h', '--help'):
        self.help()
        sys.exit()
      if o in ('-o', '--output'):
        self.output = a
      if o in ('-s', '--selection_file'):
        self.select = a
      if o in ('--reflex',):
        self.dictVersion = 'reflex'
      if o in ('--pool',):
        self.opts['pool'] = True
      if o in ('--deep',):
        self.deep = True
      if o in ('--split',):
        self.split = True
      if o in ('--comments',):
        self.opts['comments'] = True
      if o in ('--no_membertypedefs',):
        self.opts['no_membertypedefs'] = True
      if o in ('--gccxmlpath',):
        self.gccxmlpath = a
      if o in ('-c', '--capabilities'):
        self.capabilities = a
      if o in ('-I', '-U', '-D', '-P', '-C') :
        self.cppopt += o + a +' '
#----------------------------------------------------------------------------------
  def check_files_dirs(self):
    #---Check existance of input files--------------------
    if self.files :
      for f in self.files :
        if not os.path.exists(f) : 
          print 'C++ file "' + f + '" not found'
          self.usage()
    else :
      print 'No input file specified'
      self.usage()
    #---Check existance of output directory----------------
    if os.path.isdir(self.output) :
      self.outputDir  = self.output
      self.outputFile = None
    else :
      self.outputDir, self.outputFile = os.path.split(self.output)
    if self.outputDir and not os.path.isdir(self.outputDir) :
      print 'Output directory ', self.outputDir, ' not found'
      self.usage()
    #---Hande selection class file-------------------------
    classes = []
    if self.select :
      if not os.path.exists(self.select) :
        print 'Class selection file "' + self.select + '" not found'
        self.usage()
      for l in open(self.select).readlines() : classes.append(l[:-1])
    #----------GCCXML command------------------------------
    if self.gccxmlpath :
      self.gccxml = self.gccxmlpath + os.sep + 'gccxml'
    elif self.which('gccxml') :
      self.gccxml = 'gccxml'
    else :
      if sys.platform == 'win32' :
        self.gccxml = r'\\cern.ch\dfs\Experiments\sw\lcg\external\gccxml\0.6.0\win32_vc71\gccxml'
      else :
        self.gccxml = '/afs/cern.ch/sw/lcg/external/gccxml/0.6.0_patch1/rh73_gcc323/bin/gccxml'
    #---------------Open selection file-------------------
    try :
      if self.select : self.selector = selclass.selClass(self.select,parse=1)
      else           : self.selector = None
    except :
      sys.exit(1)
#----------------------------------------------------------------------------------
  def process_files(self):
    file_extension = '_rflx.cpp'
    if self.dictVersion == 'reflection' : file_extension = '_dict.cpp'
    #----------Loop oover all the input files--------------
    for source in self.files :
      path, fullname = os.path.split(source)
      name = fullname[:fullname.find('.')]
      xmlfile = os.path.join(self.outputDir,name+'.xml')
      if( self.outputFile ) :
        dicfile = os.path.join(self.outputDir,self.outputFile)
      else :
        dicfile = os.path.join(self.outputDir,name+file_extension)
      if self.split :
        sdicfile = dicfile[:dicfile.rfind(file_extension)]+'stubs'+file_extension
      else :
        sdicfile = None
      #---------------Parse the header file with GCC_XML
      cmd  = '%s %s -fxml=%s %s' %(self.gccxml, source, xmlfile, self.cppopt)
      print 'Parsing file %s with GCC_XML' % source,
      status = os.system(cmd)
      if status :
        print 'Error processing file with gccxml. Lcgdict command failed.'
        sys.exit(1)
      else: print 'OK'
     #---------------Generate the dictionary---------------
      print 'Generating LCG Dictionary',
      if ( self.dictVersion == 'reflex' ) : print '(reflex)'
      else : print
      if ( self.dictVersion == 'reflex' ) :
        dg = gendict2.genDict2(source, self.opts)
      else :
        dg = gendict.genDict(source, self.opts.get('pool', False))
      dg.parse(xmlfile)
      classes   = dg.selclasses(self.selector, self.deep)
      functions = dg.selfunctions(self.selector)
      cnames  = dg.generate(dicfile, sdicfile, classes, functions )
    #------------Produce Seal Capabilities source file------
      if self.capabilities :
        if os.path.isdir(self.capabilities) :
          capfile = os.path.join(self.capabilities, 'capabilities.cpp')
        else :
          capfile = os.path.join(self.outputDir, self.capabilities)
        gencapa.genCapabilities(capfile, name,  cnames, self.dictVersion)
    #------------Report unused class selections in selection
    if self.selector : self.selector.reportUnusedClasses()
#---------------------------------------------------------------------
  def which(self, name) :
    if 'PATH' in os.environ :
      if sys.platform == 'win32' : name += '.exe'
      for p in os.environ['PATH'].split(os.pathsep) :
        path = os.path.join(p,name)
        if os.path.exists(path) : return path
    return None
#---------------------------------------------------------------------
if __name__ == "__main__":
  l = lcgdict()
  l.parse_args()
  l.check_files_dirs()
  l.process_files()
