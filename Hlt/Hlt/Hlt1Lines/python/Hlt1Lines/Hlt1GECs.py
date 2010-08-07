def Hlt1_IT_GEC( logic ):
  from Configurables import LoKi__VoidFilter as VoidFilter
  from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
  from HltLine.HltLine import bindMembers

  modules =  CoreFactory('CoreFactory').Modules
  for i in [ 'LoKiTrigger.decorators' ] : 
    if i not in modules : modules.append(i)

  # The IT Void filter
  Hlt1_IT_GEC_Code   = "CONTAINS('Raw/IT/LiteClusters') " +logic+ " 3000"
  
  Hlt1_IT_GEC_Filter = VoidFilter('Hlt1_IT_GEC'
                                 , Code = Hlt1_IT_GEC_Code
                                 )   

  return bindMembers( None, [ Hlt1_IT_GEC_Filter ] ) 

def Hlt1_Velo_GEC( logic ):
  from Configurables import LoKi__VoidFilter as VoidFilter
  from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
  from HltLine.HltLine import bindMembers

  modules =  CoreFactory('CoreFactory').Modules
  for i in [ 'LoKiTrigger.decorators' ] : 
    if i not in modules : modules.append(i)

  # The Velo Void filter
  Hlt1_Velo_GEC_Code = "CONTAINS('Raw/Velo/LiteClusters') " +logic+ " 10000" 
  
  Hlt1_Velo_GEC_Filter = VoidFilter('Hlt1_Velo_GEC'
                                   , Code = Hlt1_Velo_GEC_Code
                                   ) 

  return bindMembers( None, [ Hlt1_Velo_GEC_Filter ] )

def Hlt1_OT_GEC( logic ):
  from Configurables import LoKi__VoidFilter as VoidFilter
  from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
  from HltLine.HltLine import bindMembers

  modules =  CoreFactory('CoreFactory').Modules
  for i in [ 'LoKiTrigger.decorators' ] : 
    if i not in modules : modules.append(i)

  # The OT Void filter
  Hlt1_OT_GEC_Code = ""
  if   (logic == "<") : Hlt1_OT_GEC_Code = "ACCEPT('Hlt::GEC/MyGEC')"
  elif (logic == ">") : Hlt1_OT_GEC_Code = "~ACCEPT('Hlt::GEC/MyGEC')" 
  else : return 0 

  Hlt1_OT_GEC_Filter = VoidFilter('Hlt1_OT_GEC'
                                   , Code = Hlt1_OT_GEC_Code
                                   )
  from Configurables import Hlt__GEC as GEC 
  Hlt1_OT_GEC_Filter.addTool ( GEC , "MyGEC" )
  Hlt1_OT_GEC_Filter.MyGEC.MaxOTHits = 10000

  return bindMembers( None, [ Hlt1_OT_GEC_Filter ] )
