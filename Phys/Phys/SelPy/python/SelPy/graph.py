"""Module for selection vizualisation tools.
"""

__all__ = ('graphEdges'     ,
           'GraphEdges'     ,
           'SelectionGraph' , 
           'dot'            ,
           '_dot'           ,
           'graph'          ,  
           'view'           )

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

import pydot


class GraphEdges(object) :
    """
    Return a list of all the edges of the selection tree for a given
    selection object.
    """
    def __init__(self) :
        self._edges = []
    def __call__(self, selection) :
        self._edges = []
        self._getEdges(selection)
        return self._edges
    def _getEdges(self, selection) :
        
        reqsels = selection.requiredSelections ()
        if not reqsels and hasattr ( selection , "_sel" ) :
            sel    = selection._sel
            if hasattr ( sel , 'requiredSelections' ) :  
                reqsels = sel.requiredSelections ()
            
        for sel in reqsels  :
            if (sel.name(), selection.name()) not in self._edges :
                self._edges.append((sel.name(), selection.name()))
            self._getEdges(sel)

graphEdges = GraphEdges()

def _dot ( selection ) :
    """
    Return a pydot.Dot object made from the edges of selection's graph.
    """
    return pydot.graph_from_edges(graphEdges(selection), directed=True)


# =============================================================================
# New stuff 
# =============================================================================

def mainSel ( name ) :
    return pydot.Node ( name                  ,
                        shape     = 'ellipse' ,
                        fillcolor = 'red'     , 
                        color     = 'red'     ,
                        style     = 'filled'  )

def reguSel ( name ) :
    return pydot.Node ( name ,
                        shape     = 'ellipse' ,
                        fillcolor = 'yellow'  , 
                        color     = 'yellow'  ,
                        style     = 'filled'  )

def filterSel ( name ) :
    return pydot.Node ( name ,
                        shape     = 'rectangle' ,
                        fillcolor = 'yellow'    , 
                        color     = 'yellow'    ,
                        style     = 'filled'    )

def combineSel ( name ) :
    return pydot.Node ( name ,
                        shape     = 'octagon'   ,
                        fillcolor = 'yellow'    , 
                        color     = 'red'       ,
                        style     = 'filled'    )

def tesSel  ( name ) :
    return pydot.Node ( name ,
                        label     = name.replace('_','/') , 
                        shape     = 'folder'              ,
                        fillcolor = 'green'               , 
                        color     = 'green'               ,
                        style     = 'filled'              )

def mergedSel  ( name ) :
    return pydot.Node ( name ,
                        shape     = 'note'                ,
                        fillcolor = 'yellow'              , 
                        color     = 'yellow'              ,
                        style     = 'filled'              )

def nodeSel ( sel ) :
    
    name = sel.name() 
    try : 
        from PhysSelPython.Wrappers import AutomaticData, MergedSelection 
        if   isinstance ( sel , AutomaticData   ) : return    tesSel ( name )
        elif isinstance ( sel , MergedSelection ) : return mergedSel ( name )
        
    except :
        pass

    try :    
        alg  = sel.algorithm()
        from Configurables import FilterDesktop, CombineParticles 
        if   isinstance ( alg , FilterDesktop    ) : return filterSel  ( name )
        elif isinstance ( alg , CombineParticles ) : return combineSel ( name )
    except:
        pass 
    
    return reguSel ( sel.name() ) 
    
class SelectionGraph ( object ) :

    def __init__ ( self , selections ) :
        
        self._nodes = set()
        self._edges = set() 
        self._graph = pydot.Dot ( 'graphname' , graph_type = 'digraph' )
        
        if not hasattr ( selections , '__iter__' ) : selections = [ selections ]

        _main = True 
        for sel in selections :
            
            name = sel.name()
            if name in self._nodes : continue
            
            self._nodes.add      ( name )
            if _main : node = mainSel ( name )
            else     : node = nodeSel ( sel  )
            _main = False
            
            self._graph.add_node ( node ) 
            
            self._add ( sel )

    ## add selection to the graph 
    def add_sel ( self , selection ) :
        """
        Add the selection to the grath 
        """
        n = selection.name()
        if not n in self._nodes :
            self._graph.add_node ( nodeSel ( selection ) )
            self._nodes.add ( n ) 
        self._add ( selection ) 

    def _add_edge ( self , node1 , node2 , **attrs ) :
        edge = node1,node2
        if not edge in self._edges :
            self._graph.add_edge ( pydot.Edge ( *edge, **attrs ) )
            self._edges.add      ( edge )
            
    def _add ( self , sel ) :
        
        reqsels = sel.requiredSelections()
        n1      = sel.name () 
        for rs in reqsels :
            n2 = rs.name()
            if not n2 in self._nodes :
                self._graph.add_node ( nodeSel ( rs ) )
                self._nodes.add      ( n2 ) 
            self._add_edge ( n2 , n1 ) 
            self._add ( rs ) ## recursion
            
        if reqsels : return
        
        ## merged selection?
        if hasattr ( sel , "_sel" ) :
            sel_    = sel._sel
            if hasattr ( sel_ , 'requiredSelections' ) :  
                reqsels = sel_.requiredSelections ()
                
        for rs in reqsels :
            n2 = rs.name()
            if not n2 in self._nodes :
                self._graph.add_node ( nodeSel ( rs ) )
                self._nodes.add      ( n2 ) 
            self._add_edge ( n2 , n1 , style='dotted' ) 
            self._add ( rs ) ## recursion

        
def dot ( selections ) :
    """
    Return a pydot.Dot object made from the edges of selection's graph.
    """
    g = SelectionGraph ( selections )
    return g._graph

# =============================================================================
## make graph for the given selection  
def graph ( selections , format = 'dot' , filename = None ) :
    """
    Make graph for the given selection
    
    >>> selection = ...
    >>> graph ( selections , format = 'jpeg' )
    
    """
    dt = dot ( selections )
    if not filename :
        if hasattr ( selections , 'name' )  : selname = selections.name() 
        else : selname = '_'.join( [ i.name() for i in selections ] )
        filename = selname + '.' + format
    #
    return filename if dt.write ( filename , format = format ) else None 

# =============================================================================
# prepare the graph and 
def view ( selection , command = None , format = 'png' ) :
    """
    View the graph for a Selection type object.
    Example:
    sel = Selection ( ... )
    view(sel)
    view(sel, command = 'open -a Preview')
    """

    class _TmpName_(object):
        def __init__ ( self , suffix = 'png' ) : self.suffix = '.' + suffix 
        def __enter__ ( self ) :
            import tempfile
            self.name = tempfile.mktemp ( suffix = self.suffix )
            return self.name 
        def __exit__  ( self , *_ ) :
            import os
            try :
                os.remove ( self.name )
            except:
                pass
            
    with _TmpName_( format ) as ofile :

        if not graph ( selection , format , filename = ofile ) :  
            print 'Error producing dot-graph for %s '% selection.name()
            return
        
        if not command: 
            import distutils.spawn  as ds
            for i in ( 'eog'     , 'display', 'gnome-photos'  , 
                       'gimp'    , 'gthumb' , 'google-chrome' ) :
                command = ds.find_executable ( i )
                if command : break
                
        if not command :
            print 'No valid command is found!'
            return
        
        import subprocess
        try:
            subprocess.check_call ( "%s %s " % ( command , ofile ) , shell = True )
        except subprocess.CalledProcessError :
            pass 
            
# =============================================================================
# The END
# =============================================================================
