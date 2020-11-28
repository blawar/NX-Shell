#include <cstdio>
#include <cstring>
#include <jansson.h>

#include "config.h"
#include "fs.h"
#include "log.h"
#include "file.h"
#include "directory.h"

#define CONFIG_VERSION 2

config_t cfg;

namespace Config {
    static const char *config_file = "{\n\t\"config_ver\": %d,\n\t\"sort\": %d,\n\t\"lang\": %d,\n\t\"dev_options\": %d,\n\t\"image_filename\": %d,\n\t\"last_dir\": \"%s\"\n}";
    static int config_version_holder = 0;
    static const int buf_size = 128 + FS_MAX_PATH;
    
    int Save(config_t config) {
        Result ret = 0;
        char *buf = new char[buf_size];
        u64 len = std::snprintf(buf, buf_size, config_file, CONFIG_VERSION, config.sort, config.lang, config.dev_options, config.image_filename, config.cwd);

        
        auto file = FS::file::open("sdmc:/switch/NX-Shell/config.json", FS::file::Mode::W);
        if (!file) {
            Log::Error("fsFsOpenFile(/switch/NX-Shell/config.json) failed\n");
            delete[] buf;
            return ret;
        }

		file->write((const u8*)buf, len);
		file->flush();
		file->close();
        
        delete[] buf;
        return 0;
    }
    
    static void SetDefault(config_t *config) {
        config->sort = 0;
        config->lang = 1;
        config->dev_options = false;
        config->image_filename = false;
		config->cwd = "sdmc:/";
    }
    
    int Load(void) {

        if (!FS::directory::exists("sdmc:/switch/"))
            FS::directory::create("sdmc:/switch");

        if (!FS::directory::exists("sdmc:/switch/NX-Shell/"))
			FS::directory::create("sdmc:/switch/NX-Shell");
            
        if (!FS::file::exists("sdmc:/switch/NX-Shell/config.json")) {
            Config::SetDefault(&cfg);
            return Config::Save(cfg);
        }
        
		auto file = FS::file::open("sdmc:/switch/NX-Shell/config.json", FS::file::Mode::R);
        if (!file)
            return -1;
        
        s64 size = file->size();

		if(!size)
		{
			return -1;
		}

		std::vector<u8> buf;
        if (!file->read(buf, size)) {
            return -1;
        }
        
        file->close();
		buf.push_back(0);
        
        json_t *root;
        json_error_t error;
        root = json_loads((const char*)&buf.front(), 0, &error);
        
        if (!root) {
            printf("error: on line %d: %s\n", error.line, error.text);
            return -1;
        }
        
        json_t *config_ver = json_object_get(root, "config_ver");
        config_version_holder = json_integer_value(config_ver);
        
        json_t *sort = json_object_get(root, "sort");
        cfg.sort = json_integer_value(sort);

        json_t *lang = json_object_get(root, "lang");
        cfg.lang = json_integer_value(lang);
        
        json_t *dev_options = json_object_get(root, "dev_options");
        cfg.dev_options = json_integer_value(dev_options);

        json_t *image_filename = json_object_get(root, "image_filename");
        cfg.image_filename = json_integer_value(image_filename);
        
        json_t *last_dir = json_object_get(root, "last_dir");
		cfg.cwd = json_string_value(last_dir);
        
		if(!FS::directory::exists(cfg.cwd))
			cfg.cwd = "sdmc:/";
            
        // Delete config file if config file is updated. This will rarely happen.
        if (config_version_holder < CONFIG_VERSION) {
            FS::file::unlink("sdmc:/switch/NX-Shell/config.json");
            Config::SetDefault(&cfg);
            return Config::Save(cfg);
        }
        
        json_decref(root);
        return 0;
    }
}
