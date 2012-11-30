"""
Module specific for interacting with Projects in the SVN repository
"""
__all__=["getProjectCmt","translateProject","isProject","containerFromProject","checkProject","diffProject"]

from LbConfiguration.Repository import getRepositories
from LbRelease import rcs
from LbUtils.CMT.Parse import parseReq
from LbUtils.Processes import callCommand

url = str(getRepositories(protocol='anonymous')["lbsvn"])
lbsvn = rcs.connect(url)

def getProjectCmt(path):
    return callCommand('svn','cat',path+'/cmt/project.cmt')[0]
    #return commands.getoutput('svn cat '+path+'/project.cmt')

from Package import getCMakeLists

def translateProject(project,version):
    """
    take project:version strings
    convert to the correct strings for the SVN repository tools
    returns a tuple, project,version

    Used to deal with silly capitalization in our software
    """
    project=project.lower()
    if project=="davinci":
        project="DaVinci"
    elif project=="lhcb":
        project="LHCb"
    elif project=="lbscripts":
        # This is a temporary fix, we need to integrate with the list of projects from LbConfiguration
        project="LbScripts"
    else:
        project=project[0].upper()+project[1:]
    if '_' in version and project.upper() == version.split('_')[0].upper():
        version=version.split('_')[-1]
    return (project,version)


def isProject(package, version):
    """
    Is the package actually a project?
    """
    exists=False
    try:
        exists=lbsvn.hasVersion(package, version)
    except:
        pass
    if exists:
        return False
    try:
        package,version=translateProject(package,version)
        exists=lbsvn.hasVersion(package, version, isProject=True)
    except:
        pass
    if exists:
        return True
    return False

def containerFromProject(project, version):
    """
    given a project parse the project.cmt and return the
    container package
    """
    project,version=translateProject(project,version)
    path=lbsvn.url(project,version,isProject=True)
    #print path1
    reqs=getProjectCmt(path)
    #print reqs
    container=parseReq(reqs,token='container')
    if len(container)==0:
        return project+"Sys"
    return container.keys()[0]


def checkProject(project, version):
    """
    Check the existance and structure of a project
    """
    project,version=translateProject(project,version)
    #prepare first package
    if not lbsvn.hasVersion(project, version, isProject=True):
        return (
            "====================\n" +
            'Error: '+project+' '+version+' does not exist\n' +
            '====================\n'
            )
    tagpath=lbsvn.url(project,version, isProject=True)
    projcmt=getProjectCmt(tagpath).strip()
    #print projcmt
    if len(projcmt)==0 or "File not found" in projcmt:
        return (
            "====================\n" +
            'Error: '+project+' '+version+' does not have a project.cmt file\n' +
            '====================\n'
            )
    cmakelists=getCMakeLists(tagpath).strip()
    #print projcmt
    if len(cmakelists)==0 or "File not found" in cmakelists:
        return (
            "====================\n" +
            'Warning: '+project+' '+version+' does not have a CMakeLists.txt file\n' +
            '====================\n'
            )
    return '=== '+project+' '+version+' OK ==='


def diffProject(project, versions):
    #prepare first package
    project,v1=translateProject(project,versions[0])
    if not lbsvn.hasVersion(project, v1, isProject=True):
        result=(
            "====================\n" +
            'Error: '+project+' '+v1+' does not exist\n' +
            '====================\n'
            )
        print result.strip()
        return

    path1=lbsvn.url(project,v1, isProject=True)
    requirements1=getProjectCmt(path1)

    #parse requirements file
    package_versions_tmp=parseReq(requirements1)
    package_versions1={}
    for pack in package_versions_tmp:
        if "*" in package_versions_tmp[pack]:
            package_versions_tmp[pack]="trunk"
        proj,ver=translateProject(pack,package_versions_tmp[pack])
        package_versions1[proj]=ver
    #diff_package_versions is the dictionary {'package' : ['v1', 'v2']} for diffing

    if versions[1] is None:
        versions=[versions[0],'trunk']

    #if a second version was given, compare with a different sys container, now parse that container
    if versions[1] is not None:
        project,v2=translateProject(project,versions[1])
        #prepare second package
        if not lbsvn.hasVersion(project, v2,isProject=True):
            result=(
                "====================\n" +
                'Error: '+project+' '+v2+' does not exist\n' +
                '====================\n'
                )
            print result.strip()
            return
        path2=lbsvn.url(project,v2,isProject=True)
        requirements2=getProjectCmt(path2)

        #parse requirements file
        package_versions_tmp=parseReq(requirements2)
        package_versions2={}
        for pack in package_versions_tmp:
            if "*" in package_versions_tmp[pack]:
                package_versions_tmp[pack]="trunk"
            proj,ver=translateProject(pack,package_versions_tmp[pack])
            package_versions2[proj]=ver

        #separate out packages for diffing
        removed_packages=[package for package in package_versions1
                           if package not in package_versions2]

        added_packages=[package for package in package_versions2
                        if package not in package_versions1]

        shared_packages=[package for package in package_versions2
                         if package in package_versions1]

        #print information about packages where no diff is required
        for package in removed_packages:
            print '=== '+package+' '+package_versions1[package]+' REMOVED ==='

        for package in added_packages:
            print '=== '+package+' '+package_versions2[package]+' ADDED   ==='

        #separate packages which are identical to those which must be diffed
        for package in shared_packages:
            if package_versions1[package]==package_versions2[package]:
                print '=== '+package+' '+package_versions1[package]+' SHARED  ==='
            else:
                print '=== '+package+' '+package_versions1[package]+'->'+ package_versions2[package]+'  ==='
    #if a second version was given, compare with a different sys container
