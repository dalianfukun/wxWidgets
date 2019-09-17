#ifndef __BUTTONPROP_H__
#define __BUTTONPROP_H__

#include "wx/defs.h"
#include "wx/propgrid/property.h"
#include "wx/propgrid/editors.h"
#include "wx/propgrid/props.h"

// -----------------------------------------------------------------------

/** @class wxButtonProperty
@ingroup classes
Basic property with boolean value.

<b>Supported special attributes:</b>
- "UseCheckbox": Set to 1 to use check box editor instead of combo box.
- "UseDClickCycling": Set to 1 to cycle combo box instead showing the list.
*/
class WXDLLIMPEXP_LSTC wxButtonProperty : public wxPGProperty
{
	wxDECLARE_CLASS(wxButtonProperty);
public:
    wxButtonProperty(const wxString& label = wxPG_LABEL, const wxString& caption = wxPG_LABEL,
        const wxString& name = wxPG_LABEL,
        bool value = false);
    virtual ~wxButtonProperty();

    virtual wxString ValueToString(wxVariant& value, int argFlags = 0) const;
    virtual bool StringToValue(wxVariant& variant,
        const wxString& text,
        int argFlags = 0) const;
    virtual bool IntToValue(wxVariant& variant,
        int number, int argFlags = 0) const;
    virtual bool DoSetAttribute(const wxString& name, wxVariant& value);
    virtual wxVariant DoGetAttribute(const wxString& name) const ;
	virtual const wxPGEditor* DoGetEditorClass() const;

private:
    wxString m_caption;
};


//
// Use custom check box code instead of native control
// for cleaner (i.e. more integrated) look.
//
class WXDLLIMPEXP_LSTC wxPGButtonEditor : public wxPGEditor
{
    wxDECLARE_CLASS(wxPGButtonEditor);
public:
    wxPGButtonEditor() {}
    virtual ~wxPGButtonEditor();

    virtual wxString GetName() const;
    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
        wxPGProperty* property,
        const wxPoint& pos,
        const wxSize& size) const;
    virtual void UpdateControl(wxPGProperty* property,
        wxWindow* ctrl) const;
    virtual bool OnEvent(wxPropertyGrid* propgrid,
        wxPGProperty* property,
        wxWindow* primaryCtrl,
        wxEvent& event) const;
    virtual bool GetValueFromControl(wxVariant& variant,
        wxPGProperty* property,
        wxWindow* ctrl) const;
    virtual void SetValueToUnspecified(wxPGProperty* property,
        wxWindow* ctrl) const;

    virtual void DrawValue(wxDC& dc,
        const wxRect& rect,
        wxPGProperty* property,
        const wxString& text) const;
    //virtual wxPGCellRenderer* GetCellRenderer() const;

    virtual void SetControlIntValue(wxPGProperty* property,
        wxWindow* ctrl,
        int value) const;

    void SetCaption(const wxString& caption);
private:
    wxString m_caption; 
};


#define ID_MULTIBTN_TEST_BEGIN 10500
enum{
	ID_TW_POST_PLOT = ID_MULTIBTN_TEST_BEGIN+1,
	ID_TW_POST_NEWPLOT,
	ID_TW_POST_ADDPLOT
};
// -----------------------------------------------------------------------
#include <wx/dynarray.h> 
#include <wx/arrimpl.cpp>

WX_DECLARE_OBJARRAY ( wxRect, btnRectArray);
class twMultiButtonProperty;
class WXDLLIMPEXP_LSTC twPGMultiButton : public wxWindow
{
public:
    twPGMultiButton(wxArrayString btns, wxArrayString cmds, wxPropertyGrid* pg, const wxRect& rc, const wxPGProperty* property );
    virtual ~twPGMultiButton();

	void OnLeftDown( wxMouseEvent& event );

	void OnMultiButtonClick ( wxCommandEvent& evt);

    unsigned int GetCount() const { return (unsigned int) m_buttons.size(); }

private:
	void OnPaint(wxPaintEvent& event);

protected:

	wxArrayString m_btnLabels, m_btnCmds;
	btnRectArray m_btnRectArray;
	twMultiButtonProperty *m_refProperty;
	int m_curClickBtn;

public:
	wxArrayPtrVoid  m_buttons;

	DECLARE_CLASS(twPGMultiButton)
	DECLARE_EVENT_TABLE()
};

class WXDLLIMPEXP_LSTC twMultiButtonEditor : public wxPGEditor
{
	DECLARE_DYNAMIC_CLASS(twMultiButtonEditor)
public:
	twMultiButtonEditor() {}
	virtual ~twMultiButtonEditor();
	virtual wxString GetName() const;

	virtual wxPGWindowList CreateControls( wxPropertyGrid* propGrid,
		wxPGProperty* property,
		const wxPoint& pos,
		const wxSize& sz ) const;
	virtual bool OnEvent( wxPropertyGrid* propGrid,
		wxPGProperty* property,
		wxWindow* ctrl,
		wxEvent& event ) const;
	virtual void DrawValue( wxDC& dc,
		const wxRect& rect,
		wxPGProperty* property,
		const wxString& text ) const;
	virtual void UpdateControl( wxPGProperty* property,
		wxWindow* ctrl ) const;
};

class WXDLLIMPEXP_LSTC twMultiButtonProperty : public wxPGProperty
{
	friend class twPropertyGrid;

public:

	twMultiButtonProperty(const wxArrayString btns, const wxArrayString btnCmds, const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL);
	virtual ~twMultiButtonProperty();
	const wxPGEditor* DoGetEditorClass() const;

	virtual wxVariant ChildChanged( wxVariant& thisValue,
		int childIndex,
		wxVariant& childValue ) const;
	virtual void RefreshChildren();
	
	bool IsOwnerDraw() {return m_ownerDraw;}
	void SetOwnerDraw(bool bOwnerdraw) { m_ownerDraw = bOwnerdraw;}

private:
	bool m_ownerDraw;

public:
	wxArrayString m_btnLabels, m_btnCmds;
	twPGMultiButton *m_myMultiBtn;
	wxRect m_myRect;
	int m_btnW, m_btnH;
};

class WXDLLIMPEXP_LSTC twMultiButtonRender : public wxPGDefaultRenderer
{
public:

	virtual bool Render( wxDC& dc, const wxRect& rect, const wxPropertyGrid* propertyGrid, wxPGProperty* property, int column, int item, int flags ) const;

};

#endif
