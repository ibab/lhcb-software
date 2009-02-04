""" General LHCb platform configuration """


def isNewStyleBinary(cmtconfig):
    newstyle = False
    if len(cmtconfig.split("-")) >1 :
#        if cmtconfig.endswith("-opt") or cmtconfig.endswith("-dbg") :
        newstyle = True
    return newstyle

def isBinaryDbg(cmtconfig):
    bindbg = True
    if isNewStyleBinary(cmtconfig) :
        if not cmtconfig.endswith("-dbg") :
            bindbg = False    
    else :
        if not cmtconfig.endswith("_dbg") :
            bindbg = False
    return bindbg

def isBinaryOpt(cmtconfig):
    binopt = True
    if isBinaryDbg(cmtconfig) :
        binopt = False
    return binopt


def getBinaryDbg(cmtconfig):
    cmtdbg = cmtconfig
    if not isBinaryDbg(cmtconfig) :
        if isNewStyleBinary(cmtconfig) :
            if cmtconfig.endswith("-opt") :
                cmtdbg = "-".join(cmtconfig.split("-")[:-1]) + "-dbg"
            else :
                cmtdbg += "-dbg"
        else :
            cmtdbg += "_dbg"
    return cmtdbg

def getBinaryOpt(cmtconfig):
    cmtopt = cmtconfig
    if isBinaryDbg(cmtconfig) :
        if isNewStyleBinary(cmtconfig) :
            cmtopt = "-".join(cmtconfig.split("-")[:-1]) + "-opt"
        else :
            cmtopt = "_".join(cmtconfig.split("_")[:-1])
    return cmtopt

def getCompiler(cmtconfig):
    compdef = None
    if isNewStyleBinary(cmtconfig) :
        compdef = cmtconfig.split("-")[2]
    else :
        compdef = cmtconfig.split("_")[2]
    return compdef

def getPlatformType(cmtconfig):
    platformtype = None
    if isNewStyleBinary(cmtconfig) :
        platformtype = cmtconfig.split("-")[1]
    else :
        platformtype = cmtconfig.split("_")[0]
    if platformtype == "sl5" :
        platformtype = "slc5"
    if platformtype == "sl4" :
        platformtype = "slc4"
    if platformtype == "sl3" :
        platformtype = "slc3"
    return platformtype


def getArchitecture(cmtconfig):
    architecture = None
    if isNewStyleBinary(cmtconfig) :
        architecture = cmtconfig.split("-")[0]
        if architecture == "ia32" :
            architecture = "i686"
        if architecture == "amd64" :
            architecture = "x86_64"
    else :
        archlist = cmtconfig.split("_")
        if not archlist[0].startswith("win") :
            architecture = archlist[1]
            if architecture == "i686" :
                architecture = "ia32"
            if architecture == "x86_64" :
                architecture = "i686"
    return architecture

def getConfig(architecture, platformtype, compiler, debug=False):
    cmtconfig = None
    if platformtype == "slc5" :
        if architecture == "ia32" :
            architecture = "i686"
        if architecture == "amd64" :
            architecture = "x86_64"
        if debug :
            cmtconfig = "-".join([architecture, platformtype, compiler, "dbg"])
        else :
            cmtconfig = "-".join([architecture, platformtype, compiler, "opt"])
    elif not platformtype.startswith("win") :
        if architecture == "i686" :
            architecture = "ia32"
        if architecture == "x86_64" :
            architecture = "i686"
        cmtconfig = "_".join([platformtype, architecture, compiler])
        if debug :
            cmtconfig += "_dbg"
    else :
        cmtconfig = "_".join([platformtype, compiler])        
        if debug :
            cmtconfig += "_dbg"
    return cmtconfig

binary_opt_list = ["slc3_ia32_gcc323", 
                   "slc4_ia32_gcc34", "slc4_amd64_gcc34", 
                   "x86_64-slc5-gcc34-opt", "i686-slc5-gcc34-opt",
                   "x86_64-slc5-gcc43-opt", "i686-slc5-gcc43-opt",
                   "win32_vc71"]

binary_dbg_list = [ getBinaryDbg(x) for x in binary_opt_list ]

binary_list = binary_opt_list + binary_dbg_list

