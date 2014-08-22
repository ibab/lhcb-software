################################################################################
##                                                                            ##
##   File: TestBandQLinesFromStrippingSettings.py                             ##
##   Authors: Lucio.Anderlini@fi.infn.it, Roberta.Cardinale@ge.infn.it        ##
##                                                                            ##
##   Ganga wrapper for TestBandQLine.py script                                ##
##                                                                            ##
##   Run as:                                                                  ##
##     $ ganga                                                                ##
##      >>> execfile('TestBandQLines.ganga.py')                               ##
##      >>> Job(t).submit()                                                   ##
##                                                                            ##
################################################################################


def getDaVinciVersion( version ):
  if version == 'stripping21' : return 'v36r0'
  if version == 'stripping20' : return 'v32r2p12'
  raise NameError( 'STRIPPING VERSION '+version+' UNEXPECTED')


# Creates a new empty job template
stripping = 'stripping21'
t = JobTemplate()
t.application = DaVinci ( version = getDaVinciVersion(stripping) 
                          , optsfile = 'TestBandQLine.py'
                          , extraopts =  
                              "configureStrippingTest('BandQ', ['BandQ'])"
                        )

t.backend = Local()
fpj = 1
t.splitter = SplitByFiles(maxFiles = 7*fpj, filesPerJob = fpj)

print 'Connection to bookkeeping database... be patient'
t.inputdata = BKQuery('/LHCb/Collision12/Beam4000GeV-VeloClosed-MagDown/Real Data/Reco14/90000000/FULL.DST', dqflag=['OK']).getDataset().files

print """
  A JobTemplate instance has been created and named t.
  Default backend is Local()
  To test the BandQ stripping lines: 
 
      >>> Job(t).submit()

"""




