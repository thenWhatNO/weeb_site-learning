# mini_site

A lightweight C library for building, running, and managing HTTP servers.

> **Status: Experimental.** Core functionality works (server startup, client handling, message dispatch), but the library has not been fully tested. APIs may change.

---

## Table of Contents

- [mini_site.h](#mini_siteh) — top-level server loop
- [server.h](#serverh) — server socket lifecycle
- [client.h](#clienth) — client connection management
- [datagram.h](#datagramh) — message storage and parsing
- [html_data.h](#html_datah) — HTTP headers and HTML page store
- [files_m.h](#files_mh) — file reading and writing

---

## mini_site.h

Top-level functions that tie the library together: listening for client activity and dispatching requests.

### `listin_server`

```c
int listin_server(Datagram_store *DTgrams, Server_socket *server_fd,
                  Clients_socket *client_socket, size_t timeout);
```

Monitors all client fds in the client struct for incoming data, waiting up to `timeout` seconds. When data arrives from any client, it is received and appended to the datagram store.

| Parameter | Description |
|---|---|
| `DTgrams` | Datagram store that receives incoming messages |
| `server_fd` | The server socket |
| `client_socket` | Client array (indexed by fd) |
| `timeout` | Maximum wait time, in seconds |

### `read_client_msg_copy`

```c
int read_client_msg_copy(Packeg *pkg, HTML_page_store *html_data,
                         Files_struct *myfiles, Clients_socket *client_sock,
                         int client_fd);
```

Reads a parsed client datagram and performs the requested action: saving new data for POST requests, or sending an HTML page for GET requests.

> **Known limitation:** This function is currently hardcoded around a single example application and may not generalize to other use cases. A full rewrite is planned.

---

## server.h

Server socket lifecycle: creation, start/stop, accepting connections, and cleanup.

### `Server_socket`

```c
typedef struct Server_socket Server_socket;
```

| Field | Type | Description |
|---|---|---|
| `fd` | `int` | The server's file descriptor |
| `ip` | `char *` | String of the IP address the server binds to |
| `status` | `int` | `1` if open, `0` if closed (but not freed) |

### `initilize_server`

```c
Server_socket *initilize_server(char *ip, char *port);
```

Creates a server bound to the given IP address and port.

**Returns:** pointer to a `Server_socket`, or `NULL` on failure.

```c
Server_socket *my_server = initilize_server("192.168.1.149", "8080");
if (my_server == NULL) {
    perror("couldn't initialize server socket");
    return -1;
}
```

### `start_server`

```c
int start_server(Server_socket *server_sock);
```

Puts the server into listening mode and sets `status` to `1`.

### `stop_server`

```c
int stop_server(Server_socket *server_sock);
```

Shuts the server down without freeing the `Server_socket`. Intended to allow restarting the server later.

> **Note:** Restart functionality is planned but not yet tested.

### `close_server`

```c
int close_server(Server_socket *server_sock);
```

Closes the server fd permanently. The server cannot be reopened after this call.

### `accept_server`

```c
int accept_server(Server_socket *server_fd, int timeout);
```

Waits up to `timeout` milliseconds for a new connection.

**Returns:** the fd of one newly connected client (accepts one connection per call).

### `free_server`

```c
int free_server(Server_socket *server_fd);
```

Frees the memory of the `Server_socket` struct.

---

## client.h

Client connection management.

### `Clients_socket`

```c
typedef struct Clients_socket Clients_socket;
```

| Field | Type | Description |
|---|---|---|
| `command` | `char[]` | The request path the client asked for |
| `active` | `int` | `1` if the slot is in use, `0` if free |
| `is_cl` | `int` | `3` if this is a client connection, `0` otherwise |

The struct is used as an array where **the index is the fd number**, giving O(1) lookup by file descriptor.

### `init_client_sock`

```c
Clients_socket *init_client_sock();
```

Allocates the client array.

**Returns:** pointer to the client array.

> **Note:** The array size is currently fixed at `MAX_CLIENT` (20). Making the limit user-configurable is planned.

### `add_client`

```c
int add_client(Clients_socket *client_sock, int fd);
```

Registers a newly connected client: sets `active = 1` and `is_cl = 3` at index `fd`.

### `add_command_to_client`

```c
int add_command_to_client(Clients_socket *client_sock, int fd, char *command);
```

Stores the request path for the given client.

### `last_open_socket`

```c
int last_open_socket(Clients_socket *client_sock);
```

**Returns:** the highest fd currently in use.

### `remove_client`

```c
int remove_client(Clients_socket *client_sock, int fd);
```

Clears the client's slot in the array and closes the fd.

### `view_client_alive`

```c
int view_client_alive(Clients_socket *client_sock, int fd);
```

Sends a ping to the fd and, based on the response, decides whether to keep the connection or close it.

### `global_sand`

```c
int global_sand(Clients_socket *client_socket, char *data, size_t data_l);
```

Sends the same data to every connected client. Useful for chat broadcasts and open SSE stream fds.

### `is_client_true`

```c
int is_client_true(Clients_socket *client_sock, int index);
```

Checks whether the given array index corresponds to a real server client.

### `view_clients_status`

```c
int view_clients_status(Clients_socket *client_sock);
```

Prints the activity/alive status of each fd to the terminal.

> **Deprecated:** Debug helper; will likely be removed.

---

## datagram.h

Storage, parsing, and maintenance of client messages ("datagrams").

### Structures

```c
typedef struct Datagram Datagram;

struct Datagram {
    char *msg;
    int client_id;
    int msg_size;
};
```

A single message and its size, tagged with the client it came from.

```c
typedef struct Datagram_store Datagram_store;

struct Datagram_store {
    int size;
    int cap;
    Datagram *datagrams;
};
```

A dynamic array of datagrams.

```c
typedef struct Packeg Packeg;

struct Packeg {
    char command[18];
    size_t link_size;
    char *link;
    size_t data_size;
    char *data;
};
```

A fully parsed client message: the request method, the request path, and the POST body if present.

### Accessors

```c
int   get_datagram_client_fd(Datagram *DG);
int   get_datagram_msg_size(Datagram *DG);
char *get_datagram_msg(Datagram *DG);
```

Return the corresponding fields of a `Datagram`. The struct's fields are not accessible outside this module, so these accessors are the only way to read them.

```c
int       get_datagram_store_size(Datagram_store *Dgs);
Datagram *get_datagram_store_datagram(Datagram_store *Dgs, int indx);
```

Return the size of the store, or a pointer to the datagram at index `indx`.

```c
char  *get_packeg_command(Packeg *p);
char  *get_packeg_link(Packeg *p);
char  *get_packeg_data(Packeg *p);
size_t get_packeg_data_size(Packeg *p);
size_t get_packeg_link_size(Packeg *p);
```

Return the fields of a parsed `Packeg`.

### Lifecycle

```c
Datagram_store *init_datagramstore();
```

Creates a datagram store. **Returns:** pointer to the new store.

```c
int add_datagram(Datagram_store *datagram, char *data, int clientfd, int data_size);
```

Appends a new datagram to the store. Called internally by `listin_server` when a message arrives from a client.

```c
Packeg *read_datagram(Datagram *dg);
```

Parses a raw datagram into a `Packeg`.

> **Memory note:** The returned `Packeg` is heap-allocated and **must be freed with `free_datagram` within the same logical block**.

```c
int free_datagram(Packeg *dg);
```

Frees a `Packeg`.

```c
int clear_datagram_store(Datagram_store *datagram);
```

Clears all datagrams from the store. Intended to be called every listening loop to discard processed messages.

```c
int free_datagram_store(Datagram_store *datagram);
```

Frees the entire store. Call at program shutdown.

---

## html_data.h

HTTP response headers and the HTML page store.

### Header templates

```c
#define MAIN_HADR "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: %zu\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n%s"

#define ALIVE_HADR "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n"

#define MAEG_HADR "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\nContent-Length: %zu\r\n\r\n%s"
```

Prebuilt HTTP response headers for HTML pages, SSE streams, and JSON responses.

> **To verify:**
> - `MAIN_HADR` declares `HTTP/1.0` while the others declare `HTTP/1.1` — confirm which is intended.
> - `MAEG_HADR` places `Content-Length` after the blank line that ends the header block, which makes it part of the body. Browsers will not parse the length correctly; the header order likely needs fixing.

### Structures

```c
typedef struct HTML_page HTML_page;

struct HTML_page {
    char *name;
    char *link;
    char *data;
    int size;
};
```

An HTML page loaded from disk: its name, the URL path it answers to, and its contents.

```c
typedef struct HTML_page_store HTML_page_store;

struct HTML_page_store {
    int len;
    int cap;
    HTML_page *pages;
};
```

A dynamic array of registered pages.

### Functions

```c
HTML_page *get_html_page(HTML_page_store *HPS, int idx);
```

Returns a pointer to the page at index `idx`.

```c
char *get_HP_name(HTML_page *hp);
char *get_HP_link(HTML_page *hp);
char *get_HP_data(HTML_page *hp);
int   get_HP_size(HTML_page *hp);
```

Accessors for `HTML_page` fields.

```c
HTML_page_store *init_html_struct();
```

Allocates and initializes a page store.

```c
int add_html_page(HTML_page_store *hps, char *name, int name_size,
                  char *link, int link_size, char *file_link);
```

Registers a new page: a name, the URL path clients will request (`link`), and the file path on disk to load the page from (`file_link`).

```c
int get_html_gape_by_link(HTML_page_store *hps, char *link, int link_size);
```

Looks up a page by its request path. **Returns:** the page's index in the store.

```c
int free_html_page_store(HTML_page_store *hps);
```

Frees the page store. Call at program shutdown.

```c
int applay_haders(char *buff, int buff_size, char *hadder, char *data, int data_l);
```

Formats `data` into `buff` with the given HTTP header template.

---

## files_m.h

File reading and writing for persistent data.

> **Planned change:** This module will be replaced by a proper storage backend (e.g., SQLite) for client data.

### Structures

```c
typedef struct File_unit File_unit;

struct File_unit {
    char *name;
    char *der;
};
```

A registered file: the name it is referenced by, and its path.

```c
typedef struct Files_struct Files_struct;

struct Files_struct {
    int cap;
    int len;
    File_unit *FU;
};
```

A collection of registered files.

### Functions

```c
Files_struct *init_File_struct();
```

Allocates and initializes the file struct.

```c
int add_file(Files_struct *myfiles, char *name, char *der);
```

Registers a file path under a name for later reference.

```c
int fine_fd_by_name(Files_struct *myfiles, char *name);
```

Looks up a file unit by name. **Returns:** pointer to the matching `File_unit`.

```c
int update_file(Files_struct *myfiles, char *name, char *data,
                size_t d_size, int offset);
```

Appends new data to a registered file.

| Parameter | Description |
|---|---|
| `name` | Name of the registered file unit |
| `data` | The new data to append |
| `d_size` | Size of the new data |
| `offset` | Number of bytes to step back from the end before writing |

> **Implementation note:** Files are opened in append mode and store JSON. To insert new data inside the closing bracket correctly, the writer steps `offset` bytes back from the end of the file before writing. This mechanism works but is fragile and will be revisited together with the storage-backend migration.

```c
int read_file(Files_struct *myfiles, char *name, char *buffer, size_t buffer_size);
```

Reads the entire contents of a registered file into `buffer`. The caller must ensure `buffer` is large enough for the file plus the null terminator.

```c
int free_file(Files_struct *myfiles);
```

Frees the file struct and all registered file units.

---

## Roadmap

- Rewrite `read_client_msg_copy` to be application-agnostic
- User-configurable `MAX_CLIENT` limit
- Replace `files_m.h` with a SQLite-backed storage layer
- Test server restart after `stop_server`
- Remove `view_clients_status`
