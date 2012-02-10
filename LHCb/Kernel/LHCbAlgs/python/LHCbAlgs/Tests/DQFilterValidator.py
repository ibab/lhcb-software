"""
Implementation of the validation code for all the DQFilter tests.
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

import re, string
from pprint import pformat

from GaudiTest import cmpTreesDicts, getCmpFailingValues


def count(keys, lines):
    """Helper function to count how many times a substring is found in a list of strings.
    """
    result = dict(zip(keys, [0] * len(keys)))
    for l in lines:
        for k in keys:
            if k in l:
                result[k] += 1
                continue
    return result

def checkEff(name, expected, stdout, result, causes):
    eff = None
    name = name.strip('"')
    qname = '"%s"' % (name)
    for l in stdout.splitlines():
        if qname in l:
            eff = filter(None, map(string.strip, l.split("|")))
            eff = tuple(map(int, eff[1:3]))
            break

    result["GaudiTest.%s.expected" % name] = result.Quote(pformat(expected))

    if eff != expected:
        causes.append(name)
        result["GaudiTest.%s.found" % name] = result.Quote(pformat(eff))

def checkCounts(expected, stdout, result, causes):
    ## Check DQFilter messages
    dqf = re.compile(r"^(DQFilter|ToolSvc\.DQAccep)")
    lines = [ l.rstrip() for l in stdout.splitlines() if dqf.match(l) ]

    counts = count(expected.keys(), lines)

    result["GaudiTest.DQFilterOut.expected"] = result.Quote(pformat(expected))

    failed = cmpTreesDicts(expected, counts)
    if failed:
        causes.append("DQFilter output")
        result["GaudiTest.DQFilterOut.found"] = result.Quote(pformat(counts))
        msg = "%s: %s != %s" % getCmpFailingValues(expected, counts, failed)
        result["GaudiTest.DQFilterOut.failure_on"] = result.Quote(msg)

