
# Test for custom allocators.

# The below failed when the placement operators where removed
# but the custom pool new/delete operators remained...

import GaudiPython

x = GaudiPython.gbl.LHCb.LHCbID(268962525)
y = x.veloID()

print x
print y
