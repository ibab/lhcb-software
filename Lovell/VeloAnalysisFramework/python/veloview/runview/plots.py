"""Methods dealing with run view plots."""
import glob

import ROOT

from veloview.runview import utils
from veloview.runview.response_formatters import dictionary_formatter


def get_run_plot(name, run, reference=False, formatter=dictionary_formatter):
    """Return the formatted object at the plot path in the run file.

    Keyword arguments:
    name -- Path within the run file to the plot object.
            A KeyError is raised if name is not found in the run file
    run -- Run number
    reference -- If True, fetch the reference plot for the given plot and run
    formatter -- Response formatter used to format the TObject
    """
    if reference:
        run = utils.reference_run(name, run)

    # Get the latest run file in the run's directory
    base = utils.run_file_path(run)
    files = sorted(glob.glob("{0}/*.root".format(base)))
    try:
        path = files[-1]
    except IndexError:
        raise IOError("Run file not found for run {0}".format(run))

    # Try to open the file
    f = ROOT.TFile(path)
    if f.IsZombie():
        raise IOError("Run file not found for run {0}".format(run))

    # Retrieve the object
    obj = f.Get(name)
    if not obj:
        raise KeyError("Plot {0} not found in run file {1}".format(name, run))
    # The file will be closed when the function returns, so we need to clone
    # the fetched object outside the file's scope
    ROOT.gROOT.cd()
    clone = obj.Clone(obj.GetName())
    f.Close()

    return formatter(clone)
