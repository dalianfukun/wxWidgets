// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/object.h"
#include "wx/hash.h"
#include "wx/string.h"
#include "wx/log.h"
#include "wx/event.h"
#include "wx/window.h"
#include "wx/panel.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/button.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/cursor.h"
#include "wx/dialog.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/choice.h"
#include "wx/stattext.h"
#include "wx/scrolwin.h"
#include "wx/dirdlg.h"
#include "wx/sizer.h"
#include "wx/textdlg.h"
#include "wx/filedlg.h"
#include "wx/statusbr.h"
#include "wx/intl.h"
#include "wx/frame.h"
#endif


#include "wx/timer.h"
#include "wx/dcbuffer.h"
#include "wx/bmpbuttn.h"
#include "wx/lstc/lstc_buttonprop.h"


// This define is necessary to prevent macro clearing
#define __wxPG_SOURCE_FILE__

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/editors.h"
#include "wx/propgrid/props.h"

#if wxPG_USE_RENDERER_NATIVE
#include "wx/renderer.h"
#endif

// How many pixels between textctrl and button
#ifdef __WXMAC__
#define wxPG_TEXTCTRL_AND_BUTTON_SPACING        4
#else
#define wxPG_TEXTCTRL_AND_BUTTON_SPACING        2
#endif

#define wxPG_BUTTON_SIZEDEC                         0

#include "wx/odcombo.h"

// -----------------------------------------------------------------------

#if defined(__WXMSW__)
// tested
#define wxPG_NAT_BUTTON_BORDER_Y            1

#define wxPG_CHECKMARK_XADJ                 1
#define wxPG_CHECKMARK_YADJ                 (-1)
#define wxPG_CHECKMARK_WADJ                 0
#define wxPG_CHECKMARK_HADJ                 0
#define wxPG_CHECKMARK_DEFLATE              0

#define wxPG_TEXTCTRLYADJUST                (m_spacingy+0)

#elif defined(__WXGTK__)
// tested
#define wxPG_CHECKMARK_XADJ                 1
#define wxPG_CHECKMARK_YADJ                 1
#define wxPG_CHECKMARK_WADJ                 (-2)
#define wxPG_CHECKMARK_HADJ                 (-2)
#define wxPG_CHECKMARK_DEFLATE              3

#define wxPG_NAT_BUTTON_BORDER_Y        1

#define wxPG_TEXTCTRLYADJUST            0

#elif defined(__WXMAC__)
// *not* tested
#define wxPG_CHECKMARK_XADJ                 4
#define wxPG_CHECKMARK_YADJ                 4
#define wxPG_CHECKMARK_WADJ                 -6
#define wxPG_CHECKMARK_HADJ                 -6
#define wxPG_CHECKMARK_DEFLATE              0

#define wxPG_NAT_BUTTON_BORDER_Y        0

#define wxPG_TEXTCTRLYADJUST            0

#else
// defaults
#define wxPG_CHECKMARK_XADJ                 0
#define wxPG_CHECKMARK_YADJ                 0
#define wxPG_CHECKMARK_WADJ                 0
#define wxPG_CHECKMARK_HADJ                 0
#define wxPG_CHECKMARK_DEFLATE              0

#define wxPG_NAT_BUTTON_BORDER_Y        0

#define wxPG_TEXTCTRLYADJUST            0

#endif

// for odcombo
#ifdef __WXMAC__
#define wxPG_CHOICEXADJUST           -3 // required because wxComboCtrl reserves 3pixels for wxTextCtrl's focus ring
#define wxPG_CHOICEYADJUST           -3
#else
#define wxPG_CHOICEXADJUST           0
#define wxPG_CHOICEYADJUST           0
#endif

// Number added to image width for SetCustomPaintWidth
#define ODCB_CUST_PAINT_MARGIN               6

// Milliseconds to wait for two mouse-ups after focus in order
// to trigger a double-click.
#define DOUBLE_CLICK_CONVERSION_TRESHOLD        500


extern "C"
{
	extern void SendCommand(const char* line);
};

// -----------------------------------------------------------------------
// wxPGButtonEditor
// -----------------------------------------------------------------------

#if wxPG_INCLUDE_CHECKBOX

#define WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS1(EDITOR,CLASSNAME,BASECLASS) \
wxIMPLEMENT_CLASS(CLASSNAME, BASECLASS); \
wxString CLASSNAME::GetName() const \
{ \
    return wxS(#EDITOR); \
} \
wxPGEditor* wxPGEditor_##EDITOR = NULL;

WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS1(Button,
    wxPGButtonEditor,
    wxPGEditor)
    // Check box state flags
    enum
{
    wxSCB_STATE_UNCHECKED = 0,
    wxSCB_STATE_CHECKED = 1,
    wxSCB_STATE_BOLD = 2,
    wxSCB_STATE_UNSPECIFIED = 4
};

const int wxSCB_SETVALUE_CYCLE = 2;

static void DrawSimpleButton(wxWindow* win, wxDC& dc, const wxRect& rect,
    int box_h, int state, const wxString& caption)
{
#if wxPG_USE_RENDERER_NATIVE
    // Box rectangle
    wxRect r(rect.x + wxPG_XBEFORETEXT, rect.y + ((rect.height - box_h) / 2),
        rect.width - wxPG_XBEFORETEXT, box_h);

    int cbFlags = 0;
    if (state & wxSCB_STATE_UNSPECIFIED)
    {
        cbFlags |= wxCONTROL_UNDETERMINED;
    }
    else if (state & wxSCB_STATE_CHECKED)
    {
        cbFlags |= wxCONTROL_CHECKED;
    }

    if (state & wxSCB_STATE_BOLD)
    {
        // wxCONTROL_CHECKED and wxCONTROL_PRESSED flags
        // are equivalent for wxOSX so we have to use
        // other flag to indicate "selected state".
#ifdef __WXOSX__
        cbFlags |= wxCONTROL_FOCUSED;
#else
        cbFlags |= wxCONTROL_PRESSED;
#endif
    }

    wxRendererNative::Get().DrawPushButton(win, dc, r, cbFlags);

    int len = caption.Length() * dc.GetCharWidth();
    r.x += (r.width - len) / 2.0;
	r.y += (r.height - dc.GetCharHeight()) / 2.0;
    wxRendererNative::Get().DrawItemText(win, dc, caption, r);
#else
    wxUnusedVar(win);

    // Box rectangle
    wxRect r(rect.x + wxPG_XBEFORETEXT, rect.y + ((rect.height - box_h) / 2),
        box_h, box_h);
    wxColour useCol = dc.GetTextForeground();

    if (state & wxSCB_STATE_UNSPECIFIED)
    {
        useCol = wxColour(220, 220, 220);
    }

    // Draw check mark first because it is likely to overdraw the
    // surrounding rectangle.
    if (state & wxSCB_STATE_CHECKED)
    {
        wxRect r2(r.x + wxPG_CHECKMARK_XADJ,
            r.y + wxPG_CHECKMARK_YADJ,
            r.width + wxPG_CHECKMARK_WADJ,
            r.height + wxPG_CHECKMARK_HADJ);
#if wxPG_CHECKMARK_DEFLATE
        r2.Deflate(wxPG_CHECKMARK_DEFLATE);
#endif
        dc.DrawCheckMark(r2);

        // This would draw a simple cross check mark.
        // dc.DrawLine(r.x,r.y,r.x+r.width-1,r.y+r.height-1);
        // dc.DrawLine(r.x,r.y+r.height-1,r.x+r.width-1,r.y);
    }

    if (!(state & wxSCB_STATE_BOLD))
    {
        // Pen for thin rectangle.
        dc.SetPen(useCol);
    }
    else
    {
        // Pen for bold rectangle.
        wxPen linepen(useCol, 2, wxPENSTYLE_SOLID);
        linepen.SetJoin(wxJOIN_MITER); // This prevents round edges.
        dc.SetPen(linepen);
        r.x++;
        r.y++;
        r.width--;
        r.height--;
    }

    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(r);
    dc.SetPen(*wxTRANSPARENT_PEN);
#endif
}

//
// Real simple custom-drawn checkbox-without-label class.
//
class wxSimpleButton : public wxControl
{
public:

    void SetValue(int value);

    wxSimpleButton(wxWindow* parent,
        wxWindowID id,
        const wxString& caption,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize)
        : wxControl(parent, id, pos, size, wxBORDER_NONE | wxWANTS_CHARS)
    {
        // Due to SetOwnFont stuff necessary for GTK+ 1.2, we need to have this
        SetFont(parent->GetFont());

        m_state = 0;
        m_boxHeight = 12;
        m_caption = caption;

        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    }

    virtual ~wxSimpleButton();

    int m_state;
    int m_boxHeight;
    wxString m_caption;

private:
    void OnPaint(wxPaintEvent& event);
    void OnLeftClick(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    void OnResize(wxSizeEvent& event)
    {
        Refresh();
        event.Skip();
    }
    void OnLeftClickActivate(wxCommandEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

wxDEFINE_EVENT(wxEVT_CB_LEFT_CLICK_ACTIVATE, wxCommandEvent);

wxBEGIN_EVENT_TABLE(wxSimpleButton, wxControl)
EVT_PAINT(wxSimpleButton::OnPaint)
EVT_LEFT_DOWN(wxSimpleButton::OnLeftClick)
EVT_LEFT_DCLICK(wxSimpleButton::OnLeftClick)
EVT_KEY_DOWN(wxSimpleButton::OnKeyDown)
EVT_SIZE(wxSimpleButton::OnResize)
EVT_COMMAND(wxID_ANY, wxEVT_CB_LEFT_CLICK_ACTIVATE, wxSimpleButton::OnLeftClickActivate)
wxEND_EVENT_TABLE()

wxSimpleButton::~wxSimpleButton()
{
}

void wxSimpleButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxRect rect(GetClientSize());
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    rect.y += 1;
    rect.width += 1;

    wxColour bgcol = GetBackgroundColour();
    dc.SetBrush(bgcol);
    dc.SetPen(bgcol);
    dc.DrawRectangle(rect);

    dc.SetTextForeground(GetForegroundColour());

    int state = m_state;
    if (!(state & wxSCB_STATE_UNSPECIFIED) &&
        GetFont().GetWeight() == wxFONTWEIGHT_BOLD)
        state |= wxSCB_STATE_BOLD;

    DrawSimpleButton(this, dc, rect, m_boxHeight, state, m_caption);
}

void wxSimpleButton::OnLeftClick(wxMouseEvent& event)
{
    //if ((event.m_x > (wxPG_XBEFORETEXT - 2)) &&
    //    (event.m_x <= (wxPG_XBEFORETEXT - 2 + m_boxHeight)))
    //{
        SetValue(wxSCB_SETVALUE_CYCLE);
    //}
}

void wxSimpleButton::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_SPACE)
    {
        SetValue(wxSCB_SETVALUE_CYCLE);
    }
}

void wxSimpleButton::SetValue(int value)
{
    if (value == wxSCB_SETVALUE_CYCLE)
    {
        m_state ^= wxSCB_STATE_CHECKED;
    }
    else
    {
        m_state = value;
    }
    Refresh();

    wxCommandEvent evt(wxEVT_CHECKBOX, GetParent()->GetId());

    wxPropertyGrid* propGrid = (wxPropertyGrid*)GetParent();
    wxASSERT(wxDynamicCast(propGrid, wxPropertyGrid));
    propGrid->HandleCustomEditorEvent(evt);
}

void wxSimpleButton::OnLeftClickActivate(wxCommandEvent& evt)
{
    // Construct mouse pseudo-event for initial mouse click
    wxMouseEvent mouseEvt(wxEVT_LEFT_DOWN);
    mouseEvt.m_x = evt.GetInt();
    mouseEvt.m_y = evt.GetExtraLong();
    OnLeftClick(mouseEvt);
}

wxPGWindowList wxPGButtonEditor::CreateControls(wxPropertyGrid* propGrid,
    wxPGProperty* property,
    const wxPoint& pos,
    const wxSize& size) const
{
    if (property->HasFlag(wxPG_PROP_READONLY))
        return NULL;

    wxPoint pt = pos;
    pt.x -= wxPG_XBEFOREWIDGET;
    wxSize sz = size;
    //sz.x = propGrid->GetFontHeight() + (wxPG_XBEFOREWIDGET * 2) + 4;

    wxSimpleButton* cb = new wxSimpleButton(propGrid->GetPanel(),
        wxID_ANY, m_caption, pt, sz);

    cb->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    UpdateControl(property, cb);

    if (!property->IsValueUnspecified())
    {
        if (propGrid->HasInternalFlag(wxPG_FL_ACTIVATION_BY_CLICK))
        {
            // Send the event to toggle the value (if mouse cursor is on the item)
            wxPoint point = cb->ScreenToClient(::wxGetMousePosition());
            wxCommandEvent *evt = new wxCommandEvent(wxEVT_CB_LEFT_CLICK_ACTIVATE, cb->GetId());
            // Store mouse pointer position
            evt->SetInt(point.x);
            evt->SetExtraLong(point.y);
            wxQueueEvent(cb, evt);
        }
    }

    propGrid->SetInternalFlag(wxPG_FL_FIXED_WIDTH_EDITOR);

    return cb;
}

void wxPGButtonEditor::DrawValue(wxDC& dc, const wxRect& rect,
    wxPGProperty* property,
    const wxString& WXUNUSED(text)) const
{
    int state = wxSCB_STATE_UNCHECKED;

    if (!property->IsValueUnspecified())
    {
        state = property->GetChoiceSelection();
        if (dc.GetFont().GetWeight() == wxFONTWEIGHT_BOLD)
            state |= wxSCB_STATE_BOLD;
    }
    else
    {
        state |= wxSCB_STATE_UNSPECIFIED;
    }

    DrawSimpleButton(property->GetGrid(), dc, rect, dc.GetCharHeight(), state, m_caption);
}

void wxPGButtonEditor::UpdateControl(wxPGProperty* property,
    wxWindow* ctrl) const
{
    wxSimpleButton* cb = (wxSimpleButton*)ctrl;
    wxASSERT(cb);

    if (!property->IsValueUnspecified())
        cb->m_state = property->GetChoiceSelection();
    else
        cb->m_state = wxSCB_STATE_UNSPECIFIED;

    wxPropertyGrid* propGrid = property->GetGrid();
    cb->m_boxHeight = propGrid->GetFontHeight();

    cb->Refresh();
}

bool wxPGButtonEditor::OnEvent(wxPropertyGrid* WXUNUSED(propGrid), wxPGProperty* WXUNUSED(property),
    wxWindow* WXUNUSED(ctrl), wxEvent& event) const
{
    if (event.GetEventType() == wxEVT_CHECKBOX)
    {
        return true;
    }
    return false;
}


bool wxPGButtonEditor::GetValueFromControl(wxVariant& variant, wxPGProperty* property, wxWindow* ctrl) const
{
    wxSimpleButton* cb = (wxSimpleButton*)ctrl;

    int index = cb->m_state;

    if (index != property->GetChoiceSelection() ||
        // Changing unspecified always causes event (returning
        // true here should be enough to trigger it).
        property->IsValueUnspecified()
        )
    {
        return property->IntToValue(variant, index, wxPG_PROPERTY_SPECIFIC);
    }
    return false;
}


void wxPGButtonEditor::SetControlIntValue(wxPGProperty* WXUNUSED(property), wxWindow* ctrl, int value) const
{
    if (value != 0) value = 1;
    ((wxSimpleButton*)ctrl)->m_state = value;
    ctrl->Refresh();
}


void wxPGButtonEditor::SetValueToUnspecified(wxPGProperty* WXUNUSED(property), wxWindow* ctrl) const
{
    ((wxSimpleButton*)ctrl)->m_state = wxSCB_STATE_UNSPECIFIED;
    ctrl->Refresh();
}

void wxPGButtonEditor::SetCaption(const wxString& caption)
{
    m_caption = caption;
}

wxPGButtonEditor::~wxPGButtonEditor()
{
    wxPG_EDITOR(Button) = NULL;
}

#endif // wxPG_INCLUDE_CHECKBOX

// -----------------------------------------------------------------------
// wxButtonProperty
// -----------------------------------------------------------------------

// We cannot use standard WX_PG_IMPLEMENT_PROPERTY_CLASS macro, since
// there is a custom GetEditorClass.

wxPGButtonEditor* s_buttonEditor = NULL;

//wxIMPLEMENT_CLASS(wxButtonProperty, wxPGProperty);

const wxPGEditor* wxButtonProperty::DoGetEditorClass() const
{
    // Select correct editor control.
#if wxPG_INCLUDE_CHECKBOX
    if (!(m_flags & wxPG_PROP_USE_CHECKBOX))
        return wxPGEditor_Choice;
    s_buttonEditor->SetCaption(m_caption);
    return wxPGEditor_Button;
#else
    return wxPGEditor_Choice;
#endif
}

wxButtonProperty::wxButtonProperty(const wxString& label, const wxString& caption, const wxString& name, bool value) :
    wxPGProperty(label, name), m_caption(caption)
{
    m_choices.Assign(wxPGGlobalVars->m_boolChoices);

    SetValue(wxPGVariant_Bool(value));

    m_flags |= wxPG_PROP_USE_DCC;

    if (!wxPGEditor_Button) {
        wxPGEditor_Button = s_buttonEditor = new wxPGButtonEditor();
        wxPropertyGrid::RegisterEditorClass(wxPGEditor_Button, "ButtonEditor");
    }
}

wxButtonProperty::~wxButtonProperty() { }

wxString wxButtonProperty::ValueToString(wxVariant& value,
    int argFlags) const
{
    bool boolValue = value.GetBool();

    // As a fragment of composite string value,
    // make it a little more readable.
   /* if (argFlags & wxPG_COMPOSITE_FRAGMENT)
    {
        if (boolValue)
        {
            return m_label;
        }
        else
        {
            if (argFlags & wxPG_UNEDITABLE_COMPOSITE_FRAGMENT)
                return wxEmptyString;

            wxString notFmt;
            if (wxPGGlobalVars->m_autoGetTranslation)
                notFmt = _("Not %s");
            else
                notFmt = wxS("Not %s");

            return wxString::Format(notFmt.c_str(), m_label.c_str());
        }
    }

    if (!(argFlags & wxPG_FULL_VALUE))
    {
        return wxPGGlobalVars->m_boolChoices[boolValue ? 1 : 0].GetText();
    }

    return boolValue ? wxS("true") : wxS("false");*/
	return "true";
}

bool wxButtonProperty::StringToValue(wxVariant& variant, const wxString& text, int WXUNUSED(argFlags)) const
{
    bool boolValue = false;
    if (text.CmpNoCase(wxPGGlobalVars->m_boolChoices[1].GetText()) == 0 ||
        text.CmpNoCase(wxS("true")) == 0 ||
        text.CmpNoCase(m_label) == 0)
        boolValue = true;

    if (text.empty())
    {
        variant.MakeNull();
        return true;
    }

    if (variant != boolValue)
    {
        variant = wxPGVariant_Bool(boolValue);
        return true;
    }
    return false;
}

bool wxButtonProperty::IntToValue(wxVariant& variant, int value, int) const
{
    bool boolValue = value ? true : false;

    if (variant != boolValue)
    {
        variant = wxPGVariant_Bool(boolValue);
        return true;
    }
    return false;
}

bool wxButtonProperty::DoSetAttribute(const wxString& name, wxVariant& value)
{
#if wxPG_INCLUDE_CHECKBOX
    if (name == wxPG_BOOL_USE_CHECKBOX)
    {
        if (value.GetLong())
            m_flags |= wxPG_PROP_USE_CHECKBOX;
        else
            m_flags &= ~(wxPG_PROP_USE_CHECKBOX);
        return true;
    }
#endif
    if (name == wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING)
    {
        if (value.GetLong())
            m_flags |= wxPG_PROP_USE_DCC;
        else
            m_flags &= ~(wxPG_PROP_USE_DCC);
        return true;
    }
    return false;
}

wxVariant wxButtonProperty::DoGetAttribute(const wxString& name) const
{
    wxVariant value;
#if wxPG_INCLUDE_CHECKBOX
    if (name == wxPG_BOOL_USE_CHECKBOX)
    {
        value = (bool)((m_flags & wxPG_PROP_USE_CHECKBOX) != 0);
    }
    else
#endif
        if (name == wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING)
        {
            value = (bool)((m_flags & wxPG_PROP_USE_DCC) != 0);
        }
    return value;
}
/*

WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(twButton,
	twMultiButtonEditor,
	wxPGEditor)
twMultiButtonEditor* s_twBtnEditor = NULL;

const wxPGEditor* twMultiButtonProperty::DoGetEditorClass() const
{
	// Select correct editor control.
	return wxPGEditor_twButton;
}

twMultiButtonProperty::twMultiButtonProperty( const wxArrayString btns, const wxArrayString btnCmds, const wxString& label,
const wxString& name)
: wxPGProperty(label,name)
{
	m_btnLabels = btns;
	m_btnCmds = btnCmds;
	m_myMultiBtn = NULL;
	m_ownerDraw = true;
	SetCommonValue(1);
	if (!wxPGEditor_twButton) {
		wxPGEditor_twButton = s_twBtnEditor = new twMultiButtonEditor();
		wxPropertyGrid::RegisterEditorClass(wxPGEditor_twButton, "twButtonEditor");
	}
}

twMultiButtonProperty::~twMultiButtonProperty() 
{
	if(m_myMultiBtn)
		delete m_myMultiBtn;
}

void twMultiButtonProperty::RefreshChildren()
{

}

wxVariant twMultiButtonProperty::ChildChanged( wxVariant& thisValue,
	int childIndex,
	wxVariant& childValue ) const
{
	return NULL;
}

void twMultiButtonEditor::UpdateControl( wxPGProperty* property,
wxWindow* ctrl ) const
{

}

twMultiButtonEditor::~twMultiButtonEditor()
{

}

wxPGWindowList twMultiButtonEditor::CreateControls( wxPropertyGrid* propGrid,
wxPGProperty* property,
const wxPoint& pos,
const wxSize& sz ) const
{
	twMultiButtonProperty *p = (twMultiButtonProperty*)property;
	twPGMultiButton *multBtn = new twPGMultiButton(p->m_btnLabels, p->m_btnCmds, propGrid, p->m_myRect, property);
	return multBtn;
}

bool twMultiButtonEditor::OnEvent( wxPropertyGrid* propGrid,
	wxPGProperty* property,
	wxWindow* ctrl,
	wxEvent& event ) const
{
	return false;
}

void twMultiButtonEditor::DrawValue( wxDC& dc, const wxRect& rect,
	wxPGProperty* property,
	const wxString& WXUNUSED(text) ) const
{
}

IMPLEMENT_CLASS(twPGMultiButton, wxWindow)
BEGIN_EVENT_TABLE(twPGMultiButton, wxWindow)
EVT_PAINT(twPGMultiButton::OnPaint)
EVT_LEFT_DOWN( twPGMultiButton::OnLeftDown )
EVT_BUTTON( wxID_ANY, twPGMultiButton::OnMultiButtonClick )
END_EVENT_TABLE()

WX_DEFINE_OBJARRAY(btnRectArray);

void twPGMultiButton::OnLeftDown( wxMouseEvent& event )
{
	////@begin wxEVT_LEFT_DOWN event handler for ID_TEST in test.
	// Before editing this code, remove the block markers.
	wxPoint pt(event.GetPosition());
	wxPoint point = ScreenToClient(::wxGetMousePosition());
	int num = m_btnRectArray.GetCount();
	m_curClickBtn = -1;
	for (int i=0; i<num; ++i)
	{
		wxRect btn_rc = m_btnRectArray.Item(i);
		if (btn_rc.Contains(point))
		{
			wxCommandEvent *evt = new wxCommandEvent(wxEVT_BUTTON, i+1);
			evt->SetInt(i+1);
			wxQueueEvent(this, evt);
			m_curClickBtn = i;
		}
	}
	if (m_curClickBtn >= 0)
		Refresh();
	event.Skip();
	////@end wxEVT_LEFT_DOWN event handler for ID_TEST in test. 
}

void twPGMultiButton::OnMultiButtonClick(wxCommandEvent& evt)
{
	int id = evt.GetInt();
	if (id > 0 && id <= m_btnCmds.GetCount())
	{
		wxString cmd;
		cmd << "lsppframe " << m_btnCmds.Item(id-1);
		SendCommand(cmd.ToAscii());
	}
}

twPGMultiButton::twPGMultiButton(wxArrayString btns, wxArrayString cmds, wxPropertyGrid* pg, 
	const wxRect& rc, const wxPGProperty* property)
{
	if(btns.GetCount() != cmds.GetCount())
	{
		wxMessageBox("Number of buttons is not equal to number of commands!");
		return;
	}

	wxWindow::Create(pg->GetPanel(), ID_MULTIBTN_TEST_BEGIN, wxPoint(rc.GetX(), rc.GetY()), wxSize(rc.GetWidth(), rc.GetHeight()-2),
		wxBORDER_NONE);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetBackgroundColour(pg->GetCellBackgroundColour());

	m_btnLabels = btns;
	m_btnCmds = cmds;
	m_refProperty = (twMultiButtonProperty *)property;

	int x = 0, y = 0;
	int gap = 5;

	for (int i=0; i<btns.GetCount(); ++i)
	{
		x += gap;
		wxRect btn_rc = wxRect(wxPoint(x+wxPG_XBEFORETEXT, y), wxSize(m_refProperty->m_btnW-wxPG_XBEFORETEXT, m_refProperty->m_btnH));
		x += m_refProperty->m_btnW;
		wxPoint point = ScreenToClient(::wxGetMousePosition());
		if (btn_rc.Contains(point))
		{
			wxCommandEvent *evt = new wxCommandEvent(wxEVT_BUTTON, i+1);
			evt->SetInt(i+1);
			wxQueueEvent(this, evt);
			m_curClickBtn = i;
		}
	}
}

twPGMultiButton::~twPGMultiButton()
{
}

void twPGMultiButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxAutoBufferedPaintDC dc(this);
	wxPoint pt = GetPosition();
	wxPoint pos(m_refProperty->m_myRect.GetPosition());
	if(pt != pos)
		Move(pos);
	dc.Clear();
	int btnNum = m_btnLabels.size();
	int x = 0, y = 0;
	int gap = 5;
	wxSize sz = GetSize();
	wxPGCell& cell = m_refProperty->GetCell(0);
	wxColour rowBgCol = cell.GetBgCol();
	wxColour rowForeCol = cell.GetFgCol();
	wxBrush rowBgBrush = wxBrush(rowBgCol);
	dc.SetBrush(rowBgBrush);
	dc.SetPen(rowBgCol);
	dc.DrawRectangle(0, 0, sz.GetWidth(), sz.GetHeight());
	dc.SetTextForeground(rowForeCol);
	m_btnRectArray.Clear();
	for (int i=0; i<btnNum; ++i)
	{
		x += gap;
		wxRect btn_rc = wxRect(wxPoint(x, y), wxSize(m_refProperty->m_btnW, m_refProperty->m_btnH));
		int state = 0;
		if (m_curClickBtn == i)
			state |= wxSCB_STATE_BOLD;
		DrawSimpleButton(this, dc, btn_rc, m_refProperty->m_btnH, state, m_btnLabels.Item(i));
		x += m_refProperty->m_btnW;
		btn_rc.x += wxPG_XBEFORETEXT;
		btn_rc.width -= wxPG_XBEFORETEXT;
		m_btnRectArray.Add(btn_rc);
	}
}

bool  twMultiButtonRender::Render( wxDC& dc,const wxRect& rect,const wxPropertyGrid* propertyGrid,wxPGProperty* property,int column,int item,int flags ) const
{
	wxColour editorBgCol(255, 255, 255);
	dc.SetBrush(editorBgCol);
	dc.SetPen(editorBgCol);
	twMultiButtonProperty *p = (twMultiButtonProperty*)property;
	wxSize sz, sz1, sz2;
	sz = propertyGrid->GetClientSize();
	wxRect rc = propertyGrid->GetClientRect();
	int gap = 5;
	int btnnum = p->m_btnLabels.GetCount();
	int btn_w = (sz.x-propertyGrid->GetMarginWidth() - (btnnum+1)*gap)/btnnum;
	int btn_h = rect.height;
	int x = rc.GetX()+propertyGrid->GetMarginWidth(), y = rect.GetY();
	wxPGCell& cell = p->GetCell(0);
	wxColour rowBgCol = cell.GetBgCol();
	wxColour rowForeCol = cell.GetFgCol();
	wxBrush rowBgBrush = wxBrush(rowBgCol);
	dc.SetBrush(rowBgBrush);
	dc.SetPen(rowBgCol);
	p->m_btnW = btn_w;
	p->m_btnH = btn_h;
	p->m_myRect.SetPosition(wxPoint(x, y));
	p->m_myRect.SetSize(wxSize(rc.GetWidth()+propertyGrid->GetMarginWidth(), propertyGrid->GetRowHeight()));
	dc.DrawRectangle(p->m_myRect);
	dc.SetPen(editorBgCol);
	dc.SetTextForeground(rowForeCol);

	for (int i=0; i<btnnum; ++i)
	{
		x += gap;
		wxRect btn_rc = wxRect(wxPoint(x, y), wxSize(btn_w, btn_h));
		DrawSimpleButton((wxWindow*)propertyGrid, dc, btn_rc, btn_h, 0, p->m_btnLabels.Item(i));
		x += btn_w;
	}
	return true;
}
*/

#endif