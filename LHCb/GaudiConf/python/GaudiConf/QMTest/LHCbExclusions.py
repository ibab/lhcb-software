from GaudiTest import normalizeExamples, RegexpReplacer, LineSkipper

preprocessor = normalizeExamples + \
    RegexpReplacer(when = "/afs/cern.ch/lhcb/software/DEV/nightlies",
        orig = r'/DEV/nightlies',
        repl = r'/releases') + \
    RegexpReplacer(when = "/DBASE/TCK/HltTCK/",
        orig = r'/v[0-9]+r[0-9]+p?[0-9]*/',
        repl = r'/vXrYpZ/') + \
    RegexpReplacer(when = "at 0x########L",
        orig = r'0x########L',
        repl = r'0x########') + \
    RegexpReplacer(when = "Connected to database",
        orig = r'ONLINE[_-][0-9 ]{1,6}(.*)"[^"]*/([0-9A-Z_]{1,8})"',
        repl = r'ONLINE_xxxxxx\1"\2"') + \
    RegexpReplacer(when = "Connected to database",
        orig = r'"[^"]*/([0-9A-Z_]{1,8})"',
        repl = r'"\1"') + \
    LineSkipper(["EventSelector        INFO Stream:EventSelector.DataStreamTool"]) + \
    LineSkipper(["/PARAM/ParamFiles/"]) + \
    LineSkipper(["INFO Using TAG", "TimingAuditor.T", "RootDBase.open"]) + \
    LineSkipper(["INFO Opened magnetic field file"]) + \
    LineSkipper(["INFO Connecting to database"]) + \
    LineSkipper(["INFO Disconnect from database after being idle"]) + \
    LineSkipper(["INFO Memory has changed from"]) + \
    LineSkipper(["INFO Memory change after pool release"]) + \
    LineSkipper(["Memory for the event exceeds 3*sigma"]) + \
    LineSkipper(["Mean 'delta-memory' exceeds 3*sigma"]) + \
    LineSkipper(["INFO  'CnvServices':"]) + \
    LineSkipper(["DEBUG Property ['Name': Value] =  'ErrorCount"]) + \
    LineSkipper(["DEBUG Property ['Name': Value] =  'RootOnTES':"]) + \
    LineSkipper(["#properties ="])
