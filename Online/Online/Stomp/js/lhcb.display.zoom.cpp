// --------------------------------------------------------------------
// Javascript Magnifier v 0.97
// Written by Dino Termini - termini@email.it - May 9, 2003
// This script is freeware (GPL) but if you use it, please let me know!
//
// Portions of code by zoomIN, zoomOUT
// Author: Nguyen Duong Minh (Obie) - obie4web@yahoo.com
// WWW: http://ObieWebsite.SourceForge.net
// License: GNU (GPL)
//
// Jupirmedia Corporation
// http://www.internet.com
// 
// Not original source! Hacked by M.Frank CERN/LHCb
// --------------------------------------------------------------------
//


// Configuration parameters
// ------------------------
// Measure unit in pixel (px) or points (pt)
// measureUnit = "pt"
var zoom_measureUnit = "px";

// Minimum size allowed for SIZE attribute (like in <FONT SIZE="1"> )
var zoom_minSize = 1;

// Minimum size allowed for STYLE attribute (like in <FONT STYLE="font-size: 10px"> )
var zoom_minStyleSize = 10;

// Maximum size allowed for SIZE attribute
var zoom_maxSize = 6;

// Maximum size allowed for STYLE attribute
var zoom_maxStyleSize = 30;

// Start size for tags with no SIZE attribute defined
var zoom_startSize = 1;

// Start size for tags with no font-size STYLE or CLASS attribute defined
var zoom_startStyleSize = 10;

// Increasing and decreasing step
var zoom_stepSize = 1;

// Increasing step for STYLE definition (measure previously declared will be used)
var zoom_stepStyleSize = 2;

var zoom_searchTags = function(childTree, level) {
    var retArray = new Array();
    var tmpArray = new Array();
    var j = 0;
    var childName = "";
    //if ( level > 10 ) return retArray;
    //alert('level:'+level);
    for (var i=0; i<childTree.length; i++) {
      childName = childTree[i].nodeName;
      if (childTree[i].hasChildNodes()) {
	if ((childTree[i].childNodes.length == 1) && (childTree[i].childNodes[0].nodeName == "#text"))
	  retArray[j++] = childTree[i];
	else {
	  tmpArray = zoom_searchTags(childTree[i].childNodes, level+1);
	  for (var k=0;k<tmpArray.length; k++)
	    retArray[j++] = tmpArray[k];
	  retArray[j++] = childTree[i];
	}
      }
      else
	retArray[j++] = childTree[i];
    }
    return(retArray);
};

var zoom_changeFontSize = function(stepSiz, stepStyleSiz) {
    if (document.body) {
      var myObj = zoom_searchTags(document.body.childNodes, 0);
      var myStepSize = stepSiz;
      var myStepStyleSize = stepStyleSiz;
      var myObjNumChilds = myObj.length;
      //var styleSize = zoom_startStyleSize;

      for (var i=0; i<myObjNumChilds; i++) {
	var myObjName = myObj[i].nodeName;

	// Only some tags will be parsed
	if (myObjName != "#text" && myObjName != "HTML" &&
	    myObjName != "HEAD" && myObjName != "TITLE" &&
	    myObjName != "STYLE" && myObjName != "SCRIPT" &&
	    myObjName != "BR" && myObjName != "TBODY" &&
	    myObjName != "#comment" && myObjName != "FORM") {
	  var size = parseInt(myObj[i].getAttribute("size"));
	  // Internet Explorer uses a different DOM implementation
	  if (myObj[i].currentStyle)
	    styleSize = parseInt(myObj[i].currentStyle.fontSize);
	  else
	    styleSize = parseInt(window.getComputedStyle(myObj[i], null).fontSize);
	  if (isNaN(size)) {
	    //alert('Size:'+size);
	    size = zoom_startSize;
	  }
	  if (isNaN(styleSize)) {
	    //alert('StyleSize:'+styleSize);
	    styleSize = zoom_startStyleSize;
	  }
	  if ( ((size  > zoom_minSize) && (size < zoom_maxSize)) || 
	       ((size == zoom_minSize) && (stepSiz > 0)) || 
	       ((size == zoom_maxSize) && (stepSiz < 0)) ) {
	    myObj[i].size = size+myStepSize;
	  }
	  if ( ((styleSize  > zoom_minStyleSize) && (styleSize < zoom_maxStyleSize)) || 
	       ((styleSize == zoom_minStyleSize) && (stepStyleSiz > 0)) ||
	       ((styleSize == zoom_maxStyleSize) && (stepStyleSiz < 0)) ) {
	    var newStyleSize = styleSize+myStepStyleSize;
	    myObj[i].style.fontSize = newStyleSize+zoom_measureUnit;
	  }
	} // End if condition ("only some tags")
      } // End main for cycle
    } // End if condition ("document.body exists")
}; // End function declaration

var zoom_changeFontSizeEx = function(num) {
    var n = parseInt(num);
    if ( n != null && !isNaN(n) ) {
      var s1 = zoom_stepSize;
      var s2 = zoom_stepStyleSize;
      zoom_changeFontSize(n*s1, n*s2);
    }
};

var zoom_increaseFontSize = function() {
  zoom_changeFontSize( zoom_stepSize,  zoom_stepStyleSize); 
};

var zoom_decreaseFontSize = function() {
  zoom_changeFontSize(-zoom_stepSize, -zoom_stepStyleSize);
};
