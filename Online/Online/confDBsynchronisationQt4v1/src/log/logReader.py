from __future__ import with_statement
from os import listdir
import os
class LogReader(object):
    def __init__(self):
        self.logDirectory = "log"
        self.seperator = "/"
    def getLatestLogFile(self, pattern):
        if os.getcwd().endswith("log"):
            directory = os.getcwd()
        else:
            directory = self.logDirectory
        files = listdir(directory)
        latest = None
        for file in files:
            if file.endswith(pattern):
                latest = file
        if file == None:
            return "No Log-File found!"
        else:
            return self.readFile(directory+self.seperator+latest)
    def getLatestSummaryLogAsString(self):
        return self.getLatestLogFile("summary.log")
    def getLatestSessionLogAsString(self):
        return self.getLatestLogFile("session.log")
    def readFile(self, file):
        log = ""
        print "opening "+file
        with open(file) as file:
            for line in file:
                log += line+"\n"
        return log