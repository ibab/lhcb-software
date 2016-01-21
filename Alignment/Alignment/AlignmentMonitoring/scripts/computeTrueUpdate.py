#!/usr/bin/env python

std_AligWork_dir = '/group/online/AligWork/'
std_new_alignment_dir = 'my_alignments/'

##########################
###   Options parser   ###
if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description ="Macro to make new xml files with alignment updates needed according to the thresholds set")
    parser.add_argument('--activity', help='choose between Velo, Tracker; default is Velo', choices = ['Velo', 'Tracker'] ,default= 'Velo')
    parser.add_argument('--AligWork_dir', help='folder with alignment', default = std_AligWork_dir)
    parser.add_argument('--new_alignment_dir', help='folder with alignment', default = std_new_alignment_dir)
    args = parser.parse_args()
##########################


import os, sys, re, shutil, pickle
from GaudiPython import gbl
AlMon = gbl.Alignment.AlignmentMonitoring

def std_vector(list, kind = 'std::string'):
    vec = gbl.std.vector(kind)()
    for i in list:
        vec.push_back(i)
    return vec

# from diffXML import getDictDifference, bigChanges, dict_max_diff 

def getListRuns(AligWork_dir = std_AligWork_dir):
    runs = []
    for directory in os.listdir(os.path.join(AligWork_dir, 'Velo')): 
        match = re.findall('^run([0-9]+)$', directory)
        if match: runs.append(int(match[0]))
    return sorted(runs)

def isUpdate(cc, activity):
    '''
    cc must have already done the comparison
    '''
    if activity == 'Velo':
        return bool(cc.GetNumWarnings(2, "Velo(Left|Right)\\..*")), cc.GetAlignablesWithWarnings(2, "VeloLeft\\..*")
    if activity == 'Tracker':
        return bool(cc.GetNumWarnings(3) or cc.GetNumWarnings(2)>3), (cc.GetAlignablesWithWarnings(2), cc.GetAlignablesWithWarnings(3))


if __name__ == '__main__':

    activity = args.activity
   
    if args.activity == 'Velo':
        detectors = {'Velo' : ['VeloGlobal', 'VeloModules']}
    elif args.activity == 'Tracker':
        detectors = {'TT' : ['TTGlobal', 'TTModules'],
                     'IT' : ['ITGlobal', 'ITModules'],
                     'OT' : ['OTGlobal', 'OTModules'],}
    elif args.activity == 'Muon':
        detectors = {'Muon' : ['MuonGlobal', 'MuonModules']}

    for detector in detectors:
        for part in detectors[detector]:
            directory = os.path.join(args.new_alignment_dir, '{0}/{1}'.format(detector, part))
            if not os.path.exists(directory):
                os.makedirs(directory)
    
    runs = getListRuns(args.AligWork_dir)

    updatesRuns = [runs[0]]

    

    xml_path = os.path.join(args.AligWork_dir,'{activity}/run{run}/xml/{detector}/{part}.xml')
    xml_copy_path = os.path.join(args.new_alignment_dir,'{detector}/{part}/{run}.xml')

    updates = []
    
    for run in runs:
        cc = AlMon.CompareConstants()
        cc.Compare(std_vector([xml_path.format(activity=activity,detector = detector, part=part, run = updatesRuns[-1]) for detector in detectors for part in detectors[detector]]),
                   std_vector([xml_path.format(activity=activity,detector = detector, part=part, run = run) for detector in detectors for part in detectors[detector]]))
        flag, alignables_update = isUpdate(cc, activity)
        if flag:
            updatesRuns.append(run)
            updates.append((run,alignables_update))

    for run in updatesRuns:
        for detector in detectors:
            for part in detectors[detector]:
                shutil.copy(xml_path.format(**locals()), (xml_copy_path.format(**locals())))

    
    pickle.dump(updates, open('{0}.pkl'.format(activity), 'wb'))


    
    # # old method, work only for Velo, used as cross check andeverything works!
    
    # sys.path.append('/home/gdujany/Alignment/alignScripts/')
    # from diffXML import getDictDifference, bigChanges, dict_max_diff
    # # dict_max_diff.update({'Tx': 0.003, 'Ty': 0.003})

    # dofs = ['Tx', 'Ty', 'Tz', 'Rx', 'Ry', 'Rz']
    # list_max_diff = [dict_max_diff[key] for key in dofs]

    # xml_path = os.path.join(AligWork_dir,'Velo/run{0}/xml/Velo/VeloGlobal.xml')

    # updatesRuns = [runs[0]]
    # updates2 = []
    
    # for run in runs:
    #     diff = getDictDifference(xml_path.format(updatesRuns[-1]), xml_path.format(run))
    #     bigs_dict = bigChanges(diff, list_max_diff, ['VeloLeft'])
    #     if bigs_dict:
    #         updatesRuns.append(run)
    #         updates2.append((run, ';'.join(bigs_dict.keys())))

            
    # for run in updatesRuns:
    #     shutil.copy(xml_path.format(run), os.path.join(alignment_dir, str(run)+'.xml'))
