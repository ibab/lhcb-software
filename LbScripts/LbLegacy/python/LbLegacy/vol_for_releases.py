""" Script create a new afs volume for a new project version
 060616 - first version
 060623 - prepare the script to be called in mkproject.py
 060925 - set python_version and day
 061031 - fix size
 070306 - add ANALYSIS
"""
#-----------------------------------------------------------------------------

from LbConfiguration.Project import getProject
import logging
import sys, os, commands

#-----------------------------------------------------------------------------------
def create_volume(pname,pversion,size=0):
    log = logging.getLogger()

    projconf = getProject(pname)

    if not size :
        size = projconf.FullSize()

# get user name
    MYNAME = commands.getstatusoutput('id -u -n')[1]

# project directory, afs group, volume root
    PNAME = projconf.NAME()
    P_DIR = projconf.ReleaseArea()
    root  = projconf.AFSVolumeRoot()
    group = projconf.AFSLibrarianGroup()

# set the volume name
    PN = projconf.AFSVolumeName()
    vol_name = 'q.'+root+'.'+PN+'_'+pversion

# check PNAME project existence
    if not os.path.exists(os.path.join(P_DIR,PNAME)):
        log.error( 'The project %s does not exist on %s' % (PNAME, P_DIR))
        sys.exit()
    else:
        if os.path.exists(os.path.join(P_DIR,PNAME,PNAME+'_'+pversion)):
            log.warning('The version %s of the project %s already exists'%(pversion, projconf.Name()))
            return
    PPATH = os.path.join(P_DIR,PNAME,PNAME+'_'+pversion)


# create the new volume
    strcmd = 'afs_admin create -u %s -q %d %s %s' % (MNAME, size, PPATH, vol_name)
    log.info(strcmd)
    os.system(strcmd)

# give write access to librarians
    if os.path.exists(PPATH):
        os.system('fs sa '+PPATH+' '+group+':librarians all')

    return
