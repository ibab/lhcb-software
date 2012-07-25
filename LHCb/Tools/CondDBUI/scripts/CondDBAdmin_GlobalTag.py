#!/usr/bin/env python

__author__ = "Marco Clemencic <marco.clemencic@cern.ch>, Illya Shapoval <illya.shapoval@cern.ch>"
__version__ = "$Id: CondDBAdmin_GlobalTag.py,v 1.1 2009-09-14 15:56:58 ishapova Exp $"

import os, sys, CondDBUI

def reallySure():
    ##########################################################################################
    # Function "Are You really-really sure?" to exit the script if user will change his mind.
    ##########################################################################################
    ans = None
    while ans is None:
        ans = raw_input("\nDo you really want to start global tagging (Yes,[No])? ")
        if not ans: ans = "No"
        if ans not in [ "Yes", "No" ]:
            print "You have to type exactly 'Yes' or 'No'"
            ans = None

    if ans == "No":
        print "Canceled by user. Global tagging wasn't done."
        return False

    try:
        import time
        print "Global tagging will start in ..."
        timeout = 5
        while timeout >= 0:
            print "%d seconds" % timeout
            time.sleep(1)
            timeout -= 1
    except KeyboardInterrupt:
        print "Canceled by user. Global tagging wasn't done."
        return False
    # Yes, really sure
    return True

def updateRelNotes(db, contributor, new_globalTag, base_globalTag, localTags, datatypes, rn_file, description = None):
    ##########################################################################################
    # Release notes update preparation procedure
    ##########################################################################################
    from CondDBUI.Admin import ReleaseNotes
    try:
        rel_notes = ReleaseNotes(rn_file)
        rel_notes.addGlobalTag(contributor,
                    new_globalTag,
                    {db.db.databaseName():(base_globalTag, localTags)},
                    datatypes,
                    description)
    except IOError:
        print "\nSorry.. Path to Release Notes file is not valid or hardware IO problems occurred.\nCommitting was not done.\nCheck path and try again."
        sys.exit(0)

    return rel_notes

def main():
    ###########################################################################
    # Configure the parser
    ###########################################################################
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] partition new_globalTag "
                          "base_globalTag [localTag1 localTag2 ... localTagN]",
                          version = __version__,
                          description =
"""This script performs global tagging in requested partition of CondDB and does the update of
the release notes. The user has to provide a source (partition, e.g.: DDDB, LHCBCOND or SIMCOND),
a name for new global tag, a base global tag name and the local tag(s) for add-on. The script will
ask for the contributor name.""")

    parser.add_option("--rel-notes", type = "string",
                      help = "XML file containing the release notes"
                      )
    parser.add_option("-m", "--message", type = "string",
                      help = "Message to include in the release notes about global tag."
                      )
    parser.add_option("-c","--contributor", type = "string",
                      help = "Name of the contributor of the global tag."
                      )
    parser.add_option("-d","--datatypes", type = "string",
                      help = "List of data types new global tag is intended for."
                      )
    parser.add_option("--hash-alg", type = "string",
                      help = "Hash algorithm to compute the sum for new global"
                      " tag (md5, sha1, etc). Default: sha1."
                      )
    parser.add_option("-n","--dry-run", action = "store_true",
                      help = "Skip the actual global tagging and the update of release notes."
                      )

    parser.set_default("rel_notes", os.path.normpath(os.path.join(os.environ["SQLITEDBPATH"],
                                                                  "..", "doc", "release_notes.xml")))
    parser.set_default("message", None)
    parser.set_default("contributor", None)
    parser.set_default("datatypes", [])
    parser.set_default("hash_alg", "sha1")

    # parse command line
    options, args = parser.parse_args()

    ###########################################################################
    # Prepare local logger
    ###########################################################################
    import logging
    log = logging.getLogger(parser.prog or os.path.basename(sys.argv[0]))
    log.setLevel(logging.INFO)

    # set the global stream handler
    from CondDBUI import LOG_FORMAT
    hndlr = logging.StreamHandler()
    hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().handlers = [hndlr]

    ###########################################################################
    # Preliminary positional arguments verifications
    ###########################################################################
    if len(args) < 3:
        parser.error("Not enough arguments. Try with --help.")
    # prepare contributor name
    if not options.contributor:
        options.contributor = raw_input("Contributor: ")
    log.info("New global tag by: %s" % options.contributor)

    # prepare the message
    if not options.message:
        options.message = ""
    else:
        options.message = options.message.replace("\\n","\n")
        log.info("Message for the changes: '%s'" % options.message)

    ###########################################################################
    # Positional arguments redefinition and partition verification
    ###########################################################################
    partition, new_globalTag, base_globalTag = args[:3]
    localTags = args[3:]

    partitions = ["DDDB", "LHCBCOND", "SIMCOND", "DQFLAGS"]
    if partition not in partitions:
        parser.error("'%s' is not a valid partition name. Allowed: %s" % \
                     (partition, partitions))

    # Checking requested hashing algorithm to be available
    import hashlib
    hashAlg = options.hash_alg
    if sys.version_info >= (2,7):
        if hashAlg not in hashlib.algorithms:
            raise Exception, "'%s' algorithm is not implemented in the hashlib." %hashAlg
    else:
        if hashAlg not in ['md5','sha1','sha224','sha256', 'sha384', 'sha512']:
            raise Exception, "'%s' algorithm is not implemented in the hashlib." %hashAlg
    initialHashSumObj = getattr(hashlib,hashAlg)()

    # Processing the data type option string
    if options.datatypes != []:
        datatypes = []
        word = ""
        for i in options.datatypes:
            if i != ",":
                word += i
            elif i == ",":
                datatypes.append(word)
                word = ""
        datatypes.append(word)
    else:
        datatypes = options.datatypes
    log.info("New global tag name: %s" %new_globalTag)
    log.info("Base global tag name: %s" %base_globalTag)
    log.info("Local tags set for add-on: %s" %localTags)
    log.info("This global tag is intended for the following data types: %s" %datatypes)
    log.info("Hash sum will be computed using: %s" %hashAlg)

    ###########################################################################
    # Connecting to the DB
    ###########################################################################
    import CondDBUI.Admin.Management
    masterURL = CondDBUI.Admin.Management._masterCoolURL(partition)
    log.info("Master DB file = %s" % masterURL)
    log.info("Release Notes file = %s" %options.rel_notes)
    db = CondDBUI.CondDB(masterURL,readOnly=False)

    ###########################################################################
    # Processing the case of cloning the global tag with new name
    ###########################################################################
    if len(localTags) == 0:
        rel_notes = updateRelNotes(db, options.contributor, new_globalTag,
                                   base_globalTag, localTags, datatypes,
                                   options.rel_notes, options.message.splitlines())
        log.info("Script entered cloning mode!")
        if not reallySure():
            return 0
        log.info("Cloning tag %s as %s" % (base_globalTag, new_globalTag))
        if not options.dry_run:
            # Performing global tag cloning
            try:
                db.cloneTag("/", base_globalTag, new_globalTag)
            except IOError:
                log.error("\nSorry.. IO error happened, probably due to some \
                 hardware problems.\nGlobal tagging wasn't done.\nPlease try again.")
                return 1
            # Calculate the hash sum for just created tag
            log.info("Hashing tag '%s'..."%new_globalTag)
            hashSumObj = db.payloadToHash(initialHashSumObj,tag = new_globalTag)
            # Add hash sum to release notes
            rel_notes.addHashToGlobalTag(hashSumObj, partition, new_globalTag)
            log.info("Done.")

            # Write to release_notes.xml file new global tag entry
            rel_notes.write()
            log.info("Updated release notes at %s"%options.rel_notes)
        else:
            log.info("Cloning wasn't performed. Dry-run mode was active.")
        return 0

    ###########################################################################
    # Find the local tag of each Folder for the preferred tag
    ###########################################################################
    log.info("Collecting tags information ...")
    nodes_tags = {}
    found_tags = set()
    nodes = db.getAllNodes()
    for n in filter(db.db.existsFolder,nodes):
        f = db.getCOOLNode(n)
        nodes_tags[n] = None
        for r in localTags + [base_globalTag]:
            try:
                loc = f.resolveTag(r)
                nodes_tags[n] = loc
                found_tags.add(r)
                break
            except:
                # tag not found
                pass
    not_found = set(localTags + [base_globalTag]) - found_tags
    if not_found:
        log.warning("You have missing tags:")
        for t in not_found:
            log.warning("\t" + t)

    ignored_files = []
    for n in nodes_tags.keys():
        if nodes_tags[n] is None:
            ignored_files.append(n)
            del nodes_tags[n]
    if ignored_files:
        log.warning("The following files will be ignored by the tag:" +
                    '\n\t' + '\n\t'.join(ignored_files))

    ###########################################################################
    # User verification and final modification of DB and Release notes
    ###########################################################################
    rel_notes = updateRelNotes(db, options.contributor, new_globalTag,
                               base_globalTag, localTags,
                               datatypes, options.rel_notes,
                               options.message.splitlines())
    if not reallySure():
        return 0
    if not options.dry_run:
        if partition == "DDDB": log.info("Be patient, DDDB tagging is time consuming...")
        log.info("Tagging ...")
        # Apply changes to connected DB
        try:
            db.moveTagOnNodes('/', new_globalTag, nodes_tags)
        except IOError:
            log.error("\nSorry.. IO error happened, probably due to some \
            hardware problems.\nGlobal tagging wasn't done.\nPlease try again.")
            return 1

        # Calculate the hash sum for just created tag
        log.info("Hashing tag '%s'..."%new_globalTag)
        hashSumObj = db.payloadToHash(initialHashSumObj,tag = new_globalTag)
        # Add hash sum to release notes
        rel_notes.addHashToGlobalTag(hashSumObj, partition, new_globalTag)
        log.info("Done.")

        # Write to release_notes.xml file new global tag entry
        rel_notes.write()
        log.info("Updated release notes at %s"%options.rel_notes)
    else:
        log.info("Global tagging wasn't done. Dry-run mode was active.")

    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())
