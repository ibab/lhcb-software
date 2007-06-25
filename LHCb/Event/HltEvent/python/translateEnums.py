# /usr/bin/env python

DEBUG = False
ROOT = "Event"
NAMESPACE = "LHCb::HltEnums::"
DIR = "../Event/"
HEADERS = [["HltEnums","HltNames"],]

def writeHeader(ofname,iheader,oheader):
    type = oheader
    if (DEBUG): print " opening... ",ofname
    ofile = open(ofname,"w")
    ss = type.upper()+"_"+type.upper()+"_H"
    s  = "#ifndef "+ss+"\n"
    s += "#define "+ss+" 1 \n"
    s += '#include "'+ROOT+'/'+iheader+'.h" \n'
    if (DEBUG): print s
    ofile.write(s)
    s =  "// Python automatic translation of Enums "+iheader+" <-> strings \n"
    s += "//                                 J.Hernando 10/5/07\n "
    if (DEBUG): print s
    ofile.write(s)
    s = "namespace "+type+" {\n"
    if (DEBUG): print s
    ofile.write(s)

def writeTail(ofname):
    if (DEBUG): print " opening... ",ofname
    ofile = open(ofname,"a")
    s = "};\n"
    s += "#endif\n"
    if (DEBUG): print s
    ofile.write(s)
    ofile.close()

def writeEnum(ofname,type,names):
    if (DEBUG): print " opening... ",ofname
    ofile = open(ofname,"a")
    ie = type.find("t")
    cha = (type[ie+1:ie+2]).lower()
    type = cha+type[ie+2:]
    def mywrite(s):
        if (DEBUG): print s
        ofile.write(s)
        
    mywrite("\t inline int "+type+"ID(const std::string& e){\n")
    for name in names:
        mywrite('\t\t if (e == "'+name+'") return '+NAMESPACE+name+";\n")
    mywrite("\t\t return -1; \n")
    mywrite("\t}\n")
    
    mywrite("\t inline std::string "+type+"Str(int e){\n")
    mywrite("\t\t switch(e) { \n")
    for name in names:
        mywrite('\t\t case '+NAMESPACE+name+' : return "'+name+'";\n')
    mywrite('\t\t default: return "'+type+'Unknown";\n')
    mywrite("\t\t}\n")
    mywrite("\t}\n")

    mywrite("\t inline std::vector<std::string> "+type+"Names(){\n")
    mywrite("\t std::vector<std::string> names;\n")
    for name in names:
        mywrite('\t names.push_back("'+name+'");\n')
    mywrite('\t return names;\n')
    mywrite("\t}\n")
    
def getEnums(ofilename):
    if (DEBUG): print " opening... ",ofilename
    ofile = open(ofilename,"r")
    lines = ofile.readlines()
    ofile.close()
    enums = {}
    enumOpen = False
    for line in lines:
        if (line.find("enum")>0):
            enumOpen = True
            enumList = []
            words = line.split()
            enumName = words[1][:-1]
            ok = words[1][-1]
            if (ok != "{"): break
            varname  = words[2][:-1]
            # print " Enum ",enumName
            ie = varname.find("=")
            if (ie>=0): varname = varname[:ie]
            # print " \t ",varname
            enumList.append(varname)                           
        elif (enumOpen):
            if (line.find("};")>=0):
                enumOpen = False
                enums[enumName] = enumList
            else:
                words = line.split()
                varname = words[0][:]
                ie = varname.find("=")
                if (ie>=0): varname = varname[:ie]
                ok = varname[-1]
                if (ok == ","): varname = varname[:-1]
                # print " \t ",varname
                enumList.append(varname)
    if (DEBUG):
        for key in enums.keys():
            print key, enums[key]
    return enums
            
def write(dir,iheader,oheader):
    ifname = dir+iheader+".h"
    ofname = dir+oheader+".h"
    enums = getEnums(ifname)
    writeHeader(ofname,iheader,oheader)
    for enum in enums.keys():
        labels = enums[enum]
        writeEnum(ofname,enum,labels)
    writeTail(ofname)

def go():

    for header in HEADERS:
        write(DIR,header[0],header[1])
     

go()
