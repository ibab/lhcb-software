"""GUI run view configuration."""
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
                'sensor_dependent': True
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
        ]
    }),
    ('common_mode', {
        'title': 'Common mode'
    }),
    ('noise', {
        'title': 'Noise',
        'plots': [
            {
                'title': 'RMS noise vs. chip channel',
                'name': 'Vetra/NoiseMon/DecodedADC/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
                'sensor_dependent': True
            },
            {
                'title': 'RMS noise vs. strip',
                'name': 'Vetra/NoiseMon/DecodedADC/TELL1_{0:03d}/RMSNoise_vs_Strip',
                'sensor_dependent': True
            },
        ]
    }),
    ('clusters', {
        'title': 'Clusters',
        'plots': [
            {
                'title': 'Number of VELO clusters per event (Default)',
                'short': 'Clusters per event',
                'name': 'Velo/VeloClusterMonitor/# VELO clusters'
            },
            {
                'title': 'Number of strips per cluster',
                'short': 'Strips per cluster',
                'name': 'Velo/VeloClusterMonitor/Cluster size',
                'options': {
                    'showUncertainties': True
                }
            },
            {
                'title': 'Active chip links versus sensor',
                'short': 'Active links per sensor',
                'name': 'Velo/VeloClusterMonitor/Active chip links vs sensor'
            },
            {
                'title': 'Number of strips per cluster versus sensor',
                'short': 'Strips per cluster vs. sensor',
                'name': 'Velo/VeloClusterMonitor/Cluster size vs sensor'
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
    ('tracks', {
        'title': 'Tracks'
    }),
    ('vertices', {
        'title': 'Vertices'
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
    })
])
