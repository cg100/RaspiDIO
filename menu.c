/*
 * menu.c
 *
 *  Created on: Jun 12, 2014
 *      Author: psanetra
 */
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "lcdpi.h"

void init_menu()
{
	memset(&selected_menu, 0, sizeof(menu));

	item_main_menu = create_menu_item("Hauptmenu",  0, 0);

	selected_menu.current_menu = item_main_menu;

}

void menu_draw()
{
	const lcd_line lines[] = {LINE1, LINE2, LINE3, LINE4};
	int start_index = selected_menu.selection_index - 1;
	int i = 0;
	const int selTextLength = 20;
	char selectedText[selTextLength];
	int submenu_length = menu_item_submenu_length(selected_menu.current_menu);
	menu_item* item = 0;

	if(!selected_menu.active) return;

	//Text des selektierten Items modifizieren
	memset(&selectedText, 0, selTextLength * sizeof(char));

	selectedText[0] = 0x7e; //Pfeil
	selectedText[1] = ' ';

	item = menu_item_get_sub_item(selected_menu.current_menu, selected_menu.selection_index);

	//item text anfügen
	for(i = 2; i < (selTextLength - 1) && item->text[i - 2]; i++)
	{
		selectedText[i] = item->text[i - 2];
	}

	//start index modifizieren
	if((start_index + 4) > submenu_length)
	{
		start_index = submenu_length - 4;
	}

	if(start_index < 0)
	{
		start_index = 0;
	}

	item = menu_item_get_sub_item(selected_menu.current_menu, start_index);

	//display clear
	lcd_display_clear();

	//durch items iterieren und text ausgeben
	for(i = 0; i < 4 && item; i++, item = item->next_item)
	{
		char* text = item->text;

		if((i + start_index) == selected_menu.selection_index)
		{
			text = &selectedText[0];
		}

		lcd_line_print(lines[i], text);
	}
}

void menu_dec()
{
	if(selected_menu.selection_index < 1 || !selected_menu.active) return;

	selected_menu.selection_index--;
	menu_draw();
}

void menu_inc()
{
	if((selected_menu.selection_index + 1) >= menu_item_submenu_length(selected_menu.current_menu)) return;

	selected_menu.selection_index++;
	menu_draw();
}

void menu_action()
{
	menu_item* item = 0;

	if(!selected_menu.active) return;

	item = menu_item_get_sub_item(selected_menu.current_menu, selected_menu.selection_index);

	if(item->action)
	{
		item->action(item);
	}
}

//------------------------------------------------------------------------
//--Menu Item Funktionen
//------------------------------------------------------------------------

menu_item* create_menu_item(char* text, void (*action)(menu_item*), void* data)
{
	menu_item* ret = 0;
	char* new_text = 0;
	int text_length = strlen(text);

	if(!text) return ret;

	new_text = (char*)calloc(text_length + 1, sizeof(char));

	if(!new_text) return ret;

	strcpy(new_text, text);

	ret = (menu_item*)calloc(1, sizeof(menu_item));

	if(!ret)
	{
		free(new_text);
		return ret;
	}

	ret->text = new_text;
	ret->action = action;
	ret->data = data;

	return ret;
}

void destroy_menu_item(menu_item* item)
{
	if(!item) return;

	if(item->text)
	{
		free(item->text);
		item->text = 0;
	}

	destroy_menu_item(item->next_item);
	item->next_item = 0;

	destroy_menu_item(item->submenu);
	item->submenu = 0;

	free(item);
}

menu_item* menu_item_find_item_by_data(menu_item* item, void* data)
{
	menu_item* ret = 0;
	if(!item) return 0;

	if(item->data == data)
		return item;

	ret = menu_item_find_item_by_data(item->next_item, data);

	if(ret)
		return ret;

	return menu_item_find_item_by_data(item->submenu, data);
}

void menu_item_remove_item(menu_item* item, menu_item* rmitem)
{
	if(!item || !rmitem) return;

	if(item->next_item == rmitem)
	{
		item->next_item = rmitem->next_item;
		rmitem->next_item = 0;
		destroy_menu_item(rmitem);
		return;
	}

	if(item->submenu == rmitem)
	{
		item->submenu = rmitem->next_item;
		rmitem->next_item = 0;
		rmitem->submenu = 0;
		destroy_menu_item(rmitem);
		return;
	}

	menu_item_remove_item(item->next_item, rmitem);
	menu_item_remove_item(item->submenu, rmitem);
}

int menu_item_submenu_length(menu_item* item)
{
	int ret = 0;
	if(!item || !item->submenu) return ret;

	ret++;

	item = item->submenu;

	for(; item->next_item; ret++)
		item = item->next_item;

	return ret;
}

void menu_item_insert_subitem(menu_item* item, menu_item* subitem, int index)
{
	menu_item* tmp_item = item->submenu;

	if(!item->submenu || index == 0)
	{
		subitem->next_item = item->submenu;
		item->submenu = subitem;
		return;
	}

	for(; index > 1 && tmp_item->next_item; index--)
	{
		tmp_item = tmp_item->next_item;
	}

	subitem->next_item = tmp_item->next_item;
	tmp_item->next_item = subitem;
}

void menu_item_add_subitem(menu_item* item, menu_item* subitem)
{
	int submenu_length = menu_item_submenu_length(item);

	menu_item_insert_subitem(item, subitem, submenu_length);
}

menu_item* menu_item_get_sub_item(menu_item* item, int i)
{
	menu_item* ret = item->submenu;

	if(!ret) return 0;

	for(; i > 0; i--)
	{
		if(ret->next_item)
			ret = ret->next_item;
		else
			return 0;
	}

	return ret;
}
