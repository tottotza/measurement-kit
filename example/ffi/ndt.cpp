/* Public domain 2017, Simone Basso <bassosimone@gmail.com. */

#include "test/winsock.hpp"

#include <measurement_kit/ffi.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static mk_task_t *task;

static void handler(int) {
    (void)write(1, "SIGINT", 6);
    mk_task_interrupt(task);
}

/*
 * Ideally it would be nice to have this written in C. For this to work,
 * however, we would need to specifically compile Measurement Kit to link
 * statically with its own version of libc++.
 */
int main() {
    task = mk_task_start(R"({
        "name": "Ndt",
        "log_level": "INFO",
        "options": {
            "net/ca_bundle_path": "cacert.pem"
        }})");
    if (task == NULL) {
        fprintf(stderr, "ERROR: cannot create/start task\n");
        exit(1);
    }
    signal(SIGINT, handler);

    while (!mk_task_is_done(task)) {
        mk_event_t *event = mk_task_wait_for_next_event(task);
        if (event == NULL) {
            fprintf(stderr, "ERROR: cannot wait for next event\n");
            exit(1);
        }
        const char *serio = mk_event_serialization(event);
        if (serio == NULL) {
            fprintf(stderr, "ERROR: cannot get event serialization\n");
            exit(1);
        }
        printf("%s\n", serio);
        fflush(stdout);
        mk_event_destroy(event);
    }

    mk_task_destroy(task);
    return 0;
}
