#!/usr/bin/env python

#---------------------------------------------------------------------
# Hack to work around bug #39117
#   (gaudirun.py --old-opts does not handle ToolHandles correctly)
import GaudiKernel.GaudiHandles
GaudiKernel.GaudiHandles.GaudiHandle.getFullName = GaudiKernel.GaudiHandles.GaudiHandle.toStringProperty

#---------------------------------------------------------------------
if __name__ == "__main__":
    import sys
    from Gaudi import gaudimain
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] <opts_file> ...")
    parser.add_option("-n","--dry-run", action="store_true",
                      help="do not run the application, just parse option files")
    parser.add_option("-p","--pickle-output", action="store", type="string",
                      metavar = "FILE",
                      help="write the parsed options as a pickle file (static option file)")
    parser.add_option("-v","--verbose", action="store_true",
                      help="print the parsed options")
    parser.add_option("--old-opts", action="store_true",
                      help="format printed options in old option files style")
    parser.add_option("--all-opts", action="store_true",
                      help="print all the option (even if equal to default)")
    parser.add_option("--option", action="append", type="string",
                      help="append the provided (Python) option to the configuration." +
                           "All options lines are executed, one after the other, in " +
                           "the same context.")
    
    opts, args = parser.parse_args() 
    
    # Hack to work around bug #39116
    #   (gaudirun.py --old-opts does not trap python style options printout)
    # Part 1: trap standard output
    if opts.old_opts:
        from StringIO import StringIO
        sys.stdout = StringIO()
    # Hack/Part 1/end
    
    c = gaudimain()
    c.configure(args)
    
    if opts.option:
        g = {}
        l = {}
        exec "from Gaudi.Configuration import *" in g, l
        for o in opts.option:
            exec o in g, l
    
    if opts.verbose:
        c.dumpconfig(opts.old_opts,opts.all_opts)
    if opts.pickle_output:
        c.writepickle(opts.pickle_output)
        
    # Hack to work around bug #39116
    #   (gaudirun.py --old-opts does not trap python style options printout)
    # Part 2: print standard output replacing '^#' with '//'
    if opts.old_opts:
        out = sys.stdout
        sys.stdout = sys.__stdout__
        out.seek(0)
        import re
        exp = re.compile("^#")
        for l in out:
            sys.stdout.write(exp.sub("//",l))
        sys.stdout.flush()
    # Hack/Part 2/end
    if not opts.dry_run:
        c.run()
