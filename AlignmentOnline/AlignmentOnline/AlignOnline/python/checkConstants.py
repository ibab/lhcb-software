#!/usr/bin/env python

import sys, os

list_runs_dir = '/group/online/AligWork/CheckConstants'
conditions_dir = '/group/online/hlt/conditions/LHCb/2015'

runs_fileName = os.path.join(list_runs_dir, 'runsAnalised.pkl')
bads_fileName = os.path.join(list_runs_dir, 'bads.pkl')

file_getHandshake = 'CondDBXfer_done' 
file_giveHandshake = 'CondDBXfer_ok'
file_notOk = 'CondDBXfer_notOk'


lock_fileName = os.path.join(list_runs_dir, '.lock')

this_file_dir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(this_file_dir)

def getOfflineRuns_old():
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


def getOfflineRuns(file2check = file_getHandshake, minRun = 157558):
    import glob    
    ll = glob.glob(os.path.join(conditions_dir,'*/{0}'.format(file2check)))
    return sorted([int(i.split('/')[-2]) for i in ll if int(i.split('/')[-2]) >= minRun])


def getAnalisedRuns(fileName = runs_fileName):
    import pickle
    try:
        toReturn = pickle.load(open(fileName, 'rb'))
    except IOError:
        toReturn = []
    return toReturn


def sendEmail(run, diff, receivers = ['gdujany@cern.ch', 'lhcb-onlinealignmentcalibration@cern.ch'], problemSolved = False):
    import smtplib
    sender = 'gdujany@cern.ch'
    message = 'From: Conditions Monitor <gdujany@cern.ch>\n'
    message += 'To: <gdujany@cern.ch>\n'
    if problemSolved:
        message += 'Subject: Run {0} Online - Offline conditions now agree\n\n'.format(run)
        message += '\nThe conditions for run {0} used to be different but now agree.\n For more informations check https://twiki.cern.ch/twiki/bin/view/LHCb/CheckOnlineOfflineConditions\n\n'.format(run)
    else:
        message += 'Subject: Run {0} different conditions Online - Offline\n\n'.format(run)
        message += '\nThis is an automatic message to warn you that some conditons are different online and offine.\n For more informations check https://twiki.cern.ch/twiki/bin/view/LHCb/CheckOnlineOfflineConditions\n\n'
        message += diff

    try:
        smtpObj = smtplib.SMTP('localhost')
        smtpObj.sendmail(sender, receivers, message)         
        print "Successfully sent email"
    except smtplib.SMTPException:
        print "Error: unable to send email"


def addAnalisedRun(run, fileName):
    import pickle
    try:
        analised_runs = pickle.load(open(fileName, 'rb'))
    except IOError:
        analised_runs = []
    analised_runs.append(run)
    pickle.dump(analised_runs, open(fileName, 'wb'))

def removeRun(run, fileName):
    import pickle
    try:
        analised_runs = pickle.load(open(fileName, 'rb'))
    except IOError:
        analised_runs = []
    try:
        analised_runs.remove(run)
    except ValueError:
        pass
    if len(analised_runs):
        pickle.dump(analised_runs, open(fileName, 'wb'))
    else:
        os.remove(fileName)


def lock():
    if os.path.exists(lock_fileName):
        print 'Lockfile '+lock_fileName+' present when it should not!'
        # sendEmail(555, 'Lockfile '+lock_fileName+' present when it should not!', receivers = ['gdujany@cern.ch'])
        sys.exit(1)
    open(lock_fileName, 'a').close()


def unlock():
    if not os.path.exists(lock_fileName):
        print 'Lockfile '+lock_fileName+' not present when it should!'
        # sendEmail(444, 'Lockfile '+lock_fileName+' not present when it should!', receivers = ['gdujany@cern.ch'])
        sys.exit(1)
    os.remove(lock_fileName)


if __name__ == '__main__':

    print 'Retrieving runs to process'
    lock()
    toAnalise = [i for i in getOfflineRuns() if i not in getAnalisedRuns()]
    bads = getAnalisedRuns(bads_fileName)

    from diffConds import diffOnlineOffline
    print toAnalise

    if len(toAnalise) > 5:
        sendEmail(333, str(len(toAnalise))+' runs still to analise, please check if something is wrong', receivers = ['gdujany@cern.ch'])
    
    
    for run in toAnalise:
        print '*'*100
        print 'Analise run', run
        try:
            diff = diffOnlineOffline(run)
            if not diff:
                addAnalisedRun(run, runs_fileName)
                open(os.path.join(conditions_dir, '{0}/{1}'.format(run, file_giveHandshake)), 'a').close()
                if run in bads:
                    removeRun(run, bads_fileName)
                    sendEmail(run, diff, problemSolved = True)
            else:
                open(os.path.join(conditions_dir, '{0}/{1}'.format(run, file_notOk)), 'a').close()
                if run not in bads:
                    addAnalisedRun(run, bads_fileName)
                    sendEmail(run, diff)
                    # if 'Alignment/Rich' in diff:
                    #     sendEmail(run, diff, receivers = [])
                    # if 'Environment/Rich' in diff:
                    #     sendEmail(run, diff, receivers = [])
                    # if ('Alignment/Velo' in diff) or ('Alignment/TT' in diff) or ('Alignment/IT' in diff) or ('Alignment/OT' in diff):
                    #     sendEmail(run, diff, receivers = [])
                    # if 'Calibration/OT' in diff:
                    #     sendEmail(run, diff, receivers = [])
        
        except AssertionError:
            # takes some times to copy constants to offline DB
            # so getting offline constants can fail
            pass

    unlock()
        
