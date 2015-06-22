import External
import Platform
import Project
import shutil
import os

def createProjectMakefile(dest, overwrite=False):
    '''Write the generic Makefile for CMT projects.
    @param dest: the name of the destination file
    @param overwrite: flag to decide if an already present file has to be kept or not (default is False)
    '''
    import logging
    if overwrite or not os.path.exists(dest):
        logging.debug("Creating '%s'", dest)
        f = open(dest, "w")
        f.write("include ${LBCONFIGURATIONROOT}/data/Makefile\n")
        f.close()

def createToolchainFile(dest, overwrite=False):
    '''Write the generic toolchain.cmake file needed by CMake-based projects.
    @param dest: destination filename
    @param overwrite: flag to decide if an already present file has to be kept or not (default is False)
    '''
    import logging
    if overwrite or not os.path.exists(dest):
        logging.debug("Creating '%s'", dest)
        f = open(dest, "w")
        f.write("include($ENV{LBUTILSROOT}/data/toolchain.cmake)\n")
        f.close()

def createDataPackageCMakeLists(pkg, dest, overwrite=False):
    '''Create the generic CMakeLists.txt file for data packages.
    @param pkg: data package name
    @param dest: destination filename
    @param overwrite: flag to decide if an already present file has to be kept or not (default is False)
    '''
    import logging
    from string import Template
    if overwrite or not os.path.exists(dest):
        logging.debug("Creating '%s'", dest)
        tpl = Template(open(os.path.join(os.environ['LBCONFIGURATIONROOT'],
                                         'data', 'DataPkgCMakeLists.tpl')).read())
        f = open(dest, "w")
        f.write(tpl.substitute(package_name=pkg))
        f.close()

def createEclipseConfiguration(dest, projectpath):
    """Create the configuration files for an Eclipse project in the directory
    'dest', setting CMTPROJECTPATH to projectpath.
    """
    import time, sys, logging
    from os import environ
    from os.path import join, exists, basename
    # data to inject in the templates
    project = basename(dest)
    data = {"ProjectName": project,
            "PythonMajor": sys.version_info[0],
            "PythonMinor": sys.version_info[1],
            "date": time.strftime("%a %b %d %H:%M:%S %Z %Y"),
            "CMTPROJECTPATH": projectpath,
            }
    # create the templates
    try:
        logging.debug("Creating Eclipse project configuration in '%s'", dest)
        if not exists(join(dest, ".settings")):
            os.mkdir(join(dest, ".settings"))
        for f, t in [(".project", "cdt_project_template.xml"),
                     (".cproject", "cdt_cproject_template.xml"),
                     (".pydevproject", "pydev_pydevproject_template.xml"),
                     (join(".settings", "org.eclipse.cdt.core.prefs"), "cdt_prefs_template.txt"),
                     ]:
            if not exists(join(dest, f)) \
               and exists(join(environ["LBCONFIGURATIONROOT"], "data", "eclipse", t)):
                t = open(join(environ["LBCONFIGURATIONROOT"], "data", "eclipse", t)).read()
                open(join(dest, f), "w").write(t % data)
    except:
        # Ignore failures
        pass

def eclipseConfigurationAddPackage(dest, package):
    """Add package-specific configuration details to an already existing Eclipse
    (CDT) project.
    """
    import logging
    from os.path import join, exists
    from xml.etree.ElementTree import parse, SubElement, tostring
    def addTarget(element, name, path, cmd = None, args = None, target = None, stopOnError = True, allBuilders = True):
        """Add a target configuration to a buildTargets element."""
        t = SubElement(element, "target", {"name": name,
                                           "path": path,
                                           "targetID": "org.eclipse.cdt.build.MakeTargetBuilder"})
        useDefaultCommand = not cmd
        if target is None: target = name
        SubElement(t, "buildCommand").text = cmd
        SubElement(t, "buildArguments").text = args
        SubElement(t, "buildTarget").text = target
        SubElement(t, "stopOnError").text = str(stopOnError).lower()
        SubElement(t, "useDefaultCommand").text = str(useDefaultCommand).lower()
        SubElement(t, "runAllBuilders").text = str(allBuilders).lower()

    try:
        logging.debug("Add package-specific configuration for %s to Eclipse project in %s", package, dest)
        conf = parse(join(dest, ".cproject"))
        # loop over all the configurations builders
        for cc in conf.findall("storageModule/cconfiguration"):
            # loop over the build targets configurations
            for btc in [ e
                         for e in cc.findall("storageModule")
                         if e.attrib.get("moduleId") == "org.eclipse.cdt.make.core.buildtargets" ]:
                # buildTargets (create if it doesn't exist)
                bts = btc.find("buildTargets")
                if not bts:
                    bts = SubElement(btc, "buildTargets")
                # add the TestPacakge target
                addTarget(bts,
                          name = "TestPackage", path = join(package, "cmt"),
                          cmd = "cmt", allBuilders = False)
        out = open(join(dest, ".cproject"), "wb")
        out.write('<?xml version="1.0" encoding="UTF-8" standalone="no"?>\n<?fileVersion 4.0.0?>\n\n')
        out.write(tostring(conf.getroot(), "utf-8"))
        out.write('\n')
        out.close()
    except:
        # Ignore failures
        pass

def initProject(path, overwrite=False):
    '''
    Initialize the sources for an LHCb project for building.

    Create the (generic) special files required for building LHCb/Gaudi
    projects.

    @param path: path to the root directory of the project
    @param overwrite: whether existing files should be overwritten, set it to
                      True to overwrite all of them or to a list of filenames

    '''
    factories = [('Makefile', createProjectMakefile),
                 ('toolchain.cmake', createToolchainFile)]

    # handle the possible values of overwrite to always have a set of names
    if overwrite in (False, None):
        overwrite = set()
    elif overwrite is True:
        overwrite = set(f[0] for f in factories)
    elif isinstance(overwrite, basestring):
        overwrite = set([overwrite])
    else:
        overwrite = set(overwrite)

    for filename, factory in factories:
        factory(os.path.join(path, filename), overwrite=filename in overwrite)

