# MakeSnapShot.py
# Author: evh, 18/02/2009
# Looks for latest version of SQLDDDB
# Parses conddb/release_notes.xml and makes a snapshot for all tags in the
# latest release
# to be called as a post install procedure for SQLDDDB

from xml.sax import ContentHandler
from xml.sax import make_parser
import sys
import os


class FindTags(ContentHandler):
    def __init__(self): 
        self.elementnamesvector=[]	
	self.i=0
	self.inpartition=0
	self.done=0
	self.foundname=0

	
    def startElement(self, name, attrs):  
    	self.i=self.i+1
	self.elementnamesvector.append(name)
	if name=='lhcb:partition':
	   self.inpartition=1

	
    def characters(self, ch):
        if self.elementnamesvector[self.i-1]=='lhcb:tag' and self.inpartition==0: 
           if self.done==0 and self.foundname==1:
	      tag=ch
	      print 'Tag '+tag+' and name '+name+' found. Making snapshot.'
	      os.system('./MakeSnapShot.sh '+tag+' '+name)
	      self.done=1 
	if self.elementnamesvector[self.i-1]=='lhcb:name' and self.inpartition==0: 
           if self.foundname==0:
	      name=ch
	      print 'Name '+name+' found.'
	      self.foundname=1
	   
    
    def endElement(self, name):
        self.elementnamesvector.pop()
	self.i=self.i-1	 
	if name=='lhcb:partition':
	   self.inpartition=0
	   self.foundname=0
		    

def main(): 
   if len(sys.argv) < 2:
      print "Give the version number of SQLDDDB"
      return 
   SQLDDDB_version=sys.argv[1]
   release_notes=os.environ['LHCb_release_area']+'/DBASE/Det/SQLDDDB/'+SQLDDDB_version+'/doc/release_notes.xml'
   xparser = make_parser()
   xh = FindTags()
   xparser.setContentHandler(xh)
   xparser.parse(release_notes)

    
if __name__ == '__main__':
   main()    
