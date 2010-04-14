/*!
 * Ext JS Library 3.1.1
 * Copyright(c) 2006-2010 Ext JS, LLC
 * licensing@extjs.com
 * http://www.extjs.com/license
 */
Ext.app.Module = function(config){
    Ext.apply(this, config);
    Ext.app.Module.superclass.constructor.call(this);
    this.init();
}

Ext.extend(Ext.app.Module, Ext.util.Observable, 	{ init : Ext.emptyFn });

Ext.app.TreePanel = function(config)  {
  Ext.apply(this, config);
  Ext.app.TreePanel.superclass.constructor.call(this);
  this.init();
}

Ext.extend(Ext.app.TreePanel, Ext.tree.TreePanel, 	{ init : Ext.emptyFn });

Ext.app.FormPanel = function(config)  {
  Ext.apply(this, config);
  Ext.app.FormPanel.superclass.constructor.call(this);
  this.init();
}

Ext.extend(Ext.app.FormPanel, Ext.FormPanel, 		{ init : Ext.emptyFn });

Ext.app.Panel = function(config)  {
  //t.app.__dump(config);
  Ext.apply(this, config);
  Ext.app.Panel.superclass.constructor.call(this);
  this.init();
}

Ext.extend(Ext.app.Panel, Ext.Panel, 			{ init : Ext.emptyFn });

