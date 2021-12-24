#ifndef V8PY_H_
#define V8PY_H_

#include <string>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "libplatform/libplatform.h"
#include "v8-version-string.h"
#include "v8.h"

void set_flags(const std::string &flags);


#ifdef WIN32
#define EXPORT __declspec(dllexport)
#endif

extern "C" {
    const char *version();
    bool initialize();
    bool dispose();

    class V8Evaluator {
        public:
            V8Evaluator();
            virtual ~V8Evaluator();
            void reset_context();
            std::string eval(const std::string &src);
            std::string call(const std::string &func, const std::string &args);
        protected:
            v8::Local<v8::Context> context();
            v8::Local<v8::Context> new_context(
                v8::Local<v8::ObjectTemplate> global_tmpl = v8::Local<v8::ObjectTemplate>(),
                v8::Local<v8::Value> global_obj = v8::Local<v8::Value>()
            );
            v8::Local<v8::String> new_string(const char *str);
            v8::Local<v8::Value> json_parse(v8::Local<v8::Context> context, v8::Local<v8::String> str);
            v8::Local<v8::String> json_stringify(v8::Local<v8::Context> context, v8::Local<v8::Value> value);
            std::string to_std_string(v8::Local<v8::Value> value);
        private:
            v8::Isolate *isolate_;
            v8::Persistent<v8::Context> context_;
            v8::Isolate::CreateParams create_params;
    };

    V8Evaluator *new_evaluator();
    void* free_evaluator(V8Evaluator *evaluator);
    void reset_evaluator(V8Evaluator *evaluator);
    const char* eval(V8Evaluator *evaluator, const char* script);
    void enable_heap_report(V8Evaluator *evaluator);
}

#endif // V8PY_H_
