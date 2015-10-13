#!/usr/bin/env python
"""lcg_InstallArea.py: Create symbolic links to external libraries and/or executables in InstallArea for the whole LCGCMT project (default) or an individual package.

Usage: lcg_InstallArea.py [OPTION]...

Mandatory arguments to long options are mandatory for short options too.
  -p, --package=PACKAGE        work on PACKAGE only
  -r, --root=DIR               root directory of PACKAGE (for optimisation)
  -H, --home=DIR               create symlinks to files from directory DIR (defaults to PACKAGE_home macro value)
  -t, --type=TYPE              create symlinks to libraries (TYPE starts with 'lib'), executables (TYPE starts with 'exe'), or both file types otherwise
  -h, --help                   display this help and exit
      --version                output version information and exit
"""
#------------------------------------------------------------------------------
import sys, os, errno, string, getopt, warnings, time, shutil, zlib, fnmatch
import commands
script_version = '080620'


def macro_value(name):
    showCmd="cmt show macro_value "
    (ret, value) = commands.getstatusoutput(showCmd + name)
    if ret:
        return None
    return value




#
#----------------------------------------------------------------------------------------------
#
#----------------------------------------------------------------------------------------------
#
def lcg_install_package(package, package_root, package_home, type=''):
    if not package_root:
        package_root = macro_value(package + "_root")
    if not package_home:
        package_home = macro_value(package + "_home")
        print 'Calculating ' + package + "_home"
    #print 'lcg_install pname %s, pversion %s, binary %s '%(pname,pversion,binary)
    import sys
    # extract major.minor numbers
    _py_version = '.'.join (map(str, sys.version_info[:2]))
  
  #lcgcmt_path = os.path.join(lcg_release_area,pname,pversion)
    lcg_lib = os.path.join(CMTINSTALLAREA,binary,'lib')
    lcg_bin = os.path.join(CMTINSTALLAREA,binary,'bin')
#   lcg_lib = os.path.join(lcgcmt_path,cmt_installarea_prefix,binary,'lib')
#   lcg_bin = os.path.join(lcgcmt_path,cmt_installarea_prefix,binary,'bin')
    lcg_pylib = os.path.join(lcg_lib, 'python%s'%_py_version)
  
    print '============= %s =================' %(package)
    
#    status,package_home = commands.getstatusoutput('cmt show macro_value '+package+'_home')
    if not os.path.isdir(package_home) or package_home.find("/Grid/") >= 0: return
    
    print package_home
    package_dir = package_root
#    package_dir = os.path.join(lcgcmt_path,'LCG_Interfaces',package)
    if not package_dir.find('LCG_Interfaces') >= 0: return
    if not os.path.isdir(package_dir): return
    if type.startswith('lib'):
        lflag, bflag = 1, 0
    elif type.startswith('exe'):
        lflag, bflag = 0, 1
    else:
        lflag, bflag = 1, 1
#     lflag,bflag = get_install_flags(os.path.join(package_dir,'cmt','requirements'))

    # derive lib dir as either 'lib' or 'lib64'
    if lflag and os.path.isdir(os.path.join(package_home,'lib')):
        package_libdir = os.path.join(package_home,'lib')
    elif lflag and os.path.isdir(os.path.join(package_home,'lib64')):    
        package_libdir = os.path.join(package_home,'lib64')
    elif lflag:
        raise
 
    if lflag and package_libdir != None:
      list_libs = fnmatch.filter(os.listdir( package_libdir ),'*.so*')
      list_libs += fnmatch.filter(os.listdir( package_libdir ),'*.dylib')
      list_libs += fnmatch.filter(os.listdir( package_libdir ),'*.rootmap')
      if package_home.find("mysql") >= 0 and os.path.exists(os.path.join( package_libdir, "mysql")):
                     list_libs += fnmatch.filter(os.listdir(os.path.join( package_libdir, "mysql")),'*.so*')
      list_libs = [x for x in list_libs if (not os.path.islink(os.path.join(package_libdir, x))) or (x != os.path.basename(os.readlink(os.path.join(package_libdir, x))))]

      if package_home.find(rootPath) == 0:
          package_home = package_home[len(rootPath):]
          package_home = os.path.normpath((".." + os.path.sep) * (lcg_lib[len(rootPath)+1:-1].count(os.path.sep) + 1 ) + package_home)
      print 'list_libs %s %s '%(len(list_libs), list_libs)
      try:
          os.makedirs(lcg_lib)
      except OSError, exc: 
          if exc.errno == errno.EEXIST:
              pass
          else: raise
      os.chdir(lcg_lib)

      for lib in list_libs:
          print os.path.join(package_libdir, lib)
          try:
              if os.path.exists(os.path.join(package_libdir, lib)): 
                  if os.path.exists(lib): os.remove(lib)
                  os.symlink(os.path.join(package_libdir, lib),lib)
              elif os.path.exists(os.path.join(package_libdir,"mysql", lib)):
                  if os.path.exists(lib): os.remove(lib)
                  os.symlink(os.path.join(package_libdir,"mysql",lib),lib)
              if os.path.splitext(lib) == ".dylib" and \
                     not os.path.exists(lib.replace(".dylib", ".so")):
                  if os.path.exists(lib.replace(".dylib", ".so")):
                      os.remove(lib.replace(".dylib", ".so"))
                  os.symlink(os.path.join(package_home,'lib',lib),
                             lib.replace(".dylib", ".so"))
          except OSError, e:
              print e
              os.system("ls -la %s" % (os.path.join(package_libdir, lib),))
              os.system("ls -la %s" %(lib))
              raise OSError(e)
    if bflag and os.path.isdir(os.path.join(package_home,'bin')):
      list_bins = os.listdir(os.path.join(package_home,'bin'))
      print 'list_bins %s %s '%(len(list_bins),list_bins)
      try:
          os.makedirs(lcg_bin)
      except OSError, exc: 
          if exc.errno == errno.EEXIST:
              pass
          else: raise
      os.chdir(lcg_bin)
      for bin in list_bins:
          if os.path.exists(bin): os.remove(bin)
          os.symlink(os.path.join(package_home,'bin',bin),bin)

    # FIXME: should rely on some CMT pattern picked up from the requirement
    #        file.
    #        Probably something like 'apply_pattern set_cpylib_path' or alike
    # special case of python packages with C-extensions
    if package in ('processing','cx_oracle',):
        _py_exts = os.listdir(os.path.join(package_home,
                                           'lib',
                                           'python%s'%_py_version,
                                           'site-packages'))
        try:
            os.makedirs(lcg_pylib)
        except OSError, exc: 
            if exc.errno == errno.EEXIST:
                pass
            else: raise
        for _py_ext in _py_exts:
            try:
                _symlink_in = os.path.join(package_home,
                                           'lib',
                                           'python%s'%_py_version,
                                           'site-packages',_py_ext)
                #_symlink_in = os.path.realpath(_py_ext)
                _symlink_to = os.path.join (lcg_pylib,_py_ext)
                if os.path.exists(_symlink_to): os.remove(_symlink_to)
                os.symlink (_symlink_in, _symlink_to)
            except (OSError,), err:
                print err
                os.system ("ls -la %s"%_symlink_in)
                os.system ("ls -la %s"%_symlink_to)
                raise
        pass # python C-extensions handling

#    os.chdir(os.path.join(LCG_RELEASEROOT, 'cmt'))   
#
#--------------------------------------------------------------------------------------------------------
def lcg_install():
  #print 'lcg_install pname %s, pversion %s, binary %s '%(pname,pversion,binary)
  import sys
  # extract major.minor numbers
  _py_version = '.'.join (map(str, sys.version_info[:2]))
  
  #lcgcmt_path = os.path.join(lcg_release_area,pname,pversion)
  lcg_lib = os.path.join(lcgcmt_path,cmt_installarea_prefix,binary,'lib')
  lcg_bin = os.path.join(lcgcmt_path,cmt_installarea_prefix,binary,'bin')
  lcg_pylib = os.path.join(lcg_lib, 'python%s'%_py_version)

  for i in (lcg_lib, lcg_pylib, lcg_bin):
      if os.path.isdir(i) : shutil.rmtree(i)
      try:
          os.makedirs(i)
      except OSError, exc: 
          if exc.errno == errno.EEXIST:
              pass
          else: raise
  
  os.chdir(os.path.join(LCG_RELEASEROOT,'cmt'))

  for line in os.popen('cmt show macros home').readlines():
    # line is supposed to be of the form MyPkg_home='/some/path/to/home'
    package = string.split(line,'=')[0]
    # preventively handle packages with '_' in their name
    package = '_'.join(string.split(package,'_')[:-1])
    print '=============%s =================' %(package)
    
    status,package_home = commands.getstatusoutput('cmt show macro_value '+package+'_home')
    if not os.path.isdir(package_home) or package_home.find("/Grid/") >= 0: continue
    
    print package_home

    if package_home.startswith('/usr'):
        print 'Library will come from the system.'
        continue

    package_dir = os.path.join(lcgcmt_path,'LCG_Interfaces',package)
    if not os.path.isdir(package_dir): continue
    lflag,bflag = get_install_flags(os.path.join(package_dir,'cmt','requirements'))
    # derive lib dir as either 'lib' or 'lib64'
    if lflag and os.path.isdir(os.path.join(package_home,'lib')):
        libdir = 'lib'
    elif lflag and os.path.isdir(os.path.join(package_home,'lib64')):
        libdir = 'lib64'
    elif lflag:
        raise

    if package_home.find(rootPath) == 0:
      package_home = package_home[len(rootPath):]
      package_home = os.path.normpath((".." + os.path.sep) * (lcg_lib[len(rootPath)+1:-1].count(os.path.sep) + 1 ) + package_home)

    if lflag and libdir != None:
      os.chdir(lcg_lib)
      list_libs = fnmatch.filter(os.listdir(os.path.join(package_home,libdir)),'*.so*')
      list_libs += fnmatch.filter(os.listdir(os.path.join(package_home,libdir)),'*.dylib')
      list_libs += fnmatch.filter(os.listdir(os.path.join(package_home,libdir)),'*.rootmap')
      if package_home.find("mysql") >= 0 and os.path.exists(os.path.join(package_home,libdir, "mysql")):
                     list_libs += fnmatch.filter(os.listdir(os.path.join(package_home,libdir, "mysql")),'*.so*')
      list_libs = [x for x in list_libs if (not os.path.islink(os.path.join(package_home, libdir,x))) or (x != os.path.basename(os.readlink(os.path.join(package_home,libdir,x))))]
      print 'list_libs %s %s '%(len(list_libs), list_libs)
      
      for lib in list_libs:
          print os.path.join(package_home,'lib',lib)
          try:
              if os.path.exists(os.path.join(package_home,libdir,lib)): 
                  os.symlink(os.path.join(package_home,libdir,lib),lib)
              elif os.path.exists(os.path.join(package_home, libdir, "mysql", lib)):
                  os.symlink(os.path.join(package_home,libdir,"mysql",lib),lib)
              if os.path.splitext(lib) == ".dylib" and \
                     not os.path.exists(lib.replace(".dylib", ".so")):
                  os.symlink(os.path.join(package_home,libdir,lib),
                             lib.replace(".dylib", ".so"))
          except OSError, e:
              print e
              os.system("ls -la %s" % (os.path.join(package_home,libdir,lib),))
              os.system("ls -la %s" %(lib))
              raise OSError(e)
    if bflag and os.path.isdir(os.path.join(package_home,'bin')):
      os.chdir(lcg_bin)
      list_bins = os.listdir(os.path.join(package_home,'bin'))
      print 'list_bins %s %s '%(len(list_bins),list_bins)
      for bin in list_bins:
        os.symlink(os.path.join(package_home,'bin',bin),bin)

    # FIXME: should rely on some CMT pattern picked up from the requirement
    #        file.
    #        Probably something like 'apply_pattern set_cpylib_path' or alike
    # special case of python packages with C-extensions
    if package in ('processing','cx_oracle',):
        _py_exts = os.listdir(os.path.join(package_home,
                                           'lib',
                                           'python%s'%_py_version,
                                           'site-packages'))
        for _py_ext in _py_exts:
            try:
                _symlink_in = os.path.join(package_home,
                                           'lib',
                                           'python%s'%_py_version,
                                           'site-packages',_py_ext)
                #_symlink_in = os.path.realpath(_py_ext)
                _symlink_to = os.path.join (lcg_pylib,_py_ext)
                os.symlink (_symlink_in, _symlink_to)
            except (OSError,), err:
                print err
                os.system ("ls -la %s"%_symlink_in)
                os.system ("ls -la %s"%_symlink_to)
                raise
        pass # python C-extensions handling

    os.chdir(os.path.join(LCG_RELEASEROOT, 'cmt'))   
#
#--------------------------------------------------------------------------------------------------------
#
def get_install_flags (requirements):
 # print ' get_install_flags - ',requirements
  libflag = 0
  binflag = 0
  
  req = open(requirements,'r')
  for line in req.readlines():
    if line.find('#') != -1 : line = line[:line.find('#')]
    if line.find('set_lib_path') != -1 or line.find('set_ext_lib_path') != -1: libflag = 1
    if line.find('set_bin_path') != -1 or line.find('set_ext_bin_path') != -1: binflag = 1
    if line.find('set_bin_and_lib_path') != -1 or line.find('set_ext_bin_and_lib_path') != -1 :
      libflag = 1
      binflag = 1
    
  req.close()
#  print 'libflag %s, binflag %s '%(libflag,binflag)
  return libflag,binflag
#
#------------------------------------------------------------------------------
#
if __name__ == "__main__":

    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   "hp:r:H:t:",
                                   ["help", "version", "package=", "root=", "home=", "type="])
    except getopt.error, e:
        print >>sys.stderr, '%s: %s' % (sys.argv[0], str(e))
        print >>sys.stderr, "Try '%s --help' for more information." % sys.argv[0]
        sys.exit(1)

    package = None
    package_root = None
    package_home = None
    type = 'all'
    for o, v in opts:
        if o in ("-h", "--help"):
            print sys.modules[__name__].__doc__
            sys.exit(0)
        elif o in ("--version",):
            print '%s %s' % (os.path.basename(sys.argv[0]), script_version)
            sys.exit(0)
        elif o in ("-p", "--package"):
            package = v
        elif o in ("-r", "--root"):
            package_root = v
        elif o in ("-H", "--home"):
            package_home = v
        elif o in ("-t", "--type"):
            type = v

    CMTINSTALLAREA = macro_value("CMTINSTALLAREA")
    if not CMTINSTALLAREA:
        print '%s %s No CMTINSTALLAREA defined, exiting ...' % (os.path.basename(sys.argv[0]), script_version)
        sys.exit(1)

    # set binary directory
    binary = os.environ['CMTCONFIG']

    if package:
        myPath = macro_value(package + "_cmtpath")
        rootPath = os.path.abspath(os.path.join(myPath, "..", ".."))
        lcg_install_package(package, package_root, package_home, type)
    else:
        lcgcmt_installarea=macro_value("lcgcmt_installarea")
        if lcgcmt_installarea == "without_installarea": 
            print "Project is build with without_installarea macro set"
            sys.exit()  

        cmt_installarea_prefix=macro_value("cmt_installarea_prefix")
        LCG_RELEASEROOT = macro_value("LCG_RELEASEROOT")
        # rootPath = macro_value("LCG_releases")
        myPath = macro_value("LCG_Release_cmtpath")
        rootPath = os.path.abspath(os.path.join(myPath, "..", "..", "..", ".."))

        debug_flag = 1

        lcgcmt_path = CMTINSTALLAREA[:CMTINSTALLAREA.find(cmt_installarea_prefix)]
  
        # print action list
        print '+++++++++++++++++++++++ start lcg_InstallArea.py - version no ',script_version

        # restore current directory at program's exit
        import atexit
        atexit.register (os.chdir, os.getcwd())

        # run install script
        lcg_install()
        print '+++++++++++++++++++++++ end lcg_InstallArea.py -version no ',script_version

