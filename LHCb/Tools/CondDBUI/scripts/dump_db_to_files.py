#!/usr/bin/env python
# $Id: dump_db_to_files.py,v 1.6 2009-06-30 14:55:08 marcocle Exp $

# @todo: Add support for channel IDs

import logging
logging.basicConfig()

from CondDBUI.Admin import DumpToFiles

def main():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-c", "--connect-string",
                      dest="connectString", type="string",
                      help="cool::DatabaseId to use for the connection [default: %default]",
                      default="sqlite_file:LocalDDDB.db/DDDB")
    parser.add_option("-T", "--tag",
                      dest="tag", type="string",
                      help="tag to extract from the DB [default: %default]",
                      default="HEAD")
    parser.add_option("-t", "--event-time",
                      dest="time", type="string",
                      help="event time to be used to select the IOV [default: %default]",
                      default="0")
    parser.add_option("-s", "--source",
                      dest="source", type="string",
                      help="directory, in the database, where the files to copy to the DB are [default: %default]",
                      default="/",
                      metavar="SRCDIR")
    parser.add_option("--options",
                      dest="options", type="string", action="append",
                      help="Option file from which to get the needed informations",
                      default=[],
                      metavar="JOBOPTS")
    parser.add_option("-d", "--dest",
                      dest="dest", type="string",
                      help="directory where to copy the files [default: %default]",
                      default="DDDB",
                      metavar="DESTPATH")
    parser.add_option("-f", "--force",
                      dest="force", action="store_true",
                      help="overwrite files during copy",
                      default=False)
    parser.add_option("-v", "--verbose",
                      dest="verbosity", action="count",
                      help="increase verbosity")
    #parser.add_option("-X", "--ext",
    #                  action="store_true", dest="extension",
    #                  help="add filename extension when copying a COOL Folder",
    #                  default=False)
    parser.add_option("", "--from-file",
                      dest="filelist", type="string",
                      metavar="FILELIST",
                      help="copy only the files specified in the file FILELIST (override -s)")
    
    
    (options, args) = parser.parse_args()
 
    if len(args) != 0 or not (options.source or options.options) or not options.dest:
        parser.error("Bad arguments, use -h for help.")

    if options.verbosity == 1:
        logging.getLogger().setLevel(logging.INFO)
    elif options.verbosity > 1:
        logging.getLogger().setLevel(logging.DEBUG)

    if options.options:
        from Gaudi.Configuration import importOptions, configurationDict
        for o in options.options:
            importOptions(o)
        conf = configurationDict()
        if "DDDB" not in conf or "LHCBCOND" not in conf:
            raise RuntimeError("Bad configuration file")
        t = int(options.time)
        log = logging.getLogger("copy_to_files")
        db = conf["DDDB"]
        l = log.getEffectiveLevel()
        log.setLevel(logging.INFO)
        log.info("Copying DDDB from %s with tag %s"%(db["ConnectionString"],db.get("DefaultTAG","HEAD")))
        log.setLevel(l)
        DumpToFiles(db["ConnectionString"],t,db.get("DefaultTAG","HEAD"),
                    ["/"],options.dest,options.force)
        db = conf["LHCBCOND"]
        l = log.getEffectiveLevel()
        log.setLevel(logging.INFO)
        log.info("Copying LHCBCOND from %s with tag %s"%(db["ConnectionString"],db.get("DefaultTAG","HEAD")))
        log.setLevel(l)
        DumpToFiles(db["ConnectionString"],t,db.get("DefaultTAG","HEAD"),
                    ["/"],options.dest,options.force)
    else:
        srcs = [ options.source ]
        if options.filelist:
            srcs = [ f.strip() for f in open(options.filelist).readlines() ]
        
        t = int(options.time)
            
        DumpToFiles(options.connectString,t,options.tag,srcs,
                    options.dest,options.force)

if __name__ == "__main__":
    main()
