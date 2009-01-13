#!/usr/bin/env python

import httplib,urllib
import urlparse
import webbrowser
import os,sys
import cgi
import cgitb

cgitb.enable()


import os,sys


def httpExists(url):
    host, path = urlparse.urlsplit(url)[1:3]
    connection = httplib.HTTPConnection(host)  ## Make HTTPConnection Object
    connection.request("HEAD", path)
    response = connection.getresponse()        ## Grab HTTPResponse Object
    if response.status == 200 : 
        return True
    return False



def doxygen(t) :
# takes as input any class instance of the LHCb software
#  if not t.classID :
#    print 'no class ID defined, do not know what to do.'
#    return

    cl = type(t).__name__
    hcl = 'class'
    for c in cl :
        if c.isupper() : hcl += '_' + c.lower()
        else           : hcl += c
    hcl += '.html'

    packages = ['LBCOM','LHCB','GAUDI','REC','PHYS','ANALYSIS','HLT','ROOT']
    url = ''
    for package in packages :
        if package == 'ROOT' :
            url = 'http://root.cern.ch/root/html/'+cl+'.html'
        else :
            vers = ''
            target = os.sep + package.upper() + os.sep
            for value in os.environ.values() :
                l =  value.rfind(target)
                if l > 0 :
                    vers = value[l+len(package)+2:].split(os.sep)[0]
                    break
            if vers :
                if package == 'GAUDI' :
                    hcl = hcl.replace('classi','class')
                    url = 'http://proj-gaudi.web.cern.ch/proj-gaudi/releases/'+package.upper()+'/'+vers+'/doc/html/'+hcl
                else :
                    pn =  package.lower()
                    vn =  vers.split('_')[1]
                    urlbase = 'http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/'+pn+'/releases/'+vn+'/doxygen/'
                    urlclass = urlbase+'classes.html'
                    if httpExists(urlclass) :
                        classname = hcl.replace('::','_1_1')
                        xx  = urllib.urlopen(urlclass)
                        all = xx.readlines()
                        for line in all :
                            if line.find(classname)>-1 :
                                p1 = line.find(classname)
                                p2 = line[:p1].rfind('href')+6
                                url = urlbase+line[p2:p1]+classname
                                if httpExists(url) : break
        if url != '' :
            if httpExists(url) : break
            url = ''
    if url :
        webbrowser.open(url,new = 1)
        print url
    else :
        print 'no package found'
    
if __name__ == '__main__':
    pass
