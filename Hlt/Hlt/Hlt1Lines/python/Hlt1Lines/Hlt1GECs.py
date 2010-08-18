
global __accept, __reject,__initialized
__initialized = False
__accept = None
__reject = None

# TODO: make this a class, and give these globals class scope...
def __data( ) :

  global __initialized,__accept,__reject
  if not __initialized : 
      from Configurables import LoKi__VoidFilter as VoidFilter
      from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
      from HltLine.HltLine import bindMembers

      modules =  CoreFactory('CoreFactory').Modules
      for i in [ 'LoKiTrigger.decorators' ] : 
        if i not in modules : modules.append(i)

      from Configurables import Hlt__GEC
      from HltLine.HltDecodeRaw import DecodeIT,DecodeVELO
      from HltLine.HltLine import Hlt1Tool
      cuts = {  'IT' :   { 'Code' : "CONTAINS('Raw/IT/LiteClusters')   < 3000" , 'Prerequisite' : DecodeIT }
             ,  'VELO' : { 'Code' : "CONTAINS('Raw/Velo/LiteClusters') < 10000", 'Prerequisite' : DecodeVELO }
             ,  'OT' :   { 'Code' : "ACCEPT('Hlt::GEC/OTGEC')", 'Prerequisite' : None, 'tools' : Hlt1Tool( Hlt__GEC, "MyGEC", MaxOTHits = 10000 ) }
             }

      
      __accept = dict()
      __reject = dict()
      for (name,config) in cuts.iteritems() :
        reject = VoidFilter("Hlt1GEC%sReject" % name, Code = config['Code'])
        accept = VoidFilter("Hlt1GEC%sAccept" % name, Code = '~( %s )' % config['Code'])
        if 'tools' in config : 
            config['tools'].createConfigurable( accept ) 
            config['tools'].createConfigurable( reject ) 
        __accept[ name ] = bindMembers( None, [ config['Prerequisite'],  accept ] )
        __reject[ name ] = bindMembers( None, [ config['Prerequisite'],  reject ] )

      __reject[ 'All' ] = bindMembers( None, [ __reject['OT'], __reject['VELO'], __reject['IT'] ] )

  return (__reject,__accept)

def Hlt1_GEC_IT( reject = True ):
    ( __reject, __accept ) = __data()
    return __reject['IT'] if reject else __accept['IT']

def Hlt1_Velo_GEC( reject = True ):
    ( __reject, __accept ) = __data()
    return __reject['VELO'] if reject else __accept['VELO']

def Hlt1_OT_GEC( reject = True ):
    ( __reject, __accept ) = __data()
    return __reject['OT'] if reject else __accept['OT']



def Hlt1_GEC( system, reject = True ):
    ( __reject, __accept ) = __data()
    return __reject[system] if reject else __accept[system]

    
