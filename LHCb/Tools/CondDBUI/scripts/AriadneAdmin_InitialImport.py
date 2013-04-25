#!/usr/bin/env python

# (C) Copyright 2013 CERN for the benefit of the LHCb Collaboration.
# This software is distributed under the terms of the GNU General Public Licence
# version 3 (GPL Version 3), copied verbatim in the file "COPYING".
# In applying this licence, CERN does not waive the privileges and immunities granted
# to it by virtue of its status as an Intergovernmental Organization or submit itself
# to any jurisdiction.

"""A script to perform an initial import of LHCb compatibility relationships. """


__author__ = "Illya Shapoval <Illya.Shapoval@cern.ch>"

from CondDBUI.Admin import ReleaseNotes
from ariadne import ariadne
import ast, os, re
from functools import partial
import logging

def _getApplicationVersions(app_name):
    """ Returns a list of known versions for an app. """

    path = os.environ.get('LHCBRELEASES')
    if not path:
        log.error("Can not find application's location. Exiting.")
        sys.exit(1)
    versions = [item.lstrip(app_name.upper() + '_')
                for item in os.listdir(os.path.join(path, app_name.upper()))
                if re.match(r'%s_v\d+(r\d+)?(p\d+)?'%app_name.upper(), item)]
    log.debug("Fetched versions are: %s"%versions)
    return versions

def _getApplicationPlatforms(app_name, app_ver):
    """ Returns a list of known platforms an application was released for. """

    path = os.environ.get('LHCBRELEASES')
    app = app_name.upper()
    platforms = [item.lstrip(app_name.upper() + '_')
                for item in os.listdir(os.path.join(path, app, app + '_' + app_ver, 'InstallArea'))
                if re.match(r'\w+-\w+-\w+-\w+', item)]
    log.debug("Fetched platforms are: %s"%platforms)
    return platforms

def _nodesCompatible(rn, node1, node2):

    node1_type = node1['type']
    node2_type = node2['type']

    if node1_type == node2_type == 'tag':
        node1_detector_types = rn.getDataType(node1['partition'], node1['name'])
        node2_detector_types = rn.getDataType(node2['partition'], node2['name'])
        common_detector_types = []
        for dt in node1_detector_types:
            if dt in node2_detector_types:
                common_detector_types.append(dt)
        return common_detector_types
    elif node1_type == 'detector_type' and node2_type == 'tag':
        node2_detector_types = rn.getDataType(node2['partition'], node2['name'])
        if node1['name'] in node2_detector_types:
            return True
    elif node1_type == 'application' and node2_type == 'tag':
        if node1['name'] == 'Gauss' and node2['partition'] == 'LHCBCOND':
            return False
        elif node1['name'] == 'Boole' and node2['partition'] == 'LHCBCOND':
            return False
        return True
    elif node1_type == 'application' and node2_type == 'detector_type':
        return True
    elif node1_type == 'application_family' and node2_type == 'application':
        return True


def main():
    # Configure the parser
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog DESTINATION [options]",
                          description =
"""A script to import data to the Ariadne Neo4j database (specified by the DESTINATION argument).""")

    parser.add_option("-a", "--application", type = "string",
                      help = "Application family to inject (e.g., Brunel), including "
                      "its all available versions, computing platforms, CondDB tag "
                      "nodes and relations to them."
                      )
    parser.add_option("-i", "--input-file", type = "string",
                      help = "A file to import containing regular strings per line each describing a graph path."
                      )
    parser.add_option("-d", "--debug", action = "store_true",
                      help = "Increase verbosity."
                      )

    # parse command line
    options, args = parser.parse_args()

    if len(args) != 1:
        parser.error("Wrong number of arguments. Try with --help.")

    ariadne_uri = args[0]
    application = options.application

    global log
    log = logging.getLogger(parser.prog or os.path.basename(sys.argv[0]))
    if options.debug:
        log.setLevel(logging.DEBUG)
        logging.getLogger("Ariadne").setLevel(logging.DEBUG)
    else:
        log.setLevel(logging.INFO)
        logging.getLogger("Ariadne").setLevel(logging.INFO)

    adb = ariadne.AriadneDB(uri = ariadne_uri)
    if not adb.db:
        log.error("Failed to initialize the Neo4J database service. Exiting.")
        sys.exit(1)

    rn_path = os.path.normpath(os.path.join(os.environ["SQLITEDBPATH"], '..', 'doc', "release_notes.xml"))
    log.debug("Using CondDB release notes at: %s"%rn_path)
    rn = ReleaseNotes(rn_path)
    _nodesCompatible_rn = partial(_nodesCompatible, rn)

    ans = None
    while ans is None:
        log.warning("This is DANGEROUS! Importing initial relationship field may"
        " result in a loss of currently known compatibility information at the destination DB!")
        ans = raw_input("\nDo you really want to start the import (Yes,[No])? ")
        if not ans: ans = "No"
        if ans not in [ "Yes", "No" ]:
            print "You have to type exactly 'Yes' or 'No'"
            ans = None
    if ans == 'No':
        log.info("Cancelled by user")
        return 0

    # Process the application requested and all the CondDB tag nodes
    if application:
        partitions = ['DDDB', 'LHCBCOND', 'SIMCOND', 'DQFLAGS']
        appfam_node = adb.get_or_create_AriadneNode({"type": "application_family", "name": application})

        log.info("Getting all known versions and platforms of %s application.." %application)
        version_nodes = []
        platforms_nodes = []
        versions = _getApplicationVersions(application)
        for v in versions:
            version_node = adb.get_or_create_AriadneNode({"type": "application", "name": application, "version": v})
            version_nodes.append(version_node)
            platforms = _getApplicationPlatforms(application, v)
            for p in platforms:
                p_node = adb.get_or_create_AriadneNode({"type": "platform", "name": p})
                if p_node not in platforms_nodes:
                    platforms_nodes.append(p_node)
            adb.interconnectNodeGroups([version_node], platforms_nodes, "BUILT_FOR")

        log.info("Creating 'detector_type' nodes..")
        detector_types_nodes = []
        for partition in partitions:
            detector_types = rn.collectAllDataTypes(partition)
            if 'BK' in detector_types: detector_types.remove('BK')
            if 'HLT' in detector_types: detector_types.remove('HLT')
            for detector_type in detector_types:
                dt_node = adb.get_or_create_AriadneNode({"type": "detector_type", "name": detector_type})
                if dt_node not in detector_types_nodes: detector_types_nodes.append(dt_node)
        log.debug("Following detector_types nodes are created: %s"%detector_types_nodes)

        log.info("Creating 'CondDB tag' nodes..")
        conddb_nodes = {}
        for partition in partitions:
            conddb_nodes[partition] = []
            detector_types = rn.collectAllDataTypes(partition)
            if 'BK' in detector_types: detector_types.remove('BK')
            if 'HLT' in detector_types: detector_types.remove('HLT')
            for detector_type in detector_types:
                tags = rn.getGlobalTags(partition, detector_type)
                log.debug("Partition: %s, DetectorType: %s, %s global tags:\n%s" %(partition, detector_type, len(tags), tags))
                for tag in tags:
                    date = tag.split('-')[1]
                    conddb_node = adb.get_or_create_AriadneNode({"type": "tag", "partition": partition, "name": tag, "release_date":date})
                    if conddb_node not in conddb_nodes[partition]: conddb_nodes[partition].append(conddb_node)
        log.debug("%s 'CondDB tag' nodes have been created"%len(detector_types_nodes))

        # Interconnect the nodes just created
        zero_node = adb.db.get_node(0)
        adb.db.get_or_create_relationships((zero_node, "COMPATIBLE", appfam_node))

        adb.interconnectNodeGroups([appfam_node], version_nodes, "COMPATIBLE", _nodesCompatible_rn)
        adb.interconnectNodeGroups(version_nodes, detector_types_nodes, "COMPATIBLE", _nodesCompatible_rn)

        adb.interconnectNodeGroups(detector_types_nodes, conddb_nodes['DDDB'], "COMPATIBLE", _nodesCompatible_rn)
        adb.interconnectNodeGroups(detector_types_nodes, conddb_nodes['LHCBCOND'], "COMPATIBLE", _nodesCompatible_rn)
        adb.interconnectNodeGroups(detector_types_nodes, conddb_nodes['SIMCOND'], "COMPATIBLE", _nodesCompatible_rn)
        adb.interconnectNodeGroups(detector_types_nodes, conddb_nodes['DQFLAGS'], "COMPATIBLE", _nodesCompatible_rn)

        adb.interconnectNodeGroups(version_nodes, conddb_nodes['DDDB'], "COMPATIBLE", _nodesCompatible_rn)
        adb.interconnectNodeGroups(version_nodes, conddb_nodes['LHCBCOND'], "COMPATIBLE", _nodesCompatible_rn)
        adb.interconnectNodeGroups(version_nodes, conddb_nodes['SIMCOND'], "COMPATIBLE", _nodesCompatible_rn)

        adb.interconnectNodeGroups(conddb_nodes['DDDB'], conddb_nodes['LHCBCOND'], "COMPATIBLE", _nodesCompatible_rn)
        adb.interconnectNodeGroups(conddb_nodes['DDDB'], conddb_nodes['SIMCOND'], "COMPATIBLE", _nodesCompatible_rn)

    # Process the input file
    if options.input_file:
        log.info("Importing records of file %s .." %options.input_file)
        with open(options.input_file) as f:
            for line in f.readlines():
                line = line.strip()
                if line and not line.startswith('#'):
                    node_spec_list = [ast.literal_eval(node_str.strip()) for node_str in line.split('--')]
                    adb.get_or_create_AriadnePath(node_spec_list, "COMPATIBLE")
        log.info("Done.")

if __name__ == '__main__':
    import sys
    sys.exit(main())