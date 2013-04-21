#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <audio.h>
#include <cmdline.h>
#include <config.h>
#include <machine.h>
#include <video.h>

static void print_usage(bool error);

extern struct machine __machines_begin, __machines_end;
extern struct audio_frontend __audio_frontends_begin, __audio_frontends_end;
extern struct video_frontend __video_frontends_begin, __video_frontends_end;

void print_usage(bool error)
{
	FILE *stream = error ? stderr : stdout;
	struct machine *m;
	struct audio_frontend *af;
	struct video_frontend *vf;

	fprintf(stream, "Usage: emux [OPTION]...\n");
	fprintf(stream, "Emulates various machines (consoles, arcades).\n");

	/* Don't print full usage in case of error */
	if (error) {
		fprintf(stream, "Try `emux --help' for more information.\n");
		return;
	}

	/* Print options */
	fprintf(stream, "\n");
	fprintf(stream, "Emux options:\n");
	fprintf(stream, "  --machine=MACH    Selects machine to emulate\n");
	fprintf(stream, "  --audio=AUDIO     Selects audio frontend\n");
	fprintf(stream, "  --video=VIDEO     Selects video frontend\n");
	fprintf(stream, "  --width=WIDTH     Overrides window width\n");
	fprintf(stream, "  --height=HEIGHT   Overrides window height\n");
	fprintf(stream, "  --help            Display this help and exit\n");
	fprintf(stream, "\n");

	/* Print supported machines */
	fprintf(stream, "Supported machines:\n");
	for (m = &__machines_begin; m < &__machines_end; m++)
		fprintf(stream, "  %s (%s)\n", m->name, m->description);
	fprintf(stream, "\n");

	/* Print audio frontends */
	fprintf(stream, "Audio frontends:\n");
	for (af = &__audio_frontends_begin; af < &__audio_frontends_end; af++)
		fprintf(stream, "  %s\n", af->name);
	fprintf(stream, "\n");

	/* Print video frontends */
	fprintf(stream, "Video frontends:\n");
	for (vf = &__video_frontends_begin; vf < &__video_frontends_end; vf++)
		fprintf(stream, "  %s\n", vf->name);
	fprintf(stream, "\n");

	/* Display project related info */
	fprintf(stream, "Report bugs to: sronsse@gmail.com\n");
	fprintf(stream, "Project page: <https://github.com/sronsse/emux>\n");
}

int main(int argc, char *argv[])
{
	char *machine;
	int i;

	/* Initialize random seed */
	srand(time(NULL));

	/* Print version and command line */
	fprintf(stdout, "Emux version %s\n", PACKAGE_VERSION);
	fprintf(stdout, "Command line:");
	for (i = 0; i < argc; i++)
		fprintf(stdout, " %s", argv[i]);
	fprintf(stdout, "\n");

	/* Initialize command line and parse it */
	cmdline_init(argc, argv);

	/* Check if user requires help */
	if (cmdline_parse_bool("help", NULL)) {
		print_usage(false);
		return 0;
	}

	/* Checks for machine selection */
	if (!cmdline_parse_string("machine", &machine)) {
		print_usage(true);
		return 1;
	}

	/* Initialize machine */
	if (!machine_init(machine))
		return 1;

	/* Run machine */
	machine_run();

	/* Deinitialize machine */
	machine_deinit();

	return 0;
}

