#!/usr/bin/env python
import sys

_tmp_argv = sys.argv
sys.argv = sys.argv[0:1]
from PyCool import cool, coral
sys.argv = _tmp_argv

import                                  conddbui
import                                  datetime
import                                  os
import                                  random
import                                  re
import                                  sys
from    calendar                import  timegm
from    optparse                import  OptionParser
from    os                      import  chdir
from    time                    import  ctime, gmtime, strftime, strptime
from    xml.dom                 import  NodeFilter
#from    xml.dom.ext             import  Print, PrettyPrint
#from    xml.dom.ext.reader      import  Sax2
#from    datetime                import  timedelta

sysIdRE = re.compile('SYSTEM[^>"\']*("[^">]*"|'+"'[^'>]*')")
def fix_system_ids(xml_data,path):
    data = xml_data
    m = sysIdRE.search(data)
    while m != None:
        pos = m.start()
        s = m.start(1) + 1
        e = m.end(1) - 1
        p = os.path.join(path,data[s:e])
        p = os.path.normpath(p)
        data = data[0:s] + p + data[e:]
        m = sysIdRE.search(data, pos + 1)
    return data

envVarRE = re.compile('\$([A-Za-z][A-Za-z0-9_]*)')
#cvs_vars = [ 'Id', 'Name', 'Log' ]
def fix_env_vars(xml_data):
    data = xml_data
    m = envVarRE.search(data)
    while m != None:
        pos = m.start()
        s = m.start(1)
        e = m.end(1)
        name = m.group(1)

        if os.environ.has_key(name):
            val = os.environ[name]
        else:
            val = name

        data = data[0:pos] + val + data[e:]
        m = envVarRE.search(data, pos + 1)
    return data

#def fillFolderDict(root, files, noextension, folderDict):
#    f = files.pop()
#    if '@' in f:
#        key, folder_name = f.split('@')
#        if noextension:
#            folder_name = os.path.splitext(folder_name)[0]
#    else:
#        key = 'data'
#        if noextension:
#            folder_name = os.path.splitext(f)[0]
#        else:
#            folder_name = f
#    if folderDict.has_key(folder_name):
#        folderDict[folder_name][key]  = os.path.join(root, f)
#    else:
#        folderDict[folder_name] = {key: os.path.join(root, f)}

############################### connectStrings: ################################
# "oracle://intr1-v.cern.ch/INTR;schema=lhcb_cool;user=lhcb_cool;dbname=DDDB"
# "sqlite://none;schema=XmlDDDB_v26r2.sqlite;user=none;password=none;dbname=DDDB"
# "mysql://pclhcb55.cern.ch;schema=MARCOCLE;user=marcocle;dbname=DDDB"
# "oracle://oradev10.cern.ch:10520/D10;schema=lhcb_lcg_cool_dev;user=lhcb_lcg_cool_dev;dbname=DDDB"
################################################################################
def main():
    parser = OptionParser()
    parser.add_option("-c", "--connect-string",
                      dest="connectString", type="string",
                      help="cool::DatabaseId to use for the connection",
                      default="sqlite://none;schema=sqlite.db;user=none;password=none;dbname=DDDB")
    parser.add_option("-s", "--source",
                      dest="source", type="string",
                      help="directory where the files to copy to the DB are",
                      metavar="SRCDIR")
    parser.add_option("-d", "--dest",
                      dest="dest", type="string",
                      help="directory, in the database, where to copy the files (default: '/')",
                      default="/",
                      metavar="DESTPATH")
    parser.add_option("-x", "--drop-db",
                      action="store_true", dest="drop",
                      help="drop the database before the copy",
                      default=False)
    parser.add_option("-X", "--noext",
                      action="store_true", dest="noextension",
                      help="Remove filename extension when creating a COOL Folder",
                      default=False)
#   parser.add_option("-k", "--keep-db",
#                     action="store_false", dest="drop",
#                     help="keep the existing database and merge with the new files (default)")
    parser.add_option("-p", "--periods-list",
                      dest="listWithPeriods", type="string",
                      help="read in the file containing a list of files \
    with their startTime, stopTime, periodicity and dispersion \
    in human-friendly units, starting from seconds, like follows: \
    #                Source                    Destination-in-CondDB     Start-Date-Time          Stop-Date-Time   Periodicity  Dispersion  \
    DDDB/Conditions/Rich1/Environment/Aerogel.xml       /           2006-01-01T00:00:00GMT   2007-01-01T00:00:00GMT   4.5 weeks   2 days    \
    DDDB/Conditions/Rich1/Environment/Gas.xml           /           2006-01-01T00:00:00GMT   2006-02-01T00:00:00GMT   1 day       3.5 hours \
    ...")

    (options, args) = parser.parse_args()
    if len(args) != 0\
    or (not options.source and not options.listWithPeriods)\
    or (options.source and options.listWithPeriods)\
    or not options.dest:
        parser.error("Bad arguments, use -h for help.")

    if options.drop:
        conddbui.CondDB.dropDatabase(options.connectString)
    db = conddbui.CondDB(options.connectString, create_new_db = True, readOnly=False)

    excludes = [ 'clhep2dtd.pl' ]

    listOfConditions = []   # This list will contain "lines" either from the input
                            # list of files with their periodicities or a dummy
                            # "line" with "infinite" range of time,
                            # "infinite" periodicity in seconds and
                            # zero dispersion as well, in case when only "source" is
                            # provided in command-line

    # The following is the case when Conditions are copied into the CondDB
    # with realistic-scenario-based IoVs, which are calculated with a
    # known periodicity (perhaps with some random smearing, TBD) within a
    # given range of time. In this case a list of Path/Conditions.xml
    # FQFileNames with the Destination-in-CondDB, Start-Date-Time,
    # Stop-Date-Time and Periodicity and Dispersion for each condition is
    # supplied. The FQN of such a steering file is supplied through the
    # command-line. Here is an example how a steering file may look like:
    #
    # #                Source                    Destination-in-CondDB     Start-Date-Time          Stop-Date-Time   Periodicity  Dispersion
    # DDDB/Conditions/Rich1/Environment/Aerogel.xml       /           2006-01-01T00:00:00GMT   2007-01-01T00:00:00GMT   4.5 weeks   2 days
    # DDDB/Conditions/Rich1/Environment/Gas.xml           /           2006-01-01T00:00:00GMT   2006-02-01T00:00:00GMT   1 day       3.5 hours
    #
    # The first line is the legend
    if options.listWithPeriods:
        f = open(options.listWithPeriods)
        f.readline()  # this is to skip the first line which is the legend

        while True:
            currentLine = f.readline()
            if (currentLine == '\n' or currentLine == ''):
                break
            else:
                currentString = currentLine.strip()
                listOfConditions.append(currentString)
        f.close()

    # And the following is the case from the older version of the script,
    # when only the source is supplied in the command-line. No steering
    # file used. Consequently, range of time and periodicity are both
    # "infinite" in the context of usage of "time" platform on 32-bit
    # machines... 
    # There are 2147483647 seconds between 1970-01-01T00:00:00GMT
    # and 2038-01-19T03:14:07GMT.
    # So this is done for consistency and hence - universality
    # of the procedure below:
    else:
        optionsSource = options.source+' '+options.dest+' 1970-01-01T00:00:00GMT 2262-04-12T00:47:16GMT 9223372036 seconds 0 seconds'
        listOfConditions.append(optionsSource)

    # Dictionary of human-friendly time units expressed in seconds:
    convTimeUnitsToSeconds = {'second' :        1, 'seconds' :        1,
                              'minute' :       60, 'minutes' :       60,
                              'hour'   :     3600, 'hours'   :     3600,
                              'day'    :    86400, 'days'    :    86400,
                              'week'   :   604800, 'weeks'   :   604800,
                              'month'  :  2419200, 'months'  :  2419200,
                              'year'   : 29030400, 'years'   : 29030400}


    # Next starts the main loop over the listOfConditions
    for conditionString in listOfConditions:
#       print 'conditionString:', conditionString
        values = conditionString.split()
#       print 'values:', values
        (xmlConditionsSource, destination, startDateTime, stopDateTime, periodicity, periodicityUnits, dispersion, dispersionUnits) = values

        # convert periodicity to seconds, using periodicityUnits,
        # but first set it to nothing:
        periodicityInSeconds = 'NOTHING'
        for timeUnit, numberOfSeconds in convTimeUnitsToSeconds.iteritems():
            if periodicityUnits == timeUnit:
                periodicityInSeconds = int( float(periodicity)*float(numberOfSeconds) )
#               print 'periodicityInSeconds =', periodicityInSeconds
                break
        if periodicityInSeconds == 'NOTHING':
            parser.error("Bad arguments, use -h for help.")

        # convert dispersion to seconds, using dispersionUnits,
        # but first set it to nothing:
        dispersionInSeconds = 'NOTHING'
        for timeUnit, numberOfSeconds in convTimeUnitsToSeconds.iteritems():
            if dispersionUnits == timeUnit:
                dispersionInSeconds = int( float(dispersion)*float(numberOfSeconds) )
#               print 'dispersionInSeconds =', dispersionInSeconds
                break
        if dispersionInSeconds == 'NOTHING':
            parser.error("Bad arguments, use -h for help.")

        # count files (just to know): that will be "sum":

        sum = 0

        dotSeparatedElements = xmlConditionsSource.split('.')
        lastDotSeparatedElement = dotSeparatedElements[len(dotSeparatedElements) - 1]
        thisIsJustAFile = False
        if lastDotSeparatedElement == 'xml':    # it is just a file, not a hierarchy
            thisIsJustAFile = True
#           print 'thisIsJustAFile !'

            sum = 1
        else:                                       # in contrast, this is some hierarchy:
                                                    # a directory name or a path
            for root, dirs, files in os.walk(xmlConditionsSource):
                # retain this print !
                #####################
                print 'root =', root
                if 'CVS' in root: # skip CVS directories
                    continue
                for x in excludes:
                    if x in files:
                        files.remove(x)
                sum += len(files)

        num_len = len(str(sum))
        # populate the database
        totalNumberOfFilesInserted = 0
        n = 0

        # The steering file use case: the source given on each line
        # of it is just a single file-name with its path prepended:
        if thisIsJustAFile:
#           print 'xmlConditionsSource =', xmlConditionsSource
            elementsOfPath = xmlConditionsSource.split('/')
            root = elementsOfPath[0]
#           print 'thisIsJustAFile: root =', root
            justAFileName = elementsOfPath[len(elementsOfPath) - 1]
#           print 'justAFileName =', justAFileName
            xmlConditionsSource = xmlConditionsSource.strip('/'+justAFileName)
#           print 'xmlConditionsSource =', xmlConditionsSource

        for root, dirs, files in os.walk(xmlConditionsSource):
            # remove base path
#           print '  root 0:', root
            basePath = root.split('/')[0]
#           print 'basePath:', basePath
            dbroot = root.replace(basePath, destination)
#           print 'dbroot 1:', dbroot
            dbroot = dbroot.replace('//', '/')
#           print 'dbroot 2:', dbroot

            if 'CVS' in root: # skip CVS directories
                continue

            if dbroot != "/": # it means it is not "/"
                if not db.db.existsFolderSet(dbroot):
                    db.createNode(path = dbroot, storageType = 'NODE')
            #else:
            #    dbroot = "/"

            # Steering file use case: the source given on each line
            # of it is just a single file-name with its path prepended:
            if thisIsJustAFile:
                files = [justAFileName]
            for x in excludes:
                if x in files:
                    files.remove(x)

            folderDict = {}
            while files:
################fillFolderDict(root, files, options.noextension, folderDict)
                # The above will also work instead of the rest of the loop body
                f = files.pop()
                if '@' in f:
                    key, folder_name = f.split('@')
                    if options.noextension:
                        folder_name = os.path.splitext(folder_name)[0]
                else:
                    key = 'data'
                    if options.noextension:
                        folder_name = os.path.splitext(f)[0]
                    else:
                        folder_name = f
                if folderDict.has_key(folder_name):
                    folderDict[folder_name][key]  = os.path.join(root, f)
                else:
                    folderDict[folder_name] = {key: os.path.join(root, f)}
#           print 'folderDict =', folderDict
            for folder_name in folderDict.keys():
                n += 1
                # retain this print !!!
                #######################
                print ("%" + str(num_len) + "d %" + str(num_len) + "d  %s") % (n, sum - n, folder_name)
                folder_keys = folderDict[folder_name].keys()
                folder_path = os.path.join(dbroot, folder_name)
                if not db.db.existsFolder(folder_path):
                    db.createNode(path = folder_path, storageKeys = folder_keys)

                payload = {}
                for k in folder_keys:
                    xml_data = open(folderDict[folder_name][k], "rb").read()
                    fixed_data = fix_system_ids(xml_data, "conddb:" + dbroot)
                    #fixed_data = xml_data
                    fixed_data = fix_env_vars(fixed_data)
                    payload[k] = fixed_data

                # Here starts the loop of storing "XML strings" with IoVs changing
                # with the given periodicity within the given range of time. But
                # first - some preliminary preparations (time is in nanoseconds
                # since the epoch):

                # the following is formal, just the starting value for the loop:
                startTimeInNanoSec = 1000000000*timegm(strptime(str(startDateTime),
                "%Y-%m-%dT%H:%M:%S%Z"))
                if  startTimeInNanoSec   <= 0:
                    startTimeInNanoSec    = 0
#               print 'startTimeInNanoSec =', startTimeInNanoSec
                stopTimeInNanoSec  = 1000000000*timegm(strptime(str(stopDateTime),
                "%Y-%m-%dT%H:%M:%S%Z"))
                # For the sake of consistency with COOL, the equivalent of
                # 2262-04-12T00:47:16GMT in nanoseconds: 9223372036000000000 is
                # replaced by the cool.ValidityKeyMax == 9223372036854775807,
                # which is equal to (2^63 - 1).
                if  stopTimeInNanoSec    >= 9223372036000000000:
                    stopTimeInNanoSec     = 9223372036854775807
#               print 'stopTimeInNanoSec  =', stopTimeInNanoSec
                periodicityInNanoSec      = 1000000000*periodicityInSeconds
                dispersionInNanoSec       = 1000000000*dispersionInSeconds
                # The same is done for the periodicity:
                if  periodicityInNanoSec >= 9223372036000000000:
                    periodicityInNanoSec  = 9223372036854775807
#               print 'periodicityInNanoSec                         =', periodicityInNanoSec
#               print 'dispersionInNanoSec                          =', dispersionInNanoSec
                stopDateTimeReached = False

                # Here we go with the actual loop,
                # but first set initial value of valid_until, just for the looping convenience:
                valid_until = startTimeInNanoSec
#               print 'valid_until        =', valid_until

                while True:
                    valid_since = valid_until
#                   print 'valid_since        =', valid_since
                    if valid_since >= stopTimeInNanoSec:
                        break
                    valid_until = valid_since + periodicityInNanoSec + random.randint(-dispersionInNanoSec, dispersionInNanoSec)
                    if  valid_until > stopTimeInNanoSec:
                        valid_until = stopTimeInNanoSec
                        stopDateTimeReached = True
#                   print 'valid_until        =', valid_until, 'IoV =', valid_until - valid_since
                    db.storeXMLString(folder_path, payload, valid_since, valid_until, 0)
                    totalNumberOfFilesInserted += 1

                    if stopDateTimeReached: break

    # retain this print !!!
    #######################
    print "Total number of conditions created/updated = %6d" % totalNumberOfFilesInserted
    # retain this print !!!
    #######################
    print "Total number of files inserted             = %6d" % totalNumberOfFilesInserted

if __name__ == '__main__':
    main()
