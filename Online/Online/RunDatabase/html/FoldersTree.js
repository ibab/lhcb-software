//**************************************************************** 
// You are free to copy the "Folder-Tree" script as long as you  
// keep this copyright notice: 
// Script found in: http://www.geocities.com/Paris/LeftBank/2178/ 
// Author: Marcelino Alves Martins (martins@hks.com) December '97. 
//**************************************************************** 
 
//Log of changes: 
//       17 Feb 98 - Fix initialization flashing problem with Netscape
//       
//       27 Jan 98 - Root folder starts open; support for USETEXTLINKS; 
//                   make the ftien4 a js file 
//       
 
// Constants for image filenames
var base_dir = "/images/Tree/";
var edge = base_dir+"ftv2lastnode.gif";
var vert = base_dir+"ftv2vertLine.gif";
var node = base_dir+"ftv2node.gif";
var plus = base_dir+"ftv2pnode.gif";
var pluslast = base_dir+"ftv2plastnode.gif";
var minus = base_dir+"ftv2mnode.gif";
var minuslast = base_dir+"ftv2mlastnode.gif";
var blank = base_dir+"ftv2blank.gif";

var folderOpen = base_dir+"ftv2FolderOpen.gif";
var folderClosed = base_dir+"ftv2FolderClosed.gif";
var item = base_dir+"ftv2doc02.gif";

var browserGapDefault = 4;
var browserGapFolder = 4;

var targetFrame = "display";
var targetF = "";


// Global variables 
// **************** 
 
USETEXTLINKS = 1 
indexOfEntries = new Array 
nEntries = 0 
doc = document 
browserVersion = 0 
selectedFolder=0
targetFrame = "display"
targetF = "display"


// Definition of class Folder 
// ***************************************************************** 
 
///  constructor
function Folder(folderDescription, hreference) {    
  //constant data 
  this.desc = folderDescription 
  this.hreference = hreference 
  this.id = -1   
  this.navObj = 0  
  this.iconImg = 0  
  this.nodeImg = 0  
  this.isLastNode = 0 
 
  //dynamic data 
  this.isOpen = true 
  this.iconSrc = folderOpen   
  this.children = new Array 
  this.nChildren = 0 
 
  //methods 
  this.initialize = initializeFolder 
  this.setState = setStateFolder 
  this.addChild = addChild 
  this.createIndex = createEntryIndex 
  this.hide = hideFolder 
  this.display = display 
  this.renderOb = drawFolder 
  this.totalHeight = totalHeight 
  this.subEntries = folderSubEntries 
  this.outputLink = outputFolderLink 
} 
 
function setStateFolder(isOpen)   { 
  var subEntries 
  var totalHeight 
  var fIt = 0 
  var i=0 
 
  if (isOpen == this.isOpen) 
    return 
 
  if (browserVersion == 2)    { 
    totalHeight = 0 
    for (i=0; i <this.nChildren; i++) 
      totalHeight = totalHeight + (this.children[i].navObj.clip.height-browserGapDefault) 
      subEntries = this.subEntries() 
    if (this.isOpen) 
      totalHeight = 0 - totalHeight 
    for (fIt = this.id + subEntries + 1; fIt <nEntries; fIt++) 
      indexOfEntries[fIt].navObj.moveBy(0, totalHeight) 
  }  
  this.isOpen = isOpen 
  propagateChangesInState(this) 
} 
 
function propagateChangesInState(folder)  {   
  var i=0  
  if (folder.isOpen)   { 
    if (folder.nodeImg) 
      if (folder.isLastNode) 
        folder.nodeImg.src = minuslast 
      else 
	  folder.nodeImg.src = minus 
    folder.iconImg.src = folderOpen 
    for (i=0; i<folder.nChildren; i++) 
      folder.children[i].display() 
  } 
  else   { 
    if (folder.nodeImg) 
      if (folder.isLastNode) 
        folder.nodeImg.src = pluslast 
      else 
	  folder.nodeImg.src = plus 
    folder.iconImg.src = folderClosed 
    for (i=0; i<folder.nChildren; i++) 
      folder.children[i].hide() 
  }  
} 
 
function hideFolder()   { 
  if (browserVersion == 1) { 
    if (this.navObj.style.display == "none") 
      return 
    this.navObj.style.display = "none" 
  } 
  else { 
    if (this.navObj.visibility == "hiden") 
      return 
    this.navObj.visibility = "hiden" 
  } 
   
  this.setState(0) 
} 
 
function initializeFolder(level, lastNode, leftSide) 
{ 
var j=0 
var i=0 
var numberOfFolders 
var numberOfDocs 
var nc 
      
  nc = this.nChildren    
  this.createIndex() 
  var auxEv = "" 
  if (browserVersion > 0) 
    auxEv = "<a onmousemove='javascript:window.status=\"Expand\";' href='javascript:clickOnNode("+this.id+")'>" 
  else 
    auxEv = "<a onmousemove='javascript:window.status=\"Expand\";' >" 
 
  if (level>0)  {
    if (lastNode)     { //the last 'brother' in the children array 
      this.renderOb(leftSide + auxEv + "<img name='nodeIcon" + this.id + "' src='" + minuslast + "' width=16 height=22 border=0></a>") 
      leftSide = leftSide + "<img src='" + blank + "' width=16 height=22>"  
      this.isLastNode = 1 
    } 
    else     { 
      this.renderOb(leftSide + auxEv + "<img name='nodeIcon" + this.id + "' src='" + minus + "' width=16 height=22 border=0></a>") 
      leftSide = leftSide + "<img src='" + vert + "' width=16 height=22>" 
      this.isLastNode = 0 
    } 
  }
  else    {
    this.renderOb("") 
  }
   
  if (nc > 0)   { 
    level = level + 1 
    for (i=0 ; i <this.nChildren; i++) { 
      if (i == this.nChildren-1) 
        this.children[i].initialize(level, 1, leftSide) 
      else 
        this.children[i].initialize(level, 0, leftSide) 
    } 
  } 
} 
 
function drawFolder(leftSide)   { 
  if (browserVersion == 2) { 
    if (!doc.yPos) 
      doc.yPos=8 
    doc.write("<layer id='folder" + this.id + "' top=" + doc.yPos + " visibility=hiden>") 
  } 
   
  doc.write("<table ") 
  if (browserVersion == 1) 
    doc.write(" id='folder" + this.id + "' style='position:block;' ") 
  doc.write(" border=0 cellspacing=0 cellpadding=0 spacing=0 padding=0>") 
  doc.write("<tr><td>") 
  doc.write(leftSide) 
  this.outputLink() 
  doc.write("<img name='folderIcon" + this.id + "' ") 
  doc.write("src='" + this.iconSrc+"' border=0 height=22></a>") 
  doc.write("</td>\n<td valign=middle nowrap>") 
  if (USETEXTLINKS)   { 
    this.outputLink() 
    doc.write(this.desc + "</a>") 
  } 
  else 
    doc.write(this.desc) 
  doc.write("</td>\n")  
  doc.write("</table>\n") 
   
  if (browserVersion == 2) { 
    doc.write("</layer>\n") 
  } 
 
  if (browserVersion == 1) { 
    this.navObj = doc.all["folder"+this.id] 
    this.iconImg = doc.all["folderIcon"+this.id] 
    this.nodeImg = doc.all["nodeIcon"+this.id] 
  }
  else if (browserVersion == 2) { 
    this.navObj = doc.layers["folder"+this.id] 
    this.iconImg = this.navObj.document.images["folderIcon"+this.id] 
    this.nodeImg = this.navObj.document.images["nodeIcon"+this.id] 
    doc.yPos=doc.yPos+(this.navObj.clip.height-browserGapFolder) 
  } 
} 
 
function outputFolderLink()   { 
  if (this.hreference)   { 
    doc.write("<a href='" + this.hreference + "' TARGET=\"" + targetFrame + "\" ") 
    if (browserVersion > 0) 
      doc.write("onmousemove='javascript:window.status=\"" + removeHTMLTags(this.desc) + "\";' onClick='javascript:clickOnFolder("+this.id+")'") 
    doc.write(">") 
  } 
  else 
    doc.write("<a>") 
//  doc.write("<a onmousemove='javascript:window.status=\"" + removeHTMLTags(this.desc) + "\";' href='javascript:clickOnFolder("+this.id+")'>")   
} 
 
function addChild(childNode)  { 
  this.children[this.nChildren] = childNode 
  this.nChildren++ 
  return childNode 
} 
 
function folderSubEntries()  { 
  var i = 0 
  var se = this.nChildren 
 
  for (i=0; i <this.nChildren; i++){ 
    if (this.children[i].children) //is a folder 
      se = se + this.children[i].subEntries() 
  } 
 
  return se 
} 
 
 
// Definition of class Item (a document or link inside a Folder) 
// ************************************************************* 
 
function Item(itemDescription, itemLink) // Constructor 
{ 
  // constant data 
  this.desc = itemDescription 
  this.link = itemLink 
  this.id = -1 //initialized in initalize() 
  this.navObj = 0 //initialized in render() 
  this.iconImg = 0 //initialized in render() 
  this.iconSrc = item 
 
  // methods 
  this.initialize = initializeItem 
  this.createIndex = createEntryIndex 
  this.hide = hideItem 
  this.display = display 
  this.renderOb = drawItem 
  this.totalHeight = totalHeight 
} 
 
function hideItem()   { 
  if (browserVersion == 1) { 
    if (this.navObj.style.display == "none") 
      return 
    this.navObj.style.display = "none" 
  }
  else { 
    if (this.navObj.visibility == "hiden") 
      return 
    this.navObj.visibility = "hiden" 
  }     
} 
 
function initializeItem(level, lastNode, leftSide)  {  
  this.createIndex() 
 
  if (level>0)  {
    if (lastNode)   { //the last 'brother' in the children array 
      this.renderOb(leftSide + "<img src='" + edge + "' width=16 height=22>") 
      leftSide = leftSide + "<img src='" + blank +"' width=16 height=22>"  
    } 
    else     { 
      this.renderOb(leftSide + "<img src='" + node + "' width=16 height=22>") 
      leftSide = leftSide + "<img src='" + vert + "' width=16 height=22>" 
    } 
  }
  else  {
    this.renderOb("")   
  }
} 
 
function drawItem(leftSide)   { 
  if (browserVersion == 2) 
    doc.write("<layer id='item" + this.id + "' top=" + doc.yPos + " visibility=hiden>") 
     
  doc.write("<table ") 
  if (browserVersion == 1) 
    doc.write(" id='item" + this.id + "' style='position:block;' ") 
  doc.write(" border=0 spacing=0 padding=0 cellspacing=0 cellpadding=0>") 
  doc.write("<tr><td>") 
  doc.write(leftSide) 
  doc.write("<a onmousemove='javascript:window.status=\"" + removeHTMLTags(this.desc) + "\";' href=" + this.link + ">") 
  doc.write("<img id='itemIcon"+this.id+"' ") 
  doc.write("src='"+this.iconSrc+"' border=0>") 
  doc.write("</a>") 
  doc.write("</td><td valign=middle nowrap>") 
  if (USETEXTLINKS) 
    doc.write("<a onmousemove='javascript:window.status=\"" + removeHTMLTags(this.desc) + "\";' href=" + this.link + ">" + this.desc + "</a>") 
  else 
    doc.write(this.desc) 
  doc.write("</table>") 
   
  if (browserVersion == 2) 
    doc.write("</layer>") 
 
  if (browserVersion == 1) { 
    this.navObj = doc.all["item"+this.id] 
    this.iconImg = doc.all["itemIcon"+this.id] 
  } else if (browserVersion == 2) { 
    this.navObj = doc.layers["item"+this.id];
    this.iconImg = this.navObj.document.images["itemIcon"+this.id];
    doc.yPos=doc.yPos+(this.navObj.clip.height-browserGapDefault); 
  } 
} 
 
 
// Methods common to both objects (pseudo-inheritance) 
// ******************************************************** 
 
function display() { 
  if (browserVersion == 1) 
    this.navObj.style.display = "block" 
  else 
    this.navObj.visibility = "show" 
} 
 
function createEntryIndex()   { 
  this.id = nEntries 
  indexOfEntries[nEntries] = this 
  nEntries++ 
} 
 
// total height of subEntries open 
function totalHeight() //used with browserVersion == 2 
{ 
  var h = this.navObj.clip.height 
  var i = 0 
   
  if (this.isOpen) //is a folder and _is_ open 
    for (i=0 ; i <this.nChildren; i++)  
      h = h + this.children[i].totalHeight() 
 
  return h 
} 
 
 
// Events 
// ********************************************************* 
 
function clickOnFolder(folderId)  { 
  var clicked = indexOfEntries[folderId] 
  if (!clicked.isOpen) 
    clickOnNode(folderId) 
  
  window.status = "Folder"; 
  return  
 
  if (clicked.isSelected) 
    return 
} 
 
function clickOnNode(folderId)  { 
  var clickedFolder = 0 
  var state = 0 
 
  clickedFolder = indexOfEntries[folderId] 
  state = clickedFolder.isOpen 
 
  clickedFolder.setState(!state) //open<->close  
  window.status = "Node";
} 
 
function initializeDocument(ftree)   { 
  if (doc.all) 
    browserVersion = 1 //IE4   
  else 
    if (doc.layers) 
      browserVersion = 2 //NS4 
    else 
      browserVersion = 0 //other 
 
  ftree.initialize(0, 1, "") 
  ftree.display()
  
  if (browserVersion > 0)     { 
    doc.write("<layer top="+indexOfEntries[nEntries-1].navObj.top+">&nbsp;</layer>") 
 
    // close the whole tree 
    clickOnNode(0) 
    // open the root folder 
    clickOnNode(0) 
  } 
} 
 
// Auxiliary Functions for Folder-Treee backward compatibility 
// ********************************************************* 
 
function gLnk(target, description, linkData, targetF)     { 
  fullLink = "" 
  if (targetF == null)    {
      targetF = targetFrame;
  }
  if (target==0)   { 
    return new Item(description, "'"+linkData+"' target=\"" + targetF + "\"");
  } 
  else  if (target==1)  {
    return new Item(description, "'http://"+linkData+"' target=_blank");
  }
  return new Item(description, "'http://"+linkData+"' target=\"" + targetF + "\"");
} 

function textLnk(dsc, linkData)     { 
  //return new Item("<div class=text><b>"+dsc+"</div></b>", "'"+linkData+"' target=\"display\"");
  return gLnk(0, "<div class=text><b>"+dsc+"</div></b>",linkData, "display");
} 
 
function fldLink(dsc, linkData)     { 
  return new Folder("<div class=text><b>"+dsc+"</div></b>", linkData);
} 
 
function removeHTMLTags(str)  {
  var result;
  var index=0;
  var secure=0;

  result = "";

  if (str.indexOf("<", index)>-1)
    result += str.substring(index, str.indexOf("<", index));
  else
    result += str.substring(index);

  while ( (str.indexOf("<", index) > -1) && (secure<10))  {
    index = str.indexOf(">", index)+1;
    secure++;

    if (str.indexOf("<", index)>-1)
      result += str.substring(index, str.indexOf("<", index));
    else
      result += str.substring(index);
  }
  return result
}

