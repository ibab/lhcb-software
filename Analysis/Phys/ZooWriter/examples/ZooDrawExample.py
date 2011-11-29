#!/usr/bin/python -O
"""
@file ZooDrawExample.py

example file illustrating how the TTree::Draw()-like functionality of the
ZooTuplePlayer can be used

@author Manuel Schiller <manuel.schiller@nikhef.nl>
@date	2011-11-24

this stuff in meant to be used mostly from the python prompt interactively
"""
import ZooPy
from ROOT import *
from ZooTuplePlayer import *

chain = ROOT.TChain('Forest')
chain.Add('MC.root')

ztp = ZooTuplePlayer(chain)

ztp.DrawZooP('MyBs',
	'B^{0}_{s} p versus p_{T} (reconstructed B^{0}_{s});p [GeV/c];p_{T} [GeV/c]',
	[ lambda zp, zev: zp.momentum().P() / 1e3,
	    lambda zp, zev: zp.momentum().Pt() / 1e3 ],
	[ lambda zp, zev: zp.momentum().Eta() >= 2. and
	    zp.momentum().Eta() <= 5.0 ])

ROOT.gPad.Print('draw.eps')

ztp.DrawZooMCP('B^{0}_{s} p versus p_{T} (B^{0}_{s} from MC);p [GeV/c];p_{T} [GeV/c]',
	[ lambda zp, zev: zp.momentum().P() / 1e3,
	    lambda zp, zev: zp.momentum().Pt() / 1e3 ],
	[ lambda zp, zev: abs(zp.pid()) == 531,
	    lambda zp, zev: zp.momentum().Eta() >= 2. and
	    zp.momentum().Eta() <= 5.0 ])

ROOT.gPad.Print('drawMC.eps')

# vim: sw=4:tw=78:ft=python
