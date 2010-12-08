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
#  with the campain of Dr.O.Callot et al.:
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
with the campain of Dr.O.Callot et al.:
 ``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__   = "Vanya Belyaev ivan.belyaev@cern.ch, Alexander Mazurov  alexander.mazurov@gmail.com"
__date__     = "2010-10-26"
__version__  = "CVS tag $Name: not supported by cvs2svn $, verison $Revision$ "
# =============================================================================
import unittest

from  GaudiPython import AppMgr, PyAlgorithm

# =============================================================================
from LoKiTrigger.decorators import *
from LoKiTrigger.functions import *
from LoKiCore.functions import *
import LoKiCore

from GaudiPython import PyAlgorithm

Hlt = LoKiCore.basic.cpp.Hlt
LoKi = LoKiCore.basic.cpp.LoKi
Hlt.Candidate.ConstVector = LoKiCore.basic.std.vector('const Hlt::Candidate*')



class TestLoKiTrigger(unittest.TestCase):
    is_app_run = False
    #candidates = []
    def setUp(self):
        self.alg = PyAlgorithm("MyAlgorithm")
        #if not self.is_app_run:
        TestLoKiTrigger.is_app_run = True
        #######################################################################
        # Create tracks
        #######################################################################
        self.tracks = []
        self.states = []
        self.stages = []
        self.candidates = []
        self.vcandidates = Hlt.Candidate.ConstVector()


        # States
        s = LHCb.State() #1
        s.setState ( 0.0 , 0.0 , 0.0 , -0.01 , -0.01 , 0.1 )
        self.states.append(s)
        s = LHCb.State() #2
        s.setState ( 0.0 , 0.0 , 0.0 , +0.01 , -0.01 , 0.05 )
        self.states.append(s)
        s = LHCb.State() #3
        s.setState ( 0.0 , 0.0 , 0.0 , -0.01 , +0.01 , 0.01 )
        self.states.append(s)
        s = LHCb.State() #4
        s.setState ( 0.0 , 0.0 , 0.0 , 0.01 , 0.01 , 0.005 )
        self.states.append(s)
        s = LHCb.State() #5
        s.setState ( 0.0 , 0.0 , 0.0 , 0.01 , 0.01 , 0.001 )
        self.states.append(s)

        #tracks
        t = LHCb.Track() #1
        t.addToStates (self.states[0] )
        self.tracks.append(t);

        t = LHCb.Track() #2
        t.addToStates (self.states[1] )
        self.tracks.append(t);
        t = LHCb.Track() #3
        t.addToStates (self.states[2] )
        self.tracks.append(t);
        t = LHCb.Track() #4
        t.addToStates (self.states[3] )
        self.tracks.append(t);
        t = LHCb.Track() #5
        t.addToStates (self.states[4] )
        self.tracks.append(t);

        # Stages
        stage = Hlt.Stage() #1
        lock = Hlt.Stage.Lock(stage, self.alg)
        stage.set(self.tracks[0])
        self.stages.append(stage)


        stage = Hlt.Stage() #2
        lock = Hlt.Stage.Lock(stage, self.alg)
        stage.set(self.tracks[1])
        self.stages.append(stage)

        stage = Hlt.Stage() #3
        lock = Hlt.Stage.Lock(stage, self.alg)
        stage.set(self.tracks[2])
        self.stages.append(stage)
        # Candidates
        c =   Hlt.Candidate() #1
        c.addToStages(self.stages[0])
        self.candidates.append(c)
        self.vcandidates.push_back(c)
        c =   Hlt.Candidate() #2
        c.addToStages(self.stages[1])
        self.candidates.append(c)
        self.vcandidates.push_back(c)
        c =   Hlt.Candidate() #3
        c.addToStages(self.stages[2])
        self.candidates.append(c)
        self.vcandidates.push_back(c)

    def test_pipe_candidate_trackpredicate_rshift(self):
        fun = select(TC_ALL) >> (TrP  > 90)
        result = self.vcandidates >> fun
        self.assertEqual(len(result),1 )

        fun = select(TC_ALL) >> (TrP > 0)
        result = self.vcandidates >> fun
        self.assertEqual(len(result),3 )

        fun = select(TC_ALL) >> TS_ALL
        result = self.vcandidates >> fun
        self.assertEqual(len(result),0 )

    def test_pipe_candidate(self):
        fun = select(TC_ALL) >> TC_FILTER(TrP  > 90)
        result = self.vcandidates >> fun
        self.assertEqual(len(result),1 )

        fun = select(TC_ALL) >> TC_FILTER(TrP > 0)
        result = self.vcandidates >> fun
        self.assertEqual(len(result),3 )

        fun = select(TC_ALL) >> TC_FILTER(TS_ALL)
        result = self.vcandidates >> fun
        self.assertEqual(len(result),0 )

    def test_map_candidate(self):
        fun = select(TC_ALL) >> TC_MAP(TrP)
        result = self.vcandidates >> fun
        presult = [x for x in result]
        self.assertEqual(presult, [10.0,20.0,100.0] )

    def test_map_candidate_rshift(self):
        fun = select(TC_ALL) >> TrP
        result = self.vcandidates >> fun
        presult = [x for x in result]
        self.assertEqual(presult, [10.0,20.0,100.0] )

    def _test_spirce_candidate(self):
        fun = TC_Source >>(TrP>90)
        print type(fun)
        #self.assertEqual(len(result),1 )



# =============================================================================
if '__main__' == __name__ :

    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    gaudi = AppMgr()
    unittest.main()

#    Hlt = LoKiCore.basic.cpp.Hlt
#    LoKi = LoKiCore.basic.cpp.LoKi
#    Hlt.Candidate.ConstVector = LoKiCore.basic.std.vector('const Hlt::Candidate*')
#
#
#    for item in cs:
#        print 'track: TrP=%s' % TrP(item.currentStage()._get())
#
#    fun1 = select(TC_ALL) >> ( monitor ( TrP ) > 90)
#
#    mc = LoKi.L0.L0MuonCut()
#    fun2 = select(TC_ALL) >> mc
#
#
#    fun3 = select(TC_ALL) >> TS_ALL
#    fun4 = select(TC_ALL) >> TrP
#
#    fun5 = select(TC_ALL) >> TC_ALL
#
#
#    print ' use fun1 %s %s ' % ( fun1 , len ( cs >> fun1 ) )
#    print ' use fun2 %s %s ' % ( fun2 , len ( cs >> fun2 ) )
#    print ' use fun3 %s %s ' % ( fun3 , len ( cs >> fun3 ) )
#    print ' use fun4 %s %s' % (fun4,  [x for x in (cs >> fun4)])
#    print ' use fun5 %s %s' % (fun5,  [x for x in (cs >> fun5)])

# =============================================================================
# The END
# =============================================================================
