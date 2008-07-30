#!/usr/bin/env python

__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: CondDBAdmin_MakeSnapshot.py,v 1.5 2008-07-30 12:07:29 marcocle Exp $"

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

def timegm(t):
    """Inverse of time.gmtime. Implementation from Gaudi::Time."""
    import time
    if t[8] != 0: # ensure that dst is not set
        t = tuple(list(t[0:8]) + [0])
    t1 = time.mktime(t)
    gt = time.gmtime(t1)
    t2 = time.mktime(gt)
    return t1 + (t1 - t2)

def timeToValKey(tstring, default):
    if not tstring: return default
    import re
    # Format YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]
    exp = re.compile(r"^(?P<year>[0-9]{4})-(?P<month>[0-9]{2})-(?P<day>[0-9]{2})"+
                     r"(?:_(?P<hour>[0-9]{2}):(?P<minute>[0-9]{2})(?::(?P<second>[0-9]{2})(?:\.(?P<decimal>[0-9]*))?)?)?"+
                     r"(?P<utc>UTC)?$")
    m = exp.match(tstring)
    if m:
        # FIXME: check for validity ranges
        def toInt(s):
            if s: return int(s)
            return 0
        tm = tuple([ toInt(n) for n in m.groups()[0:6] ] + [ 0, 0, -1 ])
        import time
        if m.group('utc'):
            # the user specified UTC
            t = timegm(tm)
        else:
            # seconds since epoch UTC, from local time tuple
            t = time.mktime(tm)
        t = int(t) * 1000000000 # to ns
        d = m.group('decimal')
        if d:
            if len(d) < 9:
                # Add the missing 0s to the decimals
                d += '0'*(9-len(d))
            else:
                # truncate decimals
                d = d[:9]
            # add decimals to t
            t += int(d)
        return t
    return default

def main(argv):
    # Configure the parser
    from optparse import OptionParser
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
                      help = "Options files to use to guess the mapping from the" +
                      " partition name to the actual connection string." +
                      " [default = %default]")
    parser.add_option("-s", "--since", type = "string",
                      help = "Start of the interesting Interval Of Validity (local time)." + 
                      " Format: YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]")
    parser.add_option("-u", "--until", type = "string",
                      help = "End of the interesting Interval Of Validity (local time)"+ 
                      " Format: YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]")
    parser.add_option("--merge", action = "store_true",
                      help = "Whether to merge the data into an existing DB")
    parser.add_option("-v", "--verbose", action = "store_true",
                      help = "Print more messages")
    #parser.add_option("-n","--dry-run", action = "store_true",
    #                  help = "Do not create the output file."
    #                  )
    parser.set_default("tags", [])
    parser.set_default("options", [os.path.join(os.environ["SQLDDDBROOT"], "options", "SQLDDDB.py")])
    parser.set_default("merge", False)
    parser.set_default("verbose", False)
        
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
    
    source = os.path.expandvars(guessConnectionString(args[0], options.options))
    dest = os.path.expandvars(args[1])
    
    from PyCool import cool
    since = timeToValKey(options.since, cool.ValidityKeyMin)
    until = timeToValKey(options.until, cool.ValidityKeyMax)
    
    from time import ctime, asctime, gmtime
    log.info("Cloning %s to %s" % (source, dest))
    log.info("IoV(loc): %s to %s" % (ctime(since/1e9), ctime(until/1e9)))
    log.info("IoV(UTC): %s to %s" % (asctime(gmtime(since/1e9)),
                                     asctime(gmtime(until/1e9))))
    log.info("Tags: %s" % options.tags)
    
    # perform the copy
    from PyCoolCopy import Selection, PyCoolCopy, log as pcc_log
    # Avoid multiple handlers 
    pcc_log.handlers = []
    if not options.verbose:
        pcc_log.setLevel(logging.WARNING)
    else:
        pcc_log.setLevel(logging.INFO)
    
    log.info("Copying, please wait...")
    try:
        selection = Selection( since = since, until = until, tags = options.tags )
        if options.merge:
            log.info("Warning: merge works only on folders that do not exist on the target")
            PyCoolCopy(source).append(dest, selection)
        else:
            PyCoolCopy(source).copy(dest, selection)
        log.info("Copy completed.")
    except Exception, details:
        log.error('Copy failed with error: %s' % str(details))
    
    return 0
    
if __name__ == '__main__':
    from sys import exit, argv
    exit(main(argv))
