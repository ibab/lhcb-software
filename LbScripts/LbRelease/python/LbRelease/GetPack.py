#!/usr/bin/env python
# $Id$

import os, sys, re
from subprocess import Popen, PIPE

from LbUtils.Script import Script
from LbUtils.VCS import SVNReposInfo, CVSReposInfo
from LbConfiguration import (initProject,
                             createDataPackageCMakeLists,
                             eclipseConfigurationAddPackage,
                             createEclipseConfiguration)
from LbConfiguration.Repository import repositories as __repositories__
from LbConfiguration.Package import package_names as __data_packages__
import rcs

## Class to select valid version tags according to LHCb policy
class LHCbVersionFilter(object):
    def __init__(self, regexp = r'v([0-9]+)r([0-9]+)(?:p([0-9]+))?(?:-pre([0-9]+))?|(?:\w+_([0-9]{4})([0-9]{2})([0-9]{2})[a-z]?)'):
        self.regexp = re.compile(regexp)
    def __call__(self, version):
        m = self.regexp.match(version)
        if m:
            # groups gives something like ("1","2","3") or ("1","2",None)
            # and we need to convert it to numbers to sort them (None can be
            # kept because it compares nicely with numbers: None < number)
            values = []
            for n in m.groups():
                if n is None:
                    values.append(None)
                else:
                    values.append(int(n))
            if '-pre' not in version:
                # this ensures that "-pre" versions are less important that actual tags
                values[3] = 99999
            return tuple(values)
        return None
rcs.setVersionFilter(LHCbVersionFilter())

def guessDefaultVersion(package):
    """
    Return the most appropriate version of a package if cmt can locate it.

    If called from within a project, "cmt show versions <package>" return a list
    of possible versions of a package. We take the first one, if present.
    """
    os.environ["PWD"] = os.getcwd() # ensure that we pass the right PWD to cmt
    out = Popen(["cmt", "show", "versions", package],
                stdout = PIPE, stderr = PIPE).communicate()[0]
    try:
        # Second element of the first line:
        # "Hat/Package version project_dir" -> version
        version = out.splitlines()[0].split()[1]
    except IndexError: # if there is no output
        version = None
    return version

## Class used as exception to allow a "quit" in any moment
class Quit:
    pass
## Class used as exception to skip a package in any moment
class Skip:
    pass

## Helper function to select one element of a list, simple version (no curses).
def selectFromListPlain(message, lst, bars = False):
    page_size = 30
    count = len(lst)
    page = 0
    pages = count/page_size + 1

    default_ans = None
    idx = -1
    while idx < 0:
        # display current page
        if bars:
            print '-' * 80
        print message
        if bars:
            print '-' * 80
        for ln in range(page*page_size, min((page+1)*page_size, count)):
            print "%3d - %s" % (ln, lst[ln])
        # prepare the question (with default answer)
        question = "Please enter your choice (0-%d, " % (count-1)
        if page < pages - 1:
            question += "(n)ext, "
        if page > 0:
            question += "(p)revious, "
        if not default_ans:
            default_ans = 'q'
            if page < pages - 1:
                default_ans = 'n'
            elif page > 0:
                default_ans = 'p'
        question += "(q)uit) [%s]: " % default_ans

        ans = None
        while ans is None:
            sys.stdout.write(question)
            ans = sys.stdin.readline().strip().lower()
            if ans == "":
                ans = default_ans
            if ans in ['n', 'next']:
                page += 1
                default_ans = ans
                if page >= pages:
                    page = pages - 1
                if page == pages - 1:
                    default_ans = 'p'
            elif ans in ['p', 'prev', 'previous']:
                page -= 1
                default_ans = ans
                if page < 0:
                    page = 0
                if page == 0:
                    default_ans = 'n'
            elif ans in [ 'q', 'quit' ]:
                raise Quit
            else:
                try:
                    ans = int(ans)
                except:
                    ans = -1
                if ans < 0 or ans >= count:
                    print "Invalid selection '%s'" % ans
                    ans = None
                else:
                    idx = ans
    return idx

# It uses curses if available.
# @todo: needs improvements and clean up
try:
    import curses
    ## Class to display a scrollable list with a selected element
    class ListBox(object):
        def __init__(self, data, ypos, xpos, ysize, xsize,
                     initsel = 0):
            self.data = data
            self._count = len(data)
            self.ypos = ypos
            self.xpos = xpos
            self.ysize = ysize
            self.xsize = xsize
            # hack to avoid false error in PyDev
            self.ACS_UARROW = getattr(curses, "ACS_UARROW")
            self.ACS_DARROW = getattr(curses, "ACS_DARROW")
            self.ACS_CKBOARD = getattr(curses, "ACS_CKBOARD")
            self.ACS_BLOCK = getattr(curses, "ACS_BLOCK")
            # The pad has to be higher than the length of the list
            # otherwise we get an error trying to write to the bottom-left char
            self._pad = curses.newpad(self._count + 1, self.xsize)
            self.selection = initsel
            self.offset = 0
            self._draw()

        def _maxwidth(self):
            if self.ysize < (self._count-1):
                return self.xsize - 1
            else:
                return self.xsize

        def _draw(self):
            self._pad.erase()
            maxwidth = self._maxwidth()
            for i in range(self._count):
                self._pad.addnstr(i, 0, self.data[i], maxwidth)
            self._highlight(self.selection)

        def _highlight(self, idx, highlighted = True):
            maxwidth = self._maxwidth()
            if highlighted:
                self._pad.addnstr(idx, 0, self.data[idx], maxwidth, curses.A_REVERSE)
            else:
                self._pad.addnstr(idx, 0, self.data[idx], maxwidth)

        def refresh(self):
            # ensure selection is visible
            if self.selection < self.offset:
                self.offset = self.selection
            elif self.selection > (self.offset + self.ysize):
                self.offset = self.selection - self.ysize

            pad = self._pad
            if self.ysize < (self._count-1):
                # update scroll bar
                top    = self.offset
                bottom = self.offset + self.ysize
                right  = self.xsize - 1
                pad.addch(top, right, self.ACS_UARROW)
                pad.addch(bottom, right, self.ACS_DARROW)
                pad.vline(top + 1, right,
                          self.ACS_CKBOARD, bottom - top - 1)
                cursor = int(round((bottom - top - 2) *
                                   float(self.selection) / (self._count-1)))
                pad.addch(top + cursor + 1, right, self.ACS_BLOCK)

            # paint
            pad.refresh(self.offset, 0,
                        self.ypos, self.xpos,
                        self.ypos + min(self.ysize, self._count),
                        self.xpos + self.xsize)

        def moveByLines(self, offset):
            self._highlight(self.selection, False)
            self.selection += offset
            if self.selection < 0 :
                self.selection = 0
            elif self.selection > (self._count - 1):
                self.selection = self._count - 1
            self._highlight(self.selection)
        def moveByPages(self, poffset):
            # self.moveByLines(poffset * (self.ysize+1))
            self.moveByLines(poffset * (self.ysize))
        def resize(self, ysize, xsize):
            self.ysize = ysize
            if self.ysize > (self.selection - self.offset):
                self.offset = max(0, self.selection - self.ysize)
            if xsize != self.xsize:
                # we need to re-create the pad
                self.xsize = xsize
                self._pad = curses.newpad(self._count + 1, self.xsize)
            self.moveByLines(0)
            self._draw()

    ## Class to display a sort of selection dialog using curses
    class Dialog(object):
        def __init__(self, screen, message, data):
            self.scr = screen
            self.min_y, self.min_x = 0, 0
            self.max_y, self.max_x = self.scr.getmaxyx()

            # hack to avoid false error in PyDev
            self.ACS_HLINE = getattr(curses, "ACS_HLINE")

            self.message = message
            self.listbox = ListBox(data,
                                   self.min_y + 3, self.min_x + 1,
                                   self.max_y - 7, self.max_x - 2)

            try:
                curses.curs_set(0)
            except:
                pass # ignore when the terminal doesn't support hidden cursor
            self._draw()

        def _draw(self):
            self.scr.addnstr(1, 1, self.message, self.max_x - 1)
            self.scr.box()
            self.scr.hline(2, 1, self.ACS_HLINE, self.max_x-2)
            self.scr.hline(self.max_y - 3, 1, self.ACS_HLINE, self.max_x-2)
            self.scr.addnstr(self.max_y - 2, 1,
                             "Use arrows, pgup, pgdown to move, Enter to select, 'q' to quit.",
                             self.max_x - 2)
            self.scr.refresh()
            self.listbox.refresh()

        def resize(self):
            self.min_y, self.min_x = 0, 0
            self.max_y, self.max_x = self.scr.getmaxyx()
            self.scr.erase()
            self.listbox.resize(self.max_y - 7, self.max_x - 2)

        def run(self):
            while True:
                try:
                    key = self.scr.getkey()
                except curses.error:
                    key = None
                if key == "KEY_UP":
                    self.listbox.moveByLines(-1)
                elif key == "KEY_DOWN":
                    self.listbox.moveByLines(1)
                elif key in ["KEY_PPAGE", 'p', 'P']:
                    self.listbox.moveByPages(-1)
                elif key in ["KEY_NPAGE", 'n', 'N']:
                    self.listbox.moveByPages(1)
                elif key == "\n":
                    return self.listbox.selection
                elif key == "KEY_RESIZE":
                    self.resize()
                elif key in ["q", "Q"]:
                    break
                self._draw()
            return None

    ## Display a dialog with the passed message and return the index of the
    #  element selected in the list.
    #  (the argument 'bars' is used only by the fall-back version)
    def selectFromListCurses(message, lst, bars = False):
        def run_dialog(screen):
            curses.use_default_colors()
            dialog = Dialog(screen, message, lst)
            return dialog.run()
        ans = curses.wrapper(run_dialog)
        if ans is None:
            raise Quit
        return ans

except ImportError:
    # Fall back on the plain implementation if curses is not available
    selectFromListCurses = selectFromListPlain

selectFromList = selectFromListCurses

from LbConfiguration.Version import ParseSvnVersion

## @class GetPack
# Main script class for getpack.
class GetPack(Script):
    _version = ParseSvnVersion("$Id$", "$URL$")
    def __init__(self):
        Script.__init__(self, usage = "\n\t%prog [options] package [ [version] ['tag'|'head'|'<default>'] ]"
                                      "\n\t%prog [options] -i [repository [hat]]"
                                      "\n\t%prog [options] --project project version",
                              description = "script to checkout/update and cmt-configure packages"
                                            " from LHCb CVS or Subversion repositories")
        self._packages = None
        self._projects = None
        self._repositories = None

        self.selected_repository = None
        self.selected_hat = None

        self.requested_package = None
        self.requested_package_version = ""

        self.project_name = None
        self.project_version = ""

        self.show_uses_regexp = re.compile(r"# (?P<spaces> *)use (?P<package>[^ ]+) (?P<version>[^ ]+) (?P<hat>[^ ]*)")

        # Return code of the script
        self.retval = 0

    def defineOpts(self):
        """ User options -- has to be overridden """
        self.parser.set_conflict_handler("resolve")

        self.parser.add_option("-i", "--interactive", action = "store_true",
                               help = "prompt for the package to get (using repository/hat to refine the list)")
        self.parser.add_option("-r", "--recursive", action = "store_true",
                               help = "checkout all packages this one directly depends on")
        self.parser.add_option("-H", "--recursive-head", action = "store_true",
                               help = "check out all subsequent packages with head revision "
                               "(it also sets the default version of the requested package/project to 'head')")
        self.parser.add_option("-R", "--really-recursive", action = "store_true",
                               help = "check out >every< package this one depends on")
        self.parser.add_option("-p", "--protocol",
                               help = "preferred access method to revision system [kerberos|kserver|ssh|anonymous]"
                                      " (kserver is valid only for CVS)")
        self.parser.add_option("-f",
                               dest = "protocol",
                               help = "obsolete: same as -p, --protocol")
        self.parser.add_option("-s", "--switch", action = "store_true",
                               help = "svn switch to recommended authenticated protocol after checkout"
                                      " False by default.")
        self.parser.add_option("-u", "--unversioned", action = "store_false",
                               dest = "version_dirs",
                               help = "do not use versioned subdirectories for the packages [default]")
        self.parser.add_option("-v", "--versioned", action = "store_true",
                               dest = "version_dirs",
                               help = "use versioned subdirectories for the packages")
        self.parser.add_option("--user",
                               help = "username to use to connect to the repositories. "
                                      "By default, the user is not explicitly used "
                                      "unless a user name is specified with the "
                                      "environment variable GETPACK_USER.")
        self.parser.add_option("-P", "--project", action = "store_true",
                               help = "create a project top level directory")
        self.parser.add_option("-b", "--batch", action = "store_true",
                               help = "never ask the user if in doubt, but skip the package")
        self.parser.add_option("--config", action = "store_true",
                               dest="config",
                               help = "prevents executing cmt config for each package")
        self.parser.add_option("--no-config", action = "store_false",
                               dest="config",
                               help = "prevents executing cmt config for each package")
        self.parser.add_option("-x", "--exclude", action = "append",
                               metavar = "REPOSITORY",
                               help = "exclude a default repository %s" % __repositories__.keys())
        self.parser.add_option("--user-svn", action = "append",
                               metavar = "URL",
                               help = "add a custom subversion repository to the known ones")
        self.parser.add_option("--user-cvs", action = "append",
                               metavar = "CVSROOT",
                               help = "add a custom CVS repository to the known ones")
        self.parser.add_option("--no-pre", action = "store_true",
                               help = "ignore the pre-release tags (those ending in '-pre[0-9]+')")
        self.parser.add_option("--no-curses", action = "store_true",
                               help = "use the simple selection from the list of packages "
                                      "instead of the curses version")
        self.parser.add_option("--eclipse", action = "store_true",
                               help = "enable eclipse-friendly check-out and configuration")
        self.parser.add_option("--no-eclipse", action = "store_false",
                               dest = "eclipse",
                               help = "prevent eclipse-friendly check-out and configuration")
        self.parser.add_option("--no-eclipse-config", action = "store_false",
                               dest = "eclipse_config",
                               help = "skip the modification of eclipse configurations")
        self.parser.add_option("--branches", action = "store_true",
                               help = "look for versions also in the branches directories "
                                      "(false by default, but implied if the version name ends with 'b')")
        self.parser.add_option("--global-tag", action = "store_true",
                               help = "usable only in conjunction with --project to check out a project global tag/branch")
        self.parser.add_option("-C", "--directory", action = "store",
                               help = "directory from where to start the checkout (default: current directory)")
        self.parser.add_option("--xml", action = "store_true",
                               help = "enable XML output for the list option")
        self.parser.add_option("--list", action = "store_true",
                               help = "print the list of known packages/projects/versions; alone prints the list "
                               "of packages, while with a package name prints the list of versions of that package; "
                               "adding --project prints informations about projects instead of packages")
        self.parser.add_option("-F", "--file", action = "store",
                               help = "take the list of packages and versions to check out from a file. "
                               "Each line of the file has to be of the format 'package version' "
                               "('#' is interpreted as a comment and empty lines are skipped). "
                               "If the file name is '-' the list is taken from the standard input. "
                               "Note: it implies '--batch' and cannot be used for projects.")
        self.parser.add_option('--export', action='store_true',
                               help='use "export" instead of "checkout" to get the packages')
        self.parser.add_option('--ignore-project-info', action='store_true',
                               help='do not use the project.info file')
        self.parser.add_option('--plain', action='store_true',
                               help='just plain checkout, do not add special files')
        self.parser.set_defaults(protocol = "default",
                                 switch = False,
                                 version_dirs = False,
                                 user_svn = [],
                                 user_cvs = [],
                                 no_pre = False,
                                 no_curses = False,
                                 exclude = [],
                                 eclipse = None,
                                 eclipse_config = True,
                                 export = False,
                                 plain = False,
                                 )
        if "GETPACK_USER" in os.environ:
            self.parser.set_defaults(user = os.environ["GETPACK_USER"])

    def _askVersion(self, versions, default = None):
        if self.options.batch:
            # never ask for a version in batch mode
            raise Skip
        ans = None
        if (default is None) or (default not in versions):
            # take the first version if not specified or not available
            default = versions[0]
        while not ans:
            sys.stdout.write("Select a version (%s, (h)ead, (q)uit [%s]): "%
                             (", ".join(versions),default))
            sys.stdout.flush()
            ans = sys.stdin.readline().strip()
            if ans == '':
                ans = default
            elif ans.lower() in [ 'h', 'head' ]:
                return "head"
            elif ans.lower() in [ 'q', 'quit' ]:
                raise Quit
            elif ans not in versions:
                print "Version '%s' not valid!" % ans
                ans = None
        return ans

    def _doCMTConfig(self, cwd):
        if os.path.exists(os.path.join(cwd,"requirements")):
            self.log.info("Executing 'cmt config' in '%s'", cwd)
            #proc = Popen(["cmt", "config"], cwd = cwd, stdout = PIPE, stderr = PIPE)
            proc = Popen(["cmt config"], cwd = cwd, shell = True, stdout = PIPE, stderr = PIPE)
            proc.communicate()
            ## @todo: check for success of 'cmt config'
            if sys.platform.startswith("win"):
                self.log.info("Executing 'cmt build vsnet' in '%s'", cwd)
                #proc = Popen(["cmt", "build", "vsnet"], cwd = cwd, stdout = PIPE, stderr = PIPE)
                proc = Popen(["cmt build vsnet"], shell = True, cwd = cwd, stdout = PIPE, stderr = PIPE)
                proc.communicate()
                ## @todo: check for success of 'cmt build vsnet'
        else:
            self.log.warning("Cannot find requirements file, 'cmt config' skipped.")

    def _getModuleRepo(self, module, isProject = False):
        """
        Return the repository containing the package/project, asking the user if
        it is in more than one repository.
        """
        try:
            if isProject:
                kind = "Project"
                reps = self.projects[module]
            else:
                kind = "Package"
                reps = self.packages[module]

            if len(reps) > 1:
                if self.options.batch:
                    # never ask for a repository in batch mode
                    raise Skip
                lst = []
                for k in reps:
                    lst.append('%s (%s)' % (k, self.repositories[k]))
                idx = selectFromList("%s '%s' in more than one repository, choose the repository." % (kind, module),
                                     lst)
            else:
                idx = 0
            return self.repositories[reps[idx]]
        except KeyError:
            return None

    def _switchProtocol(self, local_path, protocol_start, protocol_end):
        ''' use svn switch --relocate to change the repository location from start to end
        local_path: local path to checked-out VCS package
        protocol_start: the protocol which was originally used
        protocol_end: the protocol to go to

        '''
        self.log.debug("svn switching: "+local_path+", "+protocol_start+" to "+protocol_end)
        from LbUtils.VCS import svn_command
        from LbConfiguration.Repository import repository_shortpaths as rps
        if protocol_start not in rps or protocol_end not in rps:
            self.log.warning("no mechanism to translate %s to %s, skipping the switch statement" % (protocol_start,protocol_end))
            return False
        if protocol_start == protocol_end or rps[protocol_start]==rps[protocol_end]:
            self.log.info("no need to switch from %s to %s, skipping the switch statement" % (protocol_start,protocol_end))
            return True
        #insert username setting here
        for protocol in [protocol_start, protocol_end]:
            if self.options.user and rps[protocol] != rps["anonymous"]:
                rps[protocol].user=self.options.user
        #user repositories cannot be handled so simply, since it's not clear what the protocol will need to do
        # the short repository paths are only known for CERN-based lhcb repositories
        if self.options.user_svn or self.options.user_cvs:
            self.log.warning("cannot switch when using a user repository, specify the protocol directly please and turn of switching")
            return False
        return svn_command("switch","--relocate","%s" % rps[protocol_start],"%s" % rps[protocol_end],local_path)

    def _detectProtocol(self, directory):
        """
        detect the short URL used in the protocol of the checked out directory
        """
        self.log.debug("detecting protocol at existing directory: %s", directory)
        from LbUtils.VCS import svn_command
        from LbConfiguration.Repository import repository_shortpaths as rps
        out = svn_command("info", directory, stderr = None)[0]
        url = (l for l in out.splitlines() if l.startswith("URL:")).next().split()[-1]
        #print url
        for protocol in rps:
            if str(rps[protocol]) in url:
                self.log.debug("detected %s", protocol)
                return protocol
        return "default"

    def _ifExistsAction(self,directory):
        """
        callback command for inside the repository.checkout command, only used if switch is true
        """
        #If I'm not supposed to be doing any switching, just throw the usual error
        if not self.options.switch:
            return True
        protocol_old=self._detectProtocol(directory)
        self._switchProtocol(directory,protocol_old,self.options.protocol)
        return True

    def checkout(self, package, version = "trunk"):
        # Get the repository containing the package
        rep = self._getModuleRepo(package, isProject = False)
        # The version names "trunk", "head" and "<default>" are treated in a special way:
        # trunk and head are always valid, while <default> is replaced by the guessed default tag
        # or trunk if no default is found.
        if version.lower() not in ["trunk", "head", "<default>"]:
            versions = rep.listVersions(package)
            if not versions:
                self.log.warning("No version found for package '%s', using 'trunk'" % package)
                version = "trunk"
                versions = ["trunk"] # this is to pass the next check
            if version:
                if version not in versions:
                    vers = None # temporary variable
                    if not self.options.no_pre:
                        # look for "-pre" tags
                        pre_v = version + "-pre"
                        for v in versions:
                            if v.startswith(pre_v):
                                vers = v
                                break
                    if vers: # this is False if the -pre version was not wanted or not found
                        self.log.warning("Version '%s' not found for package '%s', using '%s'" % (version, package, vers))
                        version = vers
                    else:
                        # Since the version is not in the filtered list and it is not a "-pre"
                        # try to ask to the repository if it knows about it.
                        if not rep.hasVersion(package, version):
                            # No way to match the version string using any of the rules
                            self.log.warning("Version '%s' not found for package '%s'" % (version, package))
                            version = self._askVersion(versions, guessDefaultVersion(package))
            else:
                self.log.warning("Version not specified for package '%s'" % package)
                version = self._askVersion(versions, guessDefaultVersion(package))
        elif version.lower() == "<default>":
            # Use the default tag or trunk (if none found)
            version = guessDefaultVersion(package) or "trunk"

        # Fix the case of the special version "head" (done here because we may have changed
        # the value of version since last time we checked)
        if version.lower() in ["trunk", "head"]:
            version = version.lower()
        self.log.info("Checking out %s %s (from '%s')" % (package, version, rep.repository))
        try:
            rep.checkout(package, version, vers_dir=self.options.version_dirs,
                         eclipse=self.options.eclipse,
                         ifExistsAction=self._ifExistsAction,
                         export=self.options.export,
                         plain=self.options.plain)
        except TypeError:
            #need to have this option in case it's a CVS user repository, don't want to break those immediately!
            rep.checkout(package, version, vers_dir = self.options.version_dirs, eclipse = self.options.eclipse)
        pkgtopdir=""
        # Call "cmt config"
        if self.options.version_dirs:
            pkgtopdir = os.path.join(package, version)
            pkgdir = os.path.join(package, version, "cmt")
        else:
            pkgtopdir = package
            pkgdir = os.path.join(package, "cmt")
        #svn switch?
        if self.options.switch:
            self._switchProtocol(pkgtopdir,self.options.protocol,"authenticated")
        if self.options.eclipse and self.options.eclipse_config:
            # add package-specific configuration
            eclipseConfigurationAddPackage(os.getcwd(), package)
        if self.options.config:
            self._doCMTConfig(cwd = pkgdir)
        # return the path to the cmt directory of the package to be able to call
        # a "cmt show uses" and get the packages needed with the recursion
        return (package, version, pkgdir)

    def checkoutProject(self, project, version):
        rep = self._getModuleRepo(project, isProject = True)
        if version.lower() != "head": # head is always valid
            versions = rep.listVersions(project, isProject = True)
            if not versions:
                raise RuntimeError("No version found for project '%s'" % project)

            if version:
                if version not in versions:
                    vers = None # temporary variable
                    if not self.options.no_pre:
                        # look for "-pre" tags
                        pre_v = version + "-pre"
                        for v in versions:
                            if v.startswith(pre_v):
                                vers = v
                                break
                    if vers: # this is Flase if the -pre version was not wanted or not found
                        self.log.warning("Version '%s' not found for project '%s', using '%s'" % (version, project, vers))
                        version = vers
                    else:
                        # Since the version is not in the filtered list and it is not a "-pre"
                        # try to ask to the repository if it knows about it.
                        if not rep.hasVersion(project, version, isProject = True):
                            self.log.warning("Version '%s' not found for project '%s'" % (version, project))
                            version = self._askVersion(versions)
            else:
                self.log.warning("Version not specified for project '%s'" % project)
                version = self._askVersion(versions)

        # Fix the case of the special version "HEAD"
        if version.lower() == "head":
            version = version.upper()
        self.log.info("Checking out %s %s (from '%s')" % (project, version, rep.repository))
        rep.checkout(project, version, vers_dir=True, project=True,
                     eclipse= self.options.eclipse,
                     global_tag=self.options.global_tag,
                     export=self.options.export)
        project = project.upper()
        pkgdir =  os.path.abspath(os.path.join(project, "%s_%s" % (project, version)))
        #svn switch?
        if self.options.switch:
            self._switchProtocol(pkgdir,self.options.protocol,"authenticated")
        if self.options.eclipse and self.options.eclipse_config:
            # add project-specific configuration
            createEclipseConfiguration(pkgdir,
                                       os.environ.get("CMTPROJECTPATH",""))
        return (project, version, pkgdir)


    ## Prepare the repository access objects according to options
    @property
    def repositories(self):
        if self._repositories is None:
            #--- Prepare repositories urls
            # filter the requested protocols for the know repositories
            repositories = {}
            for rep in __repositories__:
                if rep in self.options.exclude:
                    continue # skip excluded repositories
                reps = __repositories__[rep]
                if self.options.protocol in reps:
                    repositories[rep] = reps[self.options.protocol]
            if not repositories:
                self.log.error("Unable to find a repository for the specified protocol")
                return 1

            # set the usernames
            if self.options.user and ( self.options.protocol not in [ "anonymous", "kserver", "kerberos" ] ):
                for rep in repositories.values():
                    rep.user = self.options.user

            if self.options.user_svn or self.options.user_cvs:
                from urllib import splittype, splituser, splithost
                # add user-defined repositories
                i = 0
                for rep in self.options.user_svn:
                    # FIXME: need some error checking
                    protocol, rest = splittype(rep)
                    rest, path = splithost(rest)
                    user, host = splituser(rest)
                    repositories["user_svn_%d" % i] = SVNReposInfo(protocol, host, path, user)
                    i += 1
                i = 0
                for rep in self.options.user_cvs:
                    # FIXME: need some error checking
                    dummy, protocol, rest, path = rep.split(":")
                    user, host = splituser(rest)
                    repositories["user_cvs_%d" % i] = CVSReposInfo(protocol, host, path, user)
                    i += 1

            # connect to repositories
            self._repositories = {}
            for rep in repositories:
                url = str(repositories[rep])
                self.log.info("Using repository '%s' for '%s'", url, rep)
                bridge = rcs.connect(url)
                if bridge:
                    # enable branches if requested
                    bridge.useBranches = self.options.branches
                    self._repositories[rep] = bridge
                else:
                    self.log.warning("Invalid repository URL '%s'", rep)
            #--- Now we are ready to talk to the repositories
        return self._repositories

    ## Retrieve the list of packages from the known repositories
    @property
    def packages(self):
        if self._packages is None:
            repos = self.repositories
            if self.selected_repository:
                rep = self.selected_repository.lower()
                if rep not in self.repositories:
                    self.log.error("Uknown repository alias '%s' (allowed: %r)",
                                   self.selected_repository, self.repositories.keys())
                    raise Skip # @fixme: (MCl) I'm not sure it is the right way to stop
                repos = { rep: self.repositories[rep] }

            # Collect the list of packages
            self._packages = {}
            for rep in repos:
                self.log.debug("Retrieving packages list from '%s'", rep)
                for p in repos[rep].listPackages(self.selected_hat):
                    if p in self._packages:
                        self._packages[p].append(rep)
                    else:
                        self._packages[p] = [ rep ]
        return self._packages

    ## Retrieve the list of packages from the known repositories
    @property
    def projects(self):
        if self._projects is None:
            repos = self.repositories
            if self.selected_repository:
                rep = self.selected_repository.lower()
                if rep not in self.repositories:
                    self.log.error("Unknown repository alias '%s' (allowed: %r)",
                                   self.selected_repository, self.repositories.keys())
                    return 1
                repos = { rep: self.repositories[rep] }

            # Collect the list of projects
            self._projects = {}
            for rep in repos:
                self.log.debug("Retrieving projects list from '%s'", rep)
                for p in repos[rep].listProjects():
                    if p == "packages":
                        # Ignore the special "fake" project "packages" (introduced in
                        # the SVN repository version 2.0)
                        continue
                    if p in self._projects:
                        self._projects[p].append(rep)
                    else:
                        self._projects[p] = [ rep ]
        return self._projects

    def parseOpts(self, args):
        # Replace old options '-rr' and '-rh' into the new equivalents
        for old, new in [ ('-rr', '-R'), ('-rh', '-H') ]:
            while old in args:
                self.log.warning("Special option '%s' is deprecated and will "
                                 "disappear in a future version. Use '%s' instead.",
                                 old, new)
                args[args.index(old)] = new

        Script.parseOpts(self, args)

        # Set the default versions if --recursive-head is requested
        if self.options.recursive_head:
            self.project_version = "head"
            self.requested_package_version = "head"

        # Validate and parse positional arguments
        # Here we have to treat the options that change the meaning of the arguments
        if self.options.interactive:
            if self.options.project:
                self.parser.error("Options '-i' and '--project' cannot be used at the same time")
            if self.options.list:
                self.parser.error("Options '-i' and '--list' cannot be used at the same time")
            # getpack.py -i [repos [hat]]
            self.selected_repository = None
            self.selected_hat = None
            if self.args:
                self.selected_repository = self.args.pop(0).lower()
                self.log.info("Query only repository '%s'", self.selected_repository)
            if self.args:
                self.selected_hat = self.args.pop(0)
                self.log.info("Search for hat '%s'", self.selected_hat)
            if self.args:
                self.parser.error("Option '-i' requires maximum 2 arguments")
        elif self.options.project:
            if self.options.file:
                self.parser.error("Option '--file' cannot be used together with '--project'")
            # getpack.py --project project [version]
            if not self.options.list:
                if self.args:
                    self.project_name = self.args.pop(0)
                    if self.args:
                        self.project_version = self.args.pop(0)
                    if self.args:
                        self.parser.error("requires maximum 2 arguments")
                else:
                    self.parser.error("project name is required")
            # I want to use the bare version number and not the conventional one
            if self.project_version and self.project_version.startswith(self.project_name.upper() + "_"):
                self.project_version = self.project_version[len(self.project_name)+1:]
        elif self.options.file:
            if self.args:
                self.parser.error("You cannot specify packages with both '--file' and on the command line")
            self.options.batch = True
        else:
            # getpack.py [-u] package [version]
            if not self.options.list:
                if self.args:
                    self.requested_package = self.args.pop(0)
                    if self.args:
                        self.requested_package_version = self.args.pop(0)
                    if self.args:
                        self.parser.error("requires maximum 2 arguments")
                else:
                    self.parser.error("package name is required unless '-i' or '--list' is used")

        tmpVersion = self.project_version or self.requested_package_version or ""
        from LbConfiguration.Version import isValidBranchName
        if isValidBranchName(tmpVersion):
            # version ending with 'b' implies --branches
            self.options.branches = True

        if self.options.really_recursive or self.options.recursive_head:
            # recursion is implied by the aboves
            self.options.recursive = True

        if self.options.global_tag and not self.options.project:
            self.parser.error("Option '--global-tag' can only be used together with '--project'")

        if self.options.no_curses:
            global selectFromList
            selectFromList = selectFromListPlain

    def askPackage(self):
        if self.options.batch:
            # never ask for a package in batch mode
            raise Skip
        # select package
        keys = self.packages.keys()
        keys.sort()
        idx = selectFromList("Select the package", keys, bars = True)
        return keys[idx]

    def askProject(self):
        if self.options.batch:
            # never ask for a project in batch mode
            raise Skip
        # select package
        keys = self.projects.keys()
        keys.sort()
        idx = selectFromList("Select the project", keys, bars = True)
        return keys[idx]

    def _getNeededPackages(self, pkgdir):
        required = {}
        if self.options.recursive:
            ## @todo: --recursive is only for the first package
            self.options.recursive = False

            self.log.debug("Executing 'cmt show uses' in '%s'", pkgdir)
            #proc = Popen(["cmt", "show", "uses"], cwd = pkgdir, stdout = PIPE, stderr = PIPE)
            proc = Popen(["cmt show uses"], shell = True, cwd = pkgdir, stdout = PIPE, stderr = PIPE)
            out, _err = proc.communicate()

            for l in out.splitlines():
                m = self.show_uses_regexp.match(l)
                # group("spaces") is present only if we have more than one space
                # between '#' and 'use', which implies indirect dependency
                if m and (self.options.really_recursive or (not m.group("spaces"))):
                    pkg = m.group("package")
                    if m.group("hat"):
                        pkg = m.group("hat") + "/" + pkg
                    # do not overwrite the already found (good) versions
                    if pkg not in required or '*' in required[pkg]:
                        if self.options.recursive_head:
                            required[pkg] = "head"
                        else:
                            required[pkg] = m.group("version")
            self.log.debug("Packages required: %r" % required)
        return required

    def getpack(self, packages=None):
        # Dictionaries (pkg->version) of done, skipped and to-do packages
        done_packages = {}
        skipped_packages = {}

        if packages:
            todo_packages = dict(packages)
        elif not self.options.file:
            if self.requested_package and self.requested_package not in self.packages:
                self.log.error("Unknown package '%s'!", self.requested_package)
                self.requested_package = None
            if self.requested_package is None:
                self.requested_package = self.askPackage()
            todo_packages = { self.requested_package: self.requested_package_version }
        else:
            # read the list of packages+versions from the file
            if self.options.file == '-':
                f = sys.stdin
            else:
                f = open(self.options.file)
            import string
            # remove comments and whitespaces from a line
            stripLine = lambda l: string.strip(l[:l.find('#')])
            try:
                todo_packages = dict( # make a dictionary
                                     map(string.split, # of all the whitespace-separated pairs
                                         filter(None, map(stripLine, f)) # in the non-empty lines
                                         )
                                     )
            except Exception, x:
                self.log.error("Problems reading the list of packages: (%s) %s",
                               x.__class__.__name__,
                               x)
                self.retval = 1
                return

        if self.options.eclipse is None:
            # no user opinion about the checkout/configuration policy, try to guess:
            # if we are in an Eclipse workspace (i.e. exists(<dest>/../.metadata)
            # we enable it
            self.options.eclipse = os.path.exists(os.path.join(os.path.dirname(os.path.abspath('.')), ".metadata"))
        self.log.debug("Eclipse checkout %s", "enabled" if self.options.eclipse else "disabled")
        self.log.debug("Starting processing loop")
        current_count = 0
        # process the list of pending packages
        while todo_packages:
            # get an item
            pkg, vers = todo_packages.popitem()
            try:
                # Check if we can process it
                if pkg in skipped_packages or pkg in done_packages:
                    # Already processed
                    continue
                elif pkg not in self.packages:
                    # Not found
                    raise Skip
                # Check out the package
                done_packages[pkg] = self.checkout(pkg, vers)
                pkgdir = done_packages[pkg][2]
                # create special CMakeLists.txt for data packages (if needed)
                # see LBCORE-751
                if os.path.basename(pkg) in __data_packages__:
                    # note that pkgdir includes the 'cmt' directory
                    createDataPackageCMakeLists(pkg,
                                                os.path.join(pkgdir, os.pardir,
                                                             'CMakeLists.txt'))
                # See if we need to check out something else
                todo_packages.update(self._getNeededPackages(pkgdir))
                # Progress report
                current_count += 1
                total_count = current_count + len(todo_packages)
                print "Checked out package %s %s (%d/%d)" % (pkg, done_packages[pkg][1],
                                                             current_count,
                                                             total_count)
                # ensure that the previous message is printed at the right moment
                sys.stdout.flush()

            except Skip:
                skipped_packages[pkg] = vers
                self.log.warning("Skipped package %s %s", pkg, vers)

        print "Processed packages:"
        for p in sorted(done_packages):
            print "\t%s\t%s" % done_packages[p][:2]

        if skipped_packages:
            print "Skipped packages (not in the repositories):"
            for p in sorted(skipped_packages):
                print "\t%s\t%s" % (p, skipped_packages[p])
            self.retval = 1 # this is a failure

        # touch the CMakeLists.txt file, if there, to ensure that the project is
        # re-configured
        if os.path.exists('CMakeLists.txt'):
            os.utime('CMakeLists.txt', None)

    def _fixProjectNameCase(self, name):
        # Check if the project is known using a case insensitive comparison
        for known_proj in self.projects:
            if name.upper() == known_proj.upper():
                # ensure that we use the right case when talking to the repository
                return known_proj
        self.log.error("Unknown project '%s'!", name)
        return None

    def getproject(self):
        self.project_name = self._fixProjectNameCase(self.project_name)
        if self.project_name is None:
            self.project_name = self.askProject()

        proj = self.checkoutProject(self.project_name, self.project_version)
        if not self.options.plain:
            # initialize the checkout out project for building
            # (does not overwrite)
            initProject(proj[2])
        if self.options.recursive:
            packages = None
            # try to get the list of packages from project.info
            proj_info_file = os.path.join(proj[2], 'project.info')
            if not self.options.ignore_project_info and os.path.exists(proj_info_file):
                from ConfigParser import ConfigParser
                project_info = ConfigParser()
                project_info.optionxform = str # make the options case sensitive
                project_info.read(proj_info_file)
                if project_info.has_section('Packages'):
                    self.log.info('Using packages from project.info')
                    packages = project_info.items('Packages')
                    if self.options.recursive_head:
                        # override the version if we got --recursive-head
                        packages = [(name, 'trunk') for name, _ in packages]
                    # if we got the packages from project.info, we should not recurse
                    self.options.recursive = False
            if not packages:
                # get the conatiner package and recurse
                try:
                    self.requested_package = None
                    for l in open(os.path.join(proj[2],"cmt","project.cmt")):
                        l = l.strip().split()
                        if len(l) == 2 and l[0] == "container":
                            self.requested_package = l[1]
                            break
                    if not self.requested_package:
                        # project.cmt does not have the "container" instruction, let's assume we want ProjectSys
                        psys = proj[0].upper() + "SYS"
                        for p in self.packages:
                            if p.upper() == psys:
                                self.requested_package = p
                                break
                    self.requested_package_version = proj[1] # this is the actual version extracted
                except IOError, x:
                    self.log.error("Cannot get data: %s", x)
            self.log.info("Retrieving packages in %s", proj[2])
            old_cwd = os.getcwd()
            try:
                os.chdir(proj[2]) # go to the project directory
                if not packages:
                    self.log.info("Container package is %s %s",
                                  self.requested_package, self.requested_package_version)
                self.getpack(packages)
            finally:
                # revert to initial directory (needed when using GetPack from python)
                os.chdir(old_cwd)
        print "Checked out project %s %s in '%s'" % proj

    def _makeListXML(self, doPackages = False):
        """
        Create the DOM document for the list of projects/packages.
        """
        import xml.dom
        impl = xml.dom.getDOMImplementation()
        doc = impl.createDocument(None, "projects", None)

        def nestedAdd(dict_var, path, value = None):
            """
            Add an entry to a nested dictionary.
            """
            if value is None:
                value = path
            if "/" in path:
                parent, child = path.split("/", 1)
                # ensure that we have a dictionary to pass to the recursion
                # (needed if a hat appears as a package too)
                if parent not in dict_var or not hasattr(dict_var[parent], "__setitem__"):
                    dict_var[parent] = {}
                nestedAdd(dict_var[parent], child, value)
            else:
                if not path in dict_var: # do not overwrite
                    dict_var[path] = value

        def dict2tree(parent, dct, top = True):
            """
            Create nested elements from a nested dictionary.
            """
            for k in sorted(dct.keys()):
                if type(dct[k]) is str:
                    el = doc.createElement("package")
                    el.setAttribute("fullname", dct[k])
                else:
                    el = doc.createElement(top and "project" or "hat")
                    dict2tree(el, dct[k], top = False)
                el.setAttribute("name", k)
                parent.appendChild(el)

        if doPackages:
            # get the list of packages in the known repositories
            packages = {}
            for r in self.repositories.values():
                if hasattr(r, "modules"):
                    packages.update(r.modules)
            # prepare the list of packages as:
            # { "Proj1":
            #    { "Hat1":
            #      { "Hat2":
            #        { "Package": "Hat1/Hat2/Package" }
            #      }
            #    }
            # }
            projects = {}
            for pack in packages:
                proj = packages[pack]
                nestedAdd(projects, "%s/%s" % (proj, pack), pack)
        else:
            projects = {}
            for p in self.projects:
                projects[p] = {}

        # create the XML structure
        dict2tree(doc.documentElement, projects)
        return doc.toprettyxml()

    def _list_packages(self):
        """
        List function specialized in packages.
        """
        if self.args:
            self.log.debug("Requested versions for packages %r", self.args)
            # we got a list of packages, so we need to print their versions
            # first collect the data as {"package": (["v1","v2",...], "default")}
            versions = {}
            for package in self.args:
                rep = self._getModuleRepo(package, isProject = False)
                if rep:
                    versions[package] = (["trunk"] + rep.listVersions(package),
                                         guessDefaultVersion(package))
                else:
                    self.log.warning("Unknown package '%s'!", package)
            if not self.options.xml:
                for package in sorted(versions):
                    print package
                    packvers, default = versions[package]
                    for v in packvers:
                        print "  %s%s" % (v, (v == default) and " (default)" or "")
            else:
                # @todo: factor out the generation of XML for versions
                import xml.dom
                impl = xml.dom.getDOMImplementation()
                doc = impl.createDocument(None, "packages", None)
                for package in sorted(versions):
                    pack = doc.createElement("package")
                    pack.setAttribute("name", package.rsplit("/", 1)[-1])
                    pack.setAttribute("fullname", package)
                    vers = doc.createElement("versions")
                    packvers, default = versions[package]
                    for v in packvers:
                        el = doc.createElement("version")
                        if v == default:
                            el.setAttribute("default", "true")
                        el.appendChild(doc.createTextNode(v))
                        vers.appendChild(el)
                    pack.appendChild(vers)
                    doc.documentElement.appendChild(pack)
                print doc.toprettyxml()
        else:
            self.log.debug("Preparing list of packages")
            if not self.options.xml:
                for p in sorted(self.packages):
                    print p
            else:
                print self._makeListXML(doPackages = True)

    def _list_projects(self):
        """
        List function specialized in packages.
        """
        if self.args:
            self.log.debug("Requested versions for projects %r", self.args)
            # in this case we need to print the versions of the projects
            projects = filter(None, map(self._fixProjectNameCase, self.args))
            # prepares the structure for the versions (as in _list_packages)
            versions = {}
            for project in projects:
                # no need to check the repository because it is implicit in the previous check
                # (self._fixProjectNameCase)
                rep = self._getModuleRepo(project, isProject = True)
                versions[project] = rep.listVersions(project, isProject = True)
            if not self.options.xml:
                for project in sorted(versions):
                    print project
                    for v in versions[project]:
                        print "  %s" % v
            else:
                # @todo: factor out the generation of XML for versions
                import xml.dom
                impl = xml.dom.getDOMImplementation()
                doc = impl.createDocument(None, "projects", None)
                for project in sorted(versions):
                    proj = doc.createElement("project")
                    proj.setAttribute("name", project)
                    vers = doc.createElement("versions")
                    for v in versions[project]:
                        el = doc.createElement("version")
                        el.appendChild(doc.createTextNode(v))
                        vers.appendChild(el)
                    proj.appendChild(vers)
                    doc.documentElement.append(proj)
                print doc.toprettyxml()
        else:
            self.log.debug("Preparing list of projects")
            if not self.options.xml:
                for p in sorted(self.projects):
                    print p
            else:
                print self._makeListXML(doPackages = False)

    def list(self): #@ReservedAssignment
        """
        Produce the list of packages/projects/versions in plain text or XML.
        """
        if self.options.project:
            self._list_projects()
        else:
            self._list_packages()

    def main(self):
        try:
            # Switch to the working directory
            if self.options.directory:
                self.log.debug("Switching to directory '%s'", self.options.directory)
                os.chdir(self.options.directory)
            if self.options.list:
                self.list()
            elif self.options.project:
                self.getproject()
            else:
                # check if the current directory looks like a project directory
                # (LBCORE-528)
                from os.path import exists, join

                def askConfirmation():
                    'Helper function to ask for configmation'
                    sys.stdout.write('WARNING: %s does not look like a project.\n'
                                     'Do you really want to check out here? (y/[n]) '%
                                     os.getcwd())
                    ans = sys.stdin.readline().strip()
                    return ans.lower().startswith('y')

                if (self.options.batch or
                    (exists('CMakeLists.txt') and exists('toolchain.cmake')) or
                    exists(join('cmt', 'project.cmt')) or
                    askConfirmation()):
                    # we can continue
                    self.getpack()
                else:
                    raise Quit()

        except Skip:
            print "Stopped!"
            self.retval = 1
        except Quit:
            print "Quit!"
            self.retval = 1
        except rcs.RcsError, x:
            print "Unrecoverable error:", x
            self.retval = 1

        #from pprint import pprint
        #pprint(packages)
        return self.retval
