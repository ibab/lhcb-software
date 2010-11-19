#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__version__ = "$Id: CondDBAdmin_HashTag.py,v 1.0 2010-10-20 00:00:00 ishapova Exp $"

import os

def main():
    ###########################################################################
    # Configure the parser
    ###########################################################################
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] partition globalTag",
                          version = __version__,
                          description =
"""This script hashes a tag of a partition and updates release notes.""")

    parser.add_option("--rel-notes", type = "string",
                      help = "XML file containing the release notes"
                      )
    parser.add_option("--hash-alg", type = "string",
                      help = "Name of a hashing algorithm to use. DEAFAULT: 'sha1'."
                      )
    parser.add_option("-n","--dry-run", action = "store_true",
                      help = "Skip adding hash sums to release notes."
                      )

    parser.set_default("rel_notes", os.path.join(os.environ["SQLDDDBROOT"],
                                                 "doc", "release_notes.xml"))
    parser.set_default("hash_alg", 'sha1')

    # parse command line
    options, args = parser.parse_args()

    ###########################################################################
    # Prepare local logger
    ###########################################################################
    import logging
    log = logging.getLogger(parser.prog or os.path.basename(sys.argv[0]))
    log.setLevel(logging.INFO)

    # set the global stream handler
    from CondDBUI import LOG_FORMAT
    hndlr = logging.StreamHandler()
    hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().handlers = [hndlr]

    ###########################################################################
    # Positional arguments verification
    ###########################################################################
    if len(args) < 2:
        parser.error("Not enough arguments. Try with --help.")

    partition, globalTag = args

    partitions = ["DDDB", "LHCBCOND", "SIMCOND"]
    if partition not in partitions:
        parser.error("'%s' is not a valid partition name. Allowed: %s" % \
                     (partition, partitions))
    import hashlib
    hashAlg = options.hash_alg
    if sys.version_info >= (2,7):
        if hashAlg not in hashlib.algorithms:
            raise Exception, "'%s' algorithm is not implemented in the hashlib." %hashAlg
    else:
        if hashAlg not in ['md5','sha1','sha224','sha256', 'sha384', 'sha512']:
            raise Exception, "'%s' algorithm is not implemented in the hashlib." %hashAlg

    ###########################################################################
    # Connecting to the DB and release notes
    ###########################################################################
    import CondDBUI.Admin.Management
    masterURL = CondDBUI.Admin.Management._masterCoolURL(partition)
    db = CondDBUI.CondDB(masterURL)

    from CondDBUI.Admin import ReleaseNotes
    rel_notes = ReleaseNotes(options.rel_notes)

    log.info("Master DB file: %s" % masterURL)
    log.info("Release Notes file: %s" %options.rel_notes)

    ###########################################################################
    # Hashing
    ###########################################################################
    # Initialize hashing object
    initialHashSumObj = getattr(hashlib,hashAlg)()
    # Get global tags to hash
    if globalTag.lower() == 'all':
        GlobalTagsToHash = rel_notes.getGlobalTags(partition)
    else:
        GlobalTagsToHash = [globalTag]

    ans = None
    while ans is None:
        ans = raw_input("\nDo you really want to start hashing (Yes,[No])? ")
        if not ans: ans = "No"
        if ans not in [ "Yes", "No" ]:
            print "You have to type exactly 'Yes' or 'No'"
            ans = None

    if ans == "No":
        print "Canceled by user. Hashing wasn't started."
        return 0

    try:
        for tag in GlobalTagsToHash:
            log.info("Hashing '%s' tag... " %tag)
            hashSumObj = db.payloadToHash(initialHashSumObj,tag = tag)
            # Add hash sum to release notes
            if not options.dry_run:
                rel_notes.addHashToGlobalTag(hashSumObj, partition, tag)
            log.info("Done!")
    except KeyboardInterrupt:
        print "Canceled by user. Release page wasn't updated."
        return 0

    if not options.dry_run:
        # Write to release_notes.xml file new global tag entry
        rel_notes.write()
        log.info("Updated release notes at %s"%options.rel_notes)
    else:
        log.info("Release notes weren't updated since in dry-run mode.")

if __name__ == '__main__':
    import sys
    sys.exit(main())