#!/usr/bin/python -O
"""
@file Bs2DshStrippingExample.py

example file illustrating how the ZooTuplePlayer can be used to strip a Zoo
tuple

since the example tuples for this one are rather large (and probably not
final), I do not include them; this file is here mostly for illustration,
as an example of how a reasonably complex Zoo stripping might be done

@author Manuel Schiller <manuel.schiller@nikhef.nl>
@date	2012-01-23
"""
import ZooPy
from ROOT import *
from ZooTuplePlayer import *

########################################################################
# helper routines with complex calculations
########################################################################

# helper function to return all stable children (tracks)
def getCh(zp):
    return ([zp] if zp.St() else
	    sum((getCh(zp.Ch(i)) for i in xrange(0, zp.NCh())), []))

# helper function to return all children (both tracks and decaying stuff)
def getAllCh(zp):
    return sum((getAllCh(zp.Ch(i)) for i in xrange(0, zp.NCh())), [zp])

# helper to get the daughter which is a D
def getD(zp):
    return (zp if (abs(zp.pid()) == 431 and zp.NCh() == 3) else
	    reduce(lambda x, y: x if x else y, 
		(getD(zp.Ch(i)) for i in xrange(0, zp.NCh())), False))

# return the direction angle
#
# angle between momentum and direction from PV to DV if no mother is given
# otherwise  angle between momentum and direction between decay vertices of
# mother and daughter
def DIRA(zp, zpmother = None):
    p = zp.momentum().Vect()
    dv = zp.decayVertexPacked().pos()
    pv = zpmother.decayVertexPacked() if zpmother else zp.pv()
    dv -= pv.pos()
    dv = ROOT.Math.XYZVector(dv[0], dv[1], dv[2])
    return p.Dot(dv) / p.R() / dv.R()

# return if the PV position is ok (within beam pipe and luminous region)
def PVRhoZOK(zp):
    pv = zp.pv().pos()
    pv = ROOT.Math.XYZVector(pv[0], pv[1], pv[2])
    return pv.Rho() < 5.0 and pv.Z() < 150.

# chi^2 distance between pv and decay vertex
def DVTXCHI2(zp):
    # calculate chi^2= (DV-PV)^T * C^-1 * (DV-PV) where C is the sum of the
    # covariance matrices of PV and DV
    dvp = zp.decayVertexPacked()
    pvp = zp.pv()
    dv = dvp.pos()
    pv = pvp.pos()
    dv -= pv
    # apparently PyROOT gets confused about SMatrix operators, so I can't just
    # write cdv += cpv
    cdv = dvp.cov()
    cpv = pvp.cov()
    # only diagonal and below is saved in array
    cdva = cdv.Array()
    cpva = cpv.Array()
    for i in xrange(0, 3 * 4 / 2):
        cdva[i] += cpva[i]
    # invert sum of covariance matrices
    cdv.InvertChol()
    # PyROOT gets equally confused about ROOT::Math::Similarity, so compute
    # chi^2 "by hand"
    return sum(dv(i) * cdv(i, j) * dv(j)
	    for j in xrange(0, 3) for i in xrange(0, 3))

########################################################################
# create chain of files to be stripped
########################################################################
chain = ROOT.TChain('Forest')
chain.AddFile('/media/data-portable/B2DX-tuples/MagDown-0.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagDown-1.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagDown-2.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagDown-3.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagDown-4.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagDown-5.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagDown-6.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagUp-0.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagUp-1.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagUp-2.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagUp-3.root')
chain.AddFile('/media/data-portable/B2DX-tuples/MagUp-4.root')

########################################################################
# initialise ZooTuplePlayer
########################################################################
ztp = ZooTuplePlayer(chain)

########################################################################
# strip 10000 events of the given chain with the cuts below, putting
# the result into test2.root
########################################################################
# define cuts to apply
cuts = [
	# mass range makes sense
	lambda zp, zev: zp.momentum().M() / 1e3 > 4.5 and
	    zp.momentum().M() / 1e3 < 7.5,
	# not track
	lambda zp, zev: zp.Dc(),
	# PV is sane
	lambda zp, zev: PVRhoZOK(zp),
	# Bs has good vertex fit
	lambda zp, zev: zp.chi2() / zp.nDoF() < 12.,
	# B_s points back to PV
	lambda zp, zev: DIRA(zp) > 0.9999,
	# PV and B_s DV well separated
	lambda zp, zev: DVTXCHI2(zp) > 64.,
	# B_s is neutral
	lambda zp, zev: zp.charge() == 0,

	# Ds mass
	lambda zp, zev: abs(getD(zp).momentum().M() / 1e3 - 1.96847) < 0.03,
	# Ds min p_T
	lambda zp, zev: getD(zp).momentum().Pt() / 1e3 > 2.0,
	# Ds has good vertex fit
	lambda zp, zev: getD(zp).chi2() / getD(zp).nDoF() < 12.,
	# D_s points back to B_s
	lambda zp, zev: DIRA(getD(zp), zp) > 0.9999,
	# PV and D_s DV extremely well separated
	lambda zp, zev: DVTXCHI2(getD(zp)) > 100,
	# D_s is charged
	lambda zp, zev: abs(getD(zp).charge()) == 1,

	# four daughters are long tracks
	lambda zp, zev: sum(d.TrInfo().type() == 3 for d in getCh(zp)) == 4,
	# min daughter momentum
	lambda zp, zev: min(i.momentum().P() / 1e3 > 2.0 for i in getCh(zp)),
	# min daughter transverse momentum
	lambda zp, zev: min(i.momentum().Pt() / 1e3 > 0.3 for i in getCh(zp)),
	# daughter IP chi^2
	lambda zp, zev: min(i.ipSig() * i.ipSig() > 9. for i in getCh(zp)),

	# D_s decays into K K pi
	lambda zp, zev: sum(abs(d.pid()) == 321 for d in getCh(getD(zp))) == 2,
	lambda zp, zev: sum(abs(d.pid()) == 211 for d in getCh(getD(zp))) == 1,
	# DLL cuts on D_s daughter tracks
	lambda zp, zev: sum(
	    ((abs(d.pid()) == 321 and d.DLL().DLLk() > 5) or
		(abs(d.pid()) == 211 and d.DLL().DLLk() < 0)) for d in getCh(getD(zp))
	    ) == 3,
	# either Bs --> Ds pi or Bs --> Ds K
	lambda zp, zev: sum(abs(d.pid()) == 321 for d in getCh(zp)) >= 2,
	lambda zp, zev: sum(abs(d.pid()) == 211 for d in getCh(zp)) >= 1,
	]

# strip the events
ztp.StripByZooP('test.root', 'Bs2DsX', cuts, 10000)

# vim: sw=4:tw=78:ft=python
