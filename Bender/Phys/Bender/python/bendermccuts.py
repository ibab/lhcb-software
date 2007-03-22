# =============================================================================
# $Id: bendermccuts.py,v 1.5 2007-03-22 18:50:47 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
# =============================================================================
## This is helper module for decoration of LoKi particle functions/cuts 
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
#
#  @see Bender::Functions
#  @see LoKi::Function
#  @see LoKi::Predicate
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" This is helper module for decoration of LoKi vertex functions/cuts  """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================

import gaudimodule

_LoKi   = gaudimodule.gbl.LoKi
_Bender = gaudimodule.gbl.Bender
_LHCb   = gaudimodule.gbl.LHCb
_MCPS   = _LoKi.MCParticles

# =============================================================================
_MCP = 'const LHCb::MCParticle*'
# =============================================================================


## @see LoKi::Cuts::MCTRUE
MCTRUE     = _LoKi.BooleanConstant( _MCP )(True)
## @see LoKi::Cuts::MCFALSE
MCFALSE    = _LoKi.BooleanConstant( _MCP )(False)
## @see LoKi::Cuts::MCALL
MCALL      = MCTRUE
## @see LoKi::Cuts::MCNONE
MCNONE     = MCFALSE
## @see LoKi::Cuts::MCONE
MCONE      = _LoKi.Constant     ( _MCP )(1.0)
## @see LoKi::Cuts::MCZERO
MCZERO     = _LoKi.Constant     ( _MCP )(0.0)
## @see LoKi::Cuts::MCMIN
MCMIN      = _LoKi.Min          ( _MCP )
## @see LoKi::Cuts::MCMIN
MCMAX      = _LoKi.Max          ( _MCP )
## @see LoKi::Cuts::MCSWITCH
MCSWITCH   = _LoKi.Switch       ( _MCP )
## @see LoKi::Cuts::MCSSWITCH
MCSSWITCH  = _LoKi.SimpleSwitch ( _MCP )
## @see LoKi::Cuts::MCVALID
MCVALID    = _LoKi.Valid        ( _MCP )()
## @see LoKi::Cuts::MCSAME
MCSAME     = _LoKi.TheSame      ( _MCP )
## @see LoKi::Cuts::OSCILLATED
OSCILLATED = _LoKi.MCParticles.Oscillated   ()
## @see LoKi::Cuts::MCOSCILLATED
MCOSCILLATED = OSCILLATED 
## @see LoKi::Cuts::MCP
MCP        = _LoKi.MCParticles.Momentum () 
## @see LoKi::Cuts::MCE
MCE        = _LoKi.MCParticles.Energy   ()
## @see LoKi::Cuts::MCPT
MCPT       = _LoKi.MCParticles.TransverseMomentum () 
## @see LoKi::Cuts::MCPX
MCPX       = _LoKi.MCParticles.MomentumX ()
## @see LoKi::Cuts::MCPY
MCPY       = _LoKi.MCParticles.MomentumY () 
## @see LoKi::Cuts::MCPZ
MCPZ       = _LoKi.MCParticles.MomentumZ () 
## @see LoKi::Cuts::MCETA
MCETA      = _LoKi.MCParticles.PseudoRapidity() 
## @see LoKi::Cuts::MCPHI
MCPHI      = _LoKi.MCParticles.Phi() 
## @see LoKi::Cuts::MCTHETA
MCTHETA    = _LoKi.MCParticles.Theta() 
## @see LoKi::Cuts::MCMASS
MCMASS     = _LoKi.MCParticles.Mass() 
## @see LoKi::Cuts::MCM
MCM        =  MCMASS 
## @see LoKi::Cuts::MCID 
MCID       = _LoKi.MCParticles.Identifier () 
## @see LoKi::Cuts::MCABSID 
MCABSID    = _LoKi.MCParticles.AbsIdentifier () 
## @see LoKi::Cuts::MC3Q
MC3Q       = _LoKi.MCParticles.ThreeCharge () 
## @see LoKi::Cuts::MCTIME
MCTIME     = _LoKi.MCParticles.ProperLifeTime() 
## @see LoKi::Cuts::MCCTAU
MCCTAU     = MCTIME 
## @see LoKi::Cuts::MCQUARK
MCQUARK    = _LoKi.MCParticles.HasQuark
## @see LoKi::Cuts::MCCHARM
##MCCHARM    =  MCQUARK( _LHCb.ParticleID.charm   ) 
MCCHARM    =  MCQUARK( 4  ) 
## @see LoKi::Cuts::MCBEAUTY
## MCBEAUTY   =  MCQUARK( _LHCb.ParticleID.bottom  ) 
MCBEAUTY   =  MCQUARK( 5  ) 
## @see LoKi::Cuts::MCTOP
##MCTOP      =  MCQUARK( _LHCb.ParticleID.top     ) 
MCTOP      =  MCQUARK( 6  ) 
## @see LoKi::Cuts::MCSTRANGE 
##MCSTRANGE  =  MCQUARK( _LHCb.ParticleID.strange ) 
MCSTRANGE  =  MCQUARK( 3  ) 
## @see LoKi::Cuts::MCCHARGED
MCCHARGED  = _LoKi.MCParticles.IsCharged () 
## @see LoKi::Cuts::MCNEUTRAL
MCNEUTRAL  = _LoKi.MCParticles.IsNeutral () 
## @see LoKi::Cuts::MCLEPTON
MCLEPTON   = _LoKi.MCParticles.IsLepton  () 
## @see LoKi::Cuts::MCMESON
MCMESON    = _LoKi.MCParticles.IsMeson   () 
## @see LoKi::Cuts::MCBARYON
MCBARYON   = _LoKi.MCParticles.IsBaryon  () 
## @see LoKi::Cuts::MCHADRON
MCHADRON   = _LoKi.MCParticles.IsHadron  () 
## @see LoKi::Cuts::MCNUCLEUS
MCNUCLEUS  = _LoKi.MCParticles.IsNucleus () 
## @see LoKi::Cuts::FROMMCTREE 
FROMMCTREE = _LoKi.MCParticles.FromMCDecayTree
## @see LoKi::Cuts::NINMCDOWN
NINMCDOWN  = _LoKi.MCParticles.NinMCdownTree
## @see LoKi::Cuts::MCMOTHER 
MCMOTHER   = _LoKi.MCParticles.MCMotherFunction
## @see LoKi::Cuts::MCMOTHCUT
MCMOTHCUT  = _LoKi.MCParticles.MCMotherPredicate
## @see LoKi::Cuts::MCMOMDIST
MCMOMDIST  = _LoKi.MCParticles.MomentumDistance
## @see LoKi::Cuts::MCPTDIR 
MCPTDIR    = _LoKi.MCParticles.TransverseMomentumRel
## @see LoKi::Cuts::MCDPHI 
MCDPHI     = _LoKi.MCParticles.DeltaPhi
## @see LoKi::Cuts::MCDETA
MCDETA     = _LoKi.MCParticles.DeltaEta
## @see LoKi::Cuts::MCOVALID 
MCOVALID   = _LoKi.MCParticles.ValidOrigin ()
## @see LoKi::Cuts::MCVFASPF 
MCCFASPF   = _LoKi.MCParticles.MCVertexFunAdapter
## @see LoKi::Cuts::MCDECAY
MCDECAY    = _LoKi.MCParticles.MCDecayPattern
## @see LoKi::Cuts::FROMGTREE
FROMGTREE  = _LoKi.MCParticles.IsFromHepMC
## @see LoKi::Cuts::RCTRUTH 
RCTRUTH    = _LoKi.MCParticles.RCTruth


## @see LoKi::Cuts::CHARM
CHARM      =  MCCHARM 
## @see LoKi::Cuts::BEAUTY
BEAUTY     =  MCBEAUTY 
## @see LoKi::Cuts::TOP
TOP        =  MCTOP 
## @see LoKi::Cuts::STRANGE 
STRANGE    =  MCSTRANGE 
## @see LoKi::Cuts::CHARGED
CHARGED    = MCCHARGED 
## @see LoKi::Cuts::NEUTRAL
NEUTRAL    = MCNEUTRAL 
## @see LoKi::Cuts::LEPTON
LEPTON     = MCLEPTON
## @see LoKi::Cuts::MESON
MESON      = MCMESON
## @see LoKi::Cuts::BARYON
BARYON     = MCBARYON
## @see LoKi::Cuts::HADRON
HADRON     = MCHADRON
## @see LoKi::Cuts::NUCLEUS
NUCLEUS    = MCNUCLEUS


if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 


# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.4  2006/11/28 18:24:17  ibelyaev
#  prepare for v6r1
#
# =============================================================================
# The END 
# =============================================================================
