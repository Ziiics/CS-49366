#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

int main() {
	struct passwd *user = getpwuid(getuid());

	if (user != NULL) {
		char *uname = user->pw_name;
		char *udir = user->pw_dir;
		char *ushell = user->pw_shell;
		const char *uenv = getenv("DISPLAY") ? getenv("DISPLAY") : "not set";

		printf("My username is %s", uname);
		printf(", my userid is %d", getuid());
		printf(", and my home directory is %s.\n", udir);

		if (getuid() / 2 == 0) {
			printf("My SHELL is %s.\n", ushell);
		} else {
			printf("The value of my DISPLAY variable is %s.\n", uenv);
		}
	} else {
		printf("Failed to retrieve user's information.\n");
	}
	return 0;
}
