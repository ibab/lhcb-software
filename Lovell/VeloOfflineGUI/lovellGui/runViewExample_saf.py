"""GUI run view configuration.

The ordered dictionary run_view_pages defined in this file is used by both GUIs
to discover what pages to create, what plots to show on each page, and how to
display them.

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
        # Display plots in a 2x3 (horizontal x vertical) grid
        layout: (2, 3)
    }

For each plot dictionary, the following keys can be defined:

    title: Title to display (required, string)
    name: Path within the Vetra output file to the plot If sensor_dependent is
          True, should contain a format placeholder defining how the sensor
          number is formatted (required, string)
    sensor_dependent: If True, there is one plot per VELO sensor, and the
                      `name` key defines at least one formatting placeholder
                      for the sensor number (boolean)
    short: A shorter title to display, useful if `title` is very long and might
           look unwieldy in a GUI tab (string)
    normalise: Normalise the plot to have unit area (boolean)
    tip: Description of the plot, including expected observerations, known 
         features and relevant contact details.
    options: Dictionary of options defining purely cosmetic attributes
             (dictionary)

The format placeholder that must be present in `name` if `sensor_dependent` is
True must follow the specification outlined in the Python format
mini-language [1]. In essence, a format placeholder is a pair of braces `{}`
containing an integer followed by a optional colon and formatting defintions.
The integer represents the index of the list of arguments passed to the
call to `string.format`, and the formatting defintions specify how the value
of that argument will be formatted in to a string. In the example above,
`format` will be called on the value of `name`, being passed the sensor number
as the first argument.

    >>> 'folder/sensor_{0:03d}/plot_name_A'.format(5)
    'folder/sensor_005/plot_name_A'

No key in the `options` dictionary is required, and valid keys are:

    showUncertainties: Should the GUI display the uncertainties defined in the
                       plots (boolean)
    yAxisMinimum: Fixed y-axis minimum to display (Real)
    yAxisMaximum: Fixed y-axis maximum to display (Real)
    yAxisZeroSuppressed: Set the y-axis range to be centered around the minimum
                         and maximum histogram values (boolean)
    asPoints: Display the histogram as a set of points centered within each
              bin, rather than as a continuous line (boolean)

If there is a cosmetic change you would like to be adjustable for each plot
that is not already available, please contact the authors of the GUIs.

[1]: https://docs.python.org/2/library/string.html#formatspec
"""
from collections import OrderedDict

run_view_pages = OrderedDict([
    ('muonCalib', {'title': 'Muon Calibration', 'layout': (2, 2), 'plots': [
                {'title': 'dEdx',
                'name': 'SafMuonCalibration/dEdx',
                'tip': 'Test of tipping - Hello world!',
                'style': 0},

                {'title': 'Energy Vs Pathlength',
                'name': 'SafCubePlots/hitsPerCube_plane1'},

                {'title': 'eVsPathLength',
                'name': 'SafCubePlots/hitsPerCube_plane1',
                'zrange': [-10, 60]},

                {'title': 'Energies',
                'plottables' : [
                    {'name': 'SafPeakPlots/TriggerIntegral',
                    'style': 1}, 
                    {'name': 'SafPeakPlots/TriggerIntegralCubes',
                    'style': 2}, 
                    {'name': 'SafPeakPlots/TriggerIntegralTracks',
                    'style': 3,
                    'color' : 'g'}
                    ],
                 'xrange': [0, 30000],
                 'yrange': [0, 15000]
                  }
                ]}),
    ('dqs', {'title': 'DQS'})
])
