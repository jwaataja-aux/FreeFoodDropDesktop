/*-
 * Copyright (c) 2016, Jason Waataja
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <glib/gi18n.h>

#include "ffddwindow.h"

struct _FfddWindow {
	GtkApplicationWindow parent;
};

struct _FfddWindowClass {
	GtkApplicationWindowClass parent_class;
};

typedef struct _FfddWindowPrivate FfddWindowPrivate;

struct _FfddWindowPrivate {
	GtkWidget	*address_entry;
	GtkWidget	*type_box;
	GtkWidget	*search_button;
	GtkWidget	*results_view;
	GtkListStore	*results_store;
};

G_DEFINE_TYPE_WITH_PRIVATE(FfddWindow, ffdd_window,
    GTK_TYPE_APPLICATION_WINDOW);

enum results_view_cols {
    ADDRESS_COLUMN = 0,
    NAME_COLUMN,
    FOOD_COLUMN,
    TIME_COLUMN,
    EXTRA_COLUMN,
    N_COLUMNS
};

static GActionEntry win_entries[] = {
    {"quit", quit_activated, NULL, NULL, NULL},
    {"about", about_activated, NULL, NULL, NULL}
};

static void
ffdd_window_init(FfddWindow *win)
{
	FfddWindowPrivate *priv;

	priv = ffdd_window_get_instance_private(win);
	gtk_widget_init_template(GTK_WIDGET(win));

	ffdd_window_init_results_view(win);

	g_signal_connect_swapped(priv->search_button, "clicked",
	    G_CALLBACK(ffdd_window_activate_search), win);

	g_action_map_add_action_entries(G_ACTION_MAP(win), win_entries,
	    G_N_ELEMENTS(win_entries), win);
}

static void
ffdd_window_class_init(FfddWindowClass *kclass)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(kclass),
	    "/com/waataja/ffdd/ui/mainwindow.ui");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(kclass),
	    FfddWindow, address_entry);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(kclass),
	    FfddWindow, type_box);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(kclass),
	    FfddWindow, search_button);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(kclass),
	    FfddWindow, results_view);
}

FfddWindow *
ffdd_window_new(FfddApplication *app)
{
	FfddWindow *win = g_object_new(FFDD_TYPE_WINDOW, "application", app,
	    NULL);

	return (win);
}

void
ffdd_window_activate_search(FfddWindow *win)
{
	const gchar *address;
	FfddWindowPrivate *priv;

	priv = ffdd_window_get_instance_private(win);

	address = gtk_entry_get_text(GTK_ENTRY(priv->address_entry));
}

void
ffdd_window_init_results_view(FfddWindow *win)
{
	FfddWindowPrivate *priv;
	GtkTreeViewColumn *address_col;
	GtkTreeViewColumn *name_col;
	GtkTreeViewColumn *food_col;
	GtkTreeViewColumn *time_col;
	GtkTreeViewColumn *extra_col;
	GtkCellRenderer *text_render;

	priv = ffdd_window_get_instance_private(win);
	priv->results_store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING,
	    G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(priv->results_view),
	    GTK_TREE_MODEL(priv->results_store));

	text_render = GTK_CELL_RENDERER(gtk_cell_renderer_text_new());

	address_col = gtk_tree_view_column_new_with_attributes(_("Address"),
	    text_render, "text", ADDRESS_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(priv->results_view),
	    address_col);
	name_col = gtk_tree_view_column_new_with_attributes(_("Name"),
	    text_render, "text", NAME_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(priv->results_view),
	    name_col);
	food_col = gtk_tree_view_column_new_with_attributes(_("Food"),
	    text_render, "text", FOOD_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(priv->results_view),
	    food_col);
	time_col = gtk_tree_view_column_new_with_attributes(_("Time"),
	    text_render, "text", TIME_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(priv->results_view),
	    time_col);
	extra_col = gtk_tree_view_column_new_with_attributes(_("Information"),
	    text_render, "text", EXTRA_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(priv->results_view),
	    extra_col);
}

void quit_activated(GSimpleAction *action, GVariant *parameter, gpointer win)
{

	gtk_widget_destroy(GTK_WIDGET(win));
}

void about_activated(GSimpleAction *action, GVariant *parameter, gpointer win)
{
  const gchar *authors[] = { "Jason Waataja", "Justin Frank", NULL };

  gtk_show_about_dialog(GTK_WINDOW (win),
      "program-name", "FreeFoodDropDesktop",
      "title", _("About FreeFoodDropDesktop"),
      "authors", authors,
      "copyright", "Copyright (C) 2016 Jason Waataja",
      "license-type", GTK_LICENSE_BSD,
      "version", "Version 0.1.0",
      "website", "https://github.com/JasonWaataja/FreeFoodDropDesktop",
      "comments", "Desktop client for the FreeFoodDrop system",
      NULL);
}
