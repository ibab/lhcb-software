#!/usr/bin/env python

__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: CondDBAdmin_MakeSnapshot.py,v 1.9 2009-06-30 14:55:08 marcocle Exp $"

import os, sys

# Fix the problem with multiple logger handlers
_hndlr = None
def _fixLogger():
    global _hndlr
    import logging
    if _hndlr is None:
        from CondDBUI import LOG_FORMAT
        _hndlr = logging.StreamHandler()
        _hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().handlers = [ _hndlr ]

isSQLiteFile = lambda path: os.path.isfile(path) and (open(path, "rb").read(6) == "SQLite")

def guessConnectionString(partition, options):
    import re
    known_partitions = [ "DDDB", "LHCBCOND", "SIMCOND", "ONLINE" ]
    if partition in known_partitions or re.match(r"ONLINE_[0-9]{6}", partition):
        # Try to extract the connection string from the default configuration
        from Gaudi.Configuration import importOptions, allConfigurables
        _fixLogger()
        for opt in options:
            importOptions(opt)
        if partition in allConfigurables \
            and hasattr(allConfigurables[partition],"ConnectionString"):
            return allConfigurables[partition].ConnectionString
    # No configurable found
    if isSQLiteFile(partition):
        # it is an SQLite file, let's try to guess the partition name
        exp = re.compile(r"([A-Z][A-Z0-9_]{0,7})\.db")
        filename = os.path.basename(partition)
        m = exp.match(filename)
        if m:
            # the filename is of type LHCBCOND.db, let's try with:
            # sqlite_file:/path/to/FILE.db/FILE
            return "sqlite_file:%s/%s" % (partition, m.group(1))
        else:
            # let's see if filename contains the name of a know partition
            for p in known_partitions:
                if p in filename:
                    # so if the file is something like "DDDB_changes.db" use DDDB
                    return "sqlite_file:%s/%s" % (partition, p)
    # No further guessing, let's assume that we are dealing with a connection string
    return partition

def main(argv):
    # Configure the parser
    from optparse import OptionParser

    default_options_file = os.path.join(os.environ["SQLDDDBROOT"], "options", "SQLDDDB.py")

    parser = OptionParser(usage = "%prog [options] partition destination",
                          version = __version__,
                          description =
"""This script produces a snapshot of the specified partition of the CondDB.
The argument 'partition' can be the partition name (the connection string is
guessed from standard or provided job options), an SQLite file or a COOL
connection string. 'destination' must be a connection string.""")
    parser.add_option("-T", "--tag", type = "string", action = "append",
                      dest = "tags",
                      help = "Tag to use for the snapshot. [default = %default]"
                      )
    parser.add_option("--options", type = "string", action = "append",
                      help = "Options files to use to guess the mapping from the "
                             "partition name to the actual connection string. "
                             "[default = %s]" % default_options_file)
    parser.add_option("-s", "--since", type = "string",
                      help = "Start of the interesting Interval Of Validity (local time). "
                             "Format: YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]")
    parser.add_option("-u", "--until", type = "string",
                      help = "End of the interesting Interval Of Validity (local time) "
                             "Format: YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]")
    parser.add_option("--merge", action = "store_true",
                      help = "Whether to merge the data into an existing DB")
    parser.add_option("-F", "--to-files", action = "store_true",
                      help = "Write the snapshot to XML files instead of a database. "
                             "When this option is enabled, the option '--since' is used "
                             "to define the event time at which the snapshot has to be "
                             "valid and <destination> is interpreted as the directory "
                             "where to create the files. If no '--since' is specified, "
                             "the current time is used.")
    parser.add_option("-v", "--verbose", action = "store_true",
                      help = "Print more messages")
    #parser.add_option("-n","--dry-run", action = "store_true",
    #                  help = "Do not create the output file."
    #                  )
    parser.set_defaults(tags = [],
                        options = [],
                        merge = False,
                        verbose = False,
                        to_files = False)

    # parse command line
    options, args = parser.parse_args(args = argv[1:])

    # check arguments
    if len(args) != 2:
        parser.error("not enough arguments. Try with --help.")

    # Prepare logger
    import logging
    log = logging.getLogger(parser.prog or os.path.basename(sys.argv[0]))
    if not options.verbose:
        log.setLevel(logging.INFO)
    else:
        log.setLevel(logging.DEBUG)
    _fixLogger() # use the global handler

    if not options.options:
        options.options.append(default_options_file)
    source = os.path.expandvars(guessConnectionString(args[0], options.options))
    dest = os.path.expandvars(args[1])

    # Import the heavy modules
    from PyCool import cool
    from CondDBUI.Admin import timeToValKey
    from time import time, ctime, asctime, gmtime

    try:
        if options.to_files:
            # special checks on options for --to-files
            if options.until:
                parser.error("conflicting options '--until' and '--to-files'. Try with --help.")

            if not options.tags:
                tag = "HEAD"
            else:
                # If many tags are give, use only the last one
                tag = options.tags[-1]

            since = timeToValKey(options.since, int(time() * 1e9))

            log.info("Dumping %s to %s" % (source, dest))
            log.info("Event Time (loc): %s" % ctime(since/1e9))
            log.info("Event Time (UTC): %s" % asctime(gmtime(since/1e9)))
            log.info("Tag: %s" % tag)

            log.info("Copying, please wait...")

            from CondDBUI.Admin import DumpToFiles
            DumpToFiles(connString = source, time = since, tag = tag,
                        destroot = dest, force = False)

        else:
            since = timeToValKey(options.since, cool.ValidityKeyMin)
            until = timeToValKey(options.until, cool.ValidityKeyMax)

            log.info("Cloning %s to %s" % (source, dest))
            log.info("IoV(loc): %s to %s" % (ctime(since/1e9), ctime(until/1e9)))
            log.info("IoV(UTC): %s to %s" % (asctime(gmtime(since/1e9)),
                                             asctime(gmtime(until/1e9))))
            log.info("Tags: %s" % options.tags)

            log.info("Copying, please wait...")

            from PyCoolCopy import Selection, PyCoolCopy, log as pcc_log
            # Avoid multiple logging handlers
            pcc_log.handlers = []
            if not options.verbose:
                pcc_log.setLevel(logging.WARNING)
            else:
                pcc_log.setLevel(logging.INFO)

            from CondDBUI import CondDB
            # FIXME: not needed because LFCReplicaService was dropped
            # Connect to source database (PyCoolCopy do not support CORAL LFCReplicaService)
            sourceDb = CondDB(source).db
            selection = Selection( since = since, until = until, tags = options.tags )
            if options.merge:
                log.info("Warning: merge works only on folders that do not exist on the target")
                PyCoolCopy(sourceDb).append(dest, selection)
            else:
                # FIXME : https://sft.its.cern.ch/jira/browse/ROOT-5603
                targetDb = CondDB(dest,create_new_db=True,readOnly=False).db
                PyCoolCopy(sourceDb).copy(targetDb, selection)
            log.info("Copy completed.")
    except Exception, details:
        log.error('Copy failed with error: %s' % str(details))

    return 0

if __name__ == '__main__':
    from sys import exit, argv
    exit(main(argv))
