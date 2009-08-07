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
_version = "$Id: install.py,v 1.2 2009-08-07 16:56:14 hmdegaud Exp $"

import os, sys
from os import (makedirs, listdir, rmdir, walk, sep)
from os.path import (exists, isdir, getmtime, split, join, realpath, dirname,
                     normpath, splitext, splitdrive)
from pickle import (dump, load)
from fnmatch import fnmatch
import itertools, shutil

def main():
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
    parser.add_option("-s","--symlink",action="store_true",
                      dest="symlink", default=False,
                      help="create symlinks instead of copy")
    #parser.add_option("-p","--permission",action="store",
    #                  metavar="PERM",
    #                  dest="permission",
    #                  help="modify the permission of the destination file (see 'man chown'). Unix only.")
    (opts,args) = parser.parse_args()
    
    # options consistency check
    if opts.uninstall:
        if opts.exclusions:
            parser.error("Exclusion list does not make sense for uninstall")
        opts.destination = args
        try:
            log = load(open(opts.logfile,"rb"))
        except:
            log = LogFile() 
        uninstall(log,opts.destination,realpath(dirname(opts.logfile)))
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
        if opts.symlink :
            if len(opts.sources) != 1:
                parser.error("no more that 2 args with --symlink")
            opts.destination, opts.destname = split(opts.destination) 
        install(opts.sources,opts.destination,
                log,opts.exclusions,opts.destname, 
                opts.symlink, realpath(dirname(opts.logfile)))
        dump(log,open(opts.logfile,"wb"))

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
    for x in patterns:
        if fnmatch(name,x):
            return True
    return default

def expand_source_dir(source, destination, exclusions = [], 
                      destname = None, logdir = realpath(".")):
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
            key = getRelativePath(dest_path, join(dirname,f))
            value = getRelativePath(logdir, join(dest_path,f))
            expansion[key] = value
    return expansion

def remove(file, logdir):
    file = normpath(join(logdir, file))
    try:
        print "Remove '%s'"%file
        os.remove(file)
        # For python files, remove the compiled versions too 
        if splitext(file)[-1] == ".py":
            for c in ['c', 'o']:
                if exists(file + c):
                    print "Remove '%s'" % (file+c)
                    os.remove(file+c)
        file_path = split(file)[0]
        while file_path and (len(listdir(file_path)) == 0):
            print "Remove empty dir '%s'"%file_path
            rmdir(file_path)
            file_path = split(file_path)[0]
    except OSError, x: # ignore file-not-found errors
        if x.errno in [2, 13] :
            print "Previous removal ignored"
        else: 
            raise
        

def getCommonPath(dirname, filename):
    # if the 2 components are on different drives (windows)
    if splitdrive(dirname)[0] != splitdrive(filename)[0]:
        return None
    dirl = dirname.split(sep)
    filel = filename.split(sep)
    commpth = []
    for d, f in itertools.izip(dirl, filel):
        if d == f :
            commpth.append(d)
        else :
            break
    commpth = sep.join(commpth)
    if not commpth:
        commpth = sep
    elif commpth[-1] != sep:
        commpth += sep
    return commpth

def getRelativePath(dirname, filename):
    """ calculate the relative path of filename with regards to dirname """
    # Translate the filename to the realpath of the parent directory + basename
    filepath,basename = os.path.split(filename)
    filename = os.path.join(os.path.realpath(filepath),basename)
    # Get the absolute pathof the destination directory
    dirname = os.path.realpath(dirname)
    commonpath = getCommonPath(dirname, filename)
    # for windows if the 2 components are on different drives
    if not commonpath:
        return filename
    relname = filename[len(commonpath):]
    reldir = dirname[len(commonpath):]
    if reldir:
        relname = (os.path.pardir+os.path.sep)*len(reldir.split(os.path.sep)) \
             + relname
    return relname
    
def update(src,dest,old_dest = None, syml = False, logdir = realpath(".")):
    realdest = normpath(join(logdir, dest))
    dest_path = split(realdest)[0]
    realsrc = normpath(join(dest_path,src))
    # The modification time is compared only with the precision of the second
    # to avoid a bug in Python 2.5 + Win32 (Fixed in Python 2.5.1).
    # See:
    #   http://bugs.python.org/issue1671965
    #   http://bugs.python.org/issue1565150
    if (not exists(realdest)) or (int(getmtime(realsrc)) > int(getmtime(realdest))):
        if not isdir(dest_path):
            print "Create dir '%s'"%(dest_path)
            makedirs(dest_path)
        # the destination file is missing or older than the source
        if syml and sys.platform != "win32" :
            if exists(realdest):
                remove(realdest,logdir)
            print "Create Link to '%s' in '%s'"%(src,dest_path)
            os.symlink(src,realdest)
        else:
            print "Copy '%s' -> '%s'"%(src,realdest)
            shutil.copy2(realsrc,realdest) # do the copy (cp -p src dest)
    #if old_dest != dest: # the file was installed somewhere else
    #    # remove the old destination
    #    if old_dest is not None:
    #        remove(old_dest,logdir)

def install(sources, destination, logfile, exclusions = [],
            destname = None, syml = False, logdir = realpath(".")):
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
            src = getRelativePath(destination,s)
            dest = getRelativePath(logdir,dest)
            old_dest = logfile.get_dest(src)
            update(src,dest,old_dest,syml,logdir)
            logfile.set_dest(src,dest) # update log
        else: # for directories
            # expand the content of the directory as a dictionary
            # mapping sources to destinations
            to_do = expand_source_dir(s,destination,exclusions,destname, logdir)
            src = getRelativePath(destination,s)
            last_done = logfile.get_dest(src)
            if last_done is None: last_done = {}
            for k in to_do:
                try:
                    old_dest = last_done[k]
                    del last_done[k]
                except KeyError:
                    old_dest = None  
                update(k,to_do[k],old_dest,syml,logdir)
            # remove files that were copied but are not anymore in the list 
            for old_dest in last_done.values():
                remove(old_dest,logdir)
            logfile.set_dest(src,to_do) # update log

def uninstall(logfile, destinations = [], logdir=realpath(".")):
    """
    Remove copied files using logfile to know what to remove.
    If destinations is not empty, only the files/directories specified are
    removed.
    """
    for s in logfile.get_sources():
        dest = logfile.get_dest(s)
        if type(dest) is str:
            if filename_match(dest,destinations,default=True):
                remove(dest, logdir)
                logfile.remove(s)
        else:
            for subs in dest.keys():
                subdest = dest[subs]
                if filename_match(subdest,destinations,default=True):
                    remove(subdest,logdir)
                    del dest[subs]
            if not dest:
                logfile.remove(s)
            
if __name__ == "__main__":
    main()
