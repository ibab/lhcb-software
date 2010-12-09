#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__version__ = "$Id: TSDBAdmin_Commit.py,v 1.0 2010-11-29 00:00:00 ishapova Exp $"

import sys, os
from CondDBUI.Admin.TagStatusDB import *

import xml.etree.ElementTree as ET
_xel = lambda t: ET.QName("http://lhcb.cern.ch",t)

def reallySure():
    ##########################################################################################
    # Function "Are You really-really sure?" to exit the script if user will change his mind.
    ##########################################################################################
    ans = None
    while ans is None:
        ans = raw_input("\nDo you really want to insert new global tag status entry (Yes,[No])? ")
        if not ans: ans = "No"
        if ans not in [ "Yes", "No" ]:
            print "You have to type exactly 'Yes' or 'No'"
            ans = None

    if ans == "No":
        print "Canceled by user. Global tag status entry wasn't inserted."
        return False

    try:
        import time
        print "Global tag status entry will be inserted in ..."
        timeout = 5
        while timeout >= 0:
            print "%d seconds" % timeout
            time.sleep(1)
            timeout -= 1
    except KeyboardInterrupt:
        print "Canceled by user. Global tag status entry wasn't inserted."
        return False
    # Yes, really sure
    return True

def main():
    ###########################################################################
    # Configure the parser
    ###########################################################################
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] partition tag",
                          version = __version__,
                          description =
"""This script commits new tag to TSDB.""")

    parser.add_option("--db", type = "string",
                      help = "TSDB connection string in the form: 'sqlite:path'."
                      )
    parser.add_option("--rel-notes", type = "string",
                      help = "XML file containing the release notes"
                      )
    parser.add_option("--hash-alg", type = "string",
                  help = "Hash algorithm type to look in 'release_notes.xml' for."
                  " tag (md5, sha1, etc). Default: sha1."
                  )

    parser.set_default("rel_notes", os.path.join(os.environ["SQLDDDBROOT"],
                                                 "doc", "release_notes.xml"))
    parser.set_default("hash_alg", "sha1")

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
    # Check and set options
    ###########################################################################
    if len(args) != 2:
        parser.error("Not enough arguments. Try with --help.")

    db_ConnStr = options.db
    if db_ConnStr == None:
        parser.error("Please specify TSDB location. Try with --help. ")

    HashAlg = options.hash_alg

    partitions = ["DDDB", "LHCBCOND", "SIMCOND"]
    partition, tag = args
    if partition not in partitions:
        parser.error("'%s' is not a valid partition name. Allowed: %s"
                     %(partition, partitions))
    log.info("Global tag: '%s/%s'" %(partition,tag))
    ###########################################################################
    # Add tag
    ###########################################################################
    # Get hash sum value for the tag from the Release Notes
    from CondDBUI.Admin import ReleaseNotes
    rn = ReleaseNotes(options.rel_notes)
    GlobTagElem = rn.getGlobalTagElement(partition,tag)
    hash = None
    partitionElem = [p for p in GlobTagElem.findall(str(_xel('partition')))
                     if p.find(str(_xel('name'))).text == partition][0]
    for hashElem in partitionElem.findall(str(_xel('hash'))):
        if hashElem.get('alg') == HashAlg:
            hash = hashElem.text
    if not hash:
        raise Exception, ("Requested hash algorithm value is not found in the"
    " SQLDDDB Release Notes.")
    log.info("With its '%s' hash sum value: '%s'" %(HashAlg,hash))

    # Open TSDB.db and commit new tag there
    from storm.tracer import debug
    debug(True, stream=sys.stdout)

    db = TagStatusDB(db_ConnStr)

    #Initialize Storm Tag instance
    from datetime import datetime
    storm_tag = Tag(unicode(tag),unicode(partition),datetime.now(),unicode(hash),unicode(HashAlg))
    #Add New row into the in-memory-db representation
    db.addRow(storm_tag)
    for site in db.sites:
        storm_tagstatus = TagStatus(site)
        storm_tagstatus.tag = storm_tag
        db.addRow(storm_tagstatus)

    if reallySure():
        db.write()

if __name__ == '__main__':
    sys.exit(main())