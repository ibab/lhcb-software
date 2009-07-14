from GaudiTest import normalizeExamples, RegexpReplacer, LineSkipper

preprocessor = normalizeExamples + \
    RegexpReplacer(when = "Connected to database",
        orig = r"(\"sqlite_file:)(.*/)([\w.]+/[0-9A-Z_]{1,8}\")",
        repl = r"\1\3") + \
    RegexpReplacer(when = "Connected to database",
        orig = "ONLINE[_-][0-9]{1,6}",
        repl = "ONLINE_xxxxxx") + \
    LineSkipper(["/PARAM/ParamFiles/"]) + \
    LineSkipper(["INFO Using TAG", "TimingAuditor.T", "RootDBase.open"]) + \
    LineSkipper(["INFO Opened magnetic field file"]) + \
    LineSkipper(["INFO Connecting to database"]) + \
    LineSkipper(["INFO Disconnect from database after being idle"])
