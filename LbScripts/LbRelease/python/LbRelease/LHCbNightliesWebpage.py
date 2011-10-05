#!/usr/bin/env python
#@PydevCodeAnalysisIgnore
#############################################################################
# File moved from LCG Nightlies repository, as the original is not used any #
# more in the LCG Nightlies and the modifications done are LHCb specific.   #
#############################################################################

import os, sys, time, string
import re
import logging
from checkTestLogs import checkTestLogs
#import configuration  # <--- doesn't work with acrontab on lxplus5
import BaseConfiguration as configuration
from optparse import OptionParser
from LbUtils.afs.directory import getDirID, getDirVolumeID, getDirVolumeName, getParentMountPoint, NotInAFS, isMountPoint, Directory, isAFSDir, isAFSFile
from LbUtils.afs.volume import Volume
from NightliesXML import messageXML, unique

log=logging.getLogger("nightlies.shownightlies")
h=logging.FileHandler("showNightlies.log")
h.setFormatter(logging.Formatter('%(asctime)s %(levelname)s %(message)s'))
log.addHandler(h)
log.setLevel(logging.INFO)
log.info("*****************************************************************")

day_names = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" ]

rssHtml = '<img src="http://lhcb-nightlies.web.cern.ch/lhcb-nightlies/images/rss.gif" width="16" height="16" border="0" valign="middle" />'

startTime = time.clock()

configHistory = []

# # Dictionary for harvesting problems:
# #   classes of problems so far:
# #     FileSystem = file system problems kept in a list of tuples with the dir, total size, free size, i.e. systemHealth['Warning']['FileSystem'] = [(dir1,total1,free1),(dir2,total2,free2),...]
systemHealth = {'Warning' : {}, 'Error' : {}}
systemHealth['Warning']['FileSystem'] = {'Header':('volume/path','total space','free space'),'Data':[]}
systemHealth['Error']['FileSystem'] = {'Header':('volume/path','total space','free space'),'Data':[]}
# # Pathes to check for their health
#systemHealthPathes = ['/afs/cern.ch/lhcb/software/nightlies', '/afs/cern.ch/lhcb/software/nightlies/www','/tmp/certainly/missing/path']
systemHealthPathes = ['/afs/cern.ch/lhcb/software/nightlies', '/afs/cern.ch/lhcb/software/nightlies/www']

# --------------------------------------------------------------------------------

def getBuildSummary(slotObj, day, pkgName, plat):
    nWarn   = None
    nErr    = None
    nMkErr  = None
    nCMTErr = None

    logDir = slotObj.wwwDir(plat)

    summaryFileName = slotObj.getName() + "." + day + "_" + pkgName +"-"+plat+"-log.summary"

    try:
        sumFil = open( os.path.join(logDir, summaryFileName), 'r')
    except OSError:
        return nWarn, nErr, nMkErr, nCMTErr
    except Exception, e:
        return (None,None,None,None) #0,1,0,0

    lines = sumFil.readlines()
    sumFil.close()

    try:
        nWarn, nErr, nMkErr, nCMTErr = lines[1].split(',')
    except IndexError:
        nWarn, nErr, nMkErr, nCMTErr = 0,1,0,0

    return nWarn, nErr, nMkErr, nCMTErr

def strStrip(toStr):
    return str(toStr).strip()

# ================================================================================

def showAll(htmlFile, xmlData, generalConf, slotList, historyFromSvn = False):

    # hide slots with "hidden='true'" flag in current xml
    hideSlots = ['']
    for x in slotList: # remove hidden slots
        if x.getVisibility() == False and x.getName() not in hideSlots: hideSlots.append(x.getName())

    bgndCol = '#f8eada'
    wheat4  = '#ffb875'
    wheat   = '#ffffcc'
    grey    = '#808080'
    green   = '#00f900'
    orange  = '#ffb600'
    red     = '#ff1c00'

    colour = { "unknown" : wheat,
               "OK"   : green,
               "warn" : orange,
               "err"  : red,
               }


    mytime = time.localtime();
    today = mytime[6]
    days = []                                                #################### days
    i = today
    n = 0
    while n < 7:
      if (i < 0):
        i = 6
      days.append(day_names[i])
      i = i - 1
      n = n + 1

    slotList = []
    slots = [] #names
    projList = []
    for dayn in xrange(1,7): # without today
        for sl in configHistory[dayn]._slotList:
            if sl.getName() not in slots and sl.getName() not in hideSlots:
                slots.append(sl.getName())
                slotList.append(sl)
                for p in sl.getProjects(hideDisabled=False):
                    pName = p.getName()
                    if pName not in projList: projList.append(pName)
    for sl in configHistory[0]._slotList: # today
        if sl.getName() not in hideSlots:
            if sl.getName() not in slots:
                slots.append(sl.getName())
                slotList.append(sl)
            for p in sl.getProjects(hideDisabled=False):
                pName = p.getName()
                if pName not in projList: projList.append(pName)

    htmlFile.write( '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">'+"\n")
    htmlFile.write( '<html>'+"\n")
    htmlFile.write( '  <head>'+"\n")
    htmlFile.write( '    <title>')
    if generalConf.has_key('wwwtitle'): htmlFile.write(generalConf['wwwtitle'])
    else: htmlFile.write( 'Summaries of nightly builds for LCG AA projects')
    htmlFile.write( '</title>'+"\n")

    htmlFile.write( """
        <link rel="stylesheet" type="text/css" href="http://lhcb-nightlies.web.cern.ch/lhcb-nightlies/css/screen.css">

        <script type="text/javascript" src="http://lhcb-nightlies.web.cern.ch/lhcb-nightlies/js/helpers.js"></script>
        <script type="text/javascript" src="http://lhcb-nightlies.web.cern.ch/lhcb-nightlies/js/date.js"></script>
        <script type="text/javascript" src="http://lhcb-nightlies.web.cern.ch/lhcb-nightlies/js/form.js"></script>
        <script type="text/javascript" src="http://code.jquery.com/jquery-latest.js"></script>

<script type="text/javascript">
function genrss() {
    $.typ = 'all'
    if ($("input[id='build']:checked").val() == 'build') { $.typ = 'build' }
    else if ($("input[id='test']:checked").val() == 'test') { $.typ = 'test' }
    $.level = 'warn'
    if ($("input[id='err']:checked").val() == 'err') { $.level = 'err' }
    else if ($("input[id='always']:checked").val() == 'all') { $.level = 'all' }
    $.age = '2'
    $.rss = 'http://cern.ch/lhcb-nightlies/cgi-bin/rss.py'
    $.rssp = ''
    if ($('#slot').val() != '') { $.rssp += '&slot=' + $('#slot').val() }
    if ($('#plat').val() != '') { $.rssp += '&plat=' + $('#plat').val() }
    if ($('#proj').val() != '') { $.rssp += '&proj=' + $('#proj').val() }
    if ($.typ != 'all') { $.rssp += '&type=' + $.typ }
    if ($.age != '2')  { $.rssp += '&age=' + $.age }
    if ($.level != 'warn') { $.rssp += '&level=' + $.level }
    if ($.rssp.length > 0) { $.rss += '?' + $.rssp.slice(1,$.rssp.length) }
    $('#live-result').text($.rss);
}
$(document).ready(function() {
  $('#left input, #left select, #left radio, #right input, #right select, #right radio').bind('blur keyup change', genrss);
genrss();
});
</script>
""")

    htmlFile.write( '    <META HTTP-EQUIV="expires" CONTENT="Wed, 19 Feb 2003 08:00:00 GMT">'+"\n")
    htmlFile.write( '    <META HTTP-EQUIV="refresh" content="1200">'+"\n")
    htmlFile.write( '    <style type="text/css">'+"\n")
    htmlFile.write( '      td {'+"\n")
    htmlFile.write( '        font-family: arial;'+"\n")
    htmlFile.write( '        font-size: 10pt;'+"\n")
    htmlFile.write( '        color: #006699;'+"\n")
    htmlFile.write( '      }'+"\n")
    htmlFile.write( '      a {text-decoration: none}'+"\n")
    htmlFile.write( '      div.tooltip {'+"\n")
    htmlFile.write( '        position:absolute;'+"\n")
    htmlFile.write( '        visibility:hidden;'+"\n")
    htmlFile.write( '        z-index:1;'+"\n")
    htmlFile.write( '        background-color:white;'+"\n")
    htmlFile.write( '      }'+"\n")
    htmlFile.write( '      h1 {font-family: arial; color: #CCCCCC;}'+"\n")
    htmlFile.write( '      h2 {font-family: arial;}'+"\n")
    htmlFile.write( '      h3 {font-family: arial; color: #006699;}'+"\n")
    htmlFile.write( '      h4 {font-family: arial;  color: #666666;}'+"\n")
    htmlFile.write( '      a.header_link{' + "\n")
    htmlFile.write( '      padding: 0px 20px;' + "\n")
    htmlFile.write( '          font-weight: bold;' + "\n")
    htmlFile.write( '          font-size: 12pt;' + "\n")
    htmlFile.write( '          text-decoration:none;' + "\n")
    htmlFile.write( '          color:#FF0000;' + "\n")
    htmlFile.write( '      }'+"\n")
    htmlFile.write( '      a.header_link_black{' + "\n")
    htmlFile.write( '      padding: 0px 20px;' + "\n")
    htmlFile.write( '          font-weight: bold;' + "\n")
    htmlFile.write( '          font-size: 12pt;' + "\n")
    htmlFile.write( '          text-decoration:none;' + "\n")
    htmlFile.write( '          color:#000000;' + "\n")
    htmlFile.write( '      }'+"\n")
    htmlFile.write( '    </style>'+"\n")
    htmlFile.write( '  '+"\n")
    htmlFile.write( '<script language="JavaScript1.2">'+" \n")
    htmlFile.write( "<!-- Begin\n")
    htmlFile.write( "var IE = document.all?true:false;\n")
    htmlFile.write( "if (!IE) document.captureEvents(Event.MOUSEMOVE)\n")
    htmlFile.write( "document.onmousemove = getMouseXY;\n")
    htmlFile.write( "var tempX = 0;\n")
    htmlFile.write( "var tempY = 0;\n")

    htmlFile.write( "function getMouseXY(e) {\n")
    htmlFile.write( "  if (IE) { // grab the x-y pos.s if browser is IE\n")
    htmlFile.write( "    tempX = event.clientX + document.body.scrollLeft;\n")
    htmlFile.write( "    tempY = event.clientY + document.body.scrollTop;\n")
    htmlFile.write( "  } else {  // grab the x-y pos.s if browser is NS\n")
    htmlFile.write( "    tempX = e.pageX;\n")
    htmlFile.write( "    tempY = e.pageY;\n")
    htmlFile.write( "  }\n")
    htmlFile.write( "  if (tempX < 0) {tempX = 0;}\n")
    htmlFile.write( "  if (tempY < 0) {tempY = 0;}\n")

    htmlFile.write( "  return true;\n")
    htmlFile.write( "}\n")

    htmlFile.write( "function tooltip(event,tooltip) {\n")
    htmlFile.write( "  var pageWidth = (document.layers)?window.innerWidth:window.document.body.offsetWidth;\n")
    htmlFile.write( "  elem = document.getElementById(tooltip);\n")
    htmlFile.write( "  divwidth = elem.offsetWidth;\n")
    htmlFile.write( "  if (tempX+5+divwidth > pageWidth) {\n")
    htmlFile.write( '    elem.style.left=(tempX+5-divwidth)+"px";'+" \n")
    htmlFile.write( "  } else {\n")
    htmlFile.write( '    elem.style.left=(tempX+5)+"px";'+" \n")
    htmlFile.write( "  }\n")
    htmlFile.write( '  elem.style.top=(tempY+5)+"px";'+" \n")
    htmlFile.write( '  elem.style.visibility="visible";'+" \n")
    htmlFile.write( "}\n")

    htmlFile.write( "function tooltipout(event,tooltip) {\n")
    htmlFile.write( "  elem = document.getElementById(tooltip);\n");
    htmlFile.write( '  elem.style.left=(-1000)+"px";'+" \n")
    htmlFile.write( '  elem.style.top=(-1000)+"px";'+" \n")
    htmlFile.write( '  elem.style.visibility="hidden";'+" \n")
    htmlFile.write( "}\n")
    htmlFile.write( "//  End -->\n")
    htmlFile.write( "</script>\n")

    htmlFile.write( '    <script language="JavaScript">'+"\n")
    htmlFile.write( '      var slot_mask = /day_cbox/;'+"\n")
    htmlFile.write( '      var slot_mask = /slot_cbox/;'+"\n")
    htmlFile.write( '      var slot_mask = /proj_cbox/;'+"\n")
    htmlFile.write( '    '+"\n")
    htmlFile.write( '      function checkall(mask) {'+"\n")
    htmlFile.write( '        for(i=0; i<document.display_criteria.elements.length; i++) {'+"\n")
    htmlFile.write( '          if(document.display_criteria.elements[i].type=="checkbox") {'+"\n")
    htmlFile.write( '            if(document.display_criteria.elements[i].name.search(mask) != -1) {'+"\n")
    htmlFile.write( '              document.display_criteria.elements[i].checked=true;'+"\n")
    htmlFile.write( '            }'+"\n")
    htmlFile.write( '          }'+"\n")
    htmlFile.write( '        }'+"\n")
    htmlFile.write( '      }'+"\n")
    htmlFile.write( '    '+"\n")
    htmlFile.write( '      function uncheckall(mask) {'+"\n")
    htmlFile.write( '        for(i=0; i<document.display_criteria.elements.length; i++) {'+"\n")
    htmlFile.write( '          if(document.display_criteria.elements[i].type=="checkbox") {'+"\n")
    htmlFile.write( '            if(document.display_criteria.elements[i].name.search(mask) != -1) {'+"\n")
    htmlFile.write( '              document.display_criteria.elements[i].checked=false;'+"\n")
    htmlFile.write( '            }'+"\n")
    htmlFile.write( '          }'+"\n")
    htmlFile.write( '        }'+"\n")
    htmlFile.write( '       }'+"\n")
    htmlFile.write( '    </script>'+"\n")

    htmlFile.write( "    <script language='JavaScript' type='text/javascript'>"+"\n")

    htmlFile.write( '    function toggleMe(a){'+"\n")
    htmlFile.write( '      var e=document.getElementById(a);'+"\n")
    htmlFile.write( '      var t=document.getElementById(a+"_toggle")'+"\n")
    htmlFile.write( '      if(!e)return true;'+"\n")
    htmlFile.write( '      if(e.style.display=="none"){'+"\n")
    htmlFile.write( '        e.style.display=""'+"\n")
    htmlFile.write( '        t.innerHTML = "-"'+"\n")
    htmlFile.write( '      } else {'+"\n")
    htmlFile.write( '        e.style.display="none"'+"\n")
    htmlFile.write( '        t.innerHTML = "+"'+"\n")
    htmlFile.write( '      }'+"\n")
    htmlFile.write( '      return true;'+"\n")
    htmlFile.write( '    }'+"\n")
    htmlFile.write( '      </script>'+"\n")

    htmlFile.write( '  </head>'+"\n")

    htmlFile.write( '<body>'+"\n")
#    htmlFile.write( '<script type="text/javascript" src="http://lhcb-nightlies.web.cern.ch/lhcb-nightlies/js/ZeroClipboard.js"></script>')

    htmlFile.write( '<!-- slots:')

    insert_comma = 0
    for slot in slots:
      if insert_comma == 1: htmlFile.write(',')
      htmlFile.write(slot)
      insert_comma = 1
    htmlFile.write( ' -->'+" \n")

    htmlFile.write( '<!-- projects:')
    insert_comma = 0
    for proj in projList:
      if insert_comma == 1: htmlFile.write(',')
      htmlFile.write(proj)
      insert_comma = 1
    htmlFile.write( ' -->'+" \n")

    htmlFile.write( '<div align="center">'+" \n")
    htmlFile.write( '<table width="100%" border="0">'+" \n")
    htmlFile.write( '  <tr>'+" \n")
    htmlFile.write( '    <td align="center" width="1%" nowrap><table><tr><td valign="top" bgcolor="#990000"><a href="https://twiki.cern.ch/twiki/bin/view/SPI/SpiNightlyBuilds"><font face="Arial" color="#FFFFFF"><b>News</b></font></a></td></tr></table></td>'+" \n")
    htmlFile.write( '    <td align="center"><img border="0" src="http://lcgapp.cern.ch/NBlogo.jpg" height="80" style="vertical-align:middle"><font face="Arial" color="#006699" size="6">')
    if generalConf.has_key('wwwtitle'): htmlFile.write(generalConf['wwwtitle'])
    else: htmlFile.write( 'Summaries of nightly builds for LCG AA projects')
    htmlFile.write( '</font><font face="Arial" color="#666666" size="3"><br>Last update: '+time.asctime()+'</font><br /></td>'+" \n")
    xmlMetaNode = xmlData.appendSpecificNode("meta",xmlData.getDoc().documentElement)
    xmlMetaTimeNode = xmlData.appendSpecificNode("time",xmlMetaNode,{"creation":strStrip(time.asctime())})
    if generalConf.has_key('wwwurl') and 'lcgapp.cern.ch' in generalConf['wwwurl']:
        htmlFile.write( '</tr><tr><td>&nbsp;</td><td align="center"><font face="Arial">A new version of this page is available <a href="http://lcgapp.cern.ch/spi/cgi-bin/nightly.py"><b>there</b></a></font></td>'+" \n")
    htmlFile.write( '  </tr>'+" \n")
    htmlFile.write( '</table>'+" \n")
    htmlFile.write( '</div>'+" \n")

    if not generalConf.has_key('wwwurl') or 'lcgapp.cern.ch' not in generalConf['wwwurl']:
        htmlFile.write( '<a class="header_link_black">Nightlies Status</a>')
        htmlFile.write( '<a class="header_link" href="./overview_nightlies.py" target="_blank" >Configuration overview</a>')
        htmlFile.write( '<a class="header_link" href="../editor.html" target="_blank" >Configuration editor</a>')
        htmlFile.write( '<a class="header_link" href="https://svnweb.cern.ch/trac/lhcb/browser/LHCbNightlyConf/trunk/configuration.xml" target="_blank" >Configuration SVN</a>')
        htmlFile.write( '<a class="header_link" href="http://lhcb-coverity.cern.ch:8080" target="_blank" >LHCb Coverity</a>')
        htmlFile.write( '<a class="header_link" href="https://lhcb-nightlies.web.cern.ch/lhcb-nightlies/index-LHCb-cache.xml" target="_blank" >XML</a>')
        htmlFile.write( '<hr/>')

    htmlFile.write('<h3>[<a id="dc_toggle" href="#" onClick="'+"toggleMe('dc')"+'">+</a>] Display Criteria</h3>'+" \n")
    htmlFile.write('<div id="dc" style="display: none; font-family: arial; font-size: 10pt;">'+" \n")
    htmlFile.write( '<form action="nightlies.py" method="get" enctype="multipart/form-data" name="display_criteria">'+" \n")
    htmlFile.write('<font color="#666666">'+" \n")
    htmlFile.write('By default, the last visit choice(s) are used. If it is the first visit, the defaults are: day=today, slot=all, project=all, show statistics.'+" \n")
    htmlFile.write('<br>'+" \n")
    htmlFile.write('Otherwise, select the desired criteria below and click on'+" \n")
    htmlFile.write('<input type="submit" id="submitreturn" name="submitreturn" value="Apply" />'+" \n ")
    htmlFile.write('</font>'+" \n")
    htmlFile.write('<br><br>'+" \n")
    htmlFile.write('<table bgcolor="#6699CC" cellspacing="1" border="0">'+" \n")
    htmlFile.write('  <tr>'+" \n")
    htmlFile.write('    <td bgcolor="#FFFFFF" valign="top">'+" \n")
    htmlFile.write('      <table border="0" width="100%" id="table2">'+" \n")
    htmlFile.write('    <tr><td bgcolor="#6699CC" colspan="2" align="center"><font color="#FFFFFF"><b>DAY</b></font></td></tr>'+" \n")
    htmlFile.write('    <tr><td colspan="2" nowrap>'+" \n")
    htmlFile.write('      <input type="button" value="MARK ALL" onClick="'+"checkall('day_cbox')"+';">'+" \n")
    htmlFile.write('      &nbsp;'+" \n")
    htmlFile.write('      <input type="button" value="CLEAR ALL" onClick="'+"uncheckall('day_cbox')"+';">'+" \n")
    htmlFile.write('    </td></tr>'+" \n")
    htmlFile.write('          <tr><td><input type="checkbox" name="day_cbox_today" value="on"></td><td>today</td></tr>'+" \n")
    htmlFile.write('          <tr><td><input type="checkbox" name="day_cbox_yesterday" value="on"></td><td>yesterday</td></tr>'+" \n")
    htmlFile.write('          <tr><td>&nbsp;</td><td>&nbsp;</td></tr>'+" \n")
    for day in days:
      htmlFile.write('        <tr><td><input type="checkbox" name="day_cbox_'+day+'" value="on"></td><td>'+day+'</td></tr>'+" \n")

    htmlFile.write('      </table>'+" \n")
    htmlFile.write('    </td>'+" \n")
    htmlFile.write('    <td bgcolor="#FFFFFF" valign="top">'+" \n")
    htmlFile.write('      <table border="0" width="100%" id="table3">'+" \n")
    htmlFile.write('    <tr><td bgcolor="#6699CC" colspan="2" align="center"><font color="#FFFFFF"><b>SLOT</b></font></td></tr>'+" \n")
    htmlFile.write('    <tr><td colspan="2" nowrap>'+" \n")
    htmlFile.write('      <input type="button" value="MARK ALL" onClick="'+"checkall('slot_cbox')"+';">'+" \n")
    htmlFile.write('      &nbsp;'+" \n")
    htmlFile.write('      <input type="button" value="CLEAR ALL" onClick="'+"uncheckall('slot_cbox')"+';">'+" \n")
    htmlFile.write('    </td></tr>'+" \n")

    for x in slotList:
        if x.getName() in hideSlots: continue
        tmpDescription = x.getDescription()
        if tmpDescription == None: tmpDescription = x.getName()
        htmlFile.write('        <tr><td><input type="checkbox" name="slot_cbox_'+str(x.getName())+'" value="on"></td><td>'+str(tmpDescription)+'</td></tr>'+" \n")
    htmlFile.write('      </table>'+" \n")
    htmlFile.write('    </td>'+" \n")
    htmlFile.write('    <td bgcolor="#FFFFFF" valign="top">'+" \n")
    htmlFile.write('      <table border="0" width="100%" id="table4">'+" \n")
    htmlFile.write('    <tr><td bgcolor="#6699CC" colspan="2" align="center"><font color="#FFFFFF"><b>PROJECT</b></font></td></tr>'+" \n")
    htmlFile.write('    <tr><td colspan="2" nowrap>'+" \n")
    htmlFile.write('      <input type="button" value="MARK ALL" onClick="'+"checkall('proj_cbox')"+';">'+" \n")
    htmlFile.write('      &nbsp;'+" \n")
    htmlFile.write('      <input type="button" value="CLEAR ALL" onClick="'+"uncheckall('proj_cbox')"+';">'+" \n")
    htmlFile.write('    </td></tr>'+" \n")

    for proj in projList:
        htmlFile.write('        <tr><td><input type="checkbox" name="proj_cbox_'+proj+'" value="on"></td><td>'+proj+'</td></tr>'+" \n")
    htmlFile.write('      </table>'+" \n")
    htmlFile.write('    </td>'+" \n")
    htmlFile.write('    <td bgcolor="#FFFFFF" valign="top">'+" \n")
    htmlFile.write('      <table border="0" width="100%" id="table5">'+" \n")
    htmlFile.write('    <tr><td bgcolor="#6699CC" colspan="2" align="center"><font color="#FFFFFF"><b>MISC.</b></font></td></tr>'+" \n")
    htmlFile.write('        <tr><td><input type="checkbox" name="hist_cbox" value="on"></td><td>display stat histogram</td></tr>'+" \n")
    htmlFile.write('        <tr><td><input type="checkbox" name="rss_cbox" value="off"></td><td>display RSS feeds</td></tr>'+" \n")
    htmlFile.write('      </table>'+" \n")
    htmlFile.write('    </td>'+" \n")

    htmlFile.write('  </tr>'+" \n")
    htmlFile.write('</table>'+" \n")

    htmlFile.write( '</form>'+" \n")
    htmlFile.write( '</div>'+" \n")

###################################
    htmlFile.write('<h3>[<a id="rssform_toggle" href="#" onClick="'+"toggleMe('rssform')"+'">+</a>] Custom RSS feed configuration</h3>'+" \n")
    htmlFile.write('<div id="rssform" style="display: none; font-family: arial; font-size: 10pt;">'+" \n")

    htmlFile.write("""
        <form class="rssfeed" action="">
                <fieldset class="contact">
                        <legend>Custom RSS feed configuration</legend>
                    <div id="left">
                        <div class="input">
                                <label for="slot">Slot</label>
                                <select name="slot" id="slot" class="select">
                                    <option value="" selected="selected">All available slots</option>
                                    <option value="">- - - - - - - - - - - - - - - - - - - - - - - - - - </option>""")
    for x in slotList:
        if x.getName() in hideSlots: continue
        htmlFile.write('        <option value="%s">%s</option>\n' % (x.getName(), x.getName()))
    htmlFile.write("""
                                </select>
                        </div>
                        <div class="input">
                                <label for="plat">Platform</label>
                                <select name="plat" id="plat" class="select">
                                    <option value="" selected="selected">All available platforms</option>
                                    <option value="">- - - - - - - - - - - - - - - - - - - - - - - - - - </option>
                                    <option value="slc4_amd64_gcc34">slc4_amd64_gcc34</option>
                                    <option value="slc4_amd64_gcc34_dbg">slc4_amd64_gcc34_dbg</option>
                                    <option value="slc4_ia32_gcc34">slc4_ia32_gcc34</option>
                                    <option value="slc4_ia32_gcc34">slc4_ia32_gcc34_dbg</option>
                                    <option value="x86_64-slc5-gcc43-opt">x86_64-slc5-gcc43-opt</option>
                                    <option value="x86_64-slc5-gcc43-dbg">x86_64-slc5-gcc43-dbg</option>
                                    <option value="i686-slc-gcc43-opt">i686-slc-gcc43-opt</option>
                                    <option value="i686-slc-gcc43-dbg">i686-slc-gcc43-dbg</option>
                                    <option value="x86_64-slc5-icc11-dbg">x86_64-slc5-icc11-dbg</option>
                                </select>
                        </div>
                        <div class="proj">
                                <label for="proj">Project</label>
                                <select name="proj" id="proj" class="select">
                                    <option value="" selected="selected">All available projects</option>
                                    <option value="">- - - - - - - - - - - - - - - - - - - - - - - - - - </option>""")
    projListSorted = projList
    projListSorted.sort()
    for proj in projListSorted:
        htmlFile.write('        <option value="%s">%s</option>\n' % (proj.upper(), proj))
    htmlFile.write("""
                                </select>
                        </div>
                    </div>
                    <div id="right">
                        <div class="radio">
                                <fieldset>
                                        <legend><span>Type</span></legend>
                                        <div>
                                                <input type="radio" id="build" name="typ" id="typ" value="build"> <label for="build">build</label>
                                        </div>
                                        <div>
                                                <input type="radio" id="test" name="typ" id="typ" value="test"> <label for="test">test</label>
                                        </div>
                                        <div>
                                                <input type="radio" id="all" name="typ" id="typ" value="all" checked="true"> <label for="all">all</label>
                                        </div>
                                </fieldset>
                        </div>
                        <div class="radio">
                                <fieldset>
                                        <legend><span>Raporting</span></legend>
                                        <div>
                                                <input type="radio" id="err" name="level" value="err"> <label for="err">errors only</label>
                                        </div>
                                        <div>
                                                <input type="radio" id="warn" name="level" value="warn" checked="true"> <label for="warn">errors &amp; warnings</label>
                                        </div>
                                        <div>
                                                <input type="radio" id="always" name="level" value="all"> <label for="always">always</label>
                                        </div>
                                </fieldset>
                        </div>
                    </div>
                    <div id="live-result"></div>""")

#    htmlFile.write("""
#                    <div id="d_clip_button" style="border:1px solid black; padding:1px;">copy</div>
#                <script language="JavaScript">
#                        var clip = new ZeroClipboard.Client();
#                        clip.setHandCursor( true );
#                        clip.setCSSEffects( true );
#                        clip.addEventListener( 'mouseDown', function(client) {
#                            clip.setText( document.getElementById('live-result').value );
#                        } );
#//                        clip.setText( $('#live-result').val() );
#                        clip.glue( 'd_clip_button' );
#                </script>""")
    htmlFile.write("""
        </body>
                </fieldset>
        </form>
    """)

# clip.setText( document.getElementById('clip_text').value );

    htmlFile.write( '</div>'+" \n")
###################################



    htmlFile.write('<h3>[<a id="cc_toggle" href="#" onClick="'+"toggleMe('cc')"+'">+</a>] Colour coding</h3>'+" \n")
    htmlFile.write('<div id="cc" style="display: none; font-family: arial; font-size: 10pt;">'+" \n")

    htmlFile.write( '<table borders="0" cellspacing="4"> <tr>'+" \n")
    htmlFile.write( '<td bgcolor="'+colour["unknown"]+'"> Unknown </td> '+" \n")
    htmlFile.write( '<td bgcolor="'+colour["OK"]  +'"> OK </td> '+" \n")
    htmlFile.write( '<td bgcolor="'+colour["warn"]+'"> warnings </td>'+" \n")
    htmlFile.write( '<td bgcolor="'+colour["err"] +'"> errors </td> '+" \n")
    htmlFile.write( '<td bgcolor="#D3D3D3"> project disabled (not built, taken from Release Area instead) </td> '+" \n")
    htmlFile.write( '</tr></table>'+" \n")
    htmlFile.write( '</div>'+" \n")

    statdivs = []

    platformsNotReady = []

    dayNo = -1
    for day in days:
        dayNo += 1
        htmlFile.write( "\n<!-- start DAY div for " + day + " -->\n")
        htmlFile.write( '<div id="' + day + '_header' + '" style="margin-left: 10px;">')

        htmlFile.write("<h1>"+day)
        xmlDayNode = None
        if day == time.strftime("%A"):
            htmlFile.write(" (Today)")
            xmlDayNode = xmlData.appendSpecificNode("day",xmlData.getRootNode(),{"name":strStrip(day),"today":"True"})
        else:
            xmlDayNode = xmlData.appendSpecificNode("day",xmlData.getRootNode(),{"name":strStrip(day),"today":"False"})
        htmlFile.write(":<hr/></h1>")

        #aSlots = configuration.getArchivedSlotNames(day[:3])
        #if aSlots != None: slots = aSlots

        for slot in slots:
            #generalConfig, sL = configuration.readConf(configuration.pathOfCopy(slot, day[:3]))

            #slotObj = configuration.findSlot(slot, sL)
            slotObj = configHistory[dayNo].findSlot(slot)
            if slotObj is None: continue

#            if not os.path.exists( os.path.join(topDir, slot, day[:3]) ) : continue
            htmlFile.write( "\n<!-- start SLOT div for " + day + '_' + slot + " -->\n")
            htmlFile.write( '<div id="' + day + '_' + slot + '" style="margin-left: 10px;">')
            htmlFile.write( '<h4>'+day+'   Slot : '+slotObj.getDescription(returnNameIfNoDescription=True)+'</h4>')
            htmlFile.write( '&nbsp;&nbsp;<a href="http://cern.ch/lhcb-nightlies/cgi-bin/rss.py?slot=%s" target="_blank">%s</a> ' % (slotObj.getName(), rssHtml))

#            motdFile = os.path.join(topDir, slot, day[:3], "motd" )
#            if os.path.exists(motdFile):
#                htmlFile.write("<p>")
#                motd = open(motdFile, "r")
#                htmlFile.write(motd.readLines())
#                htmlFile.write("</p>")
            htmlFile.write( '<table border="1" cellpadding="0" cellspacing="0" bgcolor="'+wheat+'">')
            htmlFile.write( '  <tbody>')
            htmlFile.write( '    <tr>')
            htmlFile.write( '      <td><b>Project</b></td> ')
            htmlFile.write( '      <td><b>Version</b></td> ')

            stampDay = day[:3]

            xmlSlotNode = xmlData.appendSpecificNode("slot",xmlDayNode,{"name":strStrip(slotObj.getName())})

            for platObj in slotObj.getPlatforms():
                plat = platObj.getName()
                timeStamp = None
                if '%DAY%' in slotObj._releaseDir:
                    stampFileName = os.path.join(slotObj.releaseDir(), '..', day[:3] , 'isDone-'+plat)
                    stampFileNameStarted = os.path.join(slotObj.releaseDir(), '..', day[:3] , 'isStarted-'+plat)
                else:
                    stampFileName = os.path.join(slotObj.releaseDir(), 'isDone-'+plat)
                    stampFileNameStarted = os.path.join(slotObj.releaseDir(), 'isStarted-'+plat)
                if os.path.exists(stampFileName):
                    stampFile = open(stampFileName, 'r')
                    lines = stampFile.readlines()
                    stampFile.close()
                    if lines:
                        lines[0] = string.replace(lines[0],'\r','') # needed for windows files
                        timeStamp = lines[0][:-1]
                        if timeStamp.strip() == "0":
                            stat = os.stat(stampFileName)
                            stampTime = stat.st_mtime
                            timeStamp = time.ctime(stampTime)
                            print "timeStamp found to be zero for slot ",slot," plat ", plat,", updating from mtime:", stampTime
                            timeStamp = '<em>'+time.ctime(stampTime)+'</em>'
                        else:
                            stampTime = time.mktime( time.strptime(timeStamp) ) # convert back to sec since epoch
                        # check if timestamp is less than one week old,
                        # add a few hours to allow for the actual building
                        if time.time()-stampTime > (7*86400 + 4*3600) :
                            print "found old build for :", slot, day, plat, stampTime, "("+timeStamp+")"
                        else:
                            pass
                            #timeStamp = None

                waitForPlat = ""
                waitForIs = False
                for iter in slotObj.waitForFlag():
                    if iter is not None:
                        waitForPlat = iter+plat
                        waitForIs = True

                if timeStamp:
                    # get rid of seconds
                    date_parts = timeStamp.split(':')
                    parts = date_parts[2].split(' ')
                    timeStamp = date_parts[0]+':'+date_parts[1]+' '+parts[1]
                    stampDay = date_parts[0][:3]
                    htmlFile.write( '<td nowrap> <b> '+plat+' </b>')
                    if stampDay == day[:3]:
                        htmlFile.write( '<br/>('+timeStamp+')')
                    else:
                        htmlFile.write( '<br/><font style="background-color: yellow">(%s)</font>' % timeStamp)

                    htmlFile.write( '&nbsp;&nbsp;<a href="http://cern.ch/lhcb-nightlies/cgi-bin/rss.py?slot=%s&plat=%s" target="_blank">%s</a> ' % (slotObj.getName(), plat, rssHtml))
                    if waitForIs:
                        if os.path.exists(waitForPlat):
                            htmlFile.write( '<br/>waiting for <font style="background-color: #80FF00">%s</font>' % " LCG: OK" )
                            xmlData.modifySpecificNode(xmlSlotNode,{"LCGReady":"True"})
                        else:
                            htmlFile.write( '<br/>waiting for <font style="background-color: #FF8040">%s</font>' % " LCG")
                            xmlData.modifySpecificNode(xmlSlotNode,{"LCGReady":"False"})
                    htmlFile.write( '</td> \n')
                else:
                    platformsNotReady.append((day, slot, plat,waitForIs))
                    if string.upper(str(generalConf.get('shownotfinishedplatforms', False))) != 'FALSE' and os.path.exists(stampFileNameStarted):
                        htmlFile.write( '<td nowrap> <b> '+plat+' </b>')
                        htmlFile.write( '<br/><center><em>not ready</em></center>')
                        htmlFile.write( '</td> \n')
            htmlFile.write( '    </tr>\n')

            for proj in slotObj.getProjects(hideDisabled=False):
                xmlProjectNode = xmlData.appendSpecificNode("project",xmlSlotNode,{"name":strStrip(proj.getName())})
                if proj.getDisabledFlag() is True:
                    projDisabled = True
                else:
                    projDisabled = False
                htmlFile.write( "\n<!-- start PROJECT tr for " +day+'_'+slotObj.getName()+'_'+proj.getName()+ " -->\n")
                htmlFile.write( '    <tr id="'+day+'_'+slotObj.getName()+'_'+proj.getName()+'">'+"\n")
                # RSS ##########################################################################################################
                #htmlFile.write( '      <td bgcolor="'+wheat4+'">'+proj.getName()+'</td>\n')
                if projDisabled:
                    htmlFile.write( '      <td bgcolor="#D3D3D3">')
                else:
                    htmlFile.write( '      <td bgcolor="'+wheat4+'">')
                htmlFile.write( proj.getName())
                htmlFile.write( '&nbsp;&nbsp;<a href="http://cern.ch/lhcb-nightlies/cgi-bin/rss.py?slot=%s&proj=%s" target="_blank">%s</a> ' % (slotObj.getName(), proj.getName().upper(), rssHtml))
                htmlFile.write( '</td>\n')
                ################################################################################################################
                if proj.getRename() is None:
                    projectTag = proj.getTag()
                else:
                    projectTag = proj.getRename()
                if projDisabled:
                    htmlFile.write( '      <td bgcolor="#D3D3D3">'+projectTag+'</td>\n')
                else:
                    htmlFile.write( '      <td bgcolor="'+wheat+'">'+projectTag+'</td>\n')

                for platObj in slotObj.getPlatforms():
                    xmlPlatformNode = xmlData.appendSpecificNode("platform",xmlProjectNode,{"name":strStrip(plat)})
                    if timeStamp:
                        xmlData.modifySpecificNode(xmlPlatformNode,{"platCompleted":strStrip(timeStamp),"platReady":"True"})
                        if waitForIs:
                            xmlData.modifySpecificNode(xmlSlotNode,{"platDepends":"True"})
                            if os.path.exists(waitForPlat):
                                xmlData.modifySpecificNode(xmlSlotNode,{"LCGReady":"True"})
                            else:
                                xmlData.modifySpecificNode(xmlSlotNode,{"LCGReady":"False"})
                    else:
                        if string.upper(str(generalConf.get('shownotfinishedplatforms', False))) != 'FALSE' and os.path.exists(stampFileNameStarted):
                            xmlData.modifySpecificNode(xmlPlatformNode,{"platReady":"False"})

                    plat = platObj.getName()

                    if (day, slot, plat,True) in platformsNotReady or (day, slot, plat,False) in platformsNotReady:
                        stampFileNameStarted = os.path.join(slotObj.releaseDir(), '..', day[:3] , 'isStarted-'+plat)
                        if string.upper(str(generalConf.get('shownotfinishedplatforms', False))) == 'FALSE' or not os.path.exists(stampFileNameStarted): continue
                    htmlFile.write( '<td><table border="0" cellpadding="4" cellspacing="1" width="100%">\n<tr>\n')

                    #logFileName = slotObj.getName()+'.'+day[:3]+'_'+projectTag+'-'+plat+'-log.html'
                    logFileName = slotObj.getName()+'.'+stampDay+'_'+projectTag+'-'+plat+'-log.html'

                    #nWarn, nErr, nMkErr, nCMTErr = getBuildSummary(slotObj, day[:3], projectTag, plat)
                    nWarn, nErr, nMkErr, nCMTErr = getBuildSummary(slotObj, stampDay, projectTag, plat)
                    noFile = False
                    if (nWarn, nErr, nMkErr, nCMTErr) == (None, None, None, None): noFile = True
                    if projDisabled:
                        noFile = True
                        #xmlData.modifySpecificNode(xmlProjectNode,{"disabled":"True"})
                    if not noFile:
                        flag = "OK"
                        if int(nWarn) > 0: flag = "warn"
                        if int(nErr)  > 0: flag = "err"
                        htmlFile.write( '<td align="center" nowrap bgcolor="'+colour[flag]+'" width="50%">')
                        logURL = configHistory[dayNo]._generalConfig.getParsed('logurl',{
                                                                   configuration.PATTERN_PLATFORM:plat,
                                                                   configuration.PATTERN_WEEKDAY:day[:3],
                                                         })
                        nInPar = 0
                        NoErr = 0
                        if int(nErr)>0 or int(nMkErr)>0 or int(nCMTErr)>0 :
                            nInPar = int(nErr)+int(nMkErr)+int(nCMTErr)
                            NoErr = nInPar
                        elif int(nWarn)>0 :
                            nInPar = int(nWarn)
                        htmlFile.write( '<a href="' + str(logURL) + '/' + logFileName + '">build')
                        if nInPar : htmlFile.write( ' ('+str(nInPar)+')')
                        htmlFile.write( '</a>&nbsp;&nbsp;<a href="http://cern.ch/lhcb-nightlies/cgi-bin/rss.py?slot=%s&plat=%s&proj=%s&type=build" target="_blank">%s</a> ' % (slotObj.getName(), plat, proj.getName().upper(), rssHtml))
                        #htmlFile.write( '</a> ')
                        xmlData.modifySpecificNode(xmlPlatformNode,{"nWarn":strStrip(nWarn),"nErr":strStrip(nErr),"nMkErr":strStrip(nMkErr),"nCMTErr":strStrip(nCMTErr),"logFile":strStrip(str(logURL) + '/' + logFileName)})
                    else:
                        flag = "err"
                        if string.upper(str(generalConf.get('shownotfinishedplatforms', False))) == 'FALSE':
                            if projDisabled:
                                htmlFile.write( '<td align="center" nowrap bgcolor="#D3D3D3" width="50%">')
                            else:
                                htmlFile.write( '<td align="center" nowrap bgcolor="'+colour[flag]+'" width="50%">')
                            htmlFile.write( '---')
                        else:
                            if projDisabled:
                                htmlFile.write( '<td align="center" nowrap bgcolor="#D3D3D3" width="50%">')
                            else:
                                htmlFile.write( '<td align="center" nowrap bgcolor="white" width="50%">')
                            htmlFile.write( '&nbsp;')
                        flag = "err"
                    htmlFile.write( '</td>\n')

                    if len(configHistory[dayNo]._generalConfig.getParsed('logurl', {configuration.PATTERN_PLATFORM:plat})) != len(configHistory[dayNo]._generalConfig.getParsed('logurl', {})):
                        includePlatformInPrefix = True
                    else:
                        includePlatformInPrefix = False




                    #Status=checkTestLogs(os.path.join(slotObj.wwwDir(), plat, slotObj.getName() + "." + day[:3]+'_'+proj.getTag()+'-'+plat))
                    Status=checkTestLogs(None, None, slotObj, proj, day[:3], plat, includePlatformInPrefix)
                    if time.strftime("%A") == day:
                        pass
                        if nWarn != None:
                            build = (NoErr, nWarn)
                        else:
                            build = ("U","U")
                        try:
                            import rrd
                            rrd.rrd(str(plat), str(slotObj.getName()), str(proj.getName())).update(build, Status)
                            # (?, all, fail, success)
                        except:
                            pass

                    errStr = ''
                    if Status[2] == 0:
                        if Status[3] > 0: flag = 'OK'
                        else: flag='warn'
                    if Status[2] :
                        flag = 'err'
                        errStr = ' (' + str(Status[2]) + ')'

                    htmlFile.write( '<td align="center" nowrap width="50%" bgcolor="')
                    testboxHtml = '" '
                    if len(Status) > 4:

                        divname = 'graph_' + day + '_' + slotObj.getName() + '_' + plat + '_' + projectTag
                        #divname = 'graph_' + day + '_' + slotObj.getName() + '_' + plat + '_' + proj.getTag()
                        #divname = str(proj_id)+'_'+str(plat_id)+'_'+str(slot_id)+'_'+str(day_id)+'_'+projVers

                        eventatt =  """onmouseover="tooltip(event,'"""+divname+"""')" """
                        eventatt +=  """onmouseout="tooltipout(event,'"""+divname+"""')" """
                        testboxHtml += eventatt

                        statinfo =  '<div id="'+divname+'" class="tooltip"><pre>'
                        for divdata in Status[4]:
                            statinfo += divdata
                        statinfo = statinfo.rstrip()
                        statinfo += "</pre>"
                        rrdFile = slotObj.getName() + "-" +proj.getName()+ "-" + plat +"-month.png"
                        #rrdPath = os.path.join(webDir, "rrd", rrdFile)
                        #rrdLink = "http://lcgapp.cern.ch/spi/aaLibrarian/nightlies/"+ "rrd/" + rrdFile
                        rrdPath = slotObj.rrdDir()
                        rrdLink = configHistory[dayNo]._generalConfig['rrdurl'] + '/' + rrdFile
                        if os.path.exists(rrdPath):
                            statinfo += '<img src="%s"/>'% (rrdLink)
                        statinfo += '</div>'
                        # keep divs on separate line!
                        statinfo += "\n"
                        statdivs.append(statinfo)
                    testboxHtml += '>'

                    logURL = configHistory[dayNo]._generalConfig.getParsed('logurl',{
                                                               configuration.PATTERN_PLATFORM:plat,
                                                               configuration.PATTERN_WEEKDAY:day[:3],
                                                     })
                    if includePlatformInPrefix == True:
                        logName = Status[0][len(plat)+1:]
                    else:
                        logName = Status[0]
                    htmlTestLogName = logName.replace('-qmtest.log', '-qmtest/index.html')

                    if logName and (os.path.exists(os.path.join(slotObj.wwwDir(plat), htmlTestLogName)) or (os.path.exists(os.path.join(slotObj.wwwDir(plat), logName))) ):
                        if os.path.exists(os.path.join(slotObj.wwwDir(plat), htmlTestLogName)):
                            #testboxHtml += ' <a href="' + logURL + '/' + htmlTestLogName + '">tests'+errStr+'</a> '
                            testboxHtml += ' <a href="' + logURL + '/' + htmlTestLogName + '">tests'+errStr+'</a>&nbsp;&nbsp;<a href="http://cern.ch/lhcb-nightlies/cgi-bin/rss.py?slot=%s&plat=%s&proj=%s&type=test" target="_blank">%s</a> ' % (slotObj.getName(), plat, proj.getName().upper(), rssHtml)
                            xmlData.modifySpecificNode(xmlPlatformNode,{"testLogfile":strStrip(logURL + '/' + htmlTestLogName),"testErrors":strStrip(Status[2])})
                        else:
                            #testboxHtml += ' <a href="' + logURL + '/' + logName + '">tests'+errStr+'</a> '
                            testboxHtml += ' <a href="' + logURL + '/' + logName + '">tests'+errStr+'</a>&nbsp;&nbsp;<a href="http://cern.ch/lhcb-nightlies/cgi-bin/rss.py?slot=%s&plat=%s&proj=%s&type=test" target="_blank">%s</a> ' % (slotObj.getName(), plat, proj.getName().upper(), rssHtml)
                            xmlData.modifySpecificNode(xmlPlatformNode,{"testLogfile":strStrip( logURL + '/' + logName),"testErrors":strStrip(Status[2])})
                        testboxHtml += ' </td> \n'
                        htmlFile.write(colour[flag])
                        htmlFile.write(testboxHtml)
                    else:
                        if string.upper(str(generalConf.get('shownotfinishedplatforms', False))) == 'FALSE':
                            testboxHtml += ' <a href="javascript:return true">---</a> '
                            flag="warn"
                            testboxHtml += ' </td> \n'
                            if projDisabled:
                                htmlFile.write('#D3D3D3')
                            else:
                                htmlFile.write(colour[flag])
                        else:
                            # this a_href and javascript must be here!!!!!!!! it is used for filtering html by nightlies.py script.....
                            if projDisabled:
                                testboxHtml += ' <a href="javascript:return true" style="color: #D3D3D3">---</a> '
                            else:
                                testboxHtml += ' <a href="javascript:return true" style="color: white">---</a> '
                            testboxHtml += ' </td> \n'
                            flag="warn"
                            if projDisabled:
                                htmlFile.write('#D3D3D3')
                            else:
                                htmlFile.write('white')
                        htmlFile.write(testboxHtml)



#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

                    #htmlFile.write( '<td align="center" nowrap bgcolor="'+colour[flag]+'" width="50%"')
#                    htmlFile.write( '>tests</td>')

                    htmlFile.write( '</tr>\n</table>\n</td>\n')
                htmlFile.write( '    </tr>')
                htmlFile.write( '\n<!-- end PROJECT tr for ' +day+'_'+slotObj.getName()+'_'+proj.getName()+ ' -->\n')

#            platformsNotReadyHtml = ''
#            for x in platformsNotReady:
#                if x[0]==day and x[1]==slotObj.getName(): platformsNotReadyHtml += '<em>' + x[2] + '</em>, '
#            if len(platformsNotReadyHtml) > 2:
#                platformsNotReadyHtml = platformsNotReadyHtml[:-2]
#                htmlFile.write( '<tr><td align="right" bgcolor="white" colspan="')
#                htmlFile.write( '100')
#                # str(2+len(slotObj.getPlatforms())-len(platformsNotReady))
#                htmlFile.write( '"><strong>Platforms not ready yet:</strong> ')
#                htmlFile.write(platformsNotReadyHtml)
#                htmlFile.write( '</td></tr>')

            htmlFile.write( '      </tbody>\n')
            htmlFile.write( '    </table>\n')

            platformsNotReadyHtml = ''
            if waitForIs:
              platformsNotReadyHtml += '<font style="background-color: #FF8040; font-weight:600"> wait for LCG-nightlies</font>: '
            for x in platformsNotReady:
              if waitForIs:
                if os.path.exists(waitForPlat):
                  if x[0]==day and x[1]==slotObj.getName(): platformsNotReadyHtml += '<font style="background-color: #80FF00">' + x[2] + '</font>, '
                else:
                  if x[0]==day and x[1]==slotObj.getName(): platformsNotReadyHtml += '<font style="background-color: #FF8040">' + x[2] + '</font>, '
              else:
                if x[0]==day and x[1]==slotObj.getName(): platformsNotReadyHtml += '' + x[2] + ', '
            if len(platformsNotReadyHtml) > 2:
                platformsNotReadyHtml = platformsNotReadyHtml[:-2]
                htmlFile.write( '<small><strong>Platforms not ready:</strong> ')
                htmlFile.write(platformsNotReadyHtml)
                htmlFile.write('</small>')

            htmlFile.write( '</div>')
            htmlFile.write( "\n<!-- end SLOT div for " + day + '_' + slot + " -->\n")

        htmlFile.write( '</div>')
        htmlFile.write( "\n<!-- end DAY div for " + day + " -->\n")

    htmlFile.write( "\n<!-- start STAT divs -->\n")
    for divdata in statdivs:
        htmlFile.write(str(divdata))
    htmlFile.write( "\n<!-- end STAT divs -->\n")


    htmlFile.write( '<table width="80%" border="0">'+" \n")
    htmlFile.write( '  <tr>'+" \n")
    htmlFile.write( '    <td align="left"><span style="font-family:Arial; color=#000000; font-weight: bold; font-size: 13pt" >System health messages</span></td>'+" \n")

    #checkPathHealth(systemHealthPathes)

    for problemClass in systemHealth:
        htmlFile.write( '  <tr>'+" \n")
        if problemClass is 'Warning':
            bgColor = "warn"
        elif problemClass is 'Error':
            bgColor = "err"
        else:
            bgColor = "wheat"
        htmlFile.write( '    <td align="center" bgcolor="'+colour[bgColor]+'"><span style="font-family:Arial; color=#000000; font-weight: bold; font-size: 12pt">'+str(problemClass)+'</span></td>')
        htmlFile.write( '  </tr>'+" \n")
        for problemKey in systemHealth[problemClass]:
            if len(systemHealth[problemClass][problemKey]['Data'])>0:
                htmlFile.write( '  <tr>'+" \n")
                htmlFile.write( '    <td align="center" bgcolor="'+colour[bgColor]+'"><span style="font-family:Arial; color=#000000; font-weight: bold; font-size: 11pt">'+str(problemKey)+'</span></td>')
                htmlFile.write( '  </tr>'+" \n")
                htmlFile.write( '  <tr>'+" \n")
                for problemHeader in systemHealth[problemClass][problemKey]['Header']:
                    htmlFile.write( '        <td bgcolor="'+colour[bgColor]+'">'+problemHeader+'</td>')
                htmlFile.write( '</tr>'+" \n")
                for problemData in systemHealth[problemClass][problemKey]['Data']:
                    htmlFile.write( '  <tr>'+" \n")
                    for problemElement in problemData:
                        htmlFile.write( '    <td bgcolor="'+colour[bgColor]+'">'+str(problemElement)+'</td>'+" \n")
                    htmlFile.write( '  </tr>'+" \n")
                htmlFile.write( '  </tr>'+" \n")

    htmlFile.write( '</table>'+" \n")

    htmlFile.write( '<h5>Time for processing the input: %s seconds</h5>' % (time.clock() - startTime))
    xmlData.modifySpecificNode(xmlMetaTimeNode,{"processingSecs":strStrip(time.clock() - startTime)})

    trailer = """
        <address><a href="mailto:Stefan.Roiser@cern.ch">Stefan ROISER</a></address> <address><a href="mailto:thomas.hartmann@cern.ch">Thomas Hartmann</a></address> <address><a href="mailto:piotr.kolet@cern.ch">Piotr Kolet</a></address>
      </body>
    </html>
    """
    #trailer = """
    #    <address><a href="mailto:Stefan.Roiser@cern.ch">Stefan ROISER</a></address> <address><a href="mailto:Karol.Kruzelecki@cern.ch">Karol Kruzelecki</a></address> <address><a href="mailto:piotr.kolet@cern.ch">Piotr Kolet</a></address>
    #  </body>
    #</html>
    #"""

    htmlFile.write( trailer )

def getConfigurationContents(svnDate=None):
    #svnPath = 'http://svnweb.cern.ch/guest/lhcb/LHCbNightlyConf/trunk/configuration.xml'
    #svnPathTest = 'http://svnweb.cern.ch/guest/lhcb/LHCbNightlyConf/trunk/configuration-test.xml'
    svnPath = 'https://svn.cern.ch/reps/lhcb/LHCbNightlyConf/trunk/configuration.xml'
    svnPathTest = 'https://svn.cern.ch/reps/lhcb/LHCbNightlyConf/trunk/configuration-test.xml'
    svnPathTest = 'https://svn.cern.ch/reps/lhcb/LHCbNightlyConf/trunk/configuration.xml'
    from subprocess import Popen, PIPE
    if svnDate is None: # test configuration from now
        confContents = Popen(["svn", "cat", svnPathTest], stdout=PIPE).communicate()[0]
    else: # last from today
        confContents = Popen(["svn", "cat", "-r", "{%s}" % svnDate, svnPath], stdout=PIPE).communicate()[0]
#for tests:
#        confContents = Popen(["svn", "cat", svnPath], stdout=PIPE).communicate()[0]
    return confContents

#def getArchivedConfig(day): #For example: "Monday" --> config.xml from last Monday
#    today = time.strftime("%A")
#    todayIndex = day_names.index(today)
#    weekDate = {}
#    for x in xrange(todayIndex,7):
#        weekDate[day_names[x]] = ''
#    for x in xrange(0,todayIndex):
#        weekDate[day_names[x]] = ''


def generateIndex(mainConfigFileName, resultFileName):
    global configuration
    confContents = None
    #tmpFileName = resultFileName + '.tmp.html'
    tmpFileName = resultFileName + "_" + time.strftime("%a%H%M") + '.tmp.html'
    tmpFileNameLink = resultFileName + '.tmp.html'

    xmlFileName = resultFileName.rstrip('.html')+'.xml'
    #xmlTmpFileName = xmlFileName  + '.tmp.xml'
    xmlTmpFileName = xmlFileName + "_" + time.strftime("%a%H%M") + '.tmp.xml'
    xmlTmpFileNameLink = xmlFileName + '.tmp.html'

    if os.path.lexists(tmpFileNameLink): os.remove(tmpFileNameLink)
    os.symlink(tmpFileName,tmpFileNameLink)
    if os.path.lexists(xmlTmpFileNameLink): os.remove(xmlTmpFileNameLink)
    os.symlink(xmlTmpFileName,xmlTmpFileNameLink)

    indexFile = open(tmpFileName, 'w')
    xmlData = messageXML('LHCbNightlies')

    generalConf, slotList = configuration.readConf(mainConfigFileName, confContents)
    showAll(indexFile, xmlData, generalConf, slotList, historyFromSvn = True)
    indexFile.close()
    xmlData.writeDoc(xmlTmpFileName)
    try:
        os.rename(tmpFileName, resultFileName)
        if os.path.lexists(tmpFileNameLink): os.remove(tmpFileNameLink)
        os.rename(xmlTmpFileName, xmlFileName)
        if os.path.lexists(xmlTmpFileNameLink): os.remove(xmlTmpFileNameLink)
        #os.rename(tmpFileName, resultFileName)
        #os.rename(xmlTmpFileName, xmlFileName)
    except:
        print "ERROR moving index file."
        raise
    #if prevLXG_XMLCONFIGDIR: os.environ['LCG_XMLCONFIGDIR'] = prevLXG_XMLCONFIGDIR


def generateIndexSVN(resultFileName, svnNow=False):
    import datetime
    global configHistory
    for x in xrange(7): # 0..6
        day = datetime.date.today() - datetime.timedelta(days=x)
        os.environ['NightliesSummaryHistoryDate'] = day.isoformat()
        from BaseConfiguration import Configuration as tmpConfMod
        configHistory.append(tmpConfMod())
        if not svnNow:
            configHistory[x].readConf(configFile=None, configContents=getConfigurationContents(day.strftime('%Y-%m-%d')))
        else:
            configHistory[x].readConf(configFile=None, configContents=getConfigurationContents(None))

    #xmlFileName = resultFileName.rstrip('.html')+'.xml'
    #xmlTmpFileName = xmlFileName  + '.tmp.xml'
    #tmpFileName = resultFileName + '.tmp.html'

    #tmpFileName = resultFileName + '.tmp.html'
    tmpFileName = resultFileName + "_" + time.strftime("%a%H%M") + '.tmp.html'
    tmpFileNameLink = resultFileName + '.tmp.html'

    xmlFileName = resultFileName.rstrip('.html')+'.xml'
    #xmlTmpFileName = xmlFileName  + '.tmp.xml'
    xmlTmpFileName = xmlFileName + "_" + time.strftime("%a%H%M") + '.tmp.xml'
    xmlTmpFileNameLink = xmlFileName + '.tmp.html'

    if os.path.lexists(tmpFileNameLink): os.remove(tmpFileNameLink)
    os.symlink(tmpFileName,tmpFileNameLink)
    if os.path.lexists(xmlTmpFileNameLink): os.remove(xmlTmpFileNameLink)
    os.symlink(xmlTmpFileName,xmlTmpFileNameLink)

    indexFile = open(tmpFileName, 'w')
    xmlData = messageXML()

    showAll(indexFile, xmlData, configHistory[0]._generalConfig, configHistory[0]._slotList, historyFromSvn = True)
    indexFile.close()
    xmlData.writeDoc(xmlTmpFileName)
    try:
        os.rename(tmpFileName, resultFileName)
        if os.path.lexists(tmpFileNameLink): os.remove(tmpFileNameLink)
        os.rename(xmlTmpFileName, xmlFileName)
        if os.path.lexists(xmlTmpFileNameLink): os.remove(xmlTmpFileNameLink)
        #os.rename(tmpFileName, resultFileName)
        #os.rename(xmlTmpFileName, xmlFileName)
    except:
        print "ERROR moving index file."
        raise



if __name__ == "__main__":
    parser = OptionParser()
    parser.set_usage("usage: %prog XML_FILE OUTPUT_HTML_FILE\n       %prog OUTPUT_HTML_FILE")
    parser.set_description("Creates html file containing summary of nightly builds.")
    (options, args) = parser.parse_args()
    if len(args) == 1: # SVN
        generateIndexSVN(args[0])
    elif len(args) == 2 and args[1] == 'SVN':
        generateIndexSVN(args[0], True)
    elif len(args) == 2:
        generateIndex(args[0], args[1])
    else:
        parser.print_help()
        sys.exit()


