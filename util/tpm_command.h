/** \file
 * Constructor to keep track of the TPM functions that
 * we have linked into the tpm busybox-like mega binary.
 *
 * This saves several hundred KB of space in the initrd.
 */
#pragma once

typedef struct tpm_command tpm_command_t;

struct tpm_command {
	const char * name;
	int (*main)(int argc, char *argv[]);
	void (*help)(void);
	tpm_command_t * next;
};

extern tpm_command_t * tpm_commands;

#define tpm_command_register(name,main,help) \
static void \
__attribute__((constructor)) \
tpm_command_register(void) \
{ \
	static tpm_command_t cmd = { name, main, help, NULL }; \
	cmd.next = tpm_commands; \
	tpm_commands = &cmd; \
} \

