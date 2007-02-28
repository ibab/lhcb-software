#!/usr/bin/env python

import os, sys
from time import time
from tempfile import mkdtemp
from shutil import rmtree

# needed to correctly link DDDB to LHCBCOND
os.environ["CONDITIONS_PATH"] = "conddb:/Conditions/MainCatalog.xml"

orig_dir = os.getcwd()

if len(sys.argv) >= 2:
    dest_dir = os.path.normpath(os.path.join(os.getcwd(),sys.argv[1]))
    if not os.path.isdir(dest_dir):
        print "Error: directory '%s' does not exist"%dest_dir
        sys.exit(1)
else:
    dest_dir = orig_dir

#tmp_dir = os.path.join("..","tmp")
tmp_dir = mkdtemp()
data_dir = os.path.join(tmp_dir,"Det","SQLDDDB_BackUp")

copy_script = os.path.join(os.environ['CONDDBUIROOT'],"python","copy_files_to_db.py")

cvsroot = ":kserver:isscvs.cern.ch:/local/reps/lhcb"

os.chdir(tmp_dir)
    
try:
    # get from CVS the file containing the list of tags
    retval = os.system("cvs -d %s co Det/SQLDDDB_BackUp/doc/tags "%cvsroot)
    if retval != 0:
        print "ERROR: problems getting the list of tags"
        sys.exit(retval)
    
    # parse the list of tags
    list_of_tags = []
    for l in open(os.path.join(data_dir,"doc","tags")).readlines():
        tag,desc = l.split(':')
        list_of_tags.append((tag.strip(),desc.strip()))
    
    # directories to convert to databases
    databases = [ 'DDDB', 'LHCBCOND' ]
    
    def connection_string(dbname):
        """
        Return the COOL connection string from the given database name.
        """
        return "sqlite://;schema=%s/%s.db;dbname=%s"%(dest_dir,dbname,dbname)
    
    
    import conddbui
    
    # loop over the databases to generate
    for dbname in databases:
        for tag,desc in list_of_tags:
            print "Getting %s for tag %s (%s) ... "%(dbname,tag,desc),
            sys.stdout.flush()
            out = []
            feedback = "-\|/"
            feedback = " .oOo."
            sys.stdout.write(" ")
            for l in os.popen4("cvs -d %s co -r %s Det/SQLDDDB_BackUp/%s"%(cvsroot,tag,dbname))[1].xreadlines():
                out.append(l)
                sys.stdout.write("\b"+feedback[int(time()*2)%len(feedback)])
                sys.stdout.flush()
            sys.stdout.write("\b")
            #out = os.popen4("cvs -d %s co -r %s Det/SQLDDDB_BackUp/%s"%(cvsroot,tag,dbname))[1].readlines()
            
            updated_files = [ l.split()[1].replace("Det/SQLDDDB_BackUp/%s"%dbname,"")
                              for l in out if l.startswith("U") ]
            print "found %d files"%len(updated_files)

            open(os.path.join(tmp_dir,dbname+"-includes"),"w").write('\n'.join(updated_files)+'\n')
            # this is a dictionary where a directory points to a dictionary and a file points to None
            tree = {}
            local_dir = tree
            for f in updated_files:
                local_dir = tree
                for d in f.split("/")[:-1]:
                    if d not in local_dir :
                        local_dir[d] = {}
                    local_dir = local_dir[d]
                local_dir[f.split("/")[-1]] = None

            
            os.system("python %s -s %s -c \"%s\" --include-from-file=%s"%(copy_script,
                                                                          os.path.join(data_dir,dbname),
                                                                          connection_string(dbname),
                                                                          os.path.join(tmp_dir,dbname+"-includes")))
            #os.path.rename("%s.db"%dbname,os.path.join("..","db"))
            print "Creating tag %s"%tag
            conddbui.CondDB(connection_string(dbname),readOnly=False).recursiveTag("/",tag,desc)

finally:
    # make sure we delete the temporary directory
    os.chdir(orig_dir)
    rmtree(tmp_dir)
