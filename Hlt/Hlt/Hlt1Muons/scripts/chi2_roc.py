import sys
import os
import argparse
from array import array
from operator import itemgetter
from itertools import product
from collections import defaultdict
from math import sqrt

def is_valid_file(parser, arg):
    if not arg or not os.path.exists(arg):
       parser.error("The file %s does not exist!" % arg)
    else:
       return arg

parser = argparse.ArgumentParser(usage = 'usage: %(prog)s input_file')

parser.add_argument("input_file", nargs = 1, metavar = 'FILE',
                    type = lambda x: is_valid_file(parser, x))
parser.add_argument("-t", "--tree-name", dest = 'tree_name',
                    default = "MatchChi2Algo/match_chi2")
arguments = parser.parse_args()

from ROOT import TFile, TCanvas
from ROOT import TTree, gDirectory

input_file = TFile(arguments.input_file[0])
tree = input_file.Get(arguments.tree_name)
if not tree:
    print 'Could not find tree %s in %s' % (arguments.tree_name, arguments.input_file[0])
    sys.exit(-1)

chi2_bkg = []
chi2_sig = []

for entry in tree:
    if entry.true_muon == 1 and entry.muon_overlap > 0.9:
        chi2_sig.append(entry.chi2DoF)
    else:
        chi2_bkg.append(entry.chi2DoF)

chi2_bkg = sorted(chi2_bkg)
chi2_sig = sorted(chi2_sig)

eff_sig = []
eff_bkg = []
latest = 0.01
j = 0
for i, chi2 in enumerate(chi2_sig):
    es = float(i) / float(len(chi2_sig))
    if es > latest:
        for k in range(j, len(chi2_bkg)):
            if chi2_bkg[k] > chi2_sig[i]:
                eff_sig.append(es)
                eff_bkg.append(1 - float(k) / float(len(chi2_bkg)))
                latest += 0.01
                j = k
                break

eff_sig.append(1.)
eff_bkg.append(0.)

from ROOT import TGraph, kBlue, TCanvas
g = TGraph(len(eff_sig), array('d', eff_sig), array('d', eff_bkg))
g.SetName("ROC")
g.SetTitle("ROC")
g.SetLineColor(kBlue)
g.SetLineWidth(2)
canvas = TCanvas('canvas', 'canvas', 600, 400)
g.Draw("AC")
