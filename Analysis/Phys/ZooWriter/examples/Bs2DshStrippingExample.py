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

# calculate direction angle (angle between momentum and direction from PV to
# DV
def DIRA(zp):
    p = zp.momentum().Vect()
    dv = zp.decayVertexPacked().pos()
    pv = zp.pv().pos()
    dv -= pv
    dv = dv.Unit()
    return (p.X() * dv[0] + p.Y() * dv[1] + p.Z() * dv[2]) / p.R()

# helper function to return all stable children (tracks)
def getCh(zp):
    if zp.St():
	return [ zp ]
    ch = []
    for i in range(0, zp.NCh()):
	ch += getCh(zp.Ch(i))
    return ch

# helper to get the daughter which is a D
def getD(zp):
    if zp.NCh() == 3 and abs(zp.pid()) == 431:
	return zp
    for i in range(0, zp.NCh()):
	d = getD(zp.Ch(i))
	if d:
	    return d
    return None

# return the direction angle (angle between momentum and direction between
# decay vertices of mother and daughter)
def DIRA2(zp, zpmother):
    p = zp.momentum().Vect()
    dv = zp.decayVertexPacked().pos()
    pv = zpmother.decayVertexPacked().pos()
    dv -= pv
    dv = dv.Unit()
    return (p.X() * dv[0] + p.Y() * dv[1] + p.Z() * dv[2]) / p.R()

# return if the PV position is ok (within beam pipe and luminous region)
def PVRhoZOK(zp):
    pv = zp.pv().pos()
    p = ROOT.Math.XYZPoint(pv[0], pv[1], pv[2])
    return p.Rho() < 5. and abs(p.Z()) < 150.

# chi^2 distance between pv and decay vertex
def DVTXCHI2(zp):
    # calculate chi^2= (DV-PV)^T * C^-1 * (DV-PV) where C is the sum of the
    # covariance matrices of PV and DV
    dv = zp.decayVertexPacked().pos()
    cdv = zp.decayVertexPacked().cov()
    pv = zp.pv().pos()
    cpv = zp.pv().cov()
    dv -= pv
    # apparently PyROOT gets confused about SMatrix operators, so I can't just
    # write cdv += cpv
    # only diagonal and below is saved in array
    for i in range(0, 3 * 4 / 2):
	cdv.Array()[i] += cpv.Array()[i]
    # invert sum of covariance matrices
    cdv.InvertChol()
    # PyROOT gets equally confused about ROOT::Math::Similarity, so compute
    # chi^2 "by hand"
    tmp = 0.
    for i in range(0, 3):
	for j in range(0, 3):
	    tmp += dv(i) * dv(j) * cdv(i, j)
    return tmp

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
ztp.StripByZooP('test2.root', 'Bs2DsX',
	[
	    # mass range makes sense
	    lambda zp, zev: zp.momentum().M() / 1e3 > 4.5 and zp.momentum().M() / 1e3 < 7.5,
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
	    lambda zp, zev: DIRA2(getD(zp), zp) > 0.9999,
	    # PV and D_s DV extremely well separated
	    lambda zp, zev: DVTXCHI2(getD(zp)) > 100,
	    # D_s is charged
	    lambda zp, zev: abs(getD(zp).charge()) == 1,

	    # four daughters are long tracks
	    lambda zp, zev: [ d.TrInfo().type() for d in getCh(zp) ].count(3) == 4,
	    # min daughter momentum
	    lambda zp, zev: min([ i.momentum().P() / 1e3 ] for i in getCh(zp)) > 2.0,
	    # min daughter transverse momentum
	    lambda zp, zev: min([ i.momentum().Pt() / 1e3 ] for i in getCh(zp)) > 0.3,
	    # daughter IP chi^2
	    lambda zp, zev: min([ i.ipSig() * i.ipSig() ] for i in getCh(zp)) > 9.,

	    # D_s decays into K K pi
	    lambda zp, zev: [ abs(d.pid()) for d in getCh(getD(zp)) ].count(321) == 2,
	    lambda zp, zev: [ abs(d.pid()) for d in getCh(getD(zp)) ].count(211) == 1,
	    # DLL cuts on D_s daughter tracks
	    lambda zp, zev: [
	        ((d.DLL().DLLk() > 5 and abs(d.pid()) == 321) or
	            (d.DLL().DLLk() < 0 and abs(d.pid()) == 211)) for d in getCh(getD(zp))
	        ].count(True) == 3,

	    # either Bs --> Ds pi or Bs --> Ds K
	    lambda zp, zev: [ abs(d.pid()) for d in getCh(zp) ].count(321) >= 2,
	    lambda zp, zev: [ abs(d.pid()) for d in getCh(zp) ].count(211) >= 1,
	    ], 10000 )

# vim: sw=4:tw=78:ft=python
