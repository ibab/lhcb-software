# coding=utf-8
"""This module will hold complex comparison functions"""

from ..core.interface import ComparisonFunction, check_hists1, check_hists2
from ..core.score_manipulation import ERROR_LEVELS, Score
from ..utils.rootutils import maximum, min_skip_empty, frac_above_threshold, frac_below_threshold
from logging import debug, getLogger
logger = getLogger(__name__)


class FloorThreshold(ComparisonFunction):
    """Check if any bin is below threshold.

    """

    @classmethod
    @check_hists1
    def compare(cls, data_hist, ref_hist, floor):
        """Reference histogram is ignored.

        data_hist -- data histogram
        ref_hist  -- reference histogram (ignored)
        floor     -- floor threshold

        """

        debug('Min: {}'.format(min_skip_empty(data_hist)))
        if min_skip_empty(data_hist) > floor:
            return cls.create_final_dict(Score(100), ERROR_LEVELS.OK)
        else:
            return cls.create_final_dict(Score(0), ERROR_LEVELS.ERROR)


class CeilingThreshold(ComparisonFunction):
    """Check if any bin is above threshold.

    """

    @classmethod
    @check_hists1
    def compare(cls, data_hist, ref_hist, ceiling):
        """Reference histogram is ignored.

        data_hist -- data histogram
        ref_hist  -- reference histogram (ignored)
        ceiling   -- ceiling threshold

        """

        debug('Max: {}'.format(maximum(data_hist)))
        if maximum(data_hist) < ceiling:
            return cls.create_final_dict(Score(100), ERROR_LEVELS.OK)
        else:
            return cls.create_final_dict(Score(0), ERROR_LEVELS.ERROR)


class MeanWidthDiffRef(ComparisonFunction):
    """Check the mean and width w.r.t. reference.

    All comparisons are done with respect to the reference histogram
    RMS.  A fluctuation of 30% of RMS is allowed for the mean.  For
    the width (RMS) a fluctuation of tolerance*RMS is acceptable.
    Tolerance is a fraction smaller than unity.

    Results of each comparison is treated as a boolean.  They are
    weighted and combined to give the final DQ score.  The weights
    are: mean - 70%, width - 30%.  A score > 70 is OK, >= 30 & < 70 is
    a WARNING, and anything else is an ERROR.  This scheme corresponds
    to passing both comparisons, one of the two, or none.

    """

    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, tolerance):
        """
        data_hist -- data histogram
        ref_hist  -- reference histogram
        tolerance -- tolerance (in fraction)

        """
        dmean = abs(data_hist.GetMean() - ref_hist.GetMean())
        dwidth = abs(data_hist.GetRMS() - ref_hist.GetRMS())
        score = 70.0 * (dmean < abs(0.3*ref_hist.GetRMS()))
        score += 30.0 * (dwidth < abs(tolerance*ref_hist.GetRMS()))
        if score > 70.0:        # both passes: 100
            level = ERROR_LEVELS.OK
        elif score >= 30.0:      # only one passes: 70 or 30
            level = ERROR_LEVELS.WARNING
        else:                   # both fails: 0
            level = ERROR_LEVELS.ERROR
        debug('score: {}, level: {}'.format(score, level))
        return cls.create_final_dict(Score(score), level)


class AbsoluteBandRef(ComparisonFunction):
    """Check if absolute values inside band.

    """
    pass


class ZeroCentredBandRef(ComparisonFunction):
    """Check fraction outside 0-centred band.

    Pass when more than 99% is inside tolerance band.  When comparing
    with reference histogram, use a 3 sigma tolerance band.

    """

    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, abs_band):
        """If abs_band is evaluates to True, reference histogram is ignored.

        data_hist -- data histogram
        ref_hist  -- reference histogram (ignored when abs_band present)
        abs_band  -- one-sided tolerance band width (sigma/RMS)

        """

        if not abs_band:            # 3*sigma tolerance band from reference
            abs_band = 3*ref_hist.GetRMS()
        abs_band = abs(abs_band)    # force +ve definite band
        frac_above = frac_above_threshold(data_hist, abs_band)
        frac_below = frac_below_threshold(data_hist, -abs_band)
        frac_outside = frac_above + frac_below
        debug('Fraction outside: {}, above: {}, below: {}'.
              format(frac_outside, frac_above, frac_below))
        if frac_outside > 0.01:
            return cls.create_final_dict(Score(0), ERROR_LEVELS.ERROR)
        else:
            return cls.create_final_dict(Score(100), ERROR_LEVELS.OK)
