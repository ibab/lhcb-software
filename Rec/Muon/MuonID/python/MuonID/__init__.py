"""
"""
import os
print "importing from ", os.path.abspath(__path__[0])

from GaudiPython.Bindings import gbl
IMuonIDTool = gbl.IMuonIDTool
