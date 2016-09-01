build:
	gcc -o dist/i3ws i3ws.c $(shell pkg-config --cflags --libs glib-2.0 i3ipc-glib-1.0)
