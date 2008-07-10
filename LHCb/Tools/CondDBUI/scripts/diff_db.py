#!/usr/bin/env python

def main(argv):
    if len(argv) != 6 or '-h' in argv or '--help' in argv:
        from os.path import basename 
        print "Usage: %s orig_db orig_tag new_db new_tag dest_db\n" % basename(argv[0])
        print "where:"
        print "\t%s\t%s"%("orig_db ", "the connection string for the original DB")
        print "\t%s\t%s"%("orig_tag", "the tag to use in the original DB")
        print "\t%s\t%s"%("new_db  ", "the connection string for the modified DB")
        print "\t%s\t%s"%("new_tag ", "the tag to use in the modified DB")
        print "\t%s\t%s"%("dest_db ", "the connection string for the DB for the differences")
        return 1
    
    (lhs,lhs_tag,rhs,rhs_tag,dest) = argv[1:]
    
    from CondDBUI.PyCoolDiff import diff, CondDBDiffError
    
    from CondDBUI import LOG_FORMAT
    import logging
    hndlr = logging.StreamHandler()
    hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().addHandler(hndlr)
    
    try:
        # diff(originalDB, modifiedDB, diffDB,
        #      nodeName='/',
        #      since=0L, until=9223372036854775807L, channels=<ROOT.cool::ChannelSelection object>,
        #      originalTAG='', modifiedTAG='')
        diff(lhs,rhs,dest,originalTAG=lhs_tag, modifiedTAG=rhs_tag)
    except RuntimeError, x:
        print "Runtime Error:", x
        return 1
    except CondDBDiffError, x:
        print "Diff Error:", x
        return 1
    
    return 0

if __name__ == '__main__':
    from sys import exit, argv
    exit(main(argv))
