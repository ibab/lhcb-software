# $Id: bendertypes.py,v 1.2 2004-11-23 17:13:23 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
"""
Definitition of basic Bender 'Ranges' and operations
"""

# =============================================================================
# @file
#
# defintion of basic LoKi/Bender types  
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule
import benderhelper  as helper

# namespace LoKi
loki  = gaudimodule.gbl.LoKi

Range    = helper.loadRange  ( loki.NamedRange_( 'std::vector<Particle*> '   ) )
VRange   = helper.loadRange  ( loki.NamedRange_( 'std::vector<Vertex*> '     ) )
MCRange  = helper.loadRange  ( loki.NamedRange_( 'std::vector<MCParticle*> ' ) )
MCVRange = helper.loadRange  ( loki.NamedRange_( 'std::vector<MCVertex*> '   ) )

Range.list     = helper.range2list
VRange.list    = helper.range2list
MCRange.list   = helper.range2list
MCVRange.list  = helper.range2list

Range.tuple    = helper.range2tuple
VRange.tuple   = helper.range2tuple
MCRange.tuple  = helper.range2tuple
MCVRange.tuple = helper.range2tuple

Seq2List       = helper.Seq2List
Seq2Tuple      = helper.Seq2Tuple

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
