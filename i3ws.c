#include <stdio.h>
#include <signal.h>
#include <i3ipc-glib/i3ipc-glib.h>
#define MAX_WORKSPACE_COUNT 10

struct i3ipcWorkspaceReply {
  gint num;
  gchar *name;
  gboolean visible;
  gboolean focused;
  gboolean urgent;
  gchar *output;
  i3ipcRect *rect;
};

i3ipcConnection *con;

// Prots
void for_each_workspace(gpointer data, gpointer udata);
void print_workspaces();
void on_ws_change();
void on_destroy();
void resetWorkspacesBools();
void on_kill();

// Defs
void for_each_workspace(gpointer data, gpointer udata) {
  struct i3ipcWorkspaceReply *p = data;

  if(p->focused)
    fputs("FOC", stdout);
  else
    fputs("INA", stdout);
  fputs(p->name, stdout);
  putchar(' ');
  p = NULL;
  i3ipc_workspace_reply_free(data);
}

void print_workspaces() {
  if (NULL != con) {
    GSList *workspaces = i3ipc_connection_get_workspaces(con, NULL);
    g_slist_foreach(workspaces, &for_each_workspace, NULL);
    g_slist_free(workspaces);
  }
}

void on_ws_change() {
  fputs("WSP", stdout);
  print_workspaces();
  putchar('\n');
  fflush(stdout);
}

void on_destroy(gpointer data, GClosure *closure) {
  i3ipc_connection_main_quit(con);
  g_object_unref(con);
  con = NULL;

}

void on_kill() {
  on_destroy(NULL, NULL);
}

// === Driver
int main() {
  signal(SIGINT, on_kill);
  signal(SIGTERM, on_kill);
  signal(SIGQUIT, on_kill);

  con = i3ipc_connection_new(NULL, NULL);

  GClosure *clos = g_cclosure_new(&print_workspaces, NULL, &on_destroy);
  clos->marshal = &on_ws_change;

  // Subscribe to workspace changes
  const char *type = "workspace";
  i3ipc_connection_on(con, type, clos, NULL);

  // Spit out initial info
  g_closure_invoke(clos, NULL, 0, NULL, NULL);

  i3ipc_connection_main(con);

  return 0;
}
