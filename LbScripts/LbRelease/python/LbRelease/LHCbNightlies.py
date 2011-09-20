
import os, sys, shutil
import re, pickle, time, datetime
import codecs, cgi

print "#"*60
print "#"*60
print "LHCB NIGHTLIES EXTENSION"
print "#"*60
print "#"*60

def fork_call(func):
    def f(self, *args, **kwargs):
        if self.systemType == "windows":
            return func(self, *args, **kwargs)
        elif os.fork() == 0:
            func(self, *args, **kwargs)
            time.sleep(10)
            sys.exit()
    return f
def ignore_exception(func): 
    def f(self, *args, **kwargs):
        try:
            return func(self, *args, **kwargs)
        except SystemExit:
            pass
            #raise  -- removed raising SystemExit - thartman - 2011.05.12
        except:
            print "Exception raised in Nightlies Client"
            import traceback
            traceback.print_exc()
    return f

##########################################################################################
# before in LCG part: LHCbCheckLogFiles.py 

def checkBuildLogs(slotObj, projObj, day, platform, config, createHTMLLogFile = True, createSummaryLogFile = True):
    tagName = projObj.getTag()
    try:
        if projObj.getRename() is not None:
            tagName = projObj.getRename()
    except:
        pass
    logDir = os.path.join(slotObj.buildDir(weekday=day), "www")
    fileName = os.path.join(slotObj.buildDir(weekday=day), projObj.getName(), tagName, "logs", platform+".log")
    rawLogName = '%s.%s_%s-%s' % (slotObj.getName(), day, tagName, platform)
    htmlLogName = '%s-log.html' % rawLogName

    logData = map(lambda x: x.replace('\n',''), codecs.open(fileName, 'r', 'utf-8').readlines())

    htmlData = []

    regexError = [re.compile('.*make.*\WError\s*1.*')
                 ,re.compile('.*\Werror[: ].*', re.I)
                 ,re.compile('^error .*', re.I)
                 ,re.compile('\.\.\.failed .*', re.I)
                 ,re.compile('.*isk quota exceeded.*', re.I)
                 ]
    regexWarning = [re.compile('.*warning[: ].*', re.I)
                   ]
    
    regexCoverity = [re.compile('.*cov-.*', re.I)
                     ,re.compile('.*Coverity warning[: ].*', re.I)
                     ,re.compile('.*Coverity error[: ].*', re.I)
                     ,re.compile('.*Coverity message[: ].*', re.I)
                   ]
    # + filter for further Coverity annotations
    # + mismatch between checked and existing files --> append to the general log
    # # --> add coverity to packagesList as pseudo-Package? --> ad regexp to packageStart
    
    
#    ignoreErrors = config.get('ignoreErrors', [])
#    ignoreErrorsCounters = [0]*len(ignoreErrors)
#    ignoreWarnings = config.get('ignoreWarnings', [])
#    ignoreWarningsCounters = [0]*len(ignoreWarnings)
#    ignoreErrorsRegex = config.get('ignoreErrorsRegex', [])
#    ignoreErrorsRegexCounters = [0]*len(ignoreErrorsRegex)
#    ignoreWarningsRegex = config.get('ignoreWarningsRegex', [])
#    ignoreWarningsRegexCounters = [0]*len(ignoreWarningsRegex)
    #ignoreErrorsGlob = config.get('ignoreErrorsGlob', [])
    #ignoreErrorsGlobCounters = [0]*len(ignoreErrorsGlob)
    #ignoreWarningsGlob = config.get('ignoreWarningsGlob', [])
    #ignoreWarningsGlobCounters = [0]*len(ignoreWarningsGlob)
#    errorList = []   # elements should be pairs: [['error/warn. message', [line number, line number, ...]]
#    warningList = []

    
    class StringMatcher(object):
        def __init__(self, s):
            self.string = s
            self.count = 0
        def __call__(self, s):
            if self.string in s:
                self.count += 1
                return True
            return False
        def __str__(self):
            return "%s %d" % (self.string, self.count)
        def html(self):
            return '<strong>%sx</strong>&nbsp;&rArr;&nbsp;%s<br />' % (str(self.count), self.string)
    class REMatcher(StringMatcher):
        def __init__(self, s):
            super(REMatcher, self).__init__(s)
            self.rexp = re.compile(s)
        def __call__(self, s):
            m = self.rexp.match(s)
            if m:
                self.count += 1
            return m

    errorExclusions = map(StringMatcher, config.get('ignoreErrors', [])) + map(REMatcher, config.get('ignoreErrorsRegex', []))
    warningExclusions = map(StringMatcher, config.get('ignoreWarnings', [])) + map(REMatcher, config.get('ignoreWarningsRegex', []))
    # Exclusions for coverity-expressions --> would need to change BaseConfiguration as well -- thartman - 2011.05.19
    # implement some unifying scheme for all types of warnings/errors/etc. 
    coverityExclusions = map(StringMatcher, config.get('ignoreCoverity', [])) + map(REMatcher, config.get('ignoreCoverityRegex', []))
    # coverityExclusions = ()
    
    def isIgnored(matchers, test):
        for m in matchers:
            if m(test):
                return True
        return False

    errorIgnored = lambda x: isIgnored(errorExclusions, x)
    warningIgnored = lambda x: isIgnored(warningExclusions, x)
    #coverityIgnored = lambda x: isIgnored(coverityExclusions, x)

    errorList = {}
    warningList = {}
    coverityList = {}

    def matchAny(s, rexps):
        return filter(None, map(lambda x: x.match(s), rexps))

    def matchAny(s, rexps):
        for x in rexps:
            if x.match(s):
                return True
        return False

    def updateDict(d, k, v):
        if k in d:
            d[k].append(v)
        else:
            d[k] = [v]

    def countLine(exps, dict, line, n):
        if matchAny(line, exps):
            updateDict(dict, line, n)
            return True
        return False

    for counter, line in enumerate(logData):
        countLine(regexError, errorList, line, counter) or countLine(regexWarning, warningList, line, counter) or countLine(regexCoverity, coverityList, line, counter)
    
    for k in filter(errorIgnored, errorList.keys()):
        del errorList[k]
    for k in filter(warningIgnored, warningList.keys()):
        del warningList[k]
    #for k in filter(coverityIgnored, coverityList.keys()):
    #    del coverityList[k]


    warningTypeCount = len(warningList.keys())
    errorTypeCount = len(errorList.keys())
    coverityTypeCount = len(coverityList.keys())

    getpackBegin = re.compile('GETPACK OUTPUT \(begin\)')
    getpackEnd = re.compile('GETPACK OUTPUT \(end\)')

    # preparing environment & getpack sections
    envBegin = 0
    envEnd = 0
    getBegin = 0
    getEnd = 0
    for lineNo in xrange(len(logData)):
        found = re.search(getpackBegin, logData[lineNo])
        if found:
            envEnd = lineNo - 1
            getBegin = lineNo + 2
    for lineNo in xrange(getBegin, len(logData)):
        found = re.search(getpackEnd, logData[lineNo])
        if found:
            getEnd = lineNo - 1

    # packageList generation
    packagesList = []
    packageStart = re.compile('^# Building package ([^ ]*) [[0-9]+/[0-9]+]$')
    for lineNo in xrange(len(logData)):
        found = re.search(packageStart, logData[lineNo])
        if found:
            pName = found.group(1)
            pBegin = lineNo
            pEnd = None
            packagesList.append([pName, pBegin-1, pEnd])
            if len(packagesList)>1 and packagesList[len(packagesList)-2][2] == None:
                packagesList[len(packagesList)-2][2] = lineNo-4
    packagesList[len(packagesList)-1][2] = len(logData)-1

#    print map(lambda x:x[1], packagesList)
    def packageInfo(lineNo):
        packageNo = 0
        for x in xrange(len(packagesList)):
            if packagesList[x][1] <= lineNo and packagesList[x][2] >= lineNo:
                packageNo = x
                break
        return packageNo

    # HTML generation
    htmlData.append("""<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>""")
    htmlData.append('<title>LogCheck for package %s </title>' % slotObj.getName())
    htmlData.append("""<style type="text/css">
pre { margin-top: 0px; }
.codeLink { text-decoration: none; color: blue; cursor:pointer; cursor:hand; }
.error { color: red; }
.warning { color: blue; }
a.error.highlight { background-color: #FFCCCC; }
a.warning.highlight { background-color: #CCDDFF; }
/*
li.errorli { display: none; }
li.warningli { display: none; }
*/
a.morebtn { color: red; cursor:pointer; cursor:hand; }
.packageLink { text-decoration: none; cursor:pointer; cursor:hand; color:blue; }
.odd { background-color: #FFFFFF; font-family: monospace; }
.even { background-color: #F0F0F0; font-family: monospace; }
</style>
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.5.1/jquery.min.js">
</script>
<script type="text/javascript">
""")
    htmlData.append('var logfile = "%s";' % rawLogName)
    htmlData.append('var logfileLinks = [')
    htmlData.append('{id: "env", f: %s, l: %s, desc: "Show details of environment"},' % (str(envBegin), str(envEnd),))
    htmlData.append('{id: "checkout", f: %s, l: %s, desc: "Show getpack log"},' % (str(getBegin), str(getEnd),))

    ## sort the lines in errorList & warningList  ~~> how to sort dictionary...?! ~~> get the list of keys or values and sort that?

    #packagesList.sort()
    for x in packagesList:
        htmlData.append('{id: "package%s", f: %s, l: %s, name: "%s"},' % (str(x[1]), str(x[1]), str(x[2]), x[0]))
    htmlData.append('];')
    htmlData.append('var codeLinks = [')
    ecounter = 1
#    for e in errorList:
#        if not e[1]: continue
#        for x in xrange(len(e[1])):
#            package = packagesList[packageInfo(e[1][x])]
#            htmlData.append('{id: "error%s", block: "package%s", line: %s},' % (str(ecounter), str(package[1]), str(e[1][x]) ))
#            ecounter += 1
    for e in errorList:
        if errorList[e].count == 0: continue
        errorList[e].sort()
        for x in errorList[e]:
            package = packagesList[packageInfo(x)]
            htmlData.append('{id: "error%s", block: "package%s", line: %s},' % (str(ecounter), str(package[1]), str(x) ))
            ecounter += 1
    wcounter = 1
#    for w in warningList:
#        if not w[1]: continue
#        for x in xrange(len(w[1])):
#            package = packagesList[packageInfo(w[1][x])]
#            htmlData.append('{id: "warning%s", block: "package%s", line: %s},' % (str(wcounter), str(package[1]), str(w[1][x]) ))
#            wcounter += 1
    for w in warningList:
        if warningList[w].count == 0: continue
        warningList[w].sort()
        for x in warningList[w]:
            package = packagesList[packageInfo(x)]
            htmlData.append('{id: "warning%s", block: "package%s", line: %s},' % (str(wcounter), str(package[1]), str(x) ))
            wcounter += 1
            
    coverityCounter = 1
    for iterator1 in coverityList:
        if coverityList[iterator1].count == 0: continue
        coverityList[iterator1].sort()
        for iterator2 in coverityList[iterator1]:
            package = packagesList[packageInfo(iterator2)]
            htmlData.append('{id: "coverity%s", block: "package%s", line: %s},' % (str(coverityCounter), str(package[1]), str(iterator2) ))
            coverityCounter += 1
    htmlData.append('];')
    htmlData.append('</script>')
    htmlData.append('<script type="text/javascript" src="http://lhcb-nightlies.web.cern.ch/lhcb-nightlies/js/logFileJQ.js"></script>')
    htmlData.append('</head><body>')
    htmlData.append('<h3>LogCheck for package %s on %s</h3>' % (tagName, os.environ.get('HOST')))
    htmlData.append('<p>Warnings : %s<br /> Errors   : %s<br /> Coverity messages   : %s<br /></p>' % (str(warningTypeCount), str(errorTypeCount), str(coverityTypeCount) ))

#    if len(ignoreErrors) and sum(ignoreErrorsCounters):
#        htmlData.append('<h3>Ignored errors:</h3>')
#    for w in xrange(len(ignoreErrors)):
#        if ignoreErrorsCounters[w]:
#            htmlData.append('<strong>%sx</strong>&nbsp;&rArr;&nbsp;%s<br />' % (str(ignoreErrorsCounters[w]), ignoreErrors[w]))
    if len(errorExclusions):
        htmlData.append('<h3>Ignored errors:</h3>')
    for e in errorExclusions:
        if e.count > 0: htmlData.append(e.html())

#    if len(ignoreWarnings) and sum(ignoreWarningsCounters):
#        htmlData.append('<h3>Ignored warnings:</h3>')
#    for w in xrange(len(ignoreWarnings)):
#        if ignoreWarningsCounters[w]:
#            htmlData.append('<strong>%sx</strong>&nbsp;&rArr;&nbsp;%s<br />' % (str(ignoreWarningsCounters[w]), ignoreWarnings[w]))
    if len(warningExclusions):
        htmlData.append('<h3>Ignored warnings:</h3>')
    for w in warningExclusions:
        if w.count > 0: htmlData.append(w.html())

    if len(coverityExclusions):
        htmlData.append('<h3>Ignored warnings:</h3>')
    for iterator1 in coverityExclusions:
        if iterator1.count > 0: htmlData.append(iterator1.html())
        
    htmlData.append('<h3>Shortcuts:</h3>')
    htmlData.append('<ul>')
    if errorTypeCount: htmlData.append('<li><a href="#summary_errors">Summary of errors</a>')
    if warningTypeCount: htmlData.append('<li><a href="#summary_warnings">Summary of warnings</a>')
    if coverityTypeCount: htmlData.append('<li><a href="#summary_coverity">Summary of Coverity messages</a>')
    htmlData.append('<li><a href="#environment">Environment</a>')
    htmlData.append('<li><a href="#checkout">Checkout (getpack) log</a>')
    htmlData.append('<li><a href="#packages_list">List of packages (logs)</a>')
    htmlData.append('<li><a href="#environment" onclick="javascript:$(\'.packageLink\').click();">Show all (may take long time)</a>')
    htmlData.append('</ul>')
    ecounter = 1
    if errorTypeCount:
        htmlData.append('<h3 id="summary_errors">Summary of errors:</h3><hr />')
#        for e in errorList:
#            if not e[1]: continue
#            htmlData.append('<ul class="errorul">')
#            for x in xrange(len(e[1])):
#                context = ''
#                for cx in xrange(-4,0):
#                    context += '%s\n' % cgi.escape(logData[e[1][x]+cx])
#                context += '<strong>%s</strong>\n' % cgi.escape(logData[e[1][x]])
#                package = packagesList[packageInfo(e[1][x])]
#                htmlData.append('<li class="errorli"><a class="codeLink" id="error%s"><pre>%s</pre></a></li>' % ( str(ecounter), context, ))
#                ecounter += 1
#            htmlData.append('</ul><hr />')
        for e in errorList:
            if not e[1]: continue
            htmlData.append('<ul class="errorul">')
            for x in errorList[e]:
                context = ''
                for cx in xrange(-4,0):
                    context += '%s\n' % cgi.escape(logData[x+cx])
                context += '<strong>%s</strong>\n' % cgi.escape(logData[x])
                package = packagesList[packageInfo(x)]
                htmlData.append('<li class="errorli"><a class="codeLink" id="error%s"><pre>%s</pre></a></li>' % ( str(ecounter), context, ))
                ecounter += 1
            htmlData.append('</ul><hr />')
    wcounter = 1
    if warningTypeCount:
        htmlData.append('<h3 id="summary_warnings">Summary of warnings:</h3><hr />')
#        for w in warningList:
#            if not w[1]: continue
#            htmlData.append('<ul class="warningul">')
#            for x in xrange(len(w[1])):
#                context = ''
#                for cx in xrange(-4,0):
#                    context += '%s\n' % cgi.escape(logData[w[1][x]+cx])
#                context += '<strong>%s</strong>\n' % cgi.escape(logData[w[1][x]])
#                package = packagesList[packageInfo(w[1][x])]
#                htmlData.append('<li class="warningli"><a class="codeLink" id="warning%s"><pre>%s</pre></a></li>' % ( str(wcounter), context, ))
#                wcounter += 1
#            htmlData.append('</ul><hr />')
        for w in warningList:
            if warningList[w].count == 0: continue
            htmlData.append('<ul class="warningul">')
            for x in warningList[w]:
                context = ''
                for cx in xrange(-4,0):
                    context += '%s\n' % cgi.escape(logData[x+cx])
                context += '<strong>%s</strong>\n' % cgi.escape(logData[x])
                package = packagesList[packageInfo(x)]
                htmlData.append('<li class="warningli"><a class="codeLink" id="warning%s"><pre>%s</pre></a></li>' % ( str(wcounter), context, ))
                wcounter += 1
            htmlData.append('</ul><hr />')

            
    coverityCounter = 1
    if coverityTypeCount:
        htmlData.append('<h3 id="summary_coverity">Summary of Coverity messages:</h3><hr />')
        for iterator1 in coverityList:
            if coverityList[iterator1].count == 0: continue
            htmlData.append('<ul class="coveritygul">')
            for iterator2 in coverityList[iterator1]:
                context = ''
                for iterator3 in xrange(-4,0):
                    context += '%s\n' % cgi.escape(logData[iterator2+iterator3])
                context += '<strong>%s</strong>\n' % cgi.escape(logData[iterator2])
                package = packagesList[packageInfo(iterator2)]
                htmlData.append('<li class="warningli"><a class="codeLink" id="warning%s"><pre>%s</pre></a></li>' % ( str(coverityCounter), context, ))
                coverityCounter += 1
            htmlData.append('</ul><hr />')
    htmlData.append('<div id="logfile"/>')
    htmlData.append('</body>')
    htmlData.append('</html>')

    # creating HTML file
    htmlFile = codecs.open(os.path.join(logDir, htmlLogName), 'w', 'utf-8')
    htmlFile.write('\n'.join(htmlData))
    htmlFile.close()

    # creating LOG file
    rawLog = codecs.open(os.path.join(logDir, '%s.log' % rawLogName), 'w', 'utf-8')
    rawLog.write('\n'.join(logData))
    rawLog.close()

    if createSummaryLogFile:
        summaryFileName = slotObj.getName() + "." + time.strftime("%a") + "_" + tagName+"-"+platform+"-log.summary"
        summaryFile = open( os.path.join(logDir, summaryFileName), 'w')
        now = time.time()
        summaryFile.write(str(now) + " (" + time.ctime(now) + ") " + slotObj.getName() + " " + tagName + " " + platform + "\n")
        summaryFile.write(str(warningTypeCount) + ", " + str(errorTypeCount) + ", " + str(0) + ", " + str(0) + "\n")
        summaryFile.close()

    # save data to sqlite3 database: '/afs/cern.ch/lhcb/software/nightlies/db/nightlies.results'
    try:
        import sqlite3
        import datetime
        db_date = datetime.date.today().strftime('%Y-%m-%d')
        db_time = datetime.datetime.now().strftime('%H:%M:%S.000')
        db_slot = slotObj.getName()
        db_plat = plat
        db_proj = pkgName.split('_')[0]
        db_buildlog = db_slot + "." + datetime.date.today().strftime("%a") + "_" + pkgName+"-"+plat+"-log.html"
        db_testlog = db_slot + "." + datetime.date.today().strftime("%a") + "_" + pkgName+"-"+plat+"-qmtest/index.html"
        dbconn = sqlite3.connect('/afs/cern.ch/lhcb/software/nightlies/db/nightlies.results')
        dbc = dbconn.cursor()
        dbc.execute('select count(*) from results where date=? and slot=? and platform=? and project=?', (db_date, db_slot, db_plat, db_proj))
        if dbc.fetchall()[0][0] == 0: # no record, so insert the new one
            dbc.execute('insert into results (date, time, slot, platform, project, warnings, errors, makeerrors, cmterrors, tests_failed, tests_untested, tests_passed, buildlog, testlog) values (?,?,?,?,?,?,?,?,?,NULL,NULL,NULL,?,NULL)', (db_date, db_time, db_slot, db_plat, db_proj, warningTypeCount, errorTypeCount, 0, 0, db_buildlog))
            dbconn.commit()
            dbc.close()
        else: # record is there, so update
            dbc.execute('update results set warnings=?, errors=?, makeerrors=?, cmterrors=?, buildlog=? where date=? and slot=? and platform=? and project=?', (warningTypeCount, errorTypeCount, 0, 0, db_buildlog, db_date, db_slot, db_plat, db_proj))
            dbconn.commit()
            dbc.close()
    except:
        pass
    return (warningTypeCount, errorTypeCount, 0, 0,coverityTypeCount)

##########################################################################################

class LHCbProjectBuilder(object):
    def canBuild(cls, project):
        from LbConfiguration.Project import project_names
        return (project in project_names)
    canBuild = classmethod(canBuild)

    def translatePath(self, linuxPath, destPlatform = None):
        from configuration import translatePath
        return translatePath(linuxPath, destPlatform)

    def __init__(self, slot,thread,load, proj, tag, platIn, conf, thread_counter, tpInstaller, wu, sem, tagOrder, host='', port=''):
        print "[LHCb] __init__\n"
        self.localDirsOnWindows = {}
        self.localDirsOnWindows['Z:\\cern.ch\\user\\k\\kkruzele\\cmtuser\\LBSCRIPTS\\LBSCRIPTS_v99r0\\InstallArea\\win32_vc71\\lib'] = '' # delete from the path
        self.localDirsOnWindows['Z:\\LOCALDRIVE\\C\\'] = 'C:\\'
        self.localDirsOnWindows['Z:\\LOCALDRIVE\\D\\'] = 'D:\\'
        self.localDirsOnWindows['Z:\\LOCALDRIVE\\E\\'] = 'E:\\'
        self.localDirsOnWindows['Z:\\LOCALDRIVE\\F\\'] = 'F:\\'
        self.localDirsOnWindows['Z:\\cern.ch\\sw\\lcg\\external'] = 'C:\\local\\external'
        self.localDirsOnWindows['Z:\\cern.ch\\sw\\contrib\\CMT\\v1r20p20070208'] = 'C:\\local\\releases\\CMT\\v1r20p20070208'
        self.localDirsOnWindows['Z:\\cern.ch\\sw\\Gaudi\\releases'] = 'C:\\local\\Gaudi-releases'
        self.localDirsOnWindows['Z:\\cern.ch\\sw\\lcg\\app\\releases\\LCGCMT'] = 'C:\\local\\LCGCMT'
        self.localDirsOnWindows['Z:\\cern.ch\\sw\\lcg\\app\\nightlies\\scripts'] = 'C:\\local\\LCG-nightlies'
        self.localDirsOnWindows['Z:\\cern.ch\\sw\\lcg\\external'] = 'C:\\local\\external;D:\\local\\lib\\lcg\\external'
        self.localDirsOnWindows['Z:\\cern.ch\\lhcb\\scripts'] = 'C:\\local\\LHCb-scripts;D:\\local\\lib\\scripts'
        self.localDirsOnWindows['Z:\\LOCALCOPY\\releases'] = 'C:\\local\\releases;D:\\local\\lib\\lhcb'

        self.cmtCommand = 'cmt -disable_warnings'
        self.getpackCommand = 'getpack --no-config --batch -p anonymous'
        self.pythonCommand = 'python '
        # moved coverityPath setting into coverity check in def build  -- thartman 2011.05.11

        self.status = 0

        from LbConfiguration.Project import project_names as listOfProjectNames
        self.projectNamesDict = {}
        for p in listOfProjectNames:
            self.projectNamesDict[p.upper()] = p

        self.slot = slot
        self.minusj = 50 #thread
        self.minusl = 15 #load
        self.slotName = slot.getName()
        self.project = slot.getProject(proj)
        self.projName = self.projectNamesDict[proj.upper()]
        self.tagName = tag
        self.disabled = self.project.getDisabledFlag()
        self.config = conf
        self.plat = platIn
        if self.plat.find('win') != -1:
            self.systemType = 'windows'
        else:
            self.systemType = 'linux'
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
        return os.system(cmd + log)

    def cleanBuildDir(self, projectName):
        def _fixPerms(path):
            """ change file/dir permissions to be able to remove them """
            try:
                from stat import S_IWRITE, ST_MODE, S_IMODE, S_IRWXU
                mode = S_IMODE(os.stat(path)[ST_MODE])
                if (mode & S_IRWXU) != S_IRWXU:
                    os.chmod(path, mode | S_IRWXU)
##             except OSError as (errno, stderror):
##                 self.system('echo "warning: OSError while changing permissions for removing: %s -- %s"' % (errno,stderror))
##                 pass
            except OSError:
                self.system('echo "warning: OSError while changing permissions for removing"' )
                pass
        
        if '%CMTCONFIG%' in self.slot._buildDir:
            _fixPerms((os.sep.join([self.slot.buildDir(), projectName])))
            shutil.rmtree(os.sep.join([self.slot.buildDir(), projectName]), ignore_errors=True)
            if not os.path.exists(self.slot.buildDir()):
                os.makedirs(self.slot.buildDir())
            os.makedirs(os.sep.join([self.slot.buildDir(), projectName]))

    def buildProject(self):
        if self.disabled:
            print "[LHCb] project skipped (disabled=True)"
            return
        print "[LHCb] buildProject: start"
        self.cleanBuildDir(self.project.getName())
        cfgFile = os.path.sep.join([self.slot.releaseDir(), 'configuration.xml'])
        if self.systemType != 'windows' and not os.path.exists(cfgFile):
            file(cfgFile, 'w').write(self.configContents)
            if os.path.exists(self.configurationHistoryPath):
                file(os.path.sep.join([self.configurationHistoryPath, "%s.%s.xml" % (self.day, self.slotName)]), 'w').write(self.configContents)
        #TODO: --> should be written to AFS by server, not by client - then clients would do not need AFS access.
        #TODO: --> reload configuration if it's already in the releaseDir
        self.checkout()
        self.build()
        if not self.coverityBuild: self.test()
        if not self.coverityBuild: self.docs()
        #if not self.coverityBuild: self.install()
        self.install()
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

    @ignore_exception
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
###
        if not os.path.exists(self.generatePath(self.slot, self.project, 'TAG', self.project.getName(), beforeRename=True)):
            os.makedirs(self.generatePath(self.slot, self.project, 'TAG', self.project.getName().upper(), beforeRename=True))
            if self.systemType != 'windows':
                if not os.path.exists(self.project.getName().upper()):
                    os.symlink(self.project.getName(), self.project.getName().upper())
        os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.project.getName(), beforeRename=True))
###
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

        os.chdir(self.translatePath(self.slot.buildDir()))
        self.getpackget(self.project.getName(), self.project.getVersion(), project=True)
        #if os.path.exists(self.project.getName().upper()) and self.project.getName() != self.project.getName().upper():
        #    print "renaming from: %s, to: %s" % (self.project.getName().upper(), self.project.getName())
        #    os.rename(self.project.getName().upper(), self.project.getName())
        #    print "renaming from: %s, to: %s - DONE" % (self.project.getName().upper(), self.project.getName())

        os.system('ls %s' % os.path.abspath(os.sep.join([self.generatePath(self.slot, self.project, 'TAG', self.project.getName()), '..'])))

        if self.project.getRename() is not None:
            os.chdir(os.path.abspath(os.sep.join([self.generatePath(self.slot, self.project, 'TAG', self.project.getName()), '..'])))
            os.rename(self.project.getTag(),self.project.getRename())

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

        renameProjectObject = None
        if self.project.getRename() is not None:
            renameProjectObject = self.project
        if len(self.project.getChanges()) > 0 or renameProjectObject is not None:
            # apply changes to the main `requirements` file
            for change in self.project.getChanges().items():
                #cha = changedPackageName(change[0])
                cha = change[0]
                if cha in sysPackageRF.keys():
                    sysPackageRF[cha][0] = change[1]
                changesMade[change[0]] = change[1]
            self.changeRequirementsFile(os.path.join(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName), 'requirements'), sysPackageRF, self.project.getAddons(), self.project.getRemoves(), renameProjectObj=renameProjectObject)

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
        #for p in self.slot.getProjects(hideDisabled=True):
        for p in self.slot.getProjects(hideDisabled=False): # also apply changes from disabled projects!
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
        for p in self.slot.getProjects(hideDisabled=True):
            if p.getName().upper() == 'LCGCMT': continue
            if p.getName().upper() == self.project.getName().upper(): break
            pklFile = file(os.path.join(self.generatePath(self.slot, p, 'TAG', self.projName), 'changesMade-' + os.environ['CMTCONFIG'] + '.pkl'), 'rb')
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

    @ignore_exception
    def build(self):
        print "[LHCb] build"
        
        if 'COVERITY' in os.environ.get('CMTEXTRATAGS',''):
            self.coverityBuild = True

            self.coverityPath = ''
            if os.path.exists('/build/coverity/static-analysis/bin/'):
                self.coverityPath = '/build/coverity/static-analysis/bin/'
            elif os.path.exists('/build/axel/Coverity/cov-sa/bin/'):
                self.coverityPath = '/build/axel/Coverity/cov-sa/bin/'
            else:
                print 'Coverity: path to coverity does not exists, set it anyway and will probably fail. Please check on host %s' % os.uname()[1]
                self.coverityPath = '/build/coverity/static-analysis/bin/'
            print 'Coverity: Path set to %s' % self.coverityPath


            newExtraTags = os.environ.get('CMTEXTRATAGS','').replace('COVERITY','').replace(',,', ',')
            if newExtraTags == ',': newExtraTags = ''
            os.environ['CMTEXTRATAGS'] = newExtraTags
            coverityDir = os.sep.join([self.slot.buildDir(), self.projName])
            coverityDirINT = os.sep.join(['/dev/shm/lhcbsoft',self.slotName,self.projName,'INT'])
            coverityDirINTDisk = os.sep.join([self.slot.buildDir(), self.projName,'INT'])
            derivedModelsDir = os.sep.join([self.slot.buildDir(), 'COVERITY_DERIVED_MODELS'])
        else:
            self.coverityBuild = False
        
        os.environ['CMTCONFIG'] = self.plat
        logdir = os.sep.join([self.generatePath(self.slot, self.project, 'TAG', self.projName), 'logs'])
        if not os.path.exists(logdir):
            os.makedirs(logdir)

        self.disableLCG_NIGHTLIES_BUILD()
        self.setCMTEXTRATAGS(self.slotName)
        self.setCmtProjectPath(self.slot)
        
        if '-icc' in os.environ['CMTCONFIG']:
            self.iccSetup()
            os.environ['CMTEXTRATAGS'] = os.environ['CMTEXTRATAGS'].replace('use-distcc,','').replace('use-distcc','')

        if os.path.exists(os.sep.join([self.generatePath(self.slot, self.project, 'TAG', self.projName), 'build.'+os.environ.get('CMTCONFIG','')+'.done'])):
            print 'Build already done for ' + os.environ.get('CMTCONFIG', '') + '. Skipped.'
            return
        else:
            # removing rubbish from log file
            os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.projName))
            try:
                logFileToBeRemoved = file(os.sep.join(['logs', os.environ.get('CMTCONFIG', '')])+'.log', 'w')
                logFileToBeRemoved.close()
##             except Exception as (errno, strerror):
##                 self.system('echo "warning: Error while log file handling: %s -- %s"' % errno,strerror)
##                 print err
##                 pass
            except Exception:
                self.system('echo "warning: Error while log file handling"')
                print err
                pass

        self.changeEnvVariables()

        self.outputFile = os.sep.join([logdir, os.environ.get('CMTCONFIG', '') + '.log'])
        os.chdir(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName))
        self.system('echo "' + '*'*80 + '"')
        self.system('echo "'+ time.strftime('%c', time.localtime()) +'"')
        self.system('echo "Nightlies:        '+os.path.dirname(__file__)+'"')
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
        self.system('echo "* GETPACK OUTPUT (begin)"')
        self.system('echo "' + '*'*80 + '"')
        if os.path.exists(os.path.join(self.generatePath(self.slot, self.project, 'TAG', self.projName), "checkout.log")):
            checkoutLogData = map(lambda x: x.replace('\n',''),file(os.path.join(self.generatePath(self.slot, self.project, 'TAG', self.projName), "checkout.log"), 'r').readlines())
            for x in checkoutLogData:
                self.system('echo "%s"' % x)
            self.system('echo "' + '*'*80 + '"')
        self.system('echo "* GETPACK OUTPUT (end)"')
        self.system('echo "' + '*'*80 + '"')
        os.chdir(os.sep.join(['..']*2))
        if 'CMTEXTRATAGS' in os.environ and len(os.environ['CMTEXTRATAGS'])>0:
            os.environ['CMTEXTRATAGS'] = 'no-pyzip,'+os.environ.get('CMTEXTRATAGS', '')
        else:
            os.environ['CMTEXTRATAGS'] = 'no-pyzip'
            
        if self.coverityBuild is True:
            #make list of derived model files in 'derivedModelsDir':
            if os.path.exists(derivedModelsDir):
                derivedModelsList = [x for x in os.listdir(derivedModelsDir) if x.endswith('.xmldb')]
            else:
                derivedModelsList = []
            #prepend each with '--derived-model-file ' to add to a command line
            derivedModelsList = ''.join(['--derived-model-file %s ' % os.path.sep.join([derivedModelsDir, x]) for x in derivedModelsList])

            #prepare <project_version>, for example: gaudi_trunk, or lhcb_trunk
            covName = '%s_trunk' % self.projName.lower()

            self.system('echo "(1) ***************************************************"')
            self.system('echo "(1) Start Coverity"')

            ## Create directory in RAM
            #if os.path.exists(coverityDirINT):
            #    self.system('echo "REMOVE existing intermediate directory %s"' % coverityDirINT)
            #    shutil.rmtree(coverityDirINT, ignore_errors=True)
            self.system('echo "(1) Created intermediate directory %s"' % (coverityDirINT))
            os.makedirs(coverityDirINT)
            
            self.system('echo "(1) cov-build --dir %s make -j 20 -l 16"' % (coverityDirINT))
            #########################################
            ###returnCode = self.system('%scov-build --dir %s/INT make -j 20 -l 16' % (self.coverityPath, coverityDir))
            ###self.system('echo "(1) RETURN CODE: %s"' % str(returnCode))
            ### ---> #[WARNING] Build command make -j 20 -l 16 exited with code 2. Please verify that the build completed successfully. --> 0
            from subprocess import Popen, PIPE
            p = Popen('%scov-build --dir %s make -k -j 20 -l 16 Package_failure_policy=ignore logging=enabled' % (self.coverityPath, coverityDirINT), shell = True, stdout = PIPE, stderr = PIPE)
            #returnCode = p.wait()
            #p_stdout = ''.join([l for l in p.stdout])
            #p_stderr = ''.join([l for l in p.stderr])
            p_stdout, p_stderr = p.communicate()
            returnCode = p.returncode
            time.sleep(10)
            if returnCode == 0:
                if re.search("Build command.*exited with code [0-9]*. Please verify that the build completed successfully.", p_stdout) is not None or re.search("Build command.*exited with code [0-9]*. Please verify that the build completed successfully.", p_stderr):
                    returnCode = 1
            print p_stdout
            print p_stderr
            #########################################
            if returnCode == 0:

##                 if os.path.exists('../../strip-path.list'):
##                     prev = ''.join([x.replace('\n','') for x in file('../../strip-path.list').readlines()])
##                 else:
##                     prev = ''
##                 file('../../strip-path.list','w').write('%s --strip-path %s/ --strip-path %s/' % (prev, self.generatePath(self.slot, self.project, 'TAG', self.projName),self.generatePath(self.slot, self.project, 'TAG', self.projName.upper()) ) )

                if os.path.exists('%s/../strip-path.list' % (coverityDir)):
                    prev = ''.join([x.replace('\n','') for x in file('%s/../strip-path.list' % (coverityDir)).readlines()])
                else:
                    prev = ''
                file('%s/../strip-path.list' % (coverityDir),'w').write('%s --strip-path %s/ --strip-path %s/' % (prev, self.generatePath(self.slot, self.project, 'TAG', self.projName),self.generatePath(self.slot, self.project, 'TAG', self.projName.upper()) ) )

                
                self.system('echo "(2) ***************************************************"')
                ### cov-analyze via analyze-submit.sh
                self.system('echo "(2) starting cov-analyze with "')
                self.system('echo "(2) /build/coverity/analyze-submit.sh %s %s"' % (coverityDirINT,derivedModelsDir))
                self.system('/build/coverity/analyze-submit.sh %s %s' % (coverityDirINT,derivedModelsDir) ) 
                ### cov-analyze directly
                #self.system('echo "%scov-analyze --dir %s  --enable-callgraph-metrics --enable-parse-warnings --all %s"' % (self.coverityPath, coverityDirINT, derivedModelsList))
                #self.system('%scov-analyze --dir %s  --enable-callgraph-metrics --enable-parse-warnings --all %s' % (self.coverityPath, coverityDirINT, derivedModelsList))
                self.system('echo "(3) ***************************************************"')
                # # test if cov-iternal-analyze-c process has already written anything to the cache model file
                # # /dev/shm/lhcbsoft/lhcb-coverity/Gaudi/INT/c/output/.cache/models
                #waitTimer = 0
                #while (os.path.exists(os.sep.join([coverityDirINT,'c/output/.cache/models'])) and os.path.getsize(os.sep.join([coverityDirINT,'c/output/.cache/models'])) < 4097 and waitTimer < 15):
                #    time.sleep(60)
                #    waitTimer += 1
                #    self.system('echo "(3) %s not ready; wait 60 seconds"' % (os.sep.join([coverityDirINT,'c/output/.cache/models'])))
                #if waitTimer==15:
                #    self.system('echo "(3) %s not ready after 15 minutes; gave up waiting"' % (os.sep.join([coverityDirINT,'c/output/.cache/models'])))
                    
                self.system('echo "(3) cov-collect-models --dir %s -of %s/%s.xmldb"' % (coverityDirINT, derivedModelsDir, covName))
                self.system('%scov-collect-models --dir %s -of %s/%s.xmldb' % (self.coverityPath ,coverityDirINT, derivedModelsDir, covName))
                if os.path.exists('%s/%s_trunk.xmldb.lock' % (derivedModelsDir,covName)):
                    self.system('rm %s/%s_trunk.xmldb.lock' % (derivedModelsDir,covName))
                self.system('echo "(4) ***************************************************"')
                pp = open("/afs/cern.ch/user/l/lhcbsoft/private/init").readline().strip()
                ##self.system('echo "(4) export COVERITY_PASSPHRASE=...%s... ; cov-commit-defects --host lhcb-coverity.cern.ch --port 8080 --user admin --stream %s `cat %s/../strip-path.list` `cat %s/c/output/commit-args.txt`"' % (str(len(pp)), covName, coverityDir, coverityDirINT))
                ##self.system('export COVERITY_PASSPHRASE=%s ; cov-commit-defects --host lhcb-coverity.cern.ch --port 8080 --user admin --stream %s `cat %s/../strip-path.list` `cat %s/c/output/commit-args.txt`"' % (str(len(pp)), covName, coverityDir, coverityDirINT))
##                 if os.path.exists('../../strip-path.list'):
##                     stripPathString = ' '.join([l.strip() for l in open("../../strip-path.list")])
##                 else:
##                     stripPathString = ''
##                 if os.path.exists('%s/INT/c/output/commit-args.txt' % coverityDir):
##                     commitArgsString = ' '.join([l.strip() for l in open("%s/INT/c/output/commit-args.txt" % coverityDir)])
##                 else:
##                     commitArgsString = ''
                if os.path.exists('%s/../strip-path.list' % (coverityDir)):
                    stripPathString = ' '.join([l.strip() for l in open("%s/../strip-path.list" % coverityDir)])
                else:
                    stripPathString = ''
                if os.path.exists('%s/c/output/commit-args.txt' % coverityDirINT):
                    commitArgsString = ' '.join([l.strip() for l in open("%s/c/output/commit-args.txt" % coverityDirINT)])
                else:
                    commitArgsString = ''

                self.system('echo "(4) %scov-commit-defects --host lhcb-coverity.cern.ch --port 8080 --user admin --stream %s %s %s"' % (self.coverityPath, covName, stripPathString, commitArgsString))
                self.system('export COVERITY_PASSPHRASE=%s ; %scov-commit-defects --host lhcb-coverity.cern.ch --port 8080 --user admin --stream %s %s %s' % (pp, self.coverityPath, covName, stripPathString, commitArgsString))
                self.system('echo "(5) ***************************************************"')
                self.system('echo "(5) compare checked and existing source files"')
                #self.compareCoverity()
                self.system('echo "(6) ***************************************************"')
            else:
                self.system('echo "(1) Coverity error: build finished with errors, Coverity analysis skipped."')
            self.system('echo "(-) ***************************************************"')
        else:
            self.system('make -k -j%s -l%s Package_failure_policy=ignore logging=enabled > make.%s.log' % (str(self.minusj), str(self.minusl), str(os.environ['CMTCONFIG']),) )
        os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.projName))
        logFiles = []
        for r, d, f in os.walk("."):
            if r == ".": continue
            elif "cmt" in d: d[:] = ["cmt"]
            else:
                if "build.%(CMTCONFIG)s.log" % os.environ in f:
                    logFiles.append((os.stat(os.path.join(r, "build.%(CMTCONFIG)s.log" % os.environ)).st_mtime, os.path.join(r, "build.%(CMTCONFIG)s.log" % os.environ)))
        logFiles.sort()
        sys.stdout.flush()
        sys.stderr.flush()
        for x in logFiles:
            self.system('echo "' + '-'*80 + '"')
            self.system('echo "Logfile: ' + x[1] + '"')
            self.system('echo "' + '-'*80 + '"')
            self.system('cat ' + x[1])
            sys.stdout.flush()
            sys.stderr.flush()
        self.system('echo "' + '*'*80 + '"')
        self.system('echo "Build finished: '+ time.strftime('%c', time.localtime()) +'"')

        os.chdir(self.generatePath(self.slot, self.project, 'TAG', self.projName))
        buildFlagFile = open('./build.'+os.environ.get('CMTCONFIG','')+'.done', 'w')

        if self.coverityBuild is True:
            self.system('echo " (7) Starting to clean intermediate Coverity directories in the RAM: %s"' % (coverityDirINT))
            if self.coverityBuild:
                self.system('echo " (7) Move intermediate Coverity directory from %s in RAM to disk %s"' % (coverityDirINT,coverityDirINTDisk))
                if os.path.exists(coverityDirINT):
                    self.system('echo (7) "Coverity message: move intermediate Coverity directory from %s to %s"' % (coverityDirINT,coverityDirINTDisk))
                    shutil.move(coverityDirINT,coverityDirINTDisk)
                else:
                    self.system('echo (7) "Coverity warning: did not find intermediate Coverity directory in the RAM: %s"' % coverityDirINT)
            self.system('echo "(-) ***************************************************"')

        return 0

    def compareCoverity(self):
        """ Compare for each project the source files checked by Coverity
        with the source files found in the project head
        some mismatching files are probably not a problem but just unused remnants
        """
        coverityDir = os.sep.join([self.slot.buildDir(), self.projName])
        self.system('echo "coverity directory %s"' % (coverityDir))
        coverityProject = self.project.getName()
        self.system('echo "coverity project %s"' % (coverityProject))
        #coverityDirINT = '/dev/shm/lhcbsoft/INT'
        coverityDirINT = os.sep.join(['/dev/shm/lhcbsoft',self.slotName,self.projName,'INT'])

        coverityLogPath = ''
        if os.path.exists(coverityDirINT):
            coverityLogPath = (os.path.join(coverityDirINT,'build-log.txt'))
        elif os.path.exists(os.path.join(coverityDir,'INT','build-log.txt')):
            coverityLogPath = os.path.join(coverityDir,'INT','build-log.txt')
        else:
           self.system('echo "Coverity error: Did not find build log in %s nor in %s, please check"' % (coverityDirINT,os.path.join(coverityDir,'INT','build-log.txt')))
            
        self.system('echo "Coverity message: search for build-log %s"' % (coverityLogPath))

        try:
            if os.path.exists(coverityLogPath):
                self.system('echo "Coverity message: compare analyzed and existing source files from %s"' % (coverityLogPath))
                coverityBuildLogFile = open(coverityLogPath,'r') 
                checkedFileList = []
                        
                for line in coverityBuildLogFile:
                    if line.find("Emit") > -1:
                        checkedFileList.append(line.split("'")[1])

                coverityBuildLogFile.close()
                if len(checkedFileList)>0:
                    coveritySourcePath = os.path.join(coverityPath,coverityProject,coverityProject.upper()+'_HEAD')
                    DiffFileSet = []

                    if os.path.exists(coveritySourcePath):
                        sourceFileList = []
                        sourceFileCounter = 0
            
                        for PathIterator in os.walk(coveritySourcePath):
                            for FileIterator in PathIterator[2]:
                                if FileIterator.endswith(".cpp"):
                                    sourceFileList.append(os.path.join(PathIterator[0],FileIterator))
                                    sourceFileCounter += 1

                        DiffFileSet = set(checkedFileList).symmetric_difference(set(sourceFileList))

                        if len(DiffFileSet)>0:
                            self.system('echo "Coverity warning: checked source files %i ; existing source files %i ; difference %s"' % (len(checkedFileList), len(sourceFileList), len(DiffFileSet)))
                            self.system('echo "Coverity differing files start:"')
                            for iterator1 in DiffFileSet:
                                self.system('echo "print iterator1"')
                                self.system('echo "Coverity differing files end:"')
                        else:
                            self.system('echo "Coverity message: all existing source files have been checked"')
                    else:
                        self.system('echo "Coverity error: Did not find source path %s"' % (coveritySourcePath))
                else:
                    self.system('echo "Coverity error: no checked files were found in %s"' % (coverityLogPath))
            else:
                self.system('echo "Coverity error: Did not find build log %s"' % (coverityLogPath))
##         except IOError as (errno, strerror):
##              self.system('echo "Coverity error: IOError while comparing checked and existing files: %s -- %s"' (errno, strerror))
##         except Exception as (errno, strerror):
##              self.system('echo "Coverity error: unexpected error. Break with exception: %s -- %s' % (errno,strerror))                
##              pass
        except IOError:
            self.system('echo "Coverity error: IOError while comparing checked and existing files"' )
        except Exception:
            self.system('echo "Coverity error: unexpected error. Break with exception"')                
            pass
        

    @ignore_exception
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

    def sendMails(self, slot, project, platform, subject, message):
        import smtplib
        project = project.getName()
        if project in self.config._mailTo and len(self.config._mailTo[project]) > 0:
            mailList = self.config._mailTo[project]
        elif project.lower() in self.config._mailTo and len(self.config._mailTo[project.lower()]) > 0:
            mailList = self.config._mailTo[project.lower()]
        else:
            return
        mailFrom = self.config._generalConfig['mailfrom']
        mailHeaders = 'From: %s\nTo:%s\nSubject: %s\n\n' % (mailFrom, ';'.join(mailList), subject)
        s = smtplib.SMTP('localhost')
        s.sendmail(mailFrom, mailList, mailHeaders + message)
        s.quit()

    @fork_call
    def install(self):
        print "[LHCb] install"
        #import checkLogFiles
        #import LHCbCheckLogFiles as checkLogFiles
        #import LHCbCheckLogFiles3 as checkLogFiles
        ### temporary fix ###
        from LbConfiguration.Platform import binary_list
        binary_list = binary_list + ["x86_64-slc5-icc11-dbg"]
        import LbConfiguration
        LbConfiguration.Platform.binary_list = binary_list
        #####################
        from LbConfiguration.Platform import pathBinaryMatch
        from LbConfiguration.Platform import pathSharedMatch

        copyLogs = True
        #nWarn, nErr, nMkErr, nCMTErr = checkLogFiles.checkBuildLogs(self.slot, self.project, self.day, self.plat, self.config._generalConfig)
        nWarn, nErr, nMkErr, nCMTErr, nCoverityMessages = checkBuildLogs(self.slot, self.project, self.day, self.plat, self.config._generalConfig)

        if self.systemType == 'windows':
            copyNotShared = False
            copyShared = False
            print "Windows platform. Installation skipped."
        elif nErr == 0: #  + nMkErr + nCMTErr
            copyNotShared = True
            copyShared = True
            print "No errors found during the build. Shared and common files will be copied."
            print "(%s)" % ( str(nWarn)+', '+str(nErr)+', '+str(nMkErr)+', '+str(nCMTErr) )
        else:
            copyNotShared = True
            copyShared = False
            print "Errors found during the build. Only not shared files will be copied."
            print "(%s)" % ( str(nWarn)+', '+str(nErr)+', '+str(nMkErr)+', '+str(nCMTErr) )

        self.setCmtProjectPath(self.slot)
        self.changeEnvVariables()
        self.disableLCG_NIGHTLIES_BUILD()
        self.setCMTEXTRATAGS(self.slotName)
        os.chdir(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName))

        if self.project.getRename() is not None:
            tagName = self.project.getRename()
        else:
            tagName = self.project.getTag()

        if self.systemType == "windows":
            #writing logs directly to Nightlies server (for Windows):
            try:
                import xmlrpclib
                server = xmlrpclib.Server("http://lxbuild135.cern.ch:61007") #production is: 61007, restart is: 61008
                # isDone file
                isDoneFrom = os.path.join(self.slot.buildDir(), 'isDone-'+os.environ['CMTCONFIG'])
                if os.path.exists(isDoneFrom):
                    logData = ''.join(file(isDoneFrom, 'r').readlines())
                    server.saveLog(self.day, self.slot.getName(), tagName, self.plat, 0, logData)
                # html log file
                logData = ''.join(file(os.path.join(self.slot.buildDir(), 'www', '%s.%s_%s-%s-log.html' % (self.slot.getName(), self.day, tagName, self.plat)), 'r').readlines())
                server.saveLog(self.day, self.slot.getName(), tagName, self.plat, 1, logData)
                # summary
                logData = ''.join(file(os.path.join(self.slot.buildDir(), 'www', '%s.%s_%s-%s-log.summary' % (self.slot.getName(), self.day, tagName, self.plat)), 'r').readlines())
                server.saveLog(self.day, self.slot.getName(), tagName, self.plat, 2, logData)
                # raw log file
                logData = ''.join(file(os.path.join(self.slot.buildDir(), 'www', '%s.%s_%s-%s.log' % (self.slot.getName(), self.day, tagName, self.plat)), 'r').readlines())
                server.saveLog(self.day, self.slot.getName(), tagName, self.plat, 3, logData)
##             except Exception as (errno, strerror):
##                 self.system('echo "warning: writing Windows logs to nightlies server failed: %s -- %s' % (errno,stderror))     
##                 pass
            except Exception:
                self.system('echo "warning: writing Windows logs to nightlies server failed"')     
                pass
        #end of: writing logs directly to Nightlies server (for Windows)

        # copying logs
        os.chdir(os.path.join(self.slot.buildDir(), 'www'))
        files = os.listdir(os.getcwd())
        try:
            for f in files:
                if not os.path.isfile(f): continue
                shutil.copy2(os.path.join(self.slot.buildDir(), 'www', f), os.path.join(self.slot.wwwDir(), f))
##         except IOError as (errno, strerror): #no AFS token on Windows
##             self.system('echo "warning: IOError: probably no AFS token on Windows, writing logs failed: %s -- %s' % (errno,stderror))
##             pass
        except IOError: #no AFS token on Windows
            self.system('echo "warning: IOError: probably no AFS token on Windows, writing logs failed"' )
            pass
        releasePath = self.slot.releaseDir()
        try:
            if releasePath != None:
                if self.project.getRename() is not None:
                    tagName = self.project.getRename()
                else:
                    tagName = self.project.getTag()
                #instead of the previous isDone file copying:
                isDoneFrom = os.path.join(self.slot.buildDir(), 'isDone-'+os.environ['CMTCONFIG'])
                isDoneTo = os.path.join(self.slot.releaseDir(), 'isDone-'+os.environ['CMTCONFIG'])
                if os.path.exists(isDoneFrom):
                    shutil.copy2(isDoneFrom, isDoneTo)
                isStartedTo = os.path.join(self.slot.releaseDir(), 'isStarted-'+os.environ['CMTCONFIG'])
                if not os.path.exists(isStartedTo): file(isStartedTo, "w").close()
                if os.path.exists(isDoneTo) and os.path.exists(isStartedTo): os.remove(isStartedTo)
                if copyNotShared and copyShared:
                    self.copyLocal(
                                   os.path.join(self.slot.buildDir(), self.projName.upper(), tagName),
                                   os.path.join(releasePath, self.projName.upper(), tagName)
                    )
                elif not copyNotShared and not copyShared:
                    pass
                else:
                    self.copyLocal(
                                   os.path.join(self.slot.buildDir(), self.projName.upper(), tagName),
                                   os.path.join(releasePath, self.projName.upper(), tagName),
                                   lambda path: (copyNotShared and pathBinaryMatch(path, self.plat)) or (copyShared and pathSharedMatch(path))
                    )
    
                if not os.path.exists(os.path.join(releasePath, 'configuration.xml')):  shutil.copy2(os.path.join(os.environ['LCG_XMLCONFIGDIR'], 'configuration.xml'), releasePath)
##         except IOError as (errno, strerror): #no AFS token on Windows
##             self.system('echo "warning: IOError: probably no AFS token on Windows, writing logs failed: %s -- %s' % (errno,stderror))
##             pass
        except IOError: #no AFS token on Windows
            self.system('echo "warning: IOError: probably no AFS token on Windows, writing logs failed"')
            pass
        if self.systemType != 'windows' and nWarn + nErr + nMkErr + nCMTErr > 0:
            self.sendMails(self.slot, self.project, self.plat,"[LHCb Nightlies] %s, %s on %s results of automated build" % (str(tagName), str(self.slot.getName()), self.plat),"""
+++> Errors found in log file for tag """ + tagName + """ slot """ + self.slot.getName() + """
     while building for: """ + self.plat + """
         Number  of make errors : """ + str(nMkErr) + """
         Total number of errors : """ + str(nErr + nMkErr + nCMTErr) + """
         Number of warnings     : """ + str(nWarn) + """
         Coverity messages      : """ + str(nCoverityMessages) + """

     Log file(s) available from:
     http://cern.ch/lhcb-nightlies""")
        return 0

    @fork_call
    def test(self):
        from checkTestLogs import checkTestLogs 
        import datetime
        print "[LHCb] test"
        self.disableLCG_NIGHTLIES_BUILD()
        self.setCMTEXTRATAGS(self.slotName)
        self.setCmtProjectPath(self.slot)
        self.changeEnvVariables()
        os.chdir(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName))

        if self.project.getRename() is not None:
            tagName = self.project.getRename()
        else:
            tagName = self.project.getTag()

        htmlLogDirName = self.slotName + '.' + datetime.date.today().strftime('%a') + '_' + tagName + '-' + os.environ['CMTCONFIG'] + '-qmtest'
        os.environ['GAUDI_QMTEST_HTML_OUTPUT'] = os.path.abspath( os.path.join(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName),'..','..','logs',htmlLogDirName) )
        os.system('echo "'+ time.strftime('%c', time.localtime()) +'" >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
        os.system('cmt br - cmt TestPackage')
        os.chdir(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName))
        os.system('cmt qmtest_summarize 2>&1 >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')
        os.system('echo "'+ time.strftime('%c', time.localtime()) +'" >> ../../logs/' + os.environ['CMTCONFIG'] + '-qmtest.log')

        #log summary (text file) copying:
        testSummaryFrom = os.path.join(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName), '..', '..', 'logs', os.environ['CMTCONFIG'] + '-qmtest.log')
        testSummaryTo = os.path.join(self.slot.wwwDir(), self.slotName + '.' + datetime.date.today().strftime('%a') + '_' + tagName + '-' + os.environ['CMTCONFIG'] + '-qmtest.log')
        #log summary (html) copying:
        if self.systemType != 'windows':
            if os.path.exists(testSummaryFrom): shutil.copy2(testSummaryFrom, testSummaryTo)
            testSummaryFrom = os.path.join(self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName), '..', '..', 'logs', self.slotName + '.' + datetime.date.today().strftime('%a') + '_' + tagName + '-' + os.environ['CMTCONFIG'] + '-qmtest')
            testSummaryTo = os.path.join(self.slot.wwwDir(), self.slotName + '.' + datetime.date.today().strftime('%a') + '_' + tagName + '-' + os.environ['CMTCONFIG'] + '-qmtest/')
            try:
#                print "DEBUG ******************** checkTestLogs (start)"
                testResults = checkTestLogs(None, None, self.slot, self.project, datetime.date.today().strftime('%a'), os.environ['CMTCONFIG'], False)
                import sqlite3
                import datetime
                slotObj = self.slot
                platform = os.environ['CMTCONFIG']
                projObj = self.project
                total = testResults[1]
                noErr = testResults[2]
                noOK = testResults[3]
                db_date = datetime.date.today().strftime('%Y-%m-%d')
                db_time = datetime.datetime.now().strftime('%H:%M:%S.000')
                db_slot = slotObj.getName()
                db_plat = platform
                db_proj = tagName.split('_')[0]
                db_testlog = db_slot + "." + datetime.date.today().strftime("%a") + "_" + tagName +"-"+platform+"-qmtest/index.html"
                if 1==1:
#                if os.path.exists(logfile):
                    dbconn = sqlite3.connect('/afs/cern.ch/lhcb/software/nightlies/db/nightlies.results')
                    dbc = dbconn.cursor()
                    dbc.execute('select count(*) from results where date=? and slot=? and platform=? and project=?', (db_date, db_slot, db_plat, db_proj))
                    if dbc.fetchall()[0][0] == 0: # no record, so insert the new one
                        if total is None or noErr is None or noOK is None:
                            dbc.execute('insert into results (date, time, slot, platform, project, warnings, errors, makeerrors, cmterrors, tests_failed, tests_untested, tests_passed, buildlog, testlog) values (?,?,?,?,?,NULL,NULL,NULL,NULL,?,NULL,?,NULL,?)', (db_date, db_time, db_slot, db_plat, db_proj, noErr, noOK, db_testlog))
                        else:
                            dbc.execute('insert into results (date, time, slot, platform, project, warnings, errors, makeerrors, cmterrors, tests_failed, tests_untested, tests_passed, buildlog, testlog) values (?,?,?,?,?,NULL,NULL,NULL,NULL,?,?,?,NULL,?)', (db_date, db_time, db_slot, db_plat, db_proj, noErr, total-noErr-noOK, noOK, db_testlog))
                        dbconn.commit()
                        dbc.close()
                    else: # record is there, so update
                        if total is None or noErr is None or noOK is None:
                            dbc.execute('update results set tests_failed=?, tests_passed=?, testlog=? where date=? and slot=? and platform=? and project=?', (noErr, noOK, db_testlog, db_date, db_slot, db_plat, db_proj))
                        else:
                            dbc.execute('update results set tests_failed=?, tests_untested=?, tests_passed=?, testlog=? where date=? and slot=? and platform=? and project=?', (noErr, total-noErr-noOK, noOK, db_testlog, db_date, db_slot, db_plat, db_proj))
                        dbconn.commit()
                        dbc.close()
#                print "DEBUG ******************** checkTestLogs (end)"
##             except Exception as (errno, strerror):
##                 self.system('echo "warning: Exception raised while adding Windows stats to nightlies.result db: %s -- %s' % (errno,stderror))
##                 pass
            except Exception:
                self.system('echo "warning: Exception raised while adding Windows stats to nightlies.result db"')
                pass
            if os.path.exists(testSummaryFrom):
                if os.path.exists(testSummaryTo):
                    shutil.rmtree(testSummaryTo, ignore_errors=True)
                shutil.copytree(testSummaryFrom, testSummaryTo)
        return 0

#=============================== Nightlies/function.py ==============================#
    def generatePath(self, slot, project, type, projectRealName=None, beforeRename=False):
        """ generatePath(slotObject, projectObject, type[, projectRealName]) -> string

            <type> can be: "TAG" or "SYSPACKAGECMT".
            Depending on the type selected, function returns a string representing path for the slot and the project:
            - TAG:           /build/nightlies/..slot../..day../LHCB/LHCB_v23r4
            - SYSPACKAGECMT: /build/nightlies/..slot../..day../LHCB/LHCB_v23r4/LHCbSys/cmt
        """
        if project.getRename() is not None and not beforeRename:
            tagName = project.getRename()
        else:
            tagName = project.getTag()

        projectRoot = os.path.join(slot.buildDir(), project.getName(), tagName)
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

    def changeRequirementsFile(self, filename, changesDict, addons=None, removes=None, renameProjectObj=None):
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
            # renaming Sys package's requirements file if needed
            if renameProjectObj is not None and self.generatePath(self.slot, self.project, 'SYSPACKAGECMT', self.projName) in filename:
                newline = re.sub('^version %s' % renameProjectObj.getTag().split('_')[1], 'version %s' % renameProjectObj.getRename().split('_')[1], newline)

            for x in changesDict.keys():
                nameWithoutHat = self.changedPackageName(x)
                if changesDict[x][1] != '':
                    addToRE = '\s+'+changesDict[x][1]
                else:
                    addToRE = ''
                newline = re.sub('\\b'+nameWithoutHat+'\s+([Hh][Ee][Aa][Dd]|v[\*0-9]+(?:r[\*0-9]+(?:p[\*0-9]+)?)?)'+addToRE, nameWithoutHat+'  '+changesDict[x][0]+'  '+changesDict[x][1], newline)
            # <remove>:
            if removes:
                pac = re.sub('^\s*use\s+([a-zA-Z0-9_-]+).*$', '\\1', line).replace('\n','')
                if pac in [self.changedPackageName(x) for x in removes]: continue
            writer.write(newline)
        # <addon>
        if addons:
            for a in addons.keys():
                addPackage = self.changedPackageName(a)
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
        if project:
            bRename = True
        else:
            bRename = False
        logFile = os.path.join(self.generatePath(self.slot, self.project, 'TAG', self.project.getName(), beforeRename=bRename), "checkout.log")
        cmd = self.getpackCommand
        if withVersionDir: cmd += ' -v'
        if recursive: cmd += ' -r'
        if project: cmd += ' -P'
        cmd += ' %s %s' % (package, version)
        print "*"*80
        print cmd
        print "*"*80
        if self.systemType == "windows":
            os.system(cmd)
        else:
            os.system("%s >> %s 2>&1" % (cmd, logFile))

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
                for q in self.localDirsOnWindows.keys():
                    if q.lower() in newY:
                        newY = newY.replace(q.lower(), self.localDirsOnWindows[q])
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
#        cmtProjectPathPreviuos = None
#        if os.environ.has_key('CMTPROJECTPATH'):
#            cmtProjectPathPrevious = os.environ['CMTPROJECTPATH']
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

    def copyLocal(self, sourceDir, targetDir, check=None):
        """ tar-pipe copying with locking from <sourceDir> to <targetDir>. Target directory is created if necessary.

            Locking is implemented on the level of the most deep sub-directory. Empty directories are not copied.
            Works only on Linux, both <sourceDir> and <targetDir> must be accessible locally.
        """
        from LbUtils.Lock import Lock
        #by default, the check function is always True
        if check is None: check = lambda path: True
        if not os.path.exists(targetDir): os.makedirs(targetDir)
        os.chdir(targetDir) # .lock_* files are stored in CWD and they must by on shared file system
        sourceDir = os.path.abspath(sourceDir)
        targetDir = os.path.abspath(targetDir)
        dirList = [x for x in os.walk(sourceDir, topdown=False)]
        for root, dires, files in dirList:
            if files:
                fullPath = root+os.sep
                path = fullPath.replace(sourceDir, '')
                files = [x for x in files if x[-4:] != '.pyc' and x[-4:] != '.pyo' and check(fullPath+x)]
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

    def iccSetup(self):
        envChange = {
                     'CPATH' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/ipp/em64t/include:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//mkl/include:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/tbb/include',
                     'DYLD_LIBRARY_PATH' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/tbb/intel64/cc4.1.0_libc2.4_kernel2.6.16.21/lib',
                     'FPATH' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//mkl/include',
                     'icc_c_home' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072',
                     'icc_f_home' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072',
                     'INCLUDE' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/ipp/em64t/include:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//mkl/include',
                     'LD_LIBRARY_PATH' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//lib/intel64:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/ipp/em64t/sharedlib:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//mkl/lib/em64t:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/tbb/intel64/cc4.1.0_libc2.4_kernel2.6.16.21/lib',
                     'LIB' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/ipp/em64t/lib:',
                     'LIBRARY_PATH' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//lib/intel64:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/ipp/em64t/lib:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//mkl/lib/em64t:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/tbb/intel64/cc4.1.0_libc2.4_kernel2.6.16.21/lib',
                     'MANPATH' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//man/en_US:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//mkl/man/en_US',
                     'MKLROOT' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//mkl',
                     'NLSPATH' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//lib/intel64/locale/%l_%t/%N:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/ipp/em64t/lib/locale/%l_%t/%N:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//mkl/lib/em64t/locale/%l_%t/%N:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//idb/intel64/locale/%l_%t/%N',
                     'PATH' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//bin/intel64',
                     #intel_home=/afs/cern.ch/sw/IntelSoftware/linux
                     'INTEL_LICENSE_FILE' : '28518@lxlic01.cern.ch:28518@lxlic02.cern.ch:28518@lxlic03.cern.ch:/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072//licenses:/opt/intel/licenses', #:/afs/cern.ch/user/k/kkruzele/intel/licenses
                     #'intelplat' : 'intel64',
                     'IPPROOT' : '/afs/cern.ch/sw/IntelSoftware/linux/x86_64/Compiler/11.1/072/ipp/em64t',
                     }
        for x in envChange:
            if not x in os.environ:
                os.environ[x] = envChange[x]
            else:
                os.environ[x] = "%s%s%s" % (envChange[x], os.pathsep, os.environ[x])

    

            
import BaseServer

class LHCbServer(BaseServer.Server):
    def __init__(self, cfgFile, cfgContents=None):
        import logging
        import logging.handlers
        self.unitsTaken = []
        BaseServer.Server.__init__(self, cfgFile, cfgContents)
        import logging
        logging.basicConfig(level='debug')
        self._log = logging.getLogger(__name__)
        self._log.debug('START of LHCb Server Logging')
    def saveLog(self, day, slot, project, platform, contentType, content):
        """ contentType is:
             0) isDone file
             1) build log, html
             2) build log, summary
             3) build log, raw format
        """
#        debugMsg = '----------------------\nHost: %s\nSlot: %s\nProject: %s\nPlatform: %s\nBuild log: %s\nContent length: %s\n----------------------' % (os.environ.get('HOST', 'puste'), str(slot), str(project), str(platform), str(isBuildLog), str(len(content)) )
#        os.system('echo "%s"' % debugMsg)

        # find a slot object in configuration
        from configuration import Configuration
        conf = Configuration()
        conf.readConf(configFile=None, configContents=self.config) 
        slotObj = conf.findSlot(slot, conf._slotList)

        # if doesn't exist, create isStarted file
        isStartedTo = os.path.join(slotObj.releaseDir(), 'isStarted-%s' % platform)
        if not os.path.exists(isStartedTo): file(isStartedTo, "w").close()

        if contentType == 0: # isDone file
            logFile = os.path.join(slotObj.releaseDir(), 'isDone-%s' % platform)
        elif contentType == 1: # html build log
            logFile = os.path.join(slotObj.wwwDir(), '%s.%s_%s-%s-log.html' % (slot, day, project, platform))
        elif contentType == 2: # build log summary
            logFile = os.path.join(slotObj.wwwDir(), '%s.%s_%s-%s-log.summary' % (slot, day, project, platform))
        elif contentType == 3: # raw build log
            logFile = os.path.join(slotObj.wwwDir(), '%s.%s_%s-%s.log' % (slot, day, project, platform))
        else:
            return
        if not os.path.exists(logFile):
            writeTo = file(logFile, 'w')
            writeTo.write(content)
    def setToday(self):
        "used when reseting server"
        self.unitsTaken = []
        BaseServer.Server.setToday(self)
    def detectDuplicatePriority(self):
        # do not print warnings if (slot,platform) priorities are duplicated
        pass
    def cmpWorkUnits(self, a, b): # a=(slot, platform), b=(slot, platform)
        #POLICY: slots in order, then platforms in order
        slotOrder = ['lhcb-prerelease','lhcb-head','lhcb-patches','lhcb-branches','lhcb-gaudi-head','lhcb-lcg-head']
        platOrder = ['x86_64-slc5-gcc43-opt','i686-slc5-gcc43-opt','slc4_amd64_gcc34','slc4_ia32_gcc34',
                     'x86_64-slc5-gcc43-dbg','i686-slc5-gcc43-dbg','slc4_amd64_gcc34_dbg','slc4_ia32_gcc34_dbg',
                     'win32_vc71_dbg']
        # compare slot names
        self._log.debug('cmtWorkUnits: %s %s' %(str(a), str(b)))

        if a[0] in slotOrder and b[0] not in slotOrder: return -1
        elif b[0] in slotOrder and a[0] not in slotOrder: return 1
        elif a[0] in slotOrder and b[0] in slotOrder:
            #if both of slot names on order list
            if slotOrder.index(a[0]) < slotOrder.index(b[0]): return -1
            else: return 1
        else:
            #if none of slot names on order list
            if a[1] in platOrder and b[1] not in platOrder: return -1
            elif b[1] in platOrder and a[1] not in platOrder: return 1
            elif a[1] in platOrder and b[1] in platOrder:
                #if both of platforms on order list
                if platOrder.index(a[1]) < platOrder.index(b[1]): return -1
                else: return 1
            else:
                #if none of platforms on order list
                return 0
    def getWorkUnit(self,configuration):
        import LbConfiguration.Platform
        today = datetime.date.today().strftime('%a')
        self.readConfigurationFile(self.cfgFile, self.cfgContents)
        jobs = []
        for slotObj in self.conf._slotList:
            if slotObj.getDisabled() is True: continue
            if today not in slotObj.getDays(): continue
            for plObj in slotObj.getPlatforms():
                #check "waitfor" flag, if False, skip this (slot, platform)
                plName = plObj.getName()
                try:
                    if slotObj.waitForFlag(plName) and sum([int(os.path.exists(q)) for q in slotObj.waitForFlag(plName)])!=len(slotObj.waitForFlag(plName)):
                        continue
                except IOError:
                    continue
                jobs.append((slotObj.getName(), plObj.getName()))
        jobs.sort(self.cmpWorkUnits)
        print jobs
        platformType = LbConfiguration.Platform.getPlatformType(configuration)
        architecture = LbConfiguration.Platform.getArchitecture(configuration)
        # (1) checks platform type only: slc5 on slc5, slc4 on slc4
        #toBeTaken = map(lambda x:platformType==LbConfiguration.Platform.getPlatformType(x[1]) and x not in self.unitsTaken, jobs)
        # (2) strict maching - both platform type and architecture (ia32 only on ia32 for example)
        toBeTaken = map(lambda x:platformType==LbConfiguration.Platform.getPlatformType(x[1]) and architecture==LbConfiguration.Platform.getArchitecture(x[1]) and x not in self.unitsTaken, jobs)

        print toBeTaken
        #return the first one from the list, if any
        if True in toBeTaken:
            ret = jobs[toBeTaken.index(True)]
            self.unitsTaken.append((ret[0], ret[1]))
            return ":".join([ret[0], ret[1], '50', ' priority 122: time 2010-06-04 16:00:00.123456'])
        else:
            return ""

class LHCbDefaults(object):
    @classmethod
    def serverHost(cls):
        return 'lxbuild135'
    @classmethod
    def serverPort(cls):
        return '61007' # 61007 for production, 51007 for tests

__lcg_nightlies_extensions__ = {"server": [LHCbServer],
                                "builder": [LHCbProjectBuilder],
                                "defaults": [LHCbDefaults]}
