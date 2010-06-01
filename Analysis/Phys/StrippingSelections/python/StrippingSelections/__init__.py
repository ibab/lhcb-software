# Temporary hack to force correct loading of Gaudi:: C++ namespace functions.
from GaudiPython.Bindings import gbl as cpp
cpp.Gaudi
