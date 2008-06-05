from time import strftime
from string import replace
from string import upper, lower, split
from time import time
from time import strftime
from re import compile
"""creats a summary as a log file, only preview data, no real done actions"""
def createLog(controller):
    line = "-----------------------------------------------------------------------"
    datestamp = strftime("%m.%d.%Y - %H:%M:%S")
    datestamp = replace(datestamp, ":", "-")
    lf = open("log/"+datestamp + " summary.log", "w")
    for system in controller.changed_systems:
        lf.write(str(system))
    lf.close()