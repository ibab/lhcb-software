import unittest

import SetupProject
#from SetupProject import *

import os, sys, re
from StringIO import StringIO

# utility check functions
def search_regexp(output,regexp):
    #print output
    return re.compile(regexp).search(output) is not None
        
def search_string(output,s):
    #print output
    return output.find(s) >= 0

def proj_not_found(output, project="DummyProject"):
    return output.startswith("Cannot find project '%s'" % project)

def project_configured(output, project, version = None):
    #print output
    tmps = project
    if version: tmps += " %s" % version
    #print ("Environment for %s ready." % tmps), output
    return ("Environment for %s ready." % tmps) in output

def get_all_versions(project):
    versions = SetupProject.FindProjectVersions(project, SetupProject._defaultSearchPath())
    return SetupProject.SortVersions([ v[1] for v in versions ])

if os.environ["CMTCONFIG"][0:3] == "win":
    _shell = "bat"
else:
    _shell = "sh"

class SetupProjectTestCase(unittest.TestCase):
    def _check_env(self, output, project, version, withsys=True, main=True):
        self.assert_(re.compile('PATH=.*%s'%project.upper()).search(output) is not None)
        if withsys:
            self.assert_(re.compile('%sSYSROOT=.*%s'%(project.upper(),version)).search(output) is not None)
        if main:
            self.assert_(project_configured(output,project,version))

    def test_010_mkdtemp(self):
        """mkdtemp
        """
        self.assert_('mkdtemp' in dir(SetupProject))
        tmp_dir = SetupProject.mkdtemp()
        self.assert_(os.path.exists(tmp_dir))
        self.assert_(os.path.isdir(tmp_dir))
        self.assert_(not os.path.islink(tmp_dir))
        os.rmdir(tmp_dir)
    
    def test_020_mkstemp(self):
        """mkstemp
        """
        self.assert_('mkdtemp' in dir(SetupProject))
        fd, name = SetupProject.mkstemp()
        self.assert_(os.path.exists(name))
        self.assert_(os.path.isfile(name))
        self.assert_(not os.path.islink(name))
        os.fdopen(fd,"w")
        os.remove(name)

    def test_030_removeall(self):
        """removeall
        """
        tmp_dir = SetupProject.mkdtemp()
        os.mkdir(os.path.join(tmp_dir,"test1"),0700)
        open(os.path.join(tmp_dir,"test2"),"w")
        open(os.path.join(tmp_dir,"test1","test2"),"w")
        
        SetupProject.removeall(tmp_dir)
        
        self.assert_(not os.path.exists(tmp_dir))

    def test_040_gen_script(self):
        """gen_script
        """
        env = { "A":"1", "B":"2", "C":None }
        # clean up the environment
        envguard = SetupProject.TemporaryEnvironment()
        for var in env:
            if var in envguard:
                del envguard[var]
        envguard["C"] = "test"

        expected = {}
        expected['csh'] = 'setenv A "1"\nsetenv B "2"\nunsetenv C\n'.split('\n')
        expected['csh'].sort()
        
        expected['sh'] = 'export A="1"\nexport B="2"\nunset C\n'.split('\n')
        expected['sh'].sort()

        expected['bat'] = 'set A=1\nset B=2\nset C=\n'.split('\n')
        expected['bat'].sort()

        shells = [ 'csh', 'sh', 'bat' ]
        for sh in shells:
            newenv = SetupProject.TemporaryEnvironment()
            for var in env:
                if env[var]:
                    newenv[var] = env[var]
                else:
                    del newenv[var]
            res = newenv.gen_script(sh).split('\n')
            res.sort()
            self.assertEquals(expected[sh],res)
            del newenv
        
        self.assertRaises(RuntimeError,
                          SetupProject.TemporaryEnvironment().gen_script,
                              "abc")

    def test_050_options(self):
        """option parsing
        """
        parse_opts = lambda opts: os.popen4('python -c "import SetupProject; SetupProject.SetupProject().parser.parse_args(args=%s)"'%opts)[1].read()
        parse_opts_stdout = lambda opts: os.popen3('python -c "import SetupProject; SetupProject.SetupProject().parser.parse_args(args=%s)"'%opts)[1].read()

        # This is different between Python 2.3 and Python 2.5
        exp23 = 'usage: -c [options] '
        exp25 = 'Usage: -c [options] '
        x = parse_opts(["-h"])
        self.assert_(x.startswith(exp23) or x.startswith(exp25))

        self.assert_(parse_opts_stdout(['-h']) == "")
        self.assert_(parse_opts_stdout(['--version']) == "")
        self.assert_(parse_opts_stdout(['--dummy']) == "")
        
        exp = "error: no such option: --dummy"
        x = parse_opts(["--dummy"]).strip()
        self.assert_(x.endswith(exp))
        
        sp = SetupProject.SetupProject()
        sp.parse_args(["--shell=csh"])
        self.assertEquals('csh',sp.shell)
        
        exp = "error: --mktemp cannot be used at the same time as --output"
        x = parse_opts(["--mktemp","--output","xyz"]).strip()
        self.assert_(x.endswith(exp))
        
        ### All Options
        sp = SetupProject.SetupProject()
        # This is needed because the check of LHCBDEV is done during the parsing of the options
        tmpenv = SetupProject.TemporaryEnvironment()
        if "LHCBDEV" not in tmpenv:
            tmpenv["LHCBDEV"] = "dummy"
        sp.parse_args(['--shell=sh',
                       '--output=/tmp',
                       '--site=none',
                       '--disable-CASTOR',
                       '--tag_add=A1',
                       '--tag_add=A2',
                       '--use=abc v1',
                       '--use=abc v2',
                       '--verbose',
                       '--ignore-missing',
                       '--ignore-context',
                       '--list-versions',
                       '--external-only',
                       '--dev',
                       '--ask',
                       ])
        self.assert_(sp.disable_CASTOR)
        self.assert_(sp.ignore_missing)
        self.assert_(sp.ignore_context)
        self.assert_(sp.list_versions)
        self.assert_(sp.external_only)
        self.assert_(sp.dev)
        self.assert_(sp.ask)
        self.assertEquals(sp.loglevel,2)
        self.assertEquals(sp.output,"/tmp")
        self.assertEquals(sp.site_externals,[])
        self.assertEquals(sp.use,["abc v1","abc v2"])
        self.assertEquals(sp.tag_add,["A1","A2"])
        
        sp = SetupProject.SetupProject()
        sp.parse_args([])
        self.assert_(not sp.disable_CASTOR)
        self.assert_(not sp.ignore_missing)
        self.assert_(not sp.ignore_context)
        self.assert_(not sp.list_versions)
        self.assert_(not sp.external_only)
        self.assert_(not sp.dev)
        self.assert_(not sp.ask)
        self.assertEquals(sp.loglevel,3)
        self.assert_(not sp.output)
        #self.assertEquals(sp.site_externals,[])
        self.assertEquals(sp.use,[])
        self.assertEquals(sp.tag_add,[])
        
        sp = SetupProject.SetupProject()
        sp.parse_args(['--debug',"--mktemp","--site=CERN"])
        self.assertEquals(sp.loglevel,1)
        self.assert_(sp.mktemp)
        self.assertEquals(sp.site_externals,["CASTOR"])
        
        ### Ordering
        sp = SetupProject.SetupProject()
        sp.parse_args(['Project',
                       '--shell=sh',
                       '--verbose',
                       'v123r456',
                       'ROOT','-v','1.2.3.4',
                       ])
        
        ### External version
        sp = SetupProject.SetupProject()
        sp.parse_args(['Project','ROOT','-v','1.2.3.4'])
        self.assertEquals(sp.ext_versions["ROOT"],'1.2.3.4')

        # --dev-dir implies --dev
        sp = SetupProject.SetupProject()
        sp.parse_args(['--dev-dir=MyDevDir'])
        self.assert_(sp.dev)
        
        # runtime dependencies on projects
        sp = SetupProject.SetupProject()
        sp.parse_args(['--runtime-project', 'Test', 'v1r1',
                       '--runtime-project', 'Other', 'v7r6p5',
                       '--runtime-project', 'NoVers', 'argument'
                       ])
        self.assertEquals(sp.runtime_projects,[ ("Test","v1r1"),
                                                ('Other', 'v7r6p5'),
                                                ('NoVers', None) ])

    def test_060_main(self):
        """main (wrong options)
        """
        
        x = os.popen4("python SetupProject.py")
        self.assert_(x[1].read().startswith("You have to specify a project"))
        
        x = os.popen4("python SetupProject.py Brunel")
        self.assert_(x[1].read().startswith("Internal error: shell type not specified"))
        
        x = os.popen4("python SetupProject.py Brunel --shell=xyz")
        self.assert_(x[1].readlines()[-1].startswith("SetupProject.py: error: option --shell: invalid choice:"))

    def test_065_main(self):
        """main (unknown project)
        """

        # no env
        x = os.popen4("python SetupProject.py --shell=%s DummyProject"%_shell)
        s = x[1].read().strip()
        self.assert_(proj_not_found(s))

        env = SetupProject.TemporaryEnvironment()
        tmp_dir = SetupProject.mkdtemp()
        try:
            if "CMTPROJECTPATH" not in env:
                env['CMTPROJECTPATH'] = env['LHCBPROJECTPATH']
            env['CMTPROJECTPATH'] = tmp_dir + os.pathsep + env['CMTPROJECTPATH']
            os.mkdir(os.path.join(tmp_dir,'DUMMYPROJECT'))
            x = os.popen4("python SetupProject.py --shell=%s DummyProject"%_shell)
            s = x[1].read().strip()
            self.assert_(proj_not_found(s))
        finally:
            SetupProject.removeall(tmp_dir)
            
    def test_067_main(self):
        """main (wrong case in project name)
        """
        v = SetupProject.makeProjectInfo("Brunel").version

        x = os.popen4("python SetupProject.py --shell=%s BrUnEl"%_shell)
        s = x[1].read()
        ## wrong case is valid
        # self.assert_("Could not produce the environment, check the arguments" in s)
        self._check_env(s, "Brunel", v)

    def test_070_main(self):
        """main (Brunel w/o ext)
        """
        
        v = SetupProject.makeProjectInfo("Brunel").version
        
        x = os.popen4("python SetupProject.py --shell=%s Brunel"%_shell)
        s = x[1].read()
        
        self._check_env(s, "Brunel", v)

    def test_075_main(self):
        """main (Brunel w/o ext, w/o User_release_area)
        """

        env = SetupProject.TemporaryEnvironment()
        if 'User_release_area' in env:
            del env['User_release_area']
        
        v = SetupProject.makeProjectInfo("Brunel").version

        x = os.popen4("python SetupProject.py --shell=%s Brunel"%_shell)
        s = x[1].read()
        
        self._check_env(s, "Brunel", v)

    def test_077_main(self):
        """main (Brunel w/o ext, w/o CMTPATH and CMTPROJECTPATH)
        """

        env = SetupProject.TemporaryEnvironment()
        for n in [ 'CMTPATH', 'CMTPROJECTPATH' ]:
            if n in env:
                del env[n]
        
        v = SetupProject.makeProjectInfo("Brunel").version

        x = os.popen4("python SetupProject.py --shell=%s Brunel"%_shell)
        s = x[1].read()
        
        self._check_env(s, "Brunel", v)
        
    def test_080_main(self):
        """main (Brunel with ext)
        """
        
        v = SetupProject.makeProjectInfo("Brunel").version
        
        x = os.popen4("python SetupProject.py --shell=%s Brunel mysql"%_shell)
        s = x[1].read()
        
        self._check_env(s, "Brunel", v)
        
        self.assert_(s.find('MYSQLROOT') >=0)

    def test_085_main(self):
        """main (Brunel with runtime project, Online)
        """
        
        v = SetupProject.makeProjectInfo("Brunel").version
        vo = SetupProject.makeProjectInfo("Online").version
        
        # --ignore-missing is needed to avoid problems with incompatible versions of Online ad Brunel
        x = os.popen4("python SetupProject.py --shell=%s Brunel --ignore-missing --runtime-project Online"%_shell)
        s = x[1].read()
        
        self._check_env(s, "Brunel", v)
        self._check_env(s, "Online", vo, withsys = False, main = False)

    def test_090_main(self):
        """main (Brunel with wrong ext)
        """
        
        x = os.popen4("python SetupProject.py --shell=%s Brunel NoPack"%_shell)
        s = x[1].read()
        self.assert_(s.find("Warning: package NoPack v* LCG_Interfaces not found")>=0)
        self.assert_(s.endswith("Could not produce the environment, check the arguments\n"))
        

    def test_100_main(self):
        """main (Brunel with wrong ext ignored)
        """
        
        v = SetupProject.makeProjectInfo("Brunel").version
        
        x = os.popen4("python SetupProject.py --shell=%s --ignore-missing Brunel NoPack"%_shell)
        s = x[1].read()
        self.assert_(s.find("Warning: package NoPack v* LCG_Interfaces not found")>=0)
        self.assert_(project_configured(s,"Brunel",v))
        
    def test_110_main(self):
        """main (Brunel list versions)
        """
        
        x = os.popen4("python SetupProject.py --shell=%s --list-versions Brunel"%_shell)
        l = x[1].read().split('\n')
        l = [ x for x in l if x != '' ]
        self.assert_( len(l) > 0 )
        for v in l:
            self.assert_(re.match('echo v([0-9]+)r([0-9]+)(p([0-9]+))?',v) is not None)
        
    def test_120_main(self):
        """main (Brunel explicit version)
        """
        l = get_all_versions("Brunel")
        if len(l) > 1:
            v = l[-2]
        else:
            v = l[0]
        x = os.popen4("python SetupProject.py --shell=%s Brunel %s"%(_shell,v))
        s = x[1].read()
        
        self._check_env(s,"Brunel",v)
        
    def test_125_main(self):
        """main (Brunel with runtime project Online, explicit versions)
        """
        
        l = get_all_versions("Brunel")
        if len(l) > 1:
            v = l[-2]
        else:
            v = l[0]
        
        l = get_all_versions("Online")
        if len(l) > 1:
            vo = l[-2]
        else:
            vo = l[0]
            
        # --ignore-missing is needed to avoid problems with incompatible versions of Online ad Brunel
        x = os.popen4("python SetupProject.py --shell=%s --ignore-missing Brunel %s --runtime-project Online %s"%(_shell,v,vo))
        s = x[1].read()
        
        self._check_env(s,"Brunel",v)
        self._check_env(s, "Online", vo, withsys = False, main = False)
        
    def test_130_main(self):
        """main (Brunel explicit bad version)
        """

        x = os.popen4("python SetupProject.py --shell=%s Brunel v999r999p999"%_shell)
        s = x[1].read()
        self.assert_(s.endswith("Try with --list-versions.\n"))
        
    def test_135_main(self):
        """main (Brunel + runtime project Online, explicit bad version)
        """

        x = os.popen4("python SetupProject.py --shell=%s Brunel --runtime-project Online v999r999p999"%_shell)
        s = x[1].read()
        self.assert_(s.endswith("Try with --list-versions.\n"))
        
    def test_140_main(self):
        """main (Brunel update $LHCBHOME/project/logfiles)
        """
        if 'LHCBHOME' in os.environ: # ignore the test on win32
            x = os.popen4("python SetupProject.py --shell=%s Brunel"%_shell)
            s = x[1].read()
            self.assert_(s.find('touch %s/BRUNEL'%os.path.join(os.environ['LHCBHOME'],'project','logfiles'))>=0)
        
    def test_150_main(self):
        """main (Brunel ask version bad)
        """

        l = get_all_versions("Brunel")
        
        x = os.popen3("python SetupProject.py --shell=%s Brunel --ask"%_shell)

        self.assertEquals("Please enter",x[2].read(12))

        # read up to the prompt
        while x[2].read(1) != ':':
            pass
        x[2].read(1)

        # bad answer
        x[0].write("v999r999p999\n")
        x[0].flush()
        
        s = "Version 'v999r999p999' not valid!\nPlease enter"
        self.assertEquals(s,x[2].read(len(s)))
        
    def test_160_main(self):
        """main (Brunel ask version quit)
        """

        l = get_all_versions("Brunel")
        
        x = os.popen3("python SetupProject.py --shell=%s Brunel --ask"%_shell)

        self.assertEquals("Please enter",x[2].read(12))

        # read up to the prompt
        while x[2].read(1) != ':':
            pass
        x[2].read(1)

        # quit
        x[0].write("q\n")
        x[0].flush()
        self.assertEquals('Quit\n',x[2].read())
        
    def test_170_main(self):
        """main (Brunel ask default)
        """

        l = get_all_versions("Brunel")
        
        x = os.popen3("python SetupProject.py --shell=%s Brunel --ask"%_shell)

        self.assertEquals("Please enter",x[2].read(12))

        # read up to the prompt
        while x[2].read(1) != ':':
            pass
        x[2].read(1)

        # quit
        x[0].write("\n")
        x[0].flush()
        
        self.assert_(project_configured(x[1].read(),"Brunel",l[-1]))

    def test_180_main(self):
        """main (Brunel ask arbitrary version)
        """

        l = get_all_versions("Brunel")
        
        if len(l) > 1:
            v = l[-2]
        else:
            v = l[0]
        
        x = os.popen3("python SetupProject.py --shell=%s Brunel --ask"%_shell)

        self.assertEquals("Please enter",x[2].read(12))

        # read up to the prompt
        while x[2].read(1) != ':':
            pass
        x[2].read(1) # eat the space char

        # quit
        x[0].write(v+"\n")
        x[0].flush()
        
        self.assert_(project_configured(x[1].read(),"Brunel",v))

    # disabled because of change in behavior of SetupProject.py
    def _test_190_main(self):
        """main (Brunel release area not in CMTPROJECTPATH)
        """
        env = SetupProject.TemporaryEnvironment()
        # override default PROJECTPATH
        env['LHCBPROJECTPATH'] = os.pathsep.join([env['Gaudi_release_area'],env['LCG_release_area']])
        if 'CMTPROJECTPATH' in env:
            del env['CMTPROJECTPATH']
        
        l = get_all_versions("Brunel")
        if len(l) > 1:
            v = l[-2]
        else:
            v = l[0]
        x = os.popen4("python SetupProject.py --shell=%s Brunel %s"%(_shell,v))
        s = x[1].read()
        
        self._check_env(s,"Brunel",v)
        
    def test_200_main(self):
        """main (Brunel in user release area)
        """
        l = get_all_versions("Brunel")
        v = l[-1]

        env = SetupProject.TemporaryEnvironment()
        tmp_dir = SetupProject.mkdtemp()
        try:
            # prepare fake user release area
            os.mkdir(os.path.join(tmp_dir,'Brunel_%s'%v))
            os.mkdir(os.path.join(tmp_dir,'Brunel_%s'%v,"cmt"))
            open(os.path.join(tmp_dir,'Brunel_%s'%v,"cmt","project.cmt"),"w").\
                write("project Brunel_%s\n\nuse BRUNEL BRUNEL_%s\n"%(v,v))
            
            env['User_release_area'] = tmp_dir
            x = os.popen4("python SetupProject.py --shell=%s --no-auto-override Brunel %s"%(_shell,v))
            s = x[1].read().strip()
            
            self._check_env(s,"Brunel",v)
            
            if os.environ["CMTCONFIG"][0:3] != "win":
                m = re.compile('LIBRARY_PATH=(.*)').search(s)
            else:
                m = re.compile('set PATH=(.*)').search(s)
            self.assert_(m is not None)
            self.assert_(m.group(1).split(os.pathsep)[0].find(
                            os.path.join(tmp_dir,'Brunel_%s'%v,"InstallArea"))>=0)
            #self.assert_(proj_not_found(s))
        finally:
            SetupProject.removeall(tmp_dir)
    
    def test_210_main(self):
        """main (Brunel in LHCBDEV)
        """
        l = get_all_versions("Brunel")
        v = l[-1]

        env = SetupProject.TemporaryEnvironment()
        tmp_dir = SetupProject.mkdtemp()
        try:
            # prepare fake dev area
            dsys = os.path.join(tmp_dir,'BRUNEL','BRUNEL_v999r999','BrunelSys','v999r999','cmt')
            d = tmp_dir
            for x in ['BRUNEL','BRUNEL_v999r999','BrunelSys','v999r999','cmt']:
                d = os.path.join(d,x)
                os.mkdir(d)
            open(os.path.join(d,"requirements"),"w").write("""
package BrunelSys
version v999r999
""")
            os.mkdir(os.path.join(tmp_dir,'BRUNEL','BRUNEL_v999r999','cmt'))
            open(os.path.join(tmp_dir,'BRUNEL','BRUNEL_v999r999','cmt','project.cmt'),"w").write("project Brunel\n")
            
            env['LHCBDEV'] = tmp_dir
            # get it from LHCBDEV
            x = os.popen4("python SetupProject.py --shell=%s --ignore-missing --dev Brunel"%_shell)
            s = x[1].read()
            self.assert_(project_configured(s,"Brunel","v999r999"))
            
            # get it from offical releases
            x = os.popen4("python SetupProject.py --shell=%s Brunel"%_shell)
            s = x[1].read()
            self.assert_(project_configured(s,"Brunel",v))
            
        finally:
            SetupProject.removeall(tmp_dir)
    
    def test_300_main(self):
        """main (Build-time environment expl. version)
        """
        l = get_all_versions("Brunel")
        v = l[-1]

        env = SetupProject.TemporaryEnvironment()
        tmp_dir = SetupProject.mkdtemp()
        env["User_release_area"] = tmp_dir
        try:
            x = os.popen3("python SetupProject.py --shell=%s --ignore-missing --build-env Brunel %s"%(_shell,v))
            s = x[1].read()
            proj_cmt = os.path.join(tmp_dir,"Brunel_%s" % v, "cmt", "project.cmt")
            self.assert_(os.path.isfile(proj_cmt))
            self.assert_(re.search(r"use\s+BRUNEL\s+BRUNEL_%s" % v,open(proj_cmt).read()) is not None)
        finally:
            SetupProject.removeall(tmp_dir)
            
    def test_310_main(self):
        """main (Build-time environment, no version/ask)
        """
        l = get_all_versions("Brunel")
        v = l[-1]

        env = SetupProject.TemporaryEnvironment()
        tmp_dir = SetupProject.mkdtemp()
        env["User_release_area"] = tmp_dir
        try:
            x = os.popen3("python SetupProject.py --shell=%s --ignore-missing --build-env Brunel"%_shell)
            self.assertEquals("Please enter",x[2].read(12))
        
            # read up to the prompt
            while x[2].read(1) != ':':
                pass
            x[2].read(1)
            
            x[0].write("\n")
            x[0].flush()
        
            s = x[1].read()
            proj_cmt = os.path.join(tmp_dir,"Brunel_%s" % v, "cmt", "project.cmt")
            self.assert_(os.path.isfile(proj_cmt))
            self.assert_(re.search(r"use\s+BRUNEL\s+BRUNEL_%s" % v,open(proj_cmt).read()) is not None)
        finally:
            SetupProject.removeall(tmp_dir)
            
    def test_320_main(self):
        """main (Build-time environment, bad version)
        """
        
        env = SetupProject.TemporaryEnvironment()
        tmp_dir = SetupProject.mkdtemp()
        env["User_release_area"] = tmp_dir
        try:
            x = os.popen4("python SetupProject.py --shell=%s --ignore-missing --build-env Brunel v999r999"%_shell)
            s = x[1].read()
            proj_cmt = os.path.join(tmp_dir,"Brunel_v999r999", "cmt", "project.cmt")
            self.assert_(not os.path.isfile(proj_cmt))
        finally:
            SetupProject.removeall(tmp_dir)

    def _createFakeProject(self, name = "TestProject", version = None, tmp_dir = None):
        if not tmp_dir:
            tmp_dir = SetupProject.mkdtemp()
        old = os.getcwd()
        os.chdir(tmp_dir)
        if version is None:
            x = os.popen4("cmt create_project %s" % name)
            x[1].read()
            os.chdir(os.path.join(tmp_dir,name))
            version = "v1r0" # just a default value
        else:
            x = os.popen4("cmt create_project %s %s_%s" % (name.upper(), name.upper(), version))
            x[1].read()
            os.chdir(os.path.join(tmp_dir,name.upper(),"%s_%s"%(name.upper(), version)))
        x = os.popen4("cmt create %sSys %s" % (name, version))
        x[1].read()
        os.chdir(old)
        return tmp_dir
        
    def test_400_main(self):
        """main (no-version project, no explicit version)
        """
        
        env = SetupProject.TemporaryEnvironment()
        tmp_dir = self._createFakeProject()
        self._createFakeProject(version = "v1r0", tmp_dir = tmp_dir)
        env["CMTPROJECTPATH"] = env["User_release_area"] = tmp_dir
        try:
            x = os.popen4("python SetupProject.py --shell=%s --no-auto-override --disable-CASTOR TestProject"%_shell)
            s = x[1].read()
            self.assert_(project_configured(s, "TestProject", None))
        finally:
            SetupProject.removeall(tmp_dir)
        
    def test_410_main(self):
        """main (no-version project, ask version)
        """
        
        env = SetupProject.TemporaryEnvironment()
        tmp_dir = self._createFakeProject()
        self._createFakeProject(version = "v1r0", tmp_dir = tmp_dir)
        env["CMTPROJECTPATH"] = env["User_release_area"] = tmp_dir
        
        try:
            x = os.popen3("python SetupProject.py --shell=%s --no-auto-override --disable-CASTOR TestProject --ask"%_shell)
    
            self.assertEquals("Please enter",x[2].read(12))
    
            # read up to the prompt
            while x[2].read(1) != ':':
                pass
            x[2].read(1) # eat the space char
    
            # quit
            x[0].write("None\n")
            x[0].flush()

            s = x[1].read()
            self.assert_(project_configured(s, "TestProject", None))
        finally:
            SetupProject.removeall(tmp_dir)


if __name__ == '__main__':
    unittest.main()
