__drawOptions = { 'title' : '', 'mstyle' : 1, 'msize' : 1, 'mcolour' : 1 }

def __parseDrawOptions( kwargs ) :
   for i in kwargs :
      if i in __drawOptions :  __drawOptions[ i ] = kwargs[ i ]
      else : print 'Could not set unknown property %s'%i

def __zeros( n ) :
   l = []
   for i in range( n ) : l.append( 0 )
   return l

def TGraph( x, y, yError = None, xError = None, **kwargs) :
   
   __parseDrawOptions( kwargs )
   
   import sys
   #TODO: Need to check the types
   try:
      assert( len( x ) == len( y )  )
   except AssertionError:
      print "Oops! Data points x and y aren't of the same size!"
      sys.exit()
      
   nPoints = len( x )
    
   from array import array
   if xError is None : xError = __zeros( nPoints )
   if yError is None : yError = __zeros( nPoints )
   
   from ROOT import TGraphErrors
   graph = TGraphErrors( nPoints, array( 'd', x ), array( 'd', y ), array( 'd', xError ), array( 'd', yError ) )
   graph.SetTitle( __drawOptions[ 'title' ] )
   graph.SetMarkerStyle( __drawOptions[ 'mstyle' ] )
   graph.SetMarkerSize( __drawOptions[ 'msize' ] )
   graph.SetMarkerColor( __drawOptions[ 'mcolour' ] )
   graph.SetFillStyle( 0 )
   graph.SetFillColor( 0 )
   return graph

def save( canvas, extension ) :
   from ROOT import TCanvas
   key = raw_input( 'Press "s" to save, "Enter" to continue or "q" to quit:' )
   if key == 's': 
      canvas.Print( canvas.GetName() + '.' + extension )
   elif key == 'q' :
      raise SystemExit( 'Quit' )