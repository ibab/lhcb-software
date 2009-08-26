
################################################
## basic script for the TCK-Presenter Project ##
## Klaus Neumann                              ##
## 17.07.09                                   ##
## CERN-PH-LHCb                               ##
################################################

## imported scripts.
from TCKUtils.utils import *
import myString
import myNode
import myConst

## renaming of the function to use shorter names.
Stagecontrol               = myString.Stagecontrol
Linefinder                 = myString.Linefinder
LineIndexfinder            = myString.LineIndexfinder
nf                         = myString.nf
Namefinder                 = myString.Namefinder
Nextstation                = myString.Nextstation
successors                 = myString.successors
toplevelsuccessors         = myString.toplevelsuccessors
parent                     = myString.parent
getfilename                = myString.getfilename
derivelinenames            = myString.derivelinenames
derivelinename             = myString.derivelinename
cutlayout                  = myNode.cutlayout
formatter                  = myNode.formatter
graph_start                = myNode.graph_start
create_node                = myNode.create_node
create_label               = myNode.create_label
render_dir_edge            = myNode.render_dir_edge
NODE_PLAIN                 = myConst.NODE_PLAIN
NODE_RARE                  = myConst.NODE_RARE
NODE_SCALERLIKE            = myConst.NODE_SCALERLIKE
NODE_CUT                   = myConst.NODE_CUT
NODE_TECHNICAL             = myConst.NODE_TECHNICAL 
NODE_PHYSICS               = myConst.NODE_PHYSICS
NODE_SPECIAL               = myConst.NODE_SPECIAL
NODE_CUT_WITHOUTRATE       = myConst.NODE_CUT_WITHOUTRATE
NODE_TECHNICAL_WITHOUTRATE = myConst.NODE_TECHNICAL_WITHOUTRATE
NODE_PHYSICS_WITHOUTRATE   = myConst.NODE_PHYSICS_WITHOUTRATE
NODE_SPECIAL_WITHOUTRATE   = myConst.NODE_SPECIAL_WITHOUTRATE
LABEL_PLAIN1               = myConst.LABEL_PLAIN1
LABEL_PLAIN2               = myConst.LABEL_PLAIN2
alley_realname             = myConst.alley_realname
alley_regex                = myConst.alley_regex
alley_type                 = myConst.alley_type
alley_type_withoutrate     = myConst.alley_type_withoutrate
alley_s_p                  = myConst.alley_s_p

class myGraph:

     right   = 'http://plus501/subst_temp.py/right?File='
     server  = 'http://plus501/subst_temp.py/online?File='
     offline = 'http://plus501/TCKs/'

     ## Constructer for the myGraph object which is basically
     ## used to give the object the TCK String
     def __init__(self, FC):
          self.FC = FC
          self.linelist = FC.splitlines(0)

     ## SYSTEMS: gives back the level 1 instances
     def create_level_1_instances(self):
          tls = toplevelsuccessors('Hlt', self.linelist, ('',''))
          return tls 
    
     ## ALLEYS: gives back the level 2 instances
     def create_level_2_instances(self, System):
          alleys = []
          if System == 'Hlt1':
               alleys = alley_s_p
          return alleys

     ## LINES: gives back the level 3 instances
     def create_level_3_instances(self, System, Alley):
          tls_re = toplevelsuccessors(System, self.linelist, alley_regex[Alley])
          return tls_re

     ## CUTS: gives back the level 4 instances
     def create_level_4_instances(self, System, Alley, Line):
          newtls = successors(Line, self.linelist, 1)
          return newtls



    
     ## in this procedure the graph will be created. The used language is the dot language.
     ## basically there are 2 modes (basic and offline) which suggests that these both different structures are
     ## created with respect to the modus. basic is later used for the rates where the offline
     ## mode will stay static for the rest of its lifetime.

     ## this procedure is for the production of the structure which will be used in the online version.
     ## just the systems will be produced.
     def create_level_1_graph_basic(self, TCK):
          ## accept all the diffrent functions and give new shorter names.
          create_level_1_instances = self.create_level_1_instances
          create_level_2_instances = self.create_level_2_instances
          create_level_3_instances = self.create_level_3_instances
          create_level_4_instances = self.create_level_4_instances
          linelist = self.linelist
          ## read in the systems.
          systems = create_level_1_instances()
          ## start the graph with a standard beginning 
          str = graph_start()
          ## as usual we will have one node which representes the in rate.
          str = str + '\n' + create_node('in_rate', 'in=$in_rate', '', '', NODE_RARE)
          ## create all the nodes (in this case the systems). They are all physics nodes
          for system in systems:
               str = str + '\n' + create_node(system, system, self.server+'Hlt_'+system+'$tck_partition','', NODE_PHYSICS)
          ## connect them with edges so we get a sequential graph.     
          for system_index in range(len(systems)-1):
               str = str + '\n'+ render_dir_edge(systems[system_index], systems[system_index+1])
          str = str + '\n}'
          return str
     
     ## this procedure is for the production of the structure which will be used in the offline version.
     ## just the systems will be produced.
     def create_level_1_graph_offline(self, TCK):
          ## accept all the diffrent functions and give new shorter names.
          create_level_1_instances = self.create_level_1_instances
          create_level_2_instances = self.create_level_2_instances
          create_level_3_instances = self.create_level_3_instances
          create_level_4_instances = self.create_level_4_instances
          linelist = self.linelist
          ## read in the systems.
          systems = create_level_1_instances()
          str = graph_start()
          ## as usual we will have one node which representes the in rate.
          ## create all the nodes (in this case the systems). They are all physics nodes
          for system in systems:
               str = str + '\n' + create_node(system, system, self.offline+TCK+'/offline/extended_Hlt_'+system+'.xhtml', '', NODE_PHYSICS_WITHOUTRATE)
          ## connect them with edges so we get a sequential graph.   
          for system_index in range(len(systems)-1):
               str = str + '\n'+ render_dir_edge(systems[system_index], systems[system_index+1])
          str = str + '\n}'
          return str




     ## the structure of the following procedures is repeating. It gives a dot string back, which is used to create the xhtml file later.
     ## here is the construction of the basic version of the alleys.
     def create_level_2_graph_basic(self, system, TCK):
          ## accept all the diffrent functions and give new shorter names.
          create_level_1_instances = self.create_level_1_instances
          create_level_2_instances = self.create_level_2_instances
          create_level_3_instances = self.create_level_3_instances
          create_level_4_instances = self.create_level_4_instances
          linelist = self.linelist
          ## start as usual, is basically always the same start of a funtion.
          str = graph_start()
          ## in rate as well as teh first higher stages of the graph are produced as nodes.
          str = str + '\n' + create_node('in_rate', 'in=$in_rate', '', '', NODE_RARE)
          str = str + create_node('Hlt', 'Hlt', self.server+'Hlt'+'$tck_partition', '', NODE_RARE)
          str = str + create_node(system, system, self.server+'Hlt_'+system+'$tck_partition', '', NODE_PHYSICS)
          ## the HLT1 will be treated differently to the other systems
          ## especially because many things are hardcoded here.
          if system == 'Hlt1':
               ## Global is a special case
               str = str + create_label('Hlt1Global', 'Global', LABEL_PLAIN1)
               str = str + create_node('Lumi', 'Lumi', self.server+'Hlt_Hlt1_Lumi'+'$tck_partition',self.right+'$tck_partition;Image=', alley_type["Lumi"])
               str = str + create_label('Hlt1IgnoringLumi', 'IgnoringLumi', LABEL_PLAIN2)
               ## the alleys are hardcoded (myConstants)
               alleys = create_level_2_instances(system)
               alleys.reverse()
               for alley in alleys:
                    ## Here are the different alleys and there types
                    if alley != "Lumi":
                         str = str + create_node(alley, alley, self.server+'Hlt_'+system+'_'+alley+'$tck_partition',self.right+'$tck_partition;Image=', alley_type[alley])
                    if alley=='Electron':
                         str = str + create_node('Muon', 'Muon', '', '', NODE_RARE)
               str = str + "}}"
               for alley in create_level_2_instances(system):
                    if alley.find('Muon') != -1:
                         str = str + '\n'+ render_dir_edge('Muon', alley)
                    else:
                         str = str + '\n'+ render_dir_edge(system, alley)
               str = str + '\n'+ render_dir_edge(system, 'Muon')
          str = str + '\n'+ render_dir_edge('Hlt', system)
          str = str + '\n}'
          return str
                 
     ## the structure of the following procedures is repeating. It gives a dot string back, which is used to create the xhtml file later.
     ## here is the construction of the offline version of the alleys.
     def create_level_2_graph_offline(self, system, TCK):
          create_level_1_instances = self.create_level_1_instances
          create_level_2_instances = self.create_level_2_instances
          create_level_3_instances = self.create_level_3_instances
          create_level_4_instances = self.create_level_4_instances
          linelist = self.linelist
          str = graph_start()
          str = str + create_node('Hlt', 'Hlt', self.offline+TCK+'/offline/extended_Hlt'+'.xhtml', '', NODE_RARE)
          ## basic is with rate, offline without
          str = str + create_node(system, system, self.offline+TCK+'/offline/extended_Hlt_'+system+'.xhtml', '', NODE_PHYSICS_WITHOUTRATE)
          ## The structures for the other systems then Hlt1 are not yet implemented.
          if system == 'Hlt1':
               str = str + create_label('Hlt1Global', 'Global', LABEL_PLAIN1)
               str = str + create_node('Lumi', 'Lumi', self.offline+TCK+'/offline/extended_Hlt_Hlt1_Lumi'+'.xhtml', self.server+'$tck_partition;Image=', alley_type_withoutrate["Lumi"])
               str = str + create_label('Hlt1IgnoringLumi', 'IgnoringLumi', LABEL_PLAIN2)
               ## the alleys are hardcoded (myConstants)
               alleys = create_level_2_instances(system)
               alleys.reverse()
               for alley in alleys:
                    ## Here are the different alleys and there types
                    if alley != "Lumi":
                         str = str + create_node(alley, alley, self.offline+TCK+'/offline/extended_Hlt_'+system+'_'+alley+'.xhtml', '', alley_type_withoutrate[alley])
                    if alley=='Electron':
                         str = str + create_node('Muon', 'Muon', '', '', NODE_RARE)
               str = str + "}}"
               for alley in create_level_2_instances(system):
                    if alley.find('Muon') != -1:
                         str = str + '\n'+ render_dir_edge('Muon', alley)
                    else:
                         str = str + '\n'+ render_dir_edge(system, alley)
               str = str + '\n'+ render_dir_edge(system, 'Muon')
          str = str + '\n'+ render_dir_edge('Hlt', system)
          str = str + '\n}'
          return str



     ##creation of the basic structure of the lines
     def create_level_3_graph_basic(self, system, alley, TCK):
          create_level_1_instances = self.create_level_1_instances
          create_level_2_instances = self.create_level_2_instances
          create_level_3_instances = self.create_level_3_instances
          create_level_4_instances = self.create_level_4_instances
          linelist = self.linelist
          
          str = graph_start()
          lines  = create_level_3_instances(system, alley)
          dlines = derivelinenames(alley_realname[alley], lines)
          str = str + '\n' + create_node('in_rate', 'in=$in_rate', '', '', NODE_RARE)
          str = str + create_node('Hlt', 'Hlt', self.server+'Hlt'+'$tck_partition', '', NODE_RARE)
          str = str + create_node(system, system, self.server+'Hlt_'+system+'$tck_partition', '', NODE_RARE)
          str = str + create_node(alley, alley, self.server+'Hlt_'+system+'_'+alley+'$tck_partition', self.right+'$tck_partition;Image=', alley_type[alley])
          for i in range(len(lines)):
               str = str + create_node(lines[i], dlines[i], self.server+'Hlt_'+system+'_'+alley+'_'+lines[i]+'$tck_partition', self.right+'$tck_partition;Image=', NODE_PHYSICS)
          for i in range(len(lines)):  
               str = str + '\n'+ render_dir_edge(alley, lines[i])
          str = str + '\n'+ render_dir_edge('Hlt', system) 
          str = str + '\n'+ render_dir_edge(system, alley) 
          str = str + '}'
          return str

     ##creation of the offline structure of the lines
     def create_level_3_graph_offline(self, system, alley, TCK):
          create_level_1_instances = self.create_level_1_instances
          create_level_2_instances = self.create_level_2_instances
          create_level_3_instances = self.create_level_3_instances
          create_level_4_instances = self.create_level_4_instances
          linelist = self.linelist
          
          str = graph_start()
          lines  = create_level_3_instances(system, alley)
          dlines = derivelinenames(alley_realname[alley], lines)
          str = str + create_node('Hlt', 'Hlt', self.offline+TCK+'/offline/extended_Hlt'+'.xhtml', '', NODE_RARE)
          str = str + create_node(system, system, self.offline+TCK+'/offline/extended_Hlt_'+system+'.xhtml', '', NODE_RARE)
          str = str + create_node(alley, alley, self.offline+TCK+'/offline/extended_Hlt_'+system+'_'+alley+'.xhtml', '', alley_type_withoutrate[alley])
          for i in range(len(lines)):
               str = str + create_node(lines[i], dlines[i], self.offline+TCK+'/offline/extended_Hlt_'+system+'_'+alley+'_'+lines[i]+'.xhtml', '', NODE_PHYSICS_WITHOUTRATE)
          for i in range(len(lines)):  
               str = str + '\n'+ render_dir_edge(alley, lines[i])
          str = str + '\n'+ render_dir_edge('Hlt', system) 
          str = str + '\n'+ render_dir_edge(system, alley) 
          str = str + '}'
          return str



     ##creation of the basic structure of the cuts
     def create_level_4_graph_basic(self, system, alley, line, TCK):
          linelist = self.linelist

          ## the cutinstances are used to give it back for the construction of the template file. It
          ## is only important because the computation taks to long to compute it several times.
          cut_instances=[]
          ## the whole sequences  will be biuld in this file, which is then in succ
          succ = successors(line, linelist, 1)
          ## the tls consists of the high level parts of the line. e.g. L0DU and so on.
          tls  = toplevelsuccessors(line, linelist, ('',''))
          ## we need to derive shorter names, because the names would otherwise be to long. 
          dtln = derivelinenames(line, tls)
          dln  = derivelinenames(line, succ)
          ## graph start as usual.
          str  = graph_start()
          ## get the cuts and put them as a part of the cut nodes later.
          cutlist = getProperties(TCK, line, 'Code|FilterDesc|AcceptFraction')
          cuttemp=''
          ## the first sequence of higher level nodes (alleys, lines, systems).
          str = str + '\n' + create_node('in_rate', 'in=$in_rate', '', '', NODE_RARE)
          str = str + create_node('Hlt', 'Hlt', self.server+'Hlt'+'$tck_partition', '', NODE_RARE)
          str = str + create_node(system, system, self.server+'Hlt_'+system+'$tck_partition', '', NODE_RARE)
          str = str + create_node(alley, alley, self.server+'Hlt_'+system+'_'+alley+'$tck_partition', '', NODE_RARE)
          str = str + create_node(line, derivelinename(alley_realname[alley],line), self.server+'Hlt_'+system+'_'+alley+'_'+line+'$tck_partition', self.right+'$tck_partition;Image=', NODE_PHYSICS)
          ## will be a list of starting points for the subgraphes (filtersequences).
          mode_node=[]
          str = str + '\n'+ render_dir_edge('Hlt', system)  
          str = str + '\n'+ render_dir_edge(system, alley)
          str = str + '\n'+ render_dir_edge(alley, line) 
          str = str + '\n'+ render_dir_edge(line, succ[0])
          ## cuttemp will contain the cutstring which will be part of the cut node.
          cuttemp=''
          for n in range(len(succ)-1):
               str = str + '\n'+ render_dir_edge(succ[n], succ[n+1])
          ## the nodes on the highest level will be treated differently. They are not cuts.
          for n in range(len(tls)):
               ## find a PreScaler and find out about the pre scale
               if tls[n].find('PreScaler')!=-1:
                    for cut in cutlist:####################
                         if cut[0].find(tls[n]) != -1:#####
                              cuttemp = cut[1]#############
                    str = str + create_node(tls[n], dtln[n], '', '', NODE_SCALERLIKE, cuttemp)
                    cuttemp=''
               ## find a PostScaler and find out about the post scale
               elif tls[n].find('PostScaler')!=-1:
                    for cut in cutlist:####################
                         if cut[0].find(tls[n]) != -1:#####
                              cuttemp = cut[1]#############
                    str = str + create_node(tls[n], dtln[n], '', '', NODE_SCALERLIKE, cuttemp)
                    cuttemp=''
               ## if a filter sequence is found, it is important to keep the index in the list in mind
               ## to be able to put the following nodes in the subgraph.
               elif tls[n].find('FilterSequence')!=-1:
                    mode_node.append(n)
               ## if nothing of this is the case, just plot a plain node.
               else:
                    str = str + create_node(tls[n], dtln[n], '', '', NODE_PLAIN)
          ## printing of the subgraphs as a filtersequence.
          for m_n in mode_node:
               newtls  = successors(tls[m_n], linelist, 1)
               newdtln = derivelinenames(line, newtls)
               #str = str + 'subgraph cluster%d { style="filled"; color="#fffff0"; label="Filter-Sequence"' % m_n
               str = str + create_node(tls[m_n], dtln[m_n], '', '', NODE_PLAIN)
               for n in range(len(newtls)):
                    for cut in cutlist:####################
                         if cut[0].find(newtls[n]) != -1:###
                              cuttemp = cutlayout(cut[1])####
                    str = str + '\n' + create_node(newtls[n], newdtln[n], '', '', NODE_CUT, cuttemp)
                    if cuttemp != '':
                         cut_instances.append(newtls[n])
                    cuttemp=''
               #str = str + '}'
          str = str + '\n}'
          return (cut_instances, str)

     def create_level_4_graph_offline(self, system, alley, line, TCK):
          create_level_1_instances = self.create_level_1_instances
          create_level_2_instances = self.create_level_2_instances
          create_level_3_instances = self.create_level_3_instances
          create_level_4_instances = self.create_level_4_instances
          linelist = self.linelist

          ## it is important to read in the Properties for the given TCK.
          ## the cuts are performend with a different efficiency so rates
          ## are given as well.
          cutlist = getProperties(TCK, line, 'Code|FilterDesc|AcceptFraction')
          
          cut_instances=[]
          succ = successors(line, linelist, 1)
          tls  = toplevelsuccessors(line, linelist, ('',''))
          dtln = derivelinenames(line, tls)
          dln  = derivelinenames(line, succ)
          str  = graph_start()
          str = str + create_node('Hlt', 'Hlt', self.offline+TCK+'/offline/extended_Hlt'+'.xhtml', '', NODE_RARE)
          str = str + create_node(system, system, self.offline+TCK+'/offline/extended_Hlt_'+system+'.xhtml', '', NODE_RARE)
          str = str + create_node(alley, alley, self.offline+TCK+'/offline/extended_Hlt_'+system+'_'+alley+'.xhtml', '', NODE_RARE)
          ## lines are allways from the type physics
          str = str + create_node(line, derivelinename(alley_realname[alley],line), self.offline+TCK+'/offline/extended_Hlt_'+system+'_'+alley+'_'+line+'.xhtml', '', NODE_PHYSICS_WITHOUTRATE)
          mode_node=[]
          str = str + '\n'+ render_dir_edge('Hlt', system)  
          str = str + '\n'+ render_dir_edge(system, alley)
          str = str + '\n'+ render_dir_edge(alley, line) 
          str = str + '\n'+ render_dir_edge(line, succ[0])
          cuttemp=''
          for n in range(len(succ)-1):
               str = str + '\n'+ render_dir_edge(succ[n], succ[n+1])
          for n in range(len(tls)):
               ## read in the values for the scalers pre and post.
               if tls[n].find('PreScaler')!=-1:
                    for cut in cutlist:################ 
                         if cut[0].find(tls[n]) != -1:#
                              cuttemp = cut[1]#########
                    str = str + create_node(tls[n], dtln[n], '', '', NODE_SCALERLIKE, cuttemp)
                    cuttemp=''
               elif tls[n].find('PostScaler')!=-1:
                    for cut in cutlist:################
                         if cut[0].find(tls[n]) != -1:#
                              cuttemp = cut[1]#########
                    str = str + create_node(tls[n], dtln[n], '', '', NODE_SCALERLIKE, cuttemp)
                    cuttemp=''
               elif tls[n].find('FilterSequence')!=-1:
                    mode_node.append(n)
               else:
                    str = str + create_node(tls[n], dtln[n], '', '', NODE_PLAIN)
          for m_n in mode_node:
               newtls  = successors(tls[m_n], linelist, 1)
               newdtln = derivelinenames(line, newtls)
               #str = str + 'subgraph cluster%d { style="filled"; color="#fffff0"; label="Filter-Sequence"' % m_n
               str = str + create_node(tls[m_n], dtln[m_n], '', '', NODE_PLAIN)
               for n in range(len(newtls)):
                    for cut in cutlist:###################
                         if cut[0].find(newtls[n]) != -1:#
                              cuttemp = cutlayout(cut[1])#
                    str = str + '\n' + create_node(newtls[n], newdtln[n], '', '', NODE_CUT_WITHOUTRATE, cuttemp)
                    if cuttemp != '':
                         cut_instances.append(newtls[n])
                    cuttemp=''
               #str = str + '}'
          str = str + '\n}'
          return (cut_instances, str)
