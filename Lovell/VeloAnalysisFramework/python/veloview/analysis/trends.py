# coding=utf-8
"""This module provides functions for trending information"""

from ..core.interface import ValueFunction

class Mean(ValueFunction):
    @classmethod
    def calculate(cls, data_hist, params):
        return {"mean": data_hist.GetMean()}
    @classmethod
    def vars(cls):
        return {"mean": float}

class Rms(ValueFunction):
    @classmethod
    def calculate(cls, data_hist, params):
        return {"rms": data_hist.GetRMS()}
    @classmethod
    def vars(cls):
        return {"rms": float}

class MeanRPhi(ValueFunction):
    @classmethod
    def calculate(cls, data_hist, params):
        sumR = nR = sumPhi = nPhi = 0
        for bin in xrange(data_hist.GetSize()):
            if data_hist.IsBinOverflow(bin) or data_hist.IsBinUnderflow(bin):
                continue
            center = data_hist.GetBinCenter(bin)
            if 0 < center < 42:
                sumR += data_hist.GetBinContent(bin)
                nR += 1
            elif 63 < center < 106:
                sumPhi += data_hist.GetBinContent(bin)
                nPhi += 1
        return {"meanR": 1. * sumR / nR, "meanPhi": 1. * sumPhi / nPhi}
    @classmethod
    def vars(cls):
        return {"meanR": float, "meanPhi": float}

class MeanAC(ValueFunction):
    @classmethod
    def calculate(cls, data_hist, params):
        delta = .0000001
        sumA = nA = sumC = nC = 0
        for bin in xrange(data_hist.GetSize()):
            value = data_hist.GetBinContent(bin)
            if data_hist.IsBinOverflow(bin) or data_hist.IsBinUnderflow(bin):
                continue
            if value < -delta:
                sumC += value
                nC += 1
            elif value > delta:
                sumA += value
                nA += 1

        return {"meanA": 1. * sumA / nA, "meanC": 1. * sumC / nC}

    @classmethod
    def vars(cls):
        return {"meanA": float, "meanC": float}


#class Median(ValueFunction): # Median
#class MPV(ValueFunction): # Most probable value
#class Landau(ValueFunction): # Landau fit parameters
