
SpaceMonger Version 1.4.0
-------------------------


INTRODUCTION
------------

SpaceMonger is a free tool for keeping track of the disk space on your
computer.  Instead of viewing your disk as unhelpful little icons or
pie charts, you see a single picture that shows you at a glance how
much space every file and folder are using.

The latest version of SpaceMonger is always available at its web page:

    http://www.werkema.com/software/spacemonger.html


SYSTEM REQUIREMENTS
-------------------

SpaceMonger should run on any version of Windows 95, 98, or NT.  It has
been tested on all three.  It will probably run fine on Windows 2000, but
has not been officially tested on Windows 2000.  It should be able to run
on any computer that can run Windows 95, 98, or NT; there are no
additional space or memory requirements, although we do recommend you have
at least 4 megabytes of memory installed in your computer if you want it
to run efficiently.


INSTALLATION
------------

Unpack the .ZIP archive.  A single Windows executable will be produced,
named "SpaceMonger.exe", along with "README.TXT", which is what you're
reading now.  The "SpaceMonger.exe" file is the complete program, and it
can be moved into any folder you like.	(We recommend "C:\Program Files",
but you don't have to put it there.)

Double-click on it to run it.


NOTE TO WINDOWS 2000 USERS:
---------------------------

SpaceMonger will run on Windows 2000.  However, version 1.4.0 will NOT
correctly report space used by hard links, junction points, or sparse
files.  So if you use Windows 2000, here is what you should expect:

   *  Hard linked files will exist in both places:  There will appear
       to be a full copy of the file everywhere a hard link exists,
       so these files will get over-counted.
   *  Junction points and mount points will be treated as though they
       are real folders, so their contents will be over-counted as well.
   *  Sparse files will not be treated as sparse:  If you have a
       10-megabyte sparse file that really only uses one megabyte of
       disk space, it will still be counted as 10 megabytes.

Our crack research team is investigating these problems and should be
able to resolve some of them soon.


FIRST-TIME INSTRUCTIONS
-----------------------

SpaceMonger is not a hard program to use.  In fact, you probably won't
need any instructions.  But for those of you who like to read, here are
some first-time instructions:

When you first open SpaceMonger, you will initially be presented with a
blank window.  Click "Open".  Click on a drive, like drive "C", and hit
"OK".  Wait patiently while the program collects statistics on your
drive.  My, don't you have a lot of files!

SpaceMonger will show you the contents of your drive as little
rectangles.  The larger the rectangle, the more space something is taking
up.  Rectangles that have other rectangles in them are folders.
Rectangles with no other rectangles in them are files. Colors are used
to show how deeply the files and folders are nested.

You can get more information on a given folder by clicking on its title
and then clicking "Zoom In" (or by double-clicking on its title).  The
window will change to show a larger, more detailed view of the folder
and its files.  You can zoom in as many times as you want. "Zoom Out"
will take you back out (up) by one folder.  "Zoom Full" will take you
all the way back to show the whole drive.

If you see a file or folder you want to delete, click it to select it
and hit "Delete".  It will be moved to the Recycle Bin.  If you want to
see what it is first, click on it and hit "Run or Open", which will try
to run it if is a program or open it if it is a file.

DISCLAIMER: If you don't know what something is, DON'T DELETE IT.
   Deleting things at random is a good way to break your programs or
   Windows itself or worse.  'Nuff said.

If you click the "Setup" button, you can change some aesthetic things
about the program.  You can't damage anything with the Setup dialog box,
so feel free to try various options until you like how SpaceMonger looks
and feels.

In addition, you can right-click on a folder or a file to bring up a
context-sensitive menu of options that are identical to those on the
toolbar.  This may be more efficient for some users.

If you double-click on a folder, SpaceMonger will zoom in on it.  If you
double-click on a file (either a program or a document), SpaceMonger
will attempt to run it or open it, whichever is suitable.


DISTRIBUTION
------------

SpaceMonger is freeware, not crippled or time-limited in any way.  You
can distribute it any way you like.  You can even sell it, but the latest
copy will always be available for free download at SpaceMonger's web page,
so you probably won't be able to get much money for it.  We encourage
users of the program to give away copies to their friends.


WHAT'S NEW
----------

Between 1.3.5 and 1.4.0
   * Internals rewritten to support multiple foreign langauges.
     Currently foreign language support exists for British English,
     American English, and French (possibly with grammar errors).
   * Supports popup info-tips that can display the full filename, the
     icon, the date, size, and attributes of any file or folder.
   * Supports popup name-tips that prevent names from being obscured
     by not having enough space to display them.
   * Open dialog box now uses large icons.
   * Includes rollover boxes that highlight the current folder.
   * Files display their sizes and dates if there is enough room.
   * Option to save the current window position.
   * Fixed zero-size bug that caused divide-by-zero errors with some
     Audio CD disks.
   * Minor changes to color palette to handle rollover boxes.
   * Support for Windows Property dialog box.
   * Numerous other bug fixes.
   * Support for Windows NT junction points.

Between 1.3.4 and 1.3.5
   * Toolbar buttons gray out when you can't use them.
   * Setup option to disable the "Delete" button.
   * Fixed size-computation routine that was oversizing
      files on some systems.
   * Double-clicking on files runs or opens them; double-clicking
      on folders zooms in on them.
   * Scanning now includes a percent bar so you can tell how
      long it's taking.

Between 1.3.0 and 1.3.4
   * Major speed improvements using new memory manager subsystem.
      Runs twice as fast and uses half the memory of 1.3.0.
   * Better display of free space information.
   * Right-click opens a pop-up menu of options.
   * Double-click zooms in on a folder.
   * Few minor bug-fixes.
   * Replaced old "Scanning..." window with a better one.

Between 1.2.0 and 1.3.0
   * Added the Setup button with a whole bunch of new options.
   * Internal bug-fixes to scanning routine.

Between 1.0.2 and 1.2.0
   * Lots of stuff.  1.1.0 was never released.  New buttons,
      new look, better code.  Replaced most of what's under
      the hood.
   * 1.2.0 was released for private use only.

Between 1.0.0 and 1.0.2
   * Miscellaneous bug fixes.
   * Added "Zoom In" and "Zoom Out" buttons.
   * Added "Run or Open" and "Delete" buttons.


COPYRIGHT AND CONTACT INFORMATION
---------------------------------

SpaceMonger is Copyright (C) 1998, 1999, and 2000 by Sean Werkema.
Have questions?  Found a bug?  Want a feature?	Compose your thoughts in
an e-mail and send them to sean@werkema.com.  I should be able to respond
within a few days of your message.

