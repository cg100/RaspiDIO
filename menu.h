/*
 * menu.h
 *
 *  Created on: Jun 12, 2014
 *      Author: psanetra
 */

#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>

typedef struct _menu_item {
	char* text;
	struct _menu_item* submenu;
	struct _menu_item* next_item;
	void (*action)(struct _menu_item* data);
	void* data;
} menu_item;

typedef struct _menu {
	menu_item* current_menu; //menuitem in dessen submenu wir sind
	int selection_index;
	char active;
} menu;

menu selected_menu;
menu_item* item_main_menu;

extern void init_menu();

extern void menu_draw();

extern void menu_dec();

extern void menu_inc();

extern void menu_action();

extern menu_item* create_menu_item(char* text, void (*action)(menu_item*), void* data);

extern void destroy_menu_item(menu_item* item);

extern int menu_item_submenu_length(menu_item* item);

extern menu_item* menu_item_find_item_by_data(menu_item* item, void* data);

extern void menu_item_remove_item(menu_item* item, menu_item* rmitem);

extern void menu_item_insert_subitem(menu_item* item, menu_item* subitem, int index);

extern void menu_item_add_subitem(menu_item* item, menu_item* subitem);

extern menu_item* menu_item_get_sub_item(menu_item* item, int i);

#endif /* MENU_H_ */
