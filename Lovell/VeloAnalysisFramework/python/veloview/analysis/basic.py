"""This module contains simple comparison functions"""

from ..core.interface import ComparisonFunction
from ..core.score_manipulation import ERROR_LEVELS, Score


class ReturnAlwaysHighScore(ComparisonFunction):
    def compare(self, data_hist, ref_hist, comp_arg):
        if self.check_if_two_hists_exist(data_hist, ref_hist):
            from random import randint
            return self.create_final_dict(Score(randint(80, 100)), ERROR_LEVELS.OK)
        else:
            return self.create_error_dict()


class ReturnAlwaysLowScore(ComparisonFunction):
    def compare(self, data_hist, ref_hist, compare_arg):
        if self.check_if_two_hists_exist(data_hist, ref_hist):
            from random import randint
            return self.create_final_dict(Score(randint(0, 20)), ERROR_LEVELS.ERROR)
        else:
            return self.create_error_dict()
