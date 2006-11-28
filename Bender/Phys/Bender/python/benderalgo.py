# =============================================================================
# $Id: benderalgo.py,v 1.20 2006-11-28 18:24:17 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.20 $
# =============================================================================
## Auxillary module  to decorate basic bender algorithm
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" Auxillary module  to decorate basic bender algorithm """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

import os
import gaudimodule
import GaudiAlgs

_gbl    = gaudimodule.gbl
_LoKi   = _gbl.LoKi
_Bender = _gbl.Bender

_Alg    = _Bender.Alg('LoKi::Algo')
_AlgMC  = _Bender.Alg('LoKi::AlgoMC')

class Algo   ( _Alg   ) : pass
class AlgoMC ( _AlgMC ) : pass

Algo   . _Base = _Alg
AlgoMC . _Base = _AlgMC

Algo   . initialize = GaudiAlgs._initialize_tuple_
AlgoMC . initialize = GaudiAlgs._initialize_tuple_

Algo   .execute  = Algo   .execute_ 
AlgoMC .execute  = AlgoMC .execute_ 
Algo   .analyse  = Algo   .analyse_ 
AlgoMC .analyse  = AlgoMC .analyse_ 

Algo.ntupleSvc   = GaudiAlgs._ntupleSvc
Algo.tupleSvc    = GaudiAlgs._ntupleSvc
Algo.ntupSvc     = GaudiAlgs._ntupleSvc
Algo.tupSvc      = GaudiAlgs._ntupleSvc
Algo.evtColSvc   = GaudiAlgs._evtcolSvc
Algo.evtcolSvc   = GaudiAlgs._evtcolSvc

AlgoMC.ntupleSvc = GaudiAlgs._ntupleSvc
AlgoMC.tupleSvc  = GaudiAlgs._ntupleSvc
AlgoMC.ntupSvc   = GaudiAlgs._ntupleSvc
AlgoMC.tupSvc    = GaudiAlgs._ntupleSvc
AlgoMC.evtColSvc = GaudiAlgs._evtcolSvc
AlgoMC.evtcolSvc = GaudiAlgs._evtcolSvc

GaudiAlgs._decorate_algs_    ( [ Algo , AlgoMC ] )
GaudiAlgs._decorate_plots_   ( [ Algo , AlgoMC ] )
GaudiAlgs._decorate_tuples_  ( [ Algo , AlgoMC ] )


if __name__ == '__main__' :
    print "dir(%s) : %s" % ( __name__ , dir() ) 

# =============================================================================
# The END 
# =============================================================================
