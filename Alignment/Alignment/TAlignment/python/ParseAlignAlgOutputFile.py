import os, sys, re

path = '/afs/cern.ch/user/j/janos/Al/Alignment_v3r1/Alignment/TAlignment/python/'
#log    = path + 'AlignLayersWithCosmicsNoITHitsNoConstraints.log'
log       = path +  'AlignLayersWithCosmicsNoITHitsUseNominalConstraints.log' #'AlignLayersAsModulesWithCosmicsNoITHitsConstraintTxTySzxSzyRun5Iterations.log' #'AlignLayersWithCosmicsNoITHitsConstrainTrxTryTrtxTrtyRun5Iterations.log' #'AlignLayersWithCosmicsNoITHitsConstrainFirst2andLast2LayersRun5Iterations' #'AlignLayersWithCosmicsNoITHitsNoConstraints.log'
job       = 'ConstrainToNominal'
extension = 'pdf' 

import AlLogParser as parser
inFile = parser.open( log  )

rePivot        = re.compile( r"\*\s*PivotXYZ\s*:\s*(?P<pivotXYZ>(.*))" )
reIter         = re.compile( r"Iteration:\s*(?P<iter>(\d*))" )
reTracks       = re.compile( r"Total number of tracks:\s*(?P<tracks>(\d*))" )
reAlTracks     = re.compile( r"Used\s*(?P<tracks>(\d*))\s*tracks for alignment" )
reTrackChi2DoF = re.compile( r"Track chisquare/dof:\s*(?P<chi2dof>%s)"%(parser.regexFloats) )
reAlChi2Sig    = re.compile( r"Normalised alignment change chisquare:\s*(?P<alchi2sig>%s)"%(parser.regexFloats) )
reHits         = re.compile( r"Number of hits/outliers seen:\s*(?P<hits>(\d*))\s*(?P<outliers>(\d*))" ) 
reAlConditions = re.compile( r"==>\s*iteration\s*(?P<iter>(\d*))\s*:\s*Initial\s*alignment\s*conditions\s*:\s*(?P<cond>(.*))" )
reTx           = parser.reDoF( "Tx" )

pivots      = []
iters       = []
iter        = ''
# Probably need classes for the following?
trackInfo   = { 'tracks'  : 0, 'alTracks' : 0,  'chi2DoF' : 0.0, 'alChi2Sig': 0.0}
elementInfo = { 'current' : 0.0, 'delta'  : 0.0, 'error' : 0.0, 'hits': 0, 'outliers': 0 }
info        = { }

for line in inFile :
   match = rePivot.search( line )
   if match :
      pivot = list( match.group( 'pivotXYZ' ).split( ',' ) )
      for i in pivot: pivot.insert( pivot.index( i ), float( pivot.pop( pivot.index( i ) ).strip( '()' ) ) )
      pivots.append( pivot )

   match = reAlConditions.search( line )
   if match :
      match.group( 'iter' )
      tmp = list( match.group( 'cond' ).split(', ') )
      for i in tmp: tmp.insert( tmp.index( i ), float( tmp.pop( tmp.index( i ) ).strip( '[]' ) ) )
      
   match = reIter.search( line )
   if match :
      iter = match.group( 'iter' )
      #print "Iteration: " + iter
      # key is iteration : value is list of track & element info
      info[ iter ] = []
      # 0 -> 1
      iters.append( float( iter ) + 1.0 )
            
   match = reTracks.search( line )
   if match : 
      #print "Tracks used: " + match.group( 'tracks' )
      trackInfo[ 'tracks'] = int( match.group( 'tracks' ) )
      
   match = reAlTracks.search( line )
   if match : 
      #print "Tracks used for alignment: " + match.group( 'tracks' )
      trackInfo[ 'alTracks'] = int( match.group( 'tracks' ) )
      
   match = reTrackChi2DoF.search( line )
   if match :
      trackInfo[ 'chi2DoF' ] = float( match.group( 'chi2dof' ) )
      
   match = reAlChi2Sig.search( line )
   if match :
      trackInfo[ 'alChi2Sig' ] = float( match.group( 'alchi2sig' ) )
      info[ iter ].append( trackInfo.copy() )   
   
   match = reHits.search( line)
   if match : 
      #print "hits: " + match.group( 'hits' )
      elementInfo[ 'hits' ] = int( match.group( 'hits' ) )
   
   match = parser.AlDelta( line, reTx )   
   if match:
      elementInfo[ 'current'] = float( match[ 'current' ] )
      elementInfo[ 'delta'  ] = float( match[ 'delta'   ] )
      elementInfo[ 'error'  ] = float( match[ 'error'   ] )
      info[iter].append( elementInfo.copy() )

# Close file. Don't need it anymore
del iter   
parser.close( inFile )

from ROOT import gStyle
import RootStyle
RootStyle.setPlainStyle( gStyle )
   
from ROOT import ( TCanvas, TGraphErrors )
import RootWrappers

tracks     = []
chi2DoFs   = []
alChi2Sigs = []
elements   = {}

# sort (sic) according to iteration
for i in range( len(info) ) :
   trackInfo = info[ str( i ) ][0]
   tracks.append( trackInfo['tracks'] )
   chi2DoFs.append( trackInfo[ 'chi2DoF' ] )
   alChi2Sigs.append( trackInfo[ 'alChi2Sig' ] )
   # key is iteration value is sorted list of elements 
   elements[ str( i ) ] = info[ str( i ) ][1:]

iterations = range( len( info ) )
for index, value in enumerate( iterations ) : iterations[ index ] = value + 1

canvas = TCanvas( job+'TracksVsIter', job+'TracksVsIter', 0, 0, 700, 700 )
canvas.UseCurrentStyle()
graph  = RootWrappers.TGraph( iterations, tracks, mstyle=8, msize=1 ).Clone()
graph.SetMinimum( 900.0 )
graph.SetMaximum( 930.0 )
graph.Draw( 'AP' )
canvas.Update()
RootWrappers.save( canvas, extension )

canvas.Clear()
canvas.SetName( job+'Chi2PerDoFvsIter')
canvas.SetTitle( job+'Chi2PerDoFvsIter')
graph = RootWrappers.TGraph( iterations, chi2DoFs, mstyle=8, msize=1 ).Clone()
graph.Draw( 'AP' )
canvas.Update()
RootWrappers.save( canvas, extension )

canvas.Clear()
canvas.SetName( job+'AlChi2SigVsIter' )
canvas.SetTitle( job+'AlChi2SigVsIter' )
graph = RootWrappers.TGraph( iterations, alChi2Sigs, mstyle=8, msize=1 ).Clone()
graph.Draw( 'AP' )
canvas.Update()
RootWrappers.save( canvas, extension )

canvas.Clear()
canvas.SetName( job+'nHitsVsIter' )
canvas.SetTitle( job+'nHitsVsIter' )

nElements = len( elements[ '0' ])

colours  = { 'black' : 1, 'red' : 632, 'green' : 416 }
colour   = colours.itervalues()

# sort (sic) according to iteration
for i in range( nElements ) :
   nHits = []
   for e in range( len( elements) ) :
      nHits.append( elements[ str( e ) ][ i ][ 'hits' ] )
   if i%4 is 0 : c = colour.next()
   graph = RootWrappers.TGraph( iterations, nHits, title='Layer %s'%( str( i ) ), 
                                mstyle=20+i%4, msize=1, mcolour=c ).Clone()
   if i is 0 :    
      graph.Draw( 'AP' )
      graph.SetMinimum( 900.0 )
      graph.SetMaximum( 1800.0 )
   else : 
      graph.Draw( 'P' )

canvas.BuildLegend()
canvas.Update()

RootWrappers.save( canvas, extension )   

canvas.Clear()
canvas.SetName( job+'AlDeltaVsIter' )
canvas.SetTitle( job+'AlDeltVsIter' )

canvas.Divide( 4, 3 )

import ROOT   
# sort (sic)
for i in range( nElements ) :
   AlCurrent    = []
   AlDelta      = []
   AlDeltaError = []
   for e in range( len( elements) ) :
      AlCurrent.append( elements[ str( e ) ][ i ][ 'current' ] )
      AlDelta.append( elements[ str( e ) ][ i ][ 'delta' ] )
      AlDeltaError.append( elements[ str( e ) ][ i ][ 'error' ] )
  
   canvas.cd( i + 1 )
   graph = RootWrappers.TGraph( iterations, AlCurrent, AlDeltaError, title='', mstyle=20+i%4, msize=0.50, mcolour=ROOT.kBlack ).Clone()
   graph.SetMinimum( -1.0 )
   graph.SetMaximum( 1.0 )
   graph.Draw( 'AP' )
   graph = RootWrappers.TGraph( iterations, AlDelta, AlDeltaError, title='', mstyle=20+i%4, msize=0.50, mcolour=ROOT.kRed ).Clone()
   graph.Draw( 'P' )
   
canvas.Update()

RootWrappers.save( canvas, extension )

canvas.Clear()
canvas.SetName( job+'AlDeltaVsZ' )
canvas.SetTitle( job+'AlDeltVsZ' )

iteration = str( len( info ) - 1 )
layerZs      = []
AlCurrent    = []
AlDelta      = []
AlDeltaError = []
for i in range( nElements ) :
   AlCurrent.append( elements[ iteration ][ i ][ 'current' ] )
   AlDelta.append( elements[ iteration ][ i ][ 'delta' ] )
   AlDeltaError.append( elements[ iteration ][ i ][ 'error' ] )
   layerZs.append( pivots[i][2] )
graph = RootWrappers.TGraph( layerZs, AlCurrent, AlDeltaError, title='', mstyle=20, msize=1.0, mcolour=ROOT.kBlack ).Clone()
graph.SetMinimum( -2.0 )
graph.SetMaximum( 2.0 )
graph.Draw( 'AP' )
canvas.Update()

RootWrappers.save( canvas, extension )
