#!/usr/bin/env python
# $Id: dump_db_to_files.py,v 1.1 2007-12-07 08:34:10 marcocle Exp $

# @todo: Add support for channel IDs

import logging
logging.basicConfig()

def _make_relative(cwd,dst):
    """
    Generate a relative path that appended to cwd will point to dst.
    E.g.: cwd = '/my/source/path', dst = '/my/dest/path'
          ==> '../../dest/path'
    """
    cwdlist = cwd.split('/')
    dstlist = dst.split('/')
    # count common directories
    i = 0
    while ( i < len(cwdlist) ) and ( i < len(dstlist) ) and (cwdlist[i] == dstlist[i]):
        i += 1
    reslist = ['..']*(len(cwdlist)-i) # parent levels before a common point
    reslist += dstlist[i:] # remainig dst levels from the common point
    return '/'.join(reslist)

def _fixate_string(src,re,callback):
    # find first occurrence of regular expression
    m = re.search(src)
    while m != None:
        pos = m.start()
        newsubs = callback(m) # convert the string
        if newsubs != m.group(0):
            src = src[0:m.start()] + newsubs + src[m.end():]
        # find next occurrence of regular expression
        m = re.search(src,pos+len(newsubs))
    return src
                
class _relativize_url:
    def __init__(self,node,key,base):
        self.node = node
        self.key = key
        self.base = base
        self.log = logging.getLogger("_relativize_url")
    def __call__(self,match):
        src = match.group(0)
        s = match.start(1)-match.start(0)+1
        e = match.end(1)-match.start(0)-1
        subs = src[s:e]
        self.log.debug("matched '%s' -> '%s'",src,subs)
        if subs.startswith("conddb:"):
            newsubs = _make_relative(self.base,subs[7:])
            self.log.debug("replacing '%s' -> '%s'",subs,newsubs)
            # replace the conddb url with a relative path
            return src[:s] + newsubs + src[e:]
        else:
            self.log.warning("ignoring non conddb URL in '%s[%s]': '%s'",
                             self.node, self.key, subs)
        return src

def copy_to_files(connString,time=0,tag="HEAD",srcs=['/'],
                  destroot='DDDB',force=False,addext=False):
    log = logging.getLogger("copy_to_files")
    log.debug("called with arguments: %s",repr((connString,time,tag,srcs,
                                                destroot,force,addext)))
    
    from CondDBUI import CondDB
    import os, re
    
    # Connect to the database
    db = CondDB(connString, defaultTag = tag)
    log.info("connected to database")
    
    # @note: This piece of code is needed if we want to allow only global tags
    # # Check the validity of the tag
    # if not db.isTagReady(tag):
    #     raise RuntimeError("Tag '%s' is not a valid global tag."%tag)
    
    # Collect the list of folders we are going to use.
    nodes = []
    for s in srcs:
        if db.db.existsFolder(s):
            nodes.append(s)
        elif db.db.existsFolderSet(s):
            nodes += db.getAllChildNodes(s)
        else:
            log.warning("Node '%s' does not exist. Ignored",s)
    nodes.sort()
    nodes = set(nodes)

    # matching: SYSTEM "blah"
    sysIdRE = re.compile('SYSTEM[^>"\']*("[^">]*"|'+"'[^'>]*')")
    # matching: href "conddb:blah"
    hrefRE = re.compile('href *= *("conddb:[^">]*"|'+"'conddb:[^'>]*')")
    
    for node in ( n for n in nodes if db.db.existsFolder(n) ):
        try:
            log.info("retrieve data from '%s'",node)
            data = db.getPayload(node, time, channelID = 0, tag = tag)
            
            nodesplit = node.split('/')
            nodebase = '/'.join(nodesplit[:-1])
            nodename = nodesplit[-1]
            
            if '/' != os.sep:
                tmppath = nodebase.replace('/',os.sep)
            else:
                tmppath = nodebase
            if tmppath and (tmppath[0] == os.sep):
                tmppath = tmppath[1:]
            dirname = os.path.join(destroot,tmppath)
            
            if not os.path.isdir(dirname):
                log.info("create directory '%s'",dirname)
                os.makedirs(dirname)
            
            log.debug("looping over data keys")
            for key,xml in data.items():
                
                log.debug("key: '%s'",key)
                filename = nodename
                if key != 'data':
                    filename = '%s@%s'%(key,nodename)
                tmppath = os.path.join(dirname,filename)
                
                if not force and os.path.exists(tmppath):
                    log.warning("file '%s' already exists: skipping",tmppath)
                    continue
                
                log.debug("fixating XML")
                
                # fix system IDs
                xml = _fixate_string(xml, sysIdRE, _relativize_url(node,key,nodebase))
                # fix hrefs pointing to absolute conddb urls
                xml = _fixate_string(xml, hrefRE, _relativize_url(node,key,nodebase))
                
                log.info("write '%s'",tmppath)
                open(tmppath,'w').write(xml)
                
        except RuntimeError, x:
            if "Object not found" in str(x):
                log.info("no data")
            elif "No child tag" in str(x):
                log.info("no tag")
            else:
                raise
            
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
    
    if len(args) != 0 or not options.source or not options.dest:
        parser.error("Bad arguments, use -h for help.")

    if options.verbosity == 1:
        logging.getLogger().setLevel(logging.INFO)
    elif options.verbosity > 1:
        logging.getLogger().setLevel(logging.DEBUG)
        
    srcs = [ options.source ]
    if options.filelist:
        srcs = [ f.strip() for f in open(options.filelist).readlines() ]
    
    t = int(options.time)        
        
    copy_to_files(options.connectString,t,options.tag,srcs,
                  options.dest,options.force)

if __name__ == "__main__":
    main()
