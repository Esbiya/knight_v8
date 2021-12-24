#include "v8py.h"

using namespace v8;

static std::unique_ptr<Platform> _platform = nullptr;

void set_flags(const std::string &flags) {
    V8::SetFlagsFromString(flags.c_str(), static_cast<int>(flags.length()));
}

bool initialize() {
    if (_platform) {
        return false;
    }
    _platform = v8::platform::NewDefaultPlatform();
    V8::InitializePlatform(_platform.get());
    return V8::Initialize();
}

bool dispose() {
    if (!_platform) {
        return false;
    }
    V8::Dispose();
    V8::ShutdownPlatform();
    return true;
}

V8Evaluator::V8Evaluator() {
    create_params.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate_ = Isolate::New(create_params);
    Locker locker(isolate_);
    Isolate::Scope isolate_scope(isolate_);
    HandleScope handle_scope(isolate_);
    context_.Reset(isolate_, new_context());
}

V8Evaluator::~V8Evaluator() {
    isolate_->Dispose();
    delete create_params.array_buffer_allocator;
}

Local<Context> V8Evaluator::context() {
    // assert(context_.IsEmpty());
    return Local<Context>::New(isolate_, context_);
}

Local<Context> V8Evaluator::new_context(Local<ObjectTemplate> global_tmpl, Local<Value> global_obj) {
    if (global_tmpl.IsEmpty() && global_obj.IsEmpty()) {
        Local<ObjectTemplate> global = ObjectTemplate::New(isolate_);
        return Context::New(isolate_, nullptr, global);
    } else {
        return Context::New(isolate_, nullptr, global_tmpl, global_obj);
    }
}

void V8Evaluator::reset_context() {
    Locker locker(isolate_);
    Isolate::Scope isolate_scope(isolate_);
    HandleScope handle_scope(isolate_);
    context_.Reset(isolate_, new_context());
}

Local<String> V8Evaluator::new_string(const char *str) {
    return String::NewFromUtf8(isolate_, str ? str : "", NewStringType::kNormal);
}

std::string V8Evaluator::to_std_string(Local<Value> value) {
    String::Utf8Value str(isolate_, value);
    return *str ? *str : "error: cannot convert to string";
}

Local<Value> V8Evaluator::json_parse(Local<Context> context, Local<String> str) {
    Local<Object> global = context->Global();
    Local<Object> json = global->Get(context, new_string("JSON")).ToLocalChecked()->ToObject(context).ToLocalChecked();
    Local<Function> parse = Local<Function>::Cast(json->Get(context, new_string("parse")).ToLocalChecked());
    Local<Value> result;
    Local<Value> value = str;
    if (!parse->Call(context, json, 1, &value).ToLocal(&result)) {
        return Local<Value>(); // empty
    } else {
        return result;
    }
}

Local<String> V8Evaluator::json_stringify(Local<Context> context, Local<Value> value) {
    Local<Object> global = context->Global();
    Local<Object> json = global->Get(context, new_string("JSON")).ToLocalChecked()->ToObject(context).ToLocalChecked();
    Local<Function> stringify = Local<Function>::Cast(json->Get(context, new_string("stringify")).ToLocalChecked());
    Local<Value> result;
    if (!stringify->Call(context, json, 1, &value).ToLocal(&result)) {
        return new_string("");
    } else {
        return result->ToString(context).ToLocalChecked();
    }
}

std::string V8Evaluator::eval(const std::string &src) {
    Locker locker(isolate_);
    Isolate::Scope isolate_scope(isolate_);
    HandleScope handle_scope(isolate_);
    Local<Context> context = this->context();
    Context::Scope context_scope(context);
    TryCatch try_catch(isolate_);
    Local<String> source = new_string(src.c_str());
    Local<String> name = new_string("v8py");
    ScriptOrigin origin(name);
    Local<Script> script;
    if (!Script::Compile(context, source, &origin).ToLocal(&script)) {
        return to_std_string(try_catch.Exception());
    } else {
        Local<Value> result;
        if (!script->Run(context).ToLocal(&result)) {
            Local<Value> stack;
            if (!try_catch.StackTrace(context).ToLocal(&stack)) {
                return to_std_string(try_catch.Exception());
            } else {
                return to_std_string(stack);
            }
        } else {
            return to_std_string(json_stringify(context, result));
        }
    }
}

std::string V8Evaluator::call(const std::string &func, const std::string &args) {
    Locker locker(isolate_);
    Isolate::Scope isolate_scope(isolate_);
    HandleScope handle_scope(isolate_);
    Local<Context> context = this->context();
    Context::Scope context_scope(context);
    TryCatch try_catch(isolate_);
    Local<Object> global = context->Global();
    Local<Value> result;
    if (!global->Get(context, new_string(func.c_str())).ToLocal(&result)) {
        return to_std_string(try_catch.Exception());
    } else if (!result->IsFunction()) {
        return "TypeError: '" + func + "' is not a function";
    }
    Local<Function> function = Handle<Function>::Cast(result);
    Local<Function> apply = Handle<Function>::Cast(function->Get(context, new_string("apply")).ToLocalChecked());

    Local<Value> arguments = json_parse(context, new_string(args.c_str()));

    if (arguments.IsEmpty() || !arguments->IsArray()) {
        return "TypeError: '" + args + "' is not an array";
    }
    Local<Value> values[] = {function, arguments};
    if (!apply->Call(context, function, 2, values).ToLocal(&result)) {
        return to_std_string(try_catch.Exception());
    } else {
        return to_std_string(json_stringify(context, result));
    }
}

V8Evaluator *new_evaluator() {
    return new V8Evaluator();
}

void* free_evaluator(V8Evaluator *evaluator) {
    if (evaluator != nullptr) {
        delete evaluator;
        evaluator = nullptr;
    }
    return (void*) evaluator;
}

void reset_evaluator(V8Evaluator *evaluator) {
    evaluator->reset_context();
}

const char *eval(V8Evaluator *evaluator, const char* script) {
    std::string value = evaluator->eval(script);
    char *ret = new char[value.length()];
    strcpy(ret, value.c_str());
    return ret;
}

const char *version() {
    return V8_VERSION_STRING;
}
