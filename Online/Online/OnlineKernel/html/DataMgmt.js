
function page_loading()  
{  alert("Please wait until the page is loaded!");                         }
function goto_history_back()
{  history.back();                                                         }
function goto_history_next()
{  history.forward();                                                      }
//
//
// @author M.Frank
//
function RefreshFrame(which)  {
  parent.frames[which].document.location = parent.frames[which].document.URL;
}
//
//
// @author M.Frank
//
function OnRefreshTimeout(which)  {
  if (document.FrameRefresh.autorefresh.value=="Auto Refresh OFF")  {
    defaultStatus = " Auto-refresh ON. Last update: ["+
                    Date().toString().substr(4, 15)+
                    "] "+document.URL;
    RefreshFrame(which);
    setTimeout('OnRefreshTimeout("'+which+'");', 10000);
  }
  else  {
    defaultStatus = " Auto-refresh OFF ["+
                    Date().toString().substr(4, 15)+
                    "] "+document.URL;
  }
}
//
//
// @author M.Frank
//
function OnAutoRefresh(which)  {
  if (document.FrameRefresh.autorefresh.value=="Auto Refresh ON")  {
    document.FrameRefresh.autorefresh.value="Auto Refresh OFF"
    alert('Auto-refresh is now ON.                  \\n'+
          'Do not forget to turn it off again.      \\n'+
          '------------------------------------     \\n'+
          'Be aware, that this causes heavy         \\n'+
          'load on the server!                      '
          );
    OnRefreshTimeout(which)
  }
  else  {
    document.FrameRefresh.autorefresh.value="Auto Refresh ON"
    alert('Auto-refresh is now OFF');
  }
}
//
// Create Standard header
//
// @author M.Frank
//
function make_header() {
  document.write(
    '<TABLE bgColor="#9dff9d" border="1" width="100%">\n<TR>\n<TD>\n'+
      '<DIV class="TOPNAVIGATION">\n'+
      '<TABLE border="0" cellPadding="2" cellSpacing="10" width="100%">\n'+
        '<TR>\n'+
          '<TH><B><A href="JavaScript:show_lhcb_home();">LHCb</A> </B></TH>\n'+
          '<TH><B><A href="JavaScript:show_lhcb_comp();">Computing</A> </B></TH>\n'+
          '<TH><B><A href="JavaScript:show_lhcb_gaudi()">Gaudi</A> </B></TH>\n'+
          '<TH><B><A href="http://documents.cern.ch/AGE/v2_0/displayLevel.php?level=2&amp;fid=1l22">Meetings</A></B></TH>\n'+
          '<TH><B><A href="http://wwwsearch.cern.ch:8765/query.html?col=cern&amp;qp=&amp;qs=&amp;qc=cern&amp;pw=600&amp;ws=0&amp;la=&amp;qm=1&amp;ct=1432940868">Search</A></B></TH>\n'+
          '<TH>['+Date().toString().substr(4, 15)+']</TH>\n'+
        '</TR>\n'+
      '</TABLE>\n'+
      '</DIV>\n'+
    '</TD>\n</TR>\n</TABLE>\n');
}
//
//
// @author M.Frank
//
function make_featured_by(what)  {
  document.write(
    '<TABLE cellspacing="0" cellpadding="10">'+
    '  <TR>'+
        '<TD><IMG border=0 src="/images/lhcblogo.gif"></TD>'+
        '<TD><FONT size="+3">'+what+'</FONT> featured by</TD>'+
        '<TD><IMG border=0 src="/images/Gaudi.gif" height="120"></TD>'+
      '</TR>'+
    '</TABLE>');
}
function make_footer() {
  document.write(
    '<DIV class=BOTTOMNAVIGATION><TABLE align=bottom><TR><TD><BR><BR>'+
      '<P><IMG src="/images/e-mail.gif" height="25" width="72"></IMG>'+
      '<ADDRESS>In the event of suggestions or problems send mail to '+
      '<A href="mailto:markus.frank@cern.ch">Markus Frank</A> '+
      '</ADDRESS>'+
    '</TD></TR></TABLE></DIV>'
    );
}
function make_toolbar(help,home,next) {
  next = (next == null) ? 'history.forward();' : 'setViewerLocation(\''+next+'\');';
  //txt = '<TABLE bordercolorlight="#003366" bgcolor="#AAAAAA" width="100%" border="1"><TR><TD>'+
  txt = '<TABLE width="100%" border="0"><TR><TD>'+
    '<TABLE width="90%"><TR><TD valign="middle" align="center">'+
    '<A HREF="JavaScript:setViewerLocation(\''+home+'\');" onmouseover="status=\'Goto home page\';">'+
    '<IMG ALT="Go to home page" border="0" src="/images/Home_32x32.gif"/>'+
    '</A></TD><TD>'+
    '<A HREF="JavaScript:goto_help(\''+help+'\');" onmouseover="status=\'Show help page\';">'+
    '<IMG ALT="Show help page" border="0" src="/images/Help_32x32.gif"/>'+
    '</A></TD>'+
    '<FORM><TD valign="middle" align="center">'+
    '<INPUT type="BUTTON" onclick="JavaScript:goto_help(\''+help+'\');" onmouseover="status=\'Show help page\';" name="Help" value="Help"/>'+
    '</TD></FORM><TD>'+
    '<A HREF="JavaScript:document.location=document.URL;" onmouseover="status=\'Reload page\';">'+
    '<IMG ALT="Reload page" border="0" src="/images/Refresh_32x32.gif"/>'+
    '</A></TD>'+
    '<TD WIDTH="100">&nbsp;&nbsp;</TD>'+
    '<TD valign="middle" align="center">'+
    '<A HREF="JavaScript:goto_history_back();" onmouseover="status=\'Goto the next page.\';">'+
    '<IMG ALT="Go to previous page of this selection" border="0" src="/images/Previous_32x32.gif"/>'+
    '</A>'+
    '<A HREF="JavaScript:'+next+'" onmouseover="status=\'Goto the next page.\';">'+
    '<IMG ALT="Go to next page of this selection" border="0" src="/images/Next_32x32.gif"/>'+
    '</A></TD></TR></TABLE></TD></TR></TABLE>';
  document.write(txt);
}
function makeHelp(file_name) {
  document.write(
    '<A href="JavaScript:show_bookkeeping_file(\''+file_name+'\');" class="smallblackbullets">\n'+
      '<IMG name="Help" src="Images/Help_16x16.gif" width="16" height="16" border="0" alt="Additional help"/>\n'+
      ' Click here for additional help.</A>'
  );
}
//
//
// @author M.Frank
//
function setViewerLocation(loc) {
  //alert(navigator.appName+'\n\n'+loc);
  doc = document;
  if ( parent.frames["viewer"] != null )  {
    if ( navigator.appName.indexOf("Netscape") != -1 )  {
      doc = parent.frames.viewer.document;
    }
    else if ( navigator.appName.indexOf("Microsoft") != -1 )  {
      doc = parent.viewer;
    }
    else {
      doc = parent.frames.viewer.document;
    }
  }
  doc.location = loc;
}
//
//
// @author M.Frank
//
function setNavigationLocation(loc) {
  //alert(navigator.appName+'\n\n'+loc);
  doc = document;
  if ( parent.frames["navigation"] != null )  {
    if ( navigator.appName.indexOf("Netscape") != -1 )  {
      doc = parent.frames.navigation.document;
    }
    else if ( navigator.appName.indexOf("Microsoft") != -1 )  {
      doc = parent.navigation;
    }
    else {
      doc = parent.frames.navigation.document;
    }
  }
  doc.location = loc;
}
//
//
// @author M.Frank
//
function show_lhcb_home()              {  setViewerLocation('http://lhcb.cern.ch');                         }
function show_lhcb_comp()              {  setViewerLocation('http://lhcb-comp.cern.ch');                    }
function show_lhcb_gaudi()             {  setViewerLocation('http://proj-gaudi.web.cern.ch');               }
function show_page(which)              {  setViewerLocation('http://lhcb-comp.cern.ch/lhcb-comp/ComputingModel/production/'+which);  }
function nav_file(which)               {  setNavigationLocation('http://cern.ch/LHCb.DataMgmt/Bkk/'+which); }
function goto_bkk_home()               {  setViewerLocation('http://lbnts3.cern.ch:8100/Main');             }
function goto_bkk_event_types()        {  setViewerLocation('http://lbnts3.cern.ch:8100/EvtTypes');         }
function goto_bkk_search()             {  goto_bkk_home();                                                  }
function goto_help(url)  
{  window.open(url,"Help", "toolbar=no,width=500,height=500,scrollbars=yes,alwaysRaised=yes",1); }
