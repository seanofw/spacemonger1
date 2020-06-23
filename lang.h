
#ifndef LANG_H
#define LANG_H

struct Lang {
	int toolbar_bitmap_id;
	const char *about_spacemonger;
	const char *freeware;
	const char *warranty;
	const char *email;
	const char *ok, *cancel;
	const char *bytes, *kb, *mb, *gb;
	const char *freespace_format;
	const char *zoomin, *zoomout, *zoomfull, *run, *del;
	const char *opendrive, *rescandrive, *showfreespace;
	const char *percent_format, *size_format;
	const char *total, *free, *files_total, *folders_total, *files_found, *folders_found;
	char digitpt, digitcomma;
	const char **densitynames;
	const char **colornames;
	const char **monthnames;
	const char **attribnames;
	const char *deleting;
	const char *selectdrive;
	const char *scanning;
	const char *settings;
	const char *layout, *density, *bias, *horz, *equal, *vert;
	const char *displaycolors, *files, *folders;
	const char *tooltips, *shownametips, *showinfotips, *delay, *msec;
	const char *fullpath, *filename, *smallicon, *icon, *datetime, *filesize, *attrib;
	const char *miscoptions;
	const char *autorescan, *disabledelete, *animatedzoom, *savepos;
	const char *showrolloverbox;
	const char *properties;
};

struct LangEntry {
	const char *abbrev;
	const char *name;
	Lang *text;
};

extern Lang *CurLang;		// Current language

extern Lang LangUs;			// English (US)
extern Lang LangUk;			// English (UK/Aust.)
extern Lang LangFr;			// Français (Nationale)

extern LangEntry Langs[];	// List of languages

#endif
