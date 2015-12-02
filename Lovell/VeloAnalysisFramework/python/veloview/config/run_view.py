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

PEDESTALS = (
    'pedestals', {
        'title': 'Pedestals',
        'subpages': [
            {
                'title': '1D plots',
                'plots': [
                    {
                        'title': 'Pedestal bank',
                        'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Pedestal_Bank',
                        'sensor_dependent': True
                    },
                    {
                        'title': 'Subtracted ADC profile',
                        'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Ped_Sub_ADCs_Profile',
                        'sensor_dependent': True
                    }
                ],
                'layout': (1, 2)
            },
            {
                'title': '2D plots',
                'plots': [
                    {
                        'title': 'Subtracted ADC 2D',
                        'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Ped_Sub_ADCs_2D',
                        'sensor_dependent': True
                    }
                ]
            }
        ]
    }
)

NOISE = (
    'noise', {
        'title': 'Noise',
        'subpages': [
            {
                'title': 'VELO Overview',
                'plots': [
                    {
                        'title': 'RMS CMS noise vs. station (neg. for C-side)',
                        'options': {
                            'yAxisMinimum': -4.0,
                            'yAxisMaximum': 4.0,
                            'legend': True
                        },
                        'plottables': [
                            {
                                'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_R',
                                'title': 'R'
                            },
                            {
                                'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_Phi',
                                'title': 'Phi',
                                'options': {
                                    'color': 'y'
                                }
                            },
                            {
                                'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/Sensor_vs_Station_R',
                                'options': {
                                    'asText': 'True'
                                }
                            },
                            {
                                'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/Sensor_vs_Station_Phi',
                                'options': {
                                    'asText': 'True'
                                }
                            }
                        ]
                    }
                ]
            },
            {
                'title': 'Per Sensor (Channels)',
                'plots': [
                    {
                        'title': 'RMS noise vs. chip channel',
                        'name': 'Vetra/NoiseMon/DecodedADC/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
                        'sensor_dependent': True,
                        'options': {
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 5.0
                        }
                    },
                    {
                        'title': 'RMS CMS noise vs. chip channel',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
                        'sensor_dependent': True,
                        'options': {
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 5.0
                        }
                    },
                ],
                'layout': (1, 2)
            },
            {
                'title': 'Per Sensor (Strips)',
                'plots': [
                    {
                        'title': 'RMS noise vs. strip',
                        'name': 'Vetra/NoiseMon/DecodedADC/TELL1_{0:03d}/RMSNoise_vs_Strip',
                        'sensor_dependent': True,
                        'options': {
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 5.0
                        }
                    },
                    {
                        'title': 'RMS CMS noise vs. strip',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/TELL1_{0:03d}/RMSNoise_vs_Strip',
                        'sensor_dependent': True,
                        'options': {
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 5.0
                        }
                    },
                ],
                'layout': (1, 2)
            },
            {
                'title': 'Average Distribution',
                'plots': [
                    {
                        'title': 'RMS CMS noise (R sensors)',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_R',
                        'options': {
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 45.0
                        }
                    },
                    {
                        'title': 'RMS CMS noise (Phi sensors)',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_Phi',
                        'options': {
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 45.0
                        }
                    },
                ],
                'layout': (2, 1)
            },
            {
                'title': 'Per Link Map',
                'plots': [
                    {
                        'title': 'RMS CMS noise vs. link',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Link',
                        'options': {
                            'zAxisMinimum': 0,
                            'zAxisMaximum': 5
                        }
                    },
                    {
                        'title': 'RMS noise vs. link',
                        'name': 'Vetra/NoiseMon/DecodedADC/Average/RMSNoise_vs_Link',
                        'options': {
                            'zAxisMinimum': 0,
                            'zAxisMaximum': 5
                        }
                    },
                ],
                'layout': (1, 2)
            }
        ]
    }
)

HEADER_CROSS_TALK = (
    'headercrosstalk', {
        'title': 'Header cross-talk',
        'subpages': [
            {
                'title': 'Header cross-talk 1',
                'plots': [
                    {
                        'title': 'RMS Normalised Header noise for all sensors',
                        'name': 'Vetra/NoiseMon/DecodedADC/Header/HeaderNoise_allSensors',
                        'tip': (
                            'This plot shows for every link (64 entries per powered sensor) the noise measured '
                            'in the first channel of that link, which is affected by header cross talk, normalised '
                            'to the average noise of the link. The peak of the distribution should be to the left '
                            'of 2. There should only be a very small tail to the right of 2.'
                        ),
                        'axis_titles': ['normalised header noise', 'number of channels']
                    },
                    {
                        'title': 'RMS NoisyHeaderLinksPerSensor',
                        'name': 'Vetra/NoiseMon/DecodedADC/Header/NoisyHeaderLinksPerSensor',
                        'tip': (
                            'The distribution should stay below 20. Any sensors whereby their ' +
                            'number of noisy links extends 20 should be reported.'
                        ),
                        'axis_titles': ['sensor number', 'noisy header links']
                    },
                    {
                        'title': 'RMS CMS Normalised Header noise for all sensors',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/Header/HeaderNoise_allSensors',
                    },
                    {
                        'title': 'RMS CMS NoisyHeaderLinksPerSensor',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/Header/NoisyHeaderLinksPerSensor'
                    },
                ]
            },
            {
                'title': 'Header cross-talk 2',
                'plots': [
                    {
                        'title': 'Noise on 1st Channel Minus Average Noise of Link',
                        'name':  'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_SummaryPlot_2D',
                        'tip': (
                            'The colours represent the amount of noise in the first channel of each link minus the average noise of that analogue link. It is normal to get a lot of high noise values above zero.'
                        ),
                        'axis_titles': ['sensor number', 'Analogue Link']
                    },
                    {
                        'title': 'BHXT_SummaryPlot_1D',
                        'name':  'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_SummaryPlot_1D'
                    },
                    {
                        'title': 'Noise difference between the first and the reference channel',
                        'name':  'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_DiffPlot_1D',
                        'tip': (
                            'This is a 1D summary plot of the noise difference between the '
                            'first channel in each link and the reference channel. There should '
                            'only be a small difference in ADC counts between the channels and the '
                            'reference channel. Thus we usually get a broad distribution (usually '
                            'centred somewhere between 0 and 1) with the majority of the entries '
                            'lying somewhere between -2 and 2. If there are a large number of '
                            'channels which differ significantly from the reference, i.e. entries '
                            'with a high difference in ADC counts (greater/less than +/-3), then '
                            'this should be reported.'
                        ),
                        'axis_titles': ['difference in ADC counts', 'number of channels']
                    },
                ]
            }
        ]
    }
)

CLUSTERS = (
    'clusters', {
        'title': 'Clusters',
        'subpages': [
            {
                'title': 'Cluster and Strips',
                'plots': [
                    {
                        'title': 'Number of VELO clusters per event (Default)',
                        'short': 'Clusters per event',
                        'name': 'Velo/VeloPrivateClusterMonitor/# VELO clusters',
                        'tip': (
                            'The distribution should peak extremely sharply at low cluster numbers, below 50. There is usually a very small tail extending toward higher numbers of clusters. This is quite different to earlier runs.'
                        ),
                        'axis_titles': ['Number of clusters', 'Entries']
                    },
                    {
                        'title': 'Number of strips per cluster',
                        'short': 'Strips per cluster',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster size',
                        'options': {
                            'uncertainties': True
                        },
                        'normalise': True,
                        'tip': (
                            'This distribution generally produces a staircase shape, with the highest number of strips corresponding to one or two clusters and the lowest number corresponding to four clusters.'
                        ),
                        'axis_titles': ['Cluster size', 'Entries']
                    },
                    {
                        'title': 'Active chip links versus sensor',
                        'short': 'Active links per sensor',
                        'name': 'Velo/VeloPrivateClusterMonitor/Active chip links vs sensor',
                        'tip': (
                            'The R sensors have a distinctive pattern in this particular plot. This is completely normal and simply due to the geometry of the sensors. The Phi sensors should have a mostly uniform mid-range colour. Any links which stand out, have very high or very low activity, should be reported (Note that sensor 82 link 49, sensor 85 link 28-31, and sensor 97 sensor 48-51 have bad links).'
                        ),
                        'axis_titles': ['Sensor number', 'Active links']
                    },
                    {
                        'title': 'Number of strips per cluster versus sensor',
                        'short': 'Strips per cluster vs. sensor',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster size vs sensor',
                        'tip': (
                            'This 2D plot also exhibits a distinctive shape. The number of strips decreases as cluster size increases, resulting in the colour shifting from yellow/ green to blue as you move up the y-axis. Near the interaction point there is generally a decrease in the number of strips with one cluster and an increase in the number with three clusters. There is also an increase in the number with one cluster after the interaction point. This is most visible for the R sensors.'
                        ),
                        'axis_titles': ['Sensor number', 'Cluster size']
                    },
                ],
                'layout': (2, 2)
            },
            {
                'title': 'ADC distributions',
                'plots': [
                    {
                        'title': 'Cluster ADC value',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value',
                        'tip': (
                            'The distribution should be a Landau distribution and a small peak in the low ADC region.'
                        ),
                        'axis_titles': ['ADC of cluster', 'Entries']
                    },
                    {
                        'title': 'Cluster ADC values vs sensor',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC values vs sensor',
                        'tip': (
                            'For all of the sensors the majority of the clusters should have below 50 ADC counts. This results in a narrow band of colour near the bottom of the plot.'
                        ),
                        'axis_titles': ['Sensor number', 'ADC of cluster']
                    },
                    {
                        'title': 'Number of VELO clusters per event for each sensor',
                        'short': 'Clusters per event per sensor',
                        'name': 'Velo/VeloPrivateClusterMonitor/# clusters sensor {0}',
                        'sensor_dependent': True,
                        'tip': (
                            'Per-sensor plot. The distribution should peak extremely sharply at low cluster numbers (below 50).'
                        ),
                        'axis_titles': ['Number of clusters', 'Entries']
                    },
                    {
                        'title': 'Cluster ADC values for each sensor',
                        'plottables': [
                            {
                                'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value sensor {0}',
                                'title': 'Data',
                                'options': {
                                    'style': 0
                                }
                            },
                            {
                                'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value fit function sensor {0}',
                                'title': 'Fit',
                                'options': {
                                    'color': 'k',
                                    'style': 0
                                }
                            }
                        ],
                        'sensor_dependent': True,
                        'options': {
                            'legend': True
                        },
                        'tip': (
                            'Per-sensor plot. This should be a Landau distribution.'
                        ),
                        'axis_titles': ['ADC of Clusters', 'Entries']
                    }
                ],
                'layout': (2, 2)
            },
            {
                'title': 'Low ADC rates and ADC fit parameters',
                'plots': [
                    {
                        'title': 'Rate vs DistToM2[um]',
                        'name': 'Velo/VeloTrackMonitor/Rate_DistToM2',
                        'options': {
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 1.0
                        },
                        'tip': (
                            'The relative Low ADC rate should increase when the track is closer to the M2 routing line due to the coupling effect.'
                        ),
                        'axis_titles': ['Distance to M2', 'Rate']
                    },
                    {
                        'title': 'Rate vs DistToOutStrip[um]',
                        'name': 'Velo/VeloTrackMonitor/Rate_DistToOutStrip',
                        'options': {
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 1.0
                        },
                        'tip': (
                            'The relative Low ADC rate should decrease when the track is closer to the M1 strip due to the shielding.'
                        ),
                        'axis_titles': ['Distance to M1', 'Rate']
                    },
                    {
                        'title': 'Cluster ADC values fit parameter : MPV',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value MPV vs Sensor number',
                        'tip': (
                            'The MPV in ADC counts versus the number of sensors, with R and Phi sensors separated into two distributions. All R or Phi sensors should have roughly same MPV value.'
                        ),
                        'axis_titles': ['Sensor number', 'MPV']
                    },
                    {
                        'title': 'Cluster ADC values fit parameter : FWHM',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value FWHM vs Sensor number',
                        'tip': (
                            'The FWHM in ADC counts versus the number of sensors, with R and Phi sensors separated into two distributions. There should be a peak around interaction piont. '
                        ),
                        'axis_titles': ['Sensor number', 'FWHM']
                    }
                ],
                'layout': (2, 2)
            }
        ]
    }
)

OCCUPANCY = (
    'occupancy', {
        'title': 'Occupancy',
        'subpages': [
            {
                'title': 'Occupancy 1',
                'plots': [
                    {
                        'title': 'Channel occupancy',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/OccPerChannelSens{0}',
                        'sensor_dependent': True,
                    }
                ]
            },
            {
                'title': 'Occupancy 2',
                'plots': [
                    {
                        'title': 'Average sensor occupancy',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/OccAvrgSens'
                    },
                    {
                        'title': 'Occupancy spectrum (zoom)',
                        'short': 'Occupancy spectrum',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/OccSpectMaxLow'
                    }
                ],
                'layout': (1, 2)
            },
            {
                'title': 'Occupancy 3',
                'plots': [
                    {
                        'title': '% VELO occupancy vs. LHC bunch ID (A side)',
                        'short': 'Occupancy vs. BCID (A side)',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/h_veloOccVsBunchId_ASide'
                    },
                    {
                        'title': '% VELO occupancy vs. LHC bunch ID (C side)',
                        'short': 'Occupancy vs. BCID (C side)',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/h_veloOccVsBunchId_CSide'
                    }
                ],
                'layout': (1, 2)
            }
        ]
    }
)

BAD_CHANNELS = (
    'bad_channels', {
        'title': 'Bad Channels',
        'subpages': [
            {
                'title': 'Info Per Sensor',
                'plots': [
                    {
                        'title': 'Dead Channels Per Sensor',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/DeadPerSensor',
                    },
                    {
                        'title': 'Noisy Channels Per Sensor',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/NoisyPerSensor',
                    },
                ],
                'layout': (1, 2)
            },
            {
                'title': 'Info Per Sensor and Link',
                'plots': [
                    {
                        'title': 'Dead Channels Per Sensor And Link',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/DeadPerSensorAndLink',
                    },
                    {
                        'title': 'Noisy Channels Per Sensor And Link',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/NoisyPerSensorAndLink',
                    },
                ],
                'layout': (1, 2)
            },
            {
                'title': 'Dead/Noisy',
                'plots': [
                    {
                        'title': 'Dead/Noisy Channels Per Sensor',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/DeadOrNoisyPerSensor',
                    },
                    {
                        'title': 'Dead/Noisy Channels Per Sensor And Link',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/DeadOrNoisyPerSensorAndLink',
                    },
                ],
                'layout': (1, 2)
            }
        ]
    }
)

TRACKS = (
    'tracks', {
        'title': 'Tracks',
        'subpages': [
            {
                'title': 'Tracks 1',
                'plots': [
                    {
                        'title': 'Number of clusters associated to a track',
                        'name': 'Velo/VeloTrackMonitor/NMeasurements',
                        'options': {
                            'yAxisZeroSuppressed': False,
                            'style': 3
                        },
                    },
                    {
                        'title': 'Pseudo efficiency by interpolation',
                        'name': 'Velo/VeloTrackMonitor/Track_pseudoEfficiencyByInterp',
                        'options': {
                            'yAxisZeroSuppressed': False,
                            'style': 3
                        }
                    },
                    {
                        'title': 'ADC for R clusters associated to a track',
                        'name': 'Velo/VeloTrackMonitor/Track_radc',
                        'options': {
                            'style': 3
                        }
                    },
                    {
                        'title': 'ADC for Phi clusters associated to a track',
                        'name': 'Velo/VeloTrackMonitor/Track_phiadc',
                        'title': 'Phi',
                        'options': {
                            'yAxisZeroSuppressed': False,
                            'style': 3,
                            'legend': True
                        }
                    }
                ],
                'layout': (2, 2)
            },
            {
                'title': 'Tracks 2',
                'plots': [
                    {
                        'title': 'Track polar angle theta (degrees)',
                        'name': 'Velo/VeloTrackMonitor/Track_Theta',
                        'options': {
                            'yAxisZeroSuppressed': False,
                            'style': 3
                        }
                    },
                    {
                        'title': 'Track pseudo rapidity eta',
                        'name': 'Velo/VeloTrackMonitor/Track_Eta',
                        'options': {
                            'yAxisZeroSuppressed': False,
                            'style': 3
                        }
                    },
                    {
                        'title': 'Track aimuth phi (degrees)',
                        'name': 'Velo/VeloTrackMonitor/Track_Phi',
                        'options': {
                            'yAxisZeroSuppressed': False,
                            'style': 3
                        }
                    },
                ]
            }
        ]
    }
)

VERTICES = (
    'vertices', {
        'title': 'Vertices',
        'subpages': [
            {
                'title': 'PV Overview',
                'plots': [
                    {
                        'title': 'PV left-right delta x',
                        'name': 'Track/TrackVertexMonitor/PV left-right delta x',
                        'normalise': True,
                        'options': {
                            'yAxisZeroSuppressed': False
                        }
                    },
                    {
                        'title': 'PV x position',
                        'name': 'Track/TrackVertexMonitor/PV x position',
                        'normalise': True,
                        'options': {
                            'yAxisZeroSuppressed': False
                        }
                    },
                    {
                        'title': 'PV y position',
                        'name': 'Track/TrackVertexMonitor/PV y position',
                        'normalise': True,
                        'options': {
                            'yAxisZeroSuppressed': False
                        }
                    },
                    {
                        'title': 'PV z position',
                        'name': 'Track/TrackVertexMonitor/PV z position',
                        'normalise': True,
                        'options': {
                            'yAxisZeroSuppressed': False
                        }
                    }
                ],
                'layout': (2, 2)
            },
            {
                'title': 'PV VELO Half Distance',
                'plots': [
                    {
                        'title': 'PV left-Left half x',
                        'name': 'Track/TrackVertexMonitor/PV left-Left half x',
                        'normalise': True,
                    },
                    {
                        'title': 'PV right-Right half x',
                        'name': 'Track/TrackVertexMonitor/PV right-Right half x',
                        'normalise': True,
                    },
                    {
                        'title': 'PV left-Left half y',
                        'name': 'Track/TrackVertexMonitor/PV left-Left half y',
                        'normalise': True,
                    },
                    {
                        'title': 'PV right-Right half y',
                        'name': 'Track/TrackVertexMonitor/PV right-Right half y',
                        'normalise': True,
                    }
                ],
                'layout': (2, 2)
            }
        ]
    }
)

ERRORS = (
    'errors', {
        'title': 'Errors',
        'subpages': [
            {
                'title': 'Overview',
                'plots': [
                    {
                        'title': 'Errors per sensor',
                        'name': 'Velo/ErrorMon/Error Counter',
                        'axis_titles': ['Sensor number', 'Number of errors']
                    },
                    {
                        'title': 'Mean error frequency per sensor',
                        'name': 'Velo/ErrorMon/Error Frequency',
                        'axis_titles': ['Sensor number', 'Mean error frequency']
                    },
                    {
                        'title': 'Error type frequency',
                        'name': 'Velo/ErrorMon/Types of Errors',
                        'axis_titles': ['Sensor number', 'Number of errors'],
                        'tip': '0=pseudoheader, 1=PCN, 2=adc_FIFO, 3=channel'
                    },
                    {
                        'title': 'Number of desyncs in SEU counter',
                        'name': 'Velo/SEUMonitor/desyncctr1d',
                        'axis_titles': ['Sensor number', 'Number of desyncs']
                    }
                ]
            },
            {
                'title': 'Error Banks',
                'plots': [
                    {
                        'title': 'Pseudoheader errors per link per sensor',
                        'name': 'Velo/ErrorMon/Overview pseudoheader errors',
                        'axis_titles': ['Sensor number', 'Link number']
                    },
                    {
                        'title': 'PCN errors per link per sensor',
                        'name': 'Velo/ErrorMon/Overview PCN errors',
                        'axis_titles': ['Sensor number', 'Link number']
                    },
                    {
                        'title': 'adc_FIFO errors per link per sensor',
                        'name': 'Velo/ErrorMon/Overview adc_FIFO errors',
                        'axis_titles': ['Sensor number', 'Link number']
                    },
                    {
                        'title': 'IHeader errors per link per sensor',
                        'name': 'Velo/ErrorMon/Overview IHeader errors',
                        'axis_titles': ['Sensor number', 'Link number']
                    }
                ]
            },
            {
                'title': 'Single Event Upsets',
                'plots': [
                    {
                        'title': 'Event statistics',
                        'name': 'Velo/SEUMonitor/eventstat',
                        'axis_titles': ['Event type', 'Number of events']
                    },
                    {
                        'title': 'Number of desyncs in SEU counter',
                        'name': 'Velo/SEUMonitor/desyncctr1d',
                        'axis_titles': ['Sensor number', 'Number of desyncs']
                    },
                    {
                        'title': 'Number of SEUs',
                        'name': 'Velo/SEUMonitor/seuctr1d',
                        'axis_titles': ['Sensor number', 'Number of SEU\'s']
                    },
                    {
                        'title': 'Number of desyncs in SEU counter per beetle',
                        'name': 'Velo/SEUMonitor/desyncctr2d',
                        'axis_titles': ['Sensor number', 'Beetle number']
                    }
                ]
            },
            {
                'title': 'PCN error details',
                'plots': [
                    {
                        'title': 'Beetle PCN error map',
                        'name': 'Velo/PCNErrorMon/hBeetleMap',
                        'axis_titles': ['Tell1 ID', 'Beetle number']
                    },
                    {
                        'title': 'FEM PCN errors',
                        'name': 'Velo/PCNErrorMon/hFEMPCNsync',
                        'axis_titles': ['Sensor number', 'Desync frequency']
                    }
                ]
            }
        ]
    }
)

# Clones from other tabs, as recommended by Piquets:
SENSOR_OVERVIEW = (
    'sensor_overview', {
        'title': 'Sensor overview',
        'subpages': [
            {
                'title': 'Occupancy & Noise',
                'plots': [
                    {
                        'title': 'Channel occupancy',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/OccPerChannelSens{0}',
                        'sensor_dependent': True,
                    },
                    {
                        'title': 'RMS CMS noise vs. chip channel',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
                        'sensor_dependent': True,
                        'options': {
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 5.0
                        }
                    }
                ],
                'layout': (1, 2)
            },
            {
                'title': 'Subtracted ADC 2D',
                'plots': [
                    {
                        'title': 'Subtracted ADC 2D',
                        'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Ped_Sub_ADCs_2D',
                        'sensor_dependent': True
                    }
                ]
            },
            {
                'title': 'Cluster ADC',
                'plots': [
                    {
                        'title': 'Cluster ADC values for each sensor',
                        'plottables': [
                            {
                                'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value sensor {0}',
                                'title': 'Data',
                                'options': {
                                    'style': 0
                                }
                            },
                            {
                                'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value fit function sensor {0}',
                                'title': 'Fit',
                                'options': {
                                    'color': 'k',
                                    'style': 4
                                }
                            }
                        ],
                        'sensor_dependent': True,
                        'options': {
                            'legend': True
                        }
                    }
                ]
            }
        ]
    }
)

IP_RESOLUTIONS = (
    'IP resolutions', {
        'title': 'IP resolutions',
        'subpages': [
            {
                'title': 'IP resolutions vs 1/pT',
                'plots': [
                    {
                        'title': 'IPx resolution vs 1/pT',
                        'name': 'Velo/VeloIPResolutionMonitor/IPX-Vs-InversePT-LongTracks-Sigma',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 0.1,
                            'style': 3
                        }
                    },
                    {
                        'title': 'IPy resolution vs 1/pT',
                        'name': 'Velo/VeloIPResolutionMonitor/IPY-Vs-InversePT-LongTracks-Sigma',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 0.1,
                            'style': 3
                        }
                    },
                    {
                        'title': 'IPx mean vs 1/pT',
                        'name': 'Velo/VeloIPResolutionMonitor/IPX-Vs-InversePT-LongTracks-Mean',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': -0.02,
                            'yAxisMaximum': 0.02,
                            'style': 3
                        }
                    },
                    {
                        'title': 'IPy mean vs 1/pT',
                        'name': 'Velo/VeloIPResolutionMonitor/IPY-Vs-InversePT-LongTracks-Mean',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': -0.02,
                            'yAxisMaximum': 0.02,
                            'style': 3
                        }
                    },
                ],
                'layout': (2, 2)
            },
            {
                'title': 'IP resolutions vs phi',
                'plots': [
                    {
                        'title': 'IPx resolution vs phi',
                        'name': 'Velo/VeloIPResolutionMonitor/IPX-Vs-Phi-LongTracks-Sigma',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': -0.02,
                            'yAxisMaximum': 0.02,
                            'style': 3
                        }
                    },
                    {
                        'title': 'IPy resolution vs phi',
                        'name': 'Velo/VeloIPResolutionMonitor/IPY-Vs-Phi-LongTracks-Sigma',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': -0.02,
                            'yAxisMaximum': 0.02,
                            'style': 3
                        }
                    },
                    {
                        'title': 'IPx mean vs phi',
                        'name': 'Velo/VeloIPResolutionMonitor/IPX-Vs-Phi-LongTracks-Mean',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': -0.02,
                            'yAxisMaximum': 0.02,
                            'style': 3
                        }
                    },
                    {
                        'title': 'IPy mean vs phi',
                        'name': 'Velo/VeloIPResolutionMonitor/IPY-Vs-Phi-LongTracks-Mean',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': -0.02,
                            'yAxisMaximum': 0.02,
                            'style': 3
                        }
                    }
                ],
                'layout': (2, 2)
            },
            {
                'title': 'IP resolutions vs eta',
                'plots': [
                    {
                        'title': 'IPx resolution vs eta',
                        'name': 'Velo/VeloIPResolutionMonitor/IPX-Vs-Eta-LongTracks-Sigma',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 0.13,
                            'style': 3
                        }
                    },
                    {
                        'title': 'IPy resolution vs eta',
                        'name': 'Velo/VeloIPResolutionMonitor/IPY-Vs-Eta-LongTracks-Sigma',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': 0.0,
                            'yAxisMaximum': 0.13,
                            'style': 3
                        }
                    },
                    {
                        'title': 'IPx mean vs eta',
                        'name': 'Velo/VeloIPResolutionMonitor/IPX-Vs-Eta-LongTracks-Mean',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': -0.02,
                            'yAxisMaximum': 0.02,
                            'style': 3
                        }
                    },
                    {
                        'title': 'IPy mean vs eta',
                        'name': 'Velo/VeloIPResolutionMonitor/IPY-Vs-Eta-LongTracks-Mean',
                        'options': {
                            'uncertainties': True,
                            'yAxisMinimum': -0.02,
                            'yAxisMaximum': 0.02,
                            'style': 3
                        }
                    }
                ],
                'layout': (2, 2)
            }
        ]
    }
)

IV_SCANS = (
    'IV', {
        'title': 'IV',
        'plots': [
            {
                'title': 'IV',
                'plottables': [
                    {
                        'name': 'Ramp Up',
                        'title': 'Ramp Up',
                        'isIV': True,
                        'options': {
                            'color': 'b',
                            'style': 2,
                            'marker': 'v'
                        }
                    },
                    {
                        'name': 'Ramp Down',
                        'title': 'Ramp Down',
                        'isIV': True,
                        'options': {
                            'color': 'g',
                            'style': 2
                        }
                    }
                ],
                'options': {
                    'legend': True
                }
            },
            {
                'title': 'IV',
                'plottables': [
                    {
                        'name': 'Ramp Up',
                        'title': 'Ramp Up',
                        'isIV': True,
                        'options': {
                            'color': 'b',
                            'style': 2,
                            'sensorShift': 1
                        }
                    },
                    {
                        'name': 'Ramp Down',
                        'title': 'Ramp Down',
                        'isIV': True,
                        'options': {
                            'color': 'g',
                            'style': 2,
                            'sensorShift': 1,
                            'marker': 'v'
                        }
                    }
                ]
            },
            {
                'title': 'IV',
                'plottables': [
                    {
                        'name': 'Ramp Up',
                        'title': 'Ramp Up',
                        'isIV': True,
                        'options': {
                            'color': 'b',
                            'style': 2,
                            'sensorShift': 2
                        }
                    },
                    {
                        'name': 'Ramp Down',
                        'title': 'Ramp Down',
                        'isIV': True,
                        'options': {
                            'color': 'g',
                            'style': 2,
                            'sensorShift': 2,
                            'marker': 'v'
                        }
                    }
                ]
            },
            {
                'title': 'IV',
                'plottables': [
                    {
                        'name': 'Ramp Up',
                        'title': 'Ramp Up',
                        'isIV': True,
                        'options': {
                            'color': 'b',
                            'style': 2,
                            'sensorShift': 3
                        }
                    },
                    {
                        'name': 'Ramp Down',
                        'title': 'Ramp Down',
                        'isIV': True,
                        'options': {
                            'color': 'g',
                            'style': 2,
                            'sensorShift': 3,
                            'marker': 'v'
                        }
                    }
                ]
            }
        ]
    }
)

# Define the ordered list of pages
# Pages will appear in the GUIs in this order
run_view_pages = OrderedDict([
    PEDESTALS,
    NOISE,
    HEADER_CROSS_TALK,
    CLUSTERS,
    OCCUPANCY,
    BAD_CHANNELS,
    TRACKS,
    VERTICES,
    ERRORS,
    SENSOR_OVERVIEW,
    IP_RESOLUTIONS,
    IV_SCANS
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
