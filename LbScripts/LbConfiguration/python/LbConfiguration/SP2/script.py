###############################################################################
# (c) Copyright 2013 CERN                                                     #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
__author__ = 'Marco Clemencic <marco.clemencic@cern.ch>'

import os
import sys
import logging
import EnvConfig

from LbConfiguration.SP2.lookup import (getEnvXmlPath, findProject,
                                        findDataPackage, getLCGRelocation)
from LbConfiguration.SP2.version import (isValidVersion, expandVersionAlias,
                                         DEFAULT_VERSION)
from LbConfiguration.SetupProject import FixProjectCase


auto_override_projects = [('Compat', 'latest')]

def decodePkg(s):
    '''
    Translate a string declaring a data package into a pair (package, version).

    The user can specify the data package they want to use in different ways and
    in all cases we need to return the package name with "hat" (also when the
    hat is implied) and the version pattern.

    >>> decodePkg('Hat/Package')
    ('Hat/Package', '*')
    >>> decodePkg('Package v7r*')
    ('Package', 'v7r*')
    >>> decodePkg('SQLDDDB v7r*')
    ('Det/SQLDDDB', 'v7r*')
    >>> decodePkg('Package v1r0 Hat')
    ('Hat/Package', 'v1r0')
    >>> decodePkg('Hat/Package.v1r0')
    ('Hat/Package', 'v1r0')
    >>> decodePkg('Hat.Package.v1r0')
    ('Hat/Package', 'v1r0')
    >>> decodePkg('a b c d')
    Traceback (most recent call last):
       ...
    ValueError: too many words in package declaration 'a b c d'
    '''
    #split on spaces
    l = s.split()
    if len(l) == 2:
        pkg, vers = l
    elif len(l) == 3:
        pkg, vers, hat = l
        pkg = '{0}/{1}'.format(hat, pkg)
    elif len(l) == 1:
        if '.' in s:
            pkg, vers = s.rsplit('.', 1)
            pkg = pkg.replace('.', '/')
        else:
            pkg = s
            vers = '*'
    else:
        raise ValueError('too many words in package declaration %r' % s)

    if '/' not in pkg:
        try:
            from LbConfiguration import Package
            hat = Package.getPackage(pkg).hat()
            if hat:
                pkg = '{0}/{1}'.format(hat, pkg)
        except:
            # handles transparently the case the package doesn't exists
            pass

    return pkg, vers

def projectExtraPath(projroot):
    '''
    Return any extra search path required by the project at 'projroot'.
    '''
    from LbConfiguration.SP2.options import SearchPath, NightlyPathEntry
    extra_path = SearchPath([])
    # drop the 'InstallArea' part of the path
    while 'InstallArea' in projroot:
        projroot = os.path.dirname(projroot)

    def extractList(filename, varname):
        if os.path.exists(filename):
            data = {}
            exec open(filename).read() in data #IGNORE:W0122
            # Get the list and convert it to strings
            return filter(str, data.get(varname, []))
        else:
            return []

    # check for the Python digested search path
    spFile = os.path.join(projroot, 'searchPath.py')
    if os.path.exists(spFile):
        data = {}
        exec open(spFile).read() in data #IGNORE:W0122
        extra_path = data['path']

    # check for a requested nightly slot
    build_conf = os.path.join(projroot, 'build.conf')
    if os.path.exists(build_conf):
        vals = dict(l.strip().split('=', 1)
                    for l in map(str.strip, open(build_conf))
                    if l and not l.startswith('#'))
        slot = vals.get('nightly_slot')
        day = vals.get('nightly_day')
        base = vals.get('nightly_base') or os.environ.get('LHCBNIGHTLY', '')
        if slot and day and base:
            for p in extra_path:
                if isinstance(p, NightlyPathEntry):
                    p.base, p.slot, p.day = base, slot, day
                    break
            else: # else clause for the 'for' statement
                extra_path.insert(0, NightlyPathEntry(base, slot, day))

    return extra_path

class SP2(EnvConfig.Script):
    __usage__ = "Usage: %prog [OPTION]... [NAME=VALUE]... PROJECT VERSION [COMMAND [ARG]...]"

    def _prepare_parser(self):
        from options import addSearchPath, addPlatform, addListing
        from optparse import OptionValueError

        super(SP2, self)._prepare_parser()
        parser = self.parser

        parser.add_option("--use", action="append",
                          help="add a data package to the environment")

        addPlatform(parser)
        addSearchPath(parser)
        addListing(parser)

        def extract_project_version(opt_str, rargs):
            if not rargs:
                raise OptionValueError("%s must be followed by the project name and optionally by the version" % opt_str)
            p_name = FixProjectCase(rargs.pop(0))
            if rargs and isValidVersion(p_name, rargs[0]):
                v = rargs.pop(0)
            else:
                v = DEFAULT_VERSION
            return p_name, v

        def runtime_project_option(_option, opt_str, _value, parser):
            pv = extract_project_version(opt_str, parser.rargs)
            parser.values.runtime_projects.append(pv)

        parser.add_option("--runtime-project", action="callback",
                          metavar = "PROJECT [VERSION]", type="string",
                          callback=runtime_project_option,
                          nargs = 0,
                          help="Add a project to the runtime environment")

        def overriding_project_option(_option, opt_str, _value, parser):
            pv = extract_project_version(opt_str, parser.rargs)
            parser.values.overriding_projects.append(pv)

        parser.add_option("--overriding-project", action="callback",
                          metavar = "PROJECT [VERSION]", type="string",
                          callback=overriding_project_option,
                          nargs = 0,
                          help="Add a project to override packages")

        parser.add_option("--no-auto-override", action="store_false",
                          dest = "auto_override",
                          help = "Do not automatically prepend the projects %s" % auto_override_projects)

        parser.add_option("--use-grid", action="store_true",
                          help = "Enable auto selection of LHCbGrid project")

        # Note: the profile is not used in the script class, but in the wrapper
        #       it is added to the parser to appear in the help and for checking
        parser.add_option("--profile", action="store_true",
                          help="Print some profile informations about the execution.")

        parser.set_defaults(use = [],
                            runtime_projects = [],
                            overriding_projects = [],
                            auto_override = True,
                            use_grid = False)
        self.allow_empty_version = False

    def _parse_args(self, args=None):
        super(SP2, self)._parse_args(args)
        if len(self.cmd) < 1:
            self.parser.error("missing project name")
        self.project = FixProjectCase(self.cmd.pop(0))
        if self.cmd and isValidVersion(self.project, self.cmd[0]):
            self.version = self.cmd.pop(0)
        else:
            # if no version is specified, we want to allow just the project name
            self.allow_empty_version = True
            self.version = DEFAULT_VERSION

    def _makeEnv(self):
        # FIXME: when we drop Python 2.4, this should become 'from . import path'
        from LbConfiguration.SP2 import path, SearchPathEntry
        # prepend dev dirs to the search path
        if self.opts.dev_dirs:
            path[:] = self.opts.dev_dirs + path

        if self.opts.user_area and not self.opts.no_user_area:
            path.insert(0, SearchPathEntry(self.opts.user_area))

        # FIXME: we need to handle common options like --list in a single place
        if self.opts.list:
            from lookup import listVersions
            for entry in listVersions(self.project, self.opts.platform):
                print '%s in %s' % entry
            sys.exit(0)

        self.version = expandVersionAlias(self.project, self.version,
                                          self.opts.platform)

        # prepare the list of projects to use
        projects = []
        if self.opts.use_grid:
            self.opts.overriding_projects.append(('LHCbGrid', DEFAULT_VERSION))
        if self.opts.auto_override:
            explicit = set([p[0] for p in self.opts.overriding_projects])
            projects.extend([p for p in auto_override_projects
                               if p[0] not in explicit])
        projects.extend(self.opts.overriding_projects)
        projects.append((self.project, self.version))
        projects.extend(self.opts.runtime_projects)

        # Check if the main project needs a special search path
        self.log.debug('check if we need extra search path')
        project_path = findProject(self.project, self.version,
                                   self.opts.platform,
                                   allow_empty_version=self.allow_empty_version)
        extra_path = projectExtraPath(project_path)
        if extra_path:
            self.log.debug('the project requires an extra search path')
            # we add the extra search path between the command line entries and the default
            idx = len(self.opts.dev_dirs)
            if self.opts.user_area:
                idx += 1
                path[:] = path[:idx] + extra_path + path[idx:]
            self.log.debug('final search path: %r', path)

        # set the environment XML search path
        env_path = []
        for p, v in projects:
            v = expandVersionAlias(p, v, self.opts.platform)
            env_path.extend(getEnvXmlPath(p, v, self.opts.platform))
        # FIXME: EnvConfig has got problems with unicode in the search path
        env_path = map(str, env_path) # ensure that we do not have unicode strings
        EnvConfig.path.extend(env_path)

        # set LCG relocation roots
        lcg_relocation = getLCGRelocation(os.path.join(project_path,
                                                       'manifest.xml'))
        self.opts.actions.extend(('set', (k, v))
                                 for k, v in lcg_relocation.items())

        # extend the prompt variable (bash, sh)
        if self.cmd and os.path.basename(self.cmd[0]) in ('bash', 'sh'):
            prompt = os.environ.get('PS1', r'\W \$ ')
            self.opts.actions.append(('set',
                                      ('PS1',
                                       r'[{0} {1}] {2}'.format(self.project,
                                                               self.version,
                                                               prompt))))

        # instruct the script to load the projects environment XML
        for p, _ in projects[::-1]:
            self.opts.actions.append(('loadXML', (p + '.xenv',)))

        # handle the extra data packages
        for pkg_name, pkg_vers in map(decodePkg, self.opts.use):
            xml_name = pkg_name.replace('/', '_') + '.xenv'
            xml_path = os.path.join(findDataPackage(pkg_name, pkg_vers),
                                    xml_name)
            if not os.path.exists(xml_path):
                # fall back on the old conventional name
                xml_path = xml_path[:-5] + 'Environment.xml'
            # FIXME: EnvConfig has got problems with unicode filenames
            self.opts.actions.append(('loadXML', (str(xml_path),)))

        super(SP2, self)._makeEnv()

    def compatMain(self, reason=None):
        '''
        Fall-back function to call the old SetupProject code.
        '''
        from compatibility import getOldEnvironment
        if reason:
            logging.warning('trying old SetupProject (%s)', reason)
        # fix command line to fit old SetupProject
        if self.cmd:
            args = sys.argv[1:-len(self.cmd)]
        else:
            args = sys.argv[1:]

        self.env = getOldEnvironment(args)

        if self.cmd:
            self.runCmd()
        else:
            self.dump()

    def main(self):
        from lookup import NotFoundError
        try:
            super(SP2, self).main()
        except (NotFoundError, IOError, OSError), x:
            self.compatMain(x)
