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
#  By usage of this code one clearly states the disagreement
#  with the smear campaign of Dr.O.Callot et al.:
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
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

By usage of this code one clearly states the disagreement
with the smear campaign of Dr.O.Callot et al.:
 ``No Vanya's lines are allowed in LHCb/Gaudi software.''

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

    Hlt = LoKiCore.basic.cpp.Hlt
    LoKi = LoKiCore.basic.cpp.LoKi
    Hlt.Candidate.ConstVector = LoKiCore.basic.std.vector('const Hlt::Candidate*')

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





# =============================================================================
# The END
# =============================================================================
