#!/usr/bin/env python
#author: Albert Bursche albert.bursche@physik.uzh.ch
#
#Calculates the difference of th databases given as .db files in the first two arguments
#and saves the result to the file given in the third argument.
#
#To do so the files are converted to XML and for each element with the tag "condition" in the first db the  "condition" element with the
#same name is selected and the paramVector objects "dPosXYZ" and "dRotXYZ" are substracted.
#
def InitialiseOptions():
    from optparse import OptionParser
    usage = "usage: %prog [options] arg"
    parser = OptionParser(usage)
    parser.add_option("-x", "--expession", dest="functor",
                      help="Expression to apply instead of difference "
                      "use x to acces to alignement constant form the first DB"
                      " and y for the alignement constant from the second DB."
                      "Mathmatical funtions can be accessed from the math module like math.sqrt(x) [default: %default]",
                      default="x-y")
    parser.add_option("-f","--XmlFilesToProcess",dest="files",
                      help="Pattern to match the files to process [default: %default]",
                      default="/Conditions/*/Alignment/*.xml")
    parser.add_option("-c","--CoordinatesToChange",dest="coords",
                      help="Pattern to match the files to process [default: %default]",
                      default="RxRyRzTxTyTz")
    parser.add_option("-p","--ParameterNames",dest="RegExName",
                      help="Regular expression all alignables have to match to be processed  [default: %default]",
                      default=".*")
    parser.add_option("-k","--keepTempDirs",
                      help="If this flag is given the tempdirs are not deleted. This allows to inspect the XML files for debugging.",
                      action="store_true",dest="NoCleanup",default=False)
    global Name, Params, Pattern, Functor, options
    (options, args) = parser.parse_args()
#    print options
    Name = options.RegExName
    Params = options.coords
    Functor = eval("lambda x,y:"+options.functor.replace("math","__import__('math')"))
    Name = options.files
    return args
def main():
    
    import tempfile, os, stat,shutil, sys, math
    from xml.etree.ElementTree import ElementTree
    args = InitialiseOptions()

    inputfiles = args[0:2]
    outfile = args[2]
    
    print inputfiles , outfile
    
    dirs = [tempfile.mkdtemp(),tempfile.mkdtemp()]
    getXMLFromDB(inputfiles,dirs)
    ETs = [getETs(d) for d in dirs]
    doCalculations(ETs)
    result = ETs[0]
    save(result,outfile)

    #cleanup tempfiles
    if not options.NoCleanup:
        for d in dirs:
            shutil.rmtree(d)
            
def doCalculations(ETs):
    """
loops over all files in ETs[0]
    loop over all elements with tag "condition" in  ETs[0]
       find the element with equal name in all other elements of ETs and calls calcDiff on each pair.
"""
    import re
    basenames = range(len(ETs))
    for i,ET in enumerate(ETs):
        basenames[i] = []
        for name in ET:
            basenames[i].append(name.split("/Conditions/")[1])
        basenames[i].sort()
    for i in range(len(ETs)):
        assert basenames[i]==basenames[0]
    bns = {}
    for bn in basenames[0]:
        bns[bn]=[]
        for ET in ETs:
            for name in ET:
                if bn in name:
                    bns[bn].append(ET[name])

    for (basen,bnETs) in bns.iteritems():
#        print basen, bnETs
        et0 = bnETs[0]
        ETroot = et0.getroot()
        for item in  et0.getiterator():
            if item.tag == "condition":
#                print item,item.tag,item.attrib,item.text,item.getchildren()
                name = item.attrib["name"]
                if not Name or re.match(Name,name):
                  for et in bnETs:
                    if et==et0:
                        continue
                    for item2 in et.getiterator("condition"):
#                        if item2.tag =="condition":
                            if item2.attrib["name"] == name:
#                                print name, item.attrib, item2.attrib
                                calcDiff(item,item2)

def calcDiff(a,b):
    """
Calculates the difference (or whatever the user requested) between the objects a and b
and stores it in a.

This is done for paramVector objects named "dPosXYZ" or "dRotXYZ".
"""
#    import math # may be used in Functor
    ca = a.getchildren()
    cb = b.getchildren()
    for type in ["dPosXYZ","dRotXYZ"]:
        [ea,eb] = [e for e in ca+cb if e.attrib["name"]==type]
#        print ea.attrib,eb.attrib
#        print ea.text,eb.text
        convertparamVector = lambda x: map(float,x.text.split(" "))
        #print ea.attrib["name"]
        ead = convertparamVector(ea)
        ebd = convertparamVector(eb)
        ecd = [0,0,0]
        param = ""
        if type == "dPosXYZ":
            param += "T"
        if type == "dRotXYZ":
            param += "R"
             
        
        ##calculation of the difference
        for i in range(3):
            if param+"xyz"[i] in Params:
                ecd[i]=Functor(ead[i],ebd[i])
            else:
                ecd[i]=ead[i]
#            ecd[i]=ead[i]-ebd[i]

        #print ead,ebd,ecd
        ea.text = reduce(lambda x,y:x+" "+y,map(str,ecd))

def getXMLFromDB(fnDBs,fnXMLs):
    script = ""
    for i,fnDB in enumerate(fnDBs):
        fnXML = fnXMLs[i]
        script += "dump_db_to_files.py -c sqlite_file:"+fnDB+"/LHCBCOND -d "+fnXML+"\n"
    ExecInLHCbBash(script)

#def find(directory,pattern):
#    import re,os
#    patternRE = re.compile(pattern)
#    outp = []
#    for obj in [os.path.join(directory,o) for o in os.listdir(directory)]:
##        print obj
#        if os.path.isdir(obj):
#            outp += find(obj,pattern)
#        else:
#            if patternRE.match(obj):
#                outp.append(obj)
#    return outp

def getETs(directory):
    """
    Get a dictionary of the form xmlFileName: xml.etree.ElementTree for all XML file in "directory"
    """
    from xml.etree.ElementTree import ElementTree
    import glob
#    files = find(directory,".*xml")
    files = glob.glob(directory+"/Conditions/*/Alignment/*.xml")
#    print files
#    print glob.glob(directory+"/Conditions/*/Alignment/*.xml") == find(directory,".*xml")
    outp = {}
    for file in files:
        et = ElementTree()
        et.parse(file)
#        print et
        outp[file]=et
    return outp
    
        
def save(result,outfile):
    """
    Saves the ElementTree objects in result in the file outfile.
    The XML files the Tree was created from are overwritten to get a starting point for copy_files_to_db.py
    """
    name = ""
    for (file,et) in result.iteritems():
        et.write(file)
        name = file.split("/Conditions/")[0]+"/Conditions/"
    ExecInLHCbBash("copy_files_to_db.py -s "+name+" -c sqlite_file:"+outfile+"/LHCBCOND\n")
    
def ExecInLHCbBash(code):
    
#    script =  "#!/bin/bash\n"
#    script += "source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v5r0/InstallArea/scripts/SetupProject.sh LHCb\n"
    import subprocess
    script = ""
    p = subprocess.Popen("/bin/bash", shell=False,stdin=subprocess.PIPE)
    pio = p.communicate(script+code)
    p.wait()

if __name__ == "__main__":
    main()
