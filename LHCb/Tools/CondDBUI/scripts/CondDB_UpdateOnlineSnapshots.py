#!/usr/bin/env python

from __future__ import with_statement

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__version__ = "$Id: CondDBAdmin_UpdateOnlineSnapshots.py,v 1.0 2011-01-10 00:00:00 ishapova Exp $"

import os, sys
import CondDBUI
from datetime import date
from subprocess import Popen,PIPE
import shlex

def _format_str(string, color_name, blink = False, underline = False):
    """Function to prettify strings."""
    color_vocab = {'black':30,'red':31,'green':32,'yellow':33,'blue':34,\
                   'magenta':35,'cyan':36,'white':37,'':''}
    ##########################################################################
    if color_name in color_vocab:
        color_num = str(color_vocab.get(color_name)) + ';'
    else:
        log.info("I don't know the requested color name '%s'. Known are: %s.\
        \nUsing 'White'.."%(color_name,color_vocab.keys()))
        color_num = ''
    ##########################################################################
    if blink: blink = '5;'
    else: blink = ''
    ##########################################################################
    if underline:
        underline = '4;'
    else:
        underline = ''
    ##########################################################################
    return '\x1B[' + '%s%s%s49m' %(color_num, blink, underline) + string + \
           '\x1B['+ '0m'

def _update_rel_notes(sqldddb_path,new_snapshots):
    """Function to add entry to release notes about updated ONLINE snapshots."""

    os.path.join(sqldddb_path, 'doc','release.notes')
    new_snapshots = reduce(lambda x,y: x + ', ' + y, new_snapshots)
    entry = """! %s - %s
 - ONLINE snapshots coverage is extended to include: %s.

 """ %(date.today().strftime('%Y-%m-%d'), sys.argv[0],new_snapshots)

    standard_file_name = os.path.join(sqldddb_path, 'doc','release.notes')
    temp_file_name = os.path.join(sqldddb_path, 'doc','release.notes~')

    try:
        with open(standard_file_name) as f:
            content = f.readlines()
            content.insert(10,entry)

        with open(temp_file_name,'w') as temp_file:
            temp_file.writelines(content)

        if os.path.isfile(temp_file_name):
            os.rename(temp_file_name,standard_file_name)
            log.info("Release notes at '%s' are updated." %standard_file_name)

    except Exception, ex:
        log.warning("Release notes file wasn't updated at '%s' due to: %s" %(standard_file_name,ex))

def _make_snapshots(since, until, digit_number):
    """Function to make snapshot for given 'since' and 'until' values.

    Returns True if was successful and False if wasn't.
    """

    date = since[:4] + (since[5]+since[6])*((digit_number-4)/2)

    standard_file_name = "ONLINE-%s.db"%date
    standard_file_path = os.path.join(os.environ["SQLITEDBPATH"],standard_file_name)
    new_file_name = "ONLINE-%s.db~"%date
    new_file_path = os.path.join(os.environ["SQLITEDBPATH"],new_file_name)

    command_line = "CondDBAdmin_MakeSnapshot.py " \
    "--options $SQLDDDBROOT/options/SQLDDDB-Oracle.py " \
    "-s %sUTC -u %sUTC ONLINE sqlite_file:$SQLITEDBPATH/%s/ONLINE" %(since,until,new_file_name)
    log.info(_format_str(command_line,'yellow'))
    try:
        proc_dump_sites = Popen(shlex.split(command_line), env = os.environ, stdout = PIPE)
        proc_dump_sites.wait()
        # When in subprocess errors should be parsed from stdout: ugly but works
        output = reduce(lambda x,y: x+y, proc_dump_sites.stdout.readlines(),'')
        if 'rror' in output or 'xception' in output:
            raise Exception, output
    except Exception, e:
        log.error("Snapshot '%s' creation failed: \n%s" %(standard_file_name,e))
        if os.path.isfile(new_file_path): os.remove(new_file_path)
        if os.path.isfile(new_file_path+"-journal"): os.remove(new_file_path+"-journal")
        if 'I/O' in output or 'unable to open database'in output:
            log.warning('Possible reason of the failure is lack of space to produce new snapshot.')

    if os.path.isfile(new_file_path):
        if os.path.isfile(standard_file_path):
            os.remove(standard_file_path)
            os.rename(new_file_path,standard_file_path)
            log.info(_format_str("'%s' snapshot is updated.\n" %standard_file_name,'green'))
        else:
            os.rename(new_file_path,standard_file_path)
            log.info(_format_str("New snapshot '%s' is created.\n" %standard_file_name,'green'))
    else:
        log.error(_format_str("'%s' snapshot wasn't successful.\n" % standard_file_name,'red',blink = True))
        return False

    return True

def main():
    # Configure the parser
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options]",
                          version = __version__,
                          description =
"""This script updates user's SQLDDDB ONLINE snapshots.
It detects used time splitting scheme of ONLINE snapshots
(per-year or per-month) and updates them to extend their coverage up
to current time. If no ONLINE snapshots are found it will generate them
from scratch. It updates also user's SQLDDDB release notes accordingly.""")

    parser.add_option("--sqldddb", type = "string",
                      help = "Path to SQLDDDB package ONLINE should be updated in. \
                      Default is $SQLDDDBROOT.")

    parser.set_default("sqldddb", os.environ["SQLDDDBROOT"])

    # Parse command line
    options, args = parser.parse_args()

    # Prepare local logger
    import logging
    global log
    log = logging.getLogger(parser.prog or os.path.basename(sys.argv[0]))
    log.setLevel(logging.INFO)

    # Set the global stream handler
    from CondDBUI import LOG_FORMAT
    hndlr = logging.StreamHandler()
    hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().handlers = [hndlr]

    # Decrease verbosity of PyCoolDiff
    import CondDBUI.PyCoolDiff
    CondDBUI.PyCoolDiff._log.setLevel(logging.WARNING)

    # Form and check arguments
    db_path = os.path.join(options.sqldddb, "db")
    if 'releases' in options.sqldddb:
        log.error("Released SQLDDDB can't be touched (%s)."
                  %options.sqldddb)
        return 1
    if not os.path.exists(db_path):
        log.error("Considered path '%s' doesn't exist." %db_path)
        return 1

    # Check granularity of ONLINE snapshots in SQLDDDB/db folder
    current_date = date.today()
    curr_year = current_date.year
    curr_month = current_date.month

    # Define user's time granularity of ONLINE snapshots
    online_files = [file for file in os.listdir(db_path)
                    if "ONLINE-" in file and len(file) < 17]

    if len(online_files) != 0:
        last_snap = max(online_files)
    # Ask to generate full set of new ONLINE snapshots
    else:
        ans = None
        while ans is None:
            ans = raw_input("\nNo ONLINE snapshots found. Do you want to generate full set of ONLINE snapshots (Yes,[No])? ")
            if not ans: ans = "No"
            if ans not in [ "Yes", "No" ]:
                print "You have to type exactly 'Yes' or 'No'"
                ans = None

        if ans == "No":
            print "Canceled by user. Snapshots weren't generated."
            return 0

        ans = None
        while ans is None:
            ans = raw_input("\nDo you want per-year or per-month granularity for your snapshots (Month,[Year])? ")
            if not ans: ans = "Year"
            if ans not in [ "Year", "Month" ]:
                print "You have to type exactly 'Year' or 'Month'"
                ans = None

        if ans == 'Year':
            last_snap = "ONLINE-2008.db"
        else:
            last_snap = "ONLINE-200806.db"

    last_snap_date = last_snap.lstrip("ONLINE-").rstrip(".db")
    if len(last_snap_date) == 4:
        snap_granular = "YEAR"
    elif len(last_snap_date) == 6:
        snap_granular = "MONTH"
    else:
        log.error("Can't define time granularity of ONLINE snapshots in $SQLITEDBPATH. \
        Check the snapshots naming scheme is standard.")
        return 1

    # Check how many snapshots do we need to produce for the detected granularity
    if snap_granular == "YEAR":
        delta_snap_years = curr_year - int(last_snap_date)
        number_of_new_snapshots = delta_snap_years + 1
    else:
        delta_snap_years = curr_year - int(last_snap_date[:4])
        delta_snap_months = curr_month - int(last_snap_date[4:]) + 12 * delta_snap_years
        number_of_new_snapshots = delta_snap_months + 1

    log.info("%s ONLINE snapshots found to be delivered to '%s'.\n"
             %(number_of_new_snapshots,options.sqldddb + '/db'))

    # Define 'since' and 'until' values ('year' and 'month' are date to snapshot)
    # and make snapshots
    new_snapshots = []
    if snap_granular == "YEAR":
        for s in range(number_of_new_snapshots):
            year = curr_year - s
            since = "%04d-01-01" %year
            until = "%04d-01-01" %(year + 1)

            # Make snapshot
            if _make_snapshots(since,until,4): new_snapshots.append(str(year))
            else:
                if len(new_snapshots):
                    new_snapshots_files = [os.path.join(db_path,'ONLINE-%s.db'%d)
                                           for d in new_snapshots]
                    for f in new_snapshots_files:
                        if os.path.isfile(f): os.remove(f)
                    log.info("ONLINE snapshots were not updated.")
                return 1
    else:
        for s in range(number_of_new_snapshots):
            delta = abs(s - curr_month)
            # Compute date for current year
            if curr_month > s:
                year = curr_year
                month = delta
            # Compute date for past years
            else:
                year = curr_year - delta//12 - 1
                month = 12 - (delta - 12*(delta//12))

            since = "%04d-%02d-01" %(year, month)
            # This 'if' clause is to take into account special case of 12th month
            if month != 12:
                until = "%04d-%02d-01" %(year, month + 1)
            else:
                until = "%04d-%02d-01" %(year + 1, 1)

            # Make snapshot
            if _make_snapshots(since,until,6): new_snapshots.append(str(year) + '-%02d'%month)
            else:
                if len(new_snapshots):
                    new_snapshots_files = [os.path.join(db_path,'ONLINE-%s.db'%d.replace('-',''))
                                           for d in new_snapshots]
                    for f in new_snapshots_files:
                        if os.path.isfile(f): os.remove(f)
                    log.info("ONLINE snapshots weren't updated.")
                return 1

    log.info('Snapshots are updated successfully! Delivered slices are: %s' %new_snapshots)
    _update_rel_notes(options.sqldddb,new_snapshots)

    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())