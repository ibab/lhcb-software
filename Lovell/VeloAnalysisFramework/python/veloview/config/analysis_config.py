"""
The analysis branches.
Dictionary branch name -> branch, where branch is a dictionary as follows:
    title:    human-readable title (string)
    children: list of child names (strings), each should correspond to a key in
              analysis_config_leaves
    minWW:    minimum nr of warnings in children for this to give a warning (int >= 0, default  1)
    minWE:    minimum nr of warnings in children for this to give an error  (int >= 0, default 25)
    minEW:    minimum nr of errors   in children for this to give a warning (int >= 0, default  1)
    minEE:    minimum nr of errors   in children for this to give an error  (int >= 0, default  2)
    weight:   weight of this node relative to other children of its parent
              (float in range ]0., 1.], default 1.)
"""
analysis_config_branches = {
        'MasterCombiner':
        {
            'title':    'Velo (all)',
            'children': ['pedestals', 'noise', 'crosstalk', 'clusters', 'occupancy', 'bad_channels'],
        },
        'pedestals':
        {
            'title':    'Pedestals',
            'children': ['pedestal_quality'],
            'minEE':    1,
        },
        'noise':
        {
            'title':    'Noise',
            'children': ['noise_per_station', 'noise_per_sensor', 'noise_per_link'],
        },
        'noise_per_station':
        {
            'title':    'Noise per station',
            'children': ['noise_per_station_r', 'noise_per_station_phi'],
        },
        'noise_per_sensor':
        {
            'title':    'Noise per sensor',
            'children': ['noise_per_sensor_direct'],
            'minEE':    1,
        },
        'noise_per_link':
        {
            'title':    'Noise per link',
            'children': ['noise_per_link_direct', 'noise_per_link_ref'],
        },
        'crosstalk':
        {
            'title':    'Crosstalk',
            'children': ['noisy_header_links_direct', 'noisy_header_links_ref'],
        },
        'clusters':
        {
            'title':    'Clusters',
            'children': ['clusters_per_event', 'strips_per_cluster', 'cluster_adc', 'cluster_adc_fit_params'],
        },
        'clusters_per_event':
        {
            'title':    'Clusters per event',
            'children': ['clusters_per_event_mean', 'clusters_per_event_below_200', 'clusters_per_event_mean_ref', 'clusters_per_event_below_200_ref'],
        },
        'strips_per_cluster':
        {
            'title':    'Strips per cluster',
            'children': ['strips_per_cluster_mean', 'strips_per_cluster_mean_ref', 'strips_per_cluster_vs_sensor_diff_rms'],
        },
        'cluster_adc':
        {
            'title':    'Cluster ADC',
            'children': ['cluster_adc_mean', 'cluster_adc_below_20', 'cluster_adc_mean_ref', 'cluster_adc_below_20_ref'],
        },
        'cluster_adc_fit_params':
        {
            'title':    'Cluster ADC Fit params',
            'children': ['cluster_adc_mpv', 'cluster_adc_fwhm'],
        },
        'cluster_adc_mpv':
        {
            'title':    'Cluster ADC Fit param MPV',
            'children': ['cluster_adc_mpv_r', 'cluster_adc_mpv_phi', 'cluster_adc_mpv_r_ref', 'cluster_adc_mpv_phi_ref'],
        },
        'cluster_adc_fwhm':
        {
            'title':    'Cluster ADC Fit param FWHM',
            'children': ['cluster_adc_fwhm_r', 'cluster_adc_fwhm_phi', 'cluster_adc_fwhm_r_ref', 'cluster_adc_fwhm_phi_ref'],
        },
        'occupancy':
        {
            'title':    'Occupancy',
            'children': ['occupancy_avg_r', 'occupancy_avg_phi', 'occupancy_avg_r_ref', 'occupancy_avg_phi_ref'],
        },
        'bad_channels':
        {
            'title':    'Bad channels',
            'children': ['dead_channels_per_sensor_mean', 'dead_channels_per_sensor_mean_ref', 'noisy_channels_per_sensor_integral', 'noisy_channels_per_sensor_integral_ref'],
        },
    }

"""
The analysis leaves.
Dictionary leaf name -> leaf, where leaf is a dictionary as follows:
    title:         human-readable title (string)
    name:          Path to Vetra input histogram. If key tell1 is present, must
                   contain exactly one occurrence of '{}', which will be
                   replaced with a tell1 board name (string)
    sensor_dependent: Whether this variable has one entry per sensor (bool, default False)
    comparison:    One of the ComparisonFunction classes (string)
    params:        Parameters passed to the comparison function (any or None, default None)
    trending:      List of functions to call on the histogram for trending (list)
    errThreshold:  score below this is considered error (int in range
                   [0, warnThreshold], default 50)
    warnThreshold: score below this is considered warning (int in range
                   [errThreshold, 100], default 80)
    weight:        weight of this node relative to other children of its parent
                   (float >= 0)
    minWW, minWE,
    minEW, minEE:  same as for branch, only if tell1 is present.
                   Define error levels for tell1 child leaves
"""
analysis_config_leaves = {
        'pedestal_quality':
        {
            'title': 'Pedestal subtracted ADC profile',
            'name': 'Vetra/VeloPedestalSubtractorMoni/TELL1_{0:03d}/Ped_Sub_ADCs_Profile',
            'sensor_dependent': True,
            'comparison': 'MeanRmsThreshold',
            'params': (0, 1, -1., 1., -1., 1., 1.),
            'trending': ('Mean', 'Rms'),
        },
        'noise_per_station_r':
        {
            'title': 'Noise per R station',
            'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_R',
            'comparison': 'MeanThresholdAC',
            'params': (1, 1., 3., 1.),
            'trending': ('MeanAC',),
        },
        'noise_per_station_phi':
        {
            'title': 'Noise per Phi station',
            'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Station_Phi',
            'comparison': 'MeanThresholdAC',
            'params': (1, 1., 3., 1.),
            'trending': ('MeanAC',),
        },
        'noise_per_sensor_direct':
        {
            'title': 'RMS CMS noise vs. chip channel',
            'name': 'Vetra/NoiseMon/ADCCMSuppressed/TELL1_{0:03d}/RMSNoise_vs_ChipChannel',
            'sensor_dependent': True,
            'comparison': 'MeanWidthDiffRef',
            'params': (1.,),
            'trending': ('Mean', 'Rms'),
        },
        'noise_per_link_direct':
        {
            'title': 'RMS CMS noise vs. link',
            'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Link',
            'comparison': 'CountThreshold',
            'params': (lambda x: x < 0.1, 0, 4, 1, 0),
            #'trending': (('CountBelow', 0.1),)
        },
        'noise_per_link_ref':
        {
            'title': 'RMS CMS noise vs. link reference comparison',
            'name': 'Vetra/NoiseMon/ADCCMSuppressed/Average/RMSNoise_vs_Link',
            'comparison': 'CountThreshold',
            'params': (lambda x: x > 0.5, 0, 4, 1, 1),
        },
        'noisy_header_links_direct':
        {
            'title': 'Noisy header links count',
            'name': 'Vetra/NoiseMon/ADCCMSuppressed/Header/NoisyHeaderLinksPerSensor',
            'comparison': 'IntegralThreshold',
            'params': (200, 500, 50, 0),
        },
        'noisy_header_links_ref':
        {
            'title': 'Noisy header links count reference comparison',
            'name': 'Vetra/NoiseMon/ADCCMSuppressed/Header/NoisyHeaderLinksPerSensor',
            'comparison': 'IntegralThreshold',
            'params': (-10, 10, 5, 1),
        },
        'clusters_per_event_mean':
        {
            'title': 'Mean number of clusters per event',
            'name': 'Velo/VeloPrivateClusterMonitor/# VELO clusters',
            'comparison': 'MeanRmsThreshold',
            'params': (0, 0, 620, 660, None, None, 10),
            'trending': ('Mean',),
        },
        'clusters_per_event_below_200':
        {
            'title': 'Number of events with fewer than 200 clusters',
            'name': 'Velo/VeloPrivateClusterMonitor/# VELO clusters',
            'comparison': 'FractionThreshold',
            'params': (0, None, 200, .01, .05, .01),
           #'trending': (('FractionBelow', 200),)
        },
        'clusters_per_event_mean_ref':
        {
            'title': 'Mean number of clusters per event compared to ref',
            'name': 'Velo/VeloPrivateClusterMonitor/# VELO clusters',
            'comparison': 'MeanRmsThreshold',
            'params': (1, 0, -50, 50, None, None, 10),
        },
        'clusters_per_event_below_200_ref':
        {
            'title': 'Number of events with fewer than 200 clusters compared to ref',
            'name': 'Velo/VeloPrivateClusterMonitor/# VELO clusters',
            'comparison': 'FractionThreshold',
            'params': (1, None, 200, 0., .05, .01),
            #'trending': (('FractionBelow', 200),)
        },
        'strips_per_cluster_mean':
        {
            'title': 'Mean number of strips per cluster',
            'name': 'Velo/VeloPrivateClusterMonitor/# VELO clusters',
            'comparison': 'MeanRmsThreshold',
            'params': (0, 0, 1.6, 2.0, None, None, 0.1),
            'trending': ('Mean',),
        },
        'strips_per_cluster_vs_sensor_diff_rms':
        {
            'title': 'Number of strips per cluster vs sensor RMS of difference with ref',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster size vs sensor',
            'comparison': 'DifferenceRms',
            'params': (0, 0., 0.2, 0.1),
        },
        'cluster_adc_mean':
        {
            'title': 'Cluster ADC value',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value',
            'comparison': 'MeanRmsThreshold',
            'params': (0, 0, 49, 50, None, None, 1),
            'trending': ('Mean',),
        },
        'cluster_adc_below_20':
        {
            'title': 'Fraction of clusters ADC values below 20',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value',
            'comparison': 'FractionThreshold',
            'params': (0, None, 20, .08, .12, .02),
           #'trending': (('FractionBelow', 200),)
        },
        'cluster_adc_mean_ref':
        {
            'title': 'Cluster ADC value compared to ref',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value',
            'comparison': 'MeanRmsThreshold',
            'params': (1, 0, -100, 100, None, None, 30),
        },
        'cluster_adc_below_20_ref':
        {
            'title': 'Fraction of clusters ADC values below 20 compared to ref',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value',
            'comparison': 'FractionThreshold',
            'params': (1, None, 20, 0., .05, .01),
           #'trending': (('FractionBelow', 200),)
        },
        'cluster_adc_mpv_r':
        {
            'title': 'Cluster ADC values fit parameter MPV R mean',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value MPV vs Sensor number',
            'comparison': 'MeanThresholdRPhi',
            'params': (0, 'R', 28, 33, 2),
            'trending': ('MeanRPhi',),
        },
        'cluster_adc_mpv_phi':
        {
            'title': 'Cluster ADC values fit parameter MPV Phi mean',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value MPV vs Sensor number',
            'comparison': 'MeanThresholdRPhi',
            'params': (0, 'Phi', 33, 40, 3),
        },
        'cluster_adc_mpv_r_ref':
        {
            'title': 'Cluster ADC values fit parameter MPV R mean compared to ref',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value MPV vs Sensor number',
            'comparison': 'MeanThresholdRPhi',
            'params': (1, 'R', -1, 1, 1),
        },
        'cluster_adc_mpv_phi_ref':
        {
            'title': 'Cluster ADC values fit parameter MPV Phi mean compared to ref',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value MPV vs Sensor number',
            'comparison': 'MeanThresholdRPhi',
            'params': (1, 'Phi', -1, 1, 1),
        },
        'cluster_adc_fwhm_r':
        {
            'title': 'Cluster ADC values fit parameter FWHM R mean',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value FWHM vs Sensor number',
            'comparison': 'MeanThresholdRPhi',
            'params': (0, 'R', 20, 30, 4),
            'trending': ('MeanRPhi',),
        },
        'cluster_adc_fwhm_phi':
        {
            'title': 'Cluster ADC values fit parameter FWHM Phi mean',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value FWHM vs Sensor number',
            'comparison': 'MeanThresholdRPhi',
            'params': (0, 'Phi', 20, 35, 5),
        },
        'cluster_adc_fwhm_r_ref':
        {
            'title': 'Cluster ADC values fit parameter FWHM R mean compared to ref',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value FWHM vs Sensor number',
            'comparison': 'MeanThresholdRPhi',
            'params': (1, 'R', -1, 1, 1),
        },
        'cluster_adc_fwhm_phi_ref':
        {
            'title': 'Cluster ADC values fit parameter FWHM Phi mean compared to ref',
            'name': 'Velo/VeloPrivateClusterMonitor/Cluster ADC value FWHM vs Sensor number',
            'comparison': 'MeanThresholdRPhi',
            'params': (1, 'Phi', -1, 1, 1),
        },
        'occupancy_avg_r':
        {
            'title': 'Average R sensor occupancy',
            'name': 'Velo/VeloPrivateOccupancyMonitor/OccAvrgSens',
            'comparison': 'MeanThresholdRPhi',
            'params': (0, 'R', .35, .4, .02),
            'trending': ('MeanRPhi',),
        },
        'occupancy_avg_phi':
        {
            'title': 'Average Phi sensor occupancy',
            'name': 'Velo/VeloPrivateOccupancyMonitor/OccAvrgSens',
            'comparison': 'MeanThresholdRPhi',
            'params': (0, 'Phi', .35, .4, .02),
        },
        'occupancy_avg_r_ref':
        {
            'title': 'Average R sensor occupancy compared to ref',
            'name': 'Velo/VeloPrivateOccupancyMonitor/OccAvrgSens',
            'comparison': 'MeanThresholdRPhi',
            'params': (1, 'R', -1, 1, 1),
        },
        'occupancy_avg_phi_ref':
        {
            'title': 'Average Phi sensor occupancy compared to ref',
            'name': 'Velo/VeloPrivateOccupancyMonitor/OccAvrgSens',
            'comparison': 'MeanThresholdRPhi',
            'params': (1, 'Phi', -1, 1, 1),
        },
        'dead_channels_per_sensor_mean':
        { 
            'title': 'Dead Channels Per Sensor',
            'name': 'Velo/VeloPrivateOccupancyMonitor/DeadPerSensor',
            'comparison': 'MeanRmsThreshold',
            'params': (0, 0, 0, 50, None, None, 30),
        },
        'dead_channels_per_sensor_mean_ref':
        { 
            'title': 'Dead Channels Per Sensor',
            'name': 'Velo/VeloPrivateOccupancyMonitor/DeadPerSensor',
            'comparison': 'MeanRmsThreshold',
            'params': (1, 0, -4, 4, None, None, 1),
        },
        'noisy_channels_per_sensor_integral':
        {
            'title': 'Noisy Channels Per Sensor',
            'name': 'Velo/VeloPrivateOccupancyMonitor/NoisyPerSensor',
            'comparison': 'IntegralThreshold',
            'params': (0, 1, 1, 0),
        },
        'noisy_channels_per_sensor_integral_ref':
        {
            'title': 'Noisy Channels Per Sensor compared to ref',
            'name': 'Velo/VeloPrivateOccupancyMonitor/NoisyPerSensor',
            'comparison': 'IntegralThreshold',
            'params': (0, 1, 1, 1),
        },
        'links_with_32_dead_channels':
        {
            'title': 'Links with at least 32 dead channels',
            'name': 'Velo/VeloPrivateOccupancyMonitor/DeadPerSensorAndLink',
            'comparison': 'CountThreshold',
            'params': (lambda x: x > 31.5, 0, 5, 2, 0),
        },
        'links_with_32_dead_channels_ref':
        {
            'title': 'Links with at least 32 dead channels compared to reference',
            'name': 'Velo/VeloPrivateOccupancyMonitor/DeadPerSensorAndLink',
            'comparison': 'CountThreshold',
            'params': (lambda x: x > 31.5, 0, 5, 2, 1),
        },
    }
