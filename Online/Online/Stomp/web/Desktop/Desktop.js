/*!
 * Ext JS Library 3.1.1
 * Copyright(c) 2006-2010 Ext JS, LLC
 * licensing@extjs.com
 * http://www.extjs.com/license
 */
Ext.Desktop = function(app){
    this.taskbar = new Ext.ux.TaskBar(app);
    var taskbar = this.taskbar;
	
    var desktopEl = Ext.get('x-desktop');
    var taskbarEl = Ext.get('ux-taskbar');
    //var shortcuts = Ext.get('x-shortcuts');

    this.shortcuts = new Ext.ux.Shortcuts({
    	renderTo: 'x-desktop',
    	taskbarEl: taskbarEl
    });

    var windows = new Ext.WindowGroup();
    var activeWindow;
		
    function minimizeWin(win){
	win.minimized = true;
	win.hide();
    }

    function markActive(win){
	if(activeWindow && activeWindow != win){
	    markInactive(activeWindow);
	}
	taskbar.setActiveButton(win.taskButton);
	activeWindow = win;
	Ext.fly(win.taskButton.el).addClass('active-win');
	win.minimized = false;
    }

    function markInactive(win){
	if(win == activeWindow){
	    activeWindow = null;
	    Ext.fly(win.taskButton.el).removeClass('active-win');
	}
    }

    function removeWin(win){
    	taskbar.removeTaskButton(win.taskButton);
	layout();
    }

    function layout(){
	desktopEl.setHeight(Ext.lib.Dom.getViewHeight()-taskbarEl.getHeight());
    }
    Ext.EventManager.onWindowResize(layout);

    this.layout = layout;

    this.createWindow = function(config, cls){
    	var win = new (cls||Ext.Window)(
	    Ext.applyIf(config||{}, {
		manager: windows,
		minimizable: true,
		maximizable: true
	    })
	);
	win.render(desktopEl);
	win.taskButton = taskbar.addTaskButton(win);

	win.cmenu = new Ext.menu.Menu({
	    items: [

	    ]
	});

	win.animateTarget = win.taskButton.el;
	
	win.on({
		'activate': {
			fn: markActive
		},
		'beforeshow': {
			fn: markActive
		},
		'deactivate': {
			fn: markInactive
		},
		'minimize': {
			fn: minimizeWin
		},
		'close': {
			fn: removeWin
		}
	});
	
	layout();
	return win;
    };

    this.getManager = function(){
	return windows;
    };

    this.getWindow = function(id){
	return windows.get(id);
    }
    
    this.getViewHeight = function(){
    	return (Ext.lib.Dom.getViewHeight()-taskbarEl.getHeight());
    };
    
    this.getViewWidth = function(){
    	return Ext.lib.Dom.getViewWidth();
    };

    this.getWinWidth = function(){
		var width = Ext.lib.Dom.getViewWidth();
		return width < 200 ? 200 : width;
	}
		
	this.getWinHeight = function(){
		var height = (Ext.lib.Dom.getViewHeight()-taskbarEl.getHeight());
		return height < 100 ? 100 : height;
	}
		
	this.getWinX = function(width){
		return (Ext.lib.Dom.getViewWidth() - width) / 2
	}
		
	this.getWinY = function(height){
		return (Ext.lib.Dom.getViewHeight()-taskbarEl.getHeight() - height) / 2;
	}

	this.setBackgroundColor = function(hex){
		if(hex){
			Ext.get(document.body).setStyle('background-color', '#'+hex);
			app.styles.backgroundcolor = hex;
		}
	};
	
	this.setFontColor = function(hex){
		if(hex){
			Ext.util.CSS.updateRule('.ux-shortcut-btn-text', 'color', '#'+hex);
			app.styles.fontcolor = hex;
		}
	};
	
	this.setTheme = function(o){
		if(o && o.id && o.name && o.pathtofile){
			Ext.util.CSS.swapStyleSheet('theme', o.pathtofile);
			app.styles.theme = o;
		}
	};
	
	this.setTransparency = function(v){
		if(v >= 0 && v <= 100){
			taskbarEl.addClass("transparent");
			Ext.util.CSS.updateRule('.transparent','opacity', v/100);
			Ext.util.CSS.updateRule('.transparent','-moz-opacity', v/100);
			Ext.util.CSS.updateRule('.transparent','filter', 'alpha(opacity='+v+')');
			
			app.styles.transparency = v;
		}
	};
	
	this.setWallpaper = function(o){
 		if(o && o.id && o.name && o.pathtofile){
			var notifyWin = this.showNotification({
				html: 'Loading wallpaper...'
				, title: 'Please wait'
			});
			var wp = new Image();
			wp.src = o.pathtofile;
			
			var task = new Ext.util.DelayedTask(verify, this);
			task.delay(200);			
			app.styles.wallpaper = o;
		}
		
		function verify(){
			if(wp.complete){
				task.cancel();				
				notifyWin.setIconClass('x-icon-done');
				notifyWin.setTitle('Finished');
				notifyWin.setMessage('Wallpaper loaded.');
				this.hideNotification(notifyWin);
				
				document.body.background = wp.src;
			}else{
				task.delay(200);
			}
		}
	};

	this.setWallpaperPosition = function(pos){
		if(pos){
			if(pos === "center"){
				var b = Ext.get(document.body);
				b.removeClass('wallpaper-tile');
				b.addClass('wallpaper-center');
			}else if(pos === "tile"){
				var b = Ext.get(document.body);
				b.removeClass('wallpaper-center');
				b.addClass('wallpaper-tile');
			}			
			app.styles.wallpaperposition = pos;
		}
	};
	
	this.showNotification = function(config){
		var win = new Ext.ux.Notification(Ext.apply({
			animateTarget: taskbarEl
			, autoDestroy: true
			, hideDelay: 5000
			, html: ''
			, iconCls: 'x-icon-waiting'
			, title: ''
		}, config));
		win.show();
		return win;
	};
	
	this.hideNotification = function(win, delay){
		if(win){
			(function(){ win.animHide(); }).defer(delay || 3000);
		}
	};
	
	this.addAutoRun = function(id){
		var m = app.getModule(id),
			c = app.launcher.autorun;
			
		if(m && !m.autorun){
			m.autorun = true;
			c.push(id);
		}
	};
	
	this.removeAutoRun = function(id){
		var m = app.getModule(id),
			c = app.launcher.autorun;
			
		if(m && m.autorun){
			var i = 0;
				
			while(i < c.length){
				if(c[i] == id){
					c.splice(i, 1);
				}else{
					i++;
				}
			}
			
			m.autorun = null;
		}
	};
	
	this.addShortcut = function(id, updateConfig){
		var m = app.getModule(id);
		
		if(m && !m.shortcut){
			var c = m.launcher;
			//Ext.app.__dump(c);
			m.shortcut = this.shortcuts.addShortcut({
				handler: c.handler,
				iconCls: c.shortcutIconCls,
				scope:   c.scope,
				text:    c.text,
				tooltip: c.tooltip || ''
			});
			
			if(updateConfig){
				app.launcher.shortcut.push(id);
			}
		}
		
	};

	this.removeShortcut = function(id, updateConfig){
		var m = app.getModule(id);
		
		if(m && m.shortcut){
			this.shortcuts.removeShortcut(m.shortcut);
			m.shortcut = null;
			
			if(updateConfig){
				var sc = app.launcher.shortcut,
					i = 0;
				while(i < sc.length){
					if(sc[i] == id){
						sc.splice(i, 1);
					}else{
						i++;
					}
				}
			}
		}
	};

	this.addQuickStartButton = function(id, updateConfig){
    	var m = app.getModule(id);
    	
		if(m && !m.quickStartButton){
			var c = m.launcher;
			
			m.quickStartButton = this.taskbar.quickStartPanel.add({
				handler: c.handler,
				iconCls: c.iconCls,
				scope: c.scope,
				text: c.text,
				tooltip: c.tooltip || c.text
			});
			
			if(updateConfig){
				app.launcher.quickstart.push(id);
			}
		}
    };
    
    this.removeQuickStartButton = function(id, updateConfig){
    	var m = app.getModule(id);
    	
		if(m && m.quickStartButton){
			this.taskbar.quickStartPanel.remove(m.quickStartButton);
			m.quickStartButton = null;
			
			if(updateConfig){
				var qs = app.launcher.quickstart,
					i = 0;
				while(i < qs.length){
					if(qs[i] == id){
						qs.splice(i, 1);
					}else{
						i++;
					}
				}
			}
		}
    };

    layout();
};
