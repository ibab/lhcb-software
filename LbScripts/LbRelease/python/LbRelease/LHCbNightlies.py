
import os, sys, random, shutil
import re, pickle, time, datetime
import checkLogFiles

#__all__ = ['LHCbProjectBuilder', 'builderMap']

class LHCbProjectBuilder(object):

    def canBuild(cls, project):
        from LbConfiguration.Project import project_names
        return (project in project_names)
    canBuild = classmethod(canBuild)

    def __init__(self, slot,thread,load, proj, tag, platIn, conf, thread_counter, tpInstaller, wu, sem, tagOrder, host='', port=''):
        print "[LHCb] __init__\n"
        self.cmtCommand = 'cmt -disable_warnings'
        self.getpackCommand = 'getpack --no-config --batch -p anonymous'
        self.pythonCommand = 'python '

        from LbConfiguration.Project import project_names as listOfProjectNames
        self.projectNamesDict = {}
        for p in listOfProjectNames:
            self.projectNamesDict[p.upper()] = p

        self.systemType = 'linux' # (~'windows')
        self.slot = slot
        self.slotName = slot.getName()
        self.project = slot.getProject(proj)
        self.projName = self.projectNamesDict[proj.upper()]
        self.tagName = tag
        self.config = conf
        self.plat = platIn
        #os.environ['CMTCONFIG'] = self.plat #.getName()
        self.day = datetime.date.today().strftime('%a')
        self.configurationHistoryPath = os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'lhcb', 'software', 'nightlies', 'conf', 'history'])

        if self.config._configContents:
            self.configContents = self.config._configContents
        elif self.configFile:
            self.configContents = file(self.config._configFile).read()
        else:
            self.configContents = None

        self.outputFile = None
    def system(self, cmd, logfile=None):
        if logfile is not None:
            log = ' 2>&1 | tee -a %s' % logfile
        elif self.outputFile is not None:
            log = ' 2>&1 | tee -a %s' % self.outputFile
        else:
            log = ''
        os.system(cmd + log)

    def buildProject(self):
        print "[LHCb] buildProject: start"
        ################################
        # save configuration.xml taken from the server to "history" directory if environ variable is set
        #os.environ['LCG_NIGHTLIES_XMLCONFIGCOPIES'] = self.configurationHistoryPath
        # make copy of configuration.xml file if LCG_NIGHTLIES_XMLCONFIGCOPIES exists
        self.makeCopy(datetime.date.today().strftime('%a'))
        ################################
        self.checkout()
        self.build()
        self.test()
        self.docs()
        checkLogFiles.checkBuildLogs(self.slot, self.project, self.day, self.plat, self.config._generalConfig)
        self.install()
        self.status = 0
        pass


    # --------------------------------------------------------------------------------

    def setupProj(self):
        """ call this again if any of the (input) paramaters change """

        print "[LHCb] setupProj"
        pass

    # --------------------------------------------------------------------------------

    def action(self, cmd, description, withLocking=False):

        """ Action taking place in projDir/tagDir """

        print "[LHCb] action(%s,%s,%s)" % (str(cmd), str(description), str(withLocking))
        return 0

    # --------------------------------------------------------------------------------
#    def postCheckout(self,stampfile,log):
#        pass

    def getArchivedSlotNames(self, day):
        if self.configurationHistoryPath and os.path.exists(os.path.join(self.configurationHistoryPath, day + '.pkl')):
            # if emptyFile: return None
            pklFile = file(os.path.join(self.configurationHistoryPath, day + '.pkl'), 'rb')
            tmpSlotNames = pickle.load(pklFile)
            pklFile.close()
            return tmpSlotNames
        return None

    def makeCopy(self, day):
        if self.configurationHistoryPath:
            #shutil.copy2(os.path.join(os.path.abspath(os.environ['LCG_XMLCONFIGDIR']), 'configuration.xml'), os.path.join(self.configurationHistoryPath, day + '.' + self.slotName + '.xml'))
            file(os.path.join(self.configurationHistoryPath, day + '.' + self.slotName + '.xml'), 'w').write(self.configContents)
            if not self.getArchivedSlotNames(day):
                #create slot lost for the specific day
                pklFile = open(os.path.join(self.configurationHistoryPath, day + '.pkl'), 'wb')
                pickle.dump([self.slotName], pklFile)
                pklFile.close()
            else:
                tmpSlotNames = self.getArchivedSlotNames(day)
                if self.slotName not in tmpSlotNames: tmpSlotNames.append(self.slotName)
                pklFile = open(os.path.join(self.configurationHistoryPath, day + '.pkl'), 'wb')
                pickle.dump(tmpSlotNames, pklFile)
                pklFile.close()

    def checkout(self):
        print "[LHCb] checkout"
        os.environ['CMTCONFIG'] = self.plat

        self.disableLCG_NIGHTLIES_BUILD()
        self.setCMTEXTRATAGS(self.slotName)
        ##slot, project = getSlotAndProject(slotName, projectName)
        self.setCmtProjectPath(self.slot)
        self.changeEnvVariables()

        os.chdir(self.slot.buildDir())
        if not os.path.exists('www'): os.mkdir('www')
        if os.path.exists(self.project.getName()):
            # create symbolic link: LHCB --> LHCb, LBCOM --> Lbcom, ... [ only on Linux ? ]
            if not os.path.exists(self.project.getName().upper()):
                os.symlink(self.project.getName(), self.project.getName().upper())
        elif os.path.exists(self.project.getName().lower()):
            # create symbolic link: LHCB --> lhcb, LBCOM --> lbcom, ... [ only on Linux ? ]
            if not os.path.exists(self.project.getName().upper()):
                os.symlink(self.project.getName().lower(), self.project.getName().upper())
        else:
            if os.path.exists(self.project.getName().upper()):
                os.symlink(self.project.getName().upper(), self.project.getName())
        # check if checkout is running, or finished on that machine
        if not os.path.exists(self.generatePath(self.slot, self.project, 'TAG', self.project.getName())):
            os.makedirs(self.generatePath(self.slot, self.project, 'TAG', self.project.getName().upper()))
            os.symlink(self.project.getName(), self.project.getName().upper())
        os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.project.getName()))
        waitcounter = 0
        while os.path.exists('checkout.working'):
            time.sleep(300)
            waitcounter += 1
            if waitcounter > 20: # (20*5 min = 1h)
                print 'checkout failed in another job, waiting for 1h not successful. ERROR.'
                return 1
        if os.path.exists('checkout.done'):
            print 'Checkout already done for this machine. Skipped.'
            return 0
        fW = open('checkout.working','w')
        fW.write('\n')
        fW.close()

        os.chdir(self.slot.buildDir())
        self.getpackget(self.project.getName(), self.project.getVersion(), project=True)
        #if os.path.exists(self.project.getName().upper()) and self.project.getName() != self.project.getName().upper():
        #    print "renaming from: %s, to: %s" % (self.project.getName().upper(), self.project.getName())
        #    os.rename(self.project.getName().upper(), self.project.getName())
        #    print "renaming from: %s, to: %s - DONE" % (self.project.getName().upper(), self.project.getName())

        os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.project.getName()))

        containerPackageName = self.containerPackage('.')
        self.getpackget(containerPackageName, self.project.getVersion())

        # copy the 'requirements' file
        shutil.copy2(os.path.join(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName), 'requirements'), os.path.join(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName), 'requirements_copy'))

        changesMade = {}
        #change also ProjectSys package in all requirements files later (next projects in the slot) !!!
        changesMade[containerPackageName] = self.project.getVersion()
        sysPackageRF = self.readRequirementsFile(os.path.join(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName), 'requirements'), withHat=True)

        if self.project.getHoeFlag():
            # add every package from Sys package's requirements file to the list of changes (to HEAD)
            # but IN ORDER! Order is important because of the possibility of over-write sth with <change> tag in XML file
            # --> first add HEAD to the dict, later over-write everything what was in XML file as a <change>
            changesBefore = self.project.getChanges()
            newChanges = {}
            for x in sysPackageRF.keys(): newChanges[x] = 'HEAD'              # key: Hat/Package
            for x in changesBefore.keys(): newChanges[x] = changesBefore[x]    # key: Hat/Package (in config file it must be with HAT from now if the hat exists)
            self.project.setChanges(newChanges)

        if len(self.project.getChanges()) > 0:
            # apply changes to the main `requirements` file
            for change in self.project.getChanges().items():
                #cha = changedPackageName(change[0])
                cha = change[0]
                if cha in sysPackageRF.keys():
                    sysPackageRF[cha][0] = change[1]
                changesMade[change[0]] = change[1]
            self.changeRequirementsFile(os.path.join(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName), 'requirements'), self.sysPackageRF, self.project.getAddons(), self.project.getRemoves())

        os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.projName))

        self.getpackget(containerPackageName, self.project.getVersion(), recursive=True)

        # save the `changesMade` data to a file. It will be used by next projects in the slot
        os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.projName))
        pklFile = open('changesMade-' + os.environ['CMTCONFIG'] + '.pkl', 'wb')
        pickle.dump(changesMade, pklFile)
        pklFile.close()

        # read current project's project.cmt file
        projectCmtFileName = os.path.join(self.generatePath(self.slot, self.project, 'TAG', self.projName), 'cmt', 'project.cmt')

        readerfile = file(projectCmtFileName, 'r')
        reader = readerfile.readlines()
        readerfile.close()
        writer = file(projectCmtFileName, 'w')

        # apply changes to current project's 'project.cmt' file according to previous project in the slot and <dependence> entities
        deps = self.project.getDependences()
        projs = {}
        for p in self.slot.getProjects():
            if p.getName().upper() == self.project.getName().upper(): break
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
        for p in self.slot.getProjects():
            if p.getName().upper() == 'LCGCMT': continue
            if p.getName().upper() == self.project.getName().upper(): break
            pklFile = file(os.path.join(generatePath(slot, p, 'TAG', projectName), 'changesMade-' + os.environ['CMTCONFIG'] + '.pkl'), 'rb')
            tmpChangesMade = pickle.load(pklFile)
            pklFile.close()
            previousChangesMade.update(tmpChangesMade)

        changesMadeDict = previousChangesMade.copy()
        changesMadeDict.update(changesMade)

        parentDir = self.generatePath(self.slot, self.project, 'TAG', self.projName)
        os.chdir(parentDir)
        requirementsFilesList = []
        for x in sysPackageRF.keys():
            fileNameTmp = os.sep.join([parentDir, x.replace('/', os.sep), 'cmt', 'requirements'])
            if os.path.exists(fileNameTmp):
                requirementsFilesList.append(fileNameTmp)
            else:
                os.system('echo "ERROR: file not found: ' + fileNameTmp + '"')

        for f in requirementsFilesList:
            f = f.replace('\n','')
            shutil.copy2(f, f + '_copy')
            d = self.readRequirementsFile(f, withHat=True)
            for x in d.keys():
                nameWithoutHat = self.changedPackageName(x)
                if x in changesMadeDict.keys():
                    d[x][0] = changesMadeDict[x]
            self.changeRequirementsFile(f, d)

        os.chdir(self.generatePath(self.slot, p, 'SYSPACKAGECMT', self.projName))
        os.system(self.cmtCommand + ' br "' + self.cmtCommand + ' config"')

        # creating 'done' flag; removing 'working' flag
        os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.project.getName()))
        fW = open('checkout.done','w')
        fW.write('\n')
        fW.close()
        os.remove('checkout.working')
        return 0

    def build(self):
        print "[LHCb] build"
        os.environ['CMTCONFIG'] = self.plat
        logdir = os.sep.join([self.generatePath(self.slot, self.project, 'TAG', self.projName), 'logs'])
        if not os.path.exists(logdir):
            os.makedirs(logdir)

        self.disableLCG_NIGHTLIES_BUILD()
        self.setCMTEXTRATAGS(self.slotName)
        self.setCmtProjectPath(self.slot)

        if os.path.exists(os.sep.join([self.generatePath(self.slot, self.project, 'TAG', self.projName), 'build.'+os.environ.get('CMTCONFIG','')+'.done'])):
            print 'Build already done for ' + os.environ.get('CMTCONFIG', '') + '. Skipped.'
            return
        else:
            # removing rubbish from log file
            os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.projName))
            try:
                logFileToBeRemoved = file(os.sep.join(['logs', os.environ.get('CMTCONFIG', '')])+'.log', 'w')
                logFileToBeRemoved.close()
            except:
                pass
        self.changeEnvVariables()

        self.outputFile = os.sep.join([logdir, os.environ.get('CMTCONFIG', '') + '.log'])
        os.chdir(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName))
        self.system('echo "' + '*'*80 + '"')
        self.system('echo "'+ time.strftime('%c', time.localtime()) +'"')
        self.system('echo "LCG Nightlies:    '+os.path.dirname(__file__)+'"')
        self.system('echo "SITEROOT:         '+os.environ.get('SITEROOT','')+'"')
        self.system('echo "CMTROOT:          '+os.environ.get('CMTROOT','')+'"')
        self.system('echo "CMTPROJECTPATH:   '+os.environ.get('CMTPROJECTPATH','')+'"')
        self.system('echo "CMTCONFIG:        '+os.environ.get('CMTCONFIG','')+'"')
        self.system('echo "CMTPATH:          '+os.environ.get('CMTPATH','')+'"')
        self.system('echo "CMTEXTRATAGS:     '+os.environ.get('CMTEXTRATAGS','')+'"')
        self.system('echo "LD_LIBRARY_PATH:  '+os.environ.get('LD_LIBRARY_PATH','')+'"')
        self.system('echo "LIB:              '+os.environ.get('LIB','')+'"')
        self.system('echo "INCLUDE:          '+os.environ.get('INCLUDE','')+'"')
        self.system('echo "PATH:             '+os.environ.get('PATH','')+'"')
        self.system('echo "LBUTILSROOT:      '+os.environ.get('LBUTILSROOT','')+'"')
        self.system('echo "' + '*'*80 + '"')
        self.system('cmt show macro LCG_releases')
        self.system('echo "' + '*'*80 + '"')
        self.system('cmt show projects')
        self.system('echo "' + '*'*80 + '"')
        self.system('cmt show uses')
        self.system('echo "' + '*'*80 + '"')
        self.system('cmt show tags')
        self.system('echo "' + '*'*80 + '"')
        self.system('printenv | sort')
        self.system('echo "' + '*'*80 + '"')

        if os.path.exists(os.sep.join(['..','..', 'Makefile'])):
            os.chdir(os.sep.join(['..']*2))
            if 'CMTEXTRATAGS' in os.environ and len(os.environ['CMTEXTRATAGS'])>0:
                os.environ['CMTEXTRATAGS'] = 'no-pyzip,'+os.environ.get('CMTEXTRATAGS', '')
            else:
                os.environ['CMTEXTRATAGS'] = 'no-pyzip'
            self.system('make -k Package_failure_policy=ignore logging=enabled | tee make.%s.log' % (os.environ['CMTCONFIG']) )
            os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.projName))
            logFiles = []
            for r, d, f in os.walk("."):
                if r == ".": continue
                elif "cmt" in d: d[:] = ["cmt"]
                else:
                    if "build.%(CMTCONFIG)s.log" % os.environ in f:
                        logFiles.append((os.stat(os.path.join(r, "build.%(CMTCONFIG)s.log" % os.environ)).st_mtime, os.path.join(r, "build.%(CMTCONFIG)s.log" % os.environ)))
            logFiles.sort()
            for x in logFiles:
                print "-"*80
                print "Logfile:", x[1]
                print "-"*80
                print file(x[1], 'r').read()
        else:
            makeCmd = '%s make ' % (self.cmtCommand,)
            if self.slot.getQuickMode() is not None : makeCmd += ' QUICK=' + str(self.slot.getQuickMode())  # append
            if 'CMTEXTRATAGS' in os.environ and len(os.environ['CMTEXTRATAGS'])>0:                # prepend
                makeCmd = 'CMTEXTRATAGS=no-pyzip,%(CMTEXTRATAGS)s ' % os.environ + makeCmd
            else:
                makeCmd = 'CMTEXTRATAGS=no-pyzip ' + makeCmd
            cmtCmdForBroadcast = '%s all ; %s tests' % (makeCmd, makeCmd)
            fullCmd = '%s br - "%s"' % (cmtCommand, cmtCmdForBroadcast)
            self.system('echo "COMMAND: ' + fullCmd.replace('"','\\"') + '"')
            self.system(fullCmd)
        self.system('echo "' + '*'*80 + '"')
        self.system('echo "Build finished: '+ time.strftime('%c', time.localtime()) +'"')

        os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.projName))
        buildFlagFile = open('./build.'+os.environ.get('CMTCONFIG','')+'.done', 'w')

        return 0

    def docs(self):
        self.disableLCG_NIGHTLIES_BUILD()
        self.setCMTEXTRATAGS(self.slotName)
        self.setCmtProjectPath(self.slot)
        self.changeEnvVariables()
        if not self.project.getDocsFlag(): return
        else:
            time.sleep(2)
            os.system('echo "Doxygen start: '+ time.strftime('%c', time.localtime()) +'"')
            os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.projName))
            os.system('make docs > ./docs.log')
            os.system('echo "' + '*'*80 + '"')
            os.system('echo "Doxygen finished: '+ time.strftime('%c', time.localtime()) +'"')
            releasePath = self.slot.releaseDir()
            if os.path.exists(os.path.join(self.slot.buildDir(), self.projName.upper(), self.project.getTag(), 'InstallArea', 'doc')) and releasePath != None:
                os.system('echo "' + '*'*80 + '"')
                os.system('echo "Copying documentation to AFS started: '+ time.strftime('%c', time.localtime()) +'"')
                self.copyLocal(os.path.join(self.slot.buildDir(), self.projName.upper(), self.project.getTag(), 'InstallArea', 'doc'), os.path.join(releasePath, self.projName.upper(), self.project.getTag(), 'InstallArea', 'doc'))
                os.system('echo "Copying documentation to AFS finished: '+ time.strftime('%c', time.localtime()) +'"')
        return 0

    def install(self):
        print "[LHCb] install"
        self.setCmtProjectPath(self.slot)
        self.changeEnvVariables()
        self.disableLCG_NIGHTLIES_BUILD()
        self.setCMTEXTRATAGS(self.slotName)
        os.chdir(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName))

        # copying logs
        os.chdir(os.path.join(self.slot.buildDir(), 'www'))
        files = os.listdir(os.getcwd())
        for f in files:
            if not os.path.isfile(f): continue
            shutil.copy2(os.path.join(self.slot.buildDir(), 'www', f), os.path.join(self.slot.wwwDir(), f))

        releasePath = self.slot.releaseDir()
        if releasePath != None:
            #instead of the previous isDone file copying:
            isDoneFrom = os.path.join(self.slot.buildDir(), 'isDone-'+os.environ['CMTCONFIG'])
            isDoneTo = os.path.join(self.slot.releaseDir(), 'isDone-'+os.environ['CMTCONFIG'])
            if os.path.exists(isDoneFrom):
                shutil.copy2(isDoneFrom, isDoneTo)
            isStartedTo = os.path.join(self.slot.releaseDir(), 'isStarted-'+os.environ['CMTCONFIG'])
            if not os.path.exists(isStartedTo): file(isStartedTo, "w").close()
            if os.path.exists(isDoneTo) and os.path.exists(isStartedTo): os.remove(isStartedTo)
            self.copyLocal(os.path.join(self.slot.buildDir(), self.projName.upper(), self.project.getTag()), os.path.join(releasePath, self.projName.upper(), self.project.getTag()))
            if not os.path.exists(os.path.join(releasePath, 'configuration.xml')):  shutil.copy2(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'), releasePath)
        return 0

    def test(self):
        print "[LHCb] test"

        self.disableLCG_NIGHTLIES_BUILD()
        self.setCMTEXTRATAGS(self.slotName)
        self.setCmtProjectPath(self.slot)
        self.changeEnvVariables()
        os.chdir(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName))

        os.system('echo "'+ time.strftime('%c', time.localtime()) +'" >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
        os.system('cmt br - cmt TestPackage')
        os.system('cmt qmtest_summarize 2>&1 >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
        os.system('echo "'+ time.strftime('%c', time.localtime()) +'" >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')

        #log files copying:
        testSummaryFrom = os.path.join(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName), '..', '..', 'logs', os.environ['CMTCONFIG'] + '-qmtest.log')
        testSummaryTo = os.path.join(self.slot.wwwDir(), self.slotName + '.' + datetime.date.today().strftime('%a') + '_' + self.project.getTag() + '-' + os.environ['CMTCONFIG'] + '-qmtest.log')
        if os.path.exists(testSummaryFrom): shutil.copy2(testSummaryFrom, testSummaryTo)

        return 0

#=============================== Nightlies/function.py ==============================#
    def generatePath(self, slot, project, type, projectRealName=None):
        """ generatePath(slotObject, projectObject, type[, projectRealName]) -> string

            <type> can be: "TAG" or "SYSPACKAGECMT".
            Depending on the type selected, function returns a string representing path for the slot and the project:
            - TAG:           /build/nightlies/..slot../..day../LHCB/LHCB_v23r4
            - SYSPACKAGECMT: /build/nightlies/..slot../..day../LHCB/LHCB_v23r4/LHCbSys/cmt
        """
        projectRoot = os.path.join(slot.buildDir(), project.getName(), project.getTag())
        if type == 'TAG':
            return projectRoot
        elif type == 'SYSPACKAGECMT':
            return os.path.join(projectRoot, self.containerPackage(projectRoot), 'cmt')
        else:
            raise Exception, 'type parameter: ' + type + ' not implemented.'

    def readRequirementsFile(self, filename, withHat=False):
        """ readRequirementsFile(filename[, withHat]) -> dict

            Returns dictionary containing all 'use' statements form the given file, with package names as dictionary keys.
            Depending on <withHat> parameter, keys will be preceded by "hat" names (if withHat == True, default: False)
        """
        previousDir = os.getcwd()
        os.chdir(os.path.dirname(filename))
        os.system(self.cmtCommand + ' filter ' + filename + ' ' + filename + '_filtered')
        readerfile = file(filename + '_filtered', 'r')
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

    def changeRequirementsFile(self, filename, changesDict, addons=None, removes=None):
        """ changeRequirementsFile(filename, changesDict[, addons[,removes]])

            Function changes the given file, by replacing "use" lines with the ones generated from <changesDict> (result of readReaquirementsFile function).
            Additional parameters: <addons> and <removes> - TODO: need to be checked and probably fixed (after some other changes may not work properly now).
        """
        readerfile = file(filename,'r')
        reader = readerfile.readlines()
        readerfile.close()
        writer = file(filename,'w')
        for line in reader:
            newline = line
            for x in changesDict.keys():
                nameWithoutHat = self.changedPackageName(x)
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

    def changedPackageName(self, c):
        """ changedPackageName(packageName) -> package name without "hat" part

            Returns given <packageName> without preceding "hat" name, if the given one consists the "hat". Otherwise, returns the same value.
            - Hat/Name -> Name
            - Name -> Name
        """
        cha = c.split('/')
        if len(cha) == 1: cha = cha[0]
        else: cha = cha[1]
        return cha

    def getpackget(self, package, version, withVersionDir=False, recursive=False, project=False):
        """ getpack(packageName, packageVersion[, withVersionDirectory])

            Function launches "getpack" with parameters.
        """
        cmd = self.getpackCommand
        if withVersionDir: cmd += ' -v'
        if recursive: cmd += ' -r'
        if project: cmd += ' -P'
        cmd += ' %s %s' % (package, version)
        print "*"*80
        print cmd
        print "*"*80
        os.system(cmd)

    def disableLCG_NIGHTLIES_BUILD(self):
        """ Removes "LCG_NIGHTLIES_BUILD" from CMTEXTRATAGS variable. """
        cmtExtraTags = None
        if os.environ.has_key('CMTEXTRATAGS'): cmtExtraTags = os.environ['CMTEXTRATAGS']
        if cmtExtraTags != None:
            listOfCmtExtraTags = cmtExtraTags.split(',')
            if 'LCG_NIGHTLIES_BUILD' in listOfCmtExtraTags: listOfCmtExtraTags.remove('LCG_NIGHTLIES_BUILD')
            cmtExtraTags = ','.join(listOfCmtExtraTags)
            os.environ['CMTEXTRATAGS'] = cmtExtraTags

    def setCMTEXTRATAGS(self, slotName):
        #global configuration
        #tmp = reload(configuration)
        #generalConfig, slotList = configuration.readConf(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'))
        #slot = configuration.findSlot(slotName, slotList)
        slot = self.slot
        if slot is not None and slot.cmtExtraTags is not None:
            os.environ['CMTEXTRATAGS'] = slot.cmtExtraTags

    def changeEnvVariables(self):
        if self.systemType != 'windows': return
        for x in ['CMTPROJECTPATH', 'CMTROOT', 'LD_LIBRARY_PATH', 'LIB', 'INCLUDE', 'PATH']:
            if not os.environ.has_key(x): continue
            pth = os.environ[x].split(';')
            newPth1 = []
            if x == 'CMTPROJECTPATH':
                # first position of CMTPROJECTPATH is always local build dir, and the order must not be changed
                newPth1.append(pth[0])
                pth = pth[1:]
            for y in pth:
                yy = y.lower()
                newY = yy
                for q in localDirsOnWindows.keys():
                    if q.lower() in newY:
                        newY = newY.replace(q.lower(), localDirsOnWindows[q])
                        newY = newY.lower()
                if yy != newY:
                    newPth1.append(newY)
                else:
                    newPth1.append(y)
            if ';' in os.environ[x]:
                os.environ[x] = ';'.join([xx for xx in newPth1 if len(xx)>0])
            else:
                os.environ[x] = ([xx for xx in newPth1 if len(xx)>0])[0]

    def setCmtProjectPath(self, slot): # slot = slotObj
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
            cmtProjectPathEntities.append(os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'lhcb', 'software', 'releases']))
            cmtProjectPathEntities.append(os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'sw', 'Gaudi', 'releases']))
            cmtProjectPathEntities.append(os.sep.join([os.environ['AFSROOT'], 'cern.ch', 'sw', 'lcg', 'app', 'releases']))
        for e in cmtProjectPathEntities:
            cmtProjectPathNew += e + os.pathsep
        cmtProjectPathNew = cmtProjectPathNew[:-len(os.pathsep)]
        os.environ['CMTPROJECTPATH'] = cmtProjectPathNew
        if os.environ.has_key('CMTPATH'): del os.environ['CMTPATH']

    def containerPackage(self, projectRoot):
        projectName = None
        containerPkg = None #projectName + 'Sys'
        for l in open(os.path.join(projectRoot,"cmt","project.cmt")):
            l = l.strip().split()
            #print l
            if len(l) == 2:
                if l[0] == "container":
                    containerPkg = l[1]
                    break
                elif l[0] == "project":
                    projectName = l[1]
        if containerPkg is None and projectName:
            projectName = self.projectNamesDict.get(projectName.upper(), projectName)
            containerPkg = projectName + 'Sys'
        return containerPkg

    def copyLocal(self, sourceDir, targetDir):
        """ tar-pipe copying with locking from <sourceDir> to <targetDir>. Target directory is created if necessary.

            Locking is implemented on the level of the most deep sub-directory. Empty directories are not copied.
            Works only on Linux, both <sourceDir> and <targetDir> must be accessible locally.
        """
        from LbUtils.Lock import Lock
        if not os.path.exists(targetDir): os.makedirs(targetDir)
        os.chdir(targetDir) # .lock_* files are stored in CWD and they must by on shared file system
        sourceDir = os.path.abspath(sourceDir)
        targetDir = os.path.abspath(targetDir)
        dirList = [x for x in os.walk(sourceDir, topdown=False)]
        for root, dires, files in dirList:
            if files:
                path = (root+os.sep).replace(sourceDir, '')
                files = [x for x in files if x[-4:] != '.pyc' and x[-4:] != '.pyo']
                if len(' '.join(files)) > 30720: # to be on the safe side: 30K limit (Win: 32K, Red Hat 128K)
                    maxFilesPerCopyCmd = 200 # files
                    listOfLists = [files[i:i+maxFilesPerCopyCmd] for i in range(0, len(files), maxFilesPerCopyCmd)]
                else:
                    listOfLists = [files[:]]
                for gFiles in listOfLists:
                    files = gFiles[:]
                    filesString = ' '.join(files)
                    cwd = os.getcwd()
                    commandLinux = 'mkdir -p '+ targetDir + path +' && cd '+ sourceDir + path +' && tar cf - '+filesString+' | (cd '+targetDir+path+'. ; tar xf -) && cd -'
                    commandWindows = '(if not exist '+targetDir + path+' mkdir '+ targetDir + path +')'+' '.join([' & ( if not exist '+targetDir+path+x+' copy '+sourceDir + path +x+' '+targetDir+path+' > NUL )' for x in files]) + ' & (cd '+cwd+')'
                    if self.systemType == 'windows':
                        Lock(commandWindows,str(os.getpid()), commandLinux, splitCommand='&')
                    else:
                        Lock(commandLinux,str(os.getpid()), commandLinux)

