"""Methods dealing with run view plots."""
import glob

import ROOT

from veloview.runview import utils
from veloview.runview.response_formatters import dictionary_formatter


# TODO factor out returning nominal+reference to a different method
def get_run_plot(name, run, reference=False, formatter=dictionary_formatter):
    """Return the formatted object at the plot path in the run file.

    If reference is True, a list of two plots will be returned, the first being
    the plot for `run`, and the second being the plot for the reference run for
    `run`.
    Keyword arguments:
    name -- Path within the run file to the plot object.
            A KeyError is raised if name is not found in the run file
    run -- Run number
    reference -- If True, include the reference plot for the given plot and run
    formatter -- Response formatter used to format the TObject
    """
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

    nomplot = formatter(clone)
    refrun = None
    if reference:
        try:
            refrun = utils.reference_run(name, run)
        except ValueError:
            # TODO what to do on invalid reference resolution?
            pass
    if refrun:
        refplot = get_run_plot(name, refrun, reference=False)
        # nomplot['data']['title'] += ' (nominal run {0})'.format(run)
        # refplot['data']['title'] += ' (reference run {0})'.format(refrun)
        return [nomplot, refplot]
    else:
        return nomplot
