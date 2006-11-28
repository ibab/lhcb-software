# =============================================================================
# $Id: bendermcvcuts.py,v 1.3 2006-11-28 18:24:17 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $
# =============================================================================
## This is helper module for decoration of LoKi MC vertex functions/cuts 
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
#  @see LoKi::MCVertices 
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" This is helper module for decoration of LoKi MC vertex functions/cuts  """
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

import gaudimodule

_LoKi   = gaudimodule.gbl.LoKi
_Bender = gaudimodule.gbl.Bender
_LHCb   = gaudimodule.gbl.LHCb
_MC     = _LoKi.MCVertices 

# =============================================================================
_MCV = 'const LHCb::MCVertex*'
# =============================================================================


## @see LoKi::Cuts::MCVTRUE
MCVTRUE     = _LoKi.BooleanConstant( _MCV )(True)
## @see LoKi::Cuts::MCVFALSE
MCVFALSE    = _LoKi.BooleanConstant( _MCV )(False)
## @see LoKi::Cuts::MCVALL
MCVALL      = MCVTRUE
## @see LoKi::Cuts::MCVNONE
MCVNONE     = MCVFALSE
## @see LoKi::Cuts::MCVONE
MCVONE      = _LoKi.Constant     ( _MCV )(1.0)
## @see LoKi::Cuts::MCVZERO
MCVZERO     = _LoKi.Constant     ( _MCV )(0.0)
## @see LoKi::Cuts::MCVMIN
MCVMIN      = _LoKi.Min          ( _MCV )
## @see LoKi::Cuts::MCVMIN
MCVMAX      = _LoKi.Max          ( _MCV )
## @see LoKi::Cuts::MCVSWITCH
MCVSWITCH   = _LoKi.Switch       ( _MCV )
## @see LoKi::Cuts::MCVSSWITCH
MCVSSWITCH  = _LoKi.SimpleSwitch ( _MCV )
## @see LoKi::Cuts::MCVVALID
MCVVALID    = _LoKi.Valid        ( _MCV )()
## @see LoKi::Cuts::MCVSAME
MCVSAME     = _LoKi.TheSame      ( _MCV )
## @see LoKi::Cuts::MCVTYPE
MCVTYPE     = _LoKi.MCVertices.TypeOfMCVertex ()
## @see LoKi::Cuts::MCVFOT
MCVTOF      = _LoKi.MCVertices.TimeOfFlight   ()
## @see LoKi::Cuts::MCVX 
MCVX        = _LoKi.MCVertices.VertexPositionX()
## @see LoKi::Cuts::MCVY
MCVY        = _LoKi.MCVertices.VertexPositionY()
## @see LoKi::Cuts::MCVZ 
MCVZ        = _LoKi.MCVertices.VertexPositionZ()
## @see LoKi::Cuts::MCVTIME
MCVTIME     = _LoKi.MCVertices.VertexTime     ()
## @see LoKi::Cuts::MCPRIMARY
MCPRIMARY   = _LoKi.MCVertices.Primary        ()
## @see LoKi::Cuts::MCISDECAY
MCISDECAY   = _LoKi.MCVertices.Decay          ()
## @see LoKi::Cuts::MCCDIST
MCVDIST     = _LoKi.MCVertices.MCVertexDistance
## @see LoKi::Cuts::MCVXFUN
MCVXFUN     = _LoKi.MCVertices.MCVFunAsMCFun

if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 


# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
