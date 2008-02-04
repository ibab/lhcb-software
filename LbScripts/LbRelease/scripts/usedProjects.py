#------------------------------------------------------------------------------
""" Script to check projects used by other projects"""
#------------------------------------------------------------------------------

import sys, os, string, glob, getopt

usedProjects = {}
allProjects  = []
printAll     = False

#--------------------------------------------------------------------------------
# check the arguments
#------------------------------------------------------------------------------
def argChecker():

    global printAll
    arguments = sys.argv[1:]
    if len(sys.argv) == 1:
        return
    
    try:
        keys, values = getopt.getopt(arguments,'v',
                                     ['verbose'])

    except getopt.GetoptError:
        print 'invalid argument'
        sys.exit()

    for key,value in keys:
        if key in ('-v', '--verbose'):
            printAll = True

#------------------------------------------------------------------------------
# check the environment
#------------------------------------------------------------------------------
def envChecker():
    if not os.environ.has_key( 'CMTPROJECTPATH' ):
        print 'CMTPROJECTPATH is not set'
        sys.exit(0)

        if os.environ.has_key( 'CMTPATH' ):
            del os.environ['CMTPATH']

#------------------------------------------------------------------------------
# find all projects in the given release area and dependencies from project.cmt
#------------------------------------------------------------------------------
def findAllProjects( releaseArea ):
    here = os.getcwd()
    os.chdir( os.environ.get( releaseArea ) )
    print 'Processing all projects in ', os.getcwd()

    for file in glob.glob( '*'+os.sep+'*'+os.sep+'cmt'+os.sep+'project.cmt' ):
        cmtdir =  os.environ.get(releaseArea) + os.sep + file.split('project.cmt')[0]
        os.chdir( cmtdir )
        project = file.split(os.sep)[1]
        if printAll: print 'Processing ', project 
        if project not in allProjects: allProjects.append(project)
        for line in os.popen('cmt show projects').readlines():
            if line.find('(current)') == -1 and line.split()[1].find('_') != -1:
                uses = line.split()[1]
                if printAll: print '-->  uses ', uses 
                if uses in usedProjects:
                    usedBy = usedProjects[uses]
                    if project not in usedBy:
                        usedBy.append( project )
                        usedProjects[uses] = usedBy
                else:
                    usedProjects[uses] = [ project ]
    os.chdir( here )

#------------------------------------------------------------------------------
# find all projects in the given release area and dependencies from xxEnv
#------------------------------------------------------------------------------
def processOldProjects( releaseArea ):
    here = os.getcwd()
    os.chdir( os.environ.get(releaseArea) )
    print 'Processing xxEnv projects in ', os.getcwd()

    for xxEnv in glob.glob( '*Env'+os.sep+'v*'+os.sep+'cmt' ):
        cmtdir = os.environ.get(releaseArea) + os.sep + xxEnv
        os.chdir( cmtdir )
        project = xxEnv.split('Env')[0].upper() + '_' + xxEnv.split(os.sep)[1]
        if printAll: print 'Processing ', project 
        if project not in allProjects: allProjects.append(project)
        for line in os.popen('cmt show uses').readlines():
            if line.find('#') != 0 and line.find('CMT') == -1:
                yyEnv = line.split('use ')[1].split(' (')[0]
                uses = yyEnv.split()[0].split('Env')[0].upper() + '_' + yyEnv.split()[1]
                if printAll: print '-->  uses ', uses 
                if uses in usedProjects:
                    usedBy = usedProjects[uses]
                    if project not in usedBy:
                        usedBy.append( project )
                        usedProjects[uses] = usedBy
                else:
                    usedProjects[uses] = [ project ]
    os.chdir( here )

#------------------------------------------------------------------------------
# print the results
#------------------------------------------------------------------------------
def printProjects():
    print '================================== Results =========================='
    unusedProjectVersions = {}
    unusedProjects = []
    for project in allProjects:
        if project in usedProjects:
            print project, ' used by: ', usedProjects[project]
        else:
            pName = project.split('_')[0]
            pVer  = project.split('_')[1]
            if pName not in unusedProjects: unusedProjects.append(pName)
            if pName not in unusedProjectVersions:
                unusedProjectVersions[pName] = [pVer]
            else:
                unusedVers = unusedProjectVersions[pName]
                unusedVers.append( pVer )
                unusedProjectVersions[pName] = unusedVers

    print '====================================================================='
    print "Projects not used by other projects:"
    print '====================================================================='
    unusedProjects.sort()
    for pName in unusedProjects:
        print pName, ' versions: ', unusedProjectVersions[pName]

#------------------------------------------------------------------------------
# set up the job, defining the release areas to process
#------------------------------------------------------------------------------
argChecker()
envChecker()
findAllProjects( 'Gaudi_release_area' )
findAllProjects( 'LHCb_release_area' )
processOldProjects( 'Gaudi_release_area' )
processOldProjects( 'LHCb_release_area' )
if os.environ.has_key('Vetra_release_area'): processOldProjects( 'Vetra_release_area' )

allProjects.sort()
printProjects()

