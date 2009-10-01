// Extended Tooltip Javascript
// copyright 9th August 2002, 3rd July 2005, 24th August 2008
// by Stephen Chapman, Felgall Pty Ltd

var tooltips = null;
var init_tooltips = function(body) {
  var tip = document.createElement('div');

  tooltips = tip;
  body.tips = tip;
  body.appendChild(tip);
  tip.className = 'tooltip';
  
  tip.pw = function() { return window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth; }
  tip.mouseX = function(evt) { return evt.clientX ? evt.clientX + (document.documentElement.scrollLeft || document.body.scrollLeft) : evt.pageX; }
  tip.mouseY = function(evt) { return evt.clientY ? evt.clientY + (document.documentElement.scrollTop || document.body.scrollTop) : evt.pageY; }
  tip.hide   = function() {    this.style.visibility = "hidden";   }
  tip.hide.client = tip;
  tip.show   = function(evt,text) {
    var wp = this.pw();
    var dm = this;
    var ds = dm.style; 
    var st = ds.visibility; 
    var ew;
    dm.innerHTML = text;
    if (dm.offsetWidth) ew = dm.offsetWidth; 
    else if (dm.clip.width) ew = dm.clip.width; 
    var tv = this.mouseY(evt) + 20; 
    var lv = this.mouseX(evt) - (ew/4); 
    if (lv < 2) lv = 2; 
    else if (lv + ew > wp) lv -= ew/2; 
    lv += 'px';
    tv += 'px';  
    ds.left = lv;
    ds.top = tv;
    ds.visibility = "visible";
  }
  tip.set = function(obj,text) {
    //alert('set tooltip:'+text);
    obj.onmouseout  = function() { tooltips.hide(); }
    obj.onmouseover = function() { tooltips.show(event,this.tip); }
    obj.tip = text;
  }
  return tip;
}
