/////////////////////////////////////////////////////////////////////////////
// Name:        treelistctrl.h
// Purpose:     wxTreeListCtrl class
// Author:      Robert Roebling
// Maintainer:  Otto Wyss
// Created:     01/02/97
// Modified:    08/31/06
// RCS-ID:      $Id: treelistctrl.h,v 1.32 2005/10/06 19:31:30 wyo Exp $
// Copyright:   (c) 2004 Robert Roebling, Julian Smart, Alberto Griggio,
//              Vadim Zeitlin, Otto Wyss , Guru Kathiresan
// Licence:     wxWindows
/////////////////////////////////////////////////////////////////////////////


#ifndef TREELISTCTRL_H
#define TREELISTCTRL_H

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "treelistctrl.h"
#endif

#include "wx/treectrl.h"
#include "wx/control.h"
#include "wx/pen.h"
#include "wx/listctrl.h" // for wxListEvent
#include "wx/dynarray.h"
#include "wx/arrimpl.cpp"

class WXDLLEXPORT wxTreeListItem;
class WXDLLEXPORT wxTreeListHeaderWindow;
class WXDLLEXPORT wxTreeListMainWindow;

WX_DECLARE_OBJARRAY(wxTreeItemId, TreeItemIdArray);

/*
#define wxTR_HAS_BUTTONS             0x0001     // draw collapsed/expanded btns
#define wxTR_NO_LINES                0x0004     // don't draw lines at all
#define wxTR_LINES_AT_ROOT           0x0008     // connect top-level nodes
#define wxTR_TWIST_BUTTONS           0x0010     // still used by wxTreeListCtrl

#define wxTR_MULTIPLE                0x0020     // can select multiple items
#define wxTR_EXTENDED                0x0040     // TODO: allow extended selection
#define wxTR_HAS_VARIABLE_ROW_HEIGHT 0x0080     // what it says

#define wxTR_EDIT_LABELS             0x0200     // can edit item labels
#define wxTR_ROW_LINES               0x0400     // put border around items
#define wxTR_HIDE_ROOT               0x0800     // don't display root node

#define wxTR_FULL_ROW_HIGHLIGHT      0x2000     // highlight full horz space
*/
#define wxTR_VIRTUAL    			 0x4000
#define wxTR_VRULE    			     0x8000
#define wxTR_HRULE    			     wxTR_ROW_LINES
#define wxTR_COLUMN_LINES    		 wxTR_VRULE	// compatability with Otto's Version
#define wxTR_SHOW_ROOT_LABEL_ONLY    0x0002

#define wxTR_COLUMN_NONE			0
#define wxTR_COLUMN_TEXT			1
#define wxTR_COLUMN_INT_TEXT		2
#define wxTR_COLUMN_FLOAT_TEXT		3
#define wxTR_COLUMN_COMBO			4
#define wxTR_COLUMN_DATEPICK		5
#define wxTR_COLUMN_SPIN			6
#define wxTR_COLUMN_BMPBTM			7
#define wxTR_COLUMN_IMAGE			8

// Using this typedef removes an ambiguity when calling Remove()
#ifdef __WXMSW__
#if !wxCHECK_VERSION(2, 5, 0)
typedef long wxTreeItemIdValue;
#else
typedef void *wxTreeItemIdValue;
#endif
#endif

//-----------------------------------------------------------------------------
// wxTreeListColumnAttrs
//-----------------------------------------------------------------------------

enum {
    DEFAULT_COL_WIDTH = 100
};

enum wxTreeListItemType
{
  wxItemTypeFirst = 0,
  wxNormalItemType = 0,
  wxCheckboxItemType = 1,
  wxEditBoxItemType = 2,
  wxItemTypeLast = 2
  //wxItemTypeLast = 2
};

class WXDLLEXPORT wxTreeListColumnInfo: public wxObject {
public:
    wxTreeListColumnInfo (const wxString &text = wxEmptyString,
                          int width = DEFAULT_COL_WIDTH,
                          int flag = wxALIGN_LEFT,
                          int image = -1,
                          bool shown = true,
                          bool edit = false,
						  int pick_type = wxTR_COLUMN_NONE) {
        m_text = text;
        m_width = width;
        m_flag = flag;
        m_image = image;
        m_selected_image = -1;
        m_shown = shown;
        m_edit = edit;
        m_pick_type = pick_type;
    }

    wxTreeListColumnInfo (const wxTreeListColumnInfo& other) {
        m_text = other.m_text;
        m_width = other.m_width;
        m_flag = other.m_flag;
        m_image = other.m_image;
        m_selected_image = other.m_selected_image;
        m_shown = other.m_shown;
        m_edit = other.m_edit;
		m_pick_type = other.m_pick_type;
    }

    ~wxTreeListColumnInfo() {}

    // get/set
    wxString GetText() const { return m_text; }
    wxTreeListColumnInfo& SetText (const wxString& text) { m_text = text; return *this; }

    int GetWidth() const { return m_width; }
    wxTreeListColumnInfo& SetWidth (int width) { m_width = width; return *this; }

    int GetAlignment() const { return m_flag; }
    wxTreeListColumnInfo& SetAlignment (int flag) { m_flag = flag; return *this; }

    int GetImage() const { return m_image; }
    wxTreeListColumnInfo& SetImage (int image) { m_image = image; return *this; }

    int GetSelectedImage() const { return m_selected_image; }
    wxTreeListColumnInfo& SetSelectedImage (int image) { m_selected_image = image; return *this; }

    bool IsEditable() const { return m_edit; }
    wxTreeListColumnInfo& SetEditable (bool edit)
        { m_edit = edit; return *this; }

    bool IsShown() const { return m_shown; }
    wxTreeListColumnInfo& SetShown(bool shown) { m_shown = shown; return *this; }

	int GetPickType(void) const { return m_pick_type;}
	wxTreeListColumnInfo& SetPickType(int pick_type) { m_pick_type = pick_type; return *this; }

private:
    wxString m_text;
    int m_width;
    int m_flag;
    int m_image;
    int m_selected_image;
    bool m_shown;
    bool m_edit;
    int m_pick_type;
};

//----------------------------------------------------------------------------
// wxTreeListCtrl - the multicolumn tree control
//----------------------------------------------------------------------------

// modes for navigation
const int wxTL_MODE_NAV_FULLTREE = 0x0000; // default
const int wxTL_MODE_NAV_EXPANDED = 0x0001;
const int wxTL_MODE_NAV_VISIBLE  = 0x0002;
const int wxTL_MODE_NAV_LEVEL    = 0x0004;

// modes for FindItem
const int wxTL_MODE_FIND_EXACT   = 0x0000; // default
const int wxTL_MODE_FIND_PARTIAL = 0x0010;
const int wxTL_MODE_FIND_NOCASE  = 0x0020;

// additional flag for HitTest
const int wxTREE_HITTEST_ONITEMCOLUMN = 0x2000;
extern WXDLLEXPORT const wxChar* wxTreeListCtrlNameStr;


class WXDLLEXPORT wxTreeListCtrl : public wxPanel
{
public:
    // creation
    // --------
    wxTreeListCtrl()
        : m_header_win(0), m_main_win(0), m_headerHeight(0)
    {}

    wxTreeListCtrl(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_DEFAULT_STYLE,
               const wxValidator &validator = wxDefaultValidator,
               const wxString& name = wxTreeListCtrlNameStr )
        : m_header_win(0), m_main_win(0), m_headerHeight(0)
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    virtual ~wxTreeListCtrl() {}

    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_DEFAULT_STYLE,
                const wxValidator &validator = wxDefaultValidator,
                const wxString& name = wxTreeListCtrlNameStr );

    void Refresh(bool erase=TRUE, const wxRect* rect=NULL);
    void SetFocus();
    void OnKillFocus( wxFocusEvent &event );

    // accessors
    // ---------

    // get the total number of items in the control
    size_t GetCount() const;

    // indent is the number of pixels the children are indented relative to
    // the parents position. SetIndent() also redraws the control
    // immediately.
    unsigned int GetIndent() const;
    void SetIndent(unsigned int indent);

    // line spacing is the space above and below the text on each line
    unsigned int GetLineSpacing() const;
    void SetLineSpacing(unsigned int spacing);

    // image list: these functions allow to associate an image list with
    // the control and retrieve it. Note that when assigned with
    // SetImageList, the control does _not_ delete
    // the associated image list when it's deleted in order to allow image
    // lists to be shared between different controls. If you use
    // AssignImageList, the control _does_ delete the image list.
    //
    // The normal image list is for the icons which correspond to the
    // normal tree item state (whether it is selected or not).
    // Additionally, the application might choose to show a state icon
    // which corresponds to an app-defined item state (for example,
    // checked/unchecked) which are taken from the state image list.
    wxImageList *GetImageList() const;
    wxImageList *GetStateImageList() const;
    wxImageList *GetButtonsImageList() const;

    void SetImageList(wxImageList *imageList);
    void SetStateImageList(wxImageList *imageList);
    void SetButtonsImageList(wxImageList *imageList);
    void AssignImageList(wxImageList *imageList);
    void AssignStateImageList(wxImageList *imageList);
    void AssignButtonsImageList(wxImageList *imageList);


    // Functions to work with columns

    // adds a column
    void AddColumn (const wxString& text,
                    int width = DEFAULT_COL_WIDTH,
                    int flag = wxALIGN_LEFT,
                    int image = -1,
                    bool shown = true,
                    bool edit = false) {
        AddColumn (wxTreeListColumnInfo (text, width, flag, image, shown, edit));
    }
    void AddColumn (const wxTreeListColumnInfo& colInfo);

    // inserts a column before the given one
    void InsertColumn (int before,
                       const wxString& text,
                       int width = DEFAULT_COL_WIDTH,
                       int flag = wxALIGN_LEFT,
                       int image = -1,
                       bool shown = true,
                       bool edit = false) {
        InsertColumn (before,
                      wxTreeListColumnInfo (text, width, flag, image, shown, edit));
    }
    void InsertColumn (int before, const wxTreeListColumnInfo& colInfo);

    // deletes the given column - does not delete the corresponding column
    void RemoveColumn (int column);

    // returns the number of columns in the ctrl
    int GetColumnCount() const;

    // tells which column is the "main" one, i.e. the "threaded" one
    void SetMainColumn (int column);
    int GetMainColumn() const;

    /*added by Li Zhangsheng*/
    int GetCurColumn() const;
    bool SetSortColumn(int column) ;
    void SetSortPara() ;
	void SetItemHilight(const wxTreeItemId& item, bool hilight = true);

    void SetColumn (int column, const wxTreeListColumnInfo& colInfo);
    wxTreeListColumnInfo& GetColumn (int column);
    const wxTreeListColumnInfo& GetColumn (int column) const;

    void SetColumnText (int column, const wxString& text);
    wxString GetColumnText (int column) const;

    void SetColumnWidth (int column, int width);
    int GetColumnWidth (int column) const;

    void SetColumnAlignment (int column, int flag);
    int GetColumnAlignment (int column) const;

    void SetColumnImage (int column, int image);
    int GetColumnImage (int column) const;

    void SetColumnShown (int column, bool shown = true);
    bool IsColumnShown (int column) const;

    void SetColumnEditable (int column, bool edit = true);
    bool IsColumnEditable (int column) const;

	void SetColumnPickType (int column, int pick_type);
	int GetColumnPickType (int column) const;

    void SetItemType (const wxTreeItemId& item, const int column, const wxTreeListItemType itemtype, bool checked = false);
    wxTreeListItemType GetItemType (const wxTreeItemId& item, const int column);

    // Functions to work with items.

    // accessors
    // ---------

    // retrieve item's label (of the main column)
    wxString GetItemText (const wxTreeItemId& item) const
        { return GetItemText (item, GetMainColumn()); }
    // retrieves item's label of the given column
    wxString GetItemText (const wxTreeItemId& item, int column) const;

    // get one of the images associated with the item (normal by default)
    int GetItemImage (const wxTreeItemId& item,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal) const
    { return GetItemImage (item, GetMainColumn(), which); }
    int GetItemImage (const wxTreeItemId& item, int column,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal) const;

    // get the data associated with the item
    wxTreeItemData *GetItemData (const wxTreeItemId& item) const;

    bool GetItemBold (const wxTreeItemId& item) const;
    wxColour GetItemTextColour (const wxTreeItemId& item) const;
    wxColour GetItemBackgroundColour (const wxTreeItemId& item) const;
    wxFont GetItemFont (const wxTreeItemId& item) const;
	int GetItemHeight(const wxTreeItemId& item);

    bool GetItemChecked (const wxTreeItemId& itemId, const int column);
    void SetItemChecked (const wxTreeItemId& item, const int column, bool checked);
    wxTreeItemId GetCurrentItem();
	void SetCurrentItem(wxTreeItemId item);
    bool SetItemForPartOrGeom(const wxTreeItemId& item, int type);
    int GetItemForPartOrGeom(const wxTreeItemId& item);
    void SetItemShown(const wxTreeItemId& item, bool bShow);
    bool GetItemShown(const wxTreeItemId& item);
    void SetAllShown(bool bShown);
    void SetItemDisabled(const wxTreeItemId& item, bool bDisable = true);
    bool IsItemDisabled(const wxTreeItemId& item);
	void SetItemDisabled(const wxTreeItemId& item, int column, bool bDisable = true);
	bool GetItemDisabled(const wxTreeItemId& item, int column);
	void SetComboxStringArray(wxArrayString stringArray);
	void ClearComboxStringArray();
    void SetEndDragFlag();

    // modifiers

    // set item's label
    void SetItemText (const wxTreeItemId& item, const wxString& text)
        { SetItemText (item, GetMainColumn(), text); }
    void SetItemText (const wxTreeItemId& item, int column, const wxString& text);

    // get one of the images associated with the item (normal by default)
    void SetItemImage (const wxTreeItemId& item, int image,
                       wxTreeItemIcon which = wxTreeItemIcon_Normal)
        { SetItemImage (item, GetMainColumn(), image, which); }
    // the which parameter is ignored for all columns but the main one
    void SetItemImage (const wxTreeItemId& item, int column, int image,
                       wxTreeItemIcon which = wxTreeItemIcon_Normal);

    // associate some data with the item
    void SetItemData (const wxTreeItemId& item, wxTreeItemData *data);

    // force appearance of [+] button near the item. This is useful to
    // allow the user to expand the items which don't have any children now
    // - but instead add them only when needed, thus minimizing memory
    // usage and loading time.
    void SetItemHasChildren(const wxTreeItemId& item, bool has = true);

    // the item will be shown in bold
    void SetItemBold (const wxTreeItemId& item, bool bold = true);

    // set the item's text colour
    void SetItemTextColour (const wxTreeItemId& item, const wxColour& colour);
    void SetItemTextColour (const wxTreeItemId& item, const wxColour& colour, const int& column);
    wxColor GetItemTextColour (const wxTreeItemId& item, const int& column);

    // set the item's background colour
    void SetItemBackgroundColour (const wxTreeItemId& item, const wxColour& colour);

    // set the item's font (should be of the same height for all items)
    void SetItemFont (const wxTreeItemId& item, const wxFont& font);

    // set the window font
    virtual bool SetFont ( const wxFont &font );

	virtual bool SetFont ( const wxFont &headerwin_font, const wxFont &mainwin_font);

    // set the styles.
    void SetWindowStyle (const long styles);
    long GetWindowStyle() const;
    long GetWindowStyleFlag () const { return GetWindowStyle(); }

    // item status inquiries
    // ---------------------

    // is the item visible (it might be outside the view or not expanded)?
    bool IsVisible (const wxTreeItemId& item, bool fullRow = false) const;
    // does the item has any children?
    bool HasChildren (const wxTreeItemId& item) const;
    // is the item expanded (only makes sense if HasChildren())?
    bool IsExpanded (const wxTreeItemId& item) const;
    // is this item currently selected (the same as has focus)?
    bool IsSelected (const wxTreeItemId& item) const;
    // is item text in bold font?
    bool IsBold (const wxTreeItemId& item) const;
    // does the layout include space for a button?

    // number of children
    // ------------------

    // if 'recursively' is FALSE, only immediate children count, otherwise
    // the returned number is the number of all items in this branch
    size_t GetChildrenCount (const wxTreeItemId& item, bool recursively = true);

    // navigation
    // ----------

    // wxTreeItemId.IsOk() will return FALSE if there is no such item

    // get the root tree item
    wxTreeItemId GetRootItem() const;

    // get the item currently selected (may return NULL if no selection)
    wxTreeItemId GetSelection() const;

    // get the items currently selected, return the number of such item
    size_t GetSelections (wxArrayTreeItemIds&) const;

    // get the parent of this item (may return NULL if root)
    wxTreeItemId GetItemParent (const wxTreeItemId& item) const;

    // for this enumeration function you must pass in a "cookie" parameter
    // which is opaque for the application but is necessary for the library
    // to make these functions reentrant (i.e. allow more than one
    // enumeration on one and the same object simultaneously). Of course,
    // the "cookie" passed to GetFirstChild() and GetNextChild() should be
    // the same!

    // get child of this item
#if !wxCHECK_VERSION(2, 5, 0)
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& cookie) const;
    wxTreeItemId GetNextChild(const wxTreeItemId& item, long& cookie) const;
    wxTreeItemId GetPrevChild(const wxTreeItemId& item, long& cookie) const;
    wxTreeItemId GetLastChild(const wxTreeItemId& item, long& cookie) const;
#else
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    wxTreeItemId GetPrevChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    wxTreeItemId GetLastChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
#endif

    // get sibling of this item
    wxTreeItemId GetNextSibling(const wxTreeItemId& item) const;
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const;

    // get item in the full tree (currently only for internal use)
    wxTreeItemId GetNext(const wxTreeItemId& item) const;
    wxTreeItemId GetPrev(const wxTreeItemId& item) const;

    // get expanded item, see IsExpanded()
    wxTreeItemId GetFirstExpandedItem() const;
    wxTreeItemId GetNextExpanded(const wxTreeItemId& item) const;
    wxTreeItemId GetPrevExpanded(const wxTreeItemId& item) const;

    // get visible item, see IsVisible()
    wxTreeItemId GetFirstVisibleItem(bool fullRow = false) const;
    wxTreeItemId GetNextVisible(const wxTreeItemId& item, bool fullRow = false) const;
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item, bool fullRow = false) const;

    // operations
    // ----------

    // add the root node to the tree
    wxTreeItemId AddRoot (const wxString& text,
                          wxTreeListItemType ctType = wxNormalItemType,
                          int image = -1, int selectedImage = -1,
                          wxTreeItemData *data = NULL);

    // insert a new item in as the first child of the parent
    wxTreeItemId PrependItem (const wxTreeItemId& parent,
                              const wxString& text,
                              wxTreeListItemType ctType = wxNormalItemType,
                              int image = -1, int selectedImage = -1,
                              wxTreeItemData *data = NULL);

    // insert a new item after a given one
    wxTreeItemId InsertItem (const wxTreeItemId& parent,
                             const wxTreeItemId& idPrevious,
                             const wxString& text,
                             wxTreeListItemType ctType = wxNormalItemType,
                             int image = -1, int selectedImage = -1,
                             wxTreeItemData *data = NULL);

    // insert a new item before the one with the given index
    wxTreeItemId InsertItem (const wxTreeItemId& parent,
                             size_t index,
                             const wxString& text,
                             wxTreeListItemType ctType = wxNormalItemType,
                             int image = -1, int selectedImage = -1,
                             wxTreeItemData *data = NULL);

    // insert a new item in as the last child of the parent
    wxTreeItemId AppendItem (const wxTreeItemId& parent,
                             const wxString& text,
                             wxTreeListItemType ctType = wxNormalItemType,
                             int image = -1, int selectedImage = -1,
                             wxTreeItemData *data = NULL);

    // delete this item (except root) and associated data if any
    void Delete (const wxTreeItemId& item);
	void PureDelete (const wxTreeItemId& item);
    // delete all children (but don't delete the item itself)
    // NB: this won't send wxEVT_COMMAND_TREE_ITEM_DELETED events
    void DeleteChildren (const wxTreeItemId& item);
    // delete the root and all its children from the tree
    // NB: this won't send wxEVT_COMMAND_TREE_ITEM_DELETED events
    void DeleteRoot();

    // expand this item
    void Expand (const wxTreeItemId& item);
    // expand this item and all subitems recursively
    void ExpandAll (const wxTreeItemId& item);
    // collapse this item and all subitems recursively
    void CollapseAll (const wxTreeItemId& item);
    // collapse the item without removing its children
    void Collapse (const wxTreeItemId& item);
    // collapse the item and remove all children
    void CollapseAndReset(const wxTreeItemId& item); //? TODO ???
    // toggles the current state
    void Toggle (const wxTreeItemId& item);

    // remove the selection from currently selected item (if any)
    void Unselect();
    void UnselectAll();
    // select this item
	bool SelectItem (const wxTreeItemId& item,
		const wxTreeItemId& last = (wxTreeItemId*)NULL,
		bool unselect_others = true);
    // select all items in the expanded tree
    void SelectAll();
    // make sure this item is visible (expanding the parent item and/or
    // scrolling to this item if necessary)
    void EnsureVisible (const wxTreeItemId& item);
    // scroll to this item (but don't expand its parent)
    void ScrollTo (const wxTreeItemId& item);

    // The first function is more portable (because easier to implement
    // on other platforms), but the second one returns some extra info.
    wxTreeItemId HitTest (const wxPoint& point)
        { int flags; int column; return HitTest (point, flags, column); }
    wxTreeItemId HitTest (const wxPoint& point, int& flags)
        { int column; return HitTest (point, flags, column); }
    wxTreeItemId HitTest (const wxPoint& point, int& flags, int& column);

    // get the bounding rectangle of the item (or of its label only)
    bool GetBoundingRect (const wxTreeItemId& item, wxRect& rect,
                          bool textOnly = false) const;

    // Start editing the item label: this (temporarily) replaces the item
    // with a one line edit control. The item will be selected if it hadn't
    // been before.
    void EditLabel (const wxTreeItemId& item)
        { EditLabel (item, GetMainColumn()); }
    // edit item's label of the given column
    void EditLabel (const wxTreeItemId& item, int column);

    // virtual mode
    virtual wxString OnGetItemText( wxTreeItemData* item, long column ) const;

    // sorting
    // this function is called to compare 2 items and should return -1, 0
    // or +1 if the first item is less than, equal to or greater than the
    // second one. The base class version performs alphabetic comparaison
    // of item labels (GetText)
    virtual int OnCompareItems (const wxTreeItemId& item1, const wxTreeItemId& item2, const int itemtype);
    // sort the children of this item using OnCompareItems
    // NB: this function is not reentrant and not MT-safe (FIXME)!
    void SortChildren(const wxTreeItemId& item);

    // searching
    wxTreeItemId FindItem (const wxTreeItemId& item, int column, const wxString& str, int type = 0, int mode = 0);
    void FindAllItemWithCase(const wxTreeItemId& item, int column, const wxString& str, TreeItemIdArray *itemArray, int mode = 0, int type = 0);
    void AdjustMyScrollbars();

    // overridden base class virtuals
    virtual bool SetBackgroundColour (const wxColour& colour);
    virtual bool SetForegroundColour (const wxColour& colour);

    // drop over item
    void SetDragItem (const wxTreeItemId& item = (wxTreeItemId*)NULL);


    wxTreeListHeaderWindow* GetHeaderWindow() const
        { return m_header_win; }

    wxTreeListMainWindow* GetMainWindow() const
        { return m_main_win; }

	void SetHeaderShown(bool bShow);

    virtual wxSize DoGetBestSize() const;
	void SetItemToolTip(const wxTreeItemId& item, const wxString &tip);

	void RefreshItem(const wxTreeItemId& item);
	void UnselectItem(const wxTreeItemId& item);

	void EnableRightSel(bool bEnable);
	void EnableCrossSel(bool bEnable);
	void EnableMultSelDiffParent(bool bEnable);
	void AdjustColumnWidth();

protected:
    // header window, responsible for column visualization and manipulation
    wxTreeListHeaderWindow* m_header_win;

    // main window, the "true" tree ctrl
    wxTreeListMainWindow* m_main_win;

    void CalculateAndSetHeaderHeight();
    void DoHeaderLayout();
    void OnSize(wxSizeEvent& event);

private:
    int m_headerHeight;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTreeListCtrl)
};

class wxHeaderClickEvent : public wxNotifyEvent {
public:
    wxHeaderClickEvent(wxEventType eventType = wxEVT_NULL, int id = 0) : wxNotifyEvent(eventType, id) {

    }
    wxHeaderClickEvent(const wxHeaderClickEvent &clone)
        : wxNotifyEvent(clone)
    {                                       }
    void SetClickedCol(int col) { m_clickedCol = col; }
    int GetClickedCol() { return m_clickedCol; }
public:
    int m_clickedCol;
};



DECLARE_EVENT_TYPE( wxEVT_TREEVIEWHEADER_CLICKED, -1 )

typedef void (wxEvtHandler::*wxTreeViewHeaderEventFunction)(wxHeaderClickEvent&);


#define EVT_TREEVIEWHEADERCLICK(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY(wxEVT_TREEVIEWHEADER_CLICKED, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxTreeViewHeaderEventFunction, & fn ), (wxObject *) NULL ),

class wxTreeListControlEvent : public wxNotifyEvent {
public:
    wxTreeListControlEvent(wxEventType eventType = wxEVT_NULL, int id = 0) : wxNotifyEvent(eventType, id) {

    }
    wxTreeListControlEvent(const wxTreeListControlEvent &clone)
        : wxNotifyEvent(clone)
    {                                       }
    void SetClickedCol(int col) { m_clickedCol = col; }
    int GetClickedCol() { return m_clickedCol; }
    void SetClickedItem(wxTreeItemId item) { m_clickedItem = item; }
#if wxVERSION_NUMBER >= 2900
	wxTreeItemId GetClickedItem() { return m_clickedItem; }
#else
    wxTreeItemId GetClickedItem() { return m_clickedItem; }
#endif
public:
    int m_clickedCol;
    wxTreeItemId m_clickedItem;
};



DECLARE_EVENT_TYPE( wxEVT_TREEVIEWCONTROL_START, -1 )

typedef void (wxEvtHandler::*wxTreeViewControlStartEventFunction)(wxTreeListControlEvent&);


#define EVT_TREEVIEWCONTROLSTART(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY(wxEVT_TREEVIEWCONTROL_START, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxTreeViewControlStartEventFunction, & fn ), (wxObject *) NULL ),

DECLARE_EVENT_TYPE( wxEVT_TREEVIEWCONTROL_DESELECTITEM, -1 )


#define EVT_TREEVIEWCONTROL_DESELECTITEM(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_TREEVIEWCONTROL_DESELECTITEM, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( wxTreeViewControlStartEventFunction, & fn ), (wxObject *) NULL ),

#define wxTreeViewControlStartEventHandler(func) \
	wxEVENT_HANDLER_CAST(wxTreeViewControlStartEventFunction, func)


class wxTreeListCheckBoxEvent : public wxTreeEvent {
public:
	wxTreeListCheckBoxEvent(wxEventType eventType = wxEVT_NULL, int id = 0) : wxTreeEvent(eventType, id) {

	}
	wxTreeListCheckBoxEvent(const wxTreeListCheckBoxEvent &clone)
		: wxTreeEvent(clone)
	{                                       }
	void SetClickedCol(int col) { m_clickedCol = col; }
	int GetClickedCol() { return m_clickedCol; }
public:
	int m_clickedCol;
};



DECLARE_EVENT_TYPE( wxEVT_TREEVIEWCHECKBOX_CLICKED, -1 )

typedef void (wxEvtHandler::*wxTreeViewCheckBoxEventFunction)(wxTreeListCheckBoxEvent&);


#define EVT_TREEVIEWCHECKBOXCLICK(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY(wxEVT_TREEVIEWCHECKBOX_CLICKED, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxTreeEventFunction)\
	wxStaticCastEvent( wxTreeViewCheckBoxEventFunction, & fn ), (wxObject *) NULL ),

#define wxTreeViewCheckBoxEventHandler(func) \
	wxEVENT_HANDLER_CAST(wxTreeViewCheckBoxEventFunction, func)

#endif // TREELISTCTRL_H

