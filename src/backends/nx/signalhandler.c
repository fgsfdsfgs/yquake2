/*
 * Copyright (C) 2011 Yamagi Burmeister
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * This is a signal handler for printing some hints to debug problem in
 * the case of a crash. On Linux a backtrace is printed. Additionally
 * a special handler for SIGINT and SIGTERM ist supplied.
 *
 * =======================================================================
 */

#include <signal.h>

#if defined(__linux__) || defined(__FreeBSD__)

#endif

#include "../../common/header/common.h"

#if defined(__linux__) || defined(__FreeBSD__)
#include <execinfo.h>

void
printBacktrace(int sig)
{
	static void *array[15];
	size_t size;
	char **strings;
	int i;

	size = backtrace(array, 15);
	strings = backtrace_symbols(array, size);

	printf("Product:      Yamagi Quake II\n");
	printf("Version:      %s\n", YQ2VERSION);
	printf("Platform:     %s\n", YQ2OSTYPE);
	printf("Architecture: %s\n", YQ2ARCH);
	printf("Compiler:     %s\n", __VERSION__);
	printf("Signal:       %i\n", sig);
	printf("\nBacktrace:\n");

	for (i = 0; i < size; i++)
	{
		printf("  %s\n", strings[i]);
	}

	printf("\n");
}

#elif defined(__SWITCH__)

static FILE *flog = NULL;

void
printBacktrace(int sig)
{
	fprintf(flog, "Product:      Yamagi Quake II\n");
	fprintf(flog, "Version:      %s\n", YQ2VERSION);
	fprintf(flog, "Platform:     %s\n", YQ2OSTYPE);
	fprintf(flog, "Architecture: %s\n", YQ2ARCH);
	fprintf(flog, "Compiler:     %s\n", __VERSION__);
	fprintf(flog, "Signal:       %i\n", sig);
	fprintf(flog, "\nBacktrace:\n");
	fprintf(flog, "  Not available on this plattform.\n\n");
}

#else

void
printBacktrace(int sig)
{
	printf("Product:      Yamagi Quake II\n");
	printf("Version:      %s\n", YQ2VERSION);
	printf("Platform:     %s\n", YQ2OSTYPE);
	printf("Architecture: %s\n", YQ2ARCH);
	printf("Compiler:     %s\n", __VERSION__);
	printf("Signal:       %i\n", sig);
	printf("\nBacktrace:\n");
	printf("  Not available on this plattform.\n\n");
}

#endif

#ifdef __SWITCH__
void
signalhandler(int sig)
{
	flog = fopen("/switch/nxquake2/crash.log", "w");
	if (flog)
	{
		fprintf(flog, "\n=======================================================\n");
		fprintf(flog, "\nYamagi Quake II crashed! This should not happen...\n");
		fprintf(flog, "\nMake sure that you're using the last version. It can\n");
		fprintf(flog, "be found at http://www.yamagi.org/quake2. If you do,\n");
		fprintf(flog, "send a bug report to quake2@yamagi.org and include:\n\n");
		fprintf(flog, " - This output\n");
		fprintf(flog, " - The conditions that triggered the crash\n");
		fprintf(flog, " - How to reproduce the crash (if known)\n");
		fprintf(flog, " - The following files. None of them contains private\n");
		fprintf(flog, "   data. They're necessary to analyze the backtrace:\n\n");
		fprintf(flog, "    - quake2 (the executable / binary)\n\n");
		fprintf(flog, "    - game.so (the game.so of the mod you were playing\n");
		fprintf(flog, "      when the game crashed. baseq2/game.so for the\n");
		fprintf(flog, "      main game)\n\n");
		fprintf(flog, " - Any other data which you think might be useful\n");
		fprintf(flog, "\nThank you very much for your help, making Yamagi Quake\n");
		fprintf(flog, "II an even better source port. It's much appreciated.\n");
		fprintf(flog, "\n=======================================================\n\n");

		printBacktrace(sig);

		/* make sure this is written */
		fflush(flog);
		fclose(flog);
	}

	/* reset signalhandler */
	signal(SIGSEGV, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGABRT, SIG_DFL);

	/* pass signal to the os */
	raise(sig);
}
#else
void
signalhandler(int sig)
{
	printf("\n=======================================================\n");
	printf("\nYamagi Quake II crashed! This should not happen...\n");
	printf("\nMake sure that you're using the last version. It can\n");
	printf("be found at http://www.yamagi.org/quake2. If you do,\n");
	printf("send a bug report to quake2@yamagi.org and include:\n\n");
	printf(" - This output\n");
	printf(" - The conditions that triggered the crash\n");
	printf(" - How to reproduce the crash (if known)\n");
	printf(" - The following files. None of them contains private\n");
	printf("   data. They're necessary to analyze the backtrace:\n\n");
	printf("    - quake2 (the executable / binary)\n\n");
	printf("    - game.so (the game.so of the mod you were playing\n");
	printf("      when the game crashed. baseq2/game.so for the\n");
	printf("      main game)\n\n");
	printf(" - Any other data which you think might be useful\n");
	printf("\nThank you very much for your help, making Yamagi Quake\n");
	printf("II an even better source port. It's much appreciated.\n");
	printf("\n=======================================================\n\n");

	printBacktrace(sig);

	/* make sure this is written */
	fflush(stdout);

	/* reset signalhandler */
	signal(SIGSEGV, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGABRT, SIG_DFL);

	/* pass signal to the os */
	raise(sig);
}
#endif

void
terminate(int sig)
{
	Cbuf_AddText("quit");
}

void
registerHandler(void)
{
	/* Crash */
	signal(SIGSEGV, signalhandler);
	signal(SIGILL, signalhandler);
	signal(SIGFPE, signalhandler);
	signal(SIGABRT, signalhandler);

	/* User abort */
	signal(SIGINT, terminate);
	signal(SIGTERM, terminate);
}
