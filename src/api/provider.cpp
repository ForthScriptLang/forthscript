#include <api/provider.hpp>

extern "C" {

bool apiEvalStackPopBack(void* ctx, struct overstacked_value* buf) {
    Interpreter* interp = (Interpreter*)ctx;
    std::optional<Value> stackVal = interp->evalStack.popBack();
    if (!stackVal.has_value()) {
        return false;
    }
    Value val = interp->evalStack.popBack().value();
    *buf = *(struct overstacked_value*)(&val);
    return true;
}

bool apiEvalStackAssertDepth(size_t count, void* ctx) {
    Interpreter* interp = (Interpreter*)ctx;
    return interp->evalStack.assertDepth(count);
}

void apiEvalStackPushBack(struct overstacked_value* val, void* ctx) {
    Interpreter* interp = (Interpreter*)ctx;
    Value nativeVal = *(Value*)(val);
    interp->evalStack.pushBack(nativeVal);
}

void apiSymbolTableCreateScope(void* ctx) {
    Interpreter* interp = (Interpreter*)ctx;
    interp->symTable.createScope();
}

void apiSymbolTableLeaveScope(void* ctx) {
    Interpreter* interp = (Interpreter*)ctx;
    interp->symTable.leaveScope();
}

void apiCallInterpreter(struct overstacked_array* arr, bool new_scope,
                        void* ctx, struct overstacked_exec_result* buf) {
    Interpreter* interp = (Interpreter*)ctx;
    ExecutionResult result = interp->callInterpreter((Array*)arr, new_scope);
    *buf = *(struct overstacked_exec_result*)(&result);
}

struct overstacked_value* apiGetArrayData(struct overstacked_array* arr,
                                          void*) {
    return (struct overstacked_value*)(((Array*)arr)->values.data());
}

size_t apiGetArraySize(struct overstacked_array* arr, void*) {
    return ((Array*)arr)->values.size();
}

const char32_t* apiGetStringData(struct overstacked_string* str, void*) {
    return ((String*)str)->get().data();
}

size_t apiGetStringSize(struct overstacked_string* str, void*) {
    return ((String*)str)->get().size();
}

struct overstacked_string* apiMakeString(const char32_t* str, void* ctx) {
    Interpreter* interp = (Interpreter*)ctx;
    return (struct overstacked_string*)interp->heap.makeStringObject(str);
}

struct overstacked_array* apiMakeArray(struct overstacked_value* val,
                                       size_t size, void* ctx) {
    Interpreter* interp = (Interpreter*)ctx;
    Value nativeVal = *(Value*)(val);
    return (struct overstacked_array*)interp->heap.makeArrayObject(*(Value*)val,
                                                                   size);
}
}

struct overstacked_api_table* makeAPITable(Interpreter& interp) {
    struct overstacked_api_table* result = new struct overstacked_api_table;
    result->api_version = 0;
    result->lib_ctx = nullptr;
    result->ctx = &interp;
    result->eval_stack_pop_back = apiEvalStackPopBack;
    result->eval_stack_push_back = apiEvalStackPushBack;
    result->eval_stack_assert_depth = apiEvalStackAssertDepth;
    result->sym_table_create_scope = apiSymbolTableCreateScope;
    result->sym_table_leave_scope = apiSymbolTableLeaveScope;
    result->call_interpreter = apiCallInterpreter;
    result->get_array_data = apiGetArrayData;
    result->get_array_size = apiGetArraySize;
    result->get_string_data = apiGetStringData;
    result->get_string_size = apiGetStringSize;
    result->make_string = apiMakeString;
    result->make_array = apiMakeArray;
    return result;
}