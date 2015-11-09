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
    'plots': 
        [
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
    #==========================================================================
    ('dqs', 
        {
        'title': 'DQS'
        }
    ),
    #==========================================================================
    ('pedestals', 
        {
        'title': 'Pedestals',
        'subpages': 
            [
                {
                'title': '1d plots',
                'plots': 
                    [
                        {
                        'title': 'Pedestal bank',
                        'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Pedestal_Bank',
                        'sensor_dependent': True,
                        'tip': 'Test of tipping - Hello world!',
                        'axis_titles': ['config x', 'config y']
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
                'title': '2d plots',
                'plots': 
                    [
                        {
                        'title': 'Subtracted ADC 2D',
                        'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Ped_Sub_ADCs_2D',
                        'sensor_dependent': True
                        }
                    ]
                }
            ]
        }
    ),
    #==========================================================================
    ('noise', 
        {
        'title': 'Noise',
        'subpages': 
            [
                {
                'title': 'Velo Overview',
                'plots': 
                    [
#                         {
#                         'title': 'RMS CMS noise vs. station / R sensors (neg. for C-side)',
#                         'short': 'RMS CMS noise vs. station / R sensors (neg. for C-side)',
#                         'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_R',
#                         'yrange': [-4, 4] 
#                         },
#                         {
#                         'title': 'RMS CMS noise vs. station / Phi sensors (neg. for C-side)',
#                         'short': 'RMS CMS noise vs. station / Phi sensors (neg. for C-side)',
#                         'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_Phi',
#                         'yrange': [-4, 4] 
#                         },
                        {
                        'title': 'RMS CMS noise vs. station (neg. for C-side)',
                        'yrange': [-4, 4],
                        'plottables': 
                            [
                                {
                                'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_R',
                                'legend': 'R'
                                },
                                {
                                'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_Phi',
                                'legend': 'Phi',
                                'color': 'y'
                                },
                                {
                                'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/Sensor_vs_Station_R',
                                'asText': 'True',
                                },
                                {
                                'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/Sensor_vs_Station_Phi',
                                'asText': 'True',
                                }
                            ],
                        'showLegend': True
                        }
                    ],
                'layout': (2, 1)
                },
                {
                'title': 'Per Sensor (Channels)', 
                'plots': 
                    [
                        {
                        'title': 'RMS noise vs. chip channel',
                        'name': 'Vetra/NoiseMon/DecodedADC/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
                        'sensor_dependent': True,
                        'yrange': [0, 5]
                        },
                        {
                        'title': 'RMS CMS noise vs. chip channel',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
                        'sensor_dependent': True,
                        'yrange': [0, 5]
                        },
                    ],
                 'layout': (1, 2)
                },
                {
                'title': 'Per Sensor (Strips)', 
                'plots':
                    [
                        {
                        'title': 'RMS noise vs. strip',
                        'name': 'Vetra/NoiseMon/DecodedADC/TELL1_{0:03d}/RMSNoise_vs_Strip',
                        'sensor_dependent': True,
                        'yrange': [0, 5]
                        },
                        {
                        'title': 'RMS CMS noise vs. strip',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/TELL1_{0:03d}/RMSNoise_vs_Strip',
                        'sensor_dependent': True,
                        'yrange': [0, 5]
                        },
                    ],
                 'layout': (1, 2)
                },
                {
                'title': 'Average Distribution',
                'plots': 
                    [
                        {
                        'title': 'RMS CMS noise (R sensors)',
                        'short': 'RMS CMS noise (R sensors)',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_R',
                        'options': {'yAxisMinimum': 0, 'yAxisMaximum': 45} 
                        },
                        {
                        'title': 'RMS CMS noise (Phi sensors)',
                        'short': 'RMS CMS noise (Phi sensors)',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_Phi',
                        'options': {'yAxisMinimum': 0, 'yAxisMaximum': 45}
                        }, 
                    ],
                'layout': (2, 1)      
                },
                {
                'title': 'Per Link Map',
                'plots': 
                    [
                        {
                        'title': 'RMS CMS noise vs. link',
                        'short': 'RMS CMS noise vs. link',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Link',
                        'options': {'zAxisMinimum': 0, 'zAxisMaximum': 5}
                        },
                        {
                        'title': 'RMS noise vs. link',
                        'short': 'RMS noise vs. link',
                        'name': 'Vetra/NoiseMon/DecodedADC/Average/RMSNoise_vs_Link',
                        'options': {'zAxisMinimum': 0, 'zAxisMaximum': 5}
                        },
                     ],
                'layout': (1, 2)
                }
            ]
        }
    ),
    #==========================================================================
    ('headerxtalk', 
        {
        'title': 'HeaderxTalk',
        'subpages':
            [
                {
                'title': 'HeaderxTalk_1',
                'plots': 
                    [
                        {
                        'title': 'RMS Normalised Header noise for all sensors',
                        'name': 'Vetra/NoiseMon/DecodedADC/Header/HeaderNoise_allSensors',
                        'tip': ('This plot shows for every link (64 entries per powered sensor) the noise measured ' +  
                               'in the first channel of that link, which is affected by header cross talk, normalised ' + 
                               'to the average noise of the link. The peak of the distribution should be to the left ' + 
                               'of 2. There should only be a very small tail to the right of 2.'),
                        'axis_titles': ['normalised header noise', 'number of channels']
                        },
                        {
                        'title': 'RMS NoisyHeaderLinksPerSensor',
                        'name': 'Vetra/NoiseMon/DecodedADC/Header/NoisyHeaderLinksPerSensor',
                        'tip': ('The distribution should stay below 20. Any sensors whereby their ' +
                               'number of noisy links extends 20 should be reported.'),
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
                'title': 'HeaderxTalk_2',
                'plots':
                    [
                        {
                        'title': 'Noise on 1st Channel Minus Average Noise of Link',
                        'name':  'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_SummaryPlot_2D',
                        'tip': ('The distribution should stay below 20. Any sensors whereby their ' +
                               'number of noisy links extends 20 should be reported.'),
                        'axis_titles': ['sensor number', 'Analogue Link']
                        },
                        {
                        'title': 'BHXT_SummaryPlot_1D',
                        'name':  'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_SummaryPlot_1D'
                        },
                        {
                        'title': 'Noise difference between the first and the reference channel',
                        'name':  'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_DiffPlot_1D',
                        'tip': ('This is a 1D summary plot of the noise difference between the ' +
                               'first channel in each link and the reference channel. There should ' +
                               'only be a small difference in ADC counts between the channels and the ' +
                               'reference channel. Thus we usually get a broad distribution (usually ' +
                               'centred somewhere between 0 and 1) with the majority of the entries ' +
                               'lying somewhere between -2 and 2. If there are a large number of ' +
                               'channels which differ significantly from the reference, i.e. entries ' +
                               'with a high difference in ADC counts (greater/less than +/-3), then ' +
                               'this should be reported.'),
                        'axis_titles': ['difference in ADC counts', 'number of channels']
                        },
                    ]
                } 
            ]
        }
    ),
    #==========================================================================
    ('clusters', 
        {
        'title': 'Clusters',
        'subpages': 
            [
                {
                'title': 'Clusters_1',
                'plots': 
                    [ 
                        {
                        'title': 'Number of VELO clusters per event (Default)',
                        'short': 'Clusters per event',
                        'name': 'Velo/VeloPrivateClusterMonitor/# VELO clusters'
                        },
                        {
                        'title': 'Number of strips per cluster',
                        'short': 'Strips per cluster',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster size',
                        'options': {'showUncertainties': True},
                        'normalise': True
                        },
                        {
                        'title': 'Active chip links versus sensor',
                        'short': 'Active links per sensor',
                        'name': 'Velo/VeloPrivateClusterMonitor/Active chip links vs sensor'
                        },
                        {
                        'title': 'Number of strips per cluster versus sensor',
                        'short': 'Strips per cluster vs. sensor',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster size vs sensor'
                        },
                     ],
                'layout': (2, 2)
                },
                {
                'title': 'Clusters_2',
                'plots': 
                    [ 
                        {
                        'title': 'Cluster ADC value',
                        'short': 'Cluster ADC value',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value',
                        },
                        {
                        'title': 'Cluster ADC values vs sensor',
                        'short': 'Cluster ADC values vs sensor',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC values vs sensor'
                        },
                        {
                        'title': 'Number of VELO clusters per event for each sensor',
                        'short': 'Clusters per event per sensor',
                        'name': 'Velo/VeloPrivateClusterMonitor/# clusters sensor {0}',
                        'sensor_dependent': True
                        },
                        {
                        'title': 'Cluster ADC values for each sensor',
                        'short': 'Cluster ADC values for each sensor',
                        'plottables' : 
                            [
                                {
                                'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value sensor {0}',
                                'legend': 'Data',
                                'style': 0
                                },
                                {
                                'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value fit function sensor {0}',
                                'legend': 'Fit',
                                'color':'k',
                                'style': 4
                                }
                            ],
                        'showLegend': True,
                        'sensor_dependent': True,
                        },
                    ],
                'layout': (2, 2)
                },
                {
                'title': 'Clusters_3',
                'plots': 
                    [ 
                        {
                        'title': 'Rate vs DistToM2[um]',
                        'short': 'Rate vs DistToM2[um]',
                        'name': 'Velo/VeloTrackMonitor/Rate_DistToM2',
                        'options': {'yAxisMinimum': 0, 'yAxisMaximum': 1}
                        },
                        {
                        'title': 'Rate vs DistToOutStrip[um]',
                        'short': 'Rate vs DistToOutStrip[um]',
                        'name': 'Velo/VeloTrackMonitor/Rate_DistToOutStrip',
                        'options': {'yAxisMinimum': 0, 'yAxisMaximum': 1}
                        },
                        {
                        'title': 'Cluster ADC values fit parameter : MPV',
                        'short': 'Cluster ADC values fit parameter : MPV',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value MPV vs Sensor number'
                        },
                        {
                        'title': 'Cluster ADC values fit parameter : FWHM',
                        'short': 'Cluster ADC values fit parameter : FWHM',
                        'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value FWHM vs Sensor number'
                        }
                    ],
                'layout': (2, 2)
                }
            ]
        }
    ),
    #==========================================================================
    ('occupancy', 
        {
        'title': 'Occupancy',
        'subpages':
            [
                {
                'title': 'Occupancy_1',
                'plots': 
                    [
                        {
                        'title': 'Channel occupancy',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/OccPerChannelSens{0}',
                        'sensor_dependent': True,
                        }
                    ]
                },
                {
                'title': 'Occupancy_2',
                'plots':
                    [
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
                'title': 'Occupancy_3',
                'plots':
                    [   
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
    ),
    #==========================================================================
    ('bad_channels', 
        {
        'title': 'Bad Channels',
        'subpages': 
            [
                {
                'title': 'Info Per Sensor',
                'plots': 
                    [
                        {
                        'title': 'Dead Channels Per Sensor',
                        'short': 'Dead Channels Per Sensor',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/DeadPerSensor',
                        },
                        {
                        'title': 'Noisy Channels Per Sensor',
                        'short': 'Noisy Channels Per Sensor',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/NoisyPerSensor',
                        },
                    ],
                'layout': (1, 2)
                },
                {
                'title': 'Info Per Sensor and Link',
                'plots': 
                    [
                        {
                        'title': 'Dead Channels Per Sensor And Link',
                        'short': 'Dead Channels Per Sensor And Link',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/DeadPerSensorAndLink',
                        },
                        {
                        'title': 'Noisy Channels Per Sensor And Link',
                        'short': 'Noisy Channels Per Sensor And Link',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/NoisyPerSensorAndLink',
                        },
                    ],
                'layout': (1, 2)
                },
                {
                'title': 'Dead/Noisy',
                'plots': 
                    [
                        {
                        'title': 'Dead/Noisy Channels Per Sensor',
                        'short': 'Dead/Noisy Channels Per Sensor',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/DeadOrNoisyPerSensor',
                        },
                        {
                        'title': 'Dead/Noisy Channels Per Sensor And Link',
                        'short': 'Dead/Noisy Channels Per Sensor And Link',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/DeadOrNoisyPerSensorAndLink',
                        },
                    ],
                'layout': (1, 2)
                }
            ]
        }
    ),
    #==========================================================================
    ('tracks', 
        {
        'title': 'Tracks',
        'subpages': 
            [
                {
                'title': 'Tracks_1',
                'plots': 
                    [
                        {
                        'title': 'Number of clusters associated to a track',
                        'name': 'Velo/VeloTrackMonitor/NMeasurements',
                        'style':3, 
                        'options' : {'yAxisZeroSuppressed': False}, 
                        },
                        {
                        'title': 'Pseudo efficiency by interpolation',
                        'name': 'Velo/VeloTrackMonitor/Track_pseudoEfficiencyByInterp',
                        'style':3, 
                        'options' : {'yAxisZeroSuppressed': False}, 
                        },
                        {
                        'title': 'ADC for R clusters associated to a track',
                        'name': 'Velo/VeloTrackMonitor/Track_radc', 
                        'style':3, 
                        },
                        {
                        'title': 'ADC for Phi clusters associated to a track',
                        'name': 'Velo/VeloTrackMonitor/Track_phiadc',
                        'legend': 'Phi',
                        'showLegend': True,
                        'style':3, 
                        'options' : {'yAxisZeroSuppressed': False}, 
                        }
                    ],
                    'layout': (2,2)
                },
                {
                'title': 'Tracks_2',
                'plots': 
                    [
                        {
                        'title': 'Track polar angle theta (degrees)',
                        'name': 'Velo/VeloTrackMonitor/Track_Theta',
                        'style':3, 
                        'options' : {'yAxisZeroSuppressed': False}, 
                        },
                        {
                        'title': 'Track pseudo rapidity eta',
                        'name': 'Velo/VeloTrackMonitor/Track_Eta',
                        'style':3, 
                        'options' : {'yAxisZeroSuppressed': False}, 
                        },
                        {
                        'title': 'Track aimuth phi (degrees)',
                        'name': 'Velo/VeloTrackMonitor/Track_Phi',
                        'style':3, 
                        'options' : {'yAxisZeroSuppressed': False}, 
                        },
                    ]
                }
            ]
        }
    ),
    #==========================================================================
    ('vertices', 
        {
        'title': 'Vertices',
        'subpages' : 
            [
                {
                'title' : 'PV Overview',
                'plots' : 
                    [
                        {
                        'title': 'PV left-right delta x',
                        'name': 'Track/TrackVertexMonitor/PV left-right delta x',
                        'normalise': True,
                        'options': {'yAxisZeroSuppressed': False}
                        },
                        {
                        'title': 'PV x position',
                        'name': 'Track/TrackVertexMonitor/PV x position',
                        'normalise': True,
                        'options': {'yAxisZeroSuppressed': False}
                        },
                        {
                        'title': 'PV y position',
                        'name': 'Track/TrackVertexMonitor/PV y position',
                        'normalise': True,
                        'options': {'yAxisZeroSuppressed': False}
                        },
                        {
                        'title': 'PV z position',
                        'name': 'Track/TrackVertexMonitor/PV z position',
                        'normalise': True,
                        'options': {'yAxisZeroSuppressed': False}
                        }
                    ],
                'layout': (2,2)
                },
                {
                'title' : 'PV VELO Half Distance',
                'plots' : 
                    [
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
                'layout': (2,2)
                }
            ]
        }
    ),
#==========================================================================
    ('errors',
        {
        'title': 'Errors',
        'subpages':
            [
                {
                'title': 'Overview',
                'plots':
                    [
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
                'plots':
                    [
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
                'plots':
                    [
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
                        'title': 'Number of SEU\'s',
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
                #{
                #'title': 'ProcStatus warnings',
                #'plots':
                    #[
                        #{
                        #'title': 'Number of ProcStatuses of each type found so far',
                        #'name': 'Velo/ProcStatusMonitor/m_numProcStatus',
                        #'axis_titles': ['ProcStatus type', 'Frequency']
                        #}
                    #]
                #},
                {
                'title': 'PCN error details',
                'plots':
                    [
                        {
                        'title': 'Beetle PCN error map',
                        'name': 'Velo/PCNErrorMon/hBeetleMap',
                        'axis_titles': ['Tell1 ID', 'Beetle number']
                        },
                        {
                        'title': 'FEM PCN errors',
                        'name': 'Velo/PCNErrorMon/hFEMPCNsync',
                        'axis_titles': ['Sensor number', 'Desync frequency']
                        },
                        {
                        'title': 'Per-sensor PCN error map',
                        'name': 'Velo/PCNErrorMon/hperBeetleBitMap_{0}_0',
                        'sensor_dependent': True,
                        'axis_titles': ['PCN bit', 'Correct value for PCN bit']
                        }
                    ]
                }
            ]
        }
    ),
    #==========================================================================
    ('sensor_overview', 
        # Clones from other tabs, as recommended by Piquets:
        {
        'title': 'Sensor overview',
        'subpages': 
            [
                {
                'title': 'Occupancy & Noise',
                'plots': 
                    [
                        {
                        'title': 'Channel occupancy',
                        'name': 'Velo/VeloPrivateOccupancyMonitor/OccPerChannelSens{0}',
                        'sensor_dependent': True,
                        },
                        {
                        'title': 'RMS CMS noise vs. chip channel',
                        'name': 'Vetra/NoiseMon/ADCCMSuppressed/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
                        'sensor_dependent': True,
                        'yrange': [0, 5]
                        }
                    ],
                    'layout': (1, 2)
                },
                {
                'title': 'Subtracted ADC 2D',
                'plots': 
                    [
                        {
                        'title': 'Subtracted ADC 2D',
                        'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Ped_Sub_ADCs_2D',
                        'sensor_dependent': True
                        }
                    ]
                },
                {
                'title': 'Cluster ADC',
                'plots':
                    [
                        {
                        'title': 'Cluster ADC values for each sensor',
                        'plottables': 
                            [
                                {
                                'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value sensor {0}',
                                'legend': 'Data',
                                'style': 0
                                },
                                {
                                'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value fit function sensor {0}',
                                'legend': 'Fit',
                                'color':'k',
                                'style': 4
                                }
                            ],
                        'showLegend': True
                        }
                    ],
                'showLegend': True,
                'sensor_dependent': True,
                }
#                 {
#                 'title': 'Errors per sensor',
#                 'name': 'Velo/ErrorMon/Error Counter'
#                 }
            ]
        }
    ),
    #==========================================================================
    ('IP resolutions', 
        {
        'title' : 'IP resolutions',
        'subpages' : 
            [
                {
                'title' : 'IP resolutions vs 1/pT',
                'plots' : 
                    [
                        {
                        'title' : 'IPx resolution vs 1/pT', # ROOT style formatting doesn't work, nor does Latex.
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-InversePT-LongTracks-Sigma',
                        'style': 3,
                        'yrange': [0.0, 0.1],
                        'options' : {'showUncertainties' : True}
                        # Would be good to be able to set x & y titles, needs to be done in histogramming algo.
                        },
                        {
                        'title' : 'IPy resolution vs 1/pT',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-InversePT-LongTracks-Sigma',
                        'style': 3,
                        'yrange': [0.0, 0.1],
                        'options' : {'showUncertainties' : True}
                        },
                        {
                        'title' : 'IPx mean vs 1/pT',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-InversePT-LongTracks-Mean',
                        'style': 3,
                        'yrange': [-0.02, 0.02],
                        'options' : {'showUncertainties' : True}
                        },
                        {
                        'title' : 'IPy mean vs 1/pT',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-InversePT-LongTracks-Mean',
                        'style': 3,
                        'yrange': [-0.02, 0.02],
                        'options' : {'showUncertainties' : True}
                        },
                    ],
                'layout' : (2,2)
                },
                {
                'title' : 'IP resolutions vs phi',
                'plots' : 
                    [
                        {
                        'title' : 'IPx resolution vs phi',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-Phi-LongTracks-Sigma',
                        'yrange': [-0.02, 0.02],
                        'style':3,
                        'options' : {'showUncertainties' : True}
                        },
                        {
                        'title' : 'IPy resolution vs phi',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-Phi-LongTracks-Sigma',
                        'yrange': [-0.02, 0.02],
                        'style':3,
                        'options' : {'showUncertainties' : True}
                        },
                        {
                        'title' : 'IPx mean vs phi',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-Phi-LongTracks-Mean',
                        'yrange': [-0.02, 0.02],
                        'style':3,
                        'options' : {'showUncertainties' : True}
                        },
                        {
                        'title' : 'IPy mean vs phi',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-Phi-LongTracks-Mean',
                        'yrange': [-0.02, 0.02],
                        'style':3,
                        'options' : {'showUncertainties' : True}
                        },
                    ],
                'layout' : (2,2)
                },
                {
                'title' : 'IP resolutions vs eta',
                'plots' : 
                    [
                        {
                        'title' : 'IPx resolution vs eta',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-Eta-LongTracks-Sigma',
                        'yrange': [0., 0.13],
                        'style': 3,
                        'options' : {'showUncertainties' : True}
                        },
                        {
                        'title' : 'IPy resolution vs eta',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-Eta-LongTracks-Sigma',
                        'yrange': [0., 0.13],
                        'style': 3,
                        'options' : {'showUncertainties' : True}
                        },
                        {
                        'title' : 'IPx mean vs eta',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-Eta-LongTracks-Mean',
                        'yrange': [-0.02, 0.02],
                        'style': 3,
                        'options' : {'showUncertainties' : True}
                        },
                        {
                        'title' : 'IPy mean vs eta',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-Eta-LongTracks-Mean',
                        'yrange': [-0.02, 0.02],
                        'style': 3,
                        'options' : {'showUncertainties' : True}
                        }
                     ],
                'layout' : (2,2) 
                }
            ]
        }
    ),
    #==========================================================================
    ('IV', 
        {
        'title': 'IV',
        'plots':
            [
                {
                'title': 'IV',
                'plottables':
                    [
                        {
                        'name': 'Ramp Up',
                        'color': 'b',
                        'style': 2,
                        'legend': 'Ramp Up',
                        'isIV': True,
                        'marker': 'v'
                        },
                        {
                        'name': 'Ramp Down',
                        'color': 'g',
                        'style': 2,
                        'legend': 'Ramp Down',
                        'isIV': True
                        }
                    ],
                'showLegend': True
                },
                {
                'title': 'IV',
                'plottables':
                    [
                        {
                        'name': 'Ramp Up',
                        'color': 'b',
                        'style': 2,
                        'legend': 'Ramp Up',
                        'isIV': True,
                        'sensorShift': 1
                        },
                        {
                        'name': 'Ramp Down',
                        'color': 'g',
                        'style': 2,
                        'legend': 'Ramp Down',
                        'isIV': True,
                        'sensorShift': 1,
                        'marker': 'v'
                        }
                    ]
                },
                {
                'title': 'IV',
                'plottables':
                    [
                        {
                        'name': 'Ramp Up',
                        'color': 'b',
                        'style': 2,
                        'legend': 'Ramp Up',
                        'isIV': True,
                        'sensorShift': 2,
                        },
                        {
                        'name': 'Ramp Down',
                        'color': 'g',
                        'style': 2,
                        'legend': 'Ramp Down',
                        'isIV': True,
                        'sensorShift': 2,
                        'marker': 'v'
                        }
                    ]
                },
                {
                'title': 'IV',
                'plottables':
                    [
                        {
                        'name': 'Ramp Up',
                        'color': 'b',
                        'style': 2,
                        'legend': 'Ramp Up',
                        'isIV': True,
                        'sensorShift': 3
                        },
                        {
                        'name': 'Ramp Down',
                        'color': 'g',
                        'style': 2,
                        'legend': 'Ramp Down',
                        'isIV': True,
                        'sensorShift': 3,
                        'marker': 'v'
                        }
                    ]
                }
            ]
        }
    ),
    #==========================================================================
#     ('graphics_examples',
#         {
#         'title': 'Graphics Examples',
#         'plots': 
#             [
#                 {
#                 'title': 'Normalised contours of possible line styles with legend',
#                 'plottables': 
#                     [
#                         {
#                         'name': 'Velo/VeloTrackMonitor/Track_radc', 
#                         'color':'y', 
#                         'style':0, 
#                         'legend': '0',
#                         'normalise': True
#                         },
#                         {
#                         'name': 'Velo/VeloTrackMonitor/Track_phiadc', 
#                         'style':1,
#                         'legend': '1',
#                         'normalise': True
#                         },
#                         {
#                         'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value sensor {0}',
#                         'legend': '2',
#                         'style': 2,
#                         'normalise': True
#                         },
#                         {
#                         'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value fit function sensor {0}',
#                         'legend': '4',
#                         'color':'k',
#                         'style': 2,
#                         'normalise': True
#                         },
#                         {
#                         'name': 'Velo/VeloTrackMonitor/Track_Theta',
#                         'legend': '2',
#                         'style': 2,
#                         'normalise': True
#                         }
#                     ],
#                 'showLegend': True
#                 }
#             ]
#         }
#     )
    #==========================================================================
])
