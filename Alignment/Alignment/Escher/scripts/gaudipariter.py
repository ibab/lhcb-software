#!/usr/bin/env python

# first parse all options
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] <opts_file> ...")
parser.add_option("-n","--numiter",type="int", dest="numiter",help="number of iterations", default=3)
parser.add_option("-f","--firstiter",type="int", dest="firstiter",help="first iterations", default=0)
parser.add_option("-e","--numevents",type="int", dest="numevents",help="number of events", default=-1)
parser.add_option("-p","--numprocesses",type="int", dest="numprocs",help="number of processes", default=8)
parser.add_option("-b","--baseDir", type='string', dest="basedir",help="directory to store output", default='AlignmentResults')
parser.add_option("-d","--aligndb", action = 'append', dest="aligndb",help="path to file with LHCBCOND database layer")
parser.add_option("--dddb", action = 'append', dest="dddb",help="path to file with DDDB database layer")
parser.add_option("-r", "--roothistofile",dest="histofile",help="name of histogram file",default = "histograms.root")
(opts, args) = parser.parse_args()

import os
rundir = os.getcwd()

if not os.path.isdir(opts.basedir):
    os.mkdir(opts.basedir)
os.chdir(opts.basedir)

for i in range(opts.firstiter,opts.numiter) :
    print "Iteration nr: ", i, "\n"
    # make the directory from where to run the job
    iterdir = 'Iter' + str( i )
    if os.path.isdir( iterdir ) :
        print "Directory exists. Will skip this iteration."
        continue
    
    os.mkdir ( iterdir )
    os.chdir ( iterdir )

    # beyond the first iteration, add the input database as an option
    theseoptions = ' --numevents ' + str(opts.numevents) + ' --numprocesses ' + str(opts.numprocs) + ' --iter ' + str(i) + ' -r ' + opts.histofile + ' '
    if i>0 :
        previterdb = '../Iter' + str( i-1 ) + '/Alignment.db'
        theseoptions += ' --aligndb ' + previterdb
    else :
        if opts.aligndb :
            for db in opts.aligndb :
                theseoptions += ' -d ' + db
    if opts.dddb :
        for db in opts.dddb :
            theseoptions += ' --dddb ' + db

    # add the remaining options
    for a in args:
        theseoptions += ' ' + a
        os.system( ' cp -f ' + a + ' .' )

    # run the job
    thiscommand = 'gaudipar.py' + theseoptions + '>& logfile.txt'
    print 'command: %s\n' % thiscommand
    os.system( thiscommand )
    os.system( 'gzip logfile.txt' )
    # keep only the last version of the derivatives. they take too much space.
    # os.system( 'mv -f myderivatives.dat ..')
    os.chdir(rundir+'/'+opts.basedir)

# create a single alignlog file
os.system('rm -f alignlog.txt')
os.system('cat Iter?/alignlog.txt Iter1?/alignlog.txt Iter2?/alignlog.txt > alignlog.txt')
os.chdir(rundir)
