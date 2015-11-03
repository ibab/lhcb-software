#!/usr/bin/env python
"""

Script to generate the JSON file with the list of versions

"""
import logging
import sys

from LbUtils.Script import Script
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB


class LbSdbGenerateVersionJSON(Script):
    """ Update information about a project / version to say whether it was built with CMake or CMT"""

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-d",
                          dest = "debug",
                          action = "store_true",
                          help = "Display debug output")


    def main(self):
        """ Main method for bootstrap and parsing the options.
        It invokes the appropriate method and  """
        self.log = logging.getLogger()

        opts = self.options
        args = self.args
        if opts.debug:
            self.log.setLevel(logging.DEBUG)
        else:
            self.log.setLevel(logging.WARNING)


        known_tags = [ "prod" ]
        
        # Connect to the ConfDB to update the platform
        self.mConfDB = SoftConfDB()

        allvers = {}

        for tag in known_tags:
            tmplist = []
            for p in sorted(self.mConfDB.listTag(tag.upper())):
                tmplist.append(p)
            allvers[tag] = tmplist

        import json
        print json.dumps(allvers, indent=2)


if __name__=='__main__':
    sUsage = """%prog [-r] project version tag
    Sets the flag indicating which buildtool is used for the build"""
    s = LbSdbGenerateVersionJSON(usage=sUsage)
    sys.exit(s.run())


