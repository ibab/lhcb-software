"""GUI run view configuration.

The ordered dictionary run_view_pages defined in this file is used by both GUIs
to discover what pages to create, what plots to show on each page, and how to
display them.

Pages
-----

Each page is defined by a key in the top level of run_view_pages. The `title`
key must be defined as a string of the name to show for the page. An optional
`plots` key can define a list of dictionaries, each defining a plot within a
Vetra output file. The optional `layout` key defines the matrix dimensions with
which to display the plots in. An example page dictionary might look like this:

    {
        'title': 'Foo Things',
        'plots': [
            {
                'title': 'Plot A',
                'name': 'folder/sensor_{0:03d}/plot_name_A',
                'sensor_dependent': True
            },
            {
                'title': 'Plot B',
                'name': 'folder/subfolder/plot_name_B'
            },
        ],
        # Display plots in a 1x3 (horizontal x vertical) grid
        layout: (1, 3)
    }

Plot dictionaries
-----------------

For each plot dictionary, the following keys can be defined:

    title: Title to display (required, string)
    name: Path within the Vetra output file to the plot. If sensor_dependent is
          True, should contain a format placeholder defining how the sensor
          number is formatted (required, string)
    sensor_dependent: If True, there is one plot per VELO sensor, and the
                      `name` key defines at least one formatting placeholder
                      for the sensor number (boolean)
    short: A shorter title to display, useful if `title` is very long and might
           look unwieldy in a GUI tab (string)
    normalise: Normalise the plot to have unit area (boolean)
    tip: Description of the plot, including expected observations, known
         features and relevant contact details (string)
    plottables: List of plot dictionaries, see "Multiple plots" below (list)
    options: Dictionary of options defining purely cosmetic attributes
             (dictionary)

Sensor-dependent plots
----------------------

The format placeholder, that must be present in `name` if `sensor_dependent` is
True, must follow the specification outlined in the Python format
mini-language [1]. In essence, a format placeholder is a pair of braces `{}`
containing an integer followed by a optional colon and formatting definitions.
The integer represents the index of the list of arguments passed to the
call to `string.format`, and the formatting definitions specify how the value
of that argument will be formatted in to a string. In the example above,
`format` will be called on the value of `name`, being passed the sensor number
as the first argument.

    >>> 'folder/sensor_{0:03d}/plot_name_A'.format(5)
    'folder/sensor_005/plot_name_A'

[1]: https://docs.python.org/2/library/string.html#formatspec

Plot options
------------

No key in the `options` dictionary is required, and valid keys are:

    uncertainties: Should the GUI display the uncertainties defined in the
                   plots (boolean)
    yAxisMinimum: Fixed y-axis minimum to display (Real)
    yAxisMaximum: Fixed y-axis maximum to display (Real)
    yAxisZeroSuppressed: Set the y-axis range to be centered around the minimum
                         and maximum histogram values (boolean)
    zAxisMinimum: Fixed z-axis minimum to display (Real)
    zAxisMaximum: Fixed z-axis maximum to display (Real)
    asPoints: Display the histogram as a set of points centered within each
              bin, rather than as a continuous line (boolean)
    legend: Display a legend (boolean)
    asText: Display 2D histogram bin contents as text (boolean)
    color: Colour of the 1D plot object (string)
    marker: Matplotlib marker style (string)

If there is a cosmetic change you would like to be adjustable for each plot
that is not already available, please contact the authors of the GUIs.

Multiple plots
--------------

If one wishes to take multiple objects from the Vetra output file and display
them as a single plot, a list of plot dictionaries can be specified in the
`plottables` key of the plot dictionary. For example:
    {
        'title': 'Foo Things',
        'plots': [
            {
                'title': 'Plot A',
                'name': 'folder/sensor_{0:03d}/plot_name_A',
                'plottables': [
                    {
                        'title': 'First plot',
                        'name': 'folder/subfolder/plot_a'
                    },
                    {
                        'title': 'Second plot',
                        'name': 'folder/subfolder/plot_b'
                    }
                ],
                'options': {
                    'legend': True
                }
            }
        ]
    }

Each dictionary in `plottables` has the same structure as a 'top-level' plot
dictionary. Here the `legend` key of the top-level plot has been set, within
which the `title` values of the sub-plots will be used.

Structure of this file
----------------------

Each page should be defined as a 2-tuple of `(string, dict)`, with `dict`
having the format of a page dictionary, defined above.
This two-tuple should be set to a variable in `UPPERCASE`, and then this
variable name should be inserted in to the `run_view_pages` container near the
bottom of this file.

Please read through a few of the existing page definitions before modifying
this file, to get a feel for the formatting style.
"""
from collections import OrderedDict

OVERVIEW = (
    'overview', {
        'title': 'Overview',
    }
)

TRENDS = (
    'trends', {
        'title': 'Trends',
        'plots': [
            {
                'title': 'Trend view',
                'name': 'Trends',
                'trending': True
            }
        ]
    }
)

DETAILEDTRENDS = (
    'detailedtrends', {
        'title': 'Detailed trends',
        'plots': [
            {
                'title': '2d trend view',
                'name': '2d trends',
                'trending': True
            }
        ]
    }
)

# Define the ordered list of pages
# Pages will appear in the GUIs in this order
velo_view_pages = OrderedDict([
    OVERVIEW,
    TRENDS,
    DETAILEDTRENDS
])

DEFAULT_PARAMETER_VALUES = {
    # Is the plot name dependent on the sensor number?
    'sensor_dependent': False,
    # Short name for displaying in GUI tabs
    'short': None,
    # Normalise the data to unit area
    'normalise': False,
    # Help text
    'tip': None
}

DEFAULT_DRAW_OPTIONS = {
    # Display 2D histogram bin contents as text rather than as colour
    'asText': False,
    # Colour of the 1D plot object
    # TODO should this be an actual colour?
    'color': None,
    # Marker style
    # TODO this is matplotlib-specific
    'marker': '^',
    # Show a legend? The value of the 'title' key is used as the label
    'legend': False,
    # Shift the sensor number
    # TODO(AP) I have no idea what this means, why would you want to shift?
    'sensorShift': 0,
    # Offline GUI specific style flag
    # TODO change this to something meaningful for both GUIs
    'style': 0,
    # Show uncertainties
    'uncertainties': False,
    # Only show the extent of the data if True, else fix the lower bound of the
    # y-axis to zero
    'yAxisZeroSuppressed': True
}
