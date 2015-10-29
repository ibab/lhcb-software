#!/usr/bin/env python

import os,re,fnmatch

def findLastRun(activity):
    #-->activity=Velo/Tracker/...
    rootDir = '/group/online/AligWork/{0}/'.format(activity)
    ll = sorted([(re.findall('run([0-9]+)', i)[0],  os.stat(os.path.join(rootDir, i)).st_mtime) for i in os.listdir(rootDir)if re.match('run([0-9]+)', i)], key= lambda x: x[-1])
    return ll[-1][0]
    

def findAlignlog(activity,run):
    rootDir = '/group/online/AligWork/{0}/'.format(activity)
    inDir = os.path.join(rootDir, 'run{0}'.format(run))
    alignlog_path = os.path.join(inDir, 'alignlog.txt')
    return alignlog_path


def findHistos(activity,run):
    histDir = '/hist/Savesets/2015/LHCbA/AligWrk_{0}'.format(activity)
    matches = []
    for root, dirs, files in os.walk(histDir):
        for file in files:
            match = re.findall(r'AligWrk_{activity}-{run}(.*?)-(.*?)-EOR.root'.format(**locals()), file)
            if match:
                matches.append(list(match[0])+[os.path.join(root, file)])

    matches = sorted(matches, key=lambda x: x[1], reverse = True)
   
    files_histo = {}
    if matches[0][0] == '01': # only one histogram file
        files_histo['old'] = matches[0][2]
    else:
        for match in matches:
            if match[0] == '01':
                files_histo['old'] = match[2]
                files_histo['new'] = matches[0][2]
                break
    if not len(files_histo): # temporary patch if first run has not 01, use only timestamp info, assumes each fill run only once
        files_histo['old'] = matches[-1][2]
        files_histo['new'] = matches[0][2]
    return files_histo
   

