/**************************************************************************
 **
 ** sngrep - SIP Messages flow viewer
 **
 ** Copyright (C) 2014,2015 Ivan Alonso (Kaian)
 ** Copyright (C) 2014,2015 Irontec SL. All rights reserved.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
/**
 * @file ui_column_select.c
 * @author Ivan Alonso [aka Kaian] <kaian@irontec.com>
 *
 * @brief Source of functions defined in ui_column_select.h
 *
 */
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include "ui_manager.h"
#include "ui_call_list.h"
#include "ui_column_select.h"

/**
 * Ui Structure definition for Message Diff panel
 */
ui_t ui_column_select = {
    .type = PANEL_COLUMN_SELECT,
    .panel = NULL,
    .create = column_select_create,
    .handle_key = column_select_handle_key,
    .destroy = column_select_destroy
};

PANEL *
column_select_create()
{
    int attr_id, column;
    PANEL *panel;
    WINDOW *win;
    MENU *menu;
    int height, width;
    column_select_info_t *info;

    // Calculate window dimensions
    height = 22;
    width = 60;

    // Cerate a new indow for the panel and form
    win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);

    // Create a new panel
    panel = new_panel(win);

    // Initialize Filter panel specific data
    info = malloc(sizeof(column_select_info_t));
    memset(info, 0, sizeof(column_select_info_t));

    // Store it into panel userptr
    set_panel_userptr(panel, (void*) info);

    // Initialize the fields
    info->fields[FLD_COLUMNS_SNGREPRC] = new_field(1, 1, height - 4, 3, 0, 0);
    info->fields[FLD_COLUMNS_SAVE] = new_field(1, 10, height - 2, 15, 0, 0);
    info->fields[FLD_COLUMNS_CANCEL] = new_field(1, 10, height - 2, 35, 0, 0);
    info->fields[FLD_COLUMNS_COUNT] = NULL;

    // Field Labels
    set_field_buffer(info->fields[FLD_COLUMNS_SAVE], 0, "[ Accept ]");
    set_field_buffer(info->fields[FLD_COLUMNS_CANCEL], 0, "[ Cancel ]");

    // Create the form and post it
    info->form = new_form(info->fields);
    set_form_sub(info->form, win);
    post_form(info->form);

    // Create a subwin for the menu area
    info->menu_win = derwin(win, 10, width - 2, 7, 0);

    // Initialize one field for each attribute
    for (attr_id = 0; attr_id < SIP_ATTR_SENTINEL; attr_id++) {
        // Create a new field for this column
        info->items[attr_id] = new_item("[ ]", sip_attr_get_description(attr_id));
        set_item_userptr(info->items[attr_id], (void*) sip_attr_get_name(attr_id));
    }
    info->items[SIP_ATTR_SENTINEL] = NULL;

    // Create the columns menu and post it
    info->menu = menu = new_menu(info->items);

    // Set current enabled fields
    call_list_info_t *list_info = (call_list_info_t*) panel_userptr(
                                      ui_get_panel(ui_find_by_type(PANEL_CALL_LIST)));

    // Enable current enabled fields and move them to the top
    for (column = 0; column < list_info->columncnt; column++) {
        const char *attr = list_info->columns[column].attr;
        for (attr_id = 0; attr_id < item_count(menu); attr_id++) {
            if (!strcmp(item_userptr(info->items[attr_id]), attr)) {
                column_select_toggle_item(panel, info->items[attr_id]);
                column_select_move_item(panel, info->items[attr_id], column);
                break;
            }
        }
    }

    // Set main window and sub window
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, 10, width - 5, 7, 2));
    set_menu_format(menu, 10, 1);
    set_menu_mark(menu, "");
    set_menu_fore(menu, COLOR_PAIR(CP_DEF_ON_BLUE));
    menu_opts_off(menu, O_ONEVALUE);
    post_menu(menu);

    // Draw a scrollbar to the right
    draw_vscrollbar(info->menu_win, top_row(menu), item_count(menu) - 1, 0);

    // Set the window title and boxes
    mvwprintw(win, 1, width / 2 - 14, "Call List columns selection");
    wattron(win, COLOR_PAIR(CP_BLUE_ON_DEF));
    title_foot_box(panel_window(panel));
    mvwhline(win, 6, 1, ACS_HLINE, width - 1);
    mvwaddch(win, 6, 0, ACS_LTEE);
    mvwaddch(win, 6, width - 1, ACS_RTEE);
    mvwhline(win, height - 5, 1, ACS_HLINE, width - 1);
    mvwaddch(win, height - 5, 0, ACS_LTEE);
    mvwaddch(win, height - 5, width - 1, ACS_RTEE);
    wattroff(win, COLOR_PAIR(CP_BLUE_ON_DEF));

    // Set field labels
    mvwprintw(win, 18, 2, "[ ] Remember columns state");

    // Some brief explanation abotu what window shows
    wattron(win, COLOR_PAIR(CP_CYAN_ON_DEF));
    mvwprintw(win, 3, 2, "This windows show the list of columns displayed on Call");
    mvwprintw(win, 4, 2, "List. You can enable/disable using Space Bar and reorder");
    mvwprintw(win, 5, 2, "them using + and - keys.");
    wattroff(win, COLOR_PAIR(CP_CYAN_ON_DEF));

    info->form_active = 0;

    return panel;
}

void
column_select_destroy(PANEL *panel)
{
    int i, itemcnt;
    column_select_info_t *info = (column_select_info_t*) panel_userptr(panel);

    // Get item count
    itemcnt = item_count(info->menu);

    // Unpost the menu
    unpost_menu(info->menu);

    // Free items
    for (i = 0; i < itemcnt; i++) {
        free_item(info->items[i]);
    }
}

int
column_select_handle_key(PANEL *panel, int key)
{
    // Get panel information
    column_select_info_t *info = (column_select_info_t*) panel_userptr(panel);

    if (info->form_active) {
        return column_select_handle_key_form(panel, key);
    } else {
        return column_select_handle_key_menu(panel, key);
    }
    return 0;
}

int
column_select_handle_key_menu(PANEL *panel, int key)
{
    MENU *menu;
    ITEM *current;
    int current_idx;

    // Get panel information
    column_select_info_t *info = (column_select_info_t*) panel_userptr(panel);

    menu = info->menu;
    current = current_item(menu);
    current_idx = item_index(current);

    switch (key) {
        case KEY_DOWN:
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu, REQ_UP_ITEM);
            break;
        case KEY_NPAGE:
            menu_driver(menu, REQ_SCR_DPAGE);
            break;
        case KEY_PPAGE:
            menu_driver(menu, REQ_SCR_UPAGE);
            break;
        case 10:
        case ' ':
            column_select_toggle_item(panel, current);
            column_select_update_menu(panel);
            break;
        case '+':
            column_select_move_item(panel, current, current_idx + 1);
            column_select_update_menu(panel);
            break;
        case '-':
            column_select_move_item(panel, current, current_idx - 1);
            column_select_update_menu(panel);
            break;
        case 9 /*KEY_TAB*/:
            info->form_active = 1;
            set_menu_fore(menu, COLOR_PAIR(CP_DEFAULT));
            form_driver(info->form, REQ_VALIDATION);
            curs_set(1);
            break;
        default:
            return key;
    }

    // Draw a scrollbar to the right
    draw_vscrollbar(info->menu_win, top_row(menu), item_count(menu) - 1, 0);
    wnoutrefresh(info->menu_win);
    return 0;
}

int
column_select_handle_key_form(PANEL *panel, int key)
{
    int field_idx, new_field_idx;
    char field_value[48];

    // Get panel information
    column_select_info_t *info = (column_select_info_t*) panel_userptr(panel);

    // Get current field id
    field_idx = field_index(current_field(info->form));

    // Get current field value.
    // We trim spaces with sscanf because and empty field is stored as
    // space characters
    memset(field_value, 0, sizeof(field_value));
    sscanf(field_buffer(current_field(info->form), 0), "%[^ ]", field_value);

    switch (key) {
        case 9 /*KEY_TAB*/:
        case KEY_DOWN:
            form_driver(info->form, REQ_NEXT_FIELD);
            break;
        case KEY_UP:
            form_driver(info->form, REQ_PREV_FIELD);
            break;
        case 27 /*KEY_ESC*/:
            return key;
            break;
        case ' ':
        case 10: /* KEY_ENTER */
            switch(field_idx) {
                case FLD_COLUMNS_SAVE:
                    column_select_update_columns(panel);
                    return 27;
                case FLD_COLUMNS_CANCEL:
                    return 27;
                case FLD_COLUMNS_SNGREPRC:
                    info->remember = info->remember ? 0 : 1;
                    break;
            }
            break;
        default:
            break;
    }

    // Set field values
    set_field_buffer(info->fields[FLD_COLUMNS_SNGREPRC], 0,
                     (info->remember) ? "*" : "");

    // Validate all input data
    form_driver(info->form, REQ_VALIDATION);

    // Change background and cursor of "button fields"
    set_field_back(info->fields[FLD_COLUMNS_SAVE], A_NORMAL);
    set_field_back(info->fields[FLD_COLUMNS_CANCEL], A_NORMAL);
    curs_set(1);

    // Change current field background
    new_field_idx = field_index(current_field(info->form));
    if (new_field_idx == FLD_COLUMNS_SAVE || new_field_idx == FLD_COLUMNS_CANCEL) {
        set_field_back(info->fields[new_field_idx], A_REVERSE);
        curs_set(0);
    }

    // Swap between menu and form
    if (field_idx == FLD_COLUMNS_CANCEL && new_field_idx == FLD_COLUMNS_SNGREPRC) {
        set_menu_fore(info->menu, COLOR_PAIR(CP_DEF_ON_BLUE));
        curs_set(0);
        info->form_active = 0;
    }


    return 0;
}

void
column_select_update_columns(PANEL *panel)
{
    int column, attr_id;

    // Get panel information
    column_select_info_t *info = (column_select_info_t*) panel_userptr(panel);

    // Set enabled fields
    PANEL *list_panel = ui_get_panel(ui_find_by_type(PANEL_CALL_LIST));
    call_list_info_t *list_info = (call_list_info_t*) panel_userptr(list_panel);

    // Reset column count
    list_info->columncnt = 0;

    // Add all selected columns
    for (column = 0; column < item_count(info->menu); column++) {
        // If column is active
        if (!strncmp(item_name(info->items[column]), "[ ]", 3))
            continue;

        // Get column attribute
        attr_id = sip_attr_from_name(item_userptr(info->items[column]));
        // Add a new column to the list
        call_list_add_column(list_panel, attr_id, sip_attr_get_name(attr_id),
                             sip_attr_get_title(attr_id), sip_attr_get_width(attr_id));
    }

    // Store columns in configuretion if requested
    if (info->remember)
        column_select_save_columns(panel);
}

void
column_select_save_columns(PANEL *panel)
{
    int column;
    FILE *fi, *fo;
    regex_t setting;
    char columnopt[128];
    char line[1024];
    char *home = getenv("HOME");
    char userconf[128], tmpfile[128];

    // No home dir...
    if (!home)
        return;

    // Read current $HOME/.sngreprc file
    sprintf(userconf, "%s/.sngreprc", home);
    sprintf(tmpfile, "%s/.sngreprc.old", home);

    // Move home file to temporal dir
    rename(userconf, tmpfile);

    // Create a new user conf file
    if (!(fo = fopen(userconf, "w")))  {
        return;
    }

    // Read all lines of old sngreprc file
    if ((fi = fopen(tmpfile, "r"))) {
        // Compile expression matching
        regcomp(&setting, "^\\s*set\\s+cl.column[0-9]+\\s+\\S+\\s*$", REG_EXTENDED);
        // Read all configuration file
        while (fgets(line, 1024, fi) != NULL) {
            // Dont copy set cl.columns lines
            if (regexec(&setting, line, 0, 0, 0) == 0)
                continue;
            // Put everyting in new .sngreprc file
            fputs(line, fo);
        }
        fclose(fi);
        regfree(&setting);
    }

    // Get panel information
    column_select_info_t *info = (column_select_info_t*) panel_userptr(panel);

    // Add all selected columns
    for (column = 0; column < item_count(info->menu); column++) {
        // If column is active
        if (!strncmp(item_name(info->items[column]), "[ ]", 3))
            continue;

        // Add the columns settings
        sprintf(columnopt, "set cl.column%d %s\n", column, (const char*) item_userptr(info->items[column]));
        fputs(columnopt, fo);
    }
    fclose(fo);
}


void
column_select_move_item(PANEL *panel, ITEM *item, int pos)
{
    // Get panel information
    column_select_info_t *info = (column_select_info_t*) panel_userptr(panel);

    // Check we have a valid position
    if (pos == item_count(info->menu) || pos < 0)
        return;

    // Swap position with destination
    int item_pos = item_index(item);
    info->items[item_pos] = info->items[pos];
    info->items[item_pos]->index = item_pos;
    info->items[pos] = item;
    info->items[pos]->index = pos;
}

void
column_select_toggle_item(PANEL *panel, ITEM *item)
{
    // Change item name
    if (!strncmp(item_name(item), "[ ]", 3)) {
        item->name.str = "[*]";
    } else {
        item->name.str = "[ ]";
    }
}

void
column_select_update_menu(PANEL *panel)
{
    // Get panel information
    column_select_info_t *info = (column_select_info_t*) panel_userptr(panel);
    ITEM *current = current_item(info->menu);
    int top_idx = top_row(info->menu);

    // Remove the menu from the subwindow
    unpost_menu(info->menu);
    // Set menu items
    set_menu_items(info->menu, info->items);
    // Put the menu agin into its subwindow
    post_menu(info->menu);

    // Move until the current position is set
    set_top_row(info->menu, top_idx);
    set_current_item(info->menu, current);
}
