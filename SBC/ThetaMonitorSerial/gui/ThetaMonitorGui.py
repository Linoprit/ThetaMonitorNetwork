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
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"Theta Monitor", pos = wx.DefaultPosition, size = wx.Size( 800,500 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL, name = u"MainWindow" )

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

		bSizerMain = wx.BoxSizer( wx.VERTICAL )

		self.m_panel4 = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		self.m_panel4.SetBackgroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_MENU ) )

		bSizer5 = wx.BoxSizer( wx.HORIZONTAL )

		self.m_staticText1 = wx.StaticText( self.m_panel4, wx.ID_ANY, u"Serial Device: ", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText1.Wrap( -1 )

		bSizer5.Add( self.m_staticText1, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )

		m_choiceSerialPortChoices = [ u"Uart" ]
		self.m_choiceSerialPort = wx.Choice( self.m_panel4, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, m_choiceSerialPortChoices, 0 )
		self.m_choiceSerialPort.SetSelection( 0 )
		self.m_choiceSerialPort.SetMinSize( wx.Size( 300,-1 ) )

		bSizer5.Add( self.m_choiceSerialPort, 0, wx.ALL, 5 )


		bSizer5.Add( ( 0, 0), 1, wx.EXPAND, 5 )

		self.m_bpButtonExit = wx.BitmapButton( self.m_panel4, wx.ID_ANY, wx.NullBitmap, wx.DefaultPosition, wx.DefaultSize, wx.BU_AUTODRAW|0 )

		self.m_bpButtonExit.SetBitmap( wx.ArtProvider.GetBitmap( wx.ART_QUIT, wx.ART_TOOLBAR ) )
		bSizer5.Add( self.m_bpButtonExit, 0, wx.ALL, 5 )


		self.m_panel4.SetSizer( bSizer5 )
		self.m_panel4.Layout()
		bSizer5.Fit( self.m_panel4 )
		bSizerMain.Add( self.m_panel4, 0, wx.ALL|wx.EXPAND, 5 )

		self.m_notebookMain = wx.Notebook( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_notebookMain.SetBackgroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_WINDOW ) )

		self.m_panelTabOne = wx.Panel( self.m_notebookMain, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer3 = wx.BoxSizer( wx.VERTICAL )

		self.m_htmlWinTabOne = wx.html.HtmlWindow( self.m_panelTabOne, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.html.HW_SCROLLBAR_AUTO )
		bSizer3.Add( self.m_htmlWinTabOne, 1, wx.ALL|wx.EXPAND, 5 )


		self.m_panelTabOne.SetSizer( bSizer3 )
		self.m_panelTabOne.Layout()
		bSizer3.Fit( self.m_panelTabOne )
		self.m_notebookMain.AddPage( self.m_panelTabOne, u"Monitor", True )
		self.m_panelTabTwo = wx.Panel( self.m_notebookMain, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer4 = wx.BoxSizer( wx.VERTICAL )

		self.m_htmlWin4 = wx.html.HtmlWindow( self.m_panelTabTwo, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.html.HW_SCROLLBAR_AUTO )
		bSizer4.Add( self.m_htmlWin4, 1, wx.ALL|wx.EXPAND, 5 )


		self.m_panelTabTwo.SetSizer( bSizer4 )
		self.m_panelTabTwo.Layout()
		bSizer4.Fit( self.m_panelTabTwo )
		self.m_notebookMain.AddPage( self.m_panelTabTwo, u"Console", False )
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
		bSizerMain.Add( self.m_textCtrlStatus, 0, wx.ALL|wx.EXPAND, 5 )


		self.SetSizer( bSizerMain )
		self.Layout()

		self.Centre( wx.BOTH )

		# Connect Events
		self.m_bpButtonExit.Bind( wx.EVT_BUTTON, self.on_quit_button_click )
		self.m_notebookMain.Bind( wx.EVT_NOTEBOOK_PAGE_CHANGED, self.on_notebook_page_changed )

	def __del__( self ):
		pass


	# Virtual event handlers, override them in your derived class
	def on_quit_button_click( self, event ):
		event.Skip()

	def on_notebook_page_changed( self, event ):
		event.Skip()

	# Virtual image path resolution method. Override this in your derived class.
	def get_fixed_path( self, bitmap_path ):
		return bitmap_path


