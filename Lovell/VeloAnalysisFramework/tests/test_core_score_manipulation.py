#!/usr/bin/env python
"""This module will test the scores_manipulation module"""

import os

if __name__ == '__main__':
    import sys

    # fiddle with sys.path so that package is importable
    if __file__.startswith('/'):
        sys.path.insert(0, os.path.join('/', *__file__.split('/')[:-3]))
    else:
        __path_to_script__ = __file__.split('/')[:-1]  # test directory
        __path_to_script__ += ['..', '..'] # package directory parent
        sys.path.insert(0, os.path.join(os.getcwd(), *__path_to_script__))

import unittest

from veloview.core.score_manipulation import Score
from veloview.core.exceptions import (ScoreAssignmentException,
                                      AddingScoreException)

score_for_check = 93.0
score_for_check1 = 25.0
score_for_check2 = 70.0
weight_for_check = 0.3
weight_for_check1 = 1.0
score_for_check_exc = 101.0

class TestScoreManipulation(unittest.TestCase):
    def test_score_init(self):
        string_for_check = "{:.2f}%".format(score_for_check)

        tested_score = Score(score_for_check)

        self.assertEqual(tested_score.value, score_for_check)
        self.assertEqual(str(tested_score), string_for_check)

    def test_score_add(self):
        score_for_comparison = Score(score_for_check1 + score_for_check2)

        score1 = Score(score_for_check1)
        score2 = Score(score_for_check2)
        score3 = score1 + score2

        self.assertEqual(score3, score_for_comparison)

    def test_score_iadd(self):
        score_for_comparison = Score(score_for_check1 + score_for_check2)
        score1 = Score(score_for_check1)
        score1 += Score(score_for_check2)

        self.assertEqual(score1, score_for_comparison)

    def test_score_mul(self):
        score_for_comparison = Score(score_for_check * weight_for_check)
        score1 = Score(score_for_check)
        weight1 = weight_for_check
        score1 = score1 * weight1

        self.assertEqual(score1, score_for_comparison)

    def test_score_imul(self):
        score_for_comparison = Score(score_for_check * weight_for_check)
        score1 = Score(score_for_check)
        weight1 = weight_for_check
        score1 *= weight1

        self.assertEqual(score1, score_for_comparison)

    def test_score_div(self):
        score_for_comparison = Score(score_for_check / weight_for_check1)
        score1 = Score(score_for_check)
        weight1 = weight_for_check1
        score1 = score1 / weight1

        self.assertEqual(score1, score_for_comparison)

    def test_score_idiv(self):
        score_for_comparison = Score(score_for_check / weight_for_check1)
        score1 = Score(score_for_check)
        weight1 = weight_for_check1
        score1 /= weight1

        self.assertEqual(score1, score_for_comparison)

    def test_score_cmp(self):
        score1 = Score(score_for_check)

        score_thesame = Score(score_for_check)
        score_lower = Score(score_for_check - 1)
        score_higher = Score(score_for_check + 1)

        self.assertEqual(score1, score_thesame)
        self.assertGreater(score1, score_lower)
        self.assertLess(score1, score_higher)

    def test_score_assignment(self):
        with self.assertRaises(ScoreAssignmentException):
            Score(score_for_check_exc)

    def test_score_adding(self):
        with self.assertRaises(AddingScoreException):
            Score(score_for_check) + Score(score_for_check1)


if __name__ == '__main__':
    hdr_fmt = '='*5 + '{0:^{width}}' + '='*5
    print hdr_fmt.format('TestScoreManipulation', width=40)
    from logging import basicConfig, DEBUG, INFO, WARNING
    basicConfig(format='%(levelname)s:%(module)s: %(message)s', level=WARNING)
    unittest.main()
