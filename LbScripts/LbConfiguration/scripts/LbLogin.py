#!/usr/bin/env python


from imp import find_module
from SetupProject import SetupProject

import sys
import os

# bootstraping the location of the file
_this_file = find_module(os.path.splitext(os.path.basename(__file__))[0])[1]
_scripts_dir = os.path.dirname(_this_file)
_base_dir = os.path.dirname(_scripts_dir)
# updating the sys.path for the bare minimum of the available scripts
sys.path.append(_scripts_dir)
sys.path.append(os.path.join(_base_dir, "python"))

from LbUtils.Script import Script
from LbUtils.Env import Environment, Aliases
from tempfile import mkstemp
import sys, os, logging
import re
import shutil


def getLbLoginEnv(debug=False, 
                  targetshell="csh", mysiteroot=None,
                  cmtconfig=None, userarea=None,
                  cmtvers="v1r20p20070208", sharedarea=None,
                  cmtsite=None):
    s = LbLoginScript()
    s.parseOpts("")
    s.options.targetshell = targetshell
    s.options.mysiteroot = mysiteroot
    s.options.cmtconfig = cmtconfig
    s.options.userarea = userarea
    s.options.cmtvers = cmtvers
    s.options.sharedarea = sharedarea
    s.options.cmtsite = cmtsite

    return s.setEnv(debug)

def _check_output_options_cb(option, opt_str, value, parser):
    if opt_str == "--mktemp":
        if parser.values.output:
            raise OptionValueError("--mktemp cannot be used at the same time as --output")
        parser.values.mktemp = True
    elif opt_str == "--output":
        if parser.values.mktemp:
            raise OptionValueError("--mktemp cannot be used at the same time as --output")
        parser.values.output = value



class LbLoginScript(Script):
    _version = "fake version"
    
    def __init__(self, usage=None, version=None):
        Script.__init__(self, usage, version, help_output=sys.stderr)
        self._env = Environment()
        self._aliases = Aliases()
        self._extra = ""
        self.platform = ""
        self.binary   = ""
        self.compdef  = ""
        self.output_file = None
    def _write_script(self, env):
        """ select the ouput stream according to the cmd line options """
        close_output = False
        if self.options.output:
            self.output_file = open(self.options.output,"w")
            self.options.output = None # reset the option value to avoid to reuse it
            close_output = True
        elif self.options.mktemp:
            fd, outname = mkstemp()
            self.output_file = os.fdopen(fd,"w")
            print outname
            self.options.mktemp = None # reset the option value to avoid to reuse it
            close_output = True
        else :
            self.output_file = sys.stdout
            close_output = False
        # write the data
        self.output_file.write(env)
        self.output_file.write("\n") # @todo: this may be avoided
        if close_output: 
            self.output_file.close()
    def _add_echo(self, line):
        if line[-1] == "\n" :
            line = line[:-1]           
        outline = "echo '%s'\n" % line
        
        self._extra += outline
            
    def defineOpts(self):
        """ define commandline options """
        parser = self.parser
        parser.set_defaults(targetshell="csh")
        parser.add_option("--shell", action="store", type="choice", metavar="SHELL",
                          dest = "targetshell",
                          choices = ['csh','sh','bat'],
                          help="(internal) select the type of shell to use")
        parser.set_defaults(mktemp=False)
        parser.add_option("--mktemp", action="callback",
                          callback = _check_output_options_cb,
                          help="(internal) send the output to a temporary file and print on stdout the file name (like mktemp)")
        parser.set_defaults(output=None)
        parser.add_option("--output", action="callback", metavar="FILE",
                          type = "string", callback = _check_output_options_cb,
                          help="(internal) output the command to set up the environment ot the given file instead of stdout")
        parser.set_defaults(mysiteroot=None)
        parser.add_option("-m", "--mysiteroot",
                          dest="mysiteroot",
                          help="set MYSITEROOT", 
                          fallback_env="MYSITEROOT")
#        parser.add_option("-m", "--mysiteroot",
#                          dest="mysiteroot",
#                          help="set MYSITEROOT", 
#                          fallback_env="MYSITEROOT" ,
#                          fallback_conf=os.getcwd())
        parser.set_defaults(cmtconfig=None)
        parser.add_option("-c", "--cmtconfig",
                          dest="cmtconfig",
                          help="set CMTCONFIG", 
                          fallback_env="CMTCONFIG")
        parser.set_defaults(userarea=None)
        parser.add_option("-u", "--userarea",
                          dest="userarea",
                          help="set User_release_area", 
                          fallback_env="User_release_area")
        parser.set_defaults(cmtvers="v1r20p20070208")
        parser.add_option("--cmtvers",
                          dest="cmtvers",
                          help="set CMT version")
        parser.set_defaults(sharedarea=None)
        parser.add_option("-s", "--shared",
                          dest="sharedarea",
                          help="set the shared area", 
                          fallback_env="VO_LHCB_SW_DIR")
        parser.set_defaults(cmtsite=None)
        parser.add_option("--cmtsite",
                          dest="cmtsite",
                          help="set the CMTSITE", 
                          fallback_env="CMTSITE")

#-----------------------------------------------------------------------------------

    def setPath(self):
        opts = self.options
        ev = self._env
        if sys.platform != "win32" :
            if ev.has_key("SAVEPATH") :
                if ev["PATH"] != ev["SAVEPATH"] :
                    ev["PATH"] = ev["SAVEPATH"]

    def setCVSEnv(self):
        """ CVS base setup """
        if sys.platform == "win32" :
            method = "ext"
        else :
            method = "kserver"
        server = "isscvs.cern.ch"
        self._env["GAUDIKSERVER"] = ":%s:%s:/local/reps/Gaudi" % (method, server)
        self._env["LHCBKSERVER"] = ":%s:%s:/local/reps/lhcb" % (method, server)



    def setSite(self):
        """ Site massaging """
        opts = self.options
        ev = self._env
        if opts.mysiteroot :
            ev["MYSITEROOT"] = opts.mysiteroot
            ev["SITEROOT"] = opts.mysiteroot
            ev["CMTSITE"] = "LOCAL"
            opts.cmtsite = "LOCAL"
        else :
            if sys.platform != "win32" :
                afsroot = "/afs"
            else :
                if ev.has_key("AFSROOT") :
                    afsroot = ev["AFSROOT"]
                else : 
                    afsroot = "Z:"
            cernbase = "cern.ch"
            cernroot = os.path.join(afsroot, cernbase)
            if opts.cmtsite == "LOCAL" :
                opts.mysiteroot = os.getcwd()
                self.setSite()
            elif opts.cmtsite == "CERN" :
                ev["CMTSITE"] = "CERN"
                ev["AFSROOT"] = afsroot 
                ev["SITEROOT"] = cernroot
                if ev.has_key("MYSITEROOT") :
                    del ev["MYSITEROOT"]
            elif os.path.exists(cernroot) and os.path.isdir(cernroot) :
                opts.cmtsite = "CERN"
                self.setSite()
            else : 
                self._add_echo(' the MYSITEROOT variable is not defined')
                self._add_echo(' we suggest you install all software under $MYSITEROOT')
                self._add_echo(' then LHCb software will be installed under $MYSITEROOT/lhcb')
                self._add_echo('      LCG software will be installed under $MYSITEROOT/lcg/external')
                self._add_echo('      CMT and OpenScientist will be installed under $MYSITEROOT/contrib')
                self._add_echo( ' as an example ')
                self._add_echo(' setenv $MYSITEROOT /home/ranjard/Install')

#-----------------------------------------------------------------------------------
# Core CMT business

    def setCMTBin(self):
        ev = self._env
        if sys.platform != "win32" :
            m1 = os.popen("uname").read()[:-1]
            m2 = os.popen("uname -m").read()[:-1].strip()
            ev["CMTBIN"] = "%s-%s" % (m1, m2)
        else : 
            ev["CMTBIN"] = "VisualC"

    def setCMTSystem(self):
        ev = self._env
        if sys.platform != "win32" :
            if ev.has_key("UNAME") :
                uname = ev["UNAME"]
            else :
                uname = os.popen("uname").read()[:-1]
            may_use_afs = True
            if uname == "Darwin" :
                may_use_afs = False
            if uname == "Linux" or uname == "LynxOS" :
                uname2 = os.popen("uname -m").read()[:-1].strip()
                system = "%s-%s" % (uname, uname2)
            elif uname == "Darwin" or uname.startswith("CYGWIN") :
                system = uname
            if may_use_afs :
                f = os.popen("fs sysname")
                a = f.read()
                if f.close() is None :
                   a = a.replace(" ","")
                   a = a.replace(":","")
                   a = a.replace("'","")
                   a = a.replace("Currentsysnameis","")
                   a = a.replace("Currentsysnamelistis","")
                   system = a.split()[0]
        else :
            if ev.has_key("CMTCONFIG") :
                system = ev["CMTCONFIG"]
            else :
                system = ev["CMTBIN"]

        return system

    def setCMTInternals(self):
        ev = self._env
        opts = self.options
        
        self.setCMTBin()

        if opts.targetshell == "csh" and ev.has_key("SHELL") :
            if os.path.basename(ev["SHELL"]) == "tcsh" :
                for l in open(os.path.join(ev["CMTROOT"], "src", "setup.tcsh"),"r") :
                    self._extra += l
                    
        if opts.targetshell == "sh" and ev.has_key("ZSH_NAME") :
            if ev["ZSH_NAME"] == "zsh" :
                for l in open(os.path.join(ev["CMTROOT"], "src", "setup.zsh"),"r") :
                    self._extra += l
                
        newpath = []    
        for p in ev["PATH"].split(os.pathsep) :
            if p.find(os.sep + "CMT" + os.sep) == -1 :
               newpath.append(p)
        newpath.append(os.path.join(ev["CMTROOT"], ev["CMTBIN"]))
        ev["PATH"] = os.pathsep.join(newpath)

        self._aliases["cmt"] = os.path.join(ev["CMTROOT"], ev["CMTBIN"], "cmt.exe")

        if ev.has_key("CLASSPATH") :
            newpath = []
            for p in ev["CLASSPATH"].split(os.pathsep) :
                if p.find(os.sep + "CMT" + os.sep) == -1 :
                   newpath.append(p)
            newpath.append(os.path.join(ev["CMTROOT"], "java"))
            ev["CLASSPATH"] = os.pathsep.join(newpath)
        else :
            ev["CLASSPATH"] = os.path.join(ev["CMTROOT"], "java")

        self._aliases["jcmt"] = "(java cmt_parser)"

        ev["CMTCONFIG"] = self.setCMTSystem()

    def setCMT(self):
        opts = self.options
        ev = self._env
        if opts.cmtsite == "CERN" :
            ev["CONTRIBDIR"] = os.path.join(ev["SITEROOT"], "sw", "contrib")
        else :
            ev["CONTRIBDIR"] = os.path.join(ev["SITEROOT"], "contrib")

        if sys.platform == "darwin" :
            opts.cmtvers = "v1r20p20070524"
            
        ev["CMT_DIR"] = ev["CONTRIBDIR"]
        ev["CMTROOT"] = os.path.join(ev["CMT_DIR"], "CMT", opts.cmtvers)
        
        self.setCMTInternals()
#-----------------------------------------------------------------------------------
    def setSoftLocations(self):
        ev = self._env
        opts = self.options
        if opts.cmtsite == "LOCAL" :
            ev["LHCBHOME"] = ev["MYSITEROOT"]
            ev["DIM_release_area"] = ev["CONTRIBDIR"]
            ev["XMLRPC_release_area"] = ev["CONTRIBDIR"]
            ev["LCG_release_area"] = os.path.join(ev["MYSITEROOT"], "lcg" ,"external")
            ev["LHCBRELEASES"] = os.path.join(ev["LHCBHOME"], "lhcb")
            ev["GAUDISOFT"] = ev["LHCBRELEASES"]
            ev["LHCBPROJECTPATH"] = os.pathsep.join([ev["LHCBRELEASES"],ev["LCG_release_area"]])
        else :
            ev["LHCBHOME"] = os.path.join(ev["SITEROOT"], "lhcb")
            ev["DIM_release_area"] = os.path.join(ev["LHCBHOME"], "online", "control")
            ev["XMLRPC_release_area"] = os.path.join(ev["LHCBHOME"], "project", "web", "online" )
            ev["LCG_release_area"] = os.path.join(ev["SITEROOT"], "sw", "lcg", "app", "releases" )
            ev["SOFTWARE"] = os.path.join(ev["LHCBHOME"], "software" )
            ev["LHCBRELEASES"] = os.path.join(ev["SOFTWARE"], "releases")
            ev["GAUDISOFT"] = os.path.join(ev["SITEROOT"], "sw", "Gaudi", "releases")
            ev["LHCBPROJECTPATH"] = os.pathsep.join([ev["LHCBRELEASES"], ev["GAUDISOFT"], ev["LCG_release_area"]])
            ev["LHCBDEV"] = os.path.join(ev["SITEROOT"], "lhcb", "software", "DEV" )
            ev["LHCBDOC"] = os.path.join(ev["LHCBRELEASES"], "DOC")
            ev["EMACSDIR"] = os.path.join(ev["LHCBRELEASES"], "TOOLS", "Tools", "Emacs", "pro")
            ev["LHCBSTYLE"] = os.path.join(ev["LHCBRELEASES"], "TOOLS", "Tools", "Styles", "pro")

        ev["OSC_release_area"] = ev["CONTRIBDIR"]
        ev["Gaudi_release_area"] = ev["GAUDISOFT"]
        ev["LHCb_release_area"] = ev["LHCBRELEASES"]

#-----------------------------------------------------------------------------------
                      
    def setHomeDir(self):
        ev = self._env
        opts = self.options
        if sys.platform == "win32" and not ev.has_key("HOME") :
            ev["HOME"] = os.path.join(ev["HOMEDRIVE"], ev["HOMEPATH"])
        homedir = ev["HOME"]
        rhostfile = os.path.join(homedir,".rhosts")
        if sys.platform != "win32" :
            username = ev["USER"]
        else :
            username = ev["USERNAME"]
        if not os.path.exists(rhostfile) and sys.platform != "win32" :
            self._add_echo( "Creating a %s file to use CMT" % rhostfile ) 
            self._add_echo("Joel.Closier@cern.ch")
            f = open(rhostfile, "w")
            f.write("+ %s\n" % username )
            f.close()
        # remove any .cmtrc file stored in the $HOME directory
        cmtrcfile = os.path.join(homedir, ".cmtrc")
        if os.path.exists(cmtrcfile) :
            os.remove(cmtrcfile)
        # to work with rootd the .rootauthrc file is required
        rootrcfile = os.path.join(homedir, ".rootauthrc")
        if not os.path.exists(rootrcfile) :
            if opts.cmtsite == "CERN" :
                srcrootrcfile = os.path.join(ev["AFSROOT"], "cern.ch", "lhcb", "scripts", ".rootauthrc")
            elif opts.cmtsite == "LOCAL" :
                srcrootrcfile = os.path.join(ev["MYSITEROOT"].split(os.pathsep)[0], "cern.ch", "lhcb", "scripts", ".rootauthrc")
            if os.path.exists(srcrootrcfile) :                
                shutil.copy(srcrootrcfile, rootrcfile)

        if not ev.has_key("LD_LIBRARY_PATH") :
            ev["LD_LIBRARY_PATH"] = ""

        if not ev.has_key("ROOTSYS") :
            ev["ROOTSYS"] = ""

        self.setUserArea()  

    def setUserArea(self):
        log = logging.getLogger()
        opts = self.options
        ev = self._env
        al = self._aliases
        newdir = False
        if not opts.userarea :
            opts.userarea = os.path.join(ev["HOME"], "cmtuser") # @todo: use something different for window
        ev["User_release_area"] = opts.userarea

        if os.path.exists(opts.userarea) :
            if not os.path.isdir(opts.userarea) :
                os.rename(opts.userarea, opts.userarea + "_bak")
                log.warning("Renamed file %s into %s" % (opts.userarea, opts.userarea + "_bak"))
                os.mkdir(opts.userarea)
                newdir = True
                self._add_echo(" --- a new cmtuser directory has been created in your HOME directory")
        else :
            os.mkdir(opts.userarea)
            newdir = True

        if opts.cmtsite == "CERN" :
            if newdir :
                os.system("fs setacl %s system:anyuser rl" % opts.userarea )
                self._add_echo( " --- with public access (readonly)" )
                self._add_echo( " --- use mkprivate to remove public access to the current directory" )
                self._add_echo( " --- use mkpublic to give public access to the current directory" )
            al["mkprivate"] = "find . -type d -print -exec fs setacl {} system:anyuser l \\;"
            al["mkpublic"] = "find . -type d -print -exec fs setacl {} system:anyuser rl \\;"

        dirm = os.path.join(opts.userarea, "cmt")
        if os.path.exists(dirm) :
            if os.path.isdir(dirm):
                os.rmdir(dirm)
            else:
                os.remove(dirm)
            
    def setSharedArea(self):
        log = logging.getLogger()
        opts = self.options
        ev = self._env
        if opts.sharedarea :
            if opts.cmtsite == "LOCAL" :
                opts.mysiteroot = os.pathsep.join(opts.sharedarea.split(os.pathsep))
            else :
                log.error("Cannot have a shared area if your CMTSITE is not local")
            
    def setCMTConfig(self, debug=False):
        ev = self._env
        opts = self.options
        gcclist = []
        islinux = False
        if sys.platform.find("linux") != -1 :
            islinux = True
        if ev.has_key("OSTYPE") :
            if ev["OSTYPE"] == "linux" or ev["OSTYPE"] == "linux-gnu" :
                islinux = True
        if islinux :
            for l in os.popen("gcc --version") :
                if l.find("gcc") != -1 :
                    gcclist = l[:-1].split()[2]
                    gcclist = gcclist.split(".")
                    gccvers = int("".join(gcclist[:2]))
                    if gccvers >= 34 :
                        self.compdef = "gcc%s" % "".join(gcclist[:2])
                    else :
                        self.compdef = "gcc%s" % "".join(gcclist[:3])
                    break
            
            hwdict = {"ia32" : ["i386"], "amd64" : ["x86_64"] }
            nathw = os.popen("uname -i").read()[:-1]
            for h in hwdict :
                for l in hwdict[h] :
                    if l == nathw :
                        self.binary = h
                        break
            relfiles = [ "/etc/redhat-release" , "/etc/system-release" ]
            nbre = re.compile("[0-9]")
            for r in relfiles :
                if os.path.exists(r) :
                    firstl = open(r, "r").read()[:-1]
                    distrib = firstl.split()[0]
                    rhw = nbre.search(firstl).group()
                    if distrib == "Scientific" :
                        self.platform = "slc%s" % rhw
                    elif distrib == "Fedora" :
                        self.platform = "fc%s" % rhw
                    else:
                        self.platform = "rh%s" % rhw
                    break
        elif sys.platform.find("darwin") != -1 :
            for l in os.popen("gcc --version") :
                if l.find("gcc") != -1 :
                    gcclist = l[:-1].split()[2]
                    gcclist = gcclist.split(".")
                    gccvers = int("".join(gcclist))
                    self.compdef = "gcc%s" % "".join(gcclist)
                    break
                
            nathw = os.popen("uname -p").read()[:-1]
            if nathw == "powerpc" :
                self.binary = "ppc"
            else :
                self.binary = "ia32"
            
            for l in os.popen("sw_vers") :
                if l.find("ProductVersion") != -1 :
                    platlist = l[:-1].split()[1]
                    platlist = platlist.split(".")
                    self.platform = "osx%s" % "".join(platlist[:2])


        if opts.cmtconfig :
            conflist = opts.cmtconfig.split("_")
            if len(conflist) > 2 :
                self.platform = conflist[0]
                self.binary   = conflist[1]
                self.compdef  = conflist[2]
            
            for c in conflist :
                if c.startswith("gcc32") or c == "slc3" or c == "sl3":
                    self.platform = "slc3"
                    self.binary   = "ia32"
                    self.compdef  = "gcc323"
                    break
                if c == "sl4" :
                    self.platform = "slc4"

        if self.compdef == "gcc323" :
            if "".join(gcclist[:3]) != "323" :
                if opts.cmtsite == "CERN":
                    compiler_path = "/afs/cern.ch/lhcb/externallib/SLC3COMPAT/slc3_ia32_gcc323"
                    if not os.path.isdir(compiler_path) or not os.path.isfile("/usr/bin/gcc32") :
                        self._add_echo( "%s compiler is not available on this node" % self.compdef )
                    else :
                        if ev.has_key("PATH") :
                            pthlist = ev["PATH"].split(os.pathsep)
                        else :
                            pthlist = []
                        pthlist.append(compiler_path)
                        ev["PATH"] = os.pathsep.join(pthlist)
                        if ev.has_key("LD_LIBRARY_PATH") :
                            lpthlist = ev["LD_LIBRARY_PATH"].split(os.path.sep)
                        else :
                            lpthlist = []
                        lpthlist.append(compiler_path)
                        ev["LD_LIBRARY_PATH"] = os.pathsep.join(lpthlist)


        ev["CMTOPT"] = "_".join([self.platform, self.binary, self.compdef])
        ev["CMTDEB"] = ev["CMTOPT"] + "_dbg"
        
        ev["CMTCONFIG"] = ev["CMTOPT"]
        if debug :
            ev["CMTCONFIG"] = ev["CMTDEB"]
            
    def setCMTPath(self):
        ev = self._env
        opts = self.options
        
        self.setHomeDir()
        if ev.has_key("CMTPROJECTPATH") :
            if ev.has_key("CMTPATH") :
                del ev["CMTPATH"]
        else :    
            if opts.cmtvers.find("v1r20") == -1 :
                ev["CMTPATH"] = ev["User_release_area"]
                if ev.has_key("CMTPROJECTPATH") :
                    del ev["CMTPROJECTPATH"]
            else :
                if ev.has_key("CMTPATH") :
                    del ev["CMTPATH"]
                ev["CMTPROJECTPATH"] = os.pathsep.join([ev["User_release_area"], ev["LHCBPROJECTPATH"]])

    
    def setupLbScripts(self):
        ev = self._env
        al = self._aliases
        opts = self.options
        if ev.has_key("PYTHONPATH"):
            pylist = ev["PYTHONPATH"].split(os.pathsep)
        else : 
            pylist = []
        pylist.append(os.path.join(_base_dir, "python"))
        ev["PYTHONPATH"] = os.pathsep.join(pylist)

        if ev.has_key("PATH"):
            plist = ev["PATH"].split(os.pathsep)
        else : 
            plist = []
        plist.append(os.path.join(_base_dir, "scripts"))
        ev["PATH"] = os.pathsep.join(plist)

        from AllProjectsSetup import AllProjectsSetupScript    
        AProj = AllProjectsSetupScript()
        sev, sal = AProj.getEnv()
        
        for e in sev.keys():
            if sev[e] :
                ev[e] = sev[e]
        for a in sal.keys():
            if sal[a] :
                ev[a] = sal[a]
    
    def setEnv(self, debug=False):
        self.setPath()
        self.setCVSEnv()
        self.setSite()
        self.setCMT()
        self.setSoftLocations()
        self.setSharedArea()

        self.setCMTConfig(debug)
        self.setCMTPath()
        self.setupLbScripts()

        return self._env, self._aliases, self._extra

    def Manifest(self, debug=False):
        ev = self._env
        opts = self.options
        self._add_echo( "*" * 80 )
        toprint = "*" + " " * 27 + "---- LHCb Login ----"
        self._add_echo(toprint + " " * (80-len(toprint)-1) + "*")
        toprint = "*" + " " * 11 + "Building with %s on %s_%s system" % (self.compdef, self.platform, self.binary)
        self._add_echo(toprint + " " * (80-len(toprint)-1) + "*")
        toprint = "*" + " " * 11 + "DEVELOPMENT SCRIPT"
        self._add_echo( toprint + " " * (80-len(toprint)-1) + "*" )
        self._add_echo( "*" * 80 )
        self._add_echo( " --- CMTROOT is set to %s " % ev["CMTROOT"] )
        self._add_echo( " --- CMTCONFIG is set to %s " % ev["CMTCONFIG"] )
        if debug :
            self._add_echo( " --- to compile and link in debug mode : setenv CMTCONFIG $CMTDEB ; gmake" )
        if ev.has_key("CMTPATH") :
            self._add_echo( " --- CMTPATH is set to %s" % ev["CMTPATH"]) 
        else :
            self._add_echo( " --- User_release_area is set to %s" % ev["User_release_area"])
            self._add_echo( " --- CMTPROJECTPATH is set to $User_release_area:$LHCb_release_area:$Gaudi_release_area:$LCG_release_area")
            self._add_echo( " --- projects will be searched in $CMTPROJECTPATH ")
        self._add_echo( "-" * 80)

    def main(self):
        opts = self.options
        debug = False
        for a in self.args :
            if a == "debug" :
                debug = True

        self.setEnv(debug)
        self.Manifest(debug)

        self._write_script(self._env.gen_script(opts.targetshell)
                           +self._aliases.gen_script(opts.targetshell)+self._extra)
        
        self.Manifest(debug)
        return 0


if __name__ == '__main__':
    sys.exit(LbLoginScript(usage="%prog [options]").run())


