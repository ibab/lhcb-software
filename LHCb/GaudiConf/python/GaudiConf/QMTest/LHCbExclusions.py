from GaudiTest import normalizeExamples, RegexpReplacer, LineSkipper

preprocessor = (
    normalizeExamples +
    RegexpReplacer(when = "/afs/cern.ch/lhcb/software/DEV/nightlies",
        orig = r'/afs/cern.ch/lhcb/software/DEV/nightlies',
        repl = r'') +
    RegexpReplacer(when = "/cvmfs/lhcb.cern.ch/lib/lhcb",
        orig = r'/cvmfs/lhcb.cern.ch/lib/lhcb',
        repl = r'') +
    RegexpReplacer(when = "/afs/cern.ch/lhcb/software/releases",
        orig = r'/afs/cern.ch/lhcb/software/releases',
        repl = r'') +
    RegexpReplacer(when = "/DBASE/TCK/HltTCK/",
        orig = r'/v[0-9]+r[0-9]+p?[0-9]*/',
        repl = r'/vXrYpZ/') +
    RegexpReplacer(when = "at 0x########L",
        orig = r'0x########L',
        repl = r'0x########') +
    RegexpReplacer(when = "Connected to database",
        orig = r'ONLINE[_-][0-9 ]{1,6}(.*)"[^"]*/([0-9A-Z_]{1,8})"',
        repl = r'ONLINE_xxxxxx\1"\2"') +
    RegexpReplacer(when = "Connected to database",
        orig = r'"[^"]*/([0-9A-Z_]{1,8})"',
        repl = r'"\1"') +
    LineSkipper(["EventSelector        INFO Stream:EventSelector.DataStreamTool",
                 "/PARAM/ParamFiles/",
                 "INFO Using TAG", "TimingAuditor.T", "RootDBase.open",
                 "INFO Opened magnetic field file",
                 "INFO Connecting to database",
                 "INFO Disconnect from database after being idle",
                 "INFO Memory has changed from",
                 "INFO Memory change after pool release",
                 "Memory for the event exceeds 3*sigma",
                 "Mean 'delta-memory' exceeds 3*sigma",
                 "INFO  'CnvServices':",
                 "DEBUG Property ['Name': Value] =  'ErrorCount",
                 "DEBUG Property ['Name': Value] =  'RootOnTES':",
                 "VERBOSE ServiceLocatorHelper::service: found service",
                 "DEBUG Property ['Name': Value] =  'NeededResources':[  ]",
                 "DEBUG Property ['Name': Value] =  'Cardinality':1",
                 "DEBUG Property ['Name': Value] =  'IsClonable':False",
                 "#properties =",
                ],
      regexps = [r"DEBUG Property \['Name': Value\] =  '(Timeline|(Extra|Data)(In|Out)puts)'",
                ])
   )
