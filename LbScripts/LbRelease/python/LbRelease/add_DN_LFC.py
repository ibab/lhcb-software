#!/usr/bin/env python
#Python script which allows adding new trusted DN and its corresponding directory
#input argument : userDN and nickname
#It should be run as root on a trusted machine, like volhcb04
#Author : Lana Abadie
#version : 1.0
#date : 01/08/2008

import sys, socket, os
import lfc, getopt
def checkEnv():
    hostname = socket.gethostname()
    if hostname.find("volhcb") < 0 :
        print "Please log as root on one of the volhcbXX machines at CERN"
        return - 1
    user = os.getuid()
    if user != 0 :
        print "You should be root to run this command " + str( user )
        return 1
    print "checkEnv is OK"
    return 0


def checkDN ( userDN, nickname ):
    statement = "voms-admin --vo lhcb --host voms.cern.ch list-users |grep '" + userDN + "'"
    res = os.popen( statement )
    s = res.readline()
    dn_info = s.split( ',' )
    if dn_info[0].find( userDN ) == -1:
        print "Invalid DN or DN is not a member of LHCb"
        res.close()
        return - 1
    res.close()
    userCA = dn_info[1].strip()
    statement = "voms-admin --host voms.cern.ch --vo lhcb --nousercert list-user-attributes '" + userDN + "'" + " '" + userCA + "'"
    print statement
    res = os.popen( statement )
    s = res.readline()
    if s.find( "nickname=" ) == -1 :
        print "Couldn't verify the nickname " + s
        return - 1
    nickname_temp = s.split( '=' )
    nickname_info = nickname_temp[1].split()
    #print "nickname returned by voms is s "+s
    if nickname.find( nickname_info[0] ) == -1 or len( nickname ) != len( nickname_info[0] ):
        print "The user DN " + userDN + " has the following nickname " + nickname_info[0] + " which is different from the one you provide " + nickname
        return - 1
    else :
        print "User DN and nickname exist and match"
        return 0

def checkDir ( nickname ):

    first_letter = nickname[0]
    dirname = '/grid/lhcb/user/'
    dirname += first_letter
    dirname += '/'
    dirname += nickname
    stat = lfc.lfc_filestatg()
    res = lfc.lfc_statg( dirname, "", stat )
    if res == 0:
        print "Directory exists "
        return 0
    else:
        err_num = lfc.cvar.serrno #@UndefinedVariable
        err_string = lfc.sstrerror( err_num )
        print  "There was an error while looking for " + dirname + ": Error " + str( err_num ) + " (" + err_string + ")"
        if err_num == 2:
            res = createDir( dirname, nickname )
            return res
        else:
            return - 3

def createDir ( dirname, _nickname ):

    res = lfc.lfc_mkdir( dirname, 0777 )
    if res == 0 :
        print "Directory " + dirname + " has been created and " + str( res )
        #we delete the directory
        #res = lfc.lfc_rmdir(dirname)
        return - 1
    else :
        err_num = lfc.cvar.serrno #@UndefinedVariable
        err_string = lfc.sstrerror( err_num )
        print  "There was an error while creating " + dirname + " Error " + str( err_num ) + " (" + err_string + ")"
        return - 2

def createUser ( userDN, userid ):
    res = lfc.lfc_enterusrmap( userid, userDN )
    if res == 0:
        print "User created " + userDN
        return 0
    else :
        err_num = lfc.cvar.serrno #@UndefinedVariable
        err_string = lfc.sstrerror( err_num )
        # error not due to userDN exists
        if err_num != 17 :
            print  "There was an error while adding " + userDN + " Error " + str( err_num ) + " (" + err_string + ")"
            return - 1
        #however we return 0 because we want to update the dir as it has been created by root
        else :
            print "UserDN already exists"
            return 0

def delUser ( userDN, _userid ):
    res = lfc.lfc_rmusrmap( 0, userDN )
    if res == 0:
        print "User has been removed from userinfo"
        return 0
    else:
        err_num = lfc.cvar.serrno #@UndefinedVariable
        err_string = lfc.sstrerror( err_num )
        print "There was an error while deleting user: Error " + str( err_num ) + " (" + err_string + ")"
        return - 1

def rmDir( nickname ):
    dirname = "/grid/lhcb/user/" + nickname[0] + "/" + nickname
    res = lfc.lfc_rmdir( dirname )
    if res == 0:
        print "user dir has been removed "
        return 0
    else:
        err_num = lfc.cvar.serrno #@UndefinedVariable
        err_string = lfc.sstrerror( err_num )
        print "There was an error while deleting dir: Error " + str( err_num ) + " (" + err_string + ")"
        return - 1

def getUserID ( userDN ):
    result, u_list = lfc.lfc_getusrmap()
    if ( result == 0 ):
        for i in u_list:
            if i.username == userDN:
                print "userDN exists mapped to " + str( i.userid )
                return i.userid
        print "Did not find the corresponding userid for " + userDN
        return - 1
    else :
        err_num = lfc.cvar.serrno #@UndefinedVariable
        err_string = lfc.sstrerror( err_num )
        print  "There was an error while looking at userid for " + userDN + " Error " + str( err_num ) + " (" + err_string + ")"
        return - 1

def getUserName ( userID ):
    result, u_list = lfc.lfc_getusrmap()
    if ( result == 0 ):
        for i in u_list:
            if i.userid == userID:
                print "userID exists mapped to " + str( i.username )
                return i.username
            print "Did not find the corresponding username for " + str( userID )
        return "ERROR"
    else :
        err_num = lfc.cvar.serrno #@UndefinedVariable
        err_string = lfc.sstrerror( err_num )
        print  "There was an error while looking at username for " + str( userID ) + " Error " + str( err_num ) + " (" + err_string + ")"
        return "ERROR"

def updateDirPerm ( userid, nickname, userDN ) :
    dirname = "/grid/lhcb/user/" + nickname[0] + "/" + nickname
    print "updating owner of user dir to " + str( userid )
    res = lfc.lfc_chown ( dirname, userid, -1 )
    if res == 0:
        print "Owner directory updated"
        return 0
    else :
        err_num = lfc.cvar.serrno #@UndefinedVariable
        err_string = lfc.sstrerror( err_num )
        print  "There was an error while updating ownerid for " + dirname + " Error " + str( err_num ) + " (" + err_string + ")"
        return - 1

def getOwnerACL ( nickname ):
    dirname = "/grid/lhcb/user/" + nickname[0] + "/" + nickname
    _nentries, acls_list = lfc.lfc_getacl( dirname, lfc.CA_MAXACLENTRIES )
    if len( acls_list ) > 0:
        for i in acls_list:
            if i.a_type == 1:
                return i.a_id
        print "No ACL_USER defined"
        return - 1
    else :
        print "No ACL defined for " + dirname
        return - 1
def usage ():
    print "Usage : python add_DN_to_LFC.py --userDN=<user_dn> --nickname=<nickname> -f [force to add the DN if the user dir exists]  -h [help]"
    print "Usage : the user_dn should be put between double quotes"

def main () :
    if len( sys.argv ) != 3 and len( sys.argv ) != 4:
        usage ()
        print "len list = " + str( len( sys.argv ) )
        sys.exit( -1 )
    if checkEnv() != 0 :
        print "Exiting the script"
        sys.exit( -1 )

    #os.popen ("source /afs/cern.ch/project/gd/LCG-share/current_3.1/etc/profile.d/grid-env.sh")
    os.environ["X509_USER_CERT"] = "/etc/grid-security/hostcert.pem"
    os.environ["X509_USER_KEY"] = "/etc/grid-security/hostkey.pem"
    os.environ["LFC_HOST"] = "lfc-lhcb.cern.ch"
    try:
        opts, _ = getopt.getopt( sys.argv[1:], "hfun:v", ["help", "force", "userDN=", "nickname="] )
    except getopt.GetoptError, err:
    # print help information and exit:
        print str( err ) # will print something like "option -a not recognized"
        usage()
        sys.exit( 2 )
    userDN = None
    userid = -1
    nickname = None
    force = False
    for o, a in opts:
        if o in ( "-f", "--force" ):
            force = True
        elif o in ( "-h", "--help" ):
            usage()
            sys.exit()
        elif o in ( "-u", "--userDN" ):
            userDN = a
        elif o in ( "-n", "--nickname" ):
            nickname = a
        else:
            assert False, "unhandled option"
    #check the existence of the user dir and create it if doesn't exist

    print "userDN=" + userDN
    print "nickname=" + str( nickname )
    res = checkDN( userDN, nickname )
    if res == -1:
        print "Exiting the script"
        sys.exit( -1 )
    res = checkDir ( nickname )
    #res = checkDir (nickname)
    #mean failure we exit
    if res < -1:
        print "Exiting the script"
        sys.exit( -1 )
    #mean we have to create the user as we create the dir on the fly
    if res == -1:
        if createUser( userDN, -1 ) != 0 :
            #we delete the directory as we create it
            res = rmDir ( nickname )
            print "Exiting the script"
            sys.exit( -1 )
        userid = getUserID( userDN )
        if userid == -1:
            #we delete the directory as we create it
            res = rmDir ( nickname )
            # we delete the user entry in the userinfo
            res = delUser( userDN, -1 )
            print "Exiting the script"
            sys.exit( -1 )
        if updateDirPerm( userid, nickname, userDN ) != 0:
            #we delete the directory as we create it
            res = rmDir ( nickname )
                        # we delete the user entry in the userinfo
            #print "user dn = "+ userDN
            res = delUser( userDN, -1 )
            print "Exiting the script"
            sys.exit( -1 )
    #mean the dir exists before executing the script
    if res == 0:
        userid = getUserID ( userDN )
        if userid != -1:
            if updateDirPerm( userid, nickname, userDN ) != 0:
                print "Exiting the script"
                sys.exit( -1 )
        #case where the dir exists but the new DN not
        else :
            userid = getOwnerACL ( nickname )
            #means that there is a pb with the ACL, we exit
            if userid < 0:
                print "Exiting the script"
                sys.exit( -1 )
            ownerDN = getUserName ( userid )
            #case where the ownerDN does not exist in userinfo table
            print "User " + userDN + " is not in the list of known users. "
            if ownerDN == "ERROR":
                print " His corresponding directory has no ownerDN, but the userid in the acl is " + str( userid )
            else:
                print "His corresponding directory belongs to : " + ownerDN + " and is mapped to " + str( userid )
            if force == False:
                print "Manual intervention required : old user is suspectly different than the user"
                print "Exiting the script: userDN not added"
                sys.exit( -1 )
            else:
                if createUser( userDN, userid ) != 0:
                    print "Exiting the script"
                    sys.exit( -1 )
                else :
                    print "UserDN " + userDN + " is now mapped to userid " + str( userid )
                    print "Exiting the script"
                    sys.exit( 0 )

    print "Exiting the script"
    sys.exit( 0 )

if __name__ == '__main__':
    main()

