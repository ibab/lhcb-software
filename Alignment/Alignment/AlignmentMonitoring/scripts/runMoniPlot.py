#!/usr/bin/env python

import os, sys, re, time, subprocess, shlex

AligWork_dir = '/group/online/AligWork'
Moni_dir = '/group/online/AligWork/MoniPlots'
alignment_dir = '/group/online/alignment'
activity = 'Velo'
minRun = 160140 
oldtime = {'Velo' : 0, 'Tracker' : 0}

this_file_dir = os.path.dirname(os.path.realpath(__file__))

sys.path.append('/group/online/rundb/RunDatabase/python/')
def getFillNumber(runNumber):
    import rundb
    db = rundb.RunDB()
    return db.getrun(runNumber)[0]['fillID']


def getRunsAnalised(activity='Velo'):
    alignsDir = os.path.join(Moni_dir, activity)
    runs = []
    for folder in os.listdir(alignsDir):
        if re.match('^(?:v[0-9]*_)?([0-9]*).pdf$', folder):
            runs.append(int(re.findall('^(?:v[0-9]*_)?([0-9]*).pdf$', folder)[0]))
    return sorted([i for i in runs])


def getRunsAligned(activity='Velo', minRun=0):
    alignsDir = os.path.join(AligWork_dir, activity)
    runs = []
    for folder in os.listdir(alignsDir):
        if re.match('^run([0-9]*)$', folder):
            runs.append(int(re.findall('^run([0-9]*)$', folder)[0]))
    return sorted([i for i in runs if i >= minRun])


def getRunFromXml(xml_file):
    text = open(xml_file).read()
    try:
        run = re.findall('<!-- Version:: run([0-9]*) -->', text)[0]
        return int(run)
    except IndexError:
        return None

    
def getRunsUpdated(activity = 'Velo', minRun = 0):
    if activity == 'Tracker': activity = 'IT'
    xmlDir = os.path.join(alignment_dir, '{0}/{0}Global'.format(activity))
    runs = {}
    for xml_file in os.listdir(xmlDir):
        alignVersion = int(re.findall('.*v([0-9]*).xml', xml_file)[0])
        run = getRunFromXml(os.path.join(xmlDir, xml_file))
        if run != None and run >= minRun:
            runs[run] = alignVersion
    return runs


def getRuns2Analise(activity = 'Velo', minRun = 0):
    return sorted([i for i in getRunsAligned(activity, minRun) if i not in getRunsAnalised(activity)])


def hasNewAlignment(activity = 'Velo'):
    global oldtime
    newtime = os.path.getmtime(os.path.join(AligWork_dir, activity))
    if newtime > oldtime[activity]:
        oldtime[activity] = newtime
        return True
    else:
        return False

def printTime():
    return time.strftime('%d/%m/%y %H:%M:%S |')

def writeInLogbook(Fill, activity, updated = False, file2upload=None, version = None):
    host = 'lblogbook.cern.ch' #'localhost'
    port = 8080
    username = 'common Common\\!'
    logbook = 'Alignment monitoring'
    author = 'monibot'
    Type = 'Convergence'
    instruction_file = 'https://lbgroups.cern.ch/online/Shifts/alignMonitoring.pdf'
    if activity == 'Tracker':
        instruction_file = 'https://lbgroups.cern.ch/online/Shifts/alignMonitoring_Tracker.pdf'
    if updated:
        status = 'Unchecked'
        subject = 'Monitoring plots'
        textVersion = ' (v{0}),'.format(version) if version else ','
        text = "Updated {activity} alignment for fill {Fill}{textVersion} monitoring plots in the attachment; shifter's instructions can be found at {instruction_file}".format(**locals())
    else:
        status = 'Good'
        subject = 'No alignment update'
        text = "No {activity} alignment update needed for fill {Fill}.".format(**locals())
    
    command = 'elog -h "{host}" -p {port} -l "{logbook}" -u {username} -a "Author={author}" -a "Fill={Fill}" -a "System={activity}" -a "Type={Type}" -a "Status={status}" -a "Subject={subject}" "{text}"'.format(**locals())

    if file2upload:
        command += ' -f "{0}"'.format(file2upload)

    cmd = shlex.split(command)
    FNULL = open(os.devnull, 'w')
    subprocess.call(cmd, stdout=FNULL, stderr=subprocess.STDOUT)
    return 0

if __name__ == '__main__':
    moniScript = {'Velo' : 'moniPlots.py', 'Tracker' : 'moniPlots_Tracker.py'}

    while True:
        for activity in ['Velo', 'Tracker']:
            if hasNewAlignment(activity):
                if not os.path.exists(os.path.join(Moni_dir, activity)):
                    os.mkdir(os.path.join(Moni_dir, activity))
                toAnalise = getRuns2Analise(activity, minRun)
                runsUpdated = getRunsUpdated(activity, minRun)
                print printTime(), 'Alignments, runs to analise for ', activity, ': ', toAnalise
                sys.stdout.flush()
                for run in toAnalise:
                    if runsUpdated.has_key(run):
                        version = runsUpdated[run]
                        outFile_name = os.path.join(Moni_dir, '{0}/v{1}_{2}.pdf'.format(activity, version, run))
                    else:
                        version = None
                        outFile_name = os.path.join(Moni_dir, '{0}/{1}.pdf'.format(activity, run))
                    print printTime(), 'Analising run {0}'.format(run) + (', alignment version v{0}'.format(version) if version else '')
                    sys.stdout.flush()
                    command = '{0} -r {1} -o {2}'.format(
                        os.path.join(this_file_dir, moniScript[activity]), 
                        run, 
                        outFile_name)
                    cmd = shlex.split(command)
                    FNULL = open(os.devnull, 'w')
                    subprocess.call(cmd, stdout=FNULL, stderr=subprocess.STDOUT)
                    if version:
                        writeInLogbook(Fill=getFillNumber(run), activity=activity, file2upload=outFile_name, version = version, updated = True)
                    else:
                        writeInLogbook(Fill=getFillNumber(run), activity=activity, updated = False)
                print printTime(), 'Done for now'
                sys.stdout.flush()

        time.sleep(5)
        
