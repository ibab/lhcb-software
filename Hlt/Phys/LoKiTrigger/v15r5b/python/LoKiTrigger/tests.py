#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file   LoKiTrigger/tests.py
#  The simple test script for Phys/LoKiTrigger package
#
#        This file is a part of LoKi project -
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
#  contributions and advices from G.Raven, J.van Tilburg,
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-05-29
# =============================================================================
"""
The basic tests for LoKiTrigger package

     This file is a part of LoKi project -
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
contributions and advices from G.Raven, J.van Tilburg,
A.Golutvin, P.Koppenburg have been used in the design.

"""
# =============================================================================
__author__   = "Alexander Mazurov  alexander.mazurov@gmail.com"
__date__     = "2010-10-26"
__version__  = "$Revision$ "
# =============================================================================
from  GaudiPython import AppMgr, PyAlgorithm
gaudi = AppMgr()
alg = PyAlgorithm("MyAlgorithm")
# =============================================================================
from LoKiTrigger.decorators import *
from LoKiCore.functions import *
from LoKiCore.functions import *
#from LoKiCore.basic import std, cpp
import LoKiCore

from GaudiPython import PyAlgorithm

# =============================================================================
if '__main__' == __name__ :

    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120

    from LoKiCore.basic import cpp,std,LoKi  
    Hlt.Candidate.ConstVector = std.vector('const Hlt::Candidate*')

    Hlt.Candidate.ConstVector.__repr__ = lambda s : str ( [ str(i) for i in s ] )
    Hlt.Candidate.ConstVector.__str__  = lambda s : str ( [ str(i) for i in s ] )
    
    s = LHCb.State()
    s.setState ( 0.0 , 0.0 , 0.0 , 0.01 , 0.01 , 0.2 )
    t = LHCb.Track()
    t.addToStates ( s )

    #print 'track: (TrP,TrPT)=(%s,%s)'%(TrP(t),TrPT(t))
    #print 'track: ( t>>TrP , p>>cos(TrPT) )=(%s,%s)'%(t>>TrP,t>>cos(TrPT))

    v = std.vector('LHCb::Track*')()

    #v.push_back(t)

    t1 = LHCb.Track()
    s1 = LHCb.State()
    s1.setState ( 0.0 , 0.0 , 0.0 , -0.01 , -0.01 , 0.1 )
    t1.addToStates ( s1 )
    v.push_back ( t1 )

    t2 = LHCb.Track()
    s2 = LHCb.State()
    s2.setState ( 0.0 , 0.0 , 0.0 , +0.01 , -0.01 , 0.05 )
    t2.addToStates ( s2 )
    v.push_back ( t2 )

    t3 = LHCb.Track()
    s3 = LHCb.State()
    s3.setState ( 0.0 , 0.0 , 0.0 , -0.01 , +0.01 , 0.01 )
    t3.addToStates ( s3 )
    v.push_back ( t3 )

    t4 = LHCb.Track()
    s4 = LHCb.State()
    s4.setState ( 0.0 , 0.0 , 0.0 , 0.01 , 0.01 , 0.005 )
    t4.addToStates ( s4 )
    v.push_back ( t4 )

    t5 = LHCb.Track()
    s5 = LHCb.State()
    s5.setState ( 0.0 , 0.0 , 0.0 , 0.01 , 0.01 , 0.001 )
    t5.addToStates ( s5 )
    v.push_back ( t5 )


    s1 = Hlt.Stage()
    l1 = Hlt.Stage.Lock(s1,alg)
    s1.set(t1)

    s2 = Hlt.Stage()
    l2 = Hlt.Stage.Lock(s2,alg)
    s2.set(t2)

    s3 = Hlt.Stage()
    l3 = Hlt.Stage.Lock(s3,alg)
    s3.set(t3)

    cs = Hlt.Candidate.ConstVector()
    c1 = Hlt.Candidate()
    c1.addToStages(s1)

    c2 = Hlt.Candidate()
    c2.addToStages(s2)

    c3 = Hlt.Candidate()
    c3.addToStages(s3)

    cs.push_back (c1)
    cs.push_back (c2)
    cs.push_back (c3)

    for item in cs:
        print 'track: TrP=%s' % TrP(item.currentStage()._get())

    fun1 = select(TC_ALL) >> ( monitor ( TrP ) > 90)

    mc = LoKi.L0.L0MuonCut()
    fun2 = select(TC_ALL) >> mc


    fun3 = select(TC_ALL) >> TS_ALL


    print ' use fun1 %s %s ' % ( fun1 , len ( cs >> fun1 ) )
    print ' use fun2 %s %s ' % ( fun2 , len ( cs >> fun2 ) )
    print ' use fun3 %s %s ' % ( fun3 , len ( cs >> fun3 ) )


    #
    ## particles : 
    #
    
    pcs = Hlt.Candidate.ConstVector()

    #
    ## first particle
    #
    pc1 = Hlt.Candidate()

    pp1 = LHCb.Particle( LHCb.ParticleID( 13 ) ) 
    pp1.setMomentum ( Gaudi.LorentzVector( 100 , 100 , 100 , 500 ) ) 

    ## create stage 
    ps1 = Hlt.Stage()
    pl1 = Hlt.Stage.Lock( ps1 , alg )
    ps1.set ( pp1 )
    
    pc1.addToStages(ps1)

    pcs.push_back( pc1 )

    #
    ## first particle
    #
    pc2 = Hlt.Candidate()

    pp2 = LHCb.Particle( LHCb.ParticleID( 11 ) ) 
    pp2.setMomentum ( Gaudi.LorentzVector( 100 , 0 , 0 , 5000 ) ) 
    
    ## create stage 
    ps2 = Hlt.Stage()
    pl2 = Hlt.Stage.Lock( ps2 , alg )
    ps2.set ( pp2 )
    
    pc2.addToStages(ps2)

    pcs.push_back( pc2 )

    from LoKiPhys.decorators import PT, ID, M

    
    pfun1 = select(TC_ALL) >> TS_ISPARTICLE

    pfun2 = select(TC_ALL) >> ( PT > 1   ) 

    pfun3 = select(TC_ALL) >> ( ID == 13 ) 

    pfun4 = select(TC_ALL) >> M
    
    res1 = pcs >> pfun1
    res2 = pcs >> pfun2
    res3 = pcs >> pfun3
    res4 = pcs >> pfun4
    
    print 'pfun1, res1', pfun1 ,res1 
    print 'pfun2, res2', pfun2 ,res2 
    print 'pfun3, res3', pfun3 ,res3 
    print 'pfun4, res4', pfun4 ,res4 
    



# =============================================================================
# The END
# =============================================================================
