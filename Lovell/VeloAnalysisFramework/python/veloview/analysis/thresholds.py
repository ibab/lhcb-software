# coding=utf-8
"""This module will hold complex comparison functions"""

import operator

from ..core.interface import ComparisonFunction, check_hists1, check_hists2
from ..core.score_manipulation import Score
from ..utils.rootutils import maximum, min_skip_empty, frac_above_threshold, frac_below_threshold
from logging import debug, getLogger
logger = getLogger(__name__)

def calc_score(value, minimum, maximum, tolerance):
    """Calculate a score smoothed using a certain tolerance."""
    # For now this is linear, maybe really smoothe out in the future
    if minimum is None and maximum is None:
        return Score.MAX
    if value < minimum:
        return max(Score.MIN, (Score.MAX - Score.MIN) * (-minimum + tolerance + value) / tolerance)
    elif value > maximum:
        return max(Score.MIN, (Score.MAX - Score.MIN) * ( maximum + tolerance - value) / tolerance)
    return Score.MAX

class MeanRmsThreshold(ComparisonFunction):
    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, params):
        mode, axis, minMean, maxMean, minRms, maxRms, tolerance = params
        assert(mode in (0,1))

        meanValue = data_hist.GetMean(axis)
        rmsValue = data_hist.GetRMS(axis)
        if mode == 1:
            meanValue -= ref_hist.GetMean(axis)
            rmsValue -= ref_hist.GetRMS(axis)

        meanScore = calc_score(meanValue, minMean, maxMean, tolerance)
        rmsScore  = calc_score(rmsValue,  minRms,  maxRms,  tolerance)
        return Score(min(meanScore, rmsScore))

class MeanThresholdRPhi(ComparisonFunction):
    """Mean calculation for R or Phi only"""
    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, params):
        mode, rPhi, minMean, maxMean, tolerance = params
        assert(mode in (0, 1) and rPhi in ('R', 'Phi'))
        sum = n = 0
        for bin in xrange(data_hist.GetSize()):
            if data_hist.IsBinOverflow(bin) or data_hist.IsBinUnderflow(bin):
                continue
            center = data_hist.GetBinCenter(bin)
            if (rPhi == 'R' and 0 < center < 42) or (rPhi == 'Phi' and 63 < center < 106):
                sum += data_hist.GetBinContent(bin)
                if mode == 1:
                    sum -= ref_hist.GetBinContent(bin)
                n += 1
        return Score(calc_score(1. * sum / n, minMean, maxMean, tolerance))

class MeanThresholdAC(ComparisonFunction):
    """Separate calculations for A and C side"""
    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, params):
        delta = .0000001
        minMean, maxMean = params
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
        scoreA = calc_score(1. * sumA / nA, minMean, maxMean, tolerance)
        scoreC = calc_score(1. * sumC / nC, minMean, maxMean, tolerance)
        return Score(min(scoreA, scoreC))

class CountThreshold(ComparisonFunction):
    """Check how many bins fulfill a criterium.
    mode can be 0 (check data) or 1 (check data - ref)
    """
    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, params):
        operator, minimum, maximum, tolerance, mode = params
        assert(mode in (0, 1))
        n = 0
        for bin in xrange(data_hist.GetSize()):
            dataValue = data_hist.GetBinContent(bin)
            if mode == 0:
                value = dataValue
            else:
                value = dataValue - ref_hist.GetBinContent(bin)

            if data_hist.IsBinOverflow(bin) or data_hist.IsBinUnderflow(bin):
                continue
            if operator(value):
                n += 1

        return Score(calc_score(n, minimum, maximum, tolerance))

class FractionThreshold(ComparisonFunction):
    """Check which fraction of bins fall outside a band.
    mode can be 0 (check data) or 1 (check data - ref)
    """
    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, params):
        mode, minimum, maximum, minFraction, maxFraction, tolerance = params
        n = 0
        frac_above = frac_below = 0.
        if maximum is not None:
            frac_above = frac_above_threshold(data_hist, maximum)
        if minimum is not None:
            frac_below = frac_below_threshold(data_hist, minimum)

        if mode == 1:
            if maximum is not None:
                frac_above -= frac_above_threshold(ref_hist, maximum)
            if minimum is not None:
                frac_below -= frac_below_threshold(ref_hist, minimum)

        return Score(calc_score(frac_below + frac_above, minFraction, maxFraction, tolerance))

class IntegralThreshold(ComparisonFunction):
    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, params):
        minimum, maximum, tolerance, mode = params
        assert(mode in (0, 1))
        if mode == 0:
            integral = data_hist.Integral()
        else:
            integral = data_hist.Integral() - ref_hist.Integral()
        return Score(calc_score(integral, minimum, maximum, tolerance))

class MeanWidthDiffRef(ComparisonFunction):
    """Check the mean and width w.r.t. reference.

    All comparisons are done with respect to the reference histogram
    RMS.  A fluctuation of 30% of RMS is allowed for the mean.  For
    the width (RMS) a fluctuation of tolerance*RMS is acceptable.
    Tolerance is a fraction smaller than unity.

    Results of each comparison is treated as a boolean. They are weighted and
    combined to give the final DQ score. The weights are: mean - 70%, width -
    30%. This scheme corresponds to passing both comparisons, one of the two,
    or none.
    """

    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, params):
        """
        data_hist -- data histogram
        ref_hist  -- reference histogram
        params    -- [tolerance (between 0 and 1)]
        """
        tolerance = params[0]
        dmean = abs(data_hist.GetMean() - ref_hist.GetMean())
        dwidth = abs(data_hist.GetRMS() - ref_hist.GetRMS())
        score = 70.0 * (dmean < abs(0.3*ref_hist.GetRMS()))
        score += 30.0 * (dwidth < abs(tolerance*ref_hist.GetRMS()))
        debug('score: {}'.format(score))
        return Score(score)

class DifferenceRms(ComparisonFunction):
    """Calculate the RMS of the difference between data and ref"""

    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, params):
        axis, minimum, maximum, tolerance = params

        data_copy = data_hist.Clone()
        data_copy.Add(ref_hist, -1.)
        rms = data_copy.GetRMS(axis) #TODO not ok! needs to be compared in each dimension
        return Score(calc_score(rms, minimum, maximum, tolerance))
