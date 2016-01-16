#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file LoKiMC/graph.py
#  Simple utilities to vizualize the decay tree 
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
#    ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#    
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-01-16
#  
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Simple utilities to vizualize the decay tree 

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

                  $Revision$
Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-01-16"
__version__ = "$Revision$"
__all__     = (
    'graph'        ,  ## function to produce graph 
    'MCDecayGraph'    ## helper class to produce graph 
    ) 
# =============================================================================
import pydot 
from   LoKiMC.MC import LHCb
#
node_attributes = { 'shape'     : 'ellipse' ,
                    'fillcolor' : 'yellow'  , 
                    'color'     : 'red'     ,
                    'style'     : 'filled'  }
edge_attributes = { 'color'     : 'red'     }
#
PARTICLE = LHCb.MCParticle
# 
# =============================================================================
## @class MCDecayGraph
#  Make graphical (dot) representation of the decay tree
#  @code
#  p     = ...
#  dg    = MCDecayGraph ( p )
#  graph = gd.graph()
#  graph.write('graph.png', format = 'png') 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-01-16
class MCDecayGraph ( object ) :
    """ Make graphical representation of the decay tree
    >>> p     = ...
    >>> dg    = MCDecayGraph ( p )
    >>> graph = gd.graph() 
    >>> graph.write('graph.png', format = 'png') 
    """
    def __init__ ( self , trees , decay_only = True ) :
        self._nodes      = set()
        self._edges      = set()
        self._graph      = pydot.Dot ( 'DecayTree' , graph_type = 'digraph' )
        self._decay_only = decay_only
        
        if isinstance ( trees , PARTICLE ) : trees = [ trees ] 
        for t in trees :
            self.add_tree ( t )

    ## construct node-id for the given particle
    def node_id ( self , p ) :                
        if not p  : return ''
        # get the container 
        c = p.parent() 
        if not c : return '%s:%s:#%d' % ( p.name () , hex ( id ( p ) ) , p.key() ) 
        r = c.registry()
        if not r : return '%s:%s:#%d' % ( p.name () , hex ( id ( p ) ) , p.key() )
        #
        return '%s:%s:#%d' % ( p.name() , r.identifier() , p.key() )

    ## add particle to graph 
    def add_tree ( self , p ) :

        nid = self.node_id ( p )
        if nid in self._nodes : return nid 
        
        nid = nid.replace ( ':'       , '/' )
        nid = nid.replace ( '_'       , '/' )
        nid = nid.replace ( '#'       , '/' )
        nid = nid.replace ( '<'       , ''  )
        nid = nid.replace ( '>'       , ''  )
        nid = nid.replace ( 'Unknown' , ''  )

        # create new node
        node = pydot.Node ( name      = nid       ,
                            label     = p.name()  , **node_attributes) 
        
        self._nodes.add      ( nid  )
        self._graph.add_node ( node )

        for c in p.children( self._decay_only ) :
            nidc = self.add_tree ( c )                        ## NB: recursion here
            edge = nid,nidc 
            if not edge in self._edges :
                self._graph.add_edge ( pydot.Edge ( *edge , **edge_attributes ) ) ## create edge 
                self._edges.add ( edge )
                
        return nid 

    ## get the graph 
    def graph ( self ) : return self._graph

# =============================================================================
## make a graph for given particle or container of particles 
#  @code
#  p               = ...
#  graph_file_name = graph( p , format  = 'png' )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-01-16
def graph ( particle , format = 'dot' , filename = 'mcdecaytree' , decay_only = True ) :
    """ Make a graph for given particle
    >>> p               = ...
    >>> graph_file_name = graph( p , format  = 'png' )
    """
    dg = MCDecayGraph ( particle , decay_only )
    dt = dg.graph() 
    #
    return filename if dt.write ( filename , format = format ) else None 

# =============================================================================
## make a graph for given particle or container of particles 
#  @code
#  p               = ...
#  graph_file_name = p.graph( format  = 'png' )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-01-16
def _p_graph_ ( p , format = 'dot' , filename = 'mcdecaytree' , decay_only = True ) :
    """ Make a graph for given particle
    >>> p               = ...
    >>> graph_file_name = p.graph ( format  = 'png' )
    """
    return graph ( p , format = format , filename = filename , decay_only = decay_only )

for t in ( PARTICLE             ,
           PARTICLE.Container   ,
           PARTICLE.ConstVector ,
           PARTICLE.Range       ) : t.graph = _p_graph_ 

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'

# =============================================================================
# The END 
# =============================================================================

