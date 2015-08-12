#!/usr/bin/env python

import sys, os

list_runs_dir = '/group/online/AligWork/CheckConstants'

runs_fileName = os.path.join(list_runs_dir, 'runsAnalised.pkl')
bads_fileName = os.path.join(list_runs_dir, 'bads.pkl')

lock_fileName = os.path.join(list_runs_dir, '.lock')

this_file_dir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(this_file_dir)

def getOfflineRuns():
    import sys, time
    sys.path.append('/group/online/rundb/RunDatabase/python')
    
    import rundb
    db = rundb.RunDB()
    
    # startTime = 1433109600 # June 2015
    startTime = 1436479200 # 10 July 2015
    # time.mktime(time.strptime('10:07:2015','%d:%m:%Y'))
    
    all_fills = sorted(db.getPhysicsFills(), key=lambda x: x[0])
    all_fills = [(i[0],
                  time.mktime(time.strptime(i[1], '%Y-%m-%dT%H:%M:%S')))
                 for i in all_fills]
    fills = [fill[0] for fill in all_fills if fill[1] > startTime]
    
    allOfflineRuns = sum([db.getOfflineRunsForFill(fill) for fill in fills], [])

    return allOfflineRuns


def getAnalisedRuns():
    import pickle
    try:
        return pickle.load(open(runs_fileName, 'rb'))
    except IOError:
        return []


def sendEmail(run, diff, receivers = ['gdujany@cern.ch']):
    import smtplib
    sender = 'gdujany@cern.ch'
    message = 'From: Conditions Monitor <gdujany@cern.ch>\n'
    message += 'To: <gdujany@cern.ch>\n'
    message += 'Subject: Run {0} different conditions Online - Offline\n\n'.format(run)
    message += diff
    try:
        smtpObj = smtplib.SMTP('localhost')
        smtpObj.sendmail(sender, receivers, message)         
        print "Successfully sent email"
    except SMTPException:
        print "Error: unable to send email"


def addAnalisedRun(run, fileName):
    import pickle
    try:
        analised_runs = pickle.load(open(fileName, 'rb'))
    except IOError:
        analised_runs = []
    analised_runs.append(run)
    pickle.dump(analised_runs, open(fileName, 'wb'))


def lock():
    if os.path.exists(lock_fileName):
        sendEmail(555, 'Lockfile '+lock_fileName+' present when it should not!')
        sys.exit(1)
    open(lock_fileName, 'a').close()


def unlock():
    if not os.path.exists(lock_fileName):
        sendEmail(555, 'Lockfile '+lock_fileName+' not present when it should!')
        sys.exit(1)
    os.remove(lock_fileName)


if __name__ == '__main__':

    print 'Retrieving runs to process'
    lock()
    toAnalise = [i for i in getOfflineRuns() if i not in getAnalisedRuns()]

    from diffConds import diffOnlineOffline
    print toAnalise
    for run in toAnalise:
        print '*'*100
        print 'Analise run', run
        diff = diffOnlineOffline(run)
        if not diff:
            addAnalisedRun(run, runs_fileName)
        else:
            addAnalisedRun(run, bads_fileName)
            sendEmail(run, diff)
    unlock()
        
