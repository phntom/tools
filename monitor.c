#include <stdio.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char **argv) {

	int return_val = 0;

	char* new_command;

	char* tmp_folder = "/tmp";

	int random_number;

	int command_length;

	if (argc < 2) {
		printf("%s: \"command <arguments>\" \"<newline-chars>\" \"<tmp-folder>\" \"<print-before>\" \"<print-after>\"\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (argc > 3) {
		tmp_folder = argv[3];
	}

	prctl(PR_SET_PDEATHSIG, SIGHUP, 0, 0, 0);

	srand(time(NULL));

	random_number = rand() % 100000000;

	command_length = 68 + strlen(argv[1]) + strlen(tmp_folder);

	if (argc > 1) {
		command_length += strlen(argv[2]);
	}

	new_command = malloc(command_length);

	if (argc > 1) {
		// print to a file and post process
		sprintf(new_command, "/usr/bin/env time -v -o %s/.monitor_%08d_time %s", tmp_folder, random_number, argv[1]);
	} else {
		// missing newline-chars
		// print to stdout
		sprintf(new_command, "/usr/bin/env time -v %s", argv[1]);
	}
        
	return_val = system(new_command);

	if (return_val < 0) {
		printf("%s: failed launching %s\n", argv[0], argv[1]);
		return EXIT_FAILURE;
	}

	if (argc > 1) {

		sprintf(new_command, "/usr/bin/env cat %s/.monitor_%08d_time | sed ':a;N;$!ba;s/\\n/%s/g'", 
				tmp_folder, random_number, argv[2]);

		if (argc > 4) {
			printf("%s", argv[4]);
			fflush(stdout); 
		}
		
		system(new_command);

		if (argc > 5) {
			fflush(stdout);
			printf("%s\n", argv[5]);
			fflush(stdout); 
		}
		
		sprintf(new_command, "/usr/bin/env rm %s/.monitor_%08d_time", tmp_folder, random_number);
		
		system(new_command); 
	}

	free(new_command);

	return return_val;
}
