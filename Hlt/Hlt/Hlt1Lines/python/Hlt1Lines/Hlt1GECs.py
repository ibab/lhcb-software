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
