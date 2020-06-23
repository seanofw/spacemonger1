#ifndef TIPWND_H
#define TIPWND_H

/* TipWnd messages */
enum {
	TW_AUTO_SIZE = 101,
	TW_SET_ANCHOR = 102,
	TW_GET_ANCHOR = 103,
	TW_SET_ICONPOS = 104,
	TW_GET_ICONPOS = 105,
	TW_SET_BGCOLOR = 106,
	TW_GET_BGCOLOR = 107,
	TW_SET_TEXTCOLOR = 108,
	TW_GET_TEXTCOLOR = 109,
	TW_SET_BORDERCOLOR = 110,
	TW_GET_BORDERCOLOR = 111,
	TW_SET_AUTOSHOW = 112,
	TW_GET_AUTOSHOW = 113,
	TW_SET_AUTOPOS = 114,
	TW_GET_AUTOPOS = 115,
	TW_SET_SHOWDELAY = 116,
	TW_GET_SHOWDELAY = 117,
	TW_SET_HPADDING = 118,
	TW_GET_HPADDING = 119,
	TW_SET_VPADDING = 120,
	TW_GET_VPADDING = 121,
	TW_GET_OWNER = 122,
	TW_REFLECT_MESSAGE = 199,
};

/* TipWnd message parameters */
enum {
	TW_LEFT = 0,
	TW_TOP = 1,
	TW_RIGHT = 2,
	TW_BOTTOM = 3,

	TW_ALIGN_LEFT = 1,
	TW_ALIGN_TOP = 2,
	TW_ALIGN_RIGHT = 4,
	TW_ALIGN_BOTTOM = 8,

	TW_ALIGN_TOPLEFT = 1|2,
	TW_ALIGN_TOPRIGHT = 2|4,
	TW_ALIGN_BOTTOMLEFT = 1|8,
	TW_ALIGN_BOTTOMRIGHT = 2|8,
};

/***************************************************************************/

/* TipWnd helper macros */
#define TipWnd_AutoSize(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_AUTO_SIZE, 0)
#define TipWnd_SetAnchor(hwnd, anchor) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_ANCHOR, (LPARAM)(anchor))
#define TipWnd_GetAnchor(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_ANCHOR, 0)
#define TipWnd_SetIconPos(hwnd, pos) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_ICONPOS, (LPARAM)(pos))
#define TipWnd_GetIconPos(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_ICONPOS, 0)
#define TipWnd_SetBgColor(hwnd, color) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_BGCOLOR, (LPARAM)(color))
#define TipWnd_GetBgColor(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_BGCOLOR, 0)
#define TipWnd_SetTextColor(hwnd, color) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_TEXTCOLOR, (LPARAM)(color))
#define TipWnd_GetTextColor(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_TEXTCOLOR, 0)
#define TipWnd_SetBorderColor(hwnd, color) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_BORDERCOLOR, (LPARAM)(color))
#define TipWnd_GetBorderColor(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_BORDERCOLOR, 0)
#define TipWnd_SetFont(hwnd, font, redraw) \
	SendMessage(hwnd, WM_SETFONT, (WPARAM)(font), (LPARAM)(redraw))
#define TipWnd_GetFont(hwnd) \
	SendMessage(hwnd, WM_GETFONT, 0, 0)
#define TipWnd_SetText(hwnd, text) \
	SendMessage(hwnd, WM_SETTEXT, 0, (WPARAM)(text))
#define TipWnd_GetText(hwnd, buffer, buflen) \
	SendMessage(hwnd, WM_GETTEXT, (WPARAM)(buffer), (LPARAM)(buflen))
#define TipWnd_GetTextLength(wnd) \
	SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0)
#define TipWnd_SetIcon(hwnd, type, icon) \
	SendMessage(hwnd, WM_SETICON, (WPARAM)(type), (LPARAM)(icon))
#define TipWnd_GetIcon(hwnd) \
	SendMessage(hwnd, WM_GETICON, 0, 0)
#define TipWnd_SetAutoShow(hwnd, enable) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_AUTOSHOW, (LPARAM)(enable))
#define TipWnd_GetAutoShow(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_AUTOSHOW, 0)
#define TipWnd_SetShowDelay(hwnd, delay) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_SHOWDELAY, (LPARAM)(delay))
#define TipWnd_GetShowDelay(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_SHOWDELAY, 0)
#define TipWnd_ReflectMessage(hwnd, pmsg) \
	SendMessage(hwnd, WM_COMMAND, TW_REFLECT_MESSAGE, (LPARAM)(pmsg))
#define TipWnd_SetAutoPos(hwnd, pos) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_AUTOPOS, (LPARAM)(pos))
#define TipWnd_GetAutoPos(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_AUTOPOS, 0)
#define TipWnd_SetHPadding(hwnd, padding) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_HPADDING, (WPARAM)(padding))
#define TipWnd_GetHPadding(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_HPADDING, 0)
#define TipWnd_SetVPadding(hwnd, padding) \
	SendMessage(hwnd, WM_COMMAND, TW_SET_VPADDING, (WPARAM)(padding))
#define TipWnd_GetVPadding(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_VPADDING, 0)
#define TipWnd_GetOwner(hwnd) \
	SendMessage(hwnd, WM_COMMAND, TW_GET_OWNER, 0)

/***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

// TipWnd functions
void WINAPI InitTipWnd(HINSTANCE inst);
HWND WINAPI CreateTipWnd(HWND owner, const RECT *rect, BOOL showwnd);
HWND WINAPI CreateTipWndEz(HWND owner, int x, int y,
	const char *text, HICON icon, BOOL showwnd);

#ifdef __cplusplus
};
#endif

/***************************************************************************/

#ifdef __AFXWIN_H__

/* MFC support class, if they're using MFC.  Like most of the MFC classes,
   it's just a wrapper around the Windows HWND.  You still have to call
   InitTipWnd() somewhere at the beginning of your program, though. */

class CTipWnd : public CWnd {
	DECLARE_DYNAMIC(CTipWnd)

public:
	CTipWnd();
	virtual ~CTipWnd();
	virtual BOOL Create(CWnd *owner, const RECT &rect, BOOL showwnd = 0);
	virtual BOOL CreateEz(CWnd *owner, int x, int y, const CString &text = "",
		HICON icon = NULL, BOOL showwnd = 0);
	void MoveWindow(CWnd *owner, int x, int y);
	void PushOnScreen(void);

protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult);

public:
	inline void AutoSize(void)
		{ SendMessage(WM_COMMAND, TW_AUTO_SIZE); }
	inline void SetAnchor(int anchor)
		{ SendMessage(WM_COMMAND, TW_SET_ANCHOR, (LPARAM)anchor); }
	inline int GetAnchor(void)
		{ return SendMessage(WM_COMMAND, TW_GET_ANCHOR); }
	inline void SetIconPos(int iconpos)
		{ SendMessage(WM_COMMAND, TW_SET_ICONPOS, (LPARAM)iconpos); }
	inline int GetIconPos(void)
		{ return SendMessage(WM_COMMAND, TW_GET_ICONPOS); }
	inline void SetBgColor(COLORREF color)
		{ SendMessage(WM_COMMAND, TW_SET_BGCOLOR, (LPARAM)color); }
	inline int GetBgColor(void)
		{ return SendMessage(WM_COMMAND, TW_GET_BGCOLOR); }
	inline void SetTextColor(COLORREF color)
		{ SendMessage(WM_COMMAND, TW_SET_TEXTCOLOR, (LPARAM)color); }
	inline int GetTextColor(void)
		{ return SendMessage(WM_COMMAND, TW_GET_TEXTCOLOR); }
	inline void SetBorderColor(COLORREF color)
		{ SendMessage(WM_COMMAND, TW_SET_BORDERCOLOR, (LPARAM)color); }
	inline int GetBorderColor(void)
		{ return SendMessage(WM_COMMAND, TW_GET_BORDERCOLOR); }
	inline void SetAutoShow(BOOL bEnable)
		{ SendMessage(WM_COMMAND, TW_SET_AUTOSHOW, (LPARAM)bEnable); }
	inline BOOL GetAutoShow(void)
		{ return (BOOL)SendMessage(WM_COMMAND, TW_GET_AUTOSHOW); }
	inline void SetShowDelay(int delay)
		{ SendMessage(WM_COMMAND, TW_SET_SHOWDELAY, (LPARAM)delay); }
	inline int GetShowDelay(void)
		{ return SendMessage(WM_COMMAND, TW_GET_SHOWDELAY); }
	inline void SetAutoPos(BOOL bAutoPos)
		{ SendMessage(WM_COMMAND, TW_SET_AUTOPOS, (LPARAM)bAutoPos); }
	inline int GetAutoPos(void)
		{ return SendMessage(WM_COMMAND, TW_GET_AUTOPOS); }
	inline void ReflectMessage(const MSG *msg)
		{ SendMessage(WM_COMMAND, TW_REFLECT_MESSAGE, (LPARAM)msg); }
	inline void SetHPadding(int padding)
		{ SendMessage(WM_COMMAND, TW_SET_HPADDING, (LPARAM)padding); }
	inline int GetHPadding(void)
		{ return SendMessage(WM_COMMAND, TW_GET_HPADDING); }
	inline void SetVPadding(int padding)
		{ SendMessage(WM_COMMAND, TW_SET_VPADDING, (LPARAM)padding); }
	inline int GetVPadding(void)
		{ return SendMessage(WM_COMMAND, TW_GET_VPADDING); }
	inline CWnd *GetTipWndOwner(void)
		{ return CWnd::FromHandle((HWND)SendMessage(WM_COMMAND, TW_GET_OWNER)); }
};

#endif

#endif
