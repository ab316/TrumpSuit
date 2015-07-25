#pragma once

namespace DXFramework
{ // DXFramework namespace begin

	class CDXUIButton;
	class CDXUIEditBox;
	class CDXUICheckBox;
	class CDXUIRadioButton;
	class CDXUIScrollBar;

	// Abstract class used by windows message processing function. Derived classes
	// should include functionality to respond to the messages as required.
	class IDXMouseHandler
	{
	public:
		virtual	bool	OnMouseMove(DXPoint point)			=	0;
		virtual bool	OnLeftMouseDown(DXPoint point)		=	0;
		virtual bool	OnLeftMouseUp(DXPoint point)		=	0;
	};


	// Mouse handler for the button UI element.
	class CDXUIButtonMouseHandler : public IDXMouseHandler
	{
	private:
		CDXUIButton*	m_pControl;
		bool			m_bMouseDown;

	public:
		virtual	bool	OnMouseMove(DXPoint point);
		virtual bool	OnLeftMouseDown(DXPoint point);
		virtual bool	OnLeftMouseUp(DXPoint point);

	public:
		CDXUIButtonMouseHandler(CDXUIButton* pControl) :	m_pControl(pControl),
															m_bMouseDown(false)
		{
		}
		
		~CDXUIButtonMouseHandler()
		{
		}
	};


	// Mouse handler for the Edit box UI element.
	class CDXUIEditBoxMouseHandler : public IDXMouseHandler
	{
	private:
		CDXUIEditBox*	m_pControl;

	public:
		virtual	bool	OnMouseMove(DXPoint point);
		virtual bool	OnLeftMouseDown(DXPoint point);
		virtual bool	OnLeftMouseUp(DXPoint point);

	public:
		CDXUIEditBoxMouseHandler(CDXUIEditBox* pControl) : m_pControl(pControl)
		{
		}
		
		~CDXUIEditBoxMouseHandler()
		{
		}
	};


	// Mouse handler for the CheckBox UI element.
	class CDXUICheckBoxMouseHandler : public IDXMouseHandler
	{
	private:
		CDXUICheckBox*	m_pControl;

	public:
		virtual	bool	OnMouseMove(DXPoint point);
		virtual bool	OnLeftMouseDown(DXPoint point);
		virtual bool	OnLeftMouseUp(DXPoint point);

	public:
		CDXUICheckBoxMouseHandler(CDXUICheckBox* pControl) : m_pControl(pControl)
		{
		}
		
		~CDXUICheckBoxMouseHandler()
		{
		}
	};


	// Mouse handler for the Radio button UI element.
	class CDXUIRadioButtonMouseHandler : public IDXMouseHandler
	{
	private:
		CDXUIRadioButton*	m_pControl;

	public:
		virtual	bool	OnMouseMove(DXPoint point);
		virtual bool	OnLeftMouseDown(DXPoint point);
		virtual bool	OnLeftMouseUp(DXPoint point);

	public:
		CDXUIRadioButtonMouseHandler(CDXUIRadioButton* pControl) :	m_pControl(pControl)
		{
		}
		
		~CDXUIRadioButtonMouseHandler()
		{
		}
	};


	// Mouse handler for the Scroll Bar UI element.
	class CDXUIScrollBarMouseHandler : public IDXMouseHandler
	{
	private:
		CDXUIScrollBar*		m_pControl;
		DXPoint				m_ptMousePos;
		bool				m_bMouseDown;
		bool				m_bDrag;

	public:
		virtual	bool	OnMouseMove(DXPoint point);
		virtual bool	OnLeftMouseDown(DXPoint point);
		virtual bool	OnLeftMouseUp(DXPoint point);

	public:
		CDXUIScrollBarMouseHandler(CDXUIScrollBar* pControl) :	m_pControl(pControl),
																m_ptMousePos(0, 0),
																m_bMouseDown(false),
																m_bDrag(false)
		{
		}
		
		~CDXUIScrollBarMouseHandler()
		{
		}
	};

} // DXFramework namespace end