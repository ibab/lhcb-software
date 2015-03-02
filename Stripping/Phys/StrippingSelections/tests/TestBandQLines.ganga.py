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
                          #, setupProjectOptions = "--nightly lhcb-head"
                          , optsfile = 'TestBandQLine.py'
                          , extraopts =  
                              "configureStrippingTest('BandQ', ['BandQ'])"
                        )
t.application.extraopts += """
DaVinci().EvtMax = 1000
"""

t.backend = Local()
fpj = 1
t.splitter = SplitByFiles(maxFiles = 1*fpj, filesPerJob = fpj)
t.outputfiles = [LocalFile('*.dst'), LocalFile('*.mdst')]

t.inputdata = LHCbDataset()
t.inputdata.files = [
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042960_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042961_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042962_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042963_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042964_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042966_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042967_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042968_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042969_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042970_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042971_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042972_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042973_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042974_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042975_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042976_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042977_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042978_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042979_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042980_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042981_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042982_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042983_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042984_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042985_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042986_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042987_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042988_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042989_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042990_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042991_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042992_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042993_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042994_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042996_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042997_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042998_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00042999_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043000_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043001_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043002_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043003_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043004_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043005_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043006_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043007_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043008_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043009_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043010_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043011_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043012_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043013_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043014_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043015_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043016_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043017_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043019_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043020_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043021_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043023_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043024_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043025_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043026_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043027_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043028_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043029_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043030_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043031_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043032_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043033_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043034_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043035_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043036_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043037_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043038_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043039_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043040_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043041_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043042_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043043_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043044_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043045_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043046_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043047_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043048_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043049_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043050_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043051_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043052_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043053_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043054_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043055_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043057_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043058_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043059_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043060_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043061_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043062_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043063_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043064_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043065_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043066_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043067_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043068_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043069_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043070_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043071_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043072_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043073_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043074_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043075_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043076_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043077_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043078_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043079_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043080_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043081_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043082_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043083_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043084_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043085_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043086_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043087_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043088_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043089_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043090_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043091_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043092_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043093_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043094_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043095_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043096_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043097_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043098_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043099_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043101_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043102_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043103_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043104_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043105_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043107_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043108_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043109_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043110_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043111_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043112_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043113_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043114_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043115_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043116_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043117_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043118_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043119_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043120_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043121_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043122_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043123_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043124_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043125_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043126_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043127_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043128_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043129_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043130_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043131_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043132_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043133_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043134_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043135_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043136_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043137_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043138_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043139_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043140_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043141_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043142_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043143_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043145_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043146_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043147_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043148_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043150_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043151_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043152_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043153_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043154_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043155_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043156_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043158_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043159_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043160_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043161_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043163_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043164_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043165_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043166_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043168_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043169_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043170_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043171_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043172_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043173_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043174_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043175_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043176_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043177_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043179_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043180_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043181_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043182_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00043183_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00045295_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00045336_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00045354_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00046600_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00046603_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00046820_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00046841_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00046924_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00047243_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00047631_1.full.dst'),
  LogicalFile('LFN:/lhcb/LHCb/Collision12/FULL.DST/00020330/0004/00020330_00047632_1.full.dst')
]

print """
  A JobTemplate instance has been created and named t.
  Default backend is Local()
  To test the BandQ stripping lines: 
 
      >>> Job(t).submit()

"""




