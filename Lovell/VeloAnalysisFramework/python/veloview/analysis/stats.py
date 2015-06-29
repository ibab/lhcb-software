# coding=utf-8
"""This module will hold complex comparison functions"""

from ..core.interface import ComparisonFunction, check_hists2, check_binning
from ..core.score_manipulation import ERROR_LEVELS, Score
from logging import debug, getLogger
logger = getLogger(__name__)


class KolmogorovSmirnovTest(ComparisonFunction):
    """Kolmogorov-Smirnov comparison between data and reference histograms.

    By default base comparison on probability.  Probability of 0.01 or
    above are accepted as OK, anything below is flagged as ERROR.  The
    corresponding score is obtained by remapping the probability to a
    range of 100 - 0.

    Note this comparison is useful for arbitrary (unspecified)
    underlying true distributions.  If you know them to be Normal, use
    Chi2Test instead.

    Note: comparison based on KS distance is not yet implemented.

    """

    @classmethod
    @check_hists2
    def compare(cls, data_hist, ref_hist, options=''):
        """
        data_hist -- data histogram
        ref_hist  -- reference histogram
        options   -- options for TH1::KolmogorovTest()

        """

        options = options.lower()
        KS_prob_or_dist = ref_hist.KolmogorovTest(data_hist, options)

        if (not options) or options == '':
            _msg = 'probability'
        elif options.find('m'):
            _msg = 'distance'
        else:
            # FIXME is this a valid `else` message?
            _msg = 'unknown'
        debug('{}: {}'.format(_msg, KS_prob_or_dist))

        if options.find('m') < 0: # KS probability
            # probability remapped such than 0.05 -> 5, 1 -> 100
            score = KS_prob_or_dist * 100
            debug('raw score: {}'.format(score))
            score = Score(score)
            if KS_prob_or_dist < 0.01:
                lvl = ERROR_LEVELS.ERROR
            else:
                lvl = ERROR_LEVELS.OK
        else:
            raise NotImplementedError('KS distance option not implemented yet')
        debug('level: {}'.format(lvl))

        return cls.create_final_dict(score, lvl)


class Chi2Test(ComparisonFunction):
    """χ² comparison test between data and reference histograms.

    If no option is passed, a p-value is used to do the comparison.
    ROOT calculates this from the cumulative χ² distribution,
    precisely: 1 - ∫f(χ²,ndf)dχ² (ndf = number of degrees of freedom).
    So we accept all p-values ≥ 0.05 (a 2σ deviation).

    Approximately the above corresponds to χ²/ndf ≤ 2.  If we want to
    enforce this precisely, we have to pass options 'chi2' or
    'chi2/ndf' (the latter is preferred, since it accounts for empty
    bins).

    The threshold χ²/ndf or p-value is mapped to a DQ score of 50 and
    5 respectively; a perfect match (OK) is 100.  For a p-value based
    comparison, a DQ score between 5 and 1 is a WARNING, and a score
    below 1 (i.e. outside 3σ) is an ERROR.  Similarly for a χ² based
    comparison, a score between 50 and 25 is a WARNING, and a score
    below 25 is (χ²/ndf > 3, or 3σ deviation) is an ERROR.

    """

    @classmethod
    @check_hists2
    @check_binning
    def compare(cls, data_hist, ref_hist, options):
        """
        data_hist -- data histogram
        ref_hist  -- reference histogram (ignored)
        options   -- options for TH1::Chi2Test()

        """

        options = options.lower()
        pvalue_or_chi2 = ref_hist.Chi2Test(data_hist, options)

        if (not options) or options == '':
            _msg = 'pvalue'
        else:
            _msg = options
        debug('{}: {}'.format(_msg, pvalue_or_chi2))

        if options.find('chi2') < 0: # p-value
            # p-value remapped such than 0.05 -> 5, 1 -> 100
            score = pvalue_or_chi2 * 100
            debug('raw score: {}'.format(score))
            score = Score(score)
            if pvalue_or_chi2 < 0.01: # outside 3 sigma
                lvl = ERROR_LEVELS.ERROR
            elif pvalue_or_chi2 < 0.05:
                lvl = ERROR_LEVELS.WARNING
            else:
                lvl = ERROR_LEVELS.OK
        else:                   # chi2
            if options.find('chi2/ndf') < 0: # chi2
                ndf = data_hist.GetNbinsX()
                pvalue_or_chi2 = pvalue_or_chi2/ndf
                debug('ndf: {}'.format(ndf))
            # chi2/ndf = 2 --> Score(50),  0 --> Score(100)
            score = 100 - pvalue_or_chi2*25
            debug('raw score: {}'.format(score))
            if score > 0:
                score = Score(score)
            else:
                score = Score(0)
            if pvalue_or_chi2 > 3: # < Score(25)
                lvl = ERROR_LEVELS.ERROR
            elif pvalue_or_chi2 > 2: # < Score(50)
                lvl = ERROR_LEVELS.WARNING
            else:
                lvl = ERROR_LEVELS.OK
        debug('level: {}'.format(lvl))

        return cls.create_final_dict(score, lvl)
