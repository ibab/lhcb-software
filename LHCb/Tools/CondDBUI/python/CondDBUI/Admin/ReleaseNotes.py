"""
Utilities to interact with XML ReleaseNotes. 
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: ReleaseNotes.py,v 1.3 2008-07-16 16:28:08 marcocle Exp $"

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

class ReleaseNotes(object):
    def __init__(self, filename):
        self.filename = filename
        # load XML
        self.tree = ET.parse(self.filename)
    def addNote(self, contributor, partitions, description, date = None):
        """
        And a basic entry to the release notes.
        
        contributor: person providing the changes
        partitions: dictionary in the format {"PARTITION":("tag",["file1","file2"])}
        description: list of comments, ["comment1","comment2"]
        date: date in the format "YYYY-MM-DD", the current date is used if omitted
        """
        if date is None:
            date = time.strftime("%Y-%m-%d")
        
        # helper function
        xel = lambda t: ET.QName("http://lhcb.cern.ch",t)
        
        note = ET.Element(xel("note"))
        ET.SubElement(note,xel("contributor")).text = contributor
        ET.SubElement(note,xel("date")).text = date
        
        for part in partitions:
            tag, files = partitions[part]
            part_el = ET.SubElement(note,xel("partition"))
            ET.SubElement(part_el,xel("name")).text = part
            ET.SubElement(part_el,xel("tag")).text = tag
            files = list(files)
            files.sort()
            for f in files:
                ET.SubElement(part_el,xel("file")).text = f
        
        desc_el = ET.SubElement(note,xel("description"))
        for d in description:
            ET.SubElement(desc_el, xel("di")).text = d
        
        # this should be safe enough: the first element is always "maintainer"
        pos = 0
        for i in self.tree.getroot().getchildren():
            if "maintainer" not in i.tag:
                break
            pos += 1

        self.tree.getroot().insert(pos,note)
        
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
