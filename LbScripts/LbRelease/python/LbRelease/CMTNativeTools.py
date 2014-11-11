###############################################################################
# (c) Copyright 2014 CERN                                                     #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
'''

Class to extract install LCG RPMs and repackage them as classic
LHCb tar files

Created on Mar 25, 2014

@author: Ben Couturier
'''

import os
import re
import sys
import logging
from string import Template
from subprocess import Popen, PIPE

from LbLegacy.Utils import getStatusOutput
from LbUtils.Temporary import TempDir
from LbUtils.CMT.Common import CMTCommand as CMT
from LbUtils.CMT.Common import isCMTMessage

tmpdir = TempDir(prefix="LHCbExternalsRpmSpec")
__log__ = logging.getLogger(__name__)

#
# Utilities imported from mkLCGCMTtar to extract list of native versions
# from dependencies !
#
# BEWARE: This is a copy for code from mkLCGCMtar and it needs a lot of
#         cleanup before proper production.
#
###############################################################################
#  Method added to facilitate the lookup of macro values
def get_macro_value(cmtdir, macro, extratags):
    """ Returns the value of a macro """
    here = os.getcwd()
    if cmtdir != None:
        os.chdir(cmtdir)
    cmd = ["cmt", extratags, "show", "macro_value", macro]
    __log__.debug("get_macro_value - Running: " + " ".join(cmd))
    # Invoking popen to run the command, result is on stdout first line
    p = Popen(" ".join(cmd), stdout=PIPE, stderr=PIPE, shell=True)
    line = p.stdout.readline()[:-1]
    __log__.debug("get_macro_value - %s = %s" % (macro, line))
    if cmtdir != None:
        os.chdir(here)
    return line
#
# BEWARE BEWARE BEWARE BEWARE
# This function leaves you in the cmt directory of the project
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
def get_base_project(native_version):
    NAME = native_version.split('_')[0]
    version = native_version.split('_',1)[1]
    Name = NAME.lower().capitalize()
    if NAME == "LHCBDIRAC" :
        Name = "LHCbDirac"
    if NAME == 'LCGCMT':
        Name = 'LCG'
    if NAME == 'LHCBGRID' :
        Name = 'LHCbGrid'
    if Name == 'Lhcbexternals' :
        Name = 'LHCbExternals'
    NameSys = Name+'Sys'
    if Name == 'Gaudi':
        NameSys = Name+'Release'
    if Name == 'LCG' :
        NameSys = Name+'_Release'
    release_area = Name+'_release_area'
    if os.path.isdir(os.path.join(os.environ[release_area],Name+'Env',version)):
        os.chdir(os.path.join(os.environ[release_area],Name+'Env',version,'cmt'))
        if Name == 'Gaudi': NameSys = Name
    else:
        if not os.environ.has_key('CMTPROJECTPATH'):
            print 'you should set CMTPROJECTPATH first - STOP '
            sys.exit('No CMTPROJECTPATH')
        print 'CMTPROJECTPATH = ',os.environ['CMTPROJECTPATH']
        #print " * Release area k:", release_area
        #print " * Release area  :", os.environ[release_area]
        #print " * Name          :", NAME
        #print " * Native Version:", native_version
        relarea = find_project_in_cmtprojectpath(NAME, native_version)
        #os.chdir(os.path.join(os.environ[release_area],NAME,native_version,'cmt'))
        os.chdir(os.path.join(relarea,NAME,native_version,'cmt'))
    print "get_base_project %s %s %s %s %s" % (NAME, version, Name, NameSys, release_area)
    return NAME, version, Name, NameSys, release_area

def find_project_in_cmtprojectpath(name, version):
    '''
    Method to replace the use of os.environ[release_area]
    It scans the CMTPROJECTPATH to find the right location
    '''
    for e in os.environ['CMTPROJECTPATH'].split(":"):
        epath = os.path.join(e, name, version,'cmt')
        if os.path.exists(epath):
            return e
                          
def get_project_dir(native_version):
    here = os.getcwd()
    NAME, version, Name, NameSys, release_area = get_base_project(native_version)
    #dir = os.path.join(os.environ[release_area],NAME,native_version)
    relarea = find_project_in_cmtprojectpath(NAME, native_version)
    dir = os.path.join(relarea,NAME,native_version)
    os.chdir(here)
    return dir

def get_projectcmt_file(native_version):
    dir = get_project_dir(native_version)
    return os.path.join(dir,'cmt','project.cmt')

def get_runtime_deps(filename):
    deps = dict()
    matchexpr = re.compile("#\s*runtime_use\s+\w+")
    for l in open(filename, "r") :
        if matchexpr.search(l[:-1]) :
            words = l[:-1].replace("#","").split()
            if len(words) < 3 :
                deps[words[1]] = ""
            else :
                deps[words[1]] = words[2]
    return deps

def get_runtime_cmtpath(native_version):
    file = get_projectcmt_file(native_version)
    deps = get_runtime_deps(file)
    cmtpath = []
    for d in deps.keys():
        dir = get_project_dir(deps[d])
        cmtpath.append(dir)
    return ':'.join(cmtpath)

def get_cmtpath(native_version):
    os.environ['CMTPATH'] = get_runtime_cmtpath(native_version)
    status,CMTPATH = getStatusOutput('cmt show set_value CMTPATH')
    if CMTPATH[0] == ':' :
        CMTPATH = CMTPATH[1:]
    os.environ['CMTPATH'] = CMTPATH
    print 'CMTPATH=%s' % CMTPATH
    return CMTPATH

def get_lcg_version(cmtpath):
    for p in cmtpath.split(':') :
        pos = p.find('LCGCMT_')
        if pos != -1 :
            return p[pos:]

def getPackPrefix(pak, packages_versions, with_version=True):
    lcg_ext_loc = os.environ["LCG_external_area"]
    bin_dir = packages_versions[pak][2]
    nat_version = packages_versions[pak][0]
    pack_path = ""
    if len(packages_versions[pak]) > 3 :
        full_pack_path = os.path.normpath(packages_versions[pak][3])
        #print "XXX full_pack_path: ", full_pack_path
        if full_pack_path.startswith(lcg_ext_loc + os.sep) :
            pack_path = full_pack_path.replace(lcg_ext_loc + os.sep, "")
    else :
        # special case for LCGCMT
        pack_path = os.sep.join([pak, nat_version])

    if bin_dir in pack_path :
        pack_path = pack_path[:pack_path.find(bin_dir)]

    if (not with_version) and nat_version :
        pack_path = pack_path[:pack_path.find(nat_version)]

    if pack_path.endswith(os.sep) :
        pack_path = pack_path[:-1]

    return pack_path

def getCMTExtraTags(binary):
    cmtargs = "-tag=LHCb,LHCbGrid"
    if  binary.startswith("win32") or binary.startswith("i686-winxp") :
        cmtargs = "-tag=LHCb,WIN32"
    elif  binary.startswith("win64") or binary.startswith("x86_64-winxp") :
        cmtargs = "-tag=LHCb,WIN64"
    else :
        if binary.find("slc3") != -1 :
            cmtargs = "-tag=LHCb,LHCbGrid,slc3"
    return cmtargs

def getLCGBinary(workdir, extname, binary):
    ext_home = CMT(getCMTExtraTags(binary), "show", "macro_value", "%s_home" % extname, cwd = workdir)[0][:-1]
    nat_version = CMT(getCMTExtraTags(binary), "show", "macro_value", "%s_native_version" % extname, cwd = workdir)[0][:-1]
    cfg_version = CMT(getCMTExtraTags(binary), "show", "macro_value", "%s_config_version" % extname, cwd = workdir)[0][:-1]
    if nat_version :
       ext_bin = None
       ext_bin_list = ext_home[ext_home.find(nat_version):].split(os.sep)
       if len(ext_bin_list) > 1 :
           ext_bin = ext_bin_list[1]
    else :
       ext_bin = ext_home.split(os.sep)[0]
    return nat_version, cfg_version, ext_bin, ext_home

def pkgFilter(NAME, pak, vers, binary):
    keep = True
    if NAME == "GANGA" and binary.find("slc5") != -1 :
        if pak == "Qt" and vers.startswith("3.") :
            keep = False
        if pak == "pyqt" and vers.startswith("3.") :
            keep = False
    if (binary.startswith("win32") or binary.find("winxp") != -1 )and pak == "pyqt_compat" :
        keep = False
    if pak.lower() == "icc" :
        keep = False
    if not keep :
        print "Excluding %s %s for %s" % (pak, vers, binary)
    return keep

def get_native_versions(native_version, binary):

    here = os.getcwd()
    packages_versions = {}
    extra_packages_versions = {}
    
    NAME, version, Name, NameSys, release_area = get_base_project(native_version)
    CMTPATH = get_cmtpath(native_version)
    lcgv = get_lcg_version(CMTPATH)
    relarea = find_project_in_cmtprojectpath(NAME, native_version)
    native_cmt = os.path.join(relarea, NAME, native_version, NameSys, 'cmt')
    if not os.path.exists(native_cmt) :
        native_cmt = os.path.join(relarea, NAME, native_version, NameSys, version, 'cmt')
    os.chdir(native_cmt)
    __log__.debug('get_native_version - %s %s %s %s '%(release_area, native_cmt, os.getenv('CMTPATH'), lcgv))
    if NAME != 'LHCBGRID':
        packages_versions['LCGCMT'] = [lcgv, lcgv, binary]
    cmtshow = "cmt %s show " % getCMTExtraTags(binary)
    cmtcmd = cmtshow +'macros native > '

    # run cmtcmd

    natives = os.path.join(tmpdir.getName(), native_version+'.vers')
    __log__.debug('get_native_version - %s '%(cmtcmd+natives))

    os.system (cmtcmd+natives)

    # get packages_versions
    fd = open(natives)
    fdlines = fd.readlines()
    for fdline in fdlines:
        native = fdline.split('=')[0]
        pack = fdline.split('_native')[0]
        ext_info = getLCGBinary(native_cmt, pack, binary)
        vers = ext_info[0]
        if pkgFilter(NAME, pack, vers, binary) :
            packages_versions[pack] = list(ext_info)

    os.remove(natives)
    
    for pak in packages_versions.keys() :
        if not packages_versions[pak][0] :
            __log__.warning("%s has no version. Removing it from the list" % pak)
            del packages_versions[pak]

    # Code added to cope with the relocation of some packages from
    # external to app/releases (2012/06/14)

    # We first lookup the releases and external paths to be able to compare
    # this is dirty but we have no netter solution for the moment
    LCG_external = os.path.normpath(get_macro_value(None, "LCG_external", getCMTExtraTags(binary)))
    LCG_releases = os.path.normpath(get_macro_value(None, "LCG_releases", getCMTExtraTags(binary)))


    # Iterate over package to check whether they should be kept in external
    # or app/releases. We execute a show macro_value on the package home
    for pak in packages_versions.keys() :
        __log__.info("get_native_version - Checking home for package: %s" %  pak)
        macro = pak+ "_home"
        value = os.path.normpath(get_macro_value(None, macro, getCMTExtraTags(binary)))
        __log__.debug("get_native_version - %s = %s" % ( macro, value))

	# Performing the actual check between LCG_external and LCG_releases and the home of the
        # package
        # BEWARE: LCGCMT_home is set by CMT itself and can be incorrect !
        pakType = None
        if value.startswith(LCG_external):
            pakType = "external"
        if value.startswith(LCG_releases):
            pakType = "app/releases"

        # Compatibility check for OLD packages
        # Force external in this case
        if LCG_external == LCG_releases:
            #print "get_native_version - *** Compatibility mode for OLD packages: setting the type to external ***"
            pakType = "external"

        if pakType is None:
        # Ignoring packages from the system
            if value.startswith("/usr"):
                print "get_native_version - %s home is %s, IGNORING package from system" % (pak, value)
                del packages_versions[pak]
            else:
                print "get_native_version - %s home is %s, not in external or app/releases, WARNING WARNING WARNING" % (pak, value)
                #sys.exit(1)
        else:
            # Adding the pakType to the attribute of the package in the map
            l = packages_versions[pak]
            l.append(pakType)
            print "get_native_version - Including %s package %s\t%s" % (pakType, pak, l[0])

    if binary.startswith("i686"):
        for pak in packages_versions.keys() :
            content = packages_versions[pak]
            if content[2].startswith("x86_64") :
                print "Replacing binary for %s" % pak
                newbin = content[2].replace("x86_64", "i686")
                newpath = content[3].replace("x86_64", "i686")
                packages_versions[pak] = [content[0], content[1], newbin, newpath, content[4]]

    os.chdir(here)
    return (lcgv, packages_versions)


def get_output_filename(native_version, binary, packages_versions):
    here = os.getcwd()
    NAME, version, Name, NameSys, release_area = get_base_project(native_version)
    os.chdir(here)
    plugins_flag = 0
    if plugins_flag == 0:
        if NAME == "LHCBGRID":
            filename = "LCGGrid_" + version
        elif ( NAME == "DIRAC" or NAME == "LHCBDIRAC" ):
            packver = version
            os.chdir(get_project_dir(native_version))
            for line in os.popen("cmt show projects") :
                line = line[:-1]
                if line.find("LHCBGRID_") != -1 :
                    words = line.split()
                    for w in words :
                        if w.startswith("LHCBGRID_") :
                            packver=w.split("_")[1]
            os.chdir(here)
            filename = "LCGGrid_" + packver
        elif NAME == "GANGA" :
            filename = "LCGGanga_" + version
        else:
            filename = packages_versions['LCGCMT'][0]
    else:
        filename = 'PLUGINS'
    return filename


#----------------------------------------------------------------------------
#
#  get GENSER native versions ==============================================
#
def get_genser_native_versions(native_version):
    __log__.debug('Called with arg ' + native_version)
    packages_versions = {}

    # Looking up project properties
    NAME, version, Name, NameSys, release_area = get_base_project(native_version)
    name = Name # Capitalized...
    __log__.debug('release_area = ' +release_area +' '+ name)

    # Look it up properly with this method
    release_area = find_project_in_cmtprojectpath(NAME, native_version)
    CMTPATH = get_cmtpath(native_version)
    lcgv = get_lcg_version(CMTPATH)
    __log__.debug('LCG Version  = ' + lcgv)
    __log__.debug('release_area = ' +release_area +' '+ name)

    Sys_dir = os.path.join(release_area,NAME,native_version,name+'Sys','cmt')
    if not os.path.exists(Sys_dir) :
        Sys_dir = os.path.join(release_area,NAME,native_version,name+'Sys',version,'cmt')
    __log__.debug('SysDir=' + Sys_dir)
    os.chdir(Sys_dir)

    status,value = getStatusOutput('cmt sh macro_value GENSERVERSION')
    if isCMTMessage(value) :
        GENSERVERSION = value.split('\n')[1]
    else:
        GENSERVERSION = value
    packages_versions['GENSER'] = GENSERVERSION
    __log__.debug('GENSERVERSION=' +  GENSERVERSION)

    for fdline in os.popen('cmt show macros native').readlines():
        if fdline.find('config_version') == -1:
            pack = fdline.split('_')[0]
            vers = fdline.split("'")[1]
            packages_versions[pack] = vers

    #packages_homes = {}
    #for p in packages_versions :
    #    packages_homes[p] = os.popen("cmt show macro_value %s_home" % p).read()[:-1]

    os.chdir(release_area)
    return lcgv, packages_versions

#----------------------------------------------------------------------------
#
# Various utils for the mkMCGentar
#
def get_MCGen_base(package_home, genname, genversion):
    return os.path.dirname(os.path.dirname(os.path.dirname(package_home)))
