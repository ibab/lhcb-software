#!/usr/bin/env python
from LbUtils.Script import Script
from LbConfiguration.Project import project_list
import sys, os
import logging

class CreateBatAliasesScript(Script):
    def defineOpts(self):
        parser = self.parser
        parser.set_defaults(outputdir=None)
        parser.add_option("-o", "--output-dir", 
                          action="store", 
                          metavar="DIR",
                          type = "string",
                          dest = "outputdir",
                          help="output directory for the generated alias scripts")

    def main(self):
        log = logging.getLogger()
        opts = self.options
        
        if not opts.outputdir:
            log.error("No output directory defined -- exiting")
            return 1
        for p in project_list :
            alist = p.Aliases()
            for a in alist.keys():
                aliascmd = alist[a].split()
                cmd = aliascmd[0]
                args = " ".join(aliascmd[1:])
                filename = os.path.join(opts.outputdir, "%s.bat" % a)
                log.info("Writing %s : call %%~d0%%~p0\%s.bat %s" % (filename, cmd , args))
                f = open(filename, "w")
                f.write(r" call %%~d0%%~p0\%s.bat %s\n\n" % ( cmd , args))
                f.close()
        return 0

if __name__ == '__main__':
    sys.exit(CreateBatAliasesScript(usage="%prog [options]").run())