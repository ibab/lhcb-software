"""This is an example of a combiner description dictionary (expected state dictionary)"""

import veloview.analysis.compfunctions.basic as basic_functions
import veloview.analysis.compfunctions.complex as complex_functions

EXAMPLE_EVALUATION_DICTIONARY = {
    "PedestalCombiner": basic_functions.ReturnAlwaysHighScore(),
    "OccupanciesCombiner" : basic_functions.ReturnAlwaysHighScore(),
    "NoiseCombiner" : basic_functions.ReturnAlwaysHighScore(),
    "TracksCombiner" : basic_functions.ReturnAlwaysLowScore(),
}