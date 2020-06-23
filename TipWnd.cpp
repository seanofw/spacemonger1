
#include "stdafx.h"
#include "SpaceMonger.h"
#include "TipWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/***************************************************************************/

#define CLASSNAME "TipWnd"

struct TipWndInfo {
	HICON icon;
	int flags;
	char *text;
	int textlen;
	HFONT font;
	COLORREF bgcolor;
	COLORREF textcolor;
	COLORREF bordercolor;
	short int vpadding, hpadding;
	unsigned char autoshow;
	unsigned char autopos;
	short int showdelay;
	UINT timer, hidetimer;
	HWND owner;
	int mousex, mousey;
	BOOL hidden;
	BOOL enabled;
	int ourmousex, ourmousey;
};

enum {
	TIPWND_LARGE_ICON = 1,

	TIPWND_ICON_LEFT = 0,
	TIPWND_ICON_TOP = 2,
	TIPWND_ICON_RIGHT = 4,
	TIPWND_ICON_BOTTOM = 6,

	TIPWND_ANCHOR_LEFT = 8,
	TIPWND_ANCHOR_TOP = 16,
	TIPWND_ANCHOR_RIGHT = 32,
	TIPWND_ANCHOR_BOTTOM = 64,

	TIPWND_ANCHOR_TOPLEFT = 8|16,
	TIPWND_ANCHOR_TOPRIGHT = 32|16,
	TIPWND_ANCHOR_BOTTOMLEFT = 8|64,
	TIPWND_ANCHOR_BOTTOMRIGHT = 32|64,

	TIPWND_ALIGN_LEFT = 128,
	TIPWND_ALIGN_TOP = 256,
	TIPWND_ALIGN_RIGHT = 512,
	TIPWND_ALIGN_BOTTOM = 1024,

	TIPWND_ALIGN_TOPLEFT = 128|256,
	TIPWND_ALIGN_TOPRIGHT = 512|256,
	TIPWND_ALIGN_BOTTOMLEFT = 128|1024,
	TIPWND_ALIGN_BOTTOMRIGHT = 512|1024,
};

/***************************************************************************/

static HFONT SetBestFont(HDC dc, TipWndInfo *info)
{
	if (info->font != NULL)
		return (HFONT)::SelectObject(dc, info->font);
	else return (HFONT)::SelectObject(dc, ::GetStockObject(DEFAULT_GUI_FONT));
}

static void ComputeTextExtents(HDC dc, TipWndInfo *info, SIZE *size)
{
	// Scan the text to determine the longest line (in pixels) and the
	// total height in pixels.
	const char *start, *end;
	int maxlen = 0, height = 0;
	SIZE linesize;
	TEXTMETRIC tm;
	::GetTextMetrics(dc, &tm);

	start = info->text;
	if (start != NULL) {
		while (*start != '\0') {
			end = start;
			while (*end != '\0' && *end != '\n') end++;
			linesize.cx = linesize.cy = 0;
			::GetTextExtentPoint32(dc, start, end-start, &linesize);
			height += tm.tmAscent + tm.tmDescent;
			if (linesize.cx > maxlen) maxlen = linesize.cx;
			if (*end == '\n') end++;
			start = end;
		}
	}
	size->cx = maxlen;
	size->cy = height;
}

static void DrawMultilineText(HDC dc, TipWndInfo *info, POINT *point)
{
	::SetTextColor(dc, info->textcolor);

	// Scan the text to determine the longest line (in pixels) and the
	// total height in pixels.
	const char *start, *end;
	SIZE linesize;
	TEXTMETRIC tm;
	::GetTextMetrics(dc, &tm);

	start = info->text;
	if (start != NULL) {
		while (*start != '\0') {
			end = start;
			while (*end != '\0' && *end != '\n') end++;
			linesize.cx = linesize.cy = 0;
			::TextOut(dc, point->x, point->y, start, end-start);
			point->y += tm.tmAscent + tm.tmDescent;
			if (*end == '\n') end++;
			start = end;
		}
	}
}

static LRESULT CALLBACK TipWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TipWndInfo *info = (TipWndInfo *)::GetWindowLong(hwnd, GWL_USERDATA);
	if (info == NULL && uMsg != WM_CREATE)
		return ::DefWindowProc(hwnd, uMsg, wParam, lParam);

	switch (uMsg) {

	case WM_CREATE:
		info = (TipWndInfo *)malloc(sizeof(TipWndInfo));
		info->icon = NULL;
		info->text = NULL;
		info->textlen = 0;
		info->flags = TIPWND_ANCHOR_TOPLEFT|TIPWND_ALIGN_LEFT;
		info->font = NULL;
		info->bgcolor = GetSysColor(COLOR_INFOBK);
		info->bordercolor = RGB(0,0,0);
		info->textcolor = GetSysColor(COLOR_INFOTEXT);
		info->hpadding = 2;
		info->vpadding = 2;
		info->autoshow = 0;
		info->autopos = 0;
		info->showdelay = 1000;
		info->owner = NULL;
		info->mousex = info->mousey = -32768;
		info->hidden = 0;
		info->enabled = 1;
		info->ourmousex = info->ourmousey = -32768;
		::SetWindowLong(hwnd, GWL_USERDATA, (LONG)info);
		return 0;

	case WM_DESTROY:
		if (info != NULL) {
			if (info->font != NULL) ::DeleteObject(info->font);
			if (info->text != NULL) free(info->text);
			free(info);
		}
		return 0;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC dc = ::BeginPaint(hwnd, &ps);

			RECT rect;
			::GetClientRect(hwnd, &rect);
			rect.right -= rect.left;
			rect.bottom -= rect.top;
			rect.top = 0;
			rect.left = 0;

			::SetBkMode(dc, TRANSPARENT);
			::SetTextAlign(dc, TA_TOP|TA_LEFT);

			HFONT font = SetBestFont(dc, info);

			SIZE textsize;
			ComputeTextExtents(dc, info, &textsize);
			POINT textpoint;

			SIZE pad;
			pad.cx = pad.cy = 0;
			SIZE iconoffset;
			iconoffset.cx = iconoffset.cy = 0;
			if (info->icon != NULL) {
				int delta = (info->flags & 1) ? 32 : 16;
				switch (info->flags & 6) {
				case TIPWND_ICON_LEFT:
					pad.cx = delta + 4;
					iconoffset.cx = 0;
					textsize.cx += delta + 4;
					if (textsize.cy < delta)
						pad.cy = (delta-textsize.cy)/2, textsize.cy = delta;
					break;
				case TIPWND_ICON_RIGHT:
					pad.cx = 0;
					iconoffset.cx = textsize.cx + 4;
					textsize.cx += delta + 4;
					if (textsize.cy < delta)
						pad.cy = (delta-textsize.cy)/2, textsize.cy = delta;
					break;
				case TIPWND_ICON_TOP:
					pad.cy = delta + 2;
					iconoffset.cx = 0;
					textsize.cy += delta + 2;
					if (textsize.cx < delta)
						pad.cx = (delta-textsize.cx)/2, textsize.cx = delta;
					break;
				case TIPWND_ICON_BOTTOM:
					pad.cy = 0;
					iconoffset.cy = textsize.cy + 2;
					textsize.cy += delta + 2;
					if (textsize.cx < delta)
						pad.cx = (delta-textsize.cx)/2, textsize.cx = delta;
					break;
				}
			}

			if ((info->flags & (TIPWND_ALIGN_LEFT|TIPWND_ALIGN_RIGHT)) == TIPWND_ALIGN_LEFT)
				textpoint.x = 1+info->hpadding;
			else if ((info->flags & (TIPWND_ALIGN_LEFT|TIPWND_ALIGN_RIGHT)) == TIPWND_ALIGN_RIGHT)
				textpoint.x = rect.right - 1 - info->hpadding - textsize.cx;
			else textpoint.x = rect.right / 2 - textsize.cx / 2;

			if ((info->flags & (TIPWND_ALIGN_TOP|TIPWND_ALIGN_BOTTOM)) == TIPWND_ALIGN_TOP)
				textpoint.y = 1+info->vpadding;
			else if ((info->flags & (TIPWND_ALIGN_TOP|TIPWND_ALIGN_BOTTOM)) == TIPWND_ALIGN_BOTTOM)
				textpoint.y = rect.bottom - 1 - info->vpadding - textsize.cy;
			else textpoint.y = rect.bottom / 2 - textsize.cy / 2;

			if (info->icon != NULL)
				::DrawIcon(dc, textpoint.x+iconoffset.cx,
					textpoint.y+iconoffset.cy, info->icon);
			textpoint.x += pad.cx;
			textpoint.y += pad.cy;
			DrawMultilineText(dc, info, &textpoint);

			::SelectObject(dc, font);
			::EndPaint(hwnd, &ps);
			return 0;
		}

	case WM_ERASEBKGND:
		{
			HDC dc = (HDC)wParam;
			
			RECT rect;
			::GetClientRect(hwnd, &rect);
			rect.right -= rect.left;
			rect.bottom -= rect.top;
			rect.top = 0;
			rect.left = 0;

			HBRUSH bgcolor = ::CreateSolidBrush(info->bgcolor);
			HBRUSH border = ::CreateSolidBrush(info->bordercolor);
			::FrameRect(dc, &rect, border);
			rect.top++, rect.left++;
			rect.right--, rect.bottom--;
			::FillRect(dc, &rect, bgcolor);
			::DeleteObject(bgcolor);
			::DeleteObject(border);
		}
		return 1;

	case WM_SETICON:
		if (info->icon == (HICON)lParam) return 0;
		info->icon = (HICON)lParam;
		info->flags &= ~TIPWND_LARGE_ICON;
		if (wParam == ICON_BIG) info->flags |= TIPWND_LARGE_ICON;
		::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
		return 0;

	case WM_GETICON:
		return (LRESULT)info->icon;

	case WM_CLEAR:
		if (info->text == NULL) return 0;
		free(info->text);
		info->textlen = 0;
		info->text = NULL;
		::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
		return 0;

	case WM_SETTEXT:
		if ((const char *)lParam == NULL) lParam = (LPARAM)"<null>";
		if (info->text != NULL) {
			if (!strcmp(info->text, (const char *)lParam)) return 0;
			free(info->text);
		}
		info->textlen = strlen((const char *)lParam);
		info->text = (char *)malloc(info->textlen+1);
		strcpy(info->text, (const char *)lParam);
		::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
		return 0;

	case WM_GETTEXT:
		if (lParam != NULL) {
			strncpy((char *)lParam, info->text, (int)wParam);
			((char *)lParam)[wParam-1] = '\0';
			if ((int)wParam > (int)info->textlen) return (LRESULT)info->textlen;
			else return (LRESULT)wParam;
		}
		return 0;

	case WM_GETTEXTLENGTH:
		return (LRESULT)info->textlen;

	case WM_SETFONT:
		if (info->font != NULL) ::DeleteObject(info->font);
		info->font = (HFONT)wParam;
		if (lParam != 0) ::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
		return 0;

	case WM_GETFONT:
		return (LRESULT)info->font;

	case WM_ENABLE:
		if (wParam) {
			if (!info->enabled) {
				info->enabled = 1;
				if (info->autoshow) {
					if (info->timer == 0)
						info->timer = ::SetTimer(hwnd, 1, info->showdelay, NULL);
					if (info->hidetimer == 0)
						info->hidetimer = ::SetTimer(hwnd, 2, 250, NULL);
				}
			}
		}
		else {
			if (info->enabled) {
				info->enabled = 0;
				if (info->timer != 0) ::KillTimer(hwnd, 1);
				if (info->hidetimer != 0) ::KillTimer(hwnd, 2);
				info->timer = 0;
				info->hidetimer = 0;
				if (info->autoshow) {
					info->mousex = info->mousey = -32768;
					info->ourmousex = info->ourmousey = -32768;
					if (!info->hidden) info->hidden = 1, ::ShowWindow(hwnd, SW_HIDE);
				}
			}
		}
		return 0;

	case WM_COMMAND:
		switch (wParam) {

		case TW_AUTO_SIZE:
			{
				SIZE size;
				HDC dc = ::GetDC(hwnd);
				HFONT oldfont = SetBestFont(dc, info);
				ComputeTextExtents(dc, info, &size);
				::SelectObject(dc, oldfont);
				::ReleaseDC(hwnd, dc);
				if (info->icon != NULL) {
					int delta = (info->flags & 1) ? 32 : 16;
					switch (info->flags & 6) {
					case TIPWND_ICON_LEFT:
					case TIPWND_ICON_RIGHT:
						size.cx += delta + info->hpadding*2;
						if (size.cy < delta) size.cy = delta;
						break;
					case TIPWND_ICON_TOP:
					case TIPWND_ICON_BOTTOM:
						size.cy += delta + info->vpadding*2;
						if (size.cx < delta) size.cx = delta;
						break;
					}
				}

				RECT rect;
				::GetWindowRect(hwnd, &rect);
				if (rect.right-rect.left != size.cx+2+info->hpadding*2
					|| rect.bottom-rect.top != size.cy+2+info->vpadding*2)
					::SetWindowPos(hwnd, NULL, rect.left, rect.top,
						size.cx+2+info->hpadding*2, size.cy+2+info->vpadding*2,
						SWP_NOACTIVATE|SWP_NOZORDER);
				else ::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
			}
			return 0;

		case TW_SET_ANCHOR:
			info->flags &= ~(8|16|32|64);
			info->flags |= ((lParam & 15) << 3);
			return 0;

		case TW_GET_ANCHOR:
			return (LRESULT)((info->flags >> 3) & (8|16|32|64));

		case TW_SET_ICONPOS:
			if ((info->flags & 6) != ((lParam & 3) << 1)) {
				info->flags &= ~(2|4);
				info->flags |= ((lParam & 3) << 1);
				::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
			}
			return 0;

		case TW_GET_ICONPOS:
			return (LRESULT)((info->flags >> 1) & 3);

		case TW_SET_BGCOLOR:
			if (lParam != (LPARAM)info->bgcolor) {
				info->bgcolor = lParam;
				::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
			}
			return 0;

		case TW_GET_BGCOLOR:
			return (LRESULT)info->bgcolor;

		case TW_SET_TEXTCOLOR:
			if (lParam != (LPARAM)info->textcolor) {
				info->textcolor = lParam;
				::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
			}
			return 0;

		case TW_GET_TEXTCOLOR:
			return (LRESULT)info->textcolor;

		case TW_SET_BORDERCOLOR:
			if (lParam != (LPARAM)info->bordercolor) {
				info->bordercolor = lParam;
				::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
			}
			return 0;

		case TW_GET_BORDERCOLOR:
			return (LRESULT)info->bordercolor;

		case TW_SET_AUTOSHOW:
			if (info->autoshow != lParam) {
				info->autoshow = (lParam != 0);
				if (info->timer != 0) {
					::KillTimer(hwnd, info->timer);
					info->timer = 0;
				}
				if (info->hidetimer != 0) {
					::KillTimer(hwnd, info->hidetimer);
					info->hidetimer = 0;
				}
				if (info->autoshow) {
					info->timer = ::SetTimer(hwnd, 1, info->showdelay, NULL);
					info->hidetimer = ::SetTimer(hwnd, 2, 250, NULL);
				}
			}
			return 0;

		case TW_GET_AUTOSHOW:
			return info->autoshow;

		case TW_SET_SHOWDELAY:
			if (info->showdelay != lParam) {
				info->showdelay = (short int)lParam;
				if (info->showdelay < 1) info->showdelay = 1;
				if (info->timer != 0) {
					::KillTimer(hwnd, info->timer);
					info->timer = 0;
					info->hidetimer = 0;
				}
				if (info->autoshow) {
					info->timer = ::SetTimer(hwnd, 1, info->showdelay, NULL);
				}
			}
			return 0;

		case TW_GET_SHOWDELAY:
			return info->showdelay;

		case TW_SET_AUTOPOS:
			info->autopos = (lParam != 0);
			return 0;

		case TW_GET_AUTOPOS:
			return info->autopos;

		case TW_SET_HPADDING:
			if (info->hpadding != (int)lParam) {
				info->hpadding = (int)lParam;
				::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
			}
			return 0;

		case TW_GET_HPADDING:
			return info->hpadding;

		case TW_SET_VPADDING:
			if (info->vpadding != (int)lParam) {
				info->vpadding = (int)lParam;
				::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
			}
			return 0;

		case TW_GET_VPADDING:
			return info->vpadding;

		case TW_GET_OWNER:
			return (LRESULT)info->owner;

		case TW_REFLECT_MESSAGE:
			{
				MSG *msg = (MSG *)lParam;
				if (msg->message == WM_LBUTTONDOWN || msg->message == WM_LBUTTONUP
					|| msg->message == WM_RBUTTONDOWN || msg->message == WM_RBUTTONUP
					|| msg->message == WM_MBUTTONDOWN || msg->message == WM_MBUTTONUP
					|| msg->message == WM_MOUSEMOVE || msg->message == WM_KILLFOCUS
					|| msg->message == WM_SETFOCUS || msg->message == WM_KEYDOWN
					|| msg->message == WM_KEYUP) {
					if (msg->message == WM_MOUSEMOVE) {
						if (info->mousex == LOWORD(msg->lParam)
							&& info->mousey == HIWORD(msg->lParam)) return 0;
						info->mousex = LOWORD(msg->lParam);
						info->mousey = HIWORD(msg->lParam);
					}
					if (info->timer != 0 && info->showdelay > 1) {
						::KillTimer(hwnd, info->timer);
						info->timer = 0;
						info->hidetimer = 0;
						info->mousex = info->mousey = -32768;
						info->ourmousex = info->ourmousey = -32768;
						if (!info->hidden) info->hidden = 1, ::ShowWindow(hwnd, SW_HIDE);
					}
					if (info->autoshow && msg->message != WM_KILLFOCUS) {
						info->timer = ::SetTimer(hwnd, 1, info->showdelay, NULL);
					}
				}
			}
			return 0;

		default:
			return 0;
		}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		{
			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			::ClientToScreen(hwnd, &point);
			::ScreenToClient(info->owner, &point);
			::SendMessage(info->owner, uMsg, wParam, MAKELPARAM(point.x, point.y));
		}
		return 0;

	case WM_MOUSEMOVE:
		{
			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			if (point.x != info->ourmousex || point.y != info->ourmousey) {
				info->ourmousex = point.x, info->ourmousey = point.y;
				::ClientToScreen(hwnd, &point);
				::ScreenToClient(info->owner, &point);
				::SendMessage(info->owner, uMsg, wParam, MAKELPARAM(point.x, point.y));
			}
		}
		return 0;

	case WM_TIMER:
		if (wParam == 1) {
			if (info->timer != 0) {
				::KillTimer(hwnd, info->timer);
				info->timer = 0;
				info->hidetimer = 0;
			}
			if (info->autoshow) {
				if (info->autopos) {
					RECT rect;
					POINT point;
					::GetWindowRect(hwnd, &rect);
					::GetCursorPos(&point);
					if (point.x + (rect.right-rect.left) > GetSystemMetrics(SM_CXSCREEN) - 16)
						point.x = GetSystemMetrics(SM_CXSCREEN) - (rect.right-rect.left) - 16;
					if (point.y + (rect.bottom-rect.top) > GetSystemMetrics(SM_CYSCREEN) - 16)
						point.y = GetSystemMetrics(SM_CYSCREEN) - (rect.bottom-rect.top) - 16;
					if (rect.left != point.x+16 || rect.top != point.y+16) {
						::SetWindowPos(hwnd, NULL, point.x+16, point.y+16,
							rect.right-rect.left, rect.bottom-rect.top, SWP_NOACTIVATE|SWP_NOZORDER);
						info->ourmousex = info->ourmousey = -32768;
					}
					else ::RedrawWindow(hwnd, NULL, NULL, RDW_ERASE|RDW_INTERNALPAINT);
				}
				if (::IsWindowEnabled(hwnd)) {
					::BringWindowToTop(hwnd);
					if (info->hidden) info->hidden = 0, ::ShowWindow(hwnd, SW_SHOWNOACTIVATE);
					::UpdateWindow(hwnd);
				}
				if (info->hidetimer == 0)
					info->hidetimer = ::SetTimer(hwnd, 2, 250, NULL);
			}
		}
		else if (wParam == 2) {
			// If the mouse is no longer in the window and we're using autoshow,
			// hide the tip with a quickness.
			POINT point;
			::GetCursorPos(&point);
			HWND wnd = ::WindowFromPoint(point);
			// If the window the mouse is over is a TipWnd, then the mouse is
			// really just over that TipWnd's owner.
			char cname[32];
			::GetClassName(wnd, cname, 31);
			if (!strcmp(cname, CLASSNAME))
				wnd = (HWND)::SendMessage(wnd, WM_COMMAND, TW_GET_OWNER, 0);
			// If the mouse has moved out of this TipWnd's owner, hide the TipWnd.
			if (wnd != info->owner) {
				::KillTimer(hwnd, info->timer);
				::KillTimer(hwnd, info->hidetimer);
				info->timer = 0;
				info->hidetimer = 0;
				info->mousex = info->mousey = -32768;
				info->ourmousex = info->ourmousey = -32768;
				if (!info->hidden) info->hidden = 1, ::ShowWindow(hwnd, SW_HIDE);
			}
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

/***************************************************************************/

static HINSTANCE theInst;

void WINAPI ::InitTipWnd(HINSTANCE inst)
{
	static bool initted = 0;

	if (initted) return;

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(void *);
	wc.hbrBackground = NULL;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hInstance = theInst;
	wc.lpszClassName = CLASSNAME;
	wc.lpszMenuName = NULL;
	wc.style = CS_VREDRAW|CS_HREDRAW|CS_SAVEBITS;
	wc.lpfnWndProc = TipWndProc;

	::RegisterClassEx(&wc);

	initted = 1;
}

HWND WINAPI ::CreateTipWnd(HWND owner, const RECT *rect, BOOL showwnd)
{
	HWND hwnd = ::CreateWindowEx(WS_EX_TOPMOST, CLASSNAME, "", WS_POPUP,
		rect->left, rect->top, rect->right-rect->left, rect->bottom-rect->top,
		owner, NULL, theInst, NULL);
	TipWndInfo *info = (TipWndInfo *)::GetWindowLong(hwnd, GWL_USERDATA);
	if (info != NULL) info->owner = owner;
	return hwnd;
}

HWND WINAPI ::CreateTipWndEz(HWND owner, int x, int y,
	const char *text, HICON icon, BOOL showwnd)
{
	POINT point;
	point.x = x, point.y = y;
	::ClientToScreen(owner, &point);
	RECT rect;
	rect.left = point.x, rect.top = point.y;
	rect.right = point.x + 64, rect.bottom = point.y + 16;

	HWND wnd = CreateTipWnd(owner, &rect, 0);
	if (wnd == NULL) return NULL;

	TipWnd_SetText(wnd, text);
	TipWnd_SetIcon(wnd, icon, ICON_BIG);

	if (showwnd) {
		::BringWindowToTop(wnd);
		::ShowWindow(wnd, SW_SHOWNOACTIVATE);
		::UpdateWindow(wnd);
	}

	return wnd;
}

/***************************************************************************/

/* The MFC class wrapper.  Very simple, since the Windows HWND does
   all the real work. */

IMPLEMENT_DYNAMIC(CTipWnd, CWnd)

CTipWnd::CTipWnd()
{
}

CTipWnd::~CTipWnd()
{
	DestroyWindow();
}

BOOL CTipWnd::Create(CWnd *owner, const RECT &rect, BOOL showwnd)
{
	CWnd *pWnd = this;
	if (!pWnd->CreateEx(WS_EX_TOPMOST, _T(CLASSNAME), _T(""), WS_POPUP, rect, owner, 0, NULL))
		return 0;
	TipWndInfo *info = (TipWndInfo *)::GetWindowLong(m_hWnd, GWL_USERDATA);
	if (info != NULL) info->owner = owner->m_hWnd;

	if (showwnd) {
		pWnd->BringWindowToTop();
		pWnd->ShowWindow(SW_SHOWNOACTIVATE);
		pWnd->UpdateWindow();
	}

	return 1;
}

BOOL CTipWnd::CreateEz(CWnd *owner, int x, int y, const CString &text,
	HICON icon, BOOL showwnd)
{
	CPoint point(x, y);
	owner->ClientToScreen(&point);
	CRect rect(point.x, point.y, point.x+64, point.y+16);

	if (!Create(owner, rect, 0)) return 0;

	SetWindowText(text);
	SetIcon(icon, 1);

	if (showwnd) {
		BringWindowToTop();
		ShowWindow(SW_SHOWNOACTIVATE);
		UpdateWindow();
	}

	return 1;
}

void CTipWnd::MoveWindow(CWnd *owner, int x, int y)
{
	CPoint point(x, y);
	owner->ClientToScreen(&point);
	SetWindowPos(NULL, point.x, point.y, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);
}

void CTipWnd::PushOnScreen(void)
{
	CRect rect;
	GetWindowRect(&rect);
	short int scrnwidth = GetSystemMetrics(SM_CXSCREEN);
	short int scrnheight = GetSystemMetrics(SM_CYSCREEN);
	if (rect.right > scrnwidth)
		rect.left -= rect.right-scrnwidth, rect.right = scrnwidth;
	if (rect.bottom > scrnheight)
		rect.top -= rect.bottom-scrnheight, rect.top = scrnheight;
	if (rect.left < 0)
		rect.right -= 0-rect.left, rect.left = 0;
	if (rect.top < 0)
		rect.bottom -= 0-rect.top, rect.top = 0;
	SetWindowPos(NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
		SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);
}

// MFC's default handling of OnCommand screws up our message-passing, so
// we override to fix it.
BOOL CTipWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	if (message == WM_COMMAND) {
		*pResult = TipWndProc(m_hWnd, message, wParam, lParam);
		return 1;
	}
	else return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

/***************************************************************************/
