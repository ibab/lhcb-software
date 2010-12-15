#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__version__ = "$Id: TSDBAdmin_Update.py,v 1.0 2010-11-29 00:00:00 ishapova Exp $"

import sys, os
from CondDBUI.Admin.TagStatusDB import *
import datetime
import cPickle as pickle

def main():
    ###########################################################################
    # Configure the parser
    ###########################################################################
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options]",
                          version = __version__,
                          description =
"""This script commits new tag to TSDB.""")

    parser.add_option("--db", type = "string",
                      help = "TSDB connection string in the form: 'sqlite:path'."
                      )
    parser.add_option("-i","--input", type = "string",
                      help = "Input file which contains string dictionary of tags to update."
                      )

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
    db_ConnStr = options.db
    if db_ConnStr == None:
        parser.error("Please specify TSDB location. Try with --help.")

    file = options.input
    if not file:
        parser.error("Please specify an input file. Try with --help.")

    ###########################################################################
    # Update TSDB
    ###########################################################################
    # Open TSDB.db and commit new tag there
    from storm.tracer import debug
    debug(True, stream=sys.stdout)

    # Load tag entries to be updated
    f = open(file)
    tags_to_update = pickle.load(f)

    log.info("Connecting to TSDB ...")
    db = TagStatusDB(db_ConnStr)
    log.info("Modifying TSDB ...")
    for partition in tags_to_update.keys():
        for tag in tags_to_update[partition]:
            tag_status = db.getTagStatus(tag["TagName"],unicode(partition),tag["Site"])
            tag_status.time = tag["Time"]
            tag_status.status = tag["Status"]

    db.write()
    log.info("Done!")

if __name__ == '__main__':
    sys.exit(main())