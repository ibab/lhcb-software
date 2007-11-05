#uses "ctrlUtils.cpp"
string m_dispDpName = "";

//=============================================================================
void StreamStatusDisplay_install() {
  dyn_dyn_string names;
  dyn_dyn_int types;
  names[1]  = makeDynString ("StreamDisplayData","","","");
  names[2]  = makeDynString ("","Tasks","","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
  ctrlUtils_installDataType(names,types);
  dyn_string names = dpNames(getSystemName()+"*","StreamPartition");
  for (int i=1;i<=dynlen(names);++i) {
    string dp = names[i];
    strreplace(dp,"_Slice","Display_Slice");
    strreplace(dp,getSystemName(),"");
    if ( !dpExists(dp) ) dpCreate(dp,"StreamDisplayData");
  }
  ctrlUtils_installPythonManager(51,"PVSS00DataflowMon","../python/Online/Streaming/DisplayServer.py");
}
//=============================================================================
int StreamStatusDisplay_uninstall()  {
  ctrlUtils_uninstallDataType("StreamDisplayData",true);
}
//=============================================================================
void StreamStatusDisplay_fillSliceNames()  {
  dyn_string names = dpNames("*:*","StreamPartition");
  m_sliceName.deleteAllItems();
  ctrlUtils_setListItems("m_sliceName",names);
  m_sliceName.selectedPos = 1;
}
//=============================================================================
void StreamStatusDisplay_SetCell(int nLine,string col,string bg, string fg) {
  m_table.cellBackColRC(nLine,col,bg);
  m_table.cellForeColRC(nLine,col,fg);
}
//=============================================================================
void StreamStatusDisplay_SetCells(int nLine, dyn_string& cols, string bg, string fg) {
  //StreamStatusDisplay_SetCell(nLine,cols[2],bg,fg);
  StreamStatusDisplay_SetCell(nLine,cols[3],bg,fg);
  StreamStatusDisplay_SetCell(nLine,cols[4],bg,fg);
  //StreamStatusDisplay_SetCell(nLine,cols[5],bg,fg);
}
//=============================================================================      
void StreamStatusDisplay_connectTasksTable(string slice)  {
  string dispDp = slice+".Tasks";
  strreplace(dispDp,"_Slice","Display_Slice");
  DebugN("Connecting to:"+dispDp);
  StreamStatusDisplay_initTasksTable();
  if ( dispDp != m_dispDpName )  {
    if ( strlen(m_dispDpName)>0 )  {
      dpDisconnect("StreamStatusDisplay_CheckDisplay",m_dispDpName);
      DebugN("Disconnected from "+m_dispDpName);
    }
    if ( dpExists(dispDp) )  {
      m_dispDpName = dispDp;
      int res = dpConnect("StreamStatusDisplay_CheckDisplay",dispDp);
      ctrlUtils_checkErrors(res,"Failed to connect to display datapoint:"+dispDp);
    }
    DebugN("Subscriped to "+m_dispDpName);
  }
}
//=============================================================================
void StreamStatusDisplay_startTasksTable()  {
  StreamStatusDisplay_initTasksTable();
  while(1)  {
    StreamStatusDisplay_UpdateDisplay();
    delay(2);
  }
}
//=============================================================================
void StreamStatusDisplay_initTasksTable()  {
  dyn_string cols = makeDynString("Taskset","Name","State","Action","Command");
  m_table.rowHeight = 15;
  m_table.deleteAllLines();
  for( int j=m_table.columnCount()-1; j>=0; j-- )  {
    m_table.deleteColumn(j);
  }
  for(int j=0; j<dynlen(cols); ++j )  {
    m_table.insertColumn(m_table.columnCount());
    m_table.columnHeader(j,cols[j+1]);
    m_table.columnName(j,cols[j+1]);
  }
  for(int j=0,n=dynlen(cols); j<n; ++j)  {
    //m_table.adjustColumn(j);
  }
  m_table.columnWidth(0,120);
  m_table.columnWidth(1,250);
  m_table.columnWidth(2,100);
  m_table.columnWidth(3,90);
  m_table.columnWidth(4,95);
  m_table.gridType = 0;
  //DebugN("Table columns successfully initialized.");
}
//=============================================================================
void StreamStatusDisplay_UpdateDisplay() {
  dyn_string vals;
  string dp = m_sliceName.text+".Tasks";
  strreplace(dp,"_Slice","Display_Slice");
  if ( dpExists(dp) )  {
    m_currSlice.text = substr(m_sliceName.text,strpos(m_sliceName.text,":")+1);
    dpGet(dp,vals);
    StreamStatusDisplay_CheckDisplay(dp,vals);
    return;
  }
  m_currSlice.text = "Non existent slice";
  m_table.deleteAllLines();
  m_table.appendLine("Taskset","This slice does not exist");
  m_table.adjustColumn(0);
}
//=============================================================================
void StreamStatusDisplay_CheckDisplay(string dp, dyn_string values)  {
  int cell_type=0, nLine=0, cnt=m_table.lineCount, print_node=m_incremental.state(0) ? 0 : 1;
  dyn_string items, cols = makeDynString("Taskset","Name","State","Action","Command");
  string state, action, set, command, par_state="", last_set="", taskDp, taskName;
  dyn_int lines, lineType, parents;
  if ( dynlen(values) == 0 ) {
    m_table.deleteAllLines();
    m_table.appendLine(cols[1],"No Tasks found",cols[2],"",cols[3],"",cols[4],"",cols[5],"");
    m_table.adjustColumn(0);
    return;
  }
  //m_table.visible = 0;
  m_timeField.text = formatTime("%d %b %Y %H:%M:%S",getCurrentTime());
  for(int j=1, n=dynlen(values); j<=n; ++j )  {
    items = strsplit(values[j],"#");
    set = items[3];
    state = items[5];
    action = items[6];
    if ( last_set == set )  {
      set = "";
    }
    else  {
      last_set = set;
      par_state = state;
      if ( state == "ERROR" ) print_node = 2;
      else print_node = 0;
      dynAppend(parents,print_node);
    }
    print_node = 0;
    if ( state == "ERROR" )
      print_node = 2;
    else if ( strlen(action)>0 ) 
      print_node = 1;
    else if ( state != par_state )   {
      if ( par_state=="FINALIZED" && state != "NOT_READY" )
        print_node = 1;
      else if ( par_state=="NOT_READY" && state != "UNKNOWN" )
        print_node = 1;
      else if ( par_state=="CONFIGURED" && state != "UNKNOWN" )
        print_node = 1;
      else if ( par_state=="CREATED" && state != "NOT_READY" )
        print_node = 1;
      else if ( par_state=="READY" && state != "READY" )
        print_node = 1;
      else if ( par_state=="RUNNING" && state != "RUNNING" )
        print_node = 1;
      else 
        print_node = 0;
    }
    if ( parents[dynlen(parents)] < print_node ) parents[dynlen(parents)] = print_node;
    dynAppend(lines,j);
  }
  for(int j=1, np=0, n=dynlen(lines); j<=n; ++j )  {
    items = strsplit(values[lines[j]],"#");
    set = items[3];
    state = items[5];
    action = items[6];
    command = items[7];
    if ( last_set != set )  {
      last_set = set;
      cell_type = parents[++np];
      if ( strlen(action)>0 )  {
        cell_type = 1;
      }
      else if ( cell_type <= 1 )  {
        cell_type = StreamStatusDisplay_CellColor(state);
      }
      if ( m_incremental.state(0) )  {
        if ( cnt <= nLine )  {
          m_table.appendLine(cols[1],"",cols[2],"",cols[3],"",cols[4],"",cols[5],"");
        }
        else  {
          m_table.cellValueRC(nLine,cols[1],"");
          m_table.cellValueRC(nLine,cols[2],"");
          m_table.cellValueRC(nLine,cols[3],"");
          m_table.cellValueRC(nLine,cols[4],"");
          m_table.cellValueRC(nLine,cols[5],"");
        }
        StreamStatusDisplay_SetCellColors(nLine,99,cols);
        nLine++;
      }
    }
    else  {
      set = "";
      if ( parents[np] == 2 && (state == "ERROR" || state == "UNKNOWN") )  {
        cell_type = parents[np];
      }
      else if ( parents[np] == 2 )  {
        cell_type = StreamStatusDisplay_CellColor(state);
        if ( cell_type < 0 ) cell_type = 1;
      }
      else if ( strlen(action)>0 )  {
        cell_type = 1;
      }
      else if ( parents[np] == 1 )  {
        cell_type = StreamStatusDisplay_CellColor(state);
        if ( cell_type < 0 ) cell_type = parents[np];
      }
      else if ( m_incremental.state(0) )  {
        cell_type = StreamStatusDisplay_CellColor(state);
      }
      else  {
        continue;
      }
    }
    
    taskDp = items[8];
    if ( strpos(items[4],last_set) == 0 )  dpGet(taskDp+".Name",taskName);
    else                              taskName = items[4];

    if ( cnt <= nLine )  {
      m_table.appendLine(cols[1],set,cols[2],taskName,cols[3],state,cols[4],action,cols[5],command);
    }
    else  {
      m_table.cellValueRC(nLine,cols[1],set);
      m_table.cellValueRC(nLine,cols[2],taskName);
      m_table.cellValueRC(nLine,cols[3],state);
      m_table.cellValueRC(nLine,cols[4],action);
      m_table.cellValueRC(nLine,cols[5],command);
    }
    StreamStatusDisplay_SetCellColors(nLine,cell_type,cols);
    ++nLine;
  }
  if ( cnt > nLine )  m_table.deleteLinesN(nLine-1,cnt-nLine);
  for(int j=0,n=dynlen(cols); j<n; ++j)  {
    //m_table.adjustColumn(j);
  }
  //m_table.lineVisible(0);
  //m_table.visible = 1;
}
//=============================================================================
void StreamStatusDisplay_SetCellColors(int nLine,int cell_type,dyn_string cols)  {
    if ( cell_type == 2 )
      StreamStatusDisplay_SetCells(nLine,cols,"{255,0,0}","{0,0,0}");
    else if ( cell_type == 1 )
      StreamStatusDisplay_SetCells(nLine,cols,"{200,200,200}","{0,0,0}");
    else if ( cell_type == 3 )
      StreamStatusDisplay_SetCells(nLine,cols,"{0,150,50}","{255,255,255}");
    else if ( cell_type == 5 )
      StreamStatusDisplay_SetCells(nLine,cols,"{0,220,170}","{0,0,0}");
    else if ( cell_type == 6 )
      StreamStatusDisplay_SetCells(nLine,cols,"{0,180,220}","{0,0,0}");
    else if ( cell_type == 7 )
      StreamStatusDisplay_SetCells(nLine,cols,"{255,255,100}","{0,0,0}");
    else if ( cell_type == 8 )
      StreamStatusDisplay_SetCells(nLine,cols,"{255,200,50}","{0,0,0}");
    else
      StreamStatusDisplay_SetCells(nLine,cols,"{255,255,255}","{0,0,0}");
}
//=============================================================================
int StreamStatusDisplay_CellColor(string state)  {
  if ( state=="RUNNING" ) return 5;
  else if ( state=="READY" ) return 6;
  else if ( state=="STOPPED" ) return 7;
  else if ( state=="FINALIZED" ) return 7;
  else if ( state=="CREATED" ) return 7;
  else if ( state=="CONFIGURED" ) return 7;
  else if ( state=="NOT_READY" ) return 7;
  else if ( state=="UNKNOWN" ) return 8;
  return -1;
}
