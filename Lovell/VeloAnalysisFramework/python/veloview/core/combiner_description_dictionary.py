"""This is an example of a combiner description dictionary (expected state dictionary)"""

STANDARD_BRANCH_DICT = {"weight": 1.0, "minWW": 5, "minWE": 10, "minEW": 1, "minEE": 2}
STANDARD_LEAF_DICT = {"weight": 1.0, "maxError": 50, "maxWarning": 80, "path": ""}
TELL1_NUMBERS = range(42) + range(64, 106)


def create_combiner_name(name):
    return name + "Combiner"

noise_tell1_hists_path = "Vetra/NoiseMon/DecodedADC/TELL1_{}/RMSNoise_vs_Strip"
noise_tell1_comb_name_scheme = create_combiner_name("NoiseTell1_{}")
crosstalk_hist_path = "Vetra/VeloBeetleHeaderXTalkCorrectionMoni/BHXT_SummaryPlot_1D"
pedestal_subtracted_hist_name = create_combiner_name("PedestalSubtracted")
pedestal_subtracted_hist_path = "Vetra/VeloPedestalSubtractorMoni/Proj_All_Data_Ped"
mean_pedestal_subtracted_hist_name = create_combiner_name("MeanPedestalSubtracted")
mean_pedestal_subtracted_hist_path = "Vetra/VeloPedestalSubtractorMoni/Quality_Check"
clusters_hists_path = "Velo/VeloClusterMonitor/{}"
number_of_clusters_hist_name = create_combiner_name("NumberOfClustersPerEvent")
number_of_clusters_hist_path = clusters_hists_path.format("# VELO clusters")
active_chip_links_hist_name = create_combiner_name("ActiveChipLinksvssensor")
active_chip_links_hist_path = clusters_hists_path.format("Active chip links vs sensor")
cluster_size_hist_name = create_combiner_name("ClusterSize")
cluster_size_hist_path = clusters_hists_path.format("Cluster size")
occupancy_hists_path = "Velo/VeloOccupancyMonitor/{}"
sensor_occupancy_hist_name = create_combiner_name("SensorOccupancy")
sensor_occupancy_hist_path = occupancy_hists_path.format("OccAvrgSens")
occupancy_spectrum_hist_name = create_combiner_name("OccupancySpectrum")
occupancy_spectrum_hist_path = occupancy_hists_path.format("OccSpectAll")


def tell1_formatting(tell1_number):
    """Adds an appropriate number of 0s before tell1 number"""
    if tell1_number < 10:
        tell1_name = "00{}".format(tell1_number)
    elif 10 <= tell1_number < 100:
        tell1_name = "0{}".format(tell1_number)
    else:
        tell1_name = str(tell1_number)

    return tell1_name


def merge_dicts(*dicts):
    """Merges dictionaries and returns a merged one"""
    temp_dict_ = {}
    for dict_ in dicts:
        temp_dict_.update(dict_)

    return temp_dict_


def create_leaf_dict_with_path(path, name=None):
    """Creates a leaf dictionary with a path and a name"""
    specific_leaf_dict = dict(STANDARD_LEAF_DICT)
    specific_leaf_dict["path"] = path
    if name:
        dict_ = {name : specific_leaf_dict}
    else:
        dict_ = specific_leaf_dict

    return dict_


def create_branch_with_tell1s(comb_name_scheme, comb_path_scheme):
    """Creates a branch dictionarie with tell1 leaf dictionaries"""
    dict_ = dict(STANDARD_BRANCH_DICT)
    for number in TELL1_NUMBERS:
        proper_tell1_number = tell1_formatting(number)
        name = comb_name_scheme.format(proper_tell1_number)
        path = comb_path_scheme.format(proper_tell1_number)
        specific_leaf_dict = create_leaf_dict_with_path(path)
        dict_.update({name : specific_leaf_dict})

    return dict_


EXAMPLE_COMBINER_DESCRIPTION_DICTIONARY = {
    "MasterCombiner": {"weight": 1.0, "minWW": 10, "minWE": 25, "minEW": 1, "minEE": 2,
        "NoiseCombiner": create_branch_with_tell1s(noise_tell1_comb_name_scheme, noise_tell1_hists_path),
        "CrosstalkCombiner": create_leaf_dict_with_path(crosstalk_hist_path),
        "PedestalsCombiner": merge_dicts(
                                STANDARD_BRANCH_DICT,
                                create_leaf_dict_with_path(pedestal_subtracted_hist_path, pedestal_subtracted_hist_name),
                                create_leaf_dict_with_path(mean_pedestal_subtracted_hist_path, mean_pedestal_subtracted_hist_name)),
        "ClustersCombiner": merge_dicts(
                                STANDARD_BRANCH_DICT,
                                create_leaf_dict_with_path(number_of_clusters_hist_path, number_of_clusters_hist_name),
                                create_leaf_dict_with_path(active_chip_links_hist_path, active_chip_links_hist_name),
                                create_leaf_dict_with_path(cluster_size_hist_path, cluster_size_hist_name)),
        "OccupanciesCombiner": merge_dicts(
                                STANDARD_BRANCH_DICT,
                                create_leaf_dict_with_path(sensor_occupancy_hist_path, sensor_occupancy_hist_name),
                                create_leaf_dict_with_path(occupancy_spectrum_hist_path, occupancy_spectrum_hist_name)),
    }
}

if __name__ == "__main__":
    print EXAMPLE_COMBINER_DESCRIPTION_DICTIONARY