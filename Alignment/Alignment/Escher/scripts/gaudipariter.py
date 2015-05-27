#!/usr/bin/env python

# first parse all options
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] <opts_file> ...")
parser.add_option("-n","--numiter",type="int", dest="numiter",help="number of iterations", default=3)
parser.add_option("-f","--firstiter",type="int", dest="firstiter",help="first iterations", default=0)
parser.add_option("-e","--numevents",type="int", dest="numevents",help="number of events", default=-1)
parser.add_option("-p","--numprocesses",type="int", dest="numprocs",help="number of processes", default=8)
parser.add_option("-b","--baseDir", type='string', dest="basedir",help="directory to store output", default='AlignmentResults')
parser.add_option("-d","--aligndb", action = 'append', dest="aligndb",help="path to file with CALIBOFF/LHCBCOND/SIMCOND database layer that will only be used in first iter")
parser.add_option("--conddb", action = 'append', dest="conddb",help="path to file with CALIBOFF/LHCBCOND/SIMCOND database layer that will be used in all iterations")
parser.add_option("--dddb", action = 'append', dest="dddb",help="path to file with DDDB database layer")
parser.add_option("-r", "--roothistofile",dest="histofile",help="name of histogram file",default = "histograms.root")
parser.add_option("--dryrun", action="store_true",help="dont do anything")
parser.add_option("-s", "--simulation",action="store_true",dest="simtag",help="activate if running with MC",default=False)
parser.add_option("-l", "--lhcbcond",action="store_true",dest="lhcbcondtag",help="activate if constants in lhcbcondDB",default=False)
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
    if opts.dddb :
        for db in opts.dddb :
            theseoptions += ' --dddb ' + db
    if opts.conddb :
        for db in opts.conddb :
            theseoptions += ' --aligndb ' + db
    if i>0 :
        previterdb = '../Iter' + str( i-1 ) + '/Alignment.db'
        theseoptions += ' --aligndb ' + previterdb
    else :
        if opts.aligndb :
            for db in opts.aligndb :
                theseoptions += ' --aligndb ' + db

    if opts.simtag:
        theseoptions += ' -s '
        
    if opts.lhcbcondtag:
        theseoptions += ' -l '
    # add the remaining options
    for a in args:
        theseoptions += ' ' + a
        os.system( ' cp -f ' + a + ' .' )

    # run the job
    thiscommand = 'gaudipar.py' + theseoptions + '>& logfile.txt'
    print 'command: %s\n' % thiscommand
    if not opts.dryrun:
        os.system( thiscommand )
        os.system( 'gzip logfile.txt' )
        # keep only the last version of the derivatives. they take too much space.
        # os.system( 'mv -f myderivatives.dat ..')
    os.chdir(rundir+'/'+opts.basedir)

# create a single alignlog file
os.system('rm -f alignlog.txt')
os.system('cat Iter?/alignlog.txt Iter1?/alignlog.txt Iter2?/alignlog.txt > alignlog.txt')
os.chdir(rundir)
