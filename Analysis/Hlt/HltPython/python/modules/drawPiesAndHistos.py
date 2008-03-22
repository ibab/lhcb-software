# =============================================================================
"""
@namespace drawPiesAndHistos
@brief Functions to build pies from dicts and histos from lists. Also includes some extra methods to build legends or check inputs. Has a dictionary defined to 'translate' colors to ROOT correspondent numbers. Used in frequencyAndBpercentage.py, hadHLTpies.py
@author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
@date 2008-02-05
"""
# =============================================================================

import ROOT
import array
import copy

## color dict for ROOT

COLOR_DICT={
    "white":0,
    0:"white",
    "black":1,
    1:"black",
    "red":2,
    2:"red",
    "green":3,
    3:"green",
    "blue":4,
    4:"blue",
    "yellow":5,
    5:"yellow",
    "pink":6,
    6:"pink",
    "brown":28,
    28:"brown",
    "grey":16,
    16:"grey"
    }

#---------------------------------------------------

def checkPieInputs(i_dict,title,labels,colors,opt,sort,leg,oth):
    """
    check wether inputs for drawPieFromDict function have been correctly stablished
    
    @param i_dict Input dictionary. Must have format {'a':1,'b':2,...}
    @param title Title of pie
    @param labels Dictionary with labels for each sector. Format {'a':'austria','b':'belgium',...}
    @param colors Dictionary with color for each sector. Format {'a':'red','b':'green',...}
    @param opt 'both': show both percentages and labels in pie/'perc': show only percentages in pie/'lab': show only labels
    @param sort (Bool) If true sorts out sectors from biggest to smallest
    @param leg (Bool) If true also return a legend with relation labels-colors
    @param oth (Bool) Puts label 'oth' in last position and sets it to 'Others'
    @returns title, labels, colors,opt,sort,leg and oth ready to be used in drawPieFromDict 
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """
    cont=True
    ## see if i_dict is a dictionary, if not, we're done
    if not isinstance(i_dict,dict):
       print "ERROR: PLEASE, INPUT A DICTIONARY"
       cont=False
       return None,None,None,None,None,None,leg

    ## set number of elements in dictionary
    n_el=len(i_dict)

    ## if title is not a string, warn and set it to empty string
    if not isinstance(title,str):
        title=""
        print "TITLE IS NOT A STRING"

    ## if label is not a dict or has a wrong number of elements, simply put it to dict keys 
    if not isinstance(labels,dict): labelsn=i_dict.keys()
    else:
        labelsn=[]
        for i in i_dict:
            if labels.has_key(i): labelsn.append(labels[i])
            else:
                if i!="oth": print i+" NOT KEY IN LABELS"
                labelsn.append(i)


    colorsn=[]
    ## translate colors to ROOT numbers with COLOR_DICT. If colors dict is not valid, set it to 2 onwards and warn
    if not isinstance(colors,dict): colorsn=range(2,n_el+2)
    elif len(colors)!=n_el:
       colorsn=range(2,n_el+2)
       print "WRONG NUMBER OF COLORS"
    else:
       for i in i_dict:
           c=colors[i]
           if c in COLOR_DICT: colorsn.append(COLOR_DICT[c])
           else: colorsn.append(0)

    ## check opt. If not str, set it to "both"
    if not isinstance(opt,str):
        opt="both"
        print "OPT IS NOT STR"
    ## check sort. If not bool, set it to False
    if not isinstance(sort,bool):
        sort=False
        print "SORT IS NOT BOOL"

    ## check leg. If not bool, set it to False
    if not isinstance(leg,bool):
        leg=False
        print "LEG IS NOT BOOL"

    ## check oth. If not bool, set it to False
    if not isinstance(oth,bool):
        oth=False
        print "OTH IS NOT BOOL"


    ## return correct values (prepared for function)
    return cont, title,labelsn,colorsn,opt,sort,leg,oth

#---------------------------------------------------

def checkHistInputs(i_list,title,name,nbins,fbin,lbin,color,xtitle,ytitle):
    """
    check wether inputs for drawHistFromList function have been correctly stablished
    @param i_list Input list with numbers to fill histogram
    @param title (string) Title of histogram. Default empty string
    @param name (string) Name of histogram. Default empty string
    @param nbins (int) Number of bins of histogram. Default 70
    @param fbin (float) First Bin. Default smallest element in i_list
    @param lbin (float) Last Bin. Default biggest element in i_list
    @param color (string) Color of histogram
    @param xtitle (string) Title of x axis. Default empty string
    @param ytitle (string) Title of y axis. Default '# of occurrences'
    @returns all inputs (except i_list) ready to be used in drawHistFromList
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """

    ## function to check if string. If not warn an set to empty string
    def is_str(cand,name):
        if not isinstance(cand,str):
            cand=""
            print name+" IS NOT A STRING"
        return cand

    ## check if i_list is a list. If not, we're done
    cont=True
    if not isinstance(i_list,list):
       print "ERROR: PLEASE, INPUT A LIST"
       cont= False

    ## check title, name, xtitle and ytitle.
    title=is_str(title,"TITLE")
    name=is_str(name,"NAME")
    xtitle=is_str(xtitle,"XTITLE")
    ytitle=is_str(ytitle,"YTITLE")

    ## check number of bins. Must b a positive integer
    if not isinstance(nbins,int) or nbins<1:
        nbins=70
        print "NBINS NOT A INT OR SMALLER THAN 1"

    c_list=copy.copy(i_list)
    c_list.sort()

    ## check first bin. If not int or float (or default), set it to smallest element in list.
    if fbin=="def": fbin=c_list[0]
    if not (isinstance(fbin,int) or isinstance(fbin,float)):
        print "FIRST BIN NOT INT OR FLOAT"
        fbin=c_list[0]

    ## check last bin. If not int or float (or default), set it to biggest element in list.
    if lbin=="def": lbin=c_list[len(i_list)-1]
    if not (isinstance(lbin,int) or isinstance(lbin,float)):
        print "LAST BIN NOT INT OR FLOAT"
        lbin=c_list[len(i_list)-1]

    ## check color. If not a string or not in COLOR_DICT set it to black. Else, translate it to ROOT number.
    if not isinstance(color,str):
        print "COLOR NOT STRING"
        colorn=1
    elif color not in COLOR_DICT.keys(): colorn=1
    else: colorn=COLOR_DICT[color]

    ## return correct values (prepared for function)
    return cont,title,name,nbins,fbin,lbin,colorn,xtitle,ytitle
    
#---------------------------------------------------

def legFromPie(pie,els,sort=False,oth=False):

    """
    Returns Tlegend explaining pie entries
    @param pie TPie you want to extract legend from
    @param els (int) Number of slices of pie
    @param sort (Bool) Sort legend entries according to size of slices
    @param oth (Bool) Put 'Others' label in last position
    @returns a list. First element is the legend, second is a list of TBoxes needed for the legend to be drawed
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """

    ## get colors and labels for each pie entry
    vals=[]
    for o in range(els):
        vals.append([pie.GetEntryVal(o),pie.GetEntryFillColor(o),pie.GetEntryLabel(o)])


    ## if oth, keep oth to add it later in last position
    if oth:
        for v in vals:
            if v[2]=="Others": v_oth=vals.pop(vals.index(v))
        
    ## if sort, sort them out according to value (sector size)
    if sort: vals.sort(reverse=True)

    ## if oth, add now oth in last position
    if oth: vals.append(v_oth)
    
    ## initialize legend and boxes
    legend = ROOT.TLegend(0.04,0.7,0.2,0.95)
    boxes=[]

    ## fill legend with information
    for o in range(els):
        boxes.append(ROOT.TBox(0.0,0.0,0.01,0.06))
        boxes[o].SetFillColor(vals[o][1])
        legend.AddEntry(boxes[o],vals[o][2],"F")


    ## return legend and boxes drawed in it
    return [legend,boxes]


#---------------------------------------------------

def drawPieFromDict(i_dict,title="",labels=0,colors=0,opt="both",sort=False,leg=False,oth=False):
    """
    Draw pie from a dict
    @param i_dict Input dictionary. Must have format {'a':1,'b':2,...}
    @param title (string) Title of pie
    @param labels Dictionary with labels for each sector. Format {'a':'austria','b':'belgium',...}. Default, keys of input dictionary.
    @param colors Dictionary with color for each sector. Format {'a':'red','b':'green',...}. Default, first colors from 2 onwards
    @param opt 'both': show both percentages and labels in pie/'perc': show only percentages in pie/'lab': show only labels. Default 'both'
    @param sort (Bool) If true sorts sectors from biggest to smallest. Default False
    @param leg (Bool) If true also returns a list having a legend with relation labels-colors. Legend is first element of this list. Second is a list of boxes needed to draw legend. Default false
    @param oth (Bool) If true looks for 'oth' label to put it last position
    @returns Pie with information and, if leg, also list with legend
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """

    ##check inputs
    cont,title,labels,colors,opt,sort,leg,oth=checkPieInputs(i_dict,title,labels,colors,opt,sort,leg,oth)
    
    if not cont:
        if leg: return None,None
        else: return None

    ## set number of sectors pie will have
    n_el=len(i_dict)
    
    all=0

    i_dict_v=i_dict.values()

    ## add of all elements
    for v in i_dict_v: all+=v

    ## return None if there are no inputs in i_dict
    if all==0:
        print "YOUR INPUT DICTIONARY IS EMPTY"
        if leg: return None,None
        else: return None


    ## find percentage of each one, and put it with correspondent labels and colors 
    vals_a=[]
    for i in range(n_el):
        vals_a.append([i_dict_v[i]*1.0/all,labels[i],colors[i]])
    ## if sort, sort them out according to percentages
    if sort: vals_a.sort(reverse=True)


    ## if oth and label "oth" in input dict, put it away to add it later in last position
    if oth:
        for v in vals_a:
            if v[1]=="oth": v_oth=vals_a.pop(vals_a.index(v))

    ## make arrays to call TPie
    vals=[]
    colors=[]
    labels=[]
    
    for v in vals_a:
        vals.append(v[0])
        labels.append(v[1])
        colors.append(v[2])

    ## if oth, add oth in last position and change label to "Others"
    if oth:
        vals.append(v_oth[0])
        labels.append("Others")
        colors.append(v_oth[2])

    ## finally, build arrays
    valsa=array.array("f",vals)
    colorsa=array.array("i",colors)

    ## build TPie with arrays just built
    pie=ROOT.TPie(title,title,n_el,valsa,colorsa)

    ## input labels for each entry
    for i in range(n_el):
            pie.SetEntryLabel(i,labels[i])

    ## set draw options and pie radius
    if opt=="perc": pie.SetLabelFormat("%perc")
    elif opt=="lab": pie.SetLabelFormat("%txt")
    else: pie.SetLabelFormat("#splitline{%perc}{%txt}")

    pie.SetRadius(0.25)

    ## if leg return pie and leg. If not, only leg
    if not leg: return pie
    return pie, legFromPie(pie,n_el,sort,oth)


#---------------------------------------------------

def drawPieLessElements(i_dict,els=0,title="",labels=0,colors=0,opt="perc",leg=True):
    
    """
    Draw pie from a dict only keeping highest values, grouping the rest as 'others' 
    @param i_dict Input dictionary. Must have format {'a':1,'b':2,...}
    @param els (int) Number of elements you want to keep in pie. Rest will be within 'others'
    @param title (string) Title of pie
    @param labels Dictionary with labels for each sector. Format {'a':'austria','b':'belgium',...}. Default, keys of input dictionary.
    @param colors Dictionary with color for each sector. Format {'a':'red','b':'green',...}. Default, first colors from 2 onwards
    @param opt 'both': show both percentages and labels in pie/'perc': show only percentages in pie/'lab': show only labels. Default 'both'
    @param leg (Bool) If true also returns a list having a legend with relation labels-colors. Legend is first element of this list. Second is a list of boxes needed to draw legend. Default True.
    @returns Pie with information and, if leg, also list with legend.
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """
    ## set initial number of elements
    n_el=len(i_dict)

    ## els must be bigger than one
    if els<1:
        print "ERROR, ELS IS SMALLER THAN 1"
        if leg: return None, None
        else: return None

    ## build a table with values in dict and keys
    table=[]
    for k in i_dict: table.append([i_dict[k],k])

    ## sort out this table from biggest value to smallest
    table.sort(reverse=True)

    ## keep only as many elements of dict as asked ("els" elements). Do the same for labels and colors
    o_dict={}
    o_colors={}
    o_labels={}
    for i in range(els):
        this=table[i]
        o_dict[this[1]]=this[0]
        if colors!=0: o_colors[this[1]]=colors[this[1]]
        if labels!=0: o_labels[this[1]]=labels[this[1]]

    if labels==0: o_labels=0
    if colors==0: o_colors=0

    ## build others category by adding rest of elements and setting is color to next not in list
    others=0
    for j in range(els,n_el): others+=table[j][0]
    o_dict["oth"]=others
    if colors!=0:
        for c in COLOR_DICT:
            if c not in o_colors.values() and isinstance(c,str): o_colors["oth"]=c

    
    ## if leg, return both pie and legend
    if leg:
        pie,legend= drawPieFromDict(o_dict,title,o_labels,o_colors,opt,True,leg,True)
        return pie,legend

    ## else, return only pie
    return drawPieFromDict(o_dict,title,o_labels,o_colors,opt,True,leg,True)


#---------------------------------------------------

def histFromList(i_list,title="",name="",nbins=70,fbin="def",lbin="def",color="black",xtitle="",ytitle="# of occurrences"):
    """
    Draw a histogram from elements in a list. 
    @param i_list Input list with numbers to fill histogram
    @param title (string) Title of histogram. Default empty string
    @param name (string) Name of histogram. Default empty string
    @param nbins (int) Number of bins of histogram. Default 70
    @param fbin (float) First Bin. Default smallest element in i_list
    @param lbin (float) Last Bin. Default biggest element in i_list
    @param color (string) Color of histogram. Default black
    @param xtitle (string) Title of x axis. Default empty string
    @param ytitle (string) Title of y axis. Default '# of occurrences'
    @returns histogram with provided information
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """
    ## check input conditions
    cont,title,name,nbins,fbin,lbin,color,xtitle,ytitle=checkHistInputs(i_list,title,name,nbins,fbin,lbin,color,xtitle,ytitle)

    ## if conditions not well accomplished, we're done
    if not cont:
        print "WRONG INPUT: NOT A LIST"
        return None

    ## build hist and fill it
    out_hist=ROOT.TH1F(name,title,nbins,fbin,lbin)
    for o in i_list:
	out_hist.Fill(o)
    ## set axis titles, colors and some options
    out_hist.SetXTitle(xtitle)
    out_hist.SetYTitle(ytitle)
    out_hist.SetLineColor(color)
    out_hist.GetYaxis().SetTitleOffset(1.2)
    ## return hist
    return out_hist
