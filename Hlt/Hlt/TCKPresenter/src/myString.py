
################################################
## basic script for the TCK-Presenter Project ##
## Klaus Neumann                              ##
## 17.07.09                                   ##
## CERN-PH-LHCb                               ##
################################################

## some important functions are imported.
import re
import myConst

## replacement rules are importnat for the consistent names in the
## nameing. commas and other things should not appear in the naming,
## because it makes problems with the xhtml sites.
REPLACE_RULES = myConst.REPLACE_RULES

## the 'strange' letters are replaced by a code (_number_) given by the
## REPLACE_RULES (myConst)
def replacer_to_code(word):
    for rr in REPLACE_RULES:
          word = word.replace(rr[0],rr[1])
    return word

## the 'strange' letters are replaced by a code which is used
## in xhtml given by the REPLACE_RULES (myConst)
def replacer_to_html(word):
    REPLACE_RULES.reverse()
    for rr in REPLACE_RULES:
          word = word.replace(rr[1],rr[2])
    return word

## the code will be replaced by the 'strange' letter
## given by the REPLACE_RULES (myConst)
def replacer_to_sign(word):
    REPLACE_RULES.reverse()
    for rr in REPLACE_RULES:
          word = word.replace(rr[1],rr[0])
    return word

## counts the level in the hierarchy which is in the TCK string.
## the level is given by the whitespaces in front of the word divided by three
def Stagecontrol(line):
    count=0
    for i in range(len(line)):
       if line[i]==' ':
           count=count+1
       else:
           break
    return count/3

## returns the whole line of the string, if one gives the line name.
def Linefinder(name, linelist):
    for i in range(len(linelist)):
        if name == nf(linelist[i],''):
            return linelist[i]

## the same as above but gives the index back.
def LineIndexfinder(name, linelist):
    for i in range(len(linelist)):
        if name == nf(linelist[i],''):
            return i

## short name for Namefinder
def nf(l, r):
    return Namefinder(l, r)

## Namefinder extractes the line names out of the TCK string  
def Namefinder(line, remover):
    retline=''
    tline=''
    flag=0
    for i in range(len(line)):
       if line[i]!=' ':
           retline = retline + line[i]
           flag=1
       if line[i]==' ' and flag==1:
           break
    n = len(remover)
    i=0
    tline = retline
    while i<n and i<len(retline):
        if retline[i] != remover[i]:
            break
        i = i + 1
    if i < len(retline) - 1:
        tline = retline[i:len(retline)]
        
    return tline

## gives the index of the next lines back with the same or a smaller level.
def Nextstation(line, start, linelist):
    stage = Stagecontrol(line)
    c=start
    while c < len(linelist)-1:
        c = c + 1
        if Stagecontrol(linelist[c]) <= stage:
            return c
    return c+1

## gives all the lines back with respect to a certain line, where all the levels are smaller.
def successors(parent, linelist, mode):
    pnl = nf(parent,'')
    pstage = Stagecontrol(parent)
    for i in range(len(linelist)):
        if pnl == nf(linelist[i],''):
            ns = Nextstation(linelist[i], i, linelist)
            break
    retlist = linelist[i+1:ns]
    newretlist = []
    if mode == 0:
        return retlist
    for i in range(len(retlist)):
        newretlist.append(nf(retlist[i],''))
    return newretlist

## gives all the lines back with respect to a certain line, where all the levels are smaller by one.
def toplevelsuccessors(parent, linelist, p):
    pattern = re.compile(p[0])
    pn  = nf(parent,'')
    pnl = Linefinder(pn, linelist)  
    sl  = successors(parent, linelist, 0)
    stage = Stagecontrol(pnl)
    returnlist = []
    for i in range(len(sl)):
        if stage == Stagecontrol(sl[i])-1:
            if pattern.search(nf(sl[i],'')) != None:
                    returnlist.append(nf(sl[i],''))
    if p[1]!='':
        nonpattern = re.compile(p[1])
        for elem in returnlist:
            if nonpattern.search(elem) != None:
                returnlist.remove(elem)
                
    return returnlist

## gives the lowest line index back which index and level is smaller then the certain line and 
def parent(child, linelist):
    ch = nf(child,'')
    stage = Stagecontrol(child)
    i = LineIndexfinder(ch, linelist)
    while i>0:
        i=i-1
        if Stagecontrol(linelist[i])==stage-1:
            break
    return linelist[i]

## derives filenames from the structure for the xhtml sites.
def getfilename(child, name, linelist):
    if Stagecontrol(child) == 0:
        if name == '':
            return nf(child,'')
        else:
            return nf(child,'') + '_' + name
    p = parent(child, linelist)
    if name == '':
        return getfilename(p, nf(child,''), linelist)
    else:
        return getfilename(p, nf(child,'') + '_' + name, linelist)

## makes the names shorter in respect what is known allready about the line or the cut.
def derivelinenames(alley, list):
    retlist = []
    for l in list:
        retlist.append(derivelinename(alley, l))
    return retlist

## makes a name shorter in respect what is known allready about the line or the cut.
def derivelinename(alley, str):
    l=str
    
    ind = l.find(alley) + len(alley)
    if ind == len(l):
        ind = 0
    if l.find(alley) == -1:
        ind = 0

    return l[ind:len(l)]


