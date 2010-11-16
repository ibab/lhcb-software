#!/usr/bin/env python
# $Id: GetPack.py,v 1.19 2010-01-26 19:40:25 marcocle Exp $

import os, sys, re
from subprocess import Popen, PIPE

from LbUtils.Script import Script
from LbConfiguration import createProjectMakefile
from LbConfiguration.Repository import repositories as __repositories__, SVNReposInfo, CVSReposInfo
import rcs

import __builtin__
if "set" not in dir(__builtin__):
    # pylint: disable-msg=W0622
    from sets import Set as set

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


## @class GetPack
# Main script class for getpack.
class GetPack(Script):
    _version = "$Id: GetPack.py,v 1.19 2010-01-26 19:40:25 marcocle Exp $".replace("$","").replace("Id:","").strip()
    def __init__(self):
        Script.__init__(self, usage = "\n\t%prog [options] package [ [version] ['tag'|'head'] ]"
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
        self.parser.add_option("--no-config", action = "store_true",
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
        self.parser.set_defaults(protocol = "default",
                                 version_dirs = False,
                                 user_svn = [],
                                 user_cvs = [],
                                 no_pre = False,
                                 no_curses = False,
                                 exclude = [],
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

    def checkout(self, package, version = "head"):
        reps = self.packages[package]
        if len(reps) > 1:
            if self.options.batch:
                # never ask for a repository in batch mode
                raise Skip
            lst = []
            for k in reps:
                lst.append('%s (%s)' % (k, self.repositories[k]))
            idx = selectFromList("Package '%s' in more than one repository, choose the repository." % package,
                                 lst)
        else:
            idx = 0
        rep = self.repositories[reps[idx]]
        if version.lower() != "head": # head is always valid
            versions = rep.listVersions(package)
            if not versions:
                self.log.warning("No version found for package '%s', using 'head'" % package)
                version = "head"
                versions = ["head"] # this is to pass the next check
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
        # Fix the case of the special version "head"
        if version.lower() == "head":
            version = version.lower()
        self.log.info("Checking out %s %s (from '%s')" % (package, version, rep.repository))
        rep.checkout(package, version, vers_dir = self.options.version_dirs, eclipse = self.options.eclipse)
        # Call "cmt config"
        if self.options.version_dirs:
            pkgdir =  os.path.join(package, version, "cmt")
        else:
            pkgdir =  os.path.join(package, "cmt")
        if not self.options.no_config:
            self._doCMTConfig(cwd = pkgdir)
        # return the path to the cmt directory of the package to be able to call
        # a "cmt show uses" and get the packages needed with the recursion
        return (package, version, pkgdir)

    def checkoutProject(self, project, version):
        reps = self.projects[project]
        if len(reps) > 1:
            lst = []
            for k in reps:
                lst.append('%s (%s)' % (k, self.repositories[k]))
            idx = selectFromList("Project '%s' in more than one repository, choose the repository." % project, lst)
        else:
            idx = 0
        rep = self.repositories[reps[idx]]
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
                        self.log.warning("Version '%s' not found for project '%s'" % (version, project))
                        version = self._askVersion(versions)
            else:
                self.log.warning("Version not specified for project '%s'" % project)
                version = self._askVersion(versions)

        # Fix the case of the special version "HEAD"
        if version.lower() == "head":
            version = version.upper()
        self.log.info("Checking out %s %s (from '%s')" % (project, version, rep.repository))
        rep.checkout(project, version, vers_dir = True, project = True, eclipse = self.options.eclipse)
        project = project.upper()
        pkgdir =  os.path.join(project, "%s_%s" % (project, version))
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
                args[args.index(old)] = new
        Script.parseOpts(self, args)
        # Set the default versions if --recursive-head is requested
        if self.options.recursive_head:
            self.project_version = "head"
            self.requested_package_version = "head"
        # Validate and parse positional arguments
        if self.options.interactive:
            if self.options.project:
                self.parser.error("Options '-i' and '--project' cannot be used at the same time")
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
            # getpack.py --project project [version]
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
        else:
            # getpack.py [-u] package [version]
            if self.args:
                self.requested_package = self.args.pop(0)
                if self.args:
                    self.requested_package_version = self.args.pop(0)
                if self.args:
                    self.parser.error("requires maximum 2 arguments")
            else:
                self.parser.error("package name is required unless '-i' is used")

        if self.options.really_recursive or self.options.recursive_head:
            # recursion is implied by the aboves
            self.options.recursive = True

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

    def getpack(self):
        if self.requested_package and self.requested_package not in self.packages:
            self.log.error("Unknown package '%s'!", self.requested_package)
            self.requested_package = None
        if self.requested_package is None:
            self.requested_package = self.askPackage()

        # Dictionaries (pkg->version) of done, skipped and to-do packages
        done_packages = {}
        skipped_packages = {}
        todo_packages = { self.requested_package: self.requested_package_version }

        self.log.debug("Starting processing loop")
        # process the list of pending packages
        while todo_packages:
            # get an item
            pkg, vers = todo_packages.popitem()
            # Check if we can process it
            if pkg in skipped_packages or pkg in done_packages:
                # Already processed
                continue
            elif pkg not in self.packages:
                # Not found
                skipped_packages[pkg] = vers
                continue
            try:
                # Check out the package
                done_packages[pkg] = self.checkout(pkg, vers)
                pkgdir = done_packages[pkg][2]
                # See if we need to check out something else
                todo_packages.update(self._getNeededPackages(pkgdir))
            except Skip:
                skipped_packages[pkg] = vers

        print "Processed packages:"
        pkgs = done_packages.keys()
        pkgs.sort()
        for p in pkgs:
            print "\t%s\t%s" % done_packages[p][:2]

        if skipped_packages:
            print "Skipped packages (not in the repositories):"
            pkgs = skipped_packages.keys()
            pkgs.sort()
            for p in pkgs:
                print "\t%s\t%s" % (p, skipped_packages[p])

    def getproject(self):
        if self.project_name:
            # Check if the project is known using a case insensitive comparison
            found = False
            for known_proj in self.projects:
                if self.project_name.upper() == known_proj.upper():
                    # ensure that we use the right case when talking to the repository
                    self.project_name = known_proj
                    found = True
                    break # no reason to continue looping
            if not found:
                self.log.error("Unknown project '%s'!", self.project_name)
                self.project_name = None
        if self.project_name is None:
            self.project_name = self.askProject()

        proj = self.checkoutProject(self.project_name, self.project_version)
        # create a project Makefile for the checked out project (if not present)
        createProjectMakefile(os.path.join(proj[2], "Makefile"))
        if self.options.recursive:
            # get the conatiner package too, etc.
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
                self.log.info("Retrieving packages in %s", proj[2])
                old_cwd = os.getcwd()
                try:
                    os.chdir(proj[2]) # go to the project directory
                    self.log.info("Container package is %s %s", self.requested_package, self.requested_package_version)
                    self.getpack()
                finally:
                    # revert to initial directory (needed when using GetPack from python)
                    os.chdir(old_cwd)
            except IOError, x:
                self.log.error("Problems opening the 'project.cmt' file. %s", x)
        print "Checked out project %s %s in '%s'" % proj

    def main(self):
        try:
            if self.options.project:
                self.getproject()
            else:
                self.getpack()
        except Skip:
            print "Stopped!"
        except Quit:
            print "Quit!"

        #from pprint import pprint
        #pprint(packages)
        return 0
