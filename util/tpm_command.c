/** \file
 * busybox like wrapper for the various TPM commands.
 */

#include <stdio.h>
#include <string.h>
#include "tpm_command.h"

tpm_command_t * tpm_commands;

int main(int argc, char **argv)
{
	tpm_command_t * cmd = tpm_commands;

	if (argc == 1)
	{
		// display all the commands
		while(cmd)
		{
			printf("%s\n", cmd->name);
			cmd = cmd->next;
		}

		return 0;
	}

	const char * cmd_name = argv[1];
	argc--;
	argv++;

	// find the command
	while(cmd)
	{
		if (strcmp(cmd_name, cmd->name) == 0)
			return cmd->main(argc, argv);
		cmd = cmd->next;
	}

	// here?  oops
	fprintf(stderr, "%s: no such command\n", cmd_name);
	return -1;
}
