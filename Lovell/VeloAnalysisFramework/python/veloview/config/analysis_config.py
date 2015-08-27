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
            'children': ['crosstalk', 'pedestals', 'clusters', 'noise'],
        },
        'pedestals':
        {
            'title':    'Pedestals',
            'children': ['pedestal_quality', 'pedestal_data'],
        },
        'clusters':
        {
            'title':    'Clusters',
            'children': ['cluster_count', 'cluster_active_chips', 'cluster_size'],
        },
        'noise':
        {
            'title':    'Noise',
            'children': ['noise_r', 'noise_phi'],
        },
    }

"""
The analysis leaves.
Dictionary leaf name -> leaf, where leaf is a dictionary as follows:
    title:         human-readable title (string)
    input:         Path to Vetra input histogram. If key tell1 is present, must
                   contain exactly one occurrence of '{}', which will be
                   replaced with a tell1 board name (string)
    tell1:         Which tell1 boards to loop over: R, Phi or Both (optional,
                   string)
    test:          One of FloorThreshold, CeilingThreshold, MeanWidthDiffRef,
                   AbsoluteBandRef, ZeroCentredBandRef, KolmogorovSmirnovTest,
                   Chi2Test (string)
    testarg:       argument passed to the test function (any or None,
                   default None)
    errThreshold:  score below this is considered error (int in range
                   [0, warnThreshold], default 50)
    warnThreshold: score below this is considered warning (int in range
                   [errThreshold, 100], default 80)
    weight:        weight of this node relative to other children of its parent
                   (float >= 0)
    minWW, minWE, minEW, minEE: same as for branch, only if tell1 is present.
                   Define error levels for tell1 child leaves
"""
analysis_config_leaves = {
        'crosstalk':
        {
            'title':         'Crosstalk',
            'input':         'Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_SummaryPlot_1D',
            'test':          'Chi2Test',
            'testarg':       'chi2/ndf',
        },
        'pedestal_quality':
        {
            'title':         'Pedestal Quality Check',
            'input':         'Vetra/VeloPedestalSubtractorMoni/Quality_Check',
            'test':          'MeanWidthDiffRef',
            'testarg': 1.,
        },
        'pedestal_data':
        {
            'title':         'Pedestal Proj Data',
            'input':         'Vetra/VeloPedestalSubtractorMoni/Proj_All_Data_Ped',
            'test':          'Chi2Test',
            'testarg':       'chi2/ndf',
        },
        'cluster_count':
        {
            'title':         'Number of Clusters per Event',
            'input':         'Velo/VeloClusterMonitor/# VELO clusters',
            'test':          'Chi2Test',
            'testarg':       'chi2/ndf',
        },
        'cluster_active_chips':
        {
            'title':         'Number of Active Chip Links per Sensor ',
            'input':         'Velo/VeloClusterMonitor/Active chip links vs sensor',
            'test':          'Chi2Test',
            'testarg':       'chi2/ndf',
        },
        'cluster_size':
        {
            'title':         'Cluster Size',
            'input':         'Velo/VeloClusterMonitor/Cluster size',
            'test':          'Chi2Test',
        },
        'noise_r':
        {
            'title':         'Noise (R)',
            'input':         'Vetra/NoiseMon/ADCCMSuppressed/{}/RMSNoise_vs_Strip',
            'tell1':         'R',
            'test':          'MeanWidthDiffRef',
            'testarg':        0.1,
            'weight':         0.5,
        },
        'noise_phi':
        {
            'title':         'Noise (Phi)',
            'input':         'Vetra/NoiseMon/ADCCMSuppressed/{}/RMSNoise_vs_Strip',
            'tell1':         'Phi',
            'test':          'MeanWidthDiffRef',
            'testarg':        0.1,
            'weight':         0.5,
        },
    }

"""
        'noise_floor':
        {
            'title':         'Noise lower bound',
            'input':         'Vetra/NoiseMon/ADCCMSuppressed/AvgRMSNoise_trend',
            'test':          'FloorThreshold',
            'testarg':        1.5,
        },
        'noise_ceiling':
        {
            'title':         'Noise upper bound',
            'input':         'Vetra/NoiseMon/ADCCMSuppressed/AvgRMSNoise_trend',
            'test':          'CeilingThreshold',
            'testarg':        2.5,
        },
        'noise_rphi':
        {
            'title':         'Noise (all)',
            'input':         'Vetra/NoiseMon/ADCCMSuppressed/AvgRMSNoise_all',
            'test':          'MeanWidthDiffRef',
            'testarg':        0.1,
        },
"""
