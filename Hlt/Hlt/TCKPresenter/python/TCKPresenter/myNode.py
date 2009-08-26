
################################################
## basic script for the TCK-Presenter Project ##
## Klaus Neumann                              ##
## 17.07.09                                   ##
## CERN-PH-LHCb                               ##
################################################

## import the used constants
import myConst

## used functions and constants
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
REPLACE_RULES              = myConst.REPLACE_RULES

## makes a nice and understandable representation for the cuts
def cutlayout(cut):
    try:
        retstr=''
        sublist = cut.split(' , ')
        newlist = []
        for sublistelement in sublist:
            sublistelement = sublistelement.replace('[ ', '')
            sublistelement = sublistelement.replace(' ]', '')
            sublistelement = sublistelement.replace("'", '')
            newlist.append(formatter(sublistelement))
        retstr = newlist[0]
        for nl in newlist[1:len(newlist)]:
            retstr = retstr + ' , ' + nl
        return retstr
    except:
        return cut

## produces a nice and understandable representation for the cuts as well as cutlayout
def formatter(str):
    try:
        retstr = ''
        strlist = str.split(',')

        if strlist[1] == '>':
            retstr = strlist[0] + ' &gt; ' + strlist[2]
        elif strlist[1] == '<':
            retstr = strlist[0] + ' &lt; ' + strlist[2]
        elif strlist[1] == '||>':
            retstr = '|' + strlist[0] + '|' + ' &gt; ' + strlist[2]
        elif strlist[1] == '||<':
            retstr = '|' + strlist[0] + '|' + ' &lt;' + strlist[2]
        elif strlist[1] == '||[]':
            retstr = '|' + strlist[0] + '|' + ' &isin; ' + '['+strlist[2]+', '+strlist[3]+']'
        elif strlist[1] == '[]':
            retstr = strlist[0] + ' &isin; ' + '['+strlist[2]+', '+strlist[3]+']'
        return retstr
    except:
        return str

## the choosen standard way to start graphs in a dot file
def graph_start():
    str = """digraph g {

      graph[rankdir="TB"];
      node[shape="plaintext"];    
      edge[arrowsize = "0.7"];
    """
    return str

## create a label fo the representation in the xhtml files and the graphes, basically a subcluster
def create_label(name, label, type):
    #for rr in REPLACE_RULES:
    #    label = label.replace(rr[1],rr[2])
        
    str = 'subgraph cluster_'+name+' { rankdir="TB"; scale = "0.5"; style=filled; color="'+type[3]+'"; node[] labelloc = "b"'
    str = str + 'label = <<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="4"><TR><TD BGCOLOR="'+type[3]+'" BORDER="1"><FONT POINT-SIZE="14"> '+label+' </FONT></TD></TR></TABLE>>'

    return str

## creates a string for a node which will be used in the dot representation for a node.
def create_node(name, label, url, url2, type, cut=''):
    for rr in REPLACE_RULES:
        label = label.replace(rr[1],rr[2])

    ## if no url is given no hyperlink will appear
    str = name + '[label=<<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="'+type[7]+'">'
    str = str + '<TR><TD HREF="'+url+'" BGCOLOR="'+type[3]+'" BORDER="'+type[2]+'"><FONT POINT-SIZE="'+type[4]+'">'+label+'</FONT></TD></TR>'
    if type[0] == 'y':
        str = str + '<TR><TD BGCOLOR="'+type[1]+'" CELLPADDING="1"></TD></TR><TR><TD BGCOLOR="'+type[3]+'" CELLPADDING="1"></TD></TR>'
    if cut!='':
        str = str + '<TR><TD BORDER="0"><FONT POINT-SIZE="'+type[5]+'">'+cut+'</FONT></TD></TR>'
        if type == NODE_CUT:
            str = str + '<TR><TD HREF="'+name+'" BORDER="0"><FONT POINT-SIZE="'+type[5]+'">cut= $'+name+'_out </FONT></TD></TR>'
    if type[6]!='':
        if url2 != '':
            str = str + '<TR><TD target="rightframe" HREF="'+url2+name+'" BORDER="0"><FONT POINT-SIZE="'+type[5]+'">'+type[6]+'= $'+name+'_'+type[6]+' </FONT></TD></TR>'
        else:
            str = str + '<TR><TD BORDER="0"><FONT POINT-SIZE="'+type[5]+'">'+type[6]+'= $'+name+'_'+type[6]+' </FONT></TD></TR>'
    str = str + '</TABLE>>]'
    return str

## render an edge
def render_dir_edge(node_src, node_des):
    str = '\r\r"' + node_src + '" -> "' + node_des + '"'
    return str
