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
    ('dqs', {
        'title': 'DQS'
    }),
    ('pedestals', {
        'title': 'Pedestals',
        'plots': [
            {
                'title': 'Pedestal bank',
                'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Pedestal_Bank',
                'sensor_dependent': True,
                'tip': 'Test of tipping - Hello world!'
            },
            {
                'title': 'Subtracted ADC profile',
                'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Ped_Sub_ADCs_Profile',
                'sensor_dependent': True
            },
            {
                'title': 'Subtracted ADC 2D',
                'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Ped_Sub_ADCs_2D',
                'sensor_dependent': True
            }
        ],
        'layout': (1, 3)
    }),
    ('noise', {
        'title': 'Noise',
        'subpages': [
            {
                 'title': 'Per Sensor', 
                 'plots': [
                     {
                         'title': 'RMS noise vs. chip channel',
                         'name': 'Vetra/NoiseMon/DecodedADC/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
                         'sensor_dependent': True,
                         'options': {
                              'yAxisMinimum': 0,
                              'yAxisMaximum': 5
                          }
                      },
                     {
                         'title': 'RMS noise vs. strip',
                         'name': 'Vetra/NoiseMon/DecodedADC/TELL1_{0:03d}/RMSNoise_vs_Strip',
                         'sensor_dependent': True,
                         'options': {
                              'yAxisMinimum': 0,
                              'yAxisMaximum': 5
                          }
                      },
                     {
                         'title': 'RMS CMS noise vs. chip channel',
                         'name': 'Vetra/NoiseMon/ADCCMSuppressed/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
                         'sensor_dependent': True,
                         'options': {
                              'yAxisMinimum': 0,
                              'yAxisMaximum': 5
                          }
                      },
                     {
                         'title': 'RMS CMS noise vs. strip',
                         'name': 'Vetra/NoiseMon/ADCCMSuppressed/TELL1_{0:03d}/RMSNoise_vs_Strip',
                         'sensor_dependent': True,
                         'options': {
                              'yAxisMinimum': 0,
                              'yAxisMaximum': 5
                          }
                     },
                 ],
                 'layout': (2, 2)
              },
           {
                 'title': 'Velo Overview',
                 'plots': [
                     {
                         'title': 'RMS CMS noise vs. station / R sensors (neg. for C-side)',
                         'short': 'RMS CMS noise vs. station / R sensors (neg. for C-side)',
                         'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_R',
                         'options': {
                              'yAxisMinimum': -4,
                              'yAxisMaximum': 4
                          } 
                      },
                     {
                         'title': 'RMS CMS noise vs. station / Phi sensors (neg. for C-side)',
                         'short': 'RMS CMS noise vs. station / Phi sensors (neg. for C-side)',
                         'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_Phi',
                         'options': {
                              'yAxisMinimum': -4,
                              'yAxisMaximum': 4
                          }
                      },
                 ],
                 'layout': (2, 1)
              
             },
           {
                 'title': 'Average Distribution',
                 'plots': [
                    {
                         'title': 'RMS CMS noise (R sensors)',
                         'short': 'RMS CMS noise (R sensors)',
                         'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_R',
                         'options': {
                              'yAxisMinimum': 0,
                              'yAxisMaximum': 45
                          } 
                      },
                     {
                         'title': 'RMS CMS noise (Phi sensors)',
                         'short': 'RMS CMS noise (Phi sensors)',
                         'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_Phi',
                         'options': {
                              'yAxisMinimum': 0,
                              'yAxisMaximum': 45
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
                         'short': 'RMS CMS noise vs. link',
                         'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Link',
                         'options': {
                              'zAxisMinimum': 0,
                              'zAxisMaximum': 5
                          }
                      },
                     {
                         'title': 'RMS noise vs. link',
                         'short': 'RMS noise vs. link',
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
    }),
    ('headerxtalk', {
        'title': 'HeaderxTalk',
        'plots': [
           {
               'title': 'RMS Normalised Header noise for all sensors',
               'name': 'Vetra/NoiseMon/DecodedADC/Header/HeaderNoise_allSensors'
           },
           {
               'title': 'RMS NoisyHeaderLinksPerSensor',
               'name': 'Vetra/NoiseMon/DecodedADC/Header/NoisyHeaderLinksPerSensor'
           },
           {
               'title': 'RMS CMS Normalised Header noise for all sensors',
               'name': 'Vetra/NoiseMon/ADCCMSuppressed/Header/HeaderNoise_allSensors'
           },
           {
               'title': 'RMS CMS NoisyHeaderLinksPerSensor',
               'name': 'Vetra/NoiseMon/ADCCMSuppressed/Header/NoisyHeaderLinksPerSensor'
           },
           {
               'title': 'Noise on 1st Channel Minus Average Noise of Link',
               'name':  'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_SummaryPlot_2D'
           },
           {
               'title': 'BHXT_SummaryPlot_1D',
               'name':  'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_SummaryPlot_1D'
           },
           {
               'title': 'Noise difference between the first and the reference channel',
               'name':  'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_DiffPlot_1D'
           },
        ],
        'layout': (2, 2)
    }),
     ('clusters', {
          'title': 'Clusters',
          'subpages': [
              {
                  'title': 'Clusters_1',
                  'plots': [ 
                      {
                          'title': 'Number of VELO clusters per event (Default)',
                          'short': 'Clusters per event',
                          'name': 'Velo/VeloPrivateClusterMonitor/# VELO clusters'
                      },
                      {
                          'title': 'Number of strips per cluster',
                          'short': 'Strips per cluster',
                          'name': 'Velo/VeloPrivateClusterMonitor/Cluster size',
                          'options': {
                              'showUncertainties': True
                          },
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
                  'plots': [ 
                      {
                          'title': 'Cluster ADC value',
                          'short': 'Cluster ADC value',
                          'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value'
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
                          'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value sensor {0}',
                          'fit_name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value fit function sensor {0}',
                          'sensor_dependent': True
                      },
                  ],
                  'layout': (2, 2)
              },
              {
                  'title': 'Clusters_3',
                  'plots': [ 
                      {
                          'title': 'Rate vs DistToM2[um]',
                          'short': 'Rate vs DistToM2[um]',
                          'name': 'Velo/VeloTrackMonitor/Rate_DistToM2',
                          'options': {
                              'yAxisMinimum': 0,
                              'yAxisMaximum': 1
                          }
                      },
                      {
                          'title': 'Rate vs DistToOutStrip[um]',
                          'short': 'Rate vs DistToOutStrip[um]',
                          'name': 'Velo/VeloTrackMonitor/Rate_DistToOutStrip',
                          'options': {
                              'yAxisMinimum': 0,
                              'yAxisMaximum': 1
                          }
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
     }),
    ('occupancy', {
        'title': 'Occupancy',
        'plots': [
            {
                'title': 'Channel occupancy',
                'name': 'Velo/VeloOccupancyMonitor/OccPerChannelSens{0}',
                'sensor_dependent': True
            },
            {
                'title': 'Average sensor occupancy',
                'name': 'Velo/VeloOccupancyMonitor/OccAvrgSens'
            },
            {
                'title': 'Occupancy spectrum (zoom)',
                'short': 'Occupancy spectrum',
                'name': 'Velo/VeloOccupancyMonitor/OccSpectMaxLow'
            },
            {
                'title': '% VELO occupancy vs. LHC bunch ID (A side)',
                'short': 'Occupancy vs. BCID (A side)',
                'name': 'Velo/VeloOccupancyMonitor/h_veloOccVsBunchId_ASide'
            },
            {
                'title': '% VELO occupancy vs. LHC bunch ID (C side)',
                'short': 'Occupancy vs. BCID (C side)',
                'name': 'Velo/VeloOccupancyMonitor/h_veloOccVsBunchId_CSide'
            }
        ]
    }),
    ('bad_channels', {
        'title': 'Bad Channels',
        'subpages': [
            {
                'title': 'Info Per Sensor',
                'plots': [
                    {
                        'title': 'Dead Channels Per Sensor',
                        'short': 'Dead Channels Per Sensor',
                        'name': 'Velo/VeloOccupancyMonitor/DeadPerSensor',
                    },
                    {
                        'title': 'Noisy Channels Per Sensor',
                        'short': 'Noisy Channels Per Sensor',
                        'name': 'Velo/VeloOccupancyMonitor/NoisyPerSensor',
                        },
                    ],
                'layout': (1, 2)
                },
            {
                'title': 'Info Per Sensor and Link',
                'plots': [
                    {
                        'title': 'Dead Channels Per Sensor And Link',
                        'short': 'Dead Channels Per Sensor And Link',
                        'name': 'Velo/VeloOccupancyMonitor/DeadPerSensorAndLink',
                    },
                    {
                        'title': 'Noisy Channels Per Sensor And Link',
                        'short': 'Noisy Channels Per Sensor And Link',
                        'name': 'Velo/VeloOccupancyMonitor/NoisyPerSensorAndLink',
                    },
                    ],
                'layout': (1, 2)
                },
            {
                'title': 'Dead/Noisy',
                'plots': [
                    {
                        'title': 'Dead/Noisy Channels Per Sensor',
                        'short': 'Dead/Noisy Channels Per Sensor',
                        'name': 'Velo/VeloOccupancyMonitor/DeadOrNoisyPerSensor',
                    },
                    {
                        'title': 'Dead/Noisy Channels Per Sensor And Link',
                        'short': 'Dead/Noisy Channels Per Sensor And Link',
                        'name': 'Velo/VeloOccupancyMonitor/DeadOrNoisyPerSensorAndLink',
                    },
                ],
                'layout': (1, 2)
            }
        ]
    }),
    ('tracks', {
        'title': 'Tracks',
        'plots': [
          {
            'title': 'Number of clusters associated to a track',
            'name': 'Velo/VeloTrackMonitor/NMeasurements',
            'options' : {'asPoints': True, 'yAxisZeroSuppressed': False}, 
          },
          {
            'title': 'Pseudo efficiency by interpolation',
            'name': 'Velo/VeloTrackMonitor/Track_pseudoEfficiencyByInterp',
            'options' : {'asPoints': True, 'yAxisZeroSuppressed': False}, 
          },
          {
            'title': 'ADC for R clusters associated to a track',
            'name': 'Velo/VeloTrackMonitor/Track_radc',
            'options' : {'asPoints': True, 'yAxisZeroSuppressed': False}, 
          },
          {
            'title': 'ADC for Phi clusters associated to a track',
            'name': 'Velo/VeloTrackMonitor/Track_phiadc',
            'options' : {'asPoints': True, 'yAxisZeroSuppressed': False}, 
          },
          {
            'title': 'Track polar angle theta (degrees)',
            'name': 'Velo/VeloTrackMonitor/Track_Theta',
            'options' : {'asPoints': True, 'yAxisZeroSuppressed': False}, 
          },
          {
            'title': 'Track pseudo rapidity eta',
            'name': 'Velo/VeloTrackMonitor/Track_Eta',
            'options' : {'asPoints': True, 'yAxisZeroSuppressed': False}, 
          },
          {
            'title': 'Track aimuth phi (degrees)',
            'name': 'Velo/VeloTrackMonitor/Track_Phi',
            'options' : {'asPoints': True, 'yAxisZeroSuppressed': False}, 
          },
        ]
    }),
    ('vertices', {
        'title': 'Vertices',
        'subpages' : [
          {'title' : 'PV Overview',
            'plots' : [
              {
                'title': 'PV left-right delta x',
                'name': 'Track/TrackVertexMonitor/PV left-right delta x',
                'options': {'asPoints': False, 'yAxisZeroSuppressed': False}
              },
              {
                'title': 'PV x position',
                'name': 'Track/TrackVertexMonitor/PV x position',
                'options': {'asPoints': False, 'yAxisZeroSuppressed': False}
              },
              {
                'title': 'PV y position',
                'name': 'Track/TrackVertexMonitor/PV y position',
                'options': {'asPoints': False, 'yAxisZeroSuppressed': False}
              },
              {
                'title': 'PV z position',
                'name': 'Track/TrackVertexMonitor/PV z position',
                'options': {'asPoints': False, 'yAxisZeroSuppressed': False}
              }
            ],
            'layout': (2,2)
          },
          {'title' : 'PV VELO Half Distance',
            'plots' : [
              {
                'title': 'PV left-Left half x',
                'name': 'Track/TrackVertexMonitor/PV left-Left half x',
                'options': {'asPoints': False, 'yAxisZeroSuppressed': False}
              },
              {
                'title': 'PV right-Right half x',
                'name': 'Track/TrackVertexMonitor/PV right-Right half x',
                'options': {'asPoints': False, 'yAxisZeroSuppressed': False}
              },
              {
                'title': 'PV left-Left half y',
                'name': 'Track/TrackVertexMonitor/PV left-Left half y',
                'options': {'asPoints': False, 'yAxisZeroSuppressed': False}
              },
              {
                'title': 'PV right-Right half y',
                'name': 'Track/TrackVertexMonitor/PV right-Right half y',
                'options': {'asPoints': False, 'yAxisZeroSuppressed': False}
              }
            ],
            'layout': (2,2)
          }
        ]
    }),
    ('errors', {
        'title': 'Errors',
        'plots': [
            {
                'title': 'Errors per sensor',
                'name': 'Velo/ErrorMon/Error Counter'
            }
        ]
    }),
    ('sensor_overview', {
        'title': 'Sensor overview'
    }),
    # IP plots
    ('IP resolutions', {
        'title' : 'IP resolutions',
        'subpages' : [
            {'title' : 'IP resolutions vs 1/pT',
             'plots' : [
                    {
                        'title' : 'IPx resolution vs 1/pT', # ROOT style formatting doesn't work, nor does Latex.
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-InversePT-LongTracks-Sigma',
                        'options' : {'showUncertainties' : True, 'asPoints' : True, # Doesn't currently seem to do anything.
                                     'yAxisMinimum' : 0., 'yAxisMaximum' : 0.1}
                        # Would be good to be able to set x & y titles, needs to be done in histogramming algo.
                    },
                    {
                        'title' : 'IPy resolution vs 1/pT',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-InversePT-LongTracks-Sigma',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : 0., 'yAxisMaximum' : 0.1}
                    },
                    {
                        'title' : 'IPx mean vs 1/pT',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-InversePT-LongTracks-Mean',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : -0.02, 'yAxisMaximum' : 0.02}
                    },
                    {
                        'title' : 'IPy mean vs 1/pT',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-InversePT-LongTracks-Mean',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : -0.02, 'yAxisMaximum' : 0.02}
                    },
                    ],
             'layout' : (2,2)},
            {'title' : 'IP resolutions vs phi',
             'plots' : [
                    {
                        'title' : 'IPx resolution vs phi',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-Phi-LongTracks-Sigma',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : 0., 'yAxisMaximum' : 0.1}
                    },
                    {
                        'title' : 'IPy resolution vs phi',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-Phi-LongTracks-Sigma',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : 0., 'yAxisMaximum' : 0.1}
                    },
                    {
                        'title' : 'IPx mean vs phi',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-Phi-LongTracks-Mean',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : -0.02, 'yAxisMaximum' : 0.02}
                    },
                    {
                        'title' : 'IPy mean vs phi',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-Phi-LongTracks-Mean',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : -0.02, 'yAxisMaximum' : 0.02}
                    },
                    ],
             'layout' : (2,2) },
            {'title' : 'IP resolutions vs eta',
             'plots' : [
                    {
                        'title' : 'IPx resolution vs eta',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-Eta-LongTracks-Sigma',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : 0., 'yAxisMaximum' : 0.13}
                    },
                    {
                        'title' : 'IPy resolution vs eta',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-Eta-LongTracks-Sigma',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : 0., 'yAxisMaximum' : 0.13}
                    },
                    {
                        'title' : 'IPx mean vs eta',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPX-Vs-Eta-LongTracks-Mean',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : -0.02, 'yAxisMaximum' : 0.02}
                    },
                    {
                        'title' : 'IPy mean vs eta',
                        'name' : 'Velo/VeloIPResolutionMonitor/IPY-Vs-Eta-LongTracks-Mean',
                        'options' : {'showUncertainties' : True, 'asPoints' : True,
                                     'yAxisMinimum' : -0.02, 'yAxisMaximum' : 0.02}
                    }

                ],
        'layout' : (2,2) }
            ]
    }),
])
