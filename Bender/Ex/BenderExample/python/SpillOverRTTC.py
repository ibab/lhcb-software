#!/usr/bin/env python
# =============================================================================
# $Id: SpillOverRTTC.py,v 1.1 2005-06-14 16:17:19 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
# =============================================================================

import benderROOT as root

from bendermodule import gaudi
from bendermodule import Algo
from bendermodule import SUCCESS

gaudi.loaddict('BenderDetDict')

class SpillOverRTTC(Algo) :
  """ The main algorithm """
  def analyse ( self ) :
    """ The main method, it is invoked for each event """
    
    # get the event header
    evtHdr = self.get('Header')
    
    # get ecal digits
    digits = self.get('Raw/Ecal/Digits')
    
    # book/fill histogram
    self.plot ( digits.size()      , # value 
                '# of ecal digits' , # title 
                ID   = 1           , # ID (optional)
                low  = 0           , # optional
                high = 1000        , # optional
                bins = 100         ) # optional

    lhcb = self.getDet('/dd/Structure/LHCb')
    print lhcb, type(lhcb) 
    
    return SUCCESS
  
def configure() :
  """ Job configuration """
  gaudi.config( files   =
                [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                  '$DAVINCIROOT/options/DaVinciTestData.opts' ] ) # input data
  
  rttc = SpillOverRTTC('RTTC')  
  gaudi.setAlgorithms( [ rttc ] ) 
  
  # output histogram file 
  hsvc = gaudi.service( 'HistogramPersistencySvc' )
  hsvc.HistogramPersistency = "ROOT" 
  hsvc.OutputFile = 'rttc.root'
  
if __name__ == '__main__' :
  import sys 
  # configure the job
  configure() 
  # run job 
  gaudi.run  ( 100 )

  histo = gaudi.histoSvc()['RTTC/1']
  print histo

  root.plot( histo )
  
  # terminate the Application Manager 
  #gaudi.exit ()
  
