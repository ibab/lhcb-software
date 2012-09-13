#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__credits__ = "Illya Shapoval <illya.shapoval@cern.ch>, Marco Clemencic <marco.clemencic@cern.ch>"
__maintainer__ = "Illya Shapoval"
__version__ = "$Id: CreateDBLookupFiles.py,v 1.0 2010/04/28 00:00:00 ishapova Exp $"

# FIXME: This script should not be needed

import os, sys
import xml.etree.ElementTree as ET
from subprocess import Popen,PIPE
import random
import shlex

def indent(elem, level=0):
    """
    Indent an ElementTree instance to allow pretty print.
    Code taken from http://effbot.org/zone/element-lib.htm
    """
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for child in elem:
            indent(child, level+1)
        if not child.tail or not child.tail.strip():
            child.tail = i
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

def write_file(file,dir_name):
    """Write the files to './conddb_date_time' folder"""
    opened_file = open(dir_name + '/%s'%file,'w')
    opened_file.write('<?xml version="1.0"?>\n')
    return opened_file

def cleanup_remains(dir_name):
    if dir_name:
        import shutil
        try:
            print "Cleaning up ..."
            shutil.rmtree(dir_name)
            if os.path.isfile('dblookup.xml'):
                Popen(shlex.split('tcsh -c "rm dblookup.xml"')).wait()
            if os.path.isfile('authentication.xml'):
                Popen(shlex.split('tcsh -c "rm authentication.xml"')).wait()
            print "Done."
        except OSError, (errno, strerror):
            print "Failed removing %(path)s, %(error)s\nDelete remains by hand."  % {'path' : dir_name, 'error': strerror }

def replace_attributes(element):
    """Change the attributes values of xml elements"""
    for subelement in element:
        if subelement.attrib['name'] == 'user':
            subelement.attrib['name'] = 'aid'
        elif subelement.attrib['name'] == 'password':
            subelement.attrib['name'] = 'attribute'

def prepare_service_collection(site,service_collection,log_con_string):
    """Randomize the lists and chose the first element to be the one which corresponds to the site"""
    s = site.split('.')[1]
    if s == 'RAL':
        s = 'RL'
    random.shuffle(service_collection[log_con_string])
    for service in service_collection[log_con_string]:
        if service.attrib['name'].find(s.lower()) != -1:
            service_collection[log_con_string].remove(service)
            service_collection[log_con_string].insert(0,service)
            break


def main():
    try:
        from optparse import OptionParser
        parser = OptionParser(usage = "%prog [options]",
                            version = __version__,
                            description =
    """Script to regenerate the 'authentication.xml' file and the full set of CondDB 'dblookup-*.xml' files for every '*' site, \
    which is currently available in LFC. Designed initially for the AppConfig package.""")

        # parse command line
        options, args = parser.parse_args()

        #Checking the place to write
        import datetime
        dir_name = datetime.datetime.today().strftime('conddb_%Y-%m-%d_%H:%M:%S')
        if not os.path.exists(dir_name):
            os.makedirs(dir_name)
        else:
            print "'%s' folder exists already. Try again."%dir_name
            return 0

        if os.path.isfile('dblookup.xml'):
            print "Please delete 'dblookup.xml' file then retry."
            return 0
        if os.path.isfile('authentication.xml'):
            print "Please delete 'authentication.xml' file then retry."
            return 0

        # Grabbing current set of available sites in the LFC connections configuration
        com_line_dump_sites = """tcsh -c "setenv PATH `echo $LD_LIBRARY_PATH | tr ':' '\\n' | grep 'CORAL.*lib' | tr -d '\\n'`/../bin:$PATH;\
        coral_replica_manager -l CondDB -ls | grep 'LCG' | awk '{print $5}'" """ # command line for dumping LFC connection settings
        proc_dump_sites = Popen(shlex.split(com_line_dump_sites), env = os.environ, stdout = PIPE)
        proc_dump_sites.wait()

        grabbed_sites = proc_dump_sites.communicate()[0]
        if grabbed_sites:
            grabbed_sites = grabbed_sites.split('\n')
            filtered_sites = []
            for site in set(grabbed_sites):
                if site is not '':
                    filtered_sites.append(site)
            sites = filtered_sites
            print "\nThe following sites were found in the LFC connections configuration:\n%s\n"%sites
            if 'LCG.NIKHEF.nl' in sites:
                print "'LCG.NIKHEF.nl' and 'LCG.SARA.nl' have the same connection settings.\n"
                sites.remove('LCG.NIKHEF.nl')
        else:
            print "\nAn error occurred or no site names were extracted from LFC connections configuration. \
Check environment is set up to use grid.\nNew files weren't generated."
            cleanup_remains(dir_name)
            return 0

        #sites = ["LCG.IN2P3.fr","LCG.PIC.es","LCG.CERN.ch","LCG.CNAF.it","LCG.RAL.uk","LCG.GRIDKA.de","LCG.NIKHEF.nl","LCG.SARA.nl"]

        logical_connect_strings = ['CondDB', 'CondDBOnline']
        merged_site_tree = None
        service_collection = {'CondDB':[],'CondDBOnline':[]}
        # Collecting new connection settings from files which were dumped above for each site
        for site in sites:
            for log_con_string in logical_connect_strings:
                print "Proccessing '%s' for '%s' ... "%(site,log_con_string)

                # Collecting data from the 'authentication.xml' file
                com_line_dump_sett = """tcsh -c "setenv PATH `echo $LD_LIBRARY_PATH | tr ':' '\\n' | grep 'CORAL.*lib'`/../bin:$PATH;\
                coral_replica_manager -exp -l %s -h %s" """%(log_con_string,site)
                proc_dump_sett = Popen(shlex.split(com_line_dump_sett),env = os.environ,stdout = PIPE)
                proc_dump_sett.wait()

                site_tree = ET.parse('authentication.xml')
                site_connection_list = site_tree.getroot()
                for connection in site_connection_list:
                    replace_attributes(connection)
                    roles = connection.findall('role')
                    for role in roles:
                        if role.attrib['name'] == 'owner':
                            connection.remove(role)
                        else:
                            replace_attributes(role)
                    if merged_site_tree:
                        sites_connection_list = merged_site_tree.getroot()
                        sites_connection_list.append(connection)
                    else:
                        merged_site_tree = site_tree

                # Collecting data from the 'dblookup.xml' file
                service_tree = ET.parse('dblookup.xml')
                servicelist = service_tree.getroot()
                for logicalservice in servicelist:
                    for service in logicalservice:
                        service_collection[log_con_string].append(service)
                print 'Done.'

        # Generating new authentication.xml file
        auth_file = write_file('authentication.xml',dir_name)
        indent(merged_site_tree.getroot(),level=0)
        merged_site_tree.write(auth_file)

        # Generating new dblookup-*.xml files for each site
        for site in sites:
            # Build and write the tree
            root = ET.Element('servicelist')
            merged_service_tree = ET.ElementTree(root)
            for log_con_string in logical_connect_strings:
                prepare_service_collection(site,service_collection,log_con_string) # Randomize and choose the first service
                merged_logicalservice = ET.SubElement(root,'logicalservice',{'name':log_con_string})
                for service in service_collection[log_con_string]:
                    merged_logicalservice.append(service)
            dblookup_file = write_file('dblookup-%s.xml'%site.split('.')[1],dir_name)
            indent(root, level=0)
            merged_service_tree.write(dblookup_file)
            if site.split('.')[1] == 'SARA':
                dblookup_file_nikhef = write_file('dblookup-NIKHEF.xml',dir_name)
                merged_service_tree.write(dblookup_file_nikhef)
        Popen(shlex.split('tcsh -c "rm authentication.xml dblookup.xml"')).wait()
        print "\nNew files are placed in './conddb_current_date&time/'."

    except KeyboardInterrupt:
        print "Aborted by user."
        cleanup_remains(dir_name)
        return 0

if __name__ == '__main__':
    sys.exit(main())