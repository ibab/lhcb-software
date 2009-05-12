"""
Utilities to interact with XML ReleaseNotes. 
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: ReleaseNotes.py,v 1.5 2009-05-12 15:35:55 marcocle Exp $"

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
        
    def addNote(self, contributor, partitions, description, date = None, patch = None):
        """
        And a basic entry to the release notes.
        
        contributor: person providing the changes
        partitions: dictionary in the format {"PARTITION":("tag",["file1","file2"])}
        description: list of comments, ["comment1","comment2"]
        date: date in the format "YYYY-MM-DD", the current date is used if omitted
        patch: numeric id of the patch (on savannah)
        """
        
        note = self._makeEntry("note", contributor, date)
        
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
        
        self._prependEntry(note)
        
    def addGlobalTag(self, contributor, tag, partitions, description = None, date = None, patch = None):
        """
        And an entry for a global tag to the release notes.
        
        contributor: person providing the changes
        tag: the name of the global tag
        partitions: dictionary in the format {"PARTITION":("base_tag",["tag1","tag2"])}
        description: list of comments, ["comment1","comment2"]
        date: date in the format "YYYY-MM-DD", the current date is used if omitted
        patch: numeric id of the patch (on savannah)
        """
        
        global_tag = self._makeEntry("global_tag", contributor, date)
        ET.SubElement(global_tag,_xel("tag")).text = tag
        
        self._setDescription(global_tag, description, patch)
        
        for part in partitions:
            base_tag, tags = partitions[part]
            part_el = ET.SubElement(global_tag,_xel("partition"))
            ET.SubElement(part_el,_xel("name")).text = part
            ET.SubElement(part_el,_xel("base")).text = base_tag
            tags = list(tags)
            for t in tags:
                ET.SubElement(part_el,_xel("tag")).text = t
        
        self._prependEntry(global_tag)
        
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
