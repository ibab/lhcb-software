#!/usr/bin/env python
# =============================================================================
# $Id: bendertypes.py,v 1.1 2004-07-11 15:47:06 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================


# =============================================================================
# @file
#
# defintion of basic LoKi/Bender types  
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule   as gaudi 
import benderhelper  as helper

# global namespace 
gbl   = gaudi.gbl
# namespace LoKi
loki  = gbl.LoKi


Range    = helper.loadRange  ( loki.Range_( 'std::vector<Particle*> '   ) )
VRange   = helper.loadRange  ( loki.Range_( 'std::vector<Vertex*> '     ) )
MCRange  = helper.loadRange  ( loki.Range_( 'std::vector<MCParticle*> ' ) )
MCVRange = helper.loadRange  ( loki.Range_( 'std::vector<MCVertex*> '   ) )

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
# The END 
# =============================================================================
