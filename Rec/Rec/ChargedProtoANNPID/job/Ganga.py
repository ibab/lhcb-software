
j = Job( application = DaVinci( version = 'v29r2' ) )

j.name = 'MC-ANNPID'

# Main options
j.application.optsfile = [ File('options.py') ]

j.splitter = DiracSplitter ( filesPerJob = 75, maxFiles = 999999 )

rootfiles = [ 'ProtoPIDANN.tuples.root' ]

j.outputsandbox = rootfiles 

j.merger = SmartMerger( files        = rootfiles,
                        ignorefailed = True,
                        overwrite    = True )

j.do_auto_resubmit = True

j.backend = Dirac()

print "Submitting job", j.name
j.submit()
