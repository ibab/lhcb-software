'''
Option file to run Moore v10rX on MC including L0 emulation.
Please note that this script only works for Moore v10rX,
for Moore v9rX please use RunMoorev9rXMC.py 

Please check and set the correct database tags and output file.
The input is handled in the usual way through a separate file
containing EventSelector().Input and possible file catalogs.
'''

__author__  = "Johannes Albrecht albrecht@cern.ch"
__date__    = "2010-10-10"

###
# Configure Moore to run from TCK and write a DST.
# MAKE SURE THE TCK FITS THE MOORE VERSION YOU USE.
#
# For running from threshold settings or to run from
# a TCK without making a DST see RunMoore.py
# This works for Moore v9rX OR Moore v10rX
#

tck = '0x00760037'
nEvents=50
dddbTag= 'head-20110302'
condDBTag= 'head-20110622'

from Configurables import Moore

Moore().UseTCK = True
Moore().InitialTCK = tck

Moore().L0 = False
Moore().ReplaceL0BanksWithEmulated = False
Moore().UseDBSnapshot = False
Moore().EnableRunChangeHandler = False
Moore().CheckOdin = False
Moore().Simulation = False
Moore().DataType = '2011'

Moore().EvtMax = nEvents

###
# what to write and where? everything?
#
#Moore().WriterRequires = ['Hlt1TrackAllL0PostScaler','Hlt2CharmOSTF2BodyPostScaler']#write dst with HLT in pass through
#Moore().WriterRequires = ['Hlt1MuTrackPostScaler']



###
# DB tags. MAKE SURE YOUR TAGS ARE RIGHT
#
Moore().DDDBtag = dddbTag
Moore().CondDBtag = condDBTag

Moore().inputFiles = ['/castor/cern.ch/user/a/aphan/GangaSyracuse/584/27513859_Bs2DsPi.Bs2DsPiSequence.dst']
