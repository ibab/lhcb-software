import os, sys, shutil, re, time, pickle
from LbUtils.Lock import Lock
from threading import Thread
from setup import *

sys.path.append(os.path.abspath(os.environ['LCG_NIGHTLIES_SCRIPTDIR']))
import configuration


def copyLocal(sourceDir, targetDir):
    """ tar-pipe copying with locking from <sourceDir> to <targetDir>. Target directory is created if necessary.

        Locking is implemented on the level of the most deep sub-directory. Empty directories are not copied.
        Works only on Linux, both <sourceDir> and <targetDir> must be accessible locally.
    """
    if not os.path.exists(targetDir): os.makedirs(targetDir)
    os.chdir(targetDir) # .lock_* files are stored in CWD and they must by on shared file system
    sourceDir = os.path.abspath(sourceDir)
    targetDir = os.path.abspath(targetDir)
    dirList = [x for x in os.walk(sourceDir, topdown=False)]
    for root, dires, files in dirList:
        if files:
            path = (root+os.sep).replace(sourceDir, '')
            filesString = ' '.join(files)
            Lock('mkdir -p '+ targetDir + path +' && cd '+ sourceDir + path +' && tar cf - '+filesString+' | (cd '+targetDir+path+'. ; tar xf -) && cd -',str(os.getpid()),path)

def copySsh(sourceHost, sourceDir, targetDir):
    """ tar-pipe copying by SSH with locking from <sourceHost>:<sourceDir> to <targetDir>. Target directory is created if necessary. Procedure must be used on the target host. <sourceHost> must be accessible by SSH without password.

        Locking is implemented on the level of the most deep sub-directory. Empty directories are not copied.
        Works only on Linux. <targetDir> must be accessible locally, <sourceDir> must be accessible on <sourceHost>.
    """
    if not os.path.exists(targetDir): os.makedirs(targetDir)
    os.chdir(targetDir) # .lock_* files are stored in CWD and they must by on shared file system
    sourceDir = os.path.abspath(sourceDir)
    targetDir = os.path.abspath(targetDir)
    res = os.popen('ssh ' + sourceHost + ' "python -c \'import os; sourceDir=\\\"' + sourceDir + '\\\" ; print [x for x in os.walk(sourceDir, topdown=False)]; \'"').readlines()[-1].replace('\n','')
    exec('dirList = ' + res + '')
    for root, dires, files in dirList:
        if files:
            path = (root+os.sep).replace(sourceDir, '')
            filesString = ' '.join(files)
            Lock('mkdir -p '+ targetDir + path +' && ssh ' + sourceHost + ' "cd '+ sourceDir + path +' && tar cf - '+filesString+'" | (cd '+targetDir+path+'. ; tar xf -) && cd -',str(os.getpid()),path)

def cleanPycPyoFiles(slotObject):
    print "############################"
    print "# REMOVED .pyc .pyo FILES: #"
    print "############################"
    if slotObject != None:
        for root, dirs, files in os.walk(slotObject.releaseDir()):
            for name in files:
                if name[-4:] == '.pyc' or name[-4:] == '.pyo':
                    print os.path.join(root, name)
                    os.remove(os.path.join(root, name))

class runThread(Thread):
    def __init__ (self,slotName,platform):
        Thread.__init__(self)
        self.slotName = slotName
        self.platform = platform
    def run(self):
        os.system('python ' + os.path.join(os.environ['LCG_NIGHTLIES_SCRIPTDIR'], 'doBuild.py') + ' --slots ' + self.slotName + ' --platforms ' + self.platform)

def run(slotName, minusj=None, platforms=None):
    """ bootstrap function. Starts the LHCb Nightlies for the given <slotName>

        If <minusj> option is passed to the function, LCG Nightlies "builders" will be generated with -j compiler option.
        If <platforms> list is given, Nightlies will be launched only for selected platforms (platforms need to be defined in configuration.xml anyway).
    """
    if minusj == None: minusj = 0
    else: minusj = int(minusj)
    if not platforms: platformsparam = ''
    else: platformsparam = ' --platforms "' + platforms + '"'
    os.environ['LCG_NIGHTLIES_XMLCONFIGCOPIES'] = configurationHistoryPath
    slot = configuration.findSlot(slotName)
    if slot != None:
        generateBuilders(slot.buildersDir(), listOfProjectNames, minusj)
        os.system('python ' + os.path.join(os.environ['LCG_NIGHTLIES_SCRIPTDIR'], 'doBuild.py') + ' --slots ' + slotName + platformsparam)
        cleanPycPyoFiles(slot)

def runparallel(slotName, minusj=None):
    """ bootstrap function. Starts the LHCb Nightlies for the given <slotName> with all platforms in parallel.

        If <minusj> option is passed to the function, LCG Nightlies "builders" will be generated with -j compiler option.
    """
    if minusj == None: minusj = 0
    else: minusj = int(minusj)
    os.environ['LCG_NIGHTLIES_XMLCONFIGCOPIES'] = configurationHistoryPath
    slot = configuration.findSlot(slotName)
    if slot != None:
        generateBuilders(slot.buildersDir(), listOfProjectNames, minusj)
        runningPlatformList = []
        for p in slot.getPlatforms():
            current = runThread(slotName, p)
            runningPlatformList.append(p)
            current.start()
            time.sleep(600) # wait
        for t in runningPlatformList: t.join()
        cleanPycPyoFiles(slot)

def cleanAFSSpace(slotName):
    """ removes everything from today from AFS for the <slotName> """
    pathToRemoveFiles = nightliesAFSPath + '/' + slotName + '/' + configuration.TODAY
    if os.path.exists(pathToRemoveFiles):
        for x in os.listdir(pathToRemoveFiles):
            if os.path.isdir(os.path.join(pathToRemoveFiles, x)): shutil.rmtree(os.path.join(pathToRemoveFiles, x), ignore_errors=True)
            else: os.remove(os.path.join(pathToRemoveFiles, x))


def getProjectObject(slotObject, projectName):
    """ getProjectObject(slotObject, projectName) -> projectObject

        Finds projectObject in a given slotObject.
    """
    project = None
    for p in slotObject.getProjects():
        if p.getName() == projectName.lower():
            project = p
            break
    if project == None:
        print "Project not found:", projectName
        sys.exit()
    return project

def getSlotAndProject(slotName, projectName):
    """ getSlotAndProject(slotName, projectName) -> slotObject, projectObject

        Finds slotObject first and than projectObject in the slot.
    """
    slot = configuration.findSlot(slotName)
    project = getProjectObject(slot, projectName)
    return slot, project

def setCmtProjectPath(slot):
    """ setCmtProjectPath(slotObject)

        Sets CMTPROJECTPATH according to the configuration of a given slot, Removes CMTPATH variable if set.
    """
    cmtProjectPathPreviuos = None
    if os.environ.has_key('CMTPROJECTPATH'):
        cmtProjectPathPrevious = os.environ['CMTPROJECTPATH']
    cmtProjectPathNew = ''
    cmtProjectPathEntities = []
    cmtProjectPathEntities.append(slot.buildDir())

    if slot.getCmtProjectPaths():
        cmtProjectPathEntities += slot.getCmtProjectPaths()
    else:
        cmtProjectPathEntities.append('/afs/cern.ch/lhcb/software/releases')
        cmtProjectPathEntities.append('/afs/cern.ch/sw/Gaudi/releases')
        cmtProjectPathEntities.append('/afs/cern.ch/sw/lcg/app/releases')
    for e in cmtProjectPathEntities:
        cmtProjectPathNew += e + os.pathsep
    cmtProjectPathNew = cmtProjectPathNew[:-len(os.pathsep)]
    os.environ['CMTPROJECTPATH'] = cmtProjectPathNew
    if os.environ.has_key('CMTPATH'): del os.environ['CMTPATH']

def generatePath(slot, project, type, projectRealName=None):
    """ generatePath(slotObject, projectObject, type[, projectRealName]) -> string

        <type> can be: "TAG" or "SYSPACKAGECMT".
        Depending on the type selected, function returns a string representing path for the slot and the project:
        - TAG:           /build/nightlies/..slot../..day../LHCB/LHCB_v23r4
        - SYSPACKAGECMT: /build/nightlies/..slot../..day../LHCB/LHCB_v23r4/LHCbSys/cmt
    """
    if type == 'TAG':
        return os.path.join(slot.buildDir(), project.getName().upper(), project.getTag())
    elif type == 'SYSPACKAGECMT':
        return os.path.join(slot.buildDir(), project.getName().upper(), project.getTag(), projectRealName + 'Sys', 'cmt')
    else:
        raise Exception, 'type parameter: ' + type + ' not implemented.'

def readRequirementsFile(filename, withHat=False):
    """ readRequirementsFile(filename[, withHat]) -> dict

        Returns dictionary containing all 'use' statements form the given file, with package names as dictionary keys.
        Depending on <withHat> parameter, keys will be preceded by "hat" names (if withHat == True, default: False)
    """
    previousDir = os.getcwd()
    os.chdir(os.path.dirname(filename))
    f1 = os.path.basename(filename)
    f2 = f1 + '_filtered'
    os.system(cmtCommand + ' filter ' + f1 + ' ' + f2)
    readerfile = file(f2,'r')
    reader = readerfile.readlines()
    readerfile.close()
    #TODO: erase filtered file
    values = {}
    for line in reader:
        if re.match('^\s*use\s+[a-zA-Z0-9_-]+', line):
            d = line.split()
            if len(d)<=3: d.append('')
            if len(d)<=4: d.append('')
            if len(d)>=4 and len(d[3])>0 and d[3][0] == '-': # -no_auto_imports
                d[4] = d[3]
                d[3] = ''
            values[d[1]] = [d[2], d[3]]
            for i in range(4, len(d)):
                values[d[1]].append(d[i])
    if withHat:
        newValues = {}
        for v in values.keys():
            if values[v][1] != '': newKey = values[v][1] + '/' + v
            else: newKey = v
            newValues[newKey] = values[v]
        values = newValues
    os.chdir(previousDir)
    return values

def changeRequirementsFile(filename, changesDict, addons=None, removes=None):
    """ changeRequirementsFile(filename, changesDict[, addons[,removes]])

        Function changes the given file, by replacing "use" lines with the ones generated from <changesDict> (result of readReaquirementsFile function).
        Additional parameters: <addons> and <removes> - TODO: need to be checked and probably fixed (after some other changes may not work propertly now).
    """
    readerfile = file(filename,'r')
    reader = readerfile.readlines()
    readerfile.close()
    writer = file(filename,'w')
    for line in reader:
        newline = line
        for x in changesDict.keys():
            nameWithoutHat = changedPackageName(x)
            if changesDict[x][1] != '':
                addToRE = '\s+'+changesDict[x][1]
            else:
                addToRE = ''
            newline = re.sub(nameWithoutHat+'\s+([Hh][Ee][Aa][Dd]|v[\*0-9]+(?:r[\*0-9]+(?:p[\*0-9]+)?)?)'+addToRE, nameWithoutHat+'  '+changesDict[x][0]+'  '+changesDict[x][1], newline)
        # <remove>:
        if removes:
            pac = re.sub('^\s*use\s+([a-zA-Z0-9_-]+).*$', '\\1', line).replace('\n','')
            if pac in [changedPackageName(x) for x in removes]: continue
        writer.write(newline)
    # <addon>
    if addons:
        for a in addons.keys():
            addPackage = changedPackageName(a)
            addHat = ''
            if addPackage != a: addHat = a.replace('/'+addPackage, '')
            writer.write('use ' + addPackage + ' ' + addons[a][0] + ' ' + addHat + "\n")
    writer.close()

def changedPackageName(c):
    """ changedPackageName(packageName) -> package name without "hat" part

        Returns given <packageName> without preceding "hat" name, if the given one consists the "hat". Otherwise, returns the same value.
        - Hat/Name -> Name
        - Name -> Name
    """
    cha = c.split('/')
    if len(cha) == 1: cha = cha[0]
    else: cha = cha[1]
    return cha

def generateBuilders(destPath, projectNames, minusj):
    """ generateBuilders(destinationPath, projectNames, minusj)

        Creates a set of files and directory structure to act as LCG Nightlies "builders" for each of the project given in the <projectNames> list.
        <minusj> parameter sets the value of "-j" for compiler.
    """
    actionsFileAbsPath = os.path.dirname(os.path.abspath(__file__))
    actionsFileAbsPath = os.path.join(actionsFileAbsPath, 'actionLauncher.py')
    settingEnv = 'source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v1r8/InstallArea/scripts/LbLogin.sh -q'
    #settingEnv += ' --cmtsite=$CMTSITE' #+os.environ.get('CMTSITE','""')
    #settingEnv += ' --cmtconfig=$CMTCONFIG' #+os.environ.get('CMTCONFIG','""')
    #settingEnv += ' --userarea=$User_release_area' #+os.environ.get('User_release_area','""')
    #settingEnv += ' --cmtvers='
    settingEnv += ' > /dev/null'
    lines = """
action pkg_get "mkdir -p logs ; """ + settingEnv + """ ; python """ + actionsFileAbsPath + """ get $(packageName) 2>&1 | tee -a logs/$(package)_$(CMTCONFIG)_get.log"
action pkg_config " """ + settingEnv + """ ; python """ + actionsFileAbsPath + """ config $(packageName) 2>&1"
action pkg_make " """ + settingEnv + """ ; python """ + actionsFileAbsPath + """ make $(packageName) """
    if minusj > 0: lines += str(minusj)
    else: lines += "0"
    lines += """ 2>&1 ; exit 0"
action pkg_install " """ + settingEnv + """ ; python """ + actionsFileAbsPath + """ install $(packageName) 2>&1"
action pkg_test " """ + settingEnv + """ ; python """ + actionsFileAbsPath + """ test $(packageName) 2>&1"
    """
    destPath = os.path.abspath(destPath)
    for p in projectNames:
        shutil.rmtree(os.path.join(destPath, p.lower()), ignore_errors=True)
        os.makedirs(os.path.join(destPath, p.lower(), 'cmt') )
        f = file(os.path.join(destPath, p.lower(), 'cmt', 'requirements'), 'w')
        f.write('package ' + p.lower() + os.linesep)
        f.write('macro packageName "' + p + '"' + os.linesep)
        f.write(lines)
        f.close()
    for p in projectNames:
        shutil.rmtree(os.path.join(destPath, p.upper()), ignore_errors=True)
        os.makedirs(os.path.join(destPath, p.upper(), 'cmt') )
        f = file(os.path.join(destPath, p.upper(), 'cmt', 'requirements'), 'w')
        f.write('package ' + p.upper() + os.linesep)
        f.write('macro packageName "' + p + '"' + os.linesep)
        f.write(lines)
        f.close()

def getpackget(package, version, withVersionDir=False):
    """ getpack(packageName, packageVersion[, withVersionDirectory])

        Function launches "getpack" with parameters.
    """
    if withVersionDir: wVD = '-v '
    else: wVD = '-u '
    if version == 'HEAD': version = 'head'
    os.system('echo "--> python /afs/cern.ch/lhcb/scripts/python/getpack.py ' + wVD + package + ' ' + version +'"')
    os.system(getpackCommand + ' ' + wVD + package + ' ' + version)
    time.sleep(0.5)

def disableLCG_NIGHTLIES_BUILD():
    """ Removes "LCG_NIGHTLIES_BUILD" from CMTEXTRATAGS variable. """
    cmtExtraTags = None
    if os.environ.has_key('CMTEXTRATAGS'): cmtExtraTags = os.environ['CMTEXTRATAGS']
    if cmtExtraTags != None:
        listOfCmtExtraTags = cmtExtraTags.split(',')
        if 'LCG_NIGHTLIES_BUILD' in listOfCmtExtraTags: listOfCmtExtraTags.remove('LCG_NIGHTLIES_BUILD')
        cmtExtraTags = ','.join(listOfCmtExtraTags)
        os.environ['CMTEXTRATAGS'] = cmtExtraTags

def config(slotName, projectName):
    """ config(slotName, projectName)

        action function. Starts cmt "config" action (launched from LCG Nightlies "builder").
    """
    disableLCG_NIGHTLIES_BUILD()
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)

def clean(slotName):
    """ clean(slotName)

        Removes contents of the:
        - build directory,
        - "builders" directory,
        - remove logs from web directory,
        for given slot.
    """
    disableLCG_NIGHTLIES_BUILD()
    slot = configuration.findSlot(slotName)
    #remove contents of build directory:
    shutil.rmtree(slot.buildDir(), ignore_errors=True)
    os.makedirs(slot.buildDir())
    #remove contents of builders directory:
    shutil.rmtree(slot.buildersDir(), ignore_errors=True)
    #clear logs from web
    webDir = os.path.join(slot.wwwDir())
    for f in os.listdir(webDir):
        if re.match(slotName + '.' + configuration.TODAY, f): os.remove(os.path.join(webDir, f))

def install(slotName, projectName):
    """ install(slotName, projectName)

        action function. Starts cmt "install" action (launched from LCG Nightlies "builder").
    """
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)
    disableLCG_NIGHTLIES_BUILD()
    os.chdir(generatePath(slot, project, 'SYSPACKAGECMT', projectName))
    configuration.system(cmtCommand + ' br "' + cmtCommand + ' config"')
    # copying logs
    os.chdir(os.path.join(slot.buildDir(), 'www'))
    files = os.listdir(os.getcwd())
    for f in files:
        if not os.path.isfile(f): continue
        shutil.copy2(os.path.join(slot.buildDir(), 'www', f), os.path.join(slot.wwwDir(), f))

    releasePath = slot.releaseDir()
    if releasePath != None:
        #instead of the previous isDone file copying:
        isDoneFrom = os.path.join(slot.buildDir(), 'isDone-'+os.environ['CMTCONFIG'])
        isDoneTo = os.path.join(slot.releaseDir(), 'isDone-'+os.environ['CMTCONFIG'])
        if os.path.exists(isDoneFrom): shutil.copy2(isDoneFrom, isDoneTo)
        #(end)
        #configuration.system('mkdir -p ' + os.path.join(releasePath, projectName.upper()))
        #configuration.system('mkdir -p ' + os.path.join(releasePath, projectName.upper(), project.getTag()))
        #configuration.system('cd ' + os.path.join(slot.buildDir(), projectName.upper(), project.getTag()) +' && tar cf - * | ( cd ' + os.path.join(releasePath, projectName.upper(), project.getTag(), '.') + ' ; tar xf - )')
        copyLocal(os.path.join(slot.buildDir(), projectName.upper(), project.getTag()), os.path.join(releasePath, projectName.upper(), project.getTag()))
        shutil.copy2(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'), releasePath)

def make(slotName, projectName, minusj):
    """ make(slotName, projectName, minusj)

        action function. Starts cmt "make" action (launched from LCG Nightlies "builder").
    """
    if minusj > 1: minusjcmd = ' -j ' + str(minusj)
    else: minusjcmd = ''
    disableLCG_NIGHTLIES_BUILD()
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)
    os.chdir(generatePath(slot, project, 'SYSPACKAGECMT', projectName))
###
#    hosts = '@dlxplus/10'
#    threads = 10
#    distcc = '/afs/cern.ch/user/k/kkruzele/lhcb-nightlies/distcc/bin/distcc'
#    os.environ['DISTCC_HOSTS']=hosts
###
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('date')
    configuration.system('echo "CMTPROJECTPATH:   $CMTPROJECTPATH"')
    configuration.system('echo "CMTCONFIG:        $CMTCONFIG"')
    configuration.system('echo "CMTROOT:          $CMTROOT"')
    configuration.system('echo "CMTPATH:          $CMTPATH"')
    configuration.system('echo "CMTEXTRATAGS:     $CMTEXTRATAGS"')
    configuration.system('echo "LD_LIBRARY_PATH:  $LD_LIBRARY_PATH"')
    configuration.system('echo "PATH:             $PATH"')
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('cmt show macro LCG_releases')
    configuration.system('echo "' + '*'*80 + '"')
    configuration.system('cmt show uses')
    configuration.system('echo "' + '*'*80 + '"')
    #-------------------------------------------------------------------------
    #configuration.system('cmt br - "cmt make all tests"') # it will break on Windows,
    configuration.system(cmtCommand + ' br - "' + cmtCommand + ' make' + minusjcmd + ' all ; ' + cmtCommand + ' make' + minusjcmd +' tests"') # it will break on Windows,
    #configuration.system('cmt br - "cmt make -j 4 all tests"') # it will break on Windows,
    # use: configuration.system('cmt br - cmt make all_groups') instead
    # but it breaks PANORAMIX on Linux
    #-------------------------------------------------------------------------
    #configuration.system('cmt br - cmt make all_groups')
    #configuration.system('cmt br - "/afs/cern.ch/lhcb/software/nightlies/scripts/lockdo.py \\"cmt make all_groups\\" ' + os.environ['CMTCONFIG'] + ' " &')
    #configuration.system('cmt br - "/afs/cern.ch/lhcb/software/nightlies/scripts/lockdo.py \\"export CMTCONFIGcopy=$CMTCONFIG && source /afs/cern.ch/lhcb/software/releases/scripts/CMT.sh > /dev/null && export CMTCONFIG=$CMTCONFIGcopy && cmt make all_groups 2>&1 \\" ' + os.environ['CMTCONFIG'] + ' " &')
    #2# configuration.system('cmt br - "cmt make -j 8 all ; cmt make -j 8 all_groups"')
    #3# configuration.system('cmt br - "cmt make -j ' + str(threads) + ' ' + ' cpp="' + distcc + ' g++" cc="' + distcc + ' gcc" for="' + distcc + ' g77"' + ' all ; cmt make -j 8 ' + ' cpp="' + distcc + ' g++" cc="' + distcc + ' gcc" for="' + distcc + ' g77"' + ' all_groups"')
    ###configuration.system('cmt br cmt make -j ' + str(threads) + ' cpp="' + distcc + ' g++" cc="' + distcc + ' gcc" for="' + distcc + ' g77"')

def test(slotName, projectName):
    """ test(slotName, projectName)

        action function. Starts cmt "test" action (launched from LCG Nightlies "builder").
    """
    disableLCG_NIGHTLIES_BUILD()
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)
    os.chdir(generatePath(slot, project, 'SYSPACKAGECMT', projectName))

    #configuration.system('cmt br - cmt TestPackage 2>&1 | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
    #configuration.system('cmt qmtest_summarize 2>&1 | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
    #filterTestLog('../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
    #configuration.system('date >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')

#    configuration.system('echo "' + '*'*80 + '" | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('date | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('echo "CMTPROJECTPATH:   $CMTPROJECTPATH" | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('echo "CMTCONFIG:        $CMTCONFIG" | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('echo "CMTROOT:          $CMTROOT" | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('echo "CMTPATH:          $CMTPATH" | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('echo "CMTEXTRATAGS:     $CMTEXTRATAGS" | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('echo "' + '*'*80 + '" | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('cmt show macro LCG_releases | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('echo "' + '*'*80 + '" | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('cmt show uses | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    configuration.system('echo "' + '*'*80 + '" | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')

    configuration.system('date | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
    configuration.system('printenv | sort')
    configuration.system('cmt br - cmt TestPackage')
    configuration.system('cmt qmtest_summarize 2>&1 | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
 #   configuration.system('cmt show set PYTHONPATH 2>&1 | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
    #configuration.system('echo "PYTHON sys.path: ' + sys.path + '" >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
    configuration.system('date | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')

#    configuration.system('cmt br - cmt TestPackage')
#    configuration.system('cmt qmtest_summarize 2>&1 | tee -a ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    td = file('../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log', 'r').readlines()
#    if len(td)==2 and td[1].find('Warning: no result file found! (Did you run the tests?)') != -1:
#        os.remove('../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#    else:
#        configuration.system('date >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
#        configuration.system('echo "lines: ' + len(td) + ' ---> ' + td[1] + '" >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')

    #log files copying:
    # why it's not in lhcb_install? is lhcb_install executed before lhcb_test?
    #os.chdir(os.path.join(slot.buildDir(), 'www'))
    testSummaryFrom = os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), '..', '..', 'logs', os.environ['CMTCONFIG'] + '-qmtest.log')
    testSummaryTo = os.path.join(slot.wwwDir(), slotName + '.' + configuration.TODAY + '_' + project.getTag() + '-' + os.environ['CMTCONFIG'] + '-qmtest.log')
    if os.path.exists(testSummaryFrom): shutil.copy2(testSummaryFrom, testSummaryTo)

def get(slotName, projectName):
    """ get(slotName, projectName)

        action function. Starts cmt "get" action (launched from LCG Nightlies "builder").
    """
    disableLCG_NIGHTLIES_BUILD()
    slot, project = getSlotAndProject(slotName, projectName)
    setCmtProjectPath(slot)

    os.chdir(slot.buildDir())
    # create symbolic link: LHCB --> lhcb, LBCOM --> lbcom, ... [ only on Linux ? ]
    if not os.path.exists(project.getName().upper()): os.symlink(project.getName().lower(), project.getName().upper())

    if project.getVersion() == 'HEAD':
        os.chdir(os.path.join(slot.buildDir(), project.getName().upper()))
        os.symlink(project.getTag(), 'HEAD')
        os.chdir(slot.buildDir())
        getpackget(project.getName().upper(), 'HEAD', withVersionDir = True)
        os.chdir(generatePath(slot, project, 'TAG', projectName))
        getpackget(projectName + 'Sys', 'HEAD')
    else:
        os.chdir(slot.buildDir())
        getpackget(project.getName().upper(), project.getTag(), withVersionDir = True)
        os.chdir(generatePath(slot, project, 'TAG', projectName))
        getpackget(projectName + 'Sys', project.getVersion())

    # copy the 'requirements' file
    shutil.copy2(os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), 'requirements'), os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), 'requirements_copy'))

    changesMade = {}
    #change also ProjectSys package in all requirements files later (next projects in the slot) !!!
    changesMade[projectName + 'Sys'] = project.getVersion()
    sysPackageRF = readRequirementsFile(os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), 'requirements'), withHat=True)


####################################################################
####################################################################

    if project.getHoeFlag():
        # add every package from Sys package's requirements file to the list of changes (to HEAD)
        # but IN ORDER! Order is important because of the possibility of over-write sth with <change> tag in XML file
        # --> first add HEAD to the dict, later over-write everything what was in XML file as a <change>
        changesBefore = project.getChanges()
        newChanges = {}
        for x in sysPackageRF.keys(): newChanges[x] = 'HEAD'              # key: Hat/Package
        for x in changesBefore.keys(): newChanges[x] = changesBefore[x]    # key: Hat/Package (in config file it must be with HAT from now if the hat exists)
        project.setChanges(newChanges)

    if len(project.getChanges()) > 0:
        # apply changes to the main `requirements` file
        for change in project.getChanges().items():
            #cha = changedPackageName(change[0])
            cha = change[0]
            if cha in sysPackageRF.keys():
                sysPackageRF[cha][0] = change[1]
            changesMade[change[0]] = change[1]
        changeRequirementsFile(os.path.join(generatePath(slot, project, 'SYSPACKAGECMT', projectName), 'requirements'), sysPackageRF, project.getAddons(), project.getRemoves())

    os.chdir(generatePath(slot, project, 'TAG', projectName))

    for x in sysPackageRF.keys():
        getpackget(x, sysPackageRF[x][0])

    # save the `changesMade` data to a file. It will be used by next projects in the slot
    os.chdir(generatePath(slot, project, 'TAG', projectName))
    pklFile = open('changesMade-' + os.environ['CMTCONFIG'] + '.pkl', 'wb')
    pickle.dump(changesMade, pklFile)
    pklFile.close()

    # read current project's project.cmt file
    projectCmtFileName = os.path.join(generatePath(slot, project, 'TAG', projectName), 'cmt', 'project.cmt')

    readerfile = file(projectCmtFileName, 'r')
    reader = readerfile.readlines()
    readerfile.close()
    writer = file(projectCmtFileName, 'w')

    # apply changes to current project's 'project.cmt' file according to previous project in the slot and <dependence> entities
    deps = project.getDependences()
    projs = {}
    for p in slot.getProjects():
        if p.getName().upper() == project.getName().upper(): break
        projs[p.getName().upper()] = p.getTag()
    for line in reader:
        newline = line
        for proj in projs.keys():
            newline = re.sub('^(\s*use\s+' + proj + '\s+).*$', '\\1' + projs[proj], newline)
        for change in deps.keys():
            newline = re.sub('^(\s*use\s+' + change.upper() + '\s+).*$', '\\1' + deps[change], newline)
        writer.write(newline)
    writer.close()

    # apply changes of previous projects AND the CURRENT PROJECT in the slot to current project's 'requirements' file
    previousChangesMade = {}
    for p in slot.getProjects():
        if p.getName().upper() == 'LCGCMT': continue
        if p.getName().upper() == project.getName().upper(): break
        pklFile = file(os.path.join(generatePath(slot, p, 'TAG', projectName), 'changesMade-' + os.environ['CMTCONFIG'] + '.pkl'), 'rb')
        tmpChangesMade = pickle.load(pklFile)
        pklFile.close()
        previousChangesMade.update(tmpChangesMade)

    changesMadeDict = previousChangesMade.copy()
    changesMadeDict.update(changesMade)

    os.chdir(generatePath(slot, project, 'TAG', projectName))
    #requirementsFilesList = os.popen('find . -type f -name requirements | grep -v "^./' + projectName + 'Sys/cmt/requirements"').readlines()
    requirementsFilesList = []
    for x in sysPackageRF.keys():
        requirementsFilesList.append(os.path.join(x.replace('/', os.sep), 'cmt', 'requirements'))

    for f in requirementsFilesList:
        f = f.replace('\n','')
        shutil.copy2(f, f + '_copy')
        d = readRequirementsFile(f, withHat=True)
        for x in d.keys():
            nameWithoutHat = changedPackageName(x)
            if x in changesMadeDict.keys():
                d[x][0] = changesMadeDict[x]
        changeRequirementsFile(f, d)

    os.chdir(generatePath(slot, p, 'SYSPACKAGECMT', projectName))
    configuration.system(cmtCommand + ' br "' + cmtCommand + ' config"')

