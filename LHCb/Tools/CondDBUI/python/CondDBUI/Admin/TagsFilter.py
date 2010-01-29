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
    def __init__(self, partition, datatype, search_gts):
        # Variables to store the attributes of each global tag
        self.requested_partition, self.requested_datatype = partition, datatype
        self.search_gts = search_gts
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
        1) The list of all global tags wich correspond to the given partition 
        and data type.
        2) The tuple of the most recent global tag and all subsequent local tags for it.
        """
        
        # Cancellation of whitespaces processing by characters() 
        if ch in ("\n", "\t", "\t\t", "\t\t\t"): return 0
        
        # Local tag entry treating. Grabing marked elemets values. 
        if self.found_lt:
            if self.found_lt_Partition:
                self.lt_Partition = ch
            elif self.found_lt_Name:
                self.lt_Name = ch
            elif self.found_lt_DataType:
                # Choosing among all mentioned datatypes the one requested 
                if ch == self.requested_datatype:
                    self.lt_DataType = ch
                
            if self.lt_Name and self.lt_Partition and self.lt_DataType:
                if self.lt_Partition == self.requested_partition:
                    if self.lt_Name not in self.local_tags:
                        self.local_tags.append(str(self.lt_Name))
        
        # Global tag entry treating.  Grabing marked elemets values.
        elif self.found_gt:
            if self.found_gt_Name:
                self.gt_Name = ch
            elif self.found_gt_DataType:
                # Choosing among all mentioned datatypes the one requested 
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
        """Performs unsetting the flag_*_* variables, when processed element is left."""
        
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
                

def init_finder(partition, datatype, search_gts):
    """Initializing SAX handler and parser for the "release_notes.xml" file."""
    
    handler = ReleaseNotesHandler(partition, datatype, search_gts)
    parser = make_parser()
    parser.setContentHandler(handler)
    return parser, handler

def all_gts(partition, datatype, rel_notes = None):
    """Returns for the given partition and datatype the list of all global tags.
    
    The output is in the form of a list. The fist one global tag is the most recent one.    
    """
    
    parser, handler = init_finder(partition, datatype, True)
    if not rel_notes:
        rel_notes = os.path.join(os.environ["SQLDDDBROOT"], "doc", "release_notes.xml")
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

def last_gt_lts(partition, datatype, rel_notes = None):
    """Returns for the given partition and datatype the most recent global tag and \
    all subsequent local tags.
    
    The output is in the form: (global tag, [lt1, lt2, ... ,ltN]). You have to give 
    the partition name (DDDB, LHCBCOND or SIMCOND) and the data type (e.g., 2009 or MC09). 
    Odering of the local tags: from the most recent one, to the most old one.
    If for given pair partition-datatype no global tag is found for some reason - "None"
    will be returned, even if local tags were found for the condition.
    """
    
    parser, handler = init_finder(partition, datatype, False)
    if not rel_notes:
        rel_notes = os.path.join(os.environ["SQLDDDBROOT"], "doc", "release_notes.xml")
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
    parser = OptionParser(usage = "%prog [options] PARTITION DATA_TYPE",
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
    parser.add_option("-g","--search_GTs", action="store_true", dest="search_GTs", default = False,
                    help = "Search for all global tags with the given partition and data type"
                    )
    
    try:
        parser.set_default("rel_notes",
                os.path.join(os.environ["SQLDDDBROOT"], "doc", "release_notes.xml")
                    )
    except KeyError:
        print "Sorry.. Check your environment. SQLDDDBROOT variable is not set."
        return 1

    options, args = parser.parse_args()
    
    if len(args) != 2:
        parser.error("Not enough or too much of arguments. Try with --help.")
    
    partitions = ["DDDB", "LHCBCOND", "SIMCOND"]
    if args[0] not in partitions:
        parser.error("'%s' is not a valid partition name. Allowed: %s" % \
                    (args[0], partitions))
    #########################################################################################
    
    
    # Launch parsing and return the result
    if not options.search_GTs:
        print "\nThe most recent global tag and all subsequent local tags are:\n", \
            last_gt__lts(args[0], args[1], options.rel_notes), "\n"
    else:
        print "\nGlobal tags, which correspond to the given condition, are:\n", \
            all_gts(args[0], args[1], options.rel_notes), "\n"
        
if __name__ == '__main__':
    sys.exit(main())
