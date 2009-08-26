
################################################
## basic script for the TCK-Presenter Project ##
## Klaus Neumann                              ##
## 17.07.09                                   ##
## CERN-PH-LHCb                               ##
################################################

## imported scripts. the main scipts are the graphprocessing an the stringmanipulation
## scripts. Utils is used to get the informations about the given TCK. The graphs are
## created with the pydot interface
import os
import myGraph
import myString
from TCKUtils.utils import *
import pydot

## one can start the script dotcreater without using the PresenterProject.py script.
## e.g. it could be intresting to automize these script for many TCKs. Run it like:
## dotcreater.create( TCK=0x803b0000 Path='/afs/cern.ch/user/k/kneumann/tcktestpage/' )
## in the script which is suppose to use the dotcreater.

## renaming of the function to use shorter names.
replacer_to_code          = myString.replacer_to_code
     
## the main function of the dotcreater script to create the svg files.
def create(TCK, Path):

    ## read in the algorithms for a given TCK and build out of these information the xhtml pages 
    print 'get Algorithms for the TCK: ', TCK
    FileContent = getAlgorithms(TCK)
    #f = open('0x80540000', 'r')
    #FileContent = f.read()

    ## lines can have many letters in their names, which could make complications, when one put it in the
    ## xhtml pages. The basic idea is to change the names to codings for example a comma could make a problem,
    ## it will change: Hlt1L0line,draw to Hlt1L0line_002_draw etc. (search in the myStringmanipulation or the
    ## myConstants scripts) 
    FileContent = replacer_to_code(FileContent)
    print 'finished get Algorithms for the TCK: ', TCK

    ## the file content is given to the myGraph object which builds up a inner structure to work with
    ## the content of the TCK. It is possible to get the instances of a certain level and the graphs as
    ## a dotfile, which will be used to produce a xhtml file via pydot.
    myG = myGraph.myGraph(FileContent)
    create_level_1_instances     = myG.create_level_1_instances
    create_level_1_graph_basic   = myG.create_level_1_graph_basic
    create_level_1_graph_offline = myG.create_level_1_graph_offline
    
    create_level_2_instances     = myG.create_level_2_instances
    create_level_2_graph_basic   = myG.create_level_2_graph_basic
    create_level_2_graph_offline = myG.create_level_2_graph_offline
    
    create_level_3_instances     = myG.create_level_3_instances
    create_level_3_graph_basic   = myG.create_level_3_graph_basic
    create_level_3_graph_offline = myG.create_level_3_graph_offline

    # level 4 instances not needed because we don't go deeper
    create_level_4_graph_basic   = myG.create_level_4_graph_basic
    create_level_4_graph_offline = myG.create_level_4_graph_offline

    ## new directories are created in the given directory
    ## There is the basic directory, contains the svg files which are necessary to fill in the rate values later.
    ## This is important for the online version. The offline directory contains the svg files which do not have rates inside.
    ## The online directory will not be filled with svg files by the PresenterProject. It will be used from the SubstituteTemplate
    ## script where the rates are filled in the basic structure svg files and loaded in the online directory.
    print 'make directories'
    if not os.path.exists(Path + TCK + '/'):
        os.mkdir(Path + TCK + '/')
    if not os.path.exists(Path + TCK + '/offline/'):
        os.mkdir(Path + TCK + '/offline/')
    if not os.path.exists(Path + TCK + '/basic/'):
        os.mkdir(Path + TCK + '/basic/')
    print 'finished make directories'

    ## To write a template file in addition to the svg structure, makes it easier for the script which is supposed to fill in the
    ## rates to do its job. The template file contains all the basic structure svg files in the directory and the cuts.
    ## There is a hierachical structure in the template which suggests the graph structure as well.
    TemplateContent = ''
    TemplateDump = ''

    ## Level 1: The 'systems' for the TCK, mainly divided into Hlt1, Hlt2 and the HltEndSequence.
    ## Level 2: Contains for every 'system' the alleys in Hlt hardcoded.
    ## Level 3: Contains for every 'alley' the lines 
    ## Level 4: Contains for every 'line' the cuts as well as the filter sequences which are performed.

    ## creation of the basic and offline graphs.
    ## basic graphs are later used for the online
    ## update for the rates.
    ## The TemplateContent files will be used to make the analyzing
    ## easier for script which filles in the rates.
    level_1_graph_basic   = create_level_1_graph_basic(TCK)
    level_1_graph_offline = create_level_1_graph_offline(TCK)
    ## Get the instances of the system and go on with them.
    level_1_instances     = create_level_1_instances()
    TemplateContent       = 'Hlt\n'

    ## Write the files created by the myGraphprocessing script.
    graphobject           = pydot.graph_from_dot_data(level_1_graph_basic)
    graphobject.write_svg(Path + TCK + '/basic/basic_Hlt.xhtml', prog='dot')
    graphobject           = pydot.graph_from_dot_data(level_1_graph_offline)
    graphobject.write_svg(Path + TCK + '/offline/extended_Hlt.xhtml', prog='dot')
    
    print 'basic_Hlt'
    ## Level 1 instances represent the different steps in the Hlt. Htl1 & 2 e.g.
    for level_1_instance in level_1_instances:      #level_1_instance is a system
        ## add the instances in the template and read in the next instances one level deeper.
        ## go one in the deeper levels as usual. And write out the files.
        TemplateContent       = TemplateContent + '   ' + level_1_instance + '\n'
        level_2_instances     = create_level_2_instances(level_1_instance)
        level_2_graph_basic   = create_level_2_graph_basic(level_1_instance, TCK)
        level_2_graph_offline = create_level_2_graph_offline(level_1_instance, TCK)
        graphobject           = pydot.graph_from_dot_data(level_2_graph_basic)
        graphobject.write_svg(Path + TCK + '/basic/basic_Hlt_'+level_1_instance+'.xhtml', prog='dot')
        graphobject           = pydot.graph_from_dot_data(level_2_graph_offline)
        graphobject.write_svg(Path + TCK + '/offline/extended_Hlt_'+level_1_instance+'.xhtml', prog='dot')
        print '   ' + 'basic_Hlt_'+level_1_instance
        for level_2_instance in level_2_instances:      #level_2_instance is a alley
            ## add the instances in the template and read in the next instances one level deeper.
            ## go one in the deeper levels as usual. And write out the files.
            TemplateContent       = TemplateContent + '      ' + level_2_instance + '\n'
            TemplateDump += "Partition_Adder_1/GauchoJob/MonitorSvc/monRate/" + level_2_instance  + "AlleyAccepts\n"
            level_3_instances     = create_level_3_instances(level_1_instance, level_2_instance)
            level_3_graph_basic   = create_level_3_graph_basic(level_1_instance, level_2_instance, TCK)
            level_3_graph_offline = create_level_3_graph_offline(level_1_instance, level_2_instance, TCK)
            graphobject           = pydot.graph_from_dot_data(level_3_graph_basic)
            graphobject.write_svg(Path + TCK + '/basic/basic_Hlt_'+level_1_instance+'_'+level_2_instance+'.xhtml', prog='dot')
            graphobject           = pydot.graph_from_dot_data(level_3_graph_offline)
            graphobject.write_svg(Path + TCK + '/offline/extended_Hlt_'+level_1_instance+'_'+level_2_instance+'.xhtml', prog='dot')
            print '      ' + 'basic_Hlt_'+level_1_instance+'_'+level_2_instance
            for level_3_instance in level_3_instances:      #level_3_instance is a line
                ## no level_4_instances are needed
                TemplateContent       = TemplateContent + '         ' + level_3_instance + '\n'
                TemplateDump += "Partition_Adder_1/GauchoJob/MonitorSvc/monRate/Hlt1" + level_3_instance  + "DecisionLineAccepts\n"
                level_4_temp_basic    = create_level_4_graph_basic(level_1_instance, level_2_instance, level_3_instance, TCK)
                ## attention:tuple of instances and graph
                level_4_temp_offline  = create_level_4_graph_offline(level_1_instance, level_2_instance, level_3_instance, TCK)
                ## attention:tuple of instances and graph
                level_4_graph_basic   = level_4_temp_basic[1]
                level_4_graph_offline = level_4_temp_offline[1]
                level_4_instances     = level_4_temp_basic[0]
                graphobject           = pydot.graph_from_dot_data(level_4_graph_basic)      #level_4_instance is a cut or threshold
                graphobject.write_svg(Path + TCK + '/basic/basic_Hlt_'+level_1_instance+'_'+level_2_instance+'_'+level_3_instance+'.xhtml', prog='dot')
                graphobject           = pydot.graph_from_dot_data(level_4_graph_offline)      #level_4_instance is a cut or threshold
                graphobject.write_svg(Path + TCK + '/offline/extended_Hlt_'+level_1_instance+'_'+level_2_instance+'_'+level_3_instance+'.xhtml', prog='dot')
                print '         ' + 'basic_Hlt_'+level_1_instance+'_'+level_2_instance+'_'+level_3_instance
                for level_4_instance in level_4_instances:
                    TemplateContent   = TemplateContent + '            ' + level_4_instance + '\n'

    TemplateHandle_Content = open(Path + TCK + '/basic/template', 'w')                
    TemplateHandle_Content.write(TemplateContent)
    TemplateHandle_Content.close()
    TemplateHandle_Dump = open(Path + TCK + '/basic/dump', 'w')                
    TemplateHandle_Dump.write(TemplateDump)
    TemplateHandle_Dump.close()


