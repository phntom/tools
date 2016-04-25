#include <stdio.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char **argv) {

	int return_val = 0;

	char* new_command = NULL;

	char* tmp_folder = "/tmp";

	char* output_path = NULL;

	FILE* f = NULL;

    char* line = NULL;

	int random_number = 0;

    size_t len = 0;

    ssize_t read;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	output_path = malloc(strlen(tmp_folder) + 24);

	sprintf(output_path, "%s/.monitor_%08d_time", tmp_folder, random_number);

	new_command = malloc(26 + strlen(output_path) + strlen(argv[1]));

	if (argc > 2) {
		// print to a file and post process
		sprintf(new_command, "/usr/bin/env time -v -o %s %s", output_path, argv[1]);
	} else {
		// no newline-chars specified, printing directly to stdout
		// print to stdout
		sprintf(new_command, "/usr/bin/env time -v %s", argv[1]);
	}
        
	return_val = system(new_command);

	if (return_val < 0) {
		printf("%s: failed launching %s\n", argv[0], argv[1]);
		return EXIT_FAILURE;
	}

	if (argc > 2) {

	    f = fopen(output_path, "r");

        if (f == NULL) {
            exit(EXIT_FAILURE);
        }

        if (argc > 4) {
            printf("%s", argv[4]);
        }

        while ((read = getline(&line, &len, f)) != EOF) {
            len = strlen(line) - 1;
            if (len >= 0) {
                line[len] = '\0';
            }
            printf("%s%s", line, argv[2]);
        }

        fclose(f);

        free(line);

		if (argc > 5) {
			printf("%s\n", argv[5]);
			fflush(stdout); 
		}

		unlink(output_path);
		
	}

	free(new_command);

	return return_val;
}
