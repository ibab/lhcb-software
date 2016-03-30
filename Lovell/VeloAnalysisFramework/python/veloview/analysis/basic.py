"""This module contains simple comparison functions"""

from ..core.interface import ComparisonFunction
from ..core.score_manipulation import Score

class ReturnAlwaysHighScore(ComparisonFunction):
    @classmethod
    def compare(cls, data_hist, ref_hist, comp_arg):
        if cls.check_if_two_hists_exist(data_hist, ref_hist):
            from random import randint
            return Score(randint(80, 100))
        else:
            return cls.create_error_dict()


class ReturnAlwaysLowScore(ComparisonFunction):
    @classmethod
    def compare(cls, data_hist, ref_hist, compare_arg):
        if cls.check_if_two_hists_exist(data_hist, ref_hist):
            from random import randint
            return Score(randint(0, 20))
        else:
            return cls.create_error_dict()

