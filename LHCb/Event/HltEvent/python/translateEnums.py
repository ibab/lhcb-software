
class Enums:

    def __init__(self,names,mode = 0):
        i = 0
        if (mode != 0): i = 1
        self.dict = {}
        self.names = []
        for name in names:
            self.names.append(name)
            setattr(self,name,i)
            self.dict[i] = name
            if (mode == 0): i = i+1
            else: i = i*2

    def toInt(self,name):
        return getattr(self,name)
    
    def toStr(self,i):
        return self.dict[i]

def writeHeader(ofname,type):
    ofile = open(ofname,"w")
    ss = type.upper()+"_"+type.upper()+"_H"
    s  = "#ifndef "+ss+"\n"
    s += "#define "+ss+" 1 \n"
    s += '#include "Event/HltEnums.h" \n'
    ofile.write(s)
    s = "namespace "+type+" {\n"
    ofile.write(s)

def writeTail(ofname):
    ofile = open(ofname,"a")
    s = "};\n"
    s += "#endif\n"
    ofile.write(s)
    ofile.close()

def writeCpp(type,names,ofilename):
    ofile = open(ofilename,"a")
    ie = type.find("t")
    cha = (type[ie+1:ie+2]).lower()
    type = cha+type[ie+2:]
    ofile.write("\t inline int "+type+"ID(const std::string& e){\n")
    for name in names:
        ofile.write('\t\t if (e == "'+name+'") return LHCb::HltEnums::'+name+";\n")
    ofile.write("\t\t return -1; \n")
    ofile.write("\t}\n")
    ofile.write("\t inline std::string "+type+"Str(int e){\n")
    ofile.write("\t\t switch(e) { \n")
    for name in names:
        ofile.write('\t\t case LHCb::HltEnums::'+name+' : return "'+name+'";\n')
    ofile.write('\t\t default: return "'+type+'Unknown";\n')
    ofile.write("\t\t}\n")
    ofile.write("\t}\n")

def getEnums(ofilename):
    ofile = open(ofilename,"r")
    lines = ofile.readlines()
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
            print " Enum ",enumName
            ie = varname.find("=")
            if (ie>=0): varname = varname[:ie]
            print " \t ",varname
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
                print " \t ",varname
                enumList.append(varname)
    print enums
    return enums
            
def write(ifname,ofname):
    writeHeader(ofname,"HltNames")
    enums = getEnums(ifname)
    for key in enums.keys():
        vars = enums[key]
        writeCpp(key,vars,ofname)
    writeTail(ofname)
    
                
def __main__():
    write("Hlt.h","HltNames.h")
     
