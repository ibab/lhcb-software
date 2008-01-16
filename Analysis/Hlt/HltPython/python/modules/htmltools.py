# =============================================================================
""" @namespace htmltools
@brief Collection of functions to automatically produce html text and tables
@author Hugo RUIZ hugo.ruiz@cern.ch
@author Elias LOPEZ elopez@ecm.ub.es
@date 2008-01-15# 
"""

#---------------------------------------------------
def createHtmlTable(listOfLists,firstRowBold = False,border = 1,oneColumn = False, withBullets = False, fontColor = '#000000',bold = False,textAlign = 'center',allBold = False,alternateOnChange = False):
    """ Creates Html table from a list(rows) of lists(cells)
    @param listOfLists Contains a list of column rows (a row is itself a list)
    @param firstRowBold Text in first row will be shown in bold font
    @param border Table border thickness
    @param oneColumn If set to true, table has only one column. In that case listOfList must be a simple list, not a list of lists.
    @param fontColor Self explanatory, in hexadecimal
    @bold All text contents in bold font
    @textAlign 'left','right' or 'center'
    @alternateOnChange If set to true, background color of row will change everytime that the contents of the first column change.
    @returns html code of a table
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """


    # Basic check
    if not listOfLists: return ''

    if bold:
        boldStr = ' ;font-weight:bold'
    else:
        boldStr = ''

    textAlignStr = '; text-align:' + textAlign
       
    # Prepare the one-column case
    if oneColumn:
        list = listOfLists
        listOfLists = []
        for i in list:
            if withBullets:
                i = '\n<ul style = "margin-left: 20">\n<li>' + str(i) + '</li>'
            listOfLists.append([i])

    # Begin creating table
    output = ""
    if len(listOfLists):

        output += '\n <table size="-1" border="' + str(border)
        output += '" bordercolor="C0C0C0" bgcolor="#FFFFFF"size="-1" style="border-collapse: collapse; color:'
        output += fontColor + boldStr + textAlignStr + '" cellpadding="0" cellspacing="0"> '

        firstRow = 1
        previousFirstCellContent = ''
        thisBackgroundWhite = False
        for row in listOfLists:
            output = output + '\n <tr> \n'
            if (firstRow & firstRowBold or allBold):
                boldSwitchBegin = '<strong>'
                boldSwitchFinish = '</strong>'
                firstRow = 0
            else:
                boldSwitchBegin = ' '
                boldSwitchFinish = ' '

            firstCell = True
            for cell in row:
                              
                cellContent = ' ';cellLink = ' ';cellProps = ' '
                if (str(cell) == cell):
                    cellContent = str(cell)
                else:
                    try:
                        cellContent = str(cell[0])
                    except:
                        cellContent = " "
                        
                    if (cellContent == ""):
                        cellContent = " "
                    try:
                        if (str(cell[1]).strip() != ""):
                            cellLink = '<a href="' + str(cell[1]) + '"> '
                        try:
                            cellProps = str(cell[2])
                        except:
                            pass
                    except:
                        pass
                    
                # Alternate background color
                if alternateOnChange:
                    if firstCell:
                        if previousFirstCellContent != cellContent:
                            if thisBackgroundWhite:
                                thisBackgroundWhite = False
                            else:
                                thisBackgroundWhite =  True
                        previousFirstCellContent = cellContent

                    if thisBackgroundWhite:
                        cellProps+='bgcolor="#FFFFFF"'
                        cellContent = cellContent.replace('<td','<td bgcolor="#FFFFFF"')
                    else:
                        cellProps+='bgcolor="#F0F8FF"'
                        cellContent = cellContent.replace('<td','<td bgcolor="#F0F8FF"')
                if firstCell:
                    firstCell = False


                output += '<td ' + cellProps + '>' + boldSwitchBegin
                output += cellLink + cellContent + boldSwitchFinish + '</div></td>'

    output = output + '</table>'

    return output

#---------------------------------------------------
def href( text, reference ):
    """ Creates text with html reference
    @param text Text to show
    @param reference Link to add
    @returns html code
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    return '<a href = "'+reference+'">'+text+'</a>'
