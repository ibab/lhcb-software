#!/usr/bin/env python
# =============================================================================
# @file CombineLambdaMaps.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-08-19
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"

from optparse import OptionParser
import os
import sys

from KaliCalo.Kali.LambdaMap import LambdaMap

def combineLambdaMaps(*lMaps):
  newLambdaMap = LambdaMap()
  for lMap in lMaps:
    for key in lMap:
      if not newLambdaMap.has_key(key):
        newLambdaMap._lambdas[key] = []
      newLambdaMap._lambdas[key].extend(lMap[key])
  return newLambdaMap

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option("-o", "--outputFile",
                    dest="outputFile", action="store", type="string", default=None,
                    help="join specified constants in FILE", metavar="FILE")
  (options, args) = parser.parse_args()
  if not args:
    (options, args) = parser.parse_args(sys.stdin.readline().split())
  newL = combineLambdaMaps([arg for arg in args if os.path.exists(arg)])
  if options.outputFile:
    newL.save(options.outputFile)

# EOF
