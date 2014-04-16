#!/usr/bin/env python
# =====================================================================================
from LoKiGen.decorators import *



GPV = std.vector('const HepMC::GenParticle*')
v   = GPV()

p = HepMC.GenParticle()
p.set_pdg_id(11)

v.push_back(p)

p1 = HepMC.GenParticle()
p1.set_pdg_id(13)
v.push_back(p1)

p2 = HepMC.GenParticle()
p2.set_pdg_id(22)
v.push_back( p2 )


## THIS IS OK 
for i in v : print i.pdg_id()

## make a range from vector 
r = GRange( v )

## THIS MAKES SEGMENTATION FAULT FOR ROOT6 
for i in r : print i.pdg_id() 

