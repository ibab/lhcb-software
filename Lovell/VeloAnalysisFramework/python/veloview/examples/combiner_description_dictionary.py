"""This is an example of a combiner description dictionary (expected state dictionary)"""

EXAMPLE_COMBINER_DESCRIPTION_DICTIONARY = {
    "MasterCombiner": {"weight": 1.0, "minWW": 10, "minWE": 25, "minEW": 1, "minEE": 2,
        "PedestalCombiner":     {"weight": 1.0, "maxError": 50, "maxWarning": 80,
                                 "path": "Vetra/VeloPedestalSubtractorMoni/Proj_All_Data_Ped"},
        "OccupanciesCombiner":  {"weight": 50, "maxError": 50, "maxWarning": 80,
                                 "path": "Velo/VeloOccupancyMonitor/OccAvrgSens"},
        "NoiseCombiner":        {"weight": 0.7, "maxError": 50, "maxWarning": 80,
                                 "path": "Vetra/NoiseMon/DecodedADC/TELL1_36/RMSNoise_vs_Strip"},
        "CrossTalkCombiner":    {"weight": 0.2, "maxError": 50, "maxWarning": 80,
                                 "path": "Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_SummaryPlot_1D"},
    }
}