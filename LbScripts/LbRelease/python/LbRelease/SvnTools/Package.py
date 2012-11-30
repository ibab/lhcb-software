"""
Module specific for interacting with Packages in the SVN repository
"""

__all__=["getRequirements","getNotes","diffNotes","checkOrDiff","diffContainer",
         "getCMakeLists"]

from LbConfiguration.Repository import getRepositories
from LbRelease import rcs
from LbUtils import LockOnPrintThread
from LbUtils.CMT.Parse import parseReq
import thread, Queue
from LbUtils.Processes import callCommand

url = str(getRepositories(protocol='anonymous')["lbsvn"])
lbsvn = rcs.connect(url)


def getRequirements(path):
    return callCommand('svn','cat',path+'/cmt/requirements')[0]
    #return commands.getoutput('svn cat '+path+'/cmt/requirements')

def getCMakeLists(path):
    return callCommand('svn','cat',path+'/CMakeLists.txt')[0]
    #return commands.getoutput('svn cat '+path+'/cmt/requirements')

def getNotes(path):
    return callCommand('svn','cat',path+'/doc/release.notes')[0]
    #return commands.getoutput('svn cat '+path+'/doc/release.notes')

def diffNotes(note1,note2):
    note1=[line.rstrip() for line in note1.strip().split('\n')]
    note2=[line.rstrip() for line in note2.strip().split('\n')]
    note1b=[]
    note2b=[]
    for line in note1:
        for subline in line.split('\r'):
            if len(subline.rstrip()):
                note1b.append(subline.rstrip())
    for line in note2:
        for subline in line.split('\r'):
            if len(subline.rstrip()):
                note2b.append(subline.rstrip())
    diff1='\n'.join(['<\t'+line for line in note1b if (len(line.strip()) and "!=====" not in line and line not in note2b and "/usr/bin/xauth:  error in locking authority file" not in line)])
    if len(diff1.strip()):
        diff1=diff1+'\n-\t------\n'
    diff2='\n'.join(['>\t'+line for line in note2b if (len(line.strip()) and "!=====" not in line and line not in note1b and "/usr/bin/xauth:  error in locking authority file" not in line)])
    if len(diff2.strip()):
        diff2=diff2+'\n-\t------\n'
    diffs=(diff2+diff1).strip()
    #while '\n\n' in diffs:
    #    diffs=diffs.replace('\n\n','\n')
    if len(diffs):
        diffs=diffs+'\n'
    return diffs

def checkOrDiff(package, versions=[], dodiff=True):
    '''Check if the package exists and check also the svn diffs
    returns a string for printing, terminated with a newline character'''
    if not lbsvn.hasVersion(package, versions[0]):
        return (
            "====================\n" +
            'Error: '+package+' '+versions[0]+' does not exist\n' +
            '====================\n'
            )
    if len(versions)==1:
        return '=== '+package+' '+versions[0]+' OK ===\n'

    if len(versions)>2:
        raise KeyError, "You sent more than two versions to diff, please send only one"
    if not lbsvn.hasVersion(package, versions[1]):
        return (
            "====================\n" +
            'Error: '+package+' '+versions[1]+' does not exist\n' +
            '====================\n'
            )
    if not dodiff:
        return '=== '+package+' '+versions[0]+' OK ===\n'

    path1=lbsvn.url(package,versions[0])
    path2=lbsvn.url(package,versions[1])
    diffs=callCommand("svn","diff","--summarize",path1,path2)[0].strip()
    #diffs=commands.getoutput("svn diff --summarize "+path1+' '+path2).strip()
    diffs='\n'.join([line.strip() for line in diffs.split('\n') if "/usr/bin/xauth:  error in locking authority file" not in line])
    #replace path with package to avoid annoying svn diff format
    while path1 in diffs:
        if '/'==path1[-1]:
                diffs=diffs.replace(path1,package+'/')
        else:
            diffs=diffs.replace(path1,package)
    #replace path with package to avoid annoying svn diff format
    while path2 in diffs:
        if '/'==path2[-1]:
                diffs=diffs.replace(path2,package+'/')
        else:
            diffs=diffs.replace(path2,package)

    if len(diffs):
        result=('=== '+package+' '+versions[0]+
                '->'+versions[1] +' ===\n')
        result=result+diffNotes(getNotes(path1),getNotes(path2))+diffs+'\n'
        return result

    #if there are no diffs, state that the two are equal
    return ('=== '+package+' '+versions[0]+
            '='+versions[1] +' ===\n')


def diffContainer(package,versions):
    """
    Take package:versions from this container package
    Use SVN to get the differences between them
    """
    #prepare first package
    if not lbsvn.hasVersion(package, versions[0]):
        result=(
            "====================\n" +
            'Error: '+package+' '+versions[0]+' does not exist\n' +
            '====================\n'
            )
        print result.strip()
        return

    if versions[1] is not None:
        checkRes=checkOrDiff(package,versions)
        print checkRes.strip()
    else:
        checkRes=checkOrDiff(package,[versions[0],"trunk"])
        print checkRes.strip()

    path1=lbsvn.url(package,versions[0])
    requirements1=getRequirements(path1)

    #parse requirements file
    package_versions1=parseReq(requirements1)
    for pack in package_versions1:
        if "*" in package_versions1[pack]:
            package_versions1[pack]="trunk"

    #diff_package_versions is the dictionary {'package' : ['v1', 'v2']} for diffing
    diff_package_versions={}


    #if a second version was given, compare with a different sys container, now parse that container
    if versions[1] is not None:
        #prepare second package
        if not lbsvn.hasVersion(package, versions[1]):
            result=(
                "====================\n" +
                'Error: '+package+' '+versions[1]+' does not exist\n' +
                '====================\n'
                )
            print result.strip()
            return
        path2=lbsvn.url(package,versions[1])
        requirements2=getRequirements(path2)

        #parse requirements file
        package_versions2=parseReq(requirements2)
        for pack in package_versions2:
            if "*" in package_versions2[pack]:
                package_versions2[pack]="trunk"

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
                diff_package_versions[package]=[package_versions1[package],package_versions2[package]]
    #if a second version was given, compare with a different sys container
    else:
        for package in package_versions1:
            diff_package_versions[package]=[package_versions1[package],'trunk']

    #create list of packages as a queue
    packagePool=Queue.Queue()

    for package in diff_package_versions:
        packagePool.put(package)

    def method(self, item):
        return checkOrDiff( item, self.package_versions[item])

    #loop over threads, see the class for more details
    lock=thread.allocate_lock()
    thethreads=[]
    for _i in range(20):
        t = LockOnPrintThread(packagePool, lock)
        t.package_versions=diff_package_versions
        t.method=method
        thethreads.append(t)
        #t = checkExistanceAndDiffs( packagePool,lock,diff_package_versions)
        t.start()

    for t in thethreads:
        while not t.done:
            pass
