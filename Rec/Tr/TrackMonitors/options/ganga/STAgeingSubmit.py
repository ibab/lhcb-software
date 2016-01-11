##############################################################################
#                                                                            #
#  Ganga submission script to extract CCE scans for IT and TT                #
#                                                                            #
#  @package Tr/TrackMonitors                                                 #
#  @author  Ch. Elsasser, E. Graverini                                       #
#  @date    15/06/2013                                                       #
#                                                                            #
##############################################################################

import os

# Insert your email address.
# Leave empty if no notifications are desired
notify_address = ''

# List runs belonging to fills
runDict = {
    4643 : [168257],                                                # 2015-11-22 900k evt/step
    4638 : [168092, 168139],                                        # 2015-11-20 500k evt/step. Actually 2nd run is in fill 4639!
    4518 : [166262],                                                # 2015-10-20 Bad (OT was off)
    3960 : [156887],                                                # 2015-07-05 Bad (OT was off)
    3820 : [153583, 153584, 153586, 153587, 153601, 153602],        # 2015-06-03 Bad (some steps are missing)
    3478 : [135603, 135604, 135605],                                # 2013-01-21
    3108 : [129494, 129518, 129519],                                # 2012-09-28
    2797 : [120006, 120009, 120010],                                # 2012-07-02
    2472 : [111267, 111270, 111271],                                # 2012-04-05
    2252 : [104090, 104091, 104092, 104093, 104106, 104107],        # 2011-10-25
    2083 : [101357, 101359],                                        # 2011-09-07
    1944 : [ 95930,  95932,  95933,  95934,  95936],                # 2011-07-14
    1616 : [ 87144,  87146,  87147,  87151,  87152,  87153],        # 2011-03-14
    1020 : [ 69505,  69506,  69507,  69508,  69509,  69510,
                     69511,  69513,  69517,  69521,  69525,  69526,
                     69528,  69531,  69555,  69556,  69558,  69570,
                     69572,  69573,  69574,  69575,  69576,  69577,
                     69578,  69579,  69580,  69583,  69586,
                     69589,  69592,  69593,  69594,  69595,  69596,
                     69597,  69599,  69602,  69603,  69604,  69605,
                     69606,  69607,  69608,  69610,  69611],        # 2010-04-04
    0    : [129494]
}

# FOR NEW DATA:
# just update the run dictionary and run for ONE fill at a time
fills = []

print "=============================== SUBMITTING TT ==============================="
for k in fills:
    runs = runDict[k]
    print "====================================="
    print "Fill: " + str(k)
    for i in runs:
        print "====================================="
        j = Job(application='Vetra', name="TT-" + str(i))
        print "Run: " + str(i)
        j.application.user_release_area = os.path.expandvars('$User_release_area')
        j.application.optsfile = os.path.expandvars('$User_release_area') + "/Vetra_v16r0/Tr/TrackMonitors/options/TTAgeing.py"
        j.application.version = 'v16r0'
        bkq = BKQuery(type='Run', dqflag='All')
        bkq.path = "/" + str(i) + "/Real Data/90000001/RAW"
        print "BK Query path: " + bkq.path
        j.inputdata = bkq.getDataset()
        print "Input data contain " + str(len(j.inputdata.getFileNames())) + " file(s)"
        j.backend = Dirac()
        j.backend.settings['CPUTime'] = 2 * 172800
        j.splitter = SplitByFiles(filesPerJob=5,
                                  ignoremissing=True,
                                  bulksubmit=True)
        j.outputfiles = [DiracFile('Tuple.root')]
        if notify_address:
            j.postprocessors = [Notifier(address=notify_address)]
        j.submit()
        print "Job submitted!"
        print "====================================="
        print ""
    print ""


print "=============================== SUBMITTING IT ==============================="
for k in fills:
    runs = runDict[k]
    print "====================================="
    print "Fill: " + str(k)
    for i in runs:
        print "====================================="
        j = Job(application='Vetra', name="IT-" + str(i))
        print "Run: " + str(i)
        j.application.user_release_area = os.path.expandvars('$User_release_area')
        j.application.optsfile = os.path.expandvars('$User_release_area') + "/Vetra_v16r0/Tr/TrackMonitors/options/ITAgeing.py"
        j.application.version = 'v16r0'
        bkq = BKQuery(type='Run', dqflag='All')
        bkq.path = "/" + str(i) + "/Real Data/90000001/RAW"
        print "BK Query path: " + bkq.path
        j.inputdata = bkq.getDataset()
        print "Input data contain " + str(len(j.inputdata.getFileNames())) + " file(s)"
        j.backend = Dirac()
        j.backend.settings['CPUTime'] = 2 * 172800
        j.splitter = SplitByFiles(filesPerJob=5,
                                  ignoremissing=True,
                                  bulksubmit=True)
        j.outputfiles = [DiracFile('Tuple.root')]
        if notify_address:
            j.postprocessors = [Notifier(address=notify_address)]
        j.submit()
        print "Job submitted!"
        print "====================================="
        print ""
    print ""
