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

import LbConfiguration.SP2

from string import Template

from LbConfiguration.SetupProject import FixProjectCase

def main():
    '''
    Script to generate a local development project.
    '''
    from optparse import OptionParser
    from options import addSearchPath, addOutputLevel, addPlatform, addListing
    from lookup import findProject, MissingProjectError

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

    # Note: the profile is not used in the script class, but in the wrapper
    #       it is added to the parser to appear in the help and for checking
    parser.add_option('--profile', action='store_true',
                      help='Print some profile informations about the execution.')

    parser.set_defaults(dest_dir=os.curdir)

    opts, args = parser.parse_args()

    logging.basicConfig(level=opts.log_level)

    if len(args) == 1:
        args.append('prod')

    try:
        project, version = args
    except ValueError:
        parser.error('wrong number of arguments')

    project = FixProjectCase(project)

    if opts.user_area and not opts.no_user_area:
        LbConfiguration.SP2.path.insert(0, opts.user_area)

    # FIXME: we need to handle common options like --list in a single place
    if opts.list:
        from lookup import listVersions
        for entry in listVersions(project, opts.platform):
            print '%s in %s' % entry
        import sys
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
        LbConfiguration.SP2.path[:] = map(str, opts.dev_dirs) + LbConfiguration.SP2.path

    try:
        projectDir = findProject(project, version, opts.platform)
        logging.info('using %s %s from %s', project, version, projectDir)
    except MissingProjectError, x:
        parser.error(str(x))

    use_cmake = os.path.exists(os.path.join(projectDir, project + 'Config.cmake'))
    if not use_cmake:
        logging.warning('%s %s does not seem a CMake project', project, version)

    # Create the dev project
    if not os.path.exists(opts.dest_dir):
        logging.debug('creating destination directory "%s"', opts.dest_dir)
        os.makedirs(opts.dest_dir)

    logging.debug('creating directory "%s"', devProjectDir)
    os.makedirs(devProjectDir)

    data = dict(project=project,
                version=version,
                search_path=' '.join(['"%s"' % p for p in LbConfiguration.SP2.path]),
                search_path_repr=repr(LbConfiguration.SP2.path),
                search_path_env=os.pathsep.join(LbConfiguration.SP2.path),
                use_cmake=(use_cmake and 'yes' or ''),
                PROJECT=project.upper(),
                local_project=local_project,
                local_version=local_version,
                cmt_project=opts.name)

    # FIXME: improve generation of searchPath files, so that they match the command line
    templateDir = os.path.join(os.path.dirname(__file__), 'templates')
    templates = ['CMakeLists.txt', 'toolchain.cmake', 'Makefile',
                 'searchPath.py', 'searchPath.cmake',
                 'build_env.sh', 'build_env.csh', 'run']
    # generated files that need exec permissions
    execTemplates = set(['run'])

    if opts.nightly:
        data['slot'], data['day'] = opts.nightly
        templates.append('nightly.cmake')
    else:
        data['slot'] = data['day'] = ''

    # for backward compatibility, we create the CMT configuration
    if not use_cmake:
        templates.append('cmt/project.cmt')
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


    # When the project name is not the same as the local project name, we need a
    # fake *Sys package for SetupProject (CMT only).
    if not use_cmake and project != local_project:
        t = Template(open(os.path.join(templateDir, 'cmt/requirements_')).read())
        templateName = os.path.join(local_project + 'Sys', 'cmt/requirements')
        os.makedirs(os.path.dirname(os.path.join(devProjectDir, templateName)))
        logging.debug('creating "%s"', templateName)
        open(os.path.join(devProjectDir, templateName), 'w').write(t.substitute(data))

    # Success report
    finalMessage = '''
Successfully created the local project {0} in {1}

To start working:

  > cd {2}
  > getpack MyPackage vXrY

then
'''

    finalMessageCMake = finalMessage + '''
  > make
  > make test
  > make QMTestSummary
  > make install

You can customize the configuration by editing the file 'CMakeLists.txt'
(see http://cern.ch/gaudi/CMake for details).
'''

    finalMessageCMT = finalMessage + '''
  > . build_env.sh
  > make
  > cd MyPackage/cmt
  > cmt TestPackage
'''
    if 'csh' in os.environ.get('SHELL', 'sh'):
        finalMessageCMT = finalMessageCMT.replace('. build_env.sh',
                                                  'source build_env.csh')

    msg = use_cmake and finalMessageCMake or finalMessageCMT
    print msg.format(opts.name, opts.dest_dir, devProjectDir)
