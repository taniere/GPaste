/*
 *      This file is part of GPaste.
 *
 *      Copyright 2012 Marc-Antoine Perennou <Marc-Antoine@Perennou.com>
 *
 *      GPaste is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      GPaste is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with GPaste.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gpaste-client-private.h"
#include "gdbus-defines.h"

#include <gio/gio.h>

#define G_PASTE_CLIENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), G_PASTE_TYPE_CLIENT, GPasteClientPrivate))

G_DEFINE_TYPE (GPasteClient, g_paste_client, G_TYPE_OBJECT)

struct _GPasteClientPrivate
{
    GDBusProxy *proxy;
};

enum
{
    CHANGED,
    SHOW_HISTORY,

    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

#define DBUS_CALL_NO_PARAM(method, ans_type, variant_type, fail) \
    DBUS_CALL_WITH_RETURN(method, \
        NULL, 0, \
        ans_type, variant_type, \
        fail, \
        {})
#define DBUS_CALL_WITH_PARAM(method, ans_type, variant_type, fail, param_type, param_name) \
    DBUS_CALL_WITH_RETURN(method, \
        &parameter, 1, \
        ans_type, variant_type, \
        fail, \
        GVariant *parameter = g_variant_new_##param_type (param_name))
#define DBUS_CALL_NO_PARAM_NO_RETURN(method) \
    DBUS_CALL_NO_RETURN(method, \
        NULL, 0, \
        ans_type, variant_type, \
        fail, \
        {})
#define DBUS_CALL_WITH_PARAM_NO_RETURN(method, param_type, param_name) \
    DBUS_CALL_NO_RETURN(method, \
        &parameter, 1, \
        ans_type, variant_type, \
        fail, \
        GVariant *parameter = g_variant_new_##param_type (param_name))
#define DBUS_CALL_WITH_RETURN(method, param, n_param, ans_type, variant_type, fail, decl) \
    DBUS_CALL_FULL(method, \
        param, n_param, \
        GVariantIter result_iter; \
        g_variant_iter_init (&result_iter, result); \
        GVariant *variant = g_variant_iter_next_value (&result_iter); \
        ans_type answer = g_variant_dup_##variant_type (variant, \
                                                        NULL); /* length */ \
        g_variant_unref (variant), \
        fail, decl, \
        g_return_val_if_fail (G_PASTE_IS_CLIENT (self), NULL), \
        return answer)
#define DBUS_CALL_NO_RETURN(method, param, n_param, ans_type, variant_type, fail, decl) \
    DBUS_CALL_FULL(method, \
        param, n_param, \
        {}, \
        ;, decl, \
        g_return_if_fail (G_PASTE_IS_CLIENT (self)), \
        {})
#define DBUS_CALL_FULL(method, param, n_param, extract_answer, fail, decl, guard, return_stmt) \
    guard; \
    GDBusProxy *proxy = self->priv->proxy; \
    decl; \
    GVariant *result = g_dbus_proxy_call_sync (proxy, \
                                               method, \
                                               g_variant_new_tuple (param, n_param), \
                                               G_DBUS_CALL_FLAGS_NONE, \
                                               -1, \
                                               NULL, /* cancellable */ \
                                               error); \
    if (!result) \
        return fail; \
    extract_answer; \
    g_variant_unref (result); \
    return_stmt;

/**
 * g_paste_client_get_element:
 * @self: a #GPasteClient instance
 * @index: the index of the element we want to get
 * @error: a #GError
 *
 * Get an item from the #GPasteDaemon
 *
 * Returns: a newly allocated string
 */
G_PASTE_VISIBLE gchar *
g_paste_client_get_element (GPasteClient *self,
                            guint32       index,
                            GError      **error)
{
    DBUS_CALL_WITH_PARAM (GET_ELEMENT, gchar*, string, NULL,
                          uint32, index)
}

/**
 * g_paste_client_get_history:
 * @self: a #GPasteClient instance
 * @error: a #GError
 *
 * Get the history from the #GPasteDaemon
 *
 * Returns: (transfer full): a newly allocated array of string
 */
G_PASTE_VISIBLE gchar **
g_paste_client_get_history (GPasteClient *self,
                            GError      **error)
{
    DBUS_CALL_NO_PARAM (GET_HISTORY, gchar**, strv, NULL)
}

/**
 * g_paste_client_add:
 * @self: a #GPasteClient instance
 * @text: the text to add
 * @error: a #GError
 *
 * Add an item to the #GPasteDaemon
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_client_add (GPasteClient *self,
                    const gchar  *text,
                    GError      **error)
{
    DBUS_CALL_WITH_PARAM_NO_RETURN (ADD, string, text)
}

/**
 * g_paste_client_select:
 * @self: a #GPasteClient instance
 * @index: the index of the element we want to select
 * @error: a #GError
 *
 * Select an item from the #GPasteDaemon
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_client_select (GPasteClient *self,
                       guint32       index,
                       GError      **error)
{
    DBUS_CALL_WITH_PARAM_NO_RETURN (SELECT, uint32, index)
}

/**
 * g_paste_client_delete:
 * @self: a #GPasteClient instance
 * @index: the index of the element we want to delete
 * @error: a #GError
 *
 * Delete an item from the #GPasteDaemon
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_client_delete (GPasteClient *self,
                       guint32       index,
                       GError      **error)
{
    DBUS_CALL_WITH_PARAM_NO_RETURN (DELETE, uint32, index)
}

/**
 * g_paste_client_empty:
 * @self: a #GPasteClient instance
 * @error: a #GError
 *
 * Empty the history from the #GPasteDaemon
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_client_empty (GPasteClient *self,
                      GError      **error)
{
    DBUS_CALL_NO_PARAM_NO_RETURN (EMPTY)
}

/**
 * g_paste_client_track:
 * @self: a #GPasteClient instance
 * @state: the new tracking state of the #GPasteDaemon
 * @error: a #GError
 *
 * Change the tracking state of the #GPasteDaemon
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_client_track (GPasteClient *self,
                      gboolean      state,
                      GError      **error)
{
    DBUS_CALL_WITH_PARAM_NO_RETURN (TRACK, boolean, state)
}

/**
 * g_paste_client_reexecute:
 * @self: a #GPasteClient instance
 * @error: a #GError
 *
 * Reexecute the #GPasteDaemon
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_client_reexecute (GPasteClient *self,
                          GError      **error)
{
    DBUS_CALL_NO_PARAM_NO_RETURN (REEXECUTE)
}

/**
 * g_paste_client_backup_history:
 * @self: a #GPasteClient instance
 * @name: the name of the backup
 * @error: a #GError
 *
 * Backup the current history
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_client_backup_history (GPasteClient *self,
                               const gchar  *name,
                               GError      **error)
{
    DBUS_CALL_WITH_PARAM_NO_RETURN (BACKUP_HISTORY, string, name)
}

/**
 * g_paste_client_switch_history:
 * @self: a #GPasteClient instance
 * @name: the name of the history to switch to
 * @error: a #GError
 *
 * Switch to another history
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_client_switch_history (GPasteClient *self,
                               const gchar  *name,
                               GError      **error)
{
    DBUS_CALL_WITH_PARAM_NO_RETURN (SWITCH_HISTORY, string, name)
}

/**
 * g_paste_client_delete_history:
 * @self: a #GPasteClient instance
 * @name: the name of the history to delete
 * @error: a #GError
 *
 * Delete an history
 *
 * Returns:
 */
G_PASTE_VISIBLE void
g_paste_client_delete_history (GPasteClient *self,
                               const gchar  *name,
                               GError      **error)
{
    DBUS_CALL_WITH_PARAM_NO_RETURN (DELETE_HISTORY, string, name)
}

/**
 * g_paste_client_list_histories:
 * @self: a #GPasteClient instance
 * @error: a #GError
 *
 * List all available hisotries
 *
 * Returns: (transfer full): a newly allocated array of string
 */
G_PASTE_VISIBLE gchar **
g_paste_client_list_histories (GPasteClient *self,
                               GError      **error)
{
    DBUS_CALL_NO_PARAM (LIST_HISTORIES, gchar**, strv, NULL)
}

static void
g_paste_client_handle_signal (GPasteClient *self,
                              gchar        *sender_name G_GNUC_UNUSED,
                              gchar        *signal_name,
                              GVariant     *parameters G_GNUC_UNUSED,
                              gpointer      user_data G_GNUC_UNUSED)
{
    if (g_strcmp0 (signal_name, SIG_CHANGED) == 0)
    {
        g_signal_emit (self,
                       signals[CHANGED],
                       0); /* detail */
    }
    else if (g_strcmp0 (signal_name, SIG_SHOW_HISTORY) == 0)
    {
        g_signal_emit (self,
                       signals[SHOW_HISTORY],
                       0); /* detail */
    }
}

static void
g_paste_client_dispose (GObject *object)
{
    GPasteClient *self = G_PASTE_CLIENT (object);
    GDBusProxy *proxy = self->priv->proxy;

    if (proxy)
    {
        g_signal_handlers_disconnect_by_func (proxy, (gpointer) g_paste_client_handle_signal, self);
        g_object_unref (proxy);
    }

    G_OBJECT_CLASS (g_paste_client_parent_class)->dispose (object);
}

static void
g_paste_client_finalize (GObject *object)
{
    G_OBJECT_CLASS (g_paste_client_parent_class)->finalize (object);
}

static void
g_paste_client_class_init (GPasteClientClass *klass)
{
    g_type_class_add_private (klass, sizeof (GPasteClientPrivate));

    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = g_paste_client_dispose;
    object_class->finalize = g_paste_client_finalize;

    signals[CHANGED] = g_signal_new ("changed",
                                     G_PASTE_TYPE_CLIENT,
                                     G_SIGNAL_RUN_LAST,
                                     0, /* class offset */
                                     NULL, /* accumulator */
                                     NULL, /* accumulator data */
                                     g_cclosure_marshal_VOID__VOID,
                                     G_TYPE_NONE,
                                     0); /* number of params */
    signals[SHOW_HISTORY] = g_signal_new ("show-history",
                                          G_PASTE_TYPE_CLIENT,
                                          G_SIGNAL_RUN_LAST,
                                          0, /* class offset */
                                          NULL, /* accumulator */
                                          NULL, /* accumulator data */
                                          g_cclosure_marshal_VOID__VOID,
                                          G_TYPE_NONE,
                                          0); /* number of params */
}

static void
g_paste_client_init (GPasteClient *self)
{
    GPasteClientPrivate *priv = self->priv = G_PASTE_CLIENT_GET_PRIVATE (self);

    GDBusProxy *proxy = priv->proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                                                     G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
                                                                     NULL, /* interface_info */
                                                                     G_PASTE_BUS_NAME,
                                                                     G_PASTE_OBJECT_PATH,
                                                                     G_PASTE_INTERFACE_NAME,
                                                                     NULL, /* cancellable */
                                                                     NULL); /* error */

    if (proxy)
    {
        g_signal_connect_swapped (G_OBJECT (proxy),
                                  "g-signal",
                                  G_CALLBACK (g_paste_client_handle_signal),
                                  self); /* user_data */
    }
}

/**
 * g_paste_client_new:
 *
 * Create a new instance of #GPasteClient
 *
 * Returns: a newly allocated #GPasteClient
 *          free it with g_object_unref
 */
G_PASTE_VISIBLE GPasteClient *
g_paste_client_new (void)
{
    GPasteClient *self = g_object_new (G_PASTE_TYPE_CLIENT, NULL);

    if (!self->priv->proxy)
    {
        g_object_unref (self);
        return NULL;
    }

    return self;
}
