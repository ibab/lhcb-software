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
arguments = parser.parse_args()

from ROOT import TGraphErrors
from ROOT import TFile, TF2, gDirectory, TF1, TH1D, TCanvas, TH2D
from ROOT import kBlue, kGreen, kPink
from ROOT import RooRealVar, RooArgSet, RooDataSet, RooFit
from ROOT import RooGaussian, RooAddModel, RooArgList, RooFormulaVar
from ROOT import RooCategory

zMagnet = 'zTrueX'
input_file = TFile(arguments.input_file[0])
tree = input_file.Get("MatchResidualAlgo/match_zerr")

q = RooCategory('q', 'q')
q.defineType('minux', -1)
q.defineType('plus', 1)
q.defineType('neutral', 0)
res_x = RooRealVar('res_x', 'res_x', -1000, 1000)
res_y = RooRealVar('res_y', 'res_y', -1000, 1000)
pull_x = RooRealVar('pull_x', 'pull_x', -20, 20)
pull_y = RooRealVar('pull_y', 'pull_y', -20, 20)
p_match = RooRealVar('p_match', 'p_{m}', 3., 50.)
match_res_x = RooRealVar('match_res_x', 'match_res_x', -1000, 1000)
match_res_y = RooRealVar('match_res_y', 'match_res_y', -1000, 1000)
match_pull_x = RooRealVar('match_pull_x', 'match_pull_x', -10, 10.)
match_pull_x.setRange('narrow', -2, 2)
match_pull_x.setRange('wide', match_pull_x.getMin(), match_pull_x.getMax())
match_pull_y = RooRealVar('match_pull_y', 'match_pull_y', -10, 10.)
obs = RooArgSet(q, res_x, res_y, pull_x, pull_y, match_res_x, match_res_y, match_pull_x, match_pull_y)

pull_xm = RooRealVar('dx', 'dx', -40, 40)
pull_ym = RooRealVar('dy', 'dy', -40, 40)
p_inv = RooRealVar('p_inv', '1/p', 0., 0.33)
p     = RooRealVar('p', 'p', 3., 50.)
obs_m = RooArgSet(q, pull_xm, pull_ym, p_inv, p)

pull_cor_true = {1 : 0.,
                 2 : 0.,
                 3 : 0.,
                 4 : 0.}
# pull_cor_true = {1 : 3.788,
#                  2 : 2.329,
#                  3 : 0.846,
#                  4 : 0.}
res_cor_true = {1 : 0.,
                2 : 0.,
                3 : 0.,
                4 : 0.}
pull_cor_match = {1 : 0., # {1 : 2.37,
                  2 : 0., #  2 : 0.69,
                  3 : 0., #  3 : -0.57,
                  4 : 0.} #  4 : -1.96}
res_cor_match = {1 : 0,
                 2 : 0,
                 3 : 0,
                 4 : 0}

ds_res = {k : RooDataSet("ds_M%d" % (k + 1), "ds_M%d" % (k + 1), obs) for k in range(1, 5)}
for entry in tree:
    if entry.p < 3000:
        continue
    for i in range(entry.n_hits):
        q.setIndex(entry.q)
        s = entry.res_s[i]
        res_x.setVal(entry.res_x[i] - entry.q * res_cor_true[s])
        res_y.setVal(entry.res_y[i])
        pull_x.setVal(entry.res_x[i] / (2 * entry.res_dx[i]) - entry.q * pull_cor_true[s])
        pull_y.setVal(entry.res_y[i] / (2 * entry.res_dy[i]))
        match_pull_x.setVal(entry.match_res_x[i] / (2 * entry.res_dx[i])  - entry.q * pull_cor_match[s])
        match_pull_y.setVal(entry.match_res_y[i] / (2 * entry.res_dy[i]))
        match_res_x.setVal(entry.match_res_x[i] - entry.q * res_cor_match[s])
        match_res_y.setVal(entry.match_res_y[i])
        ds_res[s].add(obs)

x_obs = match_pull_x
y_obs = match_pull_y

models = {}
gauss_models = {}
canvases = {}
__objects = []
for o, d in zip((x_obs, y_obs, pull_xm, pull_ym), ('x', 'y', 'xm', 'ym')):
    tag = '_' + d
    mean1 = RooRealVar('mean1' + tag, 'mean1' + tag, 0, -10, 10)
    s1 = RooRealVar('s1' + tag, 's1' + tag, 10, 0, 20)
    s2 = RooRealVar('s2' + tag, 's2' + tag, 15, 0, 40)
    frac = RooRealVar('f' + tag, 'f' + tag, 0.2, 0.001, 0.499)
    __objects += [mean1, s1, s2, frac]
    sm = RooRealVar('sm' + tag, 'sm' + tag, 1, 0.02, 20)
    ss = RooRealVar('ss' + tag, 'ss' + tag, 1, 0.01, 19)
    sf1 = RooFormulaVar("sf1" + tag, "sf1" + tag, "- sqrt(@0 / (1 - @0)) * @1 + @2", RooArgList(frac, ss, sm))
    sf2 = RooFormulaVar("sf2" + tag, "sf2" + tag, "sqrt((1 - @0) / @0) * @1 + @2", RooArgList(frac, ss, sm))
    __objects += [sm, ss, sf1, sf2]
    # else:
    #     mean2 = RooRealVar('mean2' + tag, 'mean2' + tag, 0, -10, 10)
    #     __objects += [mean2]
    gauss = RooGaussian("gauss" + tag, "gauss" + tag, o, mean1, s1)
    gauss_models[d] = gauss
    g1 = RooGaussian("g1" + tag, "g1" + tag, o, mean1, sf1)
    g2 = RooGaussian("g2" + tag, "g2" + tag, o, mean1, sf2)
    model = RooAddModel("model" + tag, "model" + tag, RooArgList(g1, g2), RooArgList(frac))
    __objects += [g1, g2, model]
    models[d] = model
    if d in ('x', 'y'):
        canvas = TCanvas('canvas' + tag, 'canvas' + tag, 1200, 800)
        canvas.Divide(2, 2)
        canvases[d] = canvas

canvas = TCanvas('canvas_magnet', 'canvas_magnet', 600, 800)
canvas.Divide(1, 2)
canvases['magnet'] = canvas

results = {}
frames = {}
fit_ranges = {} # (1, 'x') : 'wide',
                # (2, 'x') : 'narrow',
                # (3, 'x') : 'wide',
                # (4, 'x') : 'wide'}
fit_models = {} # (2, 'x') : gauss_models['x'],
                # (1, 'y') : gauss_models['y'],
                # (2, 'y') : gauss_models['y'],
                # (3, 'y') : gauss_models['y'],
                # (4, 'y') : gauss_models['y']}

args = [RooFit.Minimizer('Minuit2'), RooFit.Save(), RooFit.NumCPU(2)]
for k, ds in sorted(ds_res.items(), key = itemgetter(0)):
    for o, d in zip((x_obs, y_obs), ('x', 'y')):
        fit_args = args[:]
        fit_range = fit_ranges.get((k, d), '')
        if fit_range:
            fit_args += [RooFit.Range(fit_range)]
        # o.setRange(fit_range)
        # models[d].setNormRange(fit_range)
        model = fit_models.get((k, d), models[d])
        for i in range(3):
            r = model.fitTo(ds, *fit_args)
            if r.status() == 0 and abs(r.minNll()) < 5e5:
                break
        results[(k, d)] = r
        frame = o.frame()
        ds.plotOn(frame)
        if fit_range:
            model.plotOn(frame, RooFit.Range(fit_range))
        else:
            model.plotOn(frame)
        print 'Plotting ', k, d
        canvases[d].cd(k)
        frame.Draw()
        frames[(k, d)] = frame

for canvas in canvases.itervalues():
    canvas.Update()

p_inv_values = []
magnet_data = RooDataSet('magnet_data', 'magnet_data', obs_m)
magnet_results = {}
for entry in tree:
    if entry.dx < pull_xm.getMin() or entry.dx > pull_xm.getMax():
        continue
    if entry.dy < pull_ym.getMin() or entry.dy > pull_ym.getMax():
        continue
    if entry.p < 3000:
        continue
    q.setIndex(entry.q)
    pull_xm.setVal(entry.dx)
    pull_ym.setVal(entry.dy)
    p_inv.setVal(1000. / entry.p_match)
    p_inv_values.append(1000. / entry.p_match)
    p.setVal(entry.p_match / 1000.)
    magnet_data.add(obs_m)

# Make a binning with equal entries.
p_inv_values = sorted(p_inv_values)
n_bins = 9
di = int(float(len(p_inv_values)) / n_bins)
bins_p_inv = [p_inv.getMin()]
for i in range(1, n_bins):
    bins_p_inv += [p_inv_values[i * di]]
bins_p_inv += [p_inv.getMax()]

magnet_datas = {}
dp_inv = (p_inv.getMax() - p_inv.getMin()) / n_bins
dp = (p.getMax() - p.getMin()) / n_bins
bins_p = [0 + i * dp for i in range(n_bins + 1)]

graphs = {}
sigmas = defaultdict(list)
sigmas_err = defaultdict(list)
av = []
err_x = []
p_vars = {p_inv.GetName() : p_inv, p.GetName() :  p}

vname = p_inv.GetName()
bins = bins_p_inv
for i in range(n_bins):
    ds = magnet_data.reduce('{0} > {1} && {0} < {2}'.format(vname, bins[i], bins[i + 1]))
    magnet_datas[i] = ds
    av += [ds.mean(p_vars[vname])]
    err_x += [0.]

for obs, tag in ((pull_xm, 'xm'), (pull_ym, 'ym')):
    cn = 'fit_canvas_' + tag
    canvas_fit = TCanvas(cn, cn, 1200, 800)
    canvas_fit.Divide(3, 3)
    canvases['fit_' + tag] = canvas_fit
    model = models[tag]
    for i in range(n_bins):
        ds = magnet_datas[i]
        magnet_results[(tag, i)] = model.fitTo(ds, RooFit.Minimizer('Minuit2'), RooFit.Save(), RooFit.NumCPU(2))
        frame = obs.frame()
        ds.plotOn(frame)
        model.plotOn(frame)
        canvas_fit.cd(i + 1)
        frame.Draw()

for (tag, i), r in sorted(magnet_results.items()):
    par = r.floatParsFinal().find('sm_' + tag)
    sigmas[tag] += [par.getVal()]
    sigmas_err[tag] += [par.getError()]

sig_p_result = None
dx_results = {}
fun = '[0] + [1] * (x - {})^2'.format(0.25)
dx_funs = {'xm' : fun,  'ym' : fun}
for i, tag in enumerate(('xm', 'ym')):
    g = TGraphErrors(len(sigmas[tag]), array('d', av[:]), array('d', sigmas[tag]), array('d', err_x[:]), array('d', sigmas_err[tag]))
    g.SetName('sigmas_' + tag)
    g.SetTitle('err_' + tag)
    graphs[tag] = g
    canvases['magnet'].cd(i + 1)
    if type(dx_funs[tag]) == str:
        fun = TF1('err_fun_' + tag, dx_funs[tag])
        dx_funs[tag] = fun
    else:
        fun = dx_funs[tag]
    fun.SetLineColor(kBlue)
    dx_results[tag] = g.Fit(fun, 'S')
    g.Draw('AP')

from ROOT import RooCBShape
cb_mean = RooRealVar('cb_mean', 'cb_mean', 0, -100, 100)
cb_s1 = RooRealVar('cb_s1', 'cb_s1', 100, 0, 500)
cb_sf = RooRealVar('cb_sf', 'cb_sf', 100, 0.1, 500)
cb_s2 = RooFormulaVar('cb_s2', 'cb_s2', "@0 * @1", RooArgList(cb_sf, cb_s1))
cb_frac = RooRealVar('cb_f', 'f' + tag, 0.2, 0.001, 0.99)
cb_n = RooRealVar('cb_n', 'n', 1)
cb_a1 = RooRealVar('cb_a', 'alpha', 20, 0.001, 200)
cb_a2 = RooFormulaVar('cb_a2', 'cb_a2', "@0 * @1", RooArgList(cb_sf, cb_a1))
cb1 = RooCBShape("cb1" , "cb1" , match_res_x, cb_mean, cb_s1, cb_a1, cb_n)
cb2 = RooCBShape("cb2" , "cb2" , match_res_x, cb_mean, cb_s2, cb_a2, cb_n)
dcb = RooAddModel("dcb" , "double_cb" , RooArgList(cb1, cb2), RooArgList(cb_frac))


cor_xy = defaultdict(list)
sigmas_m = []
err_m = []
x_m = []
for i, (vname, j) in enumerate(product(['x', 'y'], range(1, 5))):
    x_m += [i + 1]
    r = results[(j, vname)]
    # if (vname, j) == ('x', 2) or vname == 'y':
    #     par = r.floatParsFinal().find('s1_' + vname)
    # else:
    par = r.floatParsFinal().find('sm_' + vname)
    mpar = r.floatParsFinal().find('mean1_' + vname)
    cor_xy[vname].append((mpar.getVal(), par.getVal()))
    sigmas_m += [par.getVal()]
    err_m += [par.getError()]

# g = TGraphErrors(len(sigmas_m), array('d', x_m), array('d', sigmas_m), array('d', [0. for i in x_m]), array('d', err_m))
# g.SetName('sigmas_m')
# g.SetTitle('err_M')
# graphs['M'] = g
# canvases['xm'].cd(2)
# g.Draw('AP')

import pprint
pprint.pprint(dict(cor_xy))
