#include <cstdarg>

#include "config.h"
#include "fs.h"
#include "file.h"

namespace Log {
    static std::shared_ptr<FS::file::Context> log_file;
    static s64 offset = 0;
    
    void Init(void) {
        if (!cfg.dev_options)
            return;

		log_file = FS::file::open("sdmc:/switch/NX-Shell/debug.log", FS::file::Mode::W);
        
            
        if (!log_file)
            return;
    }
    
    void Error(const char *data, ...) {
        if (!cfg.dev_options)
            return;
         
        char buf[256 + FS_MAX_PATH];
        va_list args;
        va_start(args, data);
        std::vsnprintf(buf, sizeof(buf), data, args);
        va_end(args);
        
        std::string error_string = "[ERROR] ";
        error_string.append(buf);

        std::printf("%s", error_string.c_str());

		if(log_file)
		{
			log_file->write((const u8*)error_string.data(), error_string.length());
		}
    }
    
    void Exit(void) {
        if (!cfg.dev_options)
            return;
		if(log_file)
		{
			log_file->close();
			log_file = nullptr;
		}
    }
}
