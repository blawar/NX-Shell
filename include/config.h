
#ifndef NX_SHELL_CONFIG_H
#define NX_SHELL_CONFIG_H

#include <switch.h>
#include <uri.h>

typedef struct {
	int sort = 0;
	int lang = 1;
	bool dev_options = false;
	bool image_filename = false;
	Uri cwd;
} config_t;

extern config_t cfg;

namespace Config {
	int Save(config_t config);
	int Load(void);
}

#endif
