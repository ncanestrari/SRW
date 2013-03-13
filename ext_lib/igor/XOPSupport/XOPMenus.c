/*	This file contains utilities for XOPs that add menu items or entire menus to IGOR.		HR, 10/8/96: Split these routines out from XOPSupport.c.*/#include "XOPStandardHeaders.h"			// Include ANSI headers, Mac headers, IgorXOP.h, XOP.h and XOPSupport.h#include "XOPStructureAlignmentTwoByte.h"	// All structures passed between Igor and XOP are two-byte aligned.// Menu Routines/*	WMDrawMenuBar()	Redraws the Igor menu bar. Call this after altering a top-level menu.	See MenuXOP1.c for an example.		We use WMDrawMenuBar instead of DrawMenuBar in cross-platform code	to avoid conflicting with the Windows API DrawMenuBar routine.*/#ifdef _MACINTOSH_			// On Windows, WMDrawMenuBar is provided by the Igor.lib library. [voidWMDrawMenuBar(void){	DrawMenuBar();}#endif						// ]/*	WMGetMenu(resourceID)	Gets an Igor menu handle from a menu resource stored in the XOP.	Returns NULL if there is no such resource or if some other error occurs.*/MenuHandleWMGetMenu(short resourceID){	MenuHandle mHandle;	#ifdef _MACINTOSH_		mHandle = GetMenu(resourceID);	#endif		#ifdef _WINDOWS_		mHandle = WMGetMenuFromModule(XOPModule(), resourceID);	#endif		return mHandle;}/*	WMDeleteMenu(menuID)	Deletes a menu handle from the menu list. This does not dispose the menu	handle. See MenuXOP1.c for an example.	We use WMDeleteMenu instead of DeleteMenu in cross-platform code	to avoid conflicting with the Windows API DeleteMenu routine.*/#ifdef _MACINTOSH_			// On Windows, WMDeleteMenu is provided by the Igor.lib library. [voidWMDeleteMenu(short menuID){	DeleteMenu(menuID);}#endif						// ]/*	WMInsertMenu(menuH, beforeID)	Inserts a menu handle into the menu list. See MenuXOP1.c for an example.	We use WMInsertMenu instead of InsertMenu in cross-platform code	to avoid conflicting with the Windows API InsertMenu routine.*/#ifdef _MACINTOSH_			// On Windows, WMInsertMenu is provided by the Igor.lib library. [voidWMInsertMenu(MenuHandle menuH, short beforeID){	InsertMenu(menuH, beforeID);}#endif						// ]/*	ResourceToActualMenuID(int resourceMenuID)	Given the ID of a 'MENU' resource in the XOP's resource fork, returns the	actual menu ID of that resource in memory.		Returns 0 if XOP did not add this menu to Igor menu.*/intResourceToActualMenuID(int resourceMenuID){	return(CallBack1(ACTUALMENUID, (void *)resourceMenuID));	}/*	ResourceMenuIDToMenuHandle(int resourceMenuID)	Given the ID of a 'MENU' resource in the XOP's resource fork, returns the	menu handle for that menu.		Returns NIL if XOP did not add this menu.*/MenuHandleResourceMenuIDToMenuHandle(int resourceMenuID){	return((MenuHandle)CallBack1(XOPMENUHANDLE, (void *)resourceMenuID));	}	/*	ActualToResourceMenuID(int menuID)	Given the ID of a menu in memory, returns the resource ID of the 'MENU' resource	in the XOP's resource fork.		Returns 0 if XOP did not add this menu to Igor menu.*/intActualToResourceMenuID(int menuID){	return(CallBack1(RESOURCEMENUID, (void *)menuID));	}/*	ResourceToActualItem(igorMenuID, resourceItemNumber)	Given the ID of a built-in Igor and the number of a menu item specification in the	'XMI1' resource in the XOP's resource fork, returns the actual item number of that	item in the Igor menu.		Item numbers start from 1.		Returns 0 if XOP did not add this menu item to Igor menu.*/intResourceToActualItem(int igorMenuID, int resourceItemNumber){	return(CallBack2(ACTUALITEMNUM, (void *)igorMenuID, (void *)resourceItemNumber));	}	/*	ActualToResourceItem(igorMenuID, actualItemNumber)	Given the ID of a built-in Igor and the actual number of a menu item in the Igor menu,	returns the number of the specification in the 'XMI1' resource in the XOP's resource fork	for that item.		Item numbers start from 1.		Returns 0 if XOP did not add this menu item to Igor menu.*/intActualToResourceItem(int igorMenuID, int actualItemNumber){	return(CallBack2(RESOURCEITEMNUM, (void *)igorMenuID, (void *)actualItemNumber));	}/*	SetIgorMenuItem(message, enable, text, param)	Enables or disables a built-in Igor menu item.		message is a message code that Igor normally passes to the XOP, such as COPY, CUT, PASTE.	enable is 1 to enable the corresponding item, 0 to disable.	text is normally NIL.	However, if the menu item text is variable and text is not NIL, Igor will set	the item to the specified text.	param is normally not used and should be 0.	However, for the FIND message, param is as follows:		1 to set the 'Find' item		2 to set the 'Find Same' item		3 to set the 'Find Selected Text' item.		The function result is 1 if there exists a built-in Igor menu item corresponding	to the message or zero otherwise. Normally, you can ignore this result.*/intSetIgorMenuItem(int message, int enable, const char* text, long param){	return(CallBack4(SETIGORMENUITEM, (void *)message, (void *)enable, (void *)text, (void *)param));	}/*	FillMenu(theMenu, itemList, itemListLen, afterItem)	itemList is a semicolon separated list of items to be put into theMenu.	itemListLen is the total number of characters in itemList.	afterItem specifies where the items in itemList are to appear in the menu.		afterItem = 0			new items appear at beginning of menu		afterItem = 10000		new items appear at end of menu		afterItem = item number	new items appear after specified existing item number.		NOTE: You should not call this routine to update the contents of an existing		  dialog popup menu item. Use FillPopMenu instead.*/voidFillMenu(MenuHandle theMenu, const char *itemList, long itemListLen, int afterItem){	const char *p1;	const char *p2;	char item[256];	long len, itemLen;		p1 = itemList;	while (itemListLen > 0) {		if (p2 = strchr(p1, ';'))			len = p2 - p1;		else			len = itemListLen;					/* last one */		itemLen = len;		if (itemLen > 255)			itemLen = 255;		strncpy(item, p1, itemLen);		item[itemLen] = 0;		insertmenuitem(theMenu, item, afterItem);		p1 += len+1;		itemListLen -= len+1;		afterItem++;	}}/*	FillMenuNoMeta(theMenu, itemList, itemListLen, afterItem)	This routine works exactly like FillMenu except that it does not honor	meta-characters. For example, the Macintosh Menu Manager treats left	parenthesis as a meta-character. If you add an item to a menu that contains a	left parenthesis, the item is added as a disabled item and the left parenthesis	is omitted. This behavior sometimes is not what the programmer intended. Using	FillMenuNoMeta instead of FillMenu causes the Macintosh menu manager to skip	the special treatment of such characters. 	itemList is a semicolon separated list of items to be put into theMenu.	itemListLen is the total number of characters in itemList.	afterItem specifies where the items in itemList are to appear in the menu.		afterItem = 0			new items appear at beginning of menu		afterItem = 10000		new items appear at end of menu		afterItem = item number	new items appear after specified existing item number.		NOTE: Do not call this routine to update the contents of an existing		  dialog popup menu item. Use FillPopMenu instead.*/voidFillMenuNoMeta(MenuHandle theMenu, const char *itemList, long itemListLen, int afterItem){	const char *p1;	const char *p2;	char itemText[256];	long len, itemLen;	int newItemNumber;		newItemNumber = afterItem + 1;	if (newItemNumber > 10000)		newItemNumber = CountMItems(theMenu) + 1;	p1 = itemList;	while (itemListLen > 0) {		if (p2 = strchr(p1, ';'))			len = p2 - p1;		else			len = itemListLen;								// Last one.		itemLen = len;		if (itemLen > 255)			itemLen = 255;		strncpy(itemText, p1, itemLen);		itemText[itemLen] = 0;		insertmenuitem(theMenu, "x", afterItem);		setmenuitemtext(theMenu, newItemNumber, itemText);	// This call does not treat certain characters as meta-characters.		p1 += len+1;		itemListLen -= len+1;		afterItem += 1;		newItemNumber += 1;	}}/*	FillWaveMenu(MenuHandle theMenu, char *match, char *options, int afterItem)	Puts names of waves into theMenu.	match and options are as for the Igor WaveList() function:		match = "*" for all waves		options = "" for all waves		options = "WIN: Graph0" for waves in graph0 only.		afterItem is as for FillMenu, described above.		In contrast to Macintosh menu manager routines, this routine does not	treat any characters as meta-characters.		NOTE: You should not call this routine to update the contents of an existing		  dialog popup menu item. Use FillWavePopMenu instead.*/intFillWaveMenu(MenuHandle theMenu, const char *match, const char *options, int afterItem){	Handle listHandle;	int result;			listHandle = NewHandle(0L);	result = WaveList(listHandle, match, ";", options);	MoveLockHandle(listHandle);	FillMenuNoMeta(theMenu, *listHandle, GetHandleSize(listHandle), afterItem);	DisposeHandle(listHandle);		return(result);}/*	FillPathMenu(MenuHandle theMenu, char *match, char *options, int afterItem)	Puts names of Igor paths into theMenu.	match and options are as for the Igor PathList() function:		match = "*" for all paths		options = "" for all paths		afterItem is as for FillMenu, described above.		In contrast to Macintosh menu manager routines, this routine does not	treat any characters as meta-characters.		NOTE: You should not call this routine to update the contents of an existing		  dialog popup menu item. Use FillPathPopMenu instead.*/intFillPathMenu(MenuHandle theMenu, const char *match, const char *options, int afterItem){	Handle listHandle;	int result;			listHandle = NewHandle(0L);	result = PathList(listHandle, match, ";", options);	MoveLockHandle(listHandle);	FillMenuNoMeta(theMenu, *listHandle, GetHandleSize(listHandle), afterItem);	DisposeHandle(listHandle);		return(result);}/*	FillWinMenu(MenuHandle theMenu, char *match, char *options, int afterItem)	Puts names of Igor windows into theMenu.	match and options are as for the Igor WinList() function:		match = "*" for all windows		options = "" for all windows		options = "WIN: 1" for all graphs		( bit 0 selects graphs)		options = "WIN: 2" for all tables		( bit 1 selects graphs)		options = "WIN: 4" for all layouts		( bit 2 selects graphs)		options = "WIN: 3" for all graphs and tables		afterItem is as for FillMenu, described above.		In contrast to Macintosh menu manager routines, this routine does not	treat any characters as meta-characters.		NOTE: You should not call this routine to update the contents of an existing		  dialog popup menu item. Use FillWindowPopMenu instead.*/intFillWinMenu(MenuHandle theMenu, const char *match, const char *options, int afterItem){	Handle listHandle;	int result;			listHandle = NewHandle(0L);	result = WinList(listHandle, match, ";", options);	MoveLockHandle(listHandle);	FillMenuNoMeta(theMenu, *listHandle, GetHandleSize(listHandle), afterItem);	DisposeHandle(listHandle);		return(result);}/*	WMDeleteMenuItems(theMenu, afterItem)	Deletes the contents of the existing dialog popup menu after the	specified item.		afterItem is 1-based. If afterItem is zero, all items are deleted.		NOTE: You should not call this routine to delete the contents of an existing		  dialog popup menu item. Use DeletePopMenuItems instead.		HR, 010427: This was previously called WMDeleteMenuItems but Apple usurped that name.*/voidWMDeleteMenuItems(MenuHandle theMenu, int afterItem){	int item, items;		afterItem++;						/* # of first item to delete */	items = CountMItems(theMenu);		for (item = afterItem; item <= items; item++)		DeleteMenuItem(theMenu, afterItem);}#include "XOPStructureAlignmentReset.h"	// Reset structure alignment to default.