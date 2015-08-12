#!/usr/bin/env python
import subprocess
import sys, time, os

this_file_dir = os.path.dirname(os.path.realpath(__file__))

def findStartTime(run):
    '''
    Return initial time run reading from the runDB
    '''
    try:
        import rundb
    except ImportError:
        sys.path.append('/group/online/rundb/RunDatabase/python')
        import rundb
    
    db = rundb.RunDB()
    
    startTime = db.getrun(run)[0]['startTime']
        
    return int(time.mktime(time.strptime(startTime,'%Y-%m-%d %H:%M:%S.0000')))*10**9


def parseConditions(inFile_name):
    import re
    str_conditions = open(inFile_name).read()
    conditions = re.findall(r'BEGIN CONDITION\n([^`]*?)\n([^`]*?)\nEND CONDITION', str_conditions)
    dict_conditions = {}
    for cond, value in conditions:
        dict_conditions[cond] = value
    return dict_conditions


def diffConditions(fileNames):
    
    dict_online = parseConditions(fileNames['online'])
    dict_offline = parseConditions(fileNames['offline'])
    
    shared_items = set(dict_online.items()) & set(dict_offline.items())
    
    diff_online = dict(set(dict_online.items()) - shared_items)
    diff_offline = dict(set(dict_offline.items()) - shared_items)
    del diff_online['Conditions/Online/LHCb/LHCFillingScheme']
    del diff_offline['Conditions/Online/LHCb/LHCFillingScheme']

    str_diff = ''''''
    for cond in diff_online:
        str_diff += '\n'+'#'*50
        str_diff += '\n\nCondition:\n'
        str_diff += cond
        str_diff += '\n\nOnline:\n'
        str_diff += diff_online[cond]
        str_diff += '\n\nOffline:\n'
        str_diff += diff_offline[cond]

    return str_diff


def diffOnlineOffline(run, delCond = True):
    '''
    Function that check that the same calibration constants are applied online and offline
    If no difference return false else return diff of files produced by Brunel
    '''

    import time, shlex
    
    fileNames = dict( offline = '/group/online/AligWork/CheckConstants/conditions_offline.txt',
                      online = '/group/online/AligWork/CheckConstants/conditions_online.txt')

    for fileName in fileNames.values():
        if os.path.exists(fileName):
            os.remove(fileName)

    startTime = findStartTime(run)
    print 'startTime:', startTime
    year = time.gmtime(startTime/10**9)[0]

    run_cond_dir = '/group/online/hlt/conditions/LHCb/{0}/{1}'.format(year, run)
    if not os.path.exists(os.path.join(run_cond_dir, 'HLT2Params.py')):
        print 'WARNING: Skipping due to missing HLT2Params'
        return 0
    
    env = os.environ.copy()
    env['PYTHONPATH'] = (env['PYTHONPATH']
                         + ':' + run_cond_dir
                         + ':/group/online/hlt/conditions/RunChangeHandler')
    command = 'python ' + os.path.join(this_file_dir, 'printConds.py') + ' {online_script} {startTime} {runNumber} {outFile}'
        
    opts_online = dict(
        startTime = startTime,
        runNumber = run,
        online_script = '--online',
        outFile = "'{0}'".format(fileNames['online']))
    
    opts_offline = dict(
        startTime = startTime,
        runNumber = run,
        online_script = '',
        outFile = "'{0}'".format(fileNames['offline']))

    print '######################################'
    print '###   RUN WITH ONLINE CONDITIONS   ###'
    print '######################################'
    cmd = shlex.split(command.format(**opts_online))
    subprocess.call(cmd, env = env)
    assert(os.path.exists(fileNames['online']))
    print '#######################################'
    print '###   RUN WITH OFFLINE CONDITIONS   ###'
    print '#######################################'
    cmd = shlex.split(command.format(**opts_offline))
    subprocess.call(cmd, env = env)
    assert(os.path.exists(fileNames['offline']))
    print '##################################'
    print '###   DIFF ONLINE - OFFLINE    ###'
    print '##################################'
    difference = diffConditions(fileNames)
    if difference:
        print difference
    else:
        print 'No difference'
   
    if delCond:
        for fileName in fileNames.values():
            if os.path.exists(fileName):
                os.remove(fileName)

    return difference

if __name__ == '__main__':

    import argparse, sys
    parser = argparse.ArgumentParser(description ="Macro that check that for a particular run the online and offline conditions are the same")
    parser.add_argument('run',help='run to process')
    parser.add_argument('--noDel',help='do not delete conditions file', action = 'store_true')
    args = parser.parse_args()
    
    diff = diffOnlineOffline(args.run, delCond=not args.noDel)
    
    if not diff:
        sys.exit(0)
    else:
        sys.exit(1)
