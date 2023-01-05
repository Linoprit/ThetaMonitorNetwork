# -*- coding: utf-8 -*-

###########################################################################
## Python code generated with wxFormBuilder (version 3.10.1-0-g8feb16b)
## http://www.wxformbuilder.org/
##
## PLEASE DO *NOT* EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc
import wx.html

###########################################################################
## Class ThetaMonitorFrame
###########################################################################

class ThetaMonitorFrame ( wx.Frame ):

	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"Theta Monitor", pos = wx.DefaultPosition, size = wx.Size( 1100,800 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL, name = u"MainWindow" )

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

		bSizerMain = wx.BoxSizer( wx.VERTICAL )

		self.m_panel4 = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		self.m_panel4.SetBackgroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_MENU ) )

		bSizer5 = wx.BoxSizer( wx.HORIZONTAL )

		self.m_buttonRefreshCombo = wx.Button( self.m_panel4, wx.ID_ANY, u"Serial Device", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_buttonRefreshCombo.SetToolTip( u"Refresh device combo" )

		bSizer5.Add( self.m_buttonRefreshCombo, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )

		m_choiceSerialPortChoices = [ u"Uart" ]
		self.m_choiceSerialPort = wx.Choice( self.m_panel4, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, m_choiceSerialPortChoices, 0 )
		self.m_choiceSerialPort.SetSelection( 0 )
		self.m_choiceSerialPort.SetMinSize( wx.Size( 300,-1 ) )

		bSizer5.Add( self.m_choiceSerialPort, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )

		self.m_bmToggleBtnConnect = wx.BitmapToggleButton( self.m_panel4, wx.ID_ANY, wx.ArtProvider.GetBitmap( wx.ART_TICK_MARK, wx.ART_BUTTON ), wx.DefaultPosition, wx.DefaultSize, 0 )

		self.m_bmToggleBtnConnect.SetBitmap( wx.ArtProvider.GetBitmap( wx.ART_TICK_MARK, wx.ART_BUTTON ) )
		self.m_bmToggleBtnConnect.SetToolTip( u"Connect to target" )

		bSizer5.Add( self.m_bmToggleBtnConnect, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )

		self.m_toggleBtnFilterBin = wx.ToggleButton( self.m_panel4, wx.ID_ANY, u"bin", wx.DefaultPosition, wx.Size( 40,-1 ), 0 )
		self.m_toggleBtnFilterBin.SetValue( True )
		bSizer5.Add( self.m_toggleBtnFilterBin, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )

		self.m_toggleBtnFilterTxt = wx.ToggleButton( self.m_panel4, wx.ID_ANY, u"txt", wx.DefaultPosition, wx.Size( 40,-1 ), 0 )
		self.m_toggleBtnFilterTxt.SetValue( True )
		bSizer5.Add( self.m_toggleBtnFilterTxt, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )

		self.m_button_clr = wx.Button( self.m_panel4, wx.ID_ANY, u"clr", wx.DefaultPosition, wx.Size( 40,-1 ), 0 )
		bSizer5.Add( self.m_button_clr, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )


		bSizer5.Add( ( 0, 0), 1, wx.EXPAND, 5 )

		self.m_bpButtonExit = wx.BitmapButton( self.m_panel4, wx.ID_ANY, wx.NullBitmap, wx.DefaultPosition, wx.DefaultSize, wx.BU_AUTODRAW|0 )

		self.m_bpButtonExit.SetBitmap( wx.ArtProvider.GetBitmap( wx.ART_QUIT, wx.ART_TOOLBAR ) )
		self.m_bpButtonExit.SetToolTip( u"Exit application" )

		bSizer5.Add( self.m_bpButtonExit, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )


		self.m_panel4.SetSizer( bSizer5 )
		self.m_panel4.Layout()
		bSizer5.Fit( self.m_panel4 )
		bSizerMain.Add( self.m_panel4, 0, wx.ALL|wx.EXPAND, 5 )

		self.m_notebookMain = wx.Notebook( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_notebookMain.SetBackgroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_WINDOW ) )

		self.m_panelTabConsole = wx.Panel( self.m_notebookMain, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer4 = wx.BoxSizer( wx.VERTICAL )

		self.m_htmlWin_console = wx.html.HtmlWindow( self.m_panelTabConsole, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.html.HW_SCROLLBAR_AUTO )
		bSizer4.Add( self.m_htmlWin_console, 1, wx.ALL|wx.EXPAND, 5 )

		self.m_textCtrl_com_line = wx.TextCtrl( self.m_panelTabConsole, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_PROCESS_ENTER|wx.TE_PROCESS_TAB )
		self.m_textCtrl_com_line.SetFont( wx.Font( wx.NORMAL_FONT.GetPointSize(), wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, "FreeMono" ) )

		bSizer4.Add( self.m_textCtrl_com_line, 0, wx.ALL|wx.EXPAND, 5 )


		self.m_panelTabConsole.SetSizer( bSizer4 )
		self.m_panelTabConsole.Layout()
		bSizer4.Fit( self.m_panelTabConsole )
		self.m_notebookMain.AddPage( self.m_panelTabConsole, u"Console", False )
		self.m_panelTabOne = wx.Panel( self.m_notebookMain, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer3 = wx.BoxSizer( wx.VERTICAL )

		self.m_htmlWinMonitor = wx.html.HtmlWindow( self.m_panelTabOne, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.html.HW_SCROLLBAR_AUTO )
		bSizer3.Add( self.m_htmlWinMonitor, 1, wx.ALL|wx.EXPAND, 5 )


		self.m_panelTabOne.SetSizer( bSizer3 )
		self.m_panelTabOne.Layout()
		bSizer3.Fit( self.m_panelTabOne )
		self.m_notebookMain.AddPage( self.m_panelTabOne, u"Tab Two", False )
		self.m_panelTabThree = wx.Panel( self.m_notebookMain, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer31 = wx.BoxSizer( wx.VERTICAL )

		self.m_htmlWin5 = wx.html.HtmlWindow( self.m_panelTabThree, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.html.HW_SCROLLBAR_AUTO )
		bSizer31.Add( self.m_htmlWin5, 1, wx.ALL|wx.EXPAND, 5 )


		self.m_panelTabThree.SetSizer( bSizer31 )
		self.m_panelTabThree.Layout()
		bSizer31.Fit( self.m_panelTabThree )
		self.m_notebookMain.AddPage( self.m_panelTabThree, u"Tab Three", False )

		bSizerMain.Add( self.m_notebookMain, 1, wx.EXPAND |wx.ALL, 5 )

		self.m_textCtrlStatus = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.Size( -1,100 ), wx.TE_MULTILINE|wx.TE_WORDWRAP )
		self.m_textCtrlStatus.SetFont( wx.Font( wx.NORMAL_FONT.GetPointSize(), wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, "FreeMono" ) )

		bSizerMain.Add( self.m_textCtrlStatus, 0, wx.ALL|wx.EXPAND, 5 )


		self.SetSizer( bSizerMain )
		self.Layout()

		self.Centre( wx.BOTH )

		# Connect Events
		self.m_buttonRefreshCombo.Bind( wx.EVT_BUTTON, self.on_RefreshCombo )
		self.m_choiceSerialPort.Bind( wx.EVT_CHOICE, self.on_choice_serial_port )
		self.m_bmToggleBtnConnect.Bind( wx.EVT_TOGGLEBUTTON, self.on_button_connect_toggled )
		self.m_toggleBtnFilterBin.Bind( wx.EVT_TOGGLEBUTTON, self.on_button_filter_bin_toggled )
		self.m_toggleBtnFilterTxt.Bind( wx.EVT_TOGGLEBUTTON, self.on_button_filter_txt_toggled )
		self.m_button_clr.Bind( wx.EVT_BUTTON, self.on_button_clr_pushed )
		self.m_bpButtonExit.Bind( wx.EVT_BUTTON, self.on_quit_button_click )
		self.m_notebookMain.Bind( wx.EVT_NOTEBOOK_PAGE_CHANGED, self.on_notebook_page_changed )

	def __del__( self ):
		pass


	# Virtual event handlers, override them in your derived class
	def on_RefreshCombo( self, event ):
		event.Skip()

	def on_choice_serial_port( self, event ):
		event.Skip()

	def on_button_connect_toggled( self, event ):
		event.Skip()

	def on_button_filter_bin_toggled( self, event ):
		event.Skip()

	def on_button_filter_txt_toggled( self, event ):
		event.Skip()

	def on_button_clr_pushed( self, event ):
		event.Skip()

	def on_quit_button_click( self, event ):
		event.Skip()

	def on_notebook_page_changed( self, event ):
		event.Skip()

	# Virtual image path resolution method. Override this in your derived class.
	def get_fixed_path( self, bitmap_path ):
		return bitmap_path


