#!/usr/bin/env python
# =============================================================================
# $Id: VeloSpaceTracks.py,v 1.1 2005-06-14 16:17:19 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
# =============================================================================

# import benderROOT as root

from bendermodule import *

class VeloSpaceTrack(Algo) :
  """ The main algorithm """
  def analyse ( self ) :
    """ The main method, it is invoked for each event """

    # get the relation table TrgTrack->MCParticle form TES 
    table  = self.get('Rec/Relations/TrgTracks2MCParticles')

    # get teh container of TrgTracks 
    tracks = self.get('Rec/Trg/VeloSpaceTracks')
    if not tracks :
      print 'No trigger tracks are found '
      return SUCCESS
    
    print 'number of tracks=%d' % tracks.size()

    # loop over all tracks 
    for track in tracks :

      velos = track.veloReference ()
      its   = track.ITReference   ()
      ots   = track.OTReference   ()

      # overall number of 'hits' ?  
      nHits = velos.size() + its.size() + ots.size()
      print 'nHits: Total=%s' % nHits
      
      # loop over all velo references:
      for velo in velos :
        id     = velo.first
        number = velo.second
        
      # get all MC relations for the track:
      links = table.relations( track )
      for link in links :
        mc = link.to_() 
        print "MCParticle is '%s' PT=%s" % \
              ( nameFromPID( mc.particleID() ) , MCPT( mc ) / GeV  ) 
        
    return SUCCESS
  
def configure() :
  """ Job configuration """
  gaudi.config( files   =
                [ '$BENDEREXAMPLEOPTS/BenderExample.opts'    , # general options
                  '$TRGSYSROOT/options/L1.opts'              , # run L1
                  '$TRGSYSROOT/options/Hlt.opts'             , # run HLT
                  '$HLTGENERICROOT/options/HltGeneric.opts'  , # HLT generic
                  '$TRGSYSROOT/options/TrgChecking.opts'     , 
                  '$DAVINCIROOT/options/DaVinciTestData.opts' ] ,
                options =
                [  'TrgL0Filter.Enable = false'   ,
                   'TrgL1Filter.Enable = false'   ] ) # input data
  
  #gaudi.addAlgorithm( 'TrgVertexToContainer' )
  
  gaudi.addAlgorithm ( 'GaudiSequencer/Links2Tables' )
  seq = gaudi.algorithm ('Links2Tables')
  
  seq.Members = [ 'TrgTrackToContainer'             ,
                  'TrgTrack2MCParticle'             ,
                  'TrgLinks2TrgTables/Trg2MCTables' ]
  alg=gaudi.algorithm('Trg2MCTables')
  alg.Inputs = ['Rec/Trg/VeloSpaceTracks']

  veloTR = VeloSpaceTrack('RTTC')  
  gaudi.addAlgorithm( veloTR ) 
  
  # output histogram file 
  hsvc = gaudi.service( 'HistogramPersistencySvc' )
  hsvc.HistogramPersistency = "ROOT" 
  hsvc.OutputFile = 'rttc.root'

  print gaudi.algorithms()
  
if __name__ == '__main__' :
  import sys 
  # configure the job
  configure() 
  # run job 
  gaudi.run  ( 100 )
  
  histo = gaudi.histoSvc()['RTTC/1']
  print histo

  #root.plot( histo )

  # terminate the Application Manager 
  gaudi.exit ()
  
