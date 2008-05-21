from time import strftime
from string import replace
from string import upper, lower, split
from time import time
from time import strftime
from re import compile
"""creats a summary as a log file, only preview data, no real done actions"""
def createLog(devices):
    line = "-----------------------------------------------------------------------"
    datestamp = strftime("%m.%d.%Y - %H:%M:%S")
    datestamp = replace(datestamp, ":", "-")
    lf = open("log/"+datestamp + " summary.log", "w")
    d = 0
    for item in devices.all_devices:
        if d%2 == 0:
            for i in item:
                lf.write("\n"+i.devicename+", "+ i.sn+", "+str(i.CIPadd)+", "+str(i.CMACadd)+", "+str(i.RNIPadd))
            lf.write("\n"+str(len(item))+" ")
        else:
            if item == " devices at all":
                lf.close()
                return
            lf.write(item+"\n")
            lf.write("\n"+line+"\n\n\n\n\n")
        d+=1
    lf.close()