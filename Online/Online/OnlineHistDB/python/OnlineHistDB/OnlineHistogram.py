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
MapOfStringToFloat = ROOT.std.map('string,float')
MapOfStringToString = ROOT.std.map('string,string')
VectorOfOnlineHistoOnPages = ROOT.std.vector('OnlineHistoOnPage*')

# Tuples for pad coordinates
_ordinate = namedtuple('Ordinate', ['min', 'max'])
_coordinate = namedtuple('Coordinate', ['x', 'y'])
_pad_dimensions = namedtuple('PadDimensions',
                             ['top', 'left', 'width', 'height'])


# This class isn't in its own file because I (AP) don't think there's a reason
# for it to be instantiated by anything other than OnlineHistogram
class VisibleHistogram(object):
    """Representation of a DRAWHISTOGRAM row.

    Wrapper around the OnlineHistoOnPage C++ class.
    """
    def __init__(self, cpp_obj, histogram):
        """Initialise a VisibleHistogram object.

        Keyword arguments:
        cpp_obj -- Instance of OnlineHistoOnPage C++ class
        histogram -- Python OnlineHistogram object that this object relates to
        """
        self._obj = cpp_obj
        self.histogram = histogram
        self._display_options = None

    @property
    def pad_dimensions(self):
        """Return pad dimensions as a Coordinate of (x, y) Ordinates."""
        o = self._obj
        return _coordinate(
            _ordinate(o.xmin, o.xmax), _ordinate(o.ymin, o.ymax)
        )

    @property
    def pad_id(self):
        """Return the ID of the pad this histogram should be drawn on.

        Some histograms should be drawn on top of others, in which case this
        method returns the ID of the 'parent' histogram. If there is no parent,
        the ID of this histogram is returned.
        We create a pad ID as `identifier/instance`.
        """
        return '{0.identifier}/{1.instance}'.format(self.histogram, self)

    @property
    def parent(self):
        """Return the parent VisibleHistogram, or None if it doesn't exist.

        This is the VisibleHistogram on to which this object should be
        drawn/overlaid.
        """
        overlap = self._obj.getOverlap()
        # Can't test for None as a NULL returned by getOverlap is casted to a
        # <ROOT.OnlineHistoOnPage object at 0x(nil)>, but can test this for
        # truthiness
        if overlap:
            # histo is a member of OnlineHistoOnPage pointing to the
            # OnlineHistogram
            overlap_histogram = OnlineHistogram(
                overlap.histo, self.histogram._hdb
            )
            parent_vh = VisibleHistogram(overlap, overlap_histogram)
        else:
            parent_vh = None
        return parent_vh

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

        The OnlineHistogram C++ class retrieves the options via a OCI
        statement, such that this override behaviour is transparent to the
        user.
        """
        if self._display_options is not None:
            return
        int_opts = MapOfStringToInt()
        flt_opts = MapOfStringToFloat()
        str_opts = MapOfStringToString()
        self.histogram._obj.getIntDisplayOptions(int_opts)
        self.histogram._obj.getFloatDisplayOptions(flt_opts)
        self.histogram._obj.getStringDisplayOptions(str_opts)
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

    @property
    def page(self):
        """Page, as a string, that this histogram is displayed on."""
        return str(self.histogram._obj.page())

    @property
    def instance(self):
        """Return the instance index of histogram on a single page.

        A single histogram can be displayed multiple times on any given page.
        The instance integer makes each histogram unique on each page.
        """
        return int(self.histogram._obj.instance())

    @property
    def reference_page(self):
        """Page that contains reference histograms for this histogram.

        GUIs can choose to link to the reference page.
        """
        return str(self.histogram._obj.page2display())


class OnlineHistogram(object):
    """Representation of a row in the HISTOGRAM table in the HistDB.

    Here's what I think the schema of the histogram database means: rows of the
    HISTOGRAMSET table represent groups of histograms, each of which are
    described by a row in the HISTOGRAM table.
    A histogram can be placed on a page, represented by rows in the PAGE table,
    and one histogram can be placed on many pages multiple times.

    An instance of a histogram on a page is represented by a row in the
    SHOWHISTO table. We call these 'visible histograms'. A visible histogram
    can specify its own set of display options, a row in the DISPLAYOPTIONS
    table, as can a histogram set.
    A visible histogram's display options override those of the set, which each
    visible histogram has by association through a histogram.

    The OnlineHistogram C++ class encapsulates the information contained in
    the histogram and the visible histogram rows.
    This class, along with the VisibleHistogram class, tries to disentagle this
    two, representing each database row with an object.
    """
    def __init__(self, cpp_obj, hdb):
        """Initialise an OnlineHistogram object from a C++ instance.

        Keyword arguments
        cpp_obj -- An instance of the C++ OnlineHistogram class
        hdb -- An instance of the Python OnlineHistDB class
        """
        self._obj = cpp_obj
        self._hdb = hdb
        self._visible_histogram = None

        # Fetch the details of the analysis, if there is one
        if self.is_analysis_histogram:
            # Name of the analysis algorithm
            algorithm = StdString()
            # Names of the histograms used as input to the algorithm
            sources = VectorOfStrings()
            # Algorithm parameter values
            param_values = VectorOfFloats()
            # This call fills its arguments with the information we want
            self._obj.getCreationDirections(algorithm, sources, param_values)
            self._analysis_algorithm = str(algorithm)
            # Resolve the sources, as identifiers, to OnlineHistogram objects
            # These objects won't be associated to a page, i.e. they won't have
            # an associated a VisibleHistogram
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
        return str(self._obj.descr())

    def _load_visible_histogram(self):
        """Load the VisibleHistogram object associated with this instance."""
        if self._visible_histogram is not None:
            return
        page = self._hdb.page(self._obj.page())
        vishists = VectorOfOnlineHistoOnPages()
        page.getHistogramList(vishists)
        target_obj = self._obj
        target_instance = target_obj.instance()
        # Find the OnlineHistoOnPage that matches the object this
        # OnlineHistogram represents
        for vhist in vishists:
            if vhist.histo == target_obj and vhist.instance == target_instance:
                break
        # A little hacky, but we're abusing the fact that `vhist` leaks outside
        # the scope of the for loop
        self._visible_histogram = VisibleHistogram(vhist, self)

    @property
    def visible_histogram(self):
        """Return the VisibleHistogram associated with this instance.

        If this object isn't associated to a particular page, then this method
        will return None.
        """
        self._load_visible_histogram()
        return self._visible_histogram

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
