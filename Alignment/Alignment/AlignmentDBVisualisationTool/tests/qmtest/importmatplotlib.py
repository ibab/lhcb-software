#!/usr/bin/env python
try:
    import matplotlib as mpl
    mpl.rcParams["backend"] = "PDF"
    import matplotlib.pyplot as plt
    import matplotlib.dates
    import matplotlib.ticker
    from matplotlib.figure import SubplotParams
    print "Successfully imported the required names from matplotlib"
except ImportError:
    print "Could not import all names from matplotlib"
