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
import logging
import stat
import sys

import LbConfiguration.SP2

from string import Template

from LbConfiguration.SP2.version import DEFAULT_VERSION

from LbConfiguration.SetupProject import FixProjectCase

def main():
    '''
    Script to generate a local development project.
    '''
    from optparse import OptionParser
    from options import addSearchPath, addOutputLevel, addPlatform, addListing
    from lookup import findProject, MissingProjectError
    from subprocess import call

    parser = OptionParser(usage='%prog [options] Project [version]')

    addSearchPath(parser)
    addOutputLevel(parser)
    addPlatform(parser)
    addListing(parser)

    parser.add_option('--name', action='store',
                      help='Name of the local project [default: "<proj>Dev_<vers>"].')

    parser.add_option('--dest-dir', action='store',
                      help='Where to create the local project '
                           '[default: %default].')

    parser.add_option('--git', action='store_true',
                      help='Initialize git repository in the generated directory'
                           '[default, if git is available].')

    parser.add_option('--no-git', action='store_false',
                      dest='git',
                      help='Do not initialize the git local repository.')

    # Note: the profile is not used in the script class, but in the wrapper
    #       it is added to the parser to appear in the help and for checking
    parser.add_option('--profile', action='store_true',
                      help='Print some profile informations about the execution.')

    try:
        from LbUtils import which
        has_git = bool(which('git'))
    except ImportError:
        has_git = True

    parser.set_defaults(dest_dir=os.curdir,
                        git=has_git)

    opts, args = parser.parse_args()

    logging.basicConfig(level=opts.log_level)

    if len(args) == 1:
        args.append(DEFAULT_VERSION)

    try:
        project, version = args
    except ValueError:
        parser.error('wrong number of arguments')

    project = FixProjectCase(project)

    if opts.user_area and not opts.no_user_area:
        from LbConfiguration.SP2 import EnvSearchPathEntry, SearchPathEntry
        if os.environ['User_release_area'] == opts.user_area:
            opts.dev_dirs.insert(0, EnvSearchPathEntry('User_release_area'))
        else:
            opts.dev_dirs.insert(0, SearchPathEntry(opts.user_area))

    # FIXME: we need to handle common options like --list in a single place
    if opts.list:
        from lookup import listVersions
        for entry in listVersions(project, opts.platform):
            print '%s in %s' % entry
        sys.exit(0)

    if not opts.name:
        opts.name = '{project}Dev_{version}'.format(project=project, version=version)
        local_project, local_version = project + 'Dev', version
    else:
        local_project, local_version = opts.name, 'HEAD'

    devProjectDir = os.path.join(opts.dest_dir, opts.name)

    if os.path.exists(devProjectDir):
        parser.error('directory "%s" already exist' % devProjectDir)

    # ensure that the project we want to use can be found

    # prepend dev dirs to the search path
    if opts.dev_dirs:
        LbConfiguration.SP2.path[:] = opts.dev_dirs + LbConfiguration.SP2.path

    try:
        projectDir = findProject(project, version, opts.platform)
        logging.info('using %s %s from %s', project, version, projectDir)
    except MissingProjectError, x:
        parser.error(str(x))

    # Check if it is a CMake-enabled project
    use_cmake = os.path.exists(os.path.join(projectDir, project + 'Config.cmake'))
    if not use_cmake:
        logging.warning('%s %s does not seem a CMake project', project, version)

    # Check if it is a CMT-enabled projects
    use_cmt = os.path.exists(os.path.join(projectDir, os.pardir, os.pardir,
                                          'cmt', 'project.cmt'))

    # Create the dev project
    if not os.path.exists(opts.dest_dir):
        logging.debug('creating destination directory "%s"', opts.dest_dir)
        os.makedirs(opts.dest_dir)

    logging.debug('creating directory "%s"', devProjectDir)
    if opts.git:
        call(['git', 'init', '--quiet', devProjectDir])
    else:
        os.makedirs(devProjectDir)

    data = dict(project=project,
                version=version,
                search_path=' '.join(['"%s"' % p for p in LbConfiguration.SP2.path]),
                search_path_repr=repr(LbConfiguration.SP2.path),
                search_path_env=os.pathsep.join(LbConfiguration.SP2.path),
                # we use cmake if available
                build_tool=('cmake' if use_cmake else 'cmt'),
                PROJECT=project.upper(),
                local_project=local_project,
                local_version=local_version,
                cmt_project=opts.name)

    # FIXME: improve generation of searchPath files, so that they match the command line
    templateDir = os.path.join(os.path.dirname(__file__), 'templates')
    templates = ['CMakeLists.txt', 'toolchain.cmake', 'Makefile',
                 'searchPath.py',
                 'build.conf', 'run']
    # generated files that need exec permissions
    execTemplates = set(['run'])

    if opts.nightly:
        data['slot'], data['day'], data['base'] = opts.nightly
    else:
        data['slot'] = data['day'] = data['base'] = ''

    # for backward compatibility, we create the CMT configuration and env helpers
    if use_cmt:
        templates += ['cmt/project.cmt']
        os.makedirs(os.path.join(devProjectDir, 'cmt'))

    for templateName in templates:
        t = Template(open(os.path.join(templateDir, templateName)).read())
        logging.debug('creating "%s"', templateName)
        dest = os.path.join(devProjectDir, templateName)
        with open(dest, 'w') as f:
            f.write(t.substitute(data))
        if templateName in execTemplates:
            mode = (stat.S_IMODE(os.stat(dest).st_mode) |
                    stat.S_IXUSR | stat.S_IXGRP)
            os.chmod(dest, mode)

    # generate searchPath.cmake
    if opts.dev_dirs and use_cmake:
        logging.debug('creating "%s"', 'searchPath.cmake')
        dest = os.path.join(devProjectDir, 'searchPath.cmake')
        with open(dest, 'w') as f:
            f.write('# Search path defined from lb-dev command line\n')
            f.write(opts.dev_dirs.toCMake())

    if opts.dev_dirs and use_cmt:
        for shell in ('sh', 'csh'):
            build_env_name = 'build_env.' + shell
            logging.debug('creating "%s"', build_env_name)
            dest = os.path.join(devProjectDir, build_env_name)
            with open(dest, 'w') as f:
                f.write('# Search path defined from lb-dev command line\n')
                f.write(opts.dev_dirs.toCMT(shell))

    # When the project name is not the same as the local project name, we need a
    # fake *Sys package for SetupProject (CMT only).
    if use_cmt and project != local_project:
        t = Template(open(os.path.join(templateDir, 'cmt/requirements_')).read())
        templateName = os.path.join(local_project + 'Sys', 'cmt/requirements')
        os.makedirs(os.path.dirname(os.path.join(devProjectDir, templateName)))
        logging.debug('creating "%s"', templateName)
        open(os.path.join(devProjectDir, templateName), 'w').write(t.substitute(data))
        if use_cmake: # add a CMakeLists.txt to it
            with open(os.path.join(devProjectDir, local_project + 'Sys',
                                   'CMakeLists.txt'), 'w') as cml:
                cml.write('gaudi_subdir({0} {1})\n'
                          .format(local_project + 'Sys', local_version))

    if opts.git:
        call(['git', 'add', '.'], cwd=devProjectDir)
        call(['git', 'commit', '--quiet', '-m',
              'initial version of satellite project\n\n'
              'generated with:\n\n'
              '    %s\n' % ' '.join(sys.argv)], cwd=devProjectDir)

    # Success report
    msg = '''
Successfully created the local project {0} in {1}

To start working:

  > cd {2}
  > getpack MyPackage vXrY

then

  > make
  > make test

and optionally (CMake only)

  > make install

You can customize the configuration by editing the files 'build.conf' and
'CMakeLists.txt' (see http://cern.ch/gaudi/CMake for details).
'''

    print msg.format(opts.name, opts.dest_dir, devProjectDir)
