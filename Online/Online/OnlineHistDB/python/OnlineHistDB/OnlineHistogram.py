from collections import namedtuple

import ROOT

ROOT.gSystem.Load("libOnlineHistDB.so")
ROOT.gInterpreter.Declare('#include "OnlineHistDB/OnlineHistoOnPage.h"')

# ROOT "typedefs"
StdString = ROOT.std.string
VectorOfStrings = ROOT.std.vector('string')
VectorOfInts = ROOT.std.vector('int')
VectorOfFloats = ROOT.std.vector('float')
MapOfStringToInt = ROOT.std.map('string,int')
MapOfStringToFloat = ROOT.std.map('string,int')
MapOfStringToString = ROOT.std.map('string,int')
VectorOfOnlineHistoOnPages = ROOT.std.vector('OnlineHistoOnPage*')

# Tuples for pad coordinates
_ordinate = namedtuple('Ordinate', ['min', 'max'])
_coordinate = namedtuple('Coordinate', ['x', 'y'])
_pad_dimensions = namedtuple('PadDimensions',
                             ['top', 'left', 'width', 'height'])


class OnlineHistogram(object):
    """Representation of a row in the HISTOGRAM table in the HistDB.

    Also includes information from the SHOWHISTOGRAM table, so is in some sense
    a JOIN of the two tables.
    Wraps the C++ OnlineHistogram class, providing a more Pythonic interface to
    its properties.
    """
    def __init__(self, cpp_obj, hdb):
        """Initialise an OnlineHistogram object from a C++ instance.

        Keyword arguments
        cpp_obj -- An instance of the C++ OnlineHistogram class
        hdb -- An instance of the Python OnlineHistogram class
        """
        self._obj = cpp_obj
        self._hdb = hdb
        self._display_options = None
        self._display_histogram = None

        # Fetch the details of the analysis, if there is one
        if self.is_analysis_histogram:
            # Name of the analysis algorithm
            algorithm = StdString()
            # Name of the histograms used as input to the algorithm
            sources = VectorOfStrings()
            # Algorithm parameter values
            param_values = VectorOfFloats()
            # This call fills its arguments with the information we want
            self._obj.getCreationDirections(algorithm, sources, param_values)
            self._analysis_algorithm = str(algorithm)
            # Resolve the sources, as identifiers, to OnlineHistogram objects
            self._analysis_source_histograms = map(
                self._hdb.histogram_for_id, sources
            )
            self._analysis_parameter_values = list(param_values)
            del algorithm, sources, param_values
        else:
            self._analysis_algorithm = None
            self._analysis_source_histograms = None
            self._analysis_parameter_values = None

    @property
    def name(self):
        """Histogram name, i.e. the last part of the identifier."""
        return str(self._obj.hname())

    @property
    def dimension(self):
        """Integer dimension of the histogram (0, 1, or 2)."""
        return int(self._obj.dimension())

    @property
    def identifier(self):
        """Unique identifier as `task/algorithm/name`."""
        return str(self._obj.identifier())

    @property
    def task(self):
        """Task that produces the histogram."""
        return str(self._obj.task())

    @property
    def algorithm(self):
        """Algorithm within the task that produces this histogram."""
        return str(self._obj.algorithm())

    @property
    def documentation(self):
        """Short docstring."""
        return str(self._obj.doc())

    @property
    def description(self):
        """More descriptive docstring than `documentation`."""
        return str(self._obj.desc())

    def _build_display_options(self):
        """Build dictionary of display options.

        The dictionary keys are identical to those in the DB, except lowercase.
        A visible histogram, as shown on a presenter page, may have display
        options defined in several places. In order of increasing specificity:
          1. The set of the histogram that the visible histogram references
          2. The visible histogram itself
        In other words: the set that a visible histogram belongs to, through
        the association with a histogram, may define display options. These are
        overridden by options defined on the visible histogram itself.
        """
        if self._display_options is not None:
            return
        int_opts = MapOfStringToInt()
        flt_opts = MapOfStringToFloat()
        str_opts = MapOfStringToString()
        self._obj.getIntDisplayOptions(int_opts)
        self._obj.getFloatDisplayOptions(flt_opts)
        self._obj.getStringDisplayOptions(str_opts)
        opts = {}
        # This assumes all unique option keys, else information will be lost
        for k, v in int_opts:
            opts[str(k).lower()] = int(v)
        for k, v in flt_opts:
            opts[str(k).lower()] = float(v)
        for k, v in str_opts:
            opts[str(k).lower()] = str(v)
        self._display_options = opts

    @property
    def display_options(self):
        """Return dictionary of display options."""
        self._build_display_options()
        return self._display_options

    def _load_display_histogram(self):
        """Load the OnlineHistoOnPage object."""
        if self._display_histogram is not None:
            return
        page = self._hdb.page(self.page)
        vishists = VectorOfOnlineHistoOnPages()
        page.getHistogramList(vishists)
        for vhist in vishists:
            if vhist.histo == self._obj and vhist.instance == self.instance:
                break
        self._display_histogram = vhist

    @property
    def display_histogram(self):
        self._load_display_histogram()
        return self._display_histogram

    @property
    def pad_dimensions(self):
        """Return a PadDimension tuple for this histogram."""
        dh = self.display_histogram
        # Could return this Coordinate tuple, but might be less useful
        coord = _coordinate(
            _ordinate(dh.xmin, dh.xmax), _ordinate(dh.ymin, dh.ymax)
        )
        top = coord.y.max
        left = coord.x.min
        width = coord.x.max - coord.x.min
        height = coord.y.max - coord.y.min
        return _pad_dimensions(top, left, width, height)

    @property
    def pad_id(self):
        """Return the ID of the pad this histogram should be drawn on.

        Some histograms should be drawn on top of others, in which case this
        method returns the ID of the 'parent' histogram. If there is no parent,
        the ID of this histogram is returned.
        We create a pad ID as `identifier/instance`.
        """
        return '{0.identifier}/{0.instance}'.format(self)

    @property
    def parent_pad_id(self):
        overlap = self.display_histogram.getOverlap()
        if overlap:
            parent = OnlineHistogram(overlap.histo, self._hdb)
        else:
            return None
        return parent.pad_id

    @property
    def instance(self):
        """Return the instance index of histogram on a single page.

        A single histogram can be displayed multiple times on any given page.
        The instance integer makes each histogram unique on each page.
        """
        return self._obj.instance()

    @property
    def page(self):
        """Page this histogram is displayed on."""
        return str(self._obj.page())

    @property
    def reference_page(self):
        """Page that contains reference histograms for this histogram.

        GUIs can choose to link to the reference page.
        """
        return str(self._obj.page2display())

    @property
    def set_id(self):
        """ID of the set this histogram belongs to."""
        return int(self._obj.hsid())

    @property
    def set_index(self):
        """Index of this histogram with its set."""
        return int(self._obj.ihs())

    @property
    def is_analysis_histogram(self):
        """Is this histogram produced by an analysis task?"""
        return bool(self._obj.isAnaHist())

    @property
    def analysis_algorithm(self):
        """Name of the analysis algorithm that produces this histogram."""
        return self._analysis_algorithm

    @property
    def analysis_source_histograms(self):
        """OnlineHistogram objects that our analysis depends on."""
        return self._analysis_source_histograms

    @property
    def analysis_parameter_values(self):
        """Parameter values used in the analysis."""
        return self._analysis_parameter_values
