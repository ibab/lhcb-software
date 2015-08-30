#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Graph-related decorations 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""TGraph-related decorations"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = (
    'makeGraph' , ## make graph from primitive data
    'hToGraph'  , ## convert histogram to graph 
    'hToGraph2' , ## convert histogram to graph 
    'hToGraph3' , ## convert histogram to graph
    'lw_graph'  , ## make Laffery-Wyatt's graph
    ##
    ) 
# =============================================================================
import ROOT, cppyy              ## attention here!!
cpp = cppyy.makeNamespace('')
VE  = cpp.Gaudi.Math.ValueWithError 
# 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger( 'Ostap.GraphDeco' )
else                       : logger = getLogger( __name__ )
# =============================================================================
logger.debug ( 'Graph-related decorations')
# =============================================================================
## copy graph attributes 
def copy_graph_attributes ( o_from , o_to ) :
    """
    copy graph attributes 
    """
    ##
    ## line attributes:
    ## 
    if hasattr ( o_from , 'GetLineColor' ) and hasattr ( o_to , 'SetLineColor' ) :
        o_to.SetLineColor   ( o_from.GetLineColor () ) 
    if hasattr ( o_from , 'GetLineWidth' ) and hasattr ( o_to , 'SetLineWidth' ) :
        o_to.SetLineWidth   ( o_from.GetLineWidth () ) 
    if hasattr ( o_from , 'GetLineStyle' ) and hasattr ( o_to , 'SetLineStyle' ) :
        o_to.SetLineStyle   ( o_from.GetLineStyle () ) 
    ##
    ## marker attributes:
    ## 
    if hasattr ( o_from , 'GetMarkerColor' ) and hasattr ( o_to , 'SetMarkerColor' ) :
        o_to.SetMarkerColor ( o_from.GetMarkerColor () ) 
    if hasattr ( o_from , 'GetMarkerSize'  ) and hasattr ( o_to , 'SetMarkerSize'  ) :
        o_to.SetMarkerSize  ( o_from.GetMarkerSize  () ) 
    if hasattr ( o_from , 'GetMarkerStyle' ) and hasattr ( o_to , 'SetMarkerStyle' ) :
        o_to.SetMarkerStyle ( o_from.GetMarkerStyle () ) 
    ##
    ## Fill attributes:
    ##
    if hasattr ( o_from , 'GetFillColor' ) and hasattr ( o_to , 'SetFillColor' ) :
        o_to.SetFillColor   ( o_from.GetFillColor () ) 
    if hasattr ( o_from , 'GetFillStyle' ) and hasattr ( o_to , 'SetFillStyle' ) :
        o_to.SetFillStyle   ( o_from.GetFillStyle () ) 
    
# =============================================================================
## make graph from data 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def makeGraph ( x , y = []  , ex = [] , ey = [] ) :
    """
    Make graph using the primitive data
    """
    if  isinstance ( x , dict ) and not y and not ex and not ey : 
        _x = []
        _y = []
        keys = x.keys()
        keys.sort()
        for k in keys :
            _x += [   k  ]
            _y += [ x[k] ] 
        return makeGraph ( _x , _y )
        
    if  not x : raise TypeError, "X is not a proper vector!"
    if  not y : raise TypeError, "Y is not a proper vector!"
    if len( x ) != len ( y ) :
        raise TypeError, "Mismatch X/Y-lengths"

    if ex and len(ex) != len(x) : raise TypeError, "Mismatch X/eX-lengths"
    if ey and len(ey) != len(y) : raise TypeError, "Mismatch Y/eY-lengths"

    gr = ROOT.TGraphErrors ( len(x) ) 
        
    for i in range ( 0 , len(x) ) :
        
        if ex : _ex = ex[i]
        else  : _ex = 0.0
        if ey : _ey = ey[i]
        else  : _ey = 0.0

        _x = x[i]
        if hasattr ( x[i] , 'value' ) : _x  = x[i].value ()        
        if hasattr ( x[i] , 'error' ) : _ex = x[i].error ()

        _y = y[i]
        if hasattr ( y[i] , 'value' ) : _y  = y[i].value ()        
        if hasattr ( y[i] , 'error' ) : _ey = y[i].error ()
                    
        gr .SetPoint      ( i ,  _x ,  _y )
        gr .SetPointError ( i , _ex , _ey )
        
    return gr

# =============================================================================
## convert histogram to graph
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def hToGraph ( h1                   ,
               funcx = lambda s : s ,
               funcv = lambda s : s ) :
    """
    Convert  1D-histogram into graph 
    """
    #
    ## book graph
    #
    graph = ROOT.TGraphErrors( len ( h1 )  - 2 )
    
    #
    ## copy attributes
    #
    copy_graph_attributes ( h1 , graph )

    for i in h1.iteritems () :

        x = funcx  ( i[1] ) 
        v = funcv  ( i[2] )

        ## note the different convention 
        graph [ i[0] - 1 ] = x , v 
        
    return graph

# =============================================================================
## convert histogram to graph
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def hToGraph_ ( h1 , funcx , funcy ) :
    """
    Convert  1D-histogram into TGraphAsymmError 
    """
    #
    ## book graph
    #
    graph = ROOT.TGraphAsymmErrors( len ( h1 )  - 2 )
    
    #
    ## copy attributes
    #
    copy_graph_attributes ( h1 , graph )


    for i in h1.iteritems () :
        
        ip = i[0] - 1 ## different convention for TH1 and TGraph
        x  = i[1] 
        y  = i[2]

        x0 , xep , xen = funcx ( x , y )
        y0 , yep , yen = funcy ( x , y )

        graph.SetPoint      ( ip , x0  , y0  ) 
        graph.SetPointError ( ip , xep , xen , yep , yen ) 

    return graph


# =============================================================================
## convert histogram to graph
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def hToGraph2 ( h1 , bias ) :
    """
    Convert  1D-histogram into graph with small shift in x
    Useful for overlay of very similar plots

    >>> h1 = ....
    >>> g2 = h1.asGraph2 ( 0.1 ) ## shift for 10% of bin width
    
    """
    if abs ( bias ) > 1 :
        raise ValueError, ' Illegal value for "bias" parameter '
    
    funcx = lambda x,y : ( x.value() + x.error()*bias , x.error()*(1+bias) , x.error()*(1-bias) ) 
    funcy = lambda x,y : ( y.value()                  , y.error()          , y.error()          ) 
        
    return hToGraph_ ( h1 , funcx , funcy ) 


# =============================================================================
## convert histogram to graph
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def hToGraph3 ( h1 , bias ) :
    """
    Convert  1D-histogram into graph with small shift in x
    Useful for overlay of very similar plots

    >>> h1 = ....
    >>> g2 = h1.asGraph2 ( 0.1 ) ## shift for 0.1 (absolute)
    
    """
    for p in h1.iteritems() :
        x = p[1]
        if x.error() < abs ( bias ) :
            raise ValueError, ' Illegal value for "bias" parameter '
        
    funcx = lambda x,y : ( x.value() + bias , x.error()+bias , x.error()-bias )
    funcy = lambda x,y : ( y.value()        , y.error()      , y.error()      ) 
        
    return hToGraph_ ( h1 , funcx , funcy ) 


ROOT.TH1F.asGraph2 = hToGraph2
ROOT.TH1D.asGraph2 = hToGraph2
ROOT.TH1F.toGraph2 = hToGraph2
ROOT.TH1D.toGraph2 = hToGraph2
ROOT.TH1F.asGraph3 = hToGraph3
ROOT.TH1D.asGraph3 = hToGraph3
ROOT.TH1F.toGraph3 = hToGraph3
ROOT.TH1D.toGraph3 = hToGraph3


# =============================================================================
# iterate over graph items
# =============================================================================
## iterate over points in TGraphErrors
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _gr_iter_ ( graph ) :
    """
    Iterate over graph points 
    >>> gr = ...
    >>> for i in gr : ...
    
    """
    for ip in range ( 0 , len ( graph ) ) :
        yield ip
        
# =============================================================================
## get the point in TGraph
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _gr_getitem_ ( graph , ipoint )  :
    """
    Get the point from the Graph
    """
    if not ipoint in graph : raise IndexError 
    #
    
    x_ = ROOT.Double(0)
    v_ = ROOT.Double(0)
    
    graph.GetPoint ( ipoint , x_ , v_ )
    
    return x_,v_

# =============================================================================
## set the point in TGraph
#  @code
#  graph = ...
#  x,y   = graph[1]
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _gr_setitem_ ( graph , ipoint , point )  :
    """
    Get the point from the Graph
    >>> graph = ...
    >>> x , y = graph[1]
    """
    #
    if not ipoint in graph : raise IndexError 
    #
    
    x = VE ( point[0] ).value () 
    v = VE ( point[1] ).value () 
    
    graph.SetPoint      ( ipoint , x , v )


# =============================================================================
## iterate over the points in TGraph
#  @code 
#  gr = ...
#  for i,x,v in gr.iteritems(): ...
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _gr_iteritems_ ( graph ) :
    """
    Iterate over graph points 
    >>> graph = ...
    >>> for i,x,v in graph.iteritems(): ...
    """
    for ip in graph :
        x , y = graph[ ip ] 
        yield ip , x , y 
        
        
# =============================================================================
## get the point in TGraphErrors
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _gre_getitem_ ( graph , ipoint )  :
    """
    Get the point from the Graph
    """
    if not ipoint in graph : raise IndexError 
    #
    
    x_ = ROOT.Double(0)
    v_ = ROOT.Double(0)
    
    graph.GetPoint ( ipoint , x_ , v_ )
    
    x = VE ( x_ , graph.GetErrorX ( ipoint )**2 )
    v = VE ( v_ , graph.GetErrorY ( ipoint )**2 )
    
    return x , v

# =============================================================================
## set the point in TGraphErrors
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _gre_setitem_ ( graph , ipoint , point )  :
    """
    Get the point from the Graph
    """
    #
    if not ipoint in graph    : raise IndexError
    if not 2 == len ( point ) :
        raise AttributeError("Invalid dimension of 'point'")
    
    x = VE ( point[0] ) 
    v = VE ( point[1] ) 

    graph.SetPoint      ( ipoint , x . value () , v . value () )
    graph.SetPointError ( ipoint , x . error () , v . error () )


# =============================================================================
## iterate over points in TGraphErrors
#  @code
#  gre = ...
#  for i,x,v in gre.iteritems(): ...
#  @endcode
#  @see TGraphErrors
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _gre_iteritems_ ( graph ) :
    """
    Iterate over graph points 
    >>> gre = ...
    >>> for i,x,v in gre.iteritems(): ...
    """
    for ip in graph :        
        x , y = graph[ip]
        yield ip , x , y 

# =============================================================================
## iterate over points in TGraphAsymmErrors
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _grae_getitem_ ( graph , ipoint ) :
    """
    >>> grae = ...
    >>> x,xl,xh,y,yl,yh = grae[ 1 ]
    """
    if not ipoint in graph : raise IndexError 
    #
    
    x_ = ROOT.Double(0)
    v_ = ROOT.Double(0)
    
    graph.GetPoint ( ipoint , x_ , v_ )
    
    exl = graph.GetErrorXlow  ( ipoint )
    exh = graph.GetErrorXhigh ( ipoint )
    eyl = graph.GetErrorYlow  ( ipoint )
    eyh = graph.GetErrorYhigh ( ipoint )
    
    return float( x_ ) , -exl , exh , float( v_ ) , -eyl , eyh 

# =============================================================================
## iterate over points in TGraphAsymmErrors
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _grae_setitem_ ( graph , ipoint , point ) :
    """
    Iterate over graph points 
    
    >>> grae = ...
    >>> for i,x,xl,xh,y,yl,yh in grae.iteritems(): ...
    
    """
    if not ipoint in graph : raise IndexError
    if 6 != len(point)     : raise AttributeError("Invalid lenght of 'point'")
    # 
    x   =       point[0]
    exl = abs ( point[1] )  ## allow them to be negative, to improve input format 
    exh =       point[2]
    y   =       point[3]
    eyl = abs ( point[4] )  ## allow them to be  negative to improve input format
    eyh =       point[5] 
    
    graph.SetPoint      ( ipoint , x   , y )
    graph.SetPointError ( ipoint , exl , exh , eyl , eyh )



# =============================================================================
## iterate over points in TGraphAsymmErrors
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _grae_iteritems_ ( graph ) :
    """
    Iterate over graph points 
    
    >>> grae = ...
    >>> for i,x,xl,xh,y,yl,yh in grae.iteritems(): ...
    
    """
    for ip in graph :
        vars = graph[ip]        
        yield (ip,) + vars
            
# =============================================================================
ROOT.TGraph       . __len__       = ROOT.TGraphErrors . GetN 
ROOT.TGraph       . __contains__  = lambda s,i : i in range(0,len(s))
ROOT.TGraph       . __iter__      = _gr_iter_ 

ROOT.TGraph       . __getitem__   = _gr_getitem_ 
ROOT.TGraph       . __setitem__   = _gr_setitem_
ROOT.TGraph       . iteritems     = _gr_iteritems_

ROOT.TGraphErrors . __getitem__   = _gre_getitem_ 
ROOT.TGraphErrors . __setitem__   = _gre_setitem_ 
ROOT.TGraphErrors . iteritems     = _gre_iteritems_ 

ROOT.TGraph       . __call__      = ROOT.TGraph.Eval

ROOT.TH1F.asGraph = hToGraph
ROOT.TH1D.asGraph = hToGraph
ROOT.TH1F.toGraph = hToGraph
ROOT.TH1D.toGraph = hToGraph

ROOT.TGraphAsymmErrors.__len__       = ROOT.TGraphAsymmErrors . GetN 
ROOT.TGraphAsymmErrors.__contains__  = lambda s,i : i in range(0,len(s))
ROOT.TGraphAsymmErrors.__iter__      = _gr_iter_ 
ROOT.TGraphAsymmErrors. iteritems    = _grae_iteritems_ 
ROOT.TGraphAsymmErrors.__getitem__   = _grae_getitem_ 
ROOT.TGraphAsymmErrors.__setitem__   = _grae_setitem_ 

# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _color_ ( self , color = 2 , marker = 20 , size = -1 ) :
    """
    Set color attributes

    >>> h.color ( 3 ) 
    """
    #
    if hasattr ( self , 'SetLineColor'   ) : self.SetLineColor   ( color  )
    if hasattr ( self , 'SetMarkerColor' ) : self.SetMarkerColor ( color  )
    if hasattr ( self , 'SetMarkerStyle' ) : self.SetMarkerStyle ( marker )
    ##
    if 0 > size and hasattr ( self , 'GetMarkerSize' ) and not marker in ( 1 , 6 , 7 ) :
        size = self.GetMarkerSize()
        if   marker in ( 22 , 23 , 29 , 33 ) : size *= 2.0 ## small filled objects
        elif marker in ( 24 , 25 , 28 , 4  ) : size *= 1.5 ## large open objects  
        elif marker in ( 26 , 27 , 30 , 32 ) : size *= 2.2 ## small open objects
    ##
    if 0 < size and hasattr ( self , 'SetMarkerSize' ) :
        self.SetMarkerSize ( size )
    #
    return self
# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _red_     ( self , marker   = 20 ) : return _color_( self , 2 , marker ) 
# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _blue_    ( self , marker   = 25 ) : return _color_( self , 4 , marker )
# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _magenta_ ( self , marker   = 22 ) : return _color_( self , 6 , marker ) 
# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _cyan_    ( self , marker   = 23 ) : return _color_( self , 7 , marker ) 
# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _green_   ( self , marker = 33 ) : return _color_( self , 8 , marker ) 
# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _yellow_  ( self , marker = 34 ) : return _color_( self , 92 , marker ) 


for _t in  ( ROOT.TH1D , ROOT.TH1F , ROOT.TGraph , ROOT.TGraphErrors ) :
    
    _t . color   = _color_
    _t . red     = _red_
    _t . blue    = _blue_
    _t . magenta = _magenta_
    _t . cyan    = _cyan_
    _t . green   = _green_
    _t . yellow  = _yellow_

# =============================================================================
## min-value for the graph)
def _gr_xmax_ ( graph ) :
    """
    Get x-max for the graph
    
    >>> xmax = graph.xmax()

    """
    #
    _size = len ( graph )
    if 0 == _sise : return 1
    #
    _last = _size - 1
    x_ = ROOT.Double(0)
    v_ = ROOT.Double(0)    
    g.GetPoint ( _last , x_ , v_ )
    #
    return x_
# =============================================================================
## min-value for the graph)
def _gr_xmin_ ( g ) :
    """
    Get x-min for the graph
    
    >>> xmin = graph.xmin()
    
    """
    #
    _size = len ( graph )
    if 0 == _sise : return 0
    #
    x_ = ROOT.Double(0)
    v_ = ROOT.Double(0)    
    graph.GetPoint ( 0 , x_ , v_ )
    #
    return x_

# =============================================================================
## minmax-value for the graph)
def _gr_xminmax_ ( graph ) :
    """
    Get x-minmax for the graph

    >>> xmin,xmax = graph.xminmax() 
    """
    #
    return (graph.xmin(), graph.xmax() )

ROOT.TGraph  . xmin    = _gr_xmin_
ROOT.TGraph  . xmax    = _gr_xmax_
ROOT.TGraph  . xminmax = _gr_xminmax_


# =============================================================================
## Convert the histogram to into "Laffery-Wyatt" graph
#  See G.D. Lafferty and T.R. Wyatt,
#  ``Where to stick your data points: The treatment of measurements within wide bins,''
#  Nucl. Instrum. Meth. A355, 541 (1995).
#  @param histo  the histogram
#  @param func   the model
#  @attention: the model can be any reasonable model.
#  No need in multiplicative and additive terms:
#  the affine transformations do not affect the result.
#  e.g. following three graphs are equivalent: 
#  @code
#  >>> histo = ...
#  >>> gr1   = histo.lw_graph ( lambda x :     math.exp(-x)    )
#  >>> gr2   = histo.lw_graph ( lambda x : 100*math.exp(-x)    )
#  >>> gr3   = histo.lw_graph ( lambda x : 100*math.exp(-x)-10 )
#  @endcode 
#  If no reasonable model is known, the splines can be used instead: 
#  @code
#  >>> histo  = 
#  >>> spline = histo.(p,i,d)spline( .... )
#  >>> graph  = histo.lw_graph ( spline[2] ) 
#  @endcode 
#  @see http://dx.doi.org/10.1016/0168-9002(94)01112-5
#  @author  Vanya BELYAEV  Ivan.Belyaev@itep.ru
#  @date    2014-12-08
def _lw_graph_ ( histo , func ) :
    """
    Convert the histogram to into ``Laffery-Wyatt'' graph
    See G.D. Lafferty and T.R. Wyatt,
    ``Where to stick your data points: The treatment of measurements within wide bins,''
    Nucl. Instrum. Meth. A355, 541 (1995).

    >>> histo = ... ## the histogram
    
    ## the explicit model:
    >>> graph  = histo.lw_graph ( lambda x : math.exp(-x) )
    
    ## use splines:
    >>> spline = histo.(p,i,d)spline( .... ) 
    >>> graph  = histo.lw_graph ( spline[2] )
    >>> histo.Draw('e1')
    >>> graph.Draw('e1p same')    
    """
    
    #
    ## book graph
    #
    graph = ROOT.TGraphAsymmErrors( len ( histo )  - 2 )
        
    #
    ## copy attributes
    #
    copy_graph_attributes ( histo , graph )

    #
    ## start actual evaluations
    #
    
    from scipy import integrate
    from scipy import optimize 
    
    for item in histo.iteritems () :

        ibin  = item[0]
        x     = item[1]
        y     = item[2]

        yv    = y.value()
        ye    = y.error()
        
        xv    = x.value() 
        xe    = x.error()
        
        xmx   = xv + xe 
        xmn   = xv - xe 

        #
        ##  solve the equation f(x) = 1/dx*int(f,xmin,xmax)
        #

        ##  1) calculate int(f,xmin,xmax)
        fint  = integrate.quad ( lambda x : float ( func ( x ) ) , xmn , xmx ) 

        ## bin-width 
        dx    = 2.0 * xe 
        
        fx    = float ( fint[0]/dx  ) 
        
        fxmin = float ( func ( xmn ) ) 
        fxmax = float ( func ( xmx ) ) 
        
        if 0 <= ( fxmin - fx ) * ( fxmax - fx ) : 
            logger.warning('Lafferty-Wyatt graph: invalid point: %s ' % x )
            r0 = x.value()
        else :
            ##  solve the equation f(x) - 1/dx*int(f,xmin,xmax) = 0 
            r0 = optimize.brentq ( lambda x : (float(func(x))-fx)  ,
                                   xmn               ,
                                   xmx               ,
                                   xtol = 0.005 * dx )
            
        ## fill graph
            
        ip   = ibin - 1 ## different conventions for TGraph and  TH1 

        xep  = xmx - r0
        xen  =       r0 - xmn
        
        graph.SetPoint      ( ip , r0  ,  yv           )
        graph.SetPointError ( ip , xen , xep , ye, ye  )

    return graph

ROOT.TH1D.lw_graph = _lw_graph_
ROOT.TH1F.lw_graph = _lw_graph_

# =============================================================================
## Convert the histogram to into "Laffery-Wyatt" graph
#  See G.D. Lafferty and T.R. Wyatt,
#  ``Where to stick your data points: The treatment of measurements within wide bins,''
#  Nucl. Instrum. Meth. A355, 541 (1995).
#  @param histo  the histogram
#  @param func   the model
#  @attention: the model can be any reasonable model.
#  No need in multiplicative and additive terms:
#  the affine transformations do not affect the result.
#  e.g. following three graphs are equivalent: 
#  @code
#  >>> histo = ...
#  >>> gr1   = lw_graph ( histo , lambda x :     math.exp(-x)    )
#  >>> gr2   = lw_graph ( histo , lambda x : 100*math.exp(-x)    )
#  >>> gr3   = lw_graph ( histo , lambda x : 100*math.exp(-x)-10 )
#  @endcode 
#  If no reasonable model is known, the splines can be used instead: 
#  @code
#  >>> histo  = 
#  >>> spline = histo.(p,i,d)spline( .... )
#  >>> graph  = lw_graph ( histo ,  spline[2] ) 
#  @endcode 
#  @see http://dx.doi.org/10.1016/0168-9002(94)01112-5
#  @author  Vanya BELYAEV  Ivan.Belyaev@itep.ru
#  @date    2014-12-08
def lw_graph ( histo , func ) :
    """
    Convert the histogram to into ``Laffery-Wyatt'' graph
    See G.D. Lafferty and T.R. Wyatt,
    ``Where to stick your data points: The treatment of measurements within wide bins,''
    Nucl. Instrum. Meth. A355, 541 (1995).

    >>> histo = ... ## the histogram
    
    ## the explicit model:
    >>> graph  = lw_graph ( histo , lambda x : math.exp(-x) )
    
    ## use splines:
    >>> spline = histo.(p,i,d)spline( .... ) 
    >>> graph  = lw_graph ( histo , spline[2] )
    >>> histo.Draw('e1')
    >>> graph.Draw('e1p same')    
    """
    return _lw_graph_ ( histo , func ) 

# =============================================================================
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 

# =============================================================================
# The END 
# =============================================================================
