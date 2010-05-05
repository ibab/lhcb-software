//
//
// @author M.Frank
//
function current_time() {
  if ( navigator.appName.indexOf("Netscape") != -1 )  {
    return Date().toString().substr(4,20);
  }
  return Date().toString().substr(4,15);
};

function setNavigationLocation(loc) {
  document.location = loc;
};

function make_header() {
  document.write(
    '<TABLE bgColor="#9dff9d" border="1" width="550">\n<TR>\n<TD>\n'+
      '<DIV class="TOPNAVIGATION">\n'+
      '<TABLE border="0" cellPadding="2" cellSpacing="10" width="100%">\n'+
        '<TR>\n'+
          '<TH><B><A href="http://cern.ch/lhcb">LHCb</A> </B></TH>\n'+
          '<TH><B><A href="http://cern.ch/lhcb-comp">Computing</A> </B></TH>\n'+
          '<TH><B><A href="http://lhcb-online.web.cern.ch/lhcb-online">Online</A> </B></TH>\n'+
          '<TH id="header_time_stamp"></TH>\n'+
        '</TR>\n'+
      '</TABLE>\n'+
      '</DIV>\n'+
    '</TD>\n</TR>\n'+
    '</TABLE>\n');
};

function make_footer() {
  var e = document.getElementById('header_time_stamp');
  if ( e ) {
    var handler = function() {
      document.getElementById('header_time_stamp').innerHTML = '['+Date().toString()+']';
    };
    handler();
    setInterval(handler,2000);
  }
  document.write(
    '<DIV class=BOTTOMNAVIGATION><TABLE align=bottom><TR><TD><BR><BR>'+
      'In the event of suggestions or problems send mail to '+
      'M.Frank CERN/LHCb</A> '+
    '</TD></TR></TABLE></DIV>'
    );
};

//alert('Script lhcb.display.navbar.cpp loaded successfully');
