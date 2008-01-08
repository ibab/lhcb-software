#!/usr/bin/env python
"""
Script used to install files keeping track of the files that have
been installed, so that at the next installation the file removed
from the source directory will also be removed from the destination
directory.
The script provide also the "uninstall" functionality to remove all
and only the files that it installed for the package.

Command line:

   install.py [-x exclusion1 [-x exclusion2 ...]] [-l logfile] source1 [source2 ...] dest  
   install.py -u [-l logfile] [dest1 ...]
   
@author: Marco Clemencic <marco.clemencic@cern.ch>
"""
from __future__ import generators
_version = "$Id: install.py,v 1.1.1.1 2008-01-08 16:32:42 hmdegaud Exp $"


def main():
    try:
        # optparse is available only since Python 2.3
        from optparse import OptionParser
        parser = OptionParser()
        parser.add_option("-x","--exclude",action="append",
                          metavar="PATTERN", default = [],
                          dest="exclusions", help="which files/directories to avoid to install")
        parser.add_option("-l","--log",action="store",
                          dest="logfile", default="install.log",
                          help="where to store the informations about installed files [default: %default]")
        parser.add_option("-d","--destname",action="store",
                          dest="destname", default=None,
                          help="name to use when installing the source into the destination directory [default: source name]")
        parser.add_option("-u","--uninstall",action="store_true",
                          dest="uninstall", default=False,
                          help="do uninstall")
        #parser.add_option("-p","--permission",action="store",
        #                  metavar="PERM",
        #                  dest="permission",
        #                  help="modify the permission of the destination file (see 'man chown'). Unix only.")
        (opts,args) = parser.parse_args()
    except ImportError:
        # Old style option parsing
        # It must be removed when the support for Python 2.2 is dropped
        from getopt import getopt, GetoptError
        from sys import argv,exit
        class _DummyParserClass:
            def __init__(self):
                self.usage = "usage: install.py [options]"
                self.help = """options:
  -h, --help            show this help message and exit
  -x PATTERN, --exclude=PATTERN
                        which files/directories to avoid to install
  -l LOGFILE, --log=LOGFILE
                        where to store the informations about installed files
                        [default: install.log]
  -d DESTNAME, --destname=DESTNAME
                        name to use when installing the source into the
                        destination directory [default: source name]
  -u, --uninstall       do uninstall"""
            def error(self,msg=None):
                print self.usage + "\n"
                if not msg:
                    msg = self.help
                print msg
                exit(1)
        parser = _DummyParserClass()
        try:
            optlist, args = getopt(argv[1:],"hx:l:d:u",
                                   ["help","exclude","log","destname","uninstall"])
        except GetoptError:
            # print help information and exit:
            parser.error()
        # Dummy option class
        class _DummyOptionsClass:
            def __init__(self):
                # defaults
                self.exclusions = []
                self.uninstall = False
                self.logfile = "install.log"
                self.destname = None
        opts = _DummyOptionsClass()
        for opt,val in optlist:
            if opt in [ "-h", "--help" ]:
                parser.error()
            elif opt in [ "-x", "--exclude" ]:
                opts.exclusions.append(val)
            elif opt in [ "-l", "--log" ]:
                opts.logfile = val
            elif opt in [ "-d", "--destname" ]:
                opts.destname = val
            elif opt in [ "-u", "--uninstall" ]:
                opts.uninstall = True
    # options consistency check
    from pickle import dump,load
    if opts.uninstall:
        if opts.exclusions:
            parser.error("Exclusion list does not make sense for uninstall")
        opts.destination = args
        try:
            log = load(open(opts.logfile,"rb"))
        except:
            log = LogFile() 
        uninstall(log,opts.destination)
        if log:
            dump(log,open(opts.logfile,"wb"))
        else:
            from os import remove
            try:
                remove(opts.logfile)
            except OSError, x:
                if x.errno != 2 : raise
    else : # install mode
        if len(args) < 2:
            parser.error("Specify at least one source and (only) one destination")
        opts.destination = args[-1]
        opts.sources = args[:-1]
        try:
            log = load(open(opts.logfile,"rb"))
        except:
            log = LogFile() 
        install(opts.sources,opts.destination,log,opts.exclusions,opts.destname)
        dump(log,open(opts.logfile,"wb"))

from os import makedirs, listdir, rmdir
from os.path import exists, isdir, getmtime, split, join

try:
    from os import walk
except ImportError:
    def walk(top, topdown=True, onerror=None):
        """Copied from Python 2.3 os.py (see original file for copyright)
        This function has been introduced in Python 2.3, and this copy should
        be removed once the support for Python 2.2 is dropped.
        """
    
        from os.path import join, isdir, islink
    
        # We may not have read permission for top, in which case we can't
        # get a list of the files the directory contains.  os.path.walk
        # always suppressed the exception then, rather than blow up for a
        # minor reason when (say) a thousand readable directories are still
        # left to visit.  That logic is copied here.
        try:
            # Note that listdir and error are globals in this module due
            # to earlier import-*.
            names = listdir(top)
        except error, err:
            if onerror is not None:
                onerror(err)
            return
    
        dirs, nondirs = [], []
        for name in names:
            if isdir(join(top, name)):
                dirs.append(name)
            else:
                nondirs.append(name)
    
        if topdown:
            yield top, dirs, nondirs
        for name in dirs:
            path = join(top, name)
            if not islink(path):
                for x in walk(path, topdown, onerror):
                    yield x
        if not topdown:
            yield top, dirs, nondirs

class LogFile:
    """
    Class to incapsulate the logfile functionalities.
    """
    def __init__(self):
        self._installed_files = {}
        
    def get_dest(self,source):
        try:
            return self._installed_files[source]
        except KeyError:
            return None

    def set_dest(self,source,dest):
        self._installed_files[source] = dest

    def get_sources(self):
        return self._installed_files.keys()
    
    def remove(self,source):
        try:
            del  self._installed_files[source]
        except KeyError:
            pass
    
    def __len__(self):
        return self._installed_files.__len__()
        
def filename_match(name,patterns,default=False):
    """
    Check if the name is matched by any of the patterns in exclusions.
    """
    from fnmatch import fnmatch
    for x in patterns:
        if fnmatch(name,x):
            return True
    return default

def expand_source_dir(source, destination, exclusions = [], destname = None):
    """
    Generate the list of copies. 
    """
    expansion = {}
    src_path,src_name = split(source)
    if destname:
        to_replace = source
        replacement = join(destination,destname)
    else:
        to_replace = src_path
        replacement = destination
    
    for dirname, dirs, files in walk(source):
        if to_replace:
            dest_path=dirname.replace(to_replace,replacement)
        else:
            dest_path=join(destination,dirname)
        # remove excluded dirs from the list
        dirs[:] = [ d for d in dirs if not filename_match(d,exclusions) ]
        # loop over files
        for f in files:
            if filename_match(f,exclusions): continue
            expansion[join(dirname,f)] = join(dest_path,f)
    return expansion

def remove(file):
    from os import remove
    try:
        print "Remove '%s'"%file
        remove(file)
        file_path = split(file)[0]
        while file_path and (len(listdir(file_path)) == 0):
            print "Remove empty dir '%s'"%file_path
            rmdir(file_path)
            file_path = split(file_path)[0]
    except OSError, x: # ignore file-not-found errors
        if x.errno != 2 : raise
    
def update(src,dest,old_dest = None):
    from shutil import copy2
    if (not exists(dest)) or (getmtime(src) > getmtime(dest)):
        dest_path = split(dest)[0]
        if not isdir(dest_path):
            print "Create dir '%s'"%(dest_path)
            makedirs(dest_path)
        # the destination file is missing or older than the source
        print "Copy '%s' -> '%s'"%(src,dest)
        copy2(src,dest) # do the copy (cp -p src dest)
    if old_dest != dest: # the file was installed somewhere else
        # remove the old destination
        if old_dest is not None:
            remove(old_dest)

def install(sources, destination, logfile, exclusions = [], destname = None):
    """
    Copy sources to destination keeping track of what has been done in logfile.
    The destination must be a directory and sources are copied into it.
    If exclusions is not empty, the files matching one of its elements are not
    copied.
    """
    for s in sources:
        src_path, src_name = split(s)
        if not exists(s):
            continue # silently ignore missing sources
        elif not isdir(s): # copy the file, without logging (?)
            if destname is None:
                dest = join(destination,src_name)
            else:
                dest = join(destination,destname)
            old_dest = logfile.get_dest(s)
            update(s,dest,old_dest)
            logfile.set_dest(s,dest) # update log
        else: # for directories
            # expand the content of the directory as a dictionary
            # mapping sources to destinations
            to_do = expand_source_dir(s,destination,exclusions,destname)
            last_done = logfile.get_dest(s)
            if last_done is None: last_done = {}
            for k in to_do:
                try:
                    old_dest = last_done[k]
                    del last_done[k]
                except KeyError:
                    old_dest = None  
                update(k,to_do[k],old_dest)
            # remove files that were copied but are not anymore in the list 
            for old_dest in last_done.values():
                remove(old_dest)
            logfile.set_dest(s,to_do) # update log

def uninstall(logfile, destinations = []):
    """
    Remove copied files using logfile to know what to remove.
    If destinations is not empty, only the files/directories specified are
    removed.
    """
    for s in logfile.get_sources():
        dest = logfile.get_dest(s)
        if type(dest) is str:
            if filename_match(dest,destinations,default=True):
                remove(dest)
                logfile.remove(s)
        else:
            for subs in dest.keys():
                subdest = dest[subs]
                if filename_match(subdest,destinations,default=True):
                    remove(subdest)
                    del dest[subs]
            if not dest:
                logfile.remove(s)
            
if __name__ == "__main__":
    main()
