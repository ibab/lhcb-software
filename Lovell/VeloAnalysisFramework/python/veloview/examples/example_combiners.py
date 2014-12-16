"""This module will test the containers module"""
from veloview.analysis.combiners import Combiner
from veloview.core.conf.env.combiner_description_dictionary import EXAMPLE_COMBINER_DESCRIPTION_DICTIONARY
from veloview.core.conf.env.evaluation_dictionary import create_eval_dict


dqmfile = "velodqm.root"


if __name__ == "__main__":
    comb1 = Combiner(EXAMPLE_COMBINER_DESCRIPTION_DICTIONARY, create_eval_dict(), dqmfile, dqmfile)
    comb1.evaluate()
    print comb1