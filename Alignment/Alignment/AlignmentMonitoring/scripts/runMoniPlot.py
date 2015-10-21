#!/usr/bin/env python

import os, sys, re, time, subprocess, shlex

AligWork_dir = '/group/online/AligWork'
Moni_dir = '/group/online/AligWork/MoniPlots'
alignment_dir = '/group/online/alignment'
activity = 'Velo'
minAlign = 7 #0 #164237
oldtime = 0

this_file_dir = os.path.dirname(os.path.realpath(__file__))

sys.path.append('/group/online/rundb/RunDatabase/python/')
def getFillNumber(runNumber):
    import rundb
    db = rundb.RunDB()
    return db.getrun(runNumber)[0]['fillID']

def getAlignsAnalised():
    alignsDir = os.path.join(Moni_dir, activity)
    runs = []
    for folder in os.listdir(alignsDir):
        if re.match('v[0-9]*.pdf$', folder):
            runs.append(int(re.findall('v([0-9]*).pdf$', folder)[0]))
    return sorted([i for i in runs])


def getRunFromXml(xml_file):
    text = open(xml_file).read()
    try:
        run = re.findall('<!-- Version:: run([0-9]*) -->', text)[0]
        return int(run)
    except IndexError:
        return None


def getAlignsUpdated(activity = 'Velo', minAlign = 0):
    xmlDir = os.path.join(alignment_dir, '{0}/{0}Global'.format(activity))
    runs = []
    for xml_file in os.listdir(xmlDir):
        alignVersion = int(re.findall('.*v([0-9]*).xml', xml_file)[0])
        run = getRunFromXml(os.path.join(xmlDir, xml_file))
        if run != None:
            runs.append((alignVersion, run))
    return sorted([i for i in runs if i[0] >= minAlign], key = lambda x: x[0])


def getAligns2Analise(activity = 'Velo', minAlign = 0):
    return sorted([i for i in getAlignsUpdated(activity, minAlign) if i[0] not in getAlignsAnalised()], key = lambda x: x[0])


def hasNewAlignment(activity = 'Velo'):
    global oldtime
    newtime = os.path.getmtime(os.path.join(alignment_dir,  '{0}/{0}Global'.format(activity)))
    if newtime > oldtime:
        oldtime = newtime
        return True
    else:
        return False

def printTime():
    return time.strftime('%d/%m/%y %H:%M:%S |')

def writeInLogbook(Fill, activity, file2upload=None):
    host = 'lblogbook.cern.ch' #'localhost'
    port = 8080
    username = 'common Common\\!'
    logbook = 'Alignment monitoring'
    author = 'monibot'
    Type = 'Convergence'
    status = 'Unchecked'
    subject = 'Monitoring plots'
    text = "Monitoring plots for fill {Fill}, shifter's instructions can be found at https://lbgroups.cern.ch/online/Shifts/alignMonitoring.pdf".format(**locals())
    
    command = 'elog -h "{host}" -p {port} -l "{logbook}" -u {username} -a "Author={author}" -a "Fill={Fill}" -a "System={activity}" -a "Type={Type}" -a "Status={status}" -a "Subject={subject}" "{text}"'.format(**locals())

    if file2upload:
        command += ' -f "{0}"'.format(file2upload)

    cmd = shlex.split(command)
    FNULL = open(os.devnull, 'w')
    subprocess.call(cmd)#, stdout=FNULL, stderr=subprocess.STDOUT)
    return 0

if __name__ == '__main__':
    while True:
        if hasNewAlignment(activity):
            if not os.path.exists(os.path.join(Moni_dir, activity)):
                os.mkdir(os.path.join(Moni_dir, activity))
            toAnalise = getAligns2Analise(activity, minAlign)
            print printTime(), 'Alignments, runs to analise:', toAnalise
            sys.stdout.flush()
            for alignVersion, run in toAnalise:
                print printTime(), 'Analising run {0}, alignment v{1} '.format(run ,alignVersion)
                sys.stdout.flush()
                outFile_name = os.path.join(Moni_dir, '{0}/v{1}.pdf'.format(activity, alignVersion))
                command = '{0} -r {1} -o {2}'.format(
                    os.path.join(this_file_dir, 'moniPlots.py'), 
                    run, 
                    outFile_name)
                cmd = shlex.split(command)
                FNULL = open(os.devnull, 'w')
                subprocess.call(cmd, stdout=FNULL, stderr=subprocess.STDOUT)
                writeInLogbook(Fill=getFillNumber(run), activity=activity, file2upload=outFile_name)
            print printTime(), 'Done for now'
            sys.stdout.flush()
            
    time.sleep(5)
        
