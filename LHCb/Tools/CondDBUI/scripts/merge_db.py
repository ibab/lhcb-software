#!/usr/bin/env python

def main(argv):
    if len(argv) != 3 or '-h' in argv or '--help' in argv:
        from os.path import basename 
        print "Usage: %s patch_db dest_db\n" % basename(argv[0])
        print "where:"
        print "\t%s\t%s"%("patch_db ", "the connection string for the source DB")
        print "\t%s\t%s"%("dest_db  ", "the connection string for the DB to modify")
        return 1
     
    # Main body
    from CondDBUI import merge, LOG_FORMAT
    
    import logging
    hndlr = logging.StreamHandler()
    hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().addHandler(hndlr)
    
    merge(source,target)
    
    return 0

if __name__ == '__main__':
    from sys import exit, argv
    exit(main(argv))
