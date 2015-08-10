"""Methods dealing with run view plots."""
import glob

import ROOT

from veloview.config.run_view import run_view_pages
from veloview.core import io
from veloview.runview import utils
from veloview.runview.response_formatters import dictionary_formatter

def get_run_plot(name, run, reference=False, formatter=dictionary_formatter):
    """Return the formatted object at the plot path in the run file.

    If reference is True, the corresponding plot from the reference file will
    be returned, else the plot from the `run` run will be returned.
    If no reference run can be found for `run`, None is returned.
    Keyword arguments:
    name -- Path within the run file to the plot object.
            A KeyError is raised if name is not found in the run file
    run -- Run number
    reference -- If True, include the reference plot for the given plot and run
    formatter -- Response formatter used to format the TObject
    """
    if reference:
        try:
            run = utils.reference_run(name, run)
        except ValueError:
            return None

    # Get the latest run file in the run's directory
    base = utils.run_file_path(run)
    files = sorted(glob.glob("{0}/*.root".format(base)))
    try:
        path = files[-1]
    except IndexError:
        raise IOError("Run file not found for run {0}".format(run))

    # Try to open the file
    f = ROOT.TFile(path)
    #print "Dan!", path
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

    # Normalise histogram if required
    plot_dict = get_plot_dictionary(name)
    if plot_dict.get('normalised', False):
      integral = clone.Integral()
      if integral > 0:
        clone.Scale(1.0/integral)

    return formatter(clone)


def get_run_plot_with_reference(name, run, formatter=dictionary_formatter):
    """Return the formatted nominal and reference plots.

    A 2-tuple of two plots is returned:
        1. The nominal plot, as returned by
            get_run_plot(name, run, reference=False, formatter)
        2. The reference plot, as returned by
            get_run_plot(name, run, reference=True, formatter)
    in that order.

    If the reference get_run_plot call returns None, None is returned in place
    of the reference object.
    """
    nominal = get_run_plot(name, run, reference=False, formatter=formatter)
    try:
        reference = get_run_plot(name, run, reference=True,
                                 formatter=formatter)
    except KeyError:
        reference = None
    return nominal, reference


def get_plot_dictionary(name):
  for page in run_view_pages.itervalues():
    if 'plots' not in page:
        continue
    for plot in page['plots']:
      if plot['name'] == name:
        return plot


def get_trending_plot(name, runRange, formatter = dictionary_formatter):
    """
    Get a trending plot, showing a certain variable plotted against run number.
    """
    hist = ROOT.TH1F(name, name, len(runRange), runRange[0], runRange[-1])
    for runNr in runRange:
        base = utils.run_file_path(runNr)
        files = sorted(glob.glob("{0}/analysis.root".format(base)))
        if files:
            f = io.GRFIO(files[-1])
            if f:
                runValues = f.read([name])
                hist.SetBinContent(runNr, runValues[0])
                f.Close()

    return formatter(hist)
