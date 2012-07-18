#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"

import os, sys
from CondDBUI import LOG_FORMAT, PyCoolDiff
from datetime import datetime
from subprocess import Popen, PIPE
from shlex import split
from functools import reduce
from re import match

def _format_str(string, color_name, blink = False, underline = False):
    """Function to prettify strings."""
    color_vocab = {'black':30,'red':31,'green':32,'yellow':33,'blue':34,
                   'magenta':35,'cyan':36,'white':37,'':''}
    ##########################################################################
    if color_name in color_vocab:
        color_num = str(color_vocab.get(color_name)) + ';'
    else:
        log.info("I don't know the requested color name '%s'. Known are: %s."
        "\nUsing 'White'.."%(color_name,color_vocab.keys()))
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
 - ONLINE snapshots updated/added on %s: %s.

""" %(datetime.today().strftime('%Y-%m-%d'), os.path.basename(sys.argv[0]),
      datetime.today().strftime('%H:%M:%S'), new_snapshots)

    standard_file_name = os.path.join(sqldddb_path, 'doc', 'release.notes')
    temp_file_name = os.path.join(sqldddb_path, 'doc', 'release.notes~')
    if not os.path.isfile(standard_file_name): return

    try:
        with open(standard_file_name) as f:
            content = f.readlines()
            content.insert(10, entry)

        with open(temp_file_name, 'w') as temp_file:
            temp_file.writelines(content)

        if os.path.isfile(temp_file_name):
            os.rename(temp_file_name, standard_file_name)
            log.info("Release notes at '%s' are updated." %standard_file_name)

    except Exception as ex:
        log.warning("Release notes file wasn't updated at '%s' due to: %s" %(standard_file_name, ex))

def _make_snapshots(since, until, digit_number):
    """Function to make snapshot for given 'since' and 'until' values.

    Returns True if snapshoting is successful and False otherwise.
    """

    fdate = since[:4] + (since[5] + since[6]) * ((digit_number - 4)//2)

    standard_file_name = "ONLINE-%s.db"%fdate
    standard_file_path = os.path.join(db_path, standard_file_name)
    currentDateTime = datetime.today().strftime('%Y-%m-%d_%H:%M:%S')
    new_file_name = standard_file_name + '_tmp-' + currentDateTime + '~'
    new_file_path = os.path.join(db_path, new_file_name)

    command_line = ("CondDBAdmin_MakeSnapshot.py --options %s/options/SQLDDDB-Oracle.py "
    "-s %sUTC -u %sUTC ONLINE sqlite_file:%s/ONLINE" %(dest_sqldddb, since, until, new_file_path))
    log.info(_format_str(command_line,'yellow'))
    try:
        snapshotProc = Popen(split(command_line), env = os.environ, stdout = PIPE)
        snapshotProc.wait()
        # When in subprocess errors should be parsed from stdout: ugly but works
        output = reduce(lambda x,y: x+y, snapshotProc.stdout.readlines(),'')
        if 'rror' in output or 'xception' in output:
            raise Exception, output
    except Exception as e:
        log.error("Snapshot '%s' creation failed: %s" %(standard_file_name, e))
        if os.path.isfile(new_file_path): os.remove(new_file_path)
        if os.path.isfile(new_file_path + "-journal"): os.remove(new_file_path + "-journal")
        if 'output' in locals() and ('I/O' in output or 'unable to open database' in output):
            log.warning("Possible reason of the failure is the lack of disk "
                        "space to produce the new snapshot.")
        return False

    if os.path.isfile(new_file_path):
        if os.path.isfile(standard_file_path):
            log.info(_format_str("Updating '%s' snapshot..."%standard_file_name, 'green'))
        else:
            log.info(_format_str("Creating '%s' snapshot..."%standard_file_name, 'green'))
        try:
            os.rename(new_file_path, standard_file_path)
            log.info(_format_str("Done.", 'green'))
        except OSError as er:
            log.error(_format_str("Renaming temporary snapshot file to the persistent one has failed.",
                                  'red', blink = True))
            log.error(er)
            return False
    else:
        log.error(_format_str("'%s' snapshotting has failed.\n"
                              %standard_file_name, 'red', blink = True))
        return False

    return True

def main():
    # Configure the parser
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options]",
                          description =
"""This script updates ONLINE SQLite snapshots from Oracle server.
It detects used time splitting scheme of ONLINE snapshots at destination
(per-year or per-month) and updates them to extend their coverage up
to current time. If no ONLINE snapshots are found it will generate them
from scratch.""")

    parser.add_option("-d", "--destination", type = "string",
                      help = "Path to SQLDDDB package ONLINE should be updated/regenerated in. "
                      "Default is $SQLDDDBROOT.")
    parser.add_option("-b", "--batch", action = "store_true",
                      help = "Early choice to regenerate *yearly* snapshots if "
                      "no snapshots are found at all")
    parser.add_option("--rel", action = "store_true",
                      help = "Make a record in the *release.notes* file about the update.")


    parser.set_default("destination", os.environ["SQLDDDBROOT"])

    # Parse command line
    options, args = parser.parse_args()

    # Prepare local logger
    import logging
    global log
    log = logging.getLogger(parser.prog or os.path.basename(sys.argv[0]))
    log.setLevel(logging.INFO)

    # Set the global stream handler
    hndlr = logging.StreamHandler()
    hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().handlers = [hndlr]

    # Decrease verbosity of PyCoolDiff
    PyCoolDiff._log.setLevel(logging.WARNING)

    # Form and check arguments
    if not os.path.exists(options.destination):
        log.error("Considered path '%s' doesn't exist." %options.destination)
        return 1
    global dest_sqldddb, db_path
    dest_sqldddb = options.destination.rstrip('/')
    db_path = os.path.join(dest_sqldddb, "db")
    if 'releases' in dest_sqldddb:
        log.error("Released SQLDDDB must not be touched (%s)."
                  %dest_sqldddb)
        return 1
    if not os.path.exists(db_path):
        log.error("Considered path '%s' doesn't exist." %db_path)
        return 1

    # Check granularity of ONLINE snapshots in SQLDDDB/db folder
    current_date = datetime.today()
    curr_year = current_date.year
    curr_month = current_date.month

    # Define user's time granularity of ONLINE snapshots
    online_files = [file for file in os.listdir(db_path)
                    if match(r"^ONLINE-(\d{4})(\d{2})?\.db$", file)]

    if len(online_files) != 0:
        last_snap = max(online_files)
    # Ask to generate full set of new ONLINE snapshots
    else:
        ans = 'Yes' if options.batch else None
        while ans is None:
            ans = raw_input("\nNo ONLINE snapshots found. Do you want to generate "
            "full set of ONLINE snapshots (Yes,[No])? ")
            if not ans: ans = "No"
            if ans not in [ "Yes", "No" ]:
                print "You have to type exactly 'Yes' or 'No'"
                ans = None

        if ans == "No":
            print "Canceled by user. Snapshots weren't generated."
            return 0

        ans = 'Year' if options.batch else None
        while ans is None:
            ans = raw_input("\nDo you want per-year or per-month granularity for"
                            " your snapshots (Month,[Year])? ")
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
        log.error("Can't define time granularity of ONLINE snapshots at '%s'. "
        "Check the snapshots naming scheme is standard." %db_path)
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
             %(number_of_new_snapshots, db_path))

    # Define 'since' and 'until' values ('year' and 'month' are date to snapshot)
    # and make snapshots
    new_snapshots = []
    if snap_granular == "YEAR":
        for s in range(number_of_new_snapshots):
            year = curr_year - s
            since = "%04d-01-01" %year
            until = "%04d-01-01" %(year + 1)

            # Make snapshot
            if _make_snapshots(since, until, 4): new_snapshots.append(str(year))
            else:
                if len(new_snapshots):
                    new_snapshots_files = [os.path.join(db_path, 'ONLINE-%s.db'%d)
                                           for d in new_snapshots]
                    log.warning("Not all of ONLINE snapshots were updated,"
                                "the ones updated are: %s" %new_snapshots_files)
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
                month = 12 - (delta - 12 * (delta//12))

            since = "%04d-%02d-01" %(year, month)
            # This 'if' clause is to take into account special case of 12th month
            if month != 12:
                until = "%04d-%02d-01" %(year, month + 1)
            else:
                until = "%04d-%02d-01" %(year + 1, 1)

            # Make snapshot
            if _make_snapshots(since, until, 6):
                new_snapshots.append(str(year) + '-%02d'%month)
            else:
                if len(new_snapshots):
                    new_snapshots_files = [os.path.join(db_path,'ONLINE-%s.db'%d.replace('-',''))
                                           for d in new_snapshots]
                    log.warning("Not all of ONLINE snapshots were updated,"
                                "the ones updated are: %s" %new_snapshots_files)
                return 1

    log.info('Snapshots are updated successfully! Delivered slices are: %s' %new_snapshots)
    if options.rel: _update_rel_notes(dest_sqldddb, new_snapshots)

    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())