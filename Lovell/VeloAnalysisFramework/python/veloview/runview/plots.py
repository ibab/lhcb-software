"""Methods dealing with run view plots."""
import glob

import ROOT

from veloview.config import Config
from veloview.config.run_view import run_view_pages
from veloview.core import io
from veloview.giantrootfile.gui_tree import Tree
from veloview.runview import utils
from veloview.runview.response_formatters import dictionary_formatter

def get_run_plot(name, run, reference=False, formatter=dictionary_formatter, refRun = None):
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
    if reference and refRun==None:
        try:
            run = utils.reference_run(name, run)
        except ValueError:
            return None

    else: run = refRun

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

    # Normalise histogram if required
    plot_dict = get_plot_dictionary(name)
    if plot_dict is not None and plot_dict.get('normalised', False):
      integral = clone.Integral()
      if integral > 0:
        clone.Scale(1.0/integral)

    return formatter(clone)


def get_run_plot_with_reference(name, run, formatter=dictionary_formatter, refRun == None):
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
                                 formatter=formatter, refRun = refRun)
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


def get_trending_plot(name, runRange, binsY = 10, minY = 0., maxY = 10., formatter = dictionary_formatter):
    """
    Get a trending plot, showing a certain variable plotted against run number.
    """
    f = ROOT.TFile(Config().grf_file_path, 'READ')
    t = Tree(Config().grf_tree_name)
    h = t.Plot( 
            ( # functions projecting out coordinates to plot
                lambda t: t.runnr,
                lambda t: getattr(t, name).value()
                ),
            ( # cut(s) to apply
                lambda t: t.runnr in runRange,
                ),
            # no weights, do not draw into existing histogram
            None, None,
            # histogram constructor arguments
            (name, 'run number versus {0};run number;{0}'.format(name),
                len(runRange), min(runRange) - .5, max(runRange) + .5, binsY, minY, maxY)
            )
    formatted = formatter(h)

    del h
    del t
    f.Close()
    del f

    return formatted

