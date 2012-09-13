#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__version__ = "$Id: CondDBAdmin_CompareDBs.py,v 1.1 2010-11-16 00:00:00 ishapova Exp $"

import os
import CondDBUI

def format_text(text, color_name, blink = False, underline = False):
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
    return '\x1B[' + '%s%s%s49m' %(color_num, blink, underline) + text + \
           '\x1B['+ '0m'

def main():
    ###########################################################################
    # Configure the parser
    ###########################################################################
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] globalTag",
                          version = __version__,
                          description =
"""This script performs hashing sum based comparison of two databases tagged
with a global tag of interest.""")

    parser.add_option("-m", "--master-db", type = "string",
                      help = "Master database connection string. "
                      "DEFAULT: $SQLITEDBPATH/database.db/partition"
                      )
    parser.add_option("-s", "--slave-db", type = "string",
                      help = "Slave database connection string."
                      )
    parser.add_option("--hash-alg", type = "string",
                      help = "Name of an hashing algorithm to use."
                      " DEFAULT: 'sha1'."
                      )

    parser.set_default("master_db", os.environ["SQLITEDBPATH"])
    parser.set_default("slave_db", None)
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
    # Check arguments
    ###########################################################################
    if len(args) < 1:
        parser.error("Not enough arguments. Try with --help.")
    tag = args[0]

    # Set connection strings
    slave_ConnStr = options.slave_db
    if not slave_ConnStr:
        parser.error("The target DB is not set to check in.")
    partitions = ["DDDB", "LHCBCOND", "SIMCOND"]
    partition = os.path.split(slave_ConnStr)[1]
    if partition not in partitions:
        parser.error("'%s' is not a valid partition name. Allowed: %s" % \
                     (partition, partitions))
    master_ConnStr = "sqlite_file:" + os.path.join(options.master_db,"%s.db"%partition,"%s" %partition)

    # Check hashing algorithm
    import hashlib
    hashAlg = options.hash_alg
    if sys.version_info >= (2,7):
        if hashAlg not in hashlib.algorithms:
            raise Exception, "'%s' algorithm is not implemented in the hashlib." %hashAlg
    else:
        if hashAlg not in ['md5','sha1','sha224','sha256', 'sha384', 'sha512']:
            raise Exception, "'%s' algorithm is not implemented in the hashlib." %hashAlg
    # Initialize hashing object
    initialHashSumObj = getattr(hashlib,hashAlg)()

    ###########################################################################
    import timeit
    try:
        log.info("Master DB at: %s" %master_ConnStr)
        log.info("Slave DB at: %s\n" %slave_ConnStr)

        log.info("Connecting to Master DB ...")
        dbMaster=CondDBUI.CondDB(master_ConnStr)
        log.info("Computing the hash sum for '%s' tag ..." %tag)
        hashSumObjMaster = dbMaster.payloadToHash(initialHashSumObj,tag = tag)
        hashSumMaster = hashSumObjMaster.hexdigest()
        log.info("Master DB hash sum is: '%s'\n"%format_text(hashSumMaster,"yellow"))

        log.info("Connecting to Slave DB ...")
        dbSlave=CondDBUI.CondDB(slave_ConnStr)
        log.info("Computing the hash sum for '%s' tag ..." %tag)
        hashSumObjSlave = dbSlave.payloadToHash(initialHashSumObj,tag = tag)
        hashSumSlave = hashSumObjSlave.hexdigest()
        log.info("Slave DB hash sum is: '%s'\n"%format_text(hashSumSlave,"yellow"))

        if hashSumMaster == hashSumSlave:
            log.info("Tags with the name '%s' in both DBs are %s."
                     %(tag,format_text("identical","green")))
        else:
            log.info("Tags with the name '%s' in both DBs are %s."
                     %(tag,format_text("different","green",blink=True)))

    except KeyboardInterrupt:
        print "Comparison canceled by user."
        return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())
