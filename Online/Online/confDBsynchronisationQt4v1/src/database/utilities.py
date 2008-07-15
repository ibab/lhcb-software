import re
from string import upper, lower

modules = ["Tell", "Ukl1", "Hugin"]

systems = ["","RICH2","HCAL","ECAL","L0MUON","TFC","L0CALO","TRG","PRS","MUON","MUON_A",
"MUON_C","TT","VELO","VELO_A","VELO_C","OT","OT_A","OT_C","IT","TDET","PUS"]

def dump (obj):
    excl=["__doc__", "__init__", "__module__"]
    d=dir(obj)
    for i in d:
        if (i in excl): continue
        print i, eval('obj.'+i)

"""getModulType extracts the modul type"""
def getModuleType(name):
    for module in modules:
        if(re.search(upper(module), upper(name)) != None):
            if module == "Tell":
                return "Tell1"
            else:
                return module
    #print "Dont know ", name
    return "UNKNOWN_MOUDLE_TYPE"

"""extracts detector"""
def getDetector(s):
    s = lower(s)
    for m in ["tell", "ukl1", "hugin"]:
        if re.search(m, lower(s)):
            ss = s.split(lower(m))
            if ss[0] == "mu":
                if ss[1][0] == "a":
                    return "muon_a"
                elif ss[1][0] == "c":
                    return "muon_c"
                else:
                    return "muon"
            elif ss[0] == "ve":
                if ss[1][0] == "a":
                    return "velo_a"
                elif ss[1][0] == "c":
                    return "velo_c"
                else:
                    return "c"
            elif ss[0] == "it":
                    return "it"
            elif ss[0] == "ot":
                if ss[1][0] == "a":
                    return "ot_a"
                elif ss[1][0] == "c":
                    return "ot_c"
                else:
                    return "ot"
            elif ss[0] == "tt":
                return "tt"
            elif ss[0] == "ec":
                return "ecal"
            elif ss[0] == "hc":
                return "hcal"
            elif ss[0] == "ps":
                return "prs"
            elif ss[0] == "tca":
                return "tcal"
            elif ss[0] == "tmu":
                return "mutq"
            elif ss[0] == "r1":
                return "rich1"
            elif ss[0] == "r2":
                return "rich2"
            elif ss[0] == "dum":
                return "tdet"
            elif ss[0] == "tpu":
                return "pus"
            elif ss[0] == "tfc":
                return "tfc"
            else:
                return "UNKNOWN_DETECTOR"
            break
    return "UNKNOWN_DETECTOR"

def clearscreen(numlines=100):
    """Clear the console.
    numlines is an optional argument used only as a fall-back.
    """
    import os
    if os.name == "posix":
        # Unix/Linux/MacOS/BSD/etc
        os.system('clear')
    elif os.name in ("nt", "dos", "ce"):
        # DOS/Windows
        os.system('CLS')
    else:
        # Fallback for other operating systems.
        print '\n' * numlines