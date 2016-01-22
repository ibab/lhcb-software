################################################################################################


jobs = {
  "B2CC"      : 1204, 
  "B2OC"      : 1383, 
  "BandQ"     : 1206, 
  "Charm"     : 1207, 
  "BnoC"      : 1208, 
  "QEE"       : 1209, 
  "RD"        : 1382, 
  "SL"        : 1211, 
  "Calib"     : 1212
}

dumpDetailedTable = True  # Dump the table with lines' retentions in twiki format
triggerRate = 5.e3        # Trigger rate for BW calculation in Hz
daVinciVersion = 'v38r0'   # DaVinci version (for log file name)
gangadir = '/afs/cern.ch/work/a/acontu//gangadir/workspace/acontu/LocalXML/' # Location of ganga job logs

streamEventSize = {
  "B2CC"      : {'Bhadron' : 0.,
                 'BhadronCompleteEvent' : 0.,
                 'Calibration' : 0.,
                 'Charm' : 0.,
                 'CharmCompleteEvent' : 0.,
                 'Dimuon' : 0.,
                 'EW' : 0.,
                 'Leptonic' : 0.,
                 'EW' : 0.,
                 'Radiative' : 0.,
                 'Semileptonic' : 0.,
                 'MDST.DST' : 0.
                },
  "B2OC"      : {'Bhadron' : 0.,
                 'BhadronCompleteEvent' : 0.,
                 'Calibration' : 0.,
                 'Charm' : 0.,
                 'CharmCompleteEvent' : 0.,
                 'Dimuon' : 0.,
                 'EW' : 0.,
                 'Leptonic' : 0.,
                 'EW' : 0.,
                 'Radiative' : 0.,
                 'Semileptonic' : 0.,
                 'MDST.DST' : 0.
                 },
  "BandQ"     : {'Bhadron' : 0.,
                 'BhadronCompleteEvent' : 0.,
                 'Calibration' : 0.,
                 'Charm' : 0.,
                 'CharmCompleteEvent' : 0.,
                 'Dimuon' : 0.,
                 'EW' : 0.,
                 'Leptonic' : 0.,
                 'EW' : 0.,
                 'Radiative' : 0.,
                 'Semileptonic' : 0., 
                 'MDST.DST' : 0. 
                 },
  "Charm"     : {'Bhadron' : 0.,
                'BhadronCompleteEvent' : 0.,
                'PID' : 0.,
                'MiniBias' : 0.,
                'Calibration' : 0.,
                'Charm' : 0.,
                'CharmCompleteEvent' : 0.,
                'Dimuon' : 0.,
                'EW' : 0.,
                'Leptonic' : 0.,
                'Radiative' : 0.,
                'Semileptonic' : 0.,
                'MDST.DST': 0.},
  "BnoC" : {'Bhadron' : 0.,
                 'BhadronCompleteEvent' : 0.,
                 'Calibration' : 0.,
                 'Charm' : 0.,
                 'CharmCompleteEvent' : 0.,
                 'Dimuon' : 0.,
                 'EW' : 0.,
                 'Leptonic' : 0.,
                 'EW' : 0.,
                 'Radiative' : 0.,
                 'Semileptonic' : 0., 
                 'MDST.DST' : 0. 
                },
  "QEE"       : {'Bhadron' : 0.,
                'BhadronCompleteEvent' : 0.,
                'PID' : 0.,
                'MiniBias' : 0.,
                'Calibration' : 0.,
                'Charm' : 0.,
                'CharmCompleteEvent' : 0.,
                'Dimuon' : 0.,
                'EW' : 0.,
                'Leptonic' : 0.,
                'Radiative' : 0.,
                'Semileptonic' : 0. ,
                'MDST.DST': 0.},
  "RD"        : {'Bhadron' : 0.,
                'BhadronCompleteEvent' : 0.,
                'PID' : 0.,
                'MiniBias' : 0.,
                'Calibration' : 0.,
                'Charm' : 0.,
                'CharmCompleteEvent' : 0.,
                'Dimuon' : 0.,
                'EW' : 0.,
                'Leptonic' : 0.,
                'Radiative' : 0.,
                'Semileptonic' : 0.,
                'MDST.DST': 0. },
  "SL"        : {'Bhadron' : 0.,
                'BhadronCompleteEvent' : 0.,
                'PID' : 0.,
                'MiniBias' : 0.,
                'Calibration' : 0.,
                'Charm' : 0.,
                'CharmCompleteEvent' : 0.,
                'Dimuon' : 0.,
                'EW' : 0.,
                'Leptonic' : 0.,
                'Radiative' : 0.,
                'Semileptonic' : 0.,
                'MDST.DST': 0. },
  "Calib"     : {'Bhadron' : 0.,
                'BhadronCompleteEvent' : 0.,
                'PID' : 0.,
                'MiniBias' : 0.,
                'Calibration' : 0.,
                'Charm' : 0.,
                'CharmCompleteEvent' : 0.,
                'Dimuon' : 0.,
                'EW' : 0.,
                'Leptonic' : 0.,
                'Radiative' : 0.,
                'Semileptonic' : 0.,
                'MDST.DST': 0. }
}



#################################################################################################

import os
from copy import copy
from math import sqrt

#streams = sorted(streamEventSize['Charmless'].keys())

#['Bhadron', 'BhadronCompleteEvent', 'Calibration', 'Charm',
#           'CharmCompleteEvent', 'CharmToBeSwum', 'Dimuon', 'EW',
#           'Leptonic', 'MiniBias', 'PID', 'Radiative', 'Semileptonic']

streamRate = {}
eventSize = {}

dd = {}

for (wg, job) in jobs.iteritems() : 
  d = {}

  path = gangadir + str(job)
  files = os.listdir(path)
  nfiles = 0
  curr_stream = None
  nevents = 0
#  print job
#  print files
  for filename in files : 
#    print filename
#    print job
    reachedLastEvent = False
    foundStrippingReport = False

    fullname = path + '/' + filename + '/output/stdout'
    if not os.path.isfile(fullname) : continue
    f = file(fullname)

    nfiles += 1
    
#    print fullname
    
    for l in f : 
        if l.find('Xeon') > 0 : 
#            print l
            speed = float(l.split()[8])
#            print speed
        if l.find('Application Manager Stopped successfully') > 0 : 
            reachedLastEvent = True
            continue
#        if l.find('Stripping') <= 0 : continue
        if reachedLastEvent : 
#    	    print l
            if l.find('StrippingReport') == 0: 
                #print l
                #if "[min]" not in str(l.split()[6]) and ("[ms]" not in str(l.split()[6])):
                nevents += int(l.split()[6])
                    #print l
                    #print "ciccio"
                #else:
                 #   print l
            if l.find('StrippingGlobal') > 0 : 
                foundStrippingReport = True
                ls = l.split('|')
 #               print l
                if 'ALL' not in d.keys() :  
                    d['ALL'] = [ int(ls[3]), float(ls[5])*speed/1.5, [], {}, 0, 0 ]
                else : 
                    d['ALL'][0] += int(ls[3])
                    d['ALL'][1] += float(ls[5])*speed/1.5
            elif foundStrippingReport and l.find('StrippingSequenceStream') > 0 : 
                ls = l.split('|')
                name = ls[1].strip().rstrip('_')[24:]
                curr_stream = name
#                    print ls
                nev = int(ls[3])
#		if name == 'MiniBias' : nev = float(nev)/100.
                if name not in d.keys() : 
                    d[name] = [ nev, float(ls[5])*speed/1.5, [], {}, 0, 0 ]
                else : 
                    d[name][0] += nev
                    d[name][1] += float(ls[5])*speed/1.5
            elif foundStrippingReport and l.find('Stripping') > 0 : 
                #print l
                ls = l.split('|')
                name = ls[1].strip()[1:]
                #print ls[3]
                nev = int(ls[3])
                if len(ls[5].strip())>1 : 
                    time = float(ls[5])*speed/1.5
                else :
                    time = 0
                    
                if name not in d[curr_stream][2] : 
                    d[curr_stream][2] += [ name ]
                    d[curr_stream][3][name] = [ nev, time, 0, 0 ]
                else : 
                    #print d[curr_stream][3][name][0]
                    #print nev
            	    d[curr_stream][3][name][0] += nev
            	    #print d[curr_stream][3][name][0]
            	    d[curr_stream][3][name][1] += time
            elif foundStrippingReport : 
#                print l 
                break
    f.close()

  print '%10s: job %4d, events=%10d, files=%4d' % (wg, job, nevents, nfiles)

  if nevents == 0 : continue

  for k in d.keys() : 
    d[k][4] = sqrt(float(d[k][0]))/float(nevents)
    d[k][5] = d[k][0]
    d[k][0] /= float(nevents)
    d[k][1] /= float(nfiles)
    for sl in d[k][2] : 
        d[k][3][sl][2] = sqrt(float(d[k][3][sl][0]))/float(nevents)
        d[k][3][sl][3] = d[k][3][sl][0]
        d[k][3][sl][0] /= float(nevents)
        d[k][3][sl][1] /= float(nfiles)

  dd[wg] = copy(d)

#f = open('wg_table.tex', 'w')

#s = '%16s ' % 'WG / Stream'
#s2 = '%16s ' % 'Event size, kb'
#for stream in streamEventSize['BnoC'].keys() :
    #s += '& \\begin{sideways}%5s\\end{sideways}' % stream
    #s2 += '& %5.1f' % streamEventSize['BnoC'][stream]

##    if stream not in streamRate.keys() : 
##        eventSize[stream] = [ streamEventSize[stream] ]
##        streamRate[stream] = [ d['ALL'][stream][0] ]
##    else : 
##        eventSize[stream] += [ streamEventSize[stream] ]
##        streamRate[stream] += [ d['ALL'][stream][0] ]

#s += '& \\begin{sideways}%5s\\end{sideways}' % 'Total, Mb/s\n'
#s2 += '&       \n'
#s += '& \\begin{sideways}%5s\\end{sideways} \\\\' % 'Time, ms/event\n'
#s2 += '&       \\\\\n'
#f.write(s)
#f.write('\\hline\n')
#f.write(s2)
#f.write('\\hline\n')

#for wg in sorted(jobs.keys()) :
    #if wg not in dd.keys() : continue

    #tot_bw = 0.
    #s = '%16s ' % wg

##    print dd[wg].keys()

    #for stream in streamEventSize[wg].keys() :
        #if stream in dd[wg].keys() :
            #retention = dd[wg][stream][0]
            #bw = streamEventSize[wg][stream]*retention*triggerRate/1e3
            ##print stream
            #if stream=="MDST.DST": bw = bw*0.25
            #print "yeah"
            #print stream
            #print wg
            #print bw
            #print retention
            #print streamEventSize[wg][stream]
            #tot_bw += bw
            #s += '& %5.2f' % bw
        #else :
            #s += '&      '
    #if 'ALL' in dd[wg].keys() : 
        #s += '& %5.2f & %5.1f \\\\' % (tot_bw, dd[wg]['ALL'][1])
        #if wg == 'All' : print '\\hline'
    #else : 
        #s += '&       &       \\\\'
    #f.write(s + '\n')

#f.close()

#if dumpDetailedTable : 

    #f = open('line_retentions.txt', 'w')
    #f2 = open('large_retention.txt', 'w')
    #f2.write(' |*%-19s*|*%-19s*|*%-49s*|*N evnts*|*Rate, %%*|\n' % ('WG', 'Stream', 'Line name') )

    #for wg in sorted(jobs.keys()) : 
        #if wg not in dd.keys() : continue
        #if wg == 'All' : continue
        #f.write('---+ ' + wg)
        #f.write('\n\n')
        #for stream in streamEventSize[wg].keys() : 
            #if stream in dd[wg].keys() : 
##            print ' |_%-50s|         |        |' % (stream+'_')
                #f.write('---+++ ' +  stream + '\n')
                #f.write(' |*%-49s*|*N evnts*|*Rate, %%*|*ms/evt*|\n' % 'Line name')
                #for sl in dd[wg][stream][2] : 
                    #f.write(' |!%-50s| %7d | %7.4f | %6.3f |\n' % (sl, dd[wg][stream][3][sl][0]*(nevents), dd[wg][stream][3][sl][0]*100, dd[wg][stream][3][sl][1]))
                    #if stream in ['BhadronCompleteEvent', 'CharmCompleteEvent', 'Dimuon', 'Radiative', 'Semileptonic', 'EW'] and \
                       #dd[wg][stream][3][sl][0]*100 > 0.05 : 
                       #f2.write('  |!%-20s|!%-20s|!%-50s| %7d | %7.4f |\n' % (wg, stream, sl, dd[wg][stream][3][sl][0]*(nevents), dd[wg][stream][3][sl][0]*100) )
                #f.write('\n')
        #f.write('\n')
        #f2.write('\n')
        
    #f.close()
    #f2.close()

f = open('wg_table.tex', 'w')

s = '%16s ' % 'WG / Stream'
s2 = '%16s ' % 'Event size, kb'
for stream in streamEventSize['Charm'].keys() :
    s += '& \\begin{sideways}%5s\\end{sideways}' % stream
    s2 += '& %5.1f' % streamEventSize['Charm'][stream]

#    if stream not in streamRate.keys() : 
#        eventSize[stream] = [ streamEventSize[stream] ]
#        streamRate[stream] = [ d['ALL'][stream][0] ]
#    else : 
#        eventSize[stream] += [ streamEventSize[stream] ]
#        streamRate[stream] += [ d['ALL'][stream][0] ]

s += '& \\begin{sideways}%5s\\end{sideways}' % 'Total, Mb/s\n'
s2 += '&       \n'
s += '& \\begin{sideways}%5s\\end{sideways} \\\\' % 'Time, ms/event\n'
s2 += '&       \\\\\n'
f.write(s)
f.write('\\hline\n')
f.write(s2)
f.write('\\hline\n')

for wg in sorted(jobs.keys()) :
    if wg not in dd.keys() : continue

    tot_bw = 0.
    s = '%16s ' % wg

#    print dd[wg].keys()

    for stream in streamEventSize[wg].keys() :
        if stream in dd[wg].keys() :
            retention = dd[wg][stream][0]
            bw = streamEventSize[wg][stream]*retention*triggerRate/1e3
            #print stream
            if stream=="MDST.DST": bw = bw*0.25
            #print "yeah"
            #print stream
            #print wg
            #print bw
            #print retention
            #print streamEventSize[wg][stream]
            tot_bw += bw
            s += '& %5.2f' % bw
        else :
            s += '&      '
    if 'ALL' in dd[wg].keys() : 
        s += '& %5.2f & %5.1f \\\\' % (tot_bw, dd[wg]['ALL'][1])
        if wg == 'All' : print '\\hline'
    else : 
        s += '&       &       \\\\'
    f.write(s + '\n')

f.close()

if dumpDetailedTable : 

    f = open('line_retentions.txt', 'w')
    f2 = open('large_retention.txt', 'w')
    f2.write(' |*%-19s*|*%-19s*|*%-49s*|*N evnts*|*Rate, %%*|\n' % ('WG', 'Stream', 'Line name') )

    for wg in sorted(jobs.keys()) : 
        if wg not in dd.keys() : continue
        if wg == 'All' : continue
        f.write('---+ ' + wg)
        f.write('\n\n')
        for stream in streamEventSize[wg].keys() : 
            if stream in dd[wg].keys() : 
#            print ' |_%-50s|         |        |' % (stream+'_')
                f.write('---+++ ' +  stream + '\n')
                f.write(' |*%-49s*|*N evnts*|*Rate, %%*|*ms/evt*|\n' % 'Line name')
                for sl in dd[wg][stream][2] : 
                    #print dd[wg][stream][3][sl][0]
                    #print nevents
                    #print dd[wg][stream][3][sl][0]*nevents
                    f.write(' |!%-50s| %7d | %7.5f | %6.3f |\n' % (sl, dd[wg][stream][3][sl][0]*nevents, dd[wg][stream][3][sl][0]*100, dd[wg][stream][3][sl][1]))
                    if stream in ['BhadronCompleteEvent', 'CharmCompleteEvent', 'Dimuon', 'Radiative', 'Semileptonic', 'EW'] and \
                       dd[wg][stream][3][sl][0]*100 > 0.05 : 
                       f2.write('  |!%-20s|!%-20s|!%-50s| %7d | %7.5f |\n' % (wg, stream, sl, dd[wg][stream][3][sl][0]*(nevents), dd[wg][stream][3][sl][0]*100) )
                f.write('\n')
        f.write('\n')
        f2.write('\n')
        
    f.close()
    f2.close()
