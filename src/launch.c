// gcc -Wall -Wextra -o launch launch.c $(pkg-config gtk+-3.0 --cflags --libs) -rdynamic
#include <gtk/gtk.h>
#include <glib.h>

#ifdef __MSYS__
#include <windows.h>
#endif

/*
void plotter_watch (__attribute__((unused)) GPid pid, __attribute__((unused)) gint status, __attribute__((unused)) gpointer data) {

    g_print("Child watch function activated.\n");
}
*/
static gboolean stdout_watch(GIOChannel *channel, GIOCondition cond, __attribute__((unused)) gpointer data)
{
    gchar *string;
    gsize  size;

    if(cond == G_IO_HUP)
    {
        g_io_channel_unref(channel);
        return(FALSE);
    }

    g_io_channel_read_line(channel, &string, &size, NULL, NULL);
    g_free(string);

    return(TRUE);
}

static gboolean stderr_watch(GIOChannel *channel, GIOCondition cond, __attribute__((unused)) gpointer data)
{
    gchar *string;
    gsize  size;

    if(cond == G_IO_HUP)
    {
        g_io_channel_unref(channel);
        return(FALSE);
    }

    g_io_channel_read_line(channel, &string, &size, NULL, NULL);

    if (string != NULL)
        g_print("%s", string);

    g_free(string);

    return(TRUE);
}

void quitButtonClicked(__attribute__((unused)) GtkWidget *widget, 
                       __attribute__((unused)) gpointer   data)
{
	g_print("About to Spawn Matplotlib Process...\n");

	// Set the python executable string based on the OS
	gchar* python_executable;

	#ifdef __MSYS__
	python_executable = "dist/plot/plot.exe";
	g_print("Using MSYS2\n");
	#else
	python_executable = "dist/plot/./plot";
	g_print("Not Using MSYS2\n");
	#endif
	
	// Set Environment Variable for g_spawn() needed by Python (HOMEPATH)
	g_setenv("HOMEPATH", g_get_home_dir(), TRUE);

	// Create argv for the python executable
	gchar* argv[] = {python_executable, NULL};

	GPid transition_pid;
	GError* err = NULL;

	int stdoutFD;
	int stderrFD;
	gboolean spawn_success;

	// Spawn the python program to plot the transition data
	spawn_success = 
	g_spawn_async_with_pipes (NULL,	// Inherit parent's working directory
	argv,               		// argv from above
	NULL,				// Don't set any additional environment variables
	G_SPAWN_DO_NOT_REAP_CHILD,	// Default spawn settings
	NULL,				// No setup function prior to executing
	NULL,				// No data passed to setup function
	&transition_pid,		// Store child pid
	NULL,				// No standard input file descriptor
	&stdoutFD,			// Standard output file descriptor
	&stderrFD,			// Standard error file descriptor
	&err);				// Store error

	if (err != NULL)
	{
	  g_error ("Spawning child failed: %s", err->message);
	  return;
	}

	if (spawn_success) {
	    g_print("G_SPAWN SUCCESS!\n");
	}
	else {
	    g_print("G_SPAWN FAILED!\n");
	}

	//g_child_watch_add (transition_pid, (GChildWatchFunc) plotter_watch, NULL);

	GIOChannel *out_ch, *err_ch;

	// Create channels that will be used to read data from pipes.
	#ifdef G_OS_WIN32
	    out_ch = g_io_channel_win32_new_fd(stdoutFD);
	    err_ch = g_io_channel_win32_new_fd(stderrFD);
	#else
	    out_ch = g_io_channel_unix_new(stdoutFD);
	    err_ch = g_io_channel_unix_new(stderrFD);
	#endif

	// Add watches to channels
	g_io_add_watch(out_ch, G_IO_IN | G_IO_HUP, (GIOFunc) stdout_watch, data);
	g_io_add_watch(err_ch, G_IO_IN | G_IO_HUP, (GIOFunc) stderr_watch, data);

	g_print("After Spawn of Matplotlib Process...\n");
}
  
// Handle the user trying to close the window 
gboolean windowDelete(__attribute__((unused)) GtkWidget *widget, 
                      __attribute__((unused)) GdkEvent  *event,
                      __attribute__((unused)) gpointer   data)
{
    g_print("Quit Clicked\n");
    gtk_main_quit();

    return TRUE;
}
 
int main (int argc, char **argv) 
{ 
    GtkWidget *window;
    GtkBuilder *builder = NULL;
 
    gtk_init (&argc , &argv);
 
    builder = gtk_builder_new();

    // Creating a GtkBuilder from a .glade file
    if(gtk_builder_add_from_file (builder,"launch.glade" , NULL) == 0)
    {
        printf("gtk_builder_add_from_file FAILED\n");
        return(0);
    }
 
    window  = GTK_WIDGET (gtk_builder_get_object (builder,"window1"));
   
    // Connecting event handlers
    gtk_builder_connect_signals(builder,NULL);
 
    gtk_widget_show_all (window);
    gtk_main ();
    return 0;
}