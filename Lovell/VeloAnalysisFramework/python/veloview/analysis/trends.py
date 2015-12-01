# coding=utf-8
"""This module provides functions for trending information"""

from ..core.interface import ValueFunction

class Mean(ValueFunction):
    @classmethod
    def compare(cls, data_hist, ref_hist, param):
        """Get mean"""
        return cls.create_final_dict(data_hist.GetMean())

class Variance(ValueFunction):
    @classmethod
    def compare(cls, data_hist, ref_hist, param):
        """Get mean"""
        return cls.create_final_dict(data_hist.GetRMS())


#class Median(ValueFunction): # Median
#class MPV(ValueFunction): # Most probable value
#class Landau(ValueFunction): # Landau fit parameters
