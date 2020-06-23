
#include "stdafx.h"
#include "SpaceMonger.h"
#include "lang.h"

LangEntry Langs[] = {
	{ "us", "English (US)", &LangUs },
	{ "uk", "English (UK/Aust.)", &LangUk },
	{ "fr", "Français (Nationale)", &LangFr },
	{ NULL, NULL, NULL },
};

Lang *CurLang = &LangUs;

/////////////////////////////////////////////////////////////////////////////

static const char *densitynames[] = {
	"Too Few Files", "Very Few Files", "Normal", "Lots of Files",
	"Very Many Files", "Too Many Files",
};

static const char *uscolornames[] = {
	"Rainbow", "Windows Colors", "White", "Light Gray", "Dark Gray",
	"Red", "Orange", "Yellow", "Green", "Aqua", "Blue", "Violet",
};

static const char *monthnames[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

static const char *attribnames[] = {
	"Arch", "Compress", "Folder", "Encrypt", "Hidden", "Offline",
	"Read-Only", "Reparse-Pt", "Sparse", "System", "Temp",
};

Lang LangUs = {
	IDR_TOOLBAR_EN,
	"About SpaceMonger",
	"THIS PROGRAM IS FREEWARE.",
	"You may distribute this program freely.  Because this program is"
		" free, the author cannot and will not assume responsibility"
		" for this program's behavior.  There are no warranties of any"
		" kind, expressed or implied, on this product.",
	"Have questions?  Found a bug?  Want a feature?\n"
		"E-mail to <sean@werkema.com>.",
	"OK", "Cancel",
	"bytes", "Kb", "Mb", "Gb",
	"<Free Space: %d.%d%%>",
	"Zoom &In", "Zoom &Out", "Zoom &Full", "&Run / Open", "&Delete",
	"&Open Drive...", "&Rescan Drive", "&Show Free Space",
	"%lu.%01lu%%", "%lu.%01lu %s",
	"Total", "Free", "Files Total:", "Folders Total:", "Files Found:", "Folders Found:",
	'.', ',',
	densitynames, uscolornames, monthnames, attribnames,
	"Deleting...",
	"Select Drive to View",
	"Scanning Disk...",
	"SpaceMonger Settings",
	"File Layout", "Density:", "Bias:", "Horz", "Equal", "Vert",
	"Display Colors", "Files", "Folders",
	"ToolTips", "Show file-name-tips", "Show file-info-tips", "Delay:", "msec",
	"Full Path", "Filename", "Small Icon", "Icon", "Date / Time", "File Size", "Attributes",
	"Miscellaneous Options",
	"Auto Rescan on Delete", "Disable \"Delete\" Command",
	"Animated Zoom In / Zoom Out", "Remember Window Position",
	"Show Rollover Boxes",
	"Properties...",
};

static const char *ukcolornames[] = {
	"Rainbow", "Windows Colours", "White", "Light Gray", "Dark Gray",
	"Red", "Orange", "Yellow", "Green", "Aqua", "Blue", "Violet",
};

Lang LangUk = {
	IDR_TOOLBAR_EN,
	"About SpaceMonger",
	"THIS PROGRAM IS FREEWARE.",
	"You may distribute this program freely.  Because this program is"
		" free, the author cannot and will not assume responsibility"
		" for this program's behavior.  There are no warranties of any"
		" kind, expressed or implied, on this product.",
	"Have questions?  Found a bug?  Want a feature?\n"
		"E-mail to <sean@werkema.com>.",
	"OK", "Cancel",
	"bytes", "Kb", "Mb", "Gb",
	"<Free Space: %d.%d%%>",
	"Zoom &In", "Zoom &Out", "Zoom &Full", "&Run / Open", "&Delete",
	"&Open Drive...", "&Rescan Drive", "&Show Free Space",
	"%lu.%01lu%%", "%lu.%01lu %s",
	"Total", "Free", "Files Total:", "Folders Total:", "Files Found:", "Folders Found:",
	'.', ',',
	densitynames, ukcolornames, monthnames, attribnames,
	"Deleting...",
	"Select Drive to View",
	"Scanning Disk...",
	"SpaceMonger Settings",
	"File Layout", "Density:", "Bias:", "Horz", "Equal", "Vert",
	"Display Colours", "Files", "Folders",
	"ToolTips", "Show file-name-tips", "Show file-info-tips", "Delay:", "msec",
	"Full Path", "Filename", "Small Icon", "Icon", "Date / Time", "File Size", "Attributes",
	"Miscellaneous Options",
	"Auto Rescan on Delete", "Disable \"Delete\" Command",
	"Animated Zoom In / Zoom Out", "Remember Window Position",
	"Show Rollover Boxes",
	"Properties...",
};

