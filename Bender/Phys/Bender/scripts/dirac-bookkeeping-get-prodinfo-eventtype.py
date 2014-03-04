#!/usr/bin/env python
"""
get productions information for eventtype 


"""

import DIRAC
from DIRAC.Core.Base import Script

Script.registerSwitch( '', 'FileType=', 'FileType to search [ALLSTREAMS.DST]' )

Script.setUsageMessage( '\n'.join( [ __doc__.split( '\n' )[1],
                                     'Usage:',
                                     '  %s [option] eventType  ' % Script.scriptName ] ) )
fileType = 'ALLSTREAMS.DST'
Script.parseCommandLine( ignoreErrors = True )

for switch in Script.getUnprocessedSwitches():
  if switch[0] == "FileType" :
    fileType = str(switch[1])

args = Script.getPositionalArgs()
if len(args) != 1:
  Script.showHelp()
  exit(0)
eventType = args[0]

from DIRAC                                                        import gLogger, gConfig, S_OK, S_ERROR
from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient         import BookkeepingClient
from DIRAC.Core.DISET.RPCClient                                   import RPCClient
from LHCbDIRAC.TransformationSystem.Client.TransformationClient   import TransformationClient

import pprint

class Production():
  """ 
     Simple helper emulating old Production Client
  """


  def __init__( self ):
    """Instantiates the Workflow object and some default parameters.
    """

    self.transClient = TransformationClient()

  def getParameters( self, prodID, pname = '' ):
    """Get a production parameter or all of them if no parameter name specified.
    """

    result = self.transClient.getTransformation( int( prodID ), True )
    if not result['OK']:
      gLogger.error( result )
      return S_ERROR( 'Could not retrieve parameters for production %s' % prodID )

    if not result['Value']:
      gLogger.info( result )
      return S_ERROR( 'No additional parameters available for production %s' % prodID )

    if pname:
      if result['Value'].has_key( pname ):
        return S_OK( result['Value'][pname] )
      else:
        gLogger.verbose( result )
        return S_ERROR( 'Production %s does not have parameter %s' % ( prodID, pname ) )

    return result


bk = RPCClient( 'Bookkeeping/BookkeepingManager' )
pr = RPCClient( 'ProductionManagement/ProductionRequest' )

res = bk.getProductionSummaryFromView({'EventType':eventType,'Visible':True})
if not res['OK']:
  print res['Message']
  DIRAC.exit()
prods = res['Value'] 
 
bkClient = BookkeepingClient()
prClient = Production()

prodIDs =  [ prod['Production'] for prod in prods]
prodIDs.sort()

for prodID in prodIDs:
  res = bkClient.getProductionInformations(prodID)
  nof = 0
  noe = 0
  if res['OK']:
    value = res['Value']
    path  =  value['Path'].split("\n")[1]
    for nf in value['Number of files']:
       if nf[1] == fileType:
         nof = nf[0]
    for ne in value['Number of events']:
       if ne[0] == fileType:
         noe = ne[1]	 
    #print nof, noe,
    DDDBTag = ""
    CondDBTag = ""
    SimprodID = None
    res = prClient.getParameters(prodID,'DDDBTag')
    if res['OK']:
      DDDBTag = res['Value']
    res = prClient.getParameters(prodID,'CondDBTag')
    if res['OK']:
      CondDBTag = res['Value'],
    if not (DDDBTag and CondDBTag):
      res = prClient.getParameters(prodID,'BKInputQuery')
      if res['OK']:
        simprodID = eval(res['Value']).get('ProductionID',0)
      else:
        res = prClient.getParameters(prodID,'RequestID')
	if not res['OK']:
	  print res['Message']
	  exit(2)
	reqID = int(res['Value'])
        res = pr.getProductionList(reqID)
        if res['OK']:
          simprodID = res['Value'][0]
        else:
          print res['Message']
          exit(2)
      if simprodID:
        res = prClient.getParameters(simprodID,'DDDBTag')
        if res['OK']:
          DDDBTag = res['Value']
        res = prClient.getParameters(simprodID,'CondDBTag')
        if res['OK']:
          CondDBTag = res['Value']
	  
	if not (DDDBTag and CondDBTag):
	  res = prClient.getParameters(simprodID,'BKProcessingPass')
	  if res['OK']:
	    value = res['Value']
	    dBKpp = eval(value)
	    step0 = dBKpp['Step0']
	    if not DDDBTag:
	      DDDBTag = step0['DDDb']
	    if not CondDBTag:
	      CondDBTag = step0['CondDb']
    if isinstance(CondDBTag,(tuple,list)):
      CondDBTag = CondDBTag[0]
    if isinstance(DDDBTag,(tuple,list)):
      DDDBTag = DDDBTag[0]
    #print DDDBTag,CondDBTag

    #print DDDBTag,CondDBTag
    print ( path, DDDBTag, CondDBTag, nof, noe) 
