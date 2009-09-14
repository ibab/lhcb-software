"""
Utilities to interact with XML ReleaseNotes. 
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: ReleaseNotes.py,v 1.10 2009-09-14 15:56:58 ishapova Exp $"

# exported symbols
__all__ = [ "ReleaseNotes" ]

import xml.etree.ElementTree as ET
# define needed namespaces
ET._namespace_map["http://lhcb.cern.ch"] = "lhcb"
ET._namespace_map["http://www.w3.org/2001/XMLSchema-instance"] = "xsi"

import time

from _internals import log

def indent(elem, level=0, indentation = "  "):
    """
    Indent an ElementTree instance to allow pretty print.
    Code taken from http://effbot.org/zone/element-lib.htm and adapted to our
    needs.
    """
    i = "\n" + level*indentation
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + indentation
        for elem in elem:
            indent(elem, level+1, indentation)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

# helper function
_xel = lambda t: ET.QName("http://lhcb.cern.ch",t)

class ReleaseNotes(object):
    def __init__(self, filename):
        self.filename = filename
        # load XML
        self.tree = ET.parse(self.filename)
        import logging
        self.log = logging.getLogger("CondDB.Admin.ReleaseNotes")
        
    def _setDescription(self, node, description, patch = None):
        """
        Internal function to add the description to a node ("note" or "global_tag").
        """
        desc_el = None
        if description:
            desc_el = ET.SubElement(node,_xel("description"))
            for d in description:
                ET.SubElement(desc_el, _xel("di")).text = d
            if patch:
                ET.SubElement(desc_el, _xel("patch")).text = str(patch)
        return desc_el
        
    def _makeEntry(self, type_name, contributor, date = None):
        """
        Internal function to prepare a base entry ("note" or "global_tag").
        """
        if date is None:
            date = time.strftime("%Y-%m-%d")
        
        entry = ET.Element(_xel(type_name))
        ET.SubElement(entry,_xel("contributor")).text = contributor
        ET.SubElement(entry,_xel("date")).text = date
        
        return entry
    
    def _prependEntry(self, entry):
        """
        Internal function to add the entry at the beginning of the release notes
        (after the list of maintainers.)
        """
        # this should be safe enough: the first element is always "maintainer"
        pos = 0
        for i in self.tree.getroot().getchildren():
            if "maintainer" not in str(i.tag):
                break
            pos += 1

        self.tree.getroot().insert(pos, entry)
        
    def _findHomogeneous(self, contributor, date, tag = None, patch = None, description = None):
        """
        This internal function does look-up in the release_note.xml file for possible previously added homogeneous 
        tags entries, checking whether coincidence takes place for "tag" name, date and contributor in case of 
        a global tag processing, and date, contributor, patch and description for a local tag entry processing.
        It returns the object of found entry for further modifications. The homogeneous entry is looked only in 
        the area of release_notes.xml which is after the last one sqldddb release. 
        
        contributor: person providing the changes
        date: date in the format "YYYY-MM-DD", the current date is used if omitted
        tag: the name of the global tag
        patch: numeric id of the patch (on savannah)
        description: list of comments, ["comment1","comment2"]
        """
        
        if date is None:
            date = time.strftime("%Y-%m-%d")
        
        # Accessing the root element in .xml file  
        rootelement = self.tree.getroot()
        
        # Starting of iteration procedure for analyzing each element in the tree for duplications
        counter = 0 # counter for number of current entries found with proposed global tag name per one partition
        partitionList = [] # list of partitions found already containing proposed global tag name
        if not description:
            description = []
        
        for element in rootelement:
            if counter > 0: break
            if element.tag != _xel("sqldddb_tag"):
                bingoGeneral = 0 # Number of general for global and local tags matchings found in a target entry
                bingoLT = 0 # Number of local tag matchings (for description text and patch number) found in a target entry
                if element.tag == _xel("global_tag") or element.tag == _xel("note"):
                    for subelement in element:
                        if subelement.text == contributor or subelement.text == date or subelement.text == tag:
                            bingoGeneral += 1
                            continue
                        if bingoGeneral >= 2 and subelement.tag == _xel("partition"):
                            for subsubelement in subelement:
                                if subsubelement.tag == _xel("name"): partitionList.append(subsubelement.text)
                        elif bingoGeneral == 2 and subelement.tag == _xel("description"):
                            for subsubelement in subelement:
                                for line in description:
                                    if subsubelement.text == line:
                                        bingoLT += 1
                                if subsubelement.text == str(patch):
                                    bingoLT += 1
                    if bingoGeneral == 3 or bingoLT == len(description)+1:
                        counter += 1
                        homogenEntry = element
                        self.log.info('Homogeneous entry with tag name "%s" is found to be used in the CondDB for %d partition(s): %s' % (tag,counter,partitionList))
                        self.log.info('Merging will be done in release notes for homogeneous entries for patch "%s" ...' % patch)
                    else: homogenEntry = None
            else:
                homogenEntry = None
                break
                                             
        return homogenEntry
                
    def addNote(self, contributor, partitions, description, date = None, patch = None, forceNewLT = False):
        """
        And a basic entry to the release notes.
        
        contributor: person providing the changes
        partitions: dictionary in the format {"PARTITION":("tag",["file1","file2"])}
        description: list of comments, ["comment1","comment2"]
        date: date in the format "YYYY-MM-DD", the current date is used if omitted
        patch: numeric id of the patch (on savannah)
        forceNewLT: boolean flag to control unconditional creation of local tag entry in release_notes.xml
        """
        local_tag = partitions[partitions.keys()[0]][0]
        
        homogenEntry = self._findHomogeneous(contributor, date, local_tag, patch, description)
        if forceNewLT or not homogenEntry:
            note = self._makeEntry("note", contributor, date)
            # Adding new local tag entry element to the root element of release_notes.xml tree
            self._prependEntry(note)
        elif homogenEntry:
            note = homogenEntry
            descr_ChildElem = note.find(str(_xel("description")))
            if descr_ChildElem:
                note.remove(descr_ChildElem) 
        
        # Attaching sub-sub-elements to new sub-element or element of a new local tag entry
        for part in partitions:
            tag, files = partitions[part]
            part_el = ET.SubElement(note,_xel("partition"))
            ET.SubElement(part_el,_xel("name")).text = part
            ET.SubElement(part_el,_xel("tag")).text = tag
            files = list(files)
            files.sort()
            for f in files:
                ET.SubElement(part_el,_xel("file")).text = f
                
        self._setDescription(note, description, patch)
        
    def addGlobalTag(self, contributor, tag, partitions, description = None, date = None, patch = None, forceNewGT = False):
        """
        And an entry for a global tag to the release notes.
        
        contributor: person providing the changes
        tag: the name of the global tag
        partitions: dictionary in the format {"PARTITION":("base_tag",["tag1","tag2"])}
        description: list of comments, ["comment1","comment2"]
        date: date in the format "YYYY-MM-DD", the current date is used if omitted
        patch: numeric id of the patch (on savannah)
        forceNewGT: flag for forcing creation of a new global tag entry in a release_notes.xml file 
                    as a stand-alone entry in case any previous homogeneous entries are found.  
        """
        
        homogenEntry = self._findHomogeneous(contributor,date,tag)
        if forceNewGT or not homogenEntry:
            global_tag = self._makeEntry("global_tag", contributor, date)
            ET.SubElement(global_tag,_xel("tag")).text = tag
            # Adding new global tag entry element to the root element of release_notes.xml tree
            self._prependEntry(global_tag)
        elif homogenEntry:
            global_tag = homogenEntry
            descr_ChildElem = global_tag.find(str(_xel("description")))
            if descr_ChildElem:
                global_tag.remove(descr_ChildElem)
            
        
        # Attaching sub-sub-elements to new sub-element or element of a new global tag entry
        for part in partitions:
            base_tag, tags = partitions[part]
            part_el = ET.SubElement(global_tag,_xel("partition"))
            ET.SubElement(part_el,_xel("name")).text = part
            ET.SubElement(part_el,_xel("base")).text = base_tag
            tags = list(tags)
            for t in tags:
                ET.SubElement(part_el,_xel("tag")).text = t
        
        self._setDescription(global_tag, description, patch)     
        
    def write(self, filename = None , encoding = "utf-8"):
        if filename is None:
            filename = self.filename
        
        from StringIO import StringIO
        f = StringIO()
        
        # FIXME: (MCl) work-around for a limitation of E.T. in Python 2.5
        xml_hdr = '<?xml version="1.0" encoding="UTF-8"?>\n<?xml-stylesheet type="text/xsl" href="release_notes.xsl"?>\n'
        f.write(xml_hdr)
        indent(self.tree.getroot(), indentation = "\t")
        self.tree.write(f,encoding)
        
        open(filename,"w").write(f.getvalue()+"\n")
