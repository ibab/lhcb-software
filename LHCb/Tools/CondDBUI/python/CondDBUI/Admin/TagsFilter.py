#!/usr/bin/env python
"""This is script to extract lists of tags for the given partition and data type.

Given the condition of specified partition and data type two cases can be processed:
1) Extraction of all global tags for that condition;
2) Extraction of the most recent one global tag and all subsequent local tags for it.
"""

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>"
__credits__ = "Illya Shapoval <illya.shapoval@cern.ch>, Marco Clemencic <marco.clemencic@cern.ch>"
__maintainer__ = "Illya Shapoval"
__version__ = "$Id: TagsFilter.py,v 1.0 2010/01/28 00:00:00 ishapova Exp $"

import os, sys

from xml.sax import make_parser, SAXException
from xml.sax.handler import ContentHandler

class ReleaseNotesHandler(ContentHandler):
    def __init__(self, partition, datatype, search_gts, search_lts):
        # Variables to store the attributes of each global tag
        self.requested_partition, self.requested_datatype = partition, datatype
        self.search_gts = search_gts
        self.search_lts = search_lts
        # Flags to indicate the element, content of which should be read.
        self.found_lt, self.found_lt_Name, self.found_lt_Partition, self.found_lt_DataType = False, False, False, False
        self.found_gt, self.found_gt_Name, self.found_gt_Partition, self.found_gt_DataType = False, False, False, False
        self.lt_Name, self.lt_Partition, self.lt_DataType = None, None, None
        self.gt_Name, self.gt_Partition, self.gt_DataType = None, None, None
        # Lists for found "global_tag + local tags", "local_tags" and all "global tags" for given condition
        self.globaltag_localtags = ()
        self.local_tags = []
        self.global_tags = []

    def startElement(self, name, attrs):
        """Performs claiming by setting flag_*_* variables, that needed element is opened."""

        # Local tag entry treating. Marking needed elements.
        if name == 'lhcb:note' and not self.search_gts:
            self.found_lt = True
        elif self.found_lt:
            if name == 'lhcb:name':
                self.found_lt_Partition = True
            elif name == 'lhcb:tag':
                self.found_lt_Name = True
            elif name == 'lhcb:type':
                self.found_lt_DataType = True

        # Global tag entry treating. Marking needed elements.
        elif name == 'lhcb:global_tag':
            self.found_gt = True
        elif self.found_gt:
            if not self.gt_Name and name == 'lhcb:tag':
                self.found_gt_Name = True
            elif name == 'lhcb:type':
                self.found_gt_DataType = True
            elif name == 'lhcb:name':
                self.found_gt_Partition = True

    def characters(self, ch):
        """Performs analysis of .xml file elements content.

        Depending on the request, this function fills:
        1) The list of all global tags which correspond to the given partition
        and data type.
        2) The tuple of the most recent global tag and all subsequent local tags for it.
        """

        # Cancellation of white-spaces processing by characters()
        if ch in ("\n", "\t", "\t\t", "\t\t\t"): return 0

        # Local tag entry treating. Grabbing marked elements values.
        if self.found_lt:
            if self.found_lt_Partition:
                self.lt_Partition = ch
            elif self.found_lt_Name:
                self.lt_Name = ch
            elif self.found_lt_DataType:
                # Choosing among all mentioned data types the one requested
                if ch == self.requested_datatype:
                    self.lt_DataType = ch

            if self.lt_Name and self.lt_Partition and self.lt_DataType:
                if self.lt_Partition == self.requested_partition:
                    if self.lt_Name not in self.local_tags:
                        self.local_tags.append(str(self.lt_Name))

        # Global tag entry treating.  Grabbing marked elements values.
        elif self.found_gt and not self.search_lts:
            if self.found_gt_Name:
                self.gt_Name = ch
            elif self.found_gt_DataType:
                # Choosing among all mentioned data types the one requested
                if ch == self.requested_datatype:
                    self.gt_DataType = ch
            elif self.found_gt_Partition:
                self.gt_Partition = ch

            if self.gt_Name and self.gt_Partition and self.gt_DataType:
                if self.gt_Partition == self.requested_partition:
                    if not self.search_gts:
                        self.globaltag_localtags = str(self.gt_Name), self.local_tags
                        raise SAXException('Found most recent global tag and all subsequent local tags for it.')
                    else:
                        self.global_tags.append(str(self.gt_Name))

    def endElement(self, name):
        """Performs resetting the flag_*_* variables, when processed element is left."""

        # Local tag entry treating.
        if self.found_lt:
            if name == 'lhcb:name':
                self.found_lt_Partition = False
            elif name == 'lhcb:tag':
                self.found_lt_Name = False
            elif name == 'lhcb:type':
                self.found_lt_DataType = False
            elif name == 'lhcb:note':
                self.found_lt = False
            elif name == 'lhcb:partition':
                # Prepare for searching in next LT partition element
                self.lt_Name, self.lt_Partition, self.lt_DataType = None, None, None
        # Global tag entry treating.
        elif self.found_gt:
            if name == 'lhcb:tag':
                self.found_gt_Name = False
            elif name == 'lhcb:type':
                self.found_gt_DataType = False
            elif name == 'lhcb:name':
                self.found_gt_Partition = False
                # GT entry structure allow resetting of it here,
                # instead of while leaving the partition element
                # It's a preparation for searching in next partition element of GT
                self.gt_Partition = None
            elif name == 'lhcb:global_tag':
                self.found_gt = False
                # Prepare for searching in next GT
                self.gt_Name, self.gt_DataType = None, None


def init_finder(partition, datatype, search_gts = False, search_lts = False):
    """Initializing SAX handler and parser for the "release_notes.xml" file."""

    handler = ReleaseNotesHandler(partition, datatype, search_gts, search_lts)
    parser = make_parser()
    parser.setContentHandler(handler)
    return parser, handler

def all_gts(partition, datatype, rel_notes = None):
    """Returns for the given partition and datatype the list of all global tags.

    The output is in the form of a list. The fist one global tag is the most recent one.
    """

    parser, handler = init_finder(partition, datatype, search_gts = True)
    if not rel_notes:
        rel_notes = os.path.join(os.environ["SQLITEDBPATH"], "..", "doc", "release_notes.xml")
    try:
        parser.parse(open(rel_notes))
        if len(handler.global_tags) != 0:
            return handler.global_tags
        else:
            return None
    except IOError:
        print "\nSorry.. There is no such file or IO problem has been met.\
        \nYou were using: %s \
        \nPlease check the path to 'release_notes.xml' or just try again later."\
        % rel_notes
        return 1

def all_lts(partition, datatype, rel_notes = None):
    """Returns for the given partition and datatype the list of all local tags.

    The output is in the form of a list. The fist one global tag is the most recent one.
    """

    parser, handler = init_finder(partition, datatype, search_lts = True)
    if not rel_notes:
        rel_notes = os.path.join(os.environ["SQLITEDBPATH"], "..", "doc", "release_notes.xml")
    try:
        parser.parse(open(rel_notes))
        if len(handler.local_tags) != 0:
            return handler.local_tags
        else:
            return None
    except IOError:
        print "\nSorry.. There is no such file or IO problem has been met.\
        \nYou were using: %s \
        \nPlease check the path to 'release_notes.xml' or just try again later."\
        % rel_notes
        return 1

def last_gt_lts(partition, datatype, rel_notes = None):
    """Returns for the given partition and datatype the most recent global tag and \
    all subsequent local tags.

    The output is in the form: (global tag, [lt1, lt2, ... ,ltN]). You have to give
    the partition name (DDDB, LHCBCOND or SIMCOND) and the data type (e.g., 2009 or MC09).
    Odering of the local tags: from the most recent one, to the most old one.
    If for given pair partition-datatype no global tag is found for some reason - "None"
    will be returned, even if local tags were found for the condition.
    """

    parser, handler = init_finder(partition, datatype)
    if not rel_notes:
        rel_notes = os.path.join(os.environ["SQLITEDBPATH"], "..", "doc", "release_notes.xml")
    try:
        parser.parse(open(rel_notes))
        return None
    except SAXException:
        return handler.globaltag_localtags
    except IOError:
        print "\nSorry.. There is no such file or IO problem has been met.\
        \nYou were using: %s \
        \nPlease check the path to 'release_notes.xml' or just try again later."\
        % rel_notes
        return 1

def main():
    ##########################################################################################
    # Configuring the script options and args parser
    ##########################################################################################
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] PARTITION(S) DATA_TYPE",
                        version = __version__,
                        description =
"""Script returns for the given partition and data type the most recent global tag and all \
subsequent local tags.

You have to give the partition name (DDDB, LHCBCOND or SIMCOND) and the data type
(e.g., 2009 or MC09). The output is in the form: (global_tag, [lt1, lt2, ... ,ltN]).
Ordering of the local tags: from the most recent one, to the most old one.
If for the given pair partition-datatype no global tag is found for some reason - "None"
will be returned, even if local tags were found for the given condition.
""")
    parser.add_option("-r", "--rel-notes", type = "string",
                    help = "XML file containing the release notes to analyze"
                    )
    parser.add_option("-g","--all_GTs", action="store_true", dest="all_GTs", default = False,
                    help = "Search for all global tags for the given partition and data type"
                    )
    parser.add_option("--update_bkk", action="store_true", dest="update_bkk", default = False,
                    help = "If is set, an update of bookkeeping database with a latest global tag \
for the given partition and data type will be done. The user will be asked for \
final confirmation."
                    )

    try:
        parser.set_default("rel_notes", os.path.join(os.environ["SQLITEDBPATH"],
                                                     "..", "doc", "release_notes.xml"))
    except KeyError:
        print "Sorry.. Check your environment. SQLITEDBPATH variable is not set."
        return 1

    options, args = parser.parse_args()
    if len(args) != 2:
        parser.error("Not enough or too much of arguments. Try with --help.")

    datatype = args[1]
    ################ Processing and validation of the given partitions#######################
    partitions = []
    word = ""
    for i in args[0]:
        if i != ",":
            word += i
        elif i == ",":
            partitions.append(word)
            word = ""
        elif i == " ":
            parser.error("Partitions coma separated list should be given without spaces.")
    partitions.append(word)

    standard_partitions = ["DDDB", "LHCBCOND", "SIMCOND"]
    for partition in partitions:
        if partition not in standard_partitions and partition != "all":
            parser.error("'%s' is not a valid partition name. Allowed are: %s and 'all'" % \
                        (partition, standard_partitions))
        elif partition == "all":
            partitions = standard_partitions
    print "\n###########################################################################################"
    print "#  Using %s" % options.rel_notes
    print "#  Partitions to look in: %s" %partitions
    print "#  Data type to look for is: %s" %datatype
    print "###########################################################################################"

    ########### Launch parsing and return the result#####################################
    if not options.update_bkk:
        if not options.all_GTs:
            print "\nThe most recent global tags and all new subsequent local tags for specified partition and datatype are:"
            for partition in partitions:
                print "\n\tFor %s:"%partition, last_gt_lts(partition, datatype, options.rel_notes)
        else:
            print "\nThe set of all global tags for specified partition and datatype are:"
            for partition in partitions:
                print "\n\tFor %s:"%partition, all_gts(partition, datatype, options.rel_notes)
    elif options.update_bkk and datatype == "BK":
        BK_tags = {"DDDB":[],"LHCBCOND":[],"SIMCOND":[]}
        if not options.all_GTs:
            print "\nThe most recent global tags with the 'bookkeeping' property are:"
            for partition in partitions:
                gt_lts = last_gt_lts(partition, datatype, options.rel_notes)
                if gt_lts:
                    gt,ltgs = gt_lts
                    BK_tags[partition].append(gt)
        else:
            print "\nAll global tags with the 'bookkeeping' property are:"
            for partition in partitions:
                gts = all_gts(partition, datatype, options.rel_notes)
                if gts:
                    BK_tags[partition] = gts
        print "\t", BK_tags

        ans = None
        while ans is None:
            ans = raw_input("\nDo you really want to update the Bookkeeping database (Yes,[No])? ")
            if not ans: ans = "No"
            if ans not in [ "Yes", "No" ]:
                print "You have to type exactly 'Yes' or 'No'"
                ans = None

        if ans == "No":
            print "...\nBookkeeping database update was cancelled by user. No changes were done to the db."
            return 0

        from DIRAC.Core.Base.Script import initialize
        initialize(enableCommandLine = False)
        from LHCbDIRAC.NewBookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
        cl = BookkeepingClient()
        retVal = cl.insertTag(BK_tags)
        if retVal['OK']:
            print "Bookkeeping database was updated."
            print retVal['Value']
        else:
            print retVal['Message']
    else:
        print "\nThe Bookkeeping database can only be updated with 'BK' data type global tags.\n\
The update process wasn't done."
        return 1

if __name__ == '__main__':
    sys.exit(main())

