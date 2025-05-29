#include "haxsdk_il2cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>

#include "../haxsdk_backend.h"
#include "../../haxsdk_logger.h"
#include "../../haxsdk_assertion.h"

#define GET_BACKEND_API(n)                          \
    n = (t ## _ ## n)(GetProcAddress(handle, #n));  \
    HAX_LOG_DEBUG("{} {}", #n, (void*)n);           \
    HAX_ASSERT(n)

#define GET_BACKEND_API_OPT(n)                      \
    n = (t ## _ ## n)(GetProcAddress(handle, #n));  \
    HAX_LOG_DEBUG("{} {}", #n, (void*)n)

#define FIELD_ATTRIBUTE_STATIC 0x0010

using t_il2cpp_assembly_get_image           = Il2CppImage* (*)(Il2CppAssembly* assembly);
using t_il2cpp_class_from_name              = Il2CppClass* (*)(Il2CppImage* image, const char* namespaze, const char* name);
using t_il2cpp_class_from_system_type       = Il2CppClass* (*)(Il2CppReflectionType*);
using t_il2cpp_class_get_field_from_name    = Il2CppField* (*)(Il2CppClass* pClass, const char* name);
using t_il2cpp_class_get_methods            = Il2CppMethod * (*)(Il2CppClass* pClass, void** ppIter);
using t_il2cpp_class_get_static_field_data  = void* (*)(Il2CppClass*);
using t_il2cpp_class_get_type               = Il2CppType* (*)(Il2CppClass* pClass);
using t_il2cpp_domain_assembly_open         = Il2CppAssembly* (*)(Il2CppDomain* domain, const char* name);
using t_il2cpp_domain_get                   = Il2CppDomain* (*)();
using t_il2cpp_domain_get_assemblies        = Il2CppAssembly** (*)(Il2CppDomain* domain, size_t* size);
using t_il2cpp_field_static_get_value       = void (*)(Il2CppField* pField, void* pValue);
using t_il2cpp_field_static_set_value       = void (*)(Il2CppField* pField, void* pValue);
using t_il2cpp_get_corlib                   = Il2CppImage* (*)();
using t_il2cpp_image_get_assembly           = Il2CppAssembly* (*)(Il2CppImage*);
using t_il2cpp_image_get_name               = const char* (*)(Il2CppImage*);
using t_il2cpp_method_get_class             = Il2CppClass* (*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_name              = const char* (*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_param             = Il2CppType* (*)(Il2CppMethod* pMethod, uint32_t index);
using t_il2cpp_method_get_param_count       = int32_t(*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_return_type       = Il2CppType* (*)(Il2CppMethod* pMethod);
using t_il2cpp_object_new                   = Il2CppObject* (*)(Il2CppClass* pClass);
using t_il2cpp_object_unbox                 = void* (*)(Il2CppObject* obj);
using t_il2cpp_runtime_invoke               = Il2CppObject* (*)(Il2CppMethod* pMethod, void* obj, void** params, Il2CppException** exc);
using t_il2cpp_runtime_object_init          = void (*)(Il2CppObject* obj);
using t_il2cpp_string_new                   = Il2CppString* (*)(const char* str);
using t_il2cpp_thread_attach                = Il2CppThread* (*)(Il2CppDomain* domain);
using t_il2cpp_thread_detach                = void (*)(Il2CppThread*);
using t_il2cpp_type_get_name                = const char* (*)(Il2CppType* type);
using t_il2cpp_type_get_object              = Il2CppReflectionType* (*)(Il2CppType* type);
using t_il2cpp_value_box                    = Il2CppObject* (*)(Il2CppClass*, void*);
using t_il2cpp_gchandle_new                 = uint32_t (*)(Il2CppObject*, bool);
using t_il2cpp_gchandle_new_weakref         = uint32_t (*)(Il2CppObject*, bool);
using t_il2cpp_gchandle_get_target          = Il2CppObject* (*)(uint32_t);
using t_il2cpp_gchandle_free                = void (*)(uint32_t);
using t_il2cpp_class_from_il2cpp_type       = Il2CppClass* (*)(Il2CppType*);

static t_il2cpp_object_new                  il2cpp_object_new;
static t_il2cpp_object_unbox                il2cpp_object_unbox;
static t_il2cpp_runtime_object_init         il2cpp_runtime_object_init;
static t_il2cpp_domain_get                  il2cpp_domain_get;
static t_il2cpp_domain_get_assemblies       il2cpp_domain_get_assemblies;
static t_il2cpp_thread_attach               il2cpp_thread_attach;
static t_il2cpp_assembly_get_image          il2cpp_assembly_get_image;
static t_il2cpp_class_from_name             il2cpp_class_from_name;
static t_il2cpp_class_get_type              il2cpp_class_get_type;
static t_il2cpp_class_get_methods           il2cpp_class_get_methods;
static t_il2cpp_type_get_object             il2cpp_type_get_object;
static t_il2cpp_type_get_name               il2cpp_type_get_name;
static t_il2cpp_class_get_field_from_name   il2cpp_class_get_field_from_name;
static t_il2cpp_runtime_invoke              il2cpp_runtime_invoke;
static t_il2cpp_method_get_param_count      il2cpp_method_get_param_count;
static t_il2cpp_method_get_param            il2cpp_method_get_param;
static t_il2cpp_method_get_class            il2cpp_method_get_class;
static t_il2cpp_method_get_name             il2cpp_method_get_name;
static t_il2cpp_method_get_return_type      il2cpp_method_get_return_type;
static t_il2cpp_string_new                  il2cpp_string_new;
static t_il2cpp_class_get_static_field_data il2cpp_class_get_static_field_data;
static t_il2cpp_field_static_get_value      il2cpp_field_static_get_value;
static t_il2cpp_field_static_set_value      il2cpp_field_static_set_value;
static t_il2cpp_thread_detach               il2cpp_thread_detach;
static t_il2cpp_image_get_name              il2cpp_image_get_name;
static t_il2cpp_domain_assembly_open        il2cpp_domain_assembly_open;
static t_il2cpp_class_from_system_type      il2cpp_class_from_system_type;
static t_il2cpp_get_corlib                  il2cpp_get_corlib;
static t_il2cpp_image_get_assembly          il2cpp_image_get_assembly;
static t_il2cpp_value_box                   il2cpp_value_box;
static t_il2cpp_gchandle_new                il2cpp_gchandle_new;
static t_il2cpp_gchandle_new_weakref        il2cpp_gchandle_new_weakref;
static t_il2cpp_gchandle_get_target         il2cpp_gchandle_get_target;
static t_il2cpp_gchandle_free               il2cpp_gchandle_free;
static t_il2cpp_class_from_il2cpp_type      il2cpp_class_from_il2cpp_type;

namespace il2cpp
{
    void Initialize()
    {
        HMODULE handle = static_cast<HMODULE>(HaxSdk::GetBackendHandle());
        GET_BACKEND_API(il2cpp_object_new);
        GET_BACKEND_API(il2cpp_object_unbox);
        GET_BACKEND_API(il2cpp_runtime_object_init);
        GET_BACKEND_API(il2cpp_domain_get);
        GET_BACKEND_API(il2cpp_domain_get_assemblies);
        GET_BACKEND_API(il2cpp_thread_attach);
        GET_BACKEND_API(il2cpp_assembly_get_image);
        GET_BACKEND_API(il2cpp_class_from_name);
        GET_BACKEND_API(il2cpp_class_get_type);
        GET_BACKEND_API(il2cpp_class_get_methods);
        GET_BACKEND_API(il2cpp_type_get_object);
        GET_BACKEND_API(il2cpp_type_get_name);
        GET_BACKEND_API(il2cpp_class_get_field_from_name);
        GET_BACKEND_API(il2cpp_runtime_invoke);
        GET_BACKEND_API(il2cpp_method_get_param_count);
        GET_BACKEND_API(il2cpp_method_get_param);
        GET_BACKEND_API(il2cpp_method_get_class);
        GET_BACKEND_API(il2cpp_method_get_name);
        GET_BACKEND_API(il2cpp_method_get_return_type);
        GET_BACKEND_API(il2cpp_string_new);
        GET_BACKEND_API(il2cpp_field_static_get_value);
        GET_BACKEND_API(il2cpp_field_static_set_value);
        GET_BACKEND_API_OPT(il2cpp_class_get_static_field_data);
        GET_BACKEND_API(il2cpp_thread_detach);
        GET_BACKEND_API(il2cpp_image_get_name);
        GET_BACKEND_API(il2cpp_domain_assembly_open);
        GET_BACKEND_API(il2cpp_class_from_system_type);
        GET_BACKEND_API(il2cpp_get_corlib);
        GET_BACKEND_API(il2cpp_image_get_assembly);
        GET_BACKEND_API(il2cpp_value_box);
        GET_BACKEND_API(il2cpp_gchandle_new);
        GET_BACKEND_API(il2cpp_gchandle_new_weakref);
        GET_BACKEND_API(il2cpp_gchandle_get_target);
        GET_BACKEND_API(il2cpp_gchandle_free);
        GET_BACKEND_API(il2cpp_class_from_il2cpp_type);
    }
}

static void GetMethodSignature(Il2CppMethod* method, char* buff, size_t buffSize)
{
    strcpy_s(buff, buffSize, il2cpp_type_get_name(il2cpp_method_get_return_type(method)));
    strcat_s(buff, buffSize, "(");
    uint32_t nParams = il2cpp_method_get_param_count(method);
    if (nParams > 0)
    {
        strcat_s(buff, buffSize, il2cpp_type_get_name(il2cpp_method_get_param(method, 0)));
        for (uint32_t i = 1; i < nParams; ++i)
        {
            strcat_s(buff, buffSize, ",");
            strcat_s(buff, buffSize, il2cpp_type_get_name(il2cpp_method_get_param(method, i)));
        }
    }
    strcat_s(buff, buffSize, ")");
}

Il2CppObject* Il2CppObject::Box(Il2CppClass* klass, void* data)
{
    return il2cpp_value_box(klass, data);
}

Il2CppObject* Il2CppObject::New(Il2CppClass* klass)
{
    return il2cpp_object_new(klass);
}

void Il2CppObject::Ctor()
{
    return il2cpp_runtime_object_init(this);
}

Il2CppDomain* Il2CppDomain::GetRoot() 
{ 
    return il2cpp_domain_get(); 
}

Il2CppAssembly* Il2CppDomain::GetAssembly(const char* name)
{ 
    return il2cpp_domain_assembly_open(il2cpp_domain_get(), name);
}

Il2CppThread* Il2CppThread::Attach() { return il2cpp_thread_attach(il2cpp_domain_get()); }

void Il2CppThread::Detach() { il2cpp_thread_detach(this); }


Il2CppImage* Il2CppAssembly::GetImage()
{
    return il2cpp_assembly_get_image(this);
}

Il2CppImage* Il2CppImage::GetCorlib()
{
    return il2cpp_get_corlib();
}

Il2CppImage* Il2CppImage::GetUnityCore()
{
    Il2CppDomain* domain = il2cpp_domain_get();
    Il2CppAssembly* assembly = domain->GetAssembly("UnityEngine.CoreModule");
    if (!assembly)
        domain->GetAssembly("UnityEngine");
    return assembly ? assembly->GetImage() : nullptr;
}

Il2CppClass* Il2CppImage::GetClass(const char* nameSpace, const char* name)
{
    return il2cpp_class_from_name(this, nameSpace, name);
}

Il2CppField* Il2CppClass::GetField(const char* name)
{
    return il2cpp_class_get_field_from_name(this, name);
}

Il2CppMethod* Il2CppClass::GetMethod(const char* name, const char* signature)
{
    void* iter = nullptr;
    while (Il2CppMethod* method = il2cpp_class_get_methods(this, &iter))
    {
        const char* methodName = il2cpp_method_get_name(method);
        if (strcmp(methodName, name) == 0)
        {
            if (!signature || !signature[0])
                return method;

            char buff[256] = { 0 };
            GetMethodSignature(method, buff, sizeof(buff));
            if (strcmp(buff, signature) == 0)
                return method;
        }
    }
    return nullptr;
}

void* Il2CppClass::GetStaticFieldData()
{
    return il2cpp_class_get_static_field_data(this);
}

Il2CppType* Il2CppClass::GetType()
{
    return il2cpp_class_get_type(this);
}

Il2CppObject* Il2CppMethod::Invoke(void* __this, void** args, Il2CppException** ex)
{
    return il2cpp_runtime_invoke(this, __this, args, ex);
}

Il2CppReflectionType* Il2CppType::GetReflectionType()
{
    return il2cpp_type_get_object(this);
}

Il2CppClass* Il2CppType::GetClass()
{
    return il2cpp_class_from_il2cpp_type(this);
}

void Il2CppField::GetStaticValue(void* value)
{
    il2cpp_field_static_get_value(this, value);
}

void* Il2CppField::GetValuePtr(void* __this)
{
    if (IsStatic())
        return (char*)parent->GetStaticFieldData() + offset;
    return (char*)__this + offset;
}

Il2CppString* Il2CppString::New(const char* str)
{
    return il2cpp_string_new(str);
}

uint32_t Il2CppGCHandle::New(Il2CppObject* obj, bool pinned)
{
    return il2cpp_gchandle_new(obj, pinned);
}

uint32_t Il2CppGCHandle::NewWeak(Il2CppObject* obj, bool trackResurrection)
{
    return il2cpp_gchandle_new_weakref(obj, trackResurrection);
}

Il2CppObject* Il2CppGCHandle::GetTarget(uint32_t handle)
{
    return il2cpp_gchandle_get_target(handle);
}

void Il2CppGCHandle::Free(uint32_t handle)
{
    il2cpp_gchandle_free(handle);
}
