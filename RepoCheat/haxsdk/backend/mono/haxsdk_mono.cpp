#include "haxsdk_mono.h"

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

using GFunc                                 = void(__cdecl*)(void* data, void* user_data);
using t_mono_assembly_foreach               = void (*)(GFunc, void*);
using t_mono_assembly_get_image             = MonoImage* (*)(MonoAssembly*);
using t_mono_class_from_name                = MonoClass* (*)(MonoImage*, const char*, const char*);
using t_mono_class_get_field_from_name      = MonoField * (*)(MonoClass*, const char*);
using t_mono_class_get_method_from_name     = MonoMethod* (*)(MonoClass*, const char*, int);
using t_mono_class_get_methods              = MonoMethod* (*)(MonoClass*, void**);
using t_mono_class_get_name                 = const char* (*)(MonoClass*);
using t_mono_class_get_namespace            = const char* (*)(MonoClass*);
using t_mono_class_get_type                 = MonoType* (*)(MonoClass*);
using t_mono_class_vtable                   = MonoVTable* (*)(MonoDomain*, MonoClass*);
using t_mono_compile_method                 = void* (*)(MonoMethod*);
using t_mono_domain_assembly_open           = MonoAssembly* (*)(MonoDomain*, const char*);
using t_mono_domain_get                     = MonoDomain* (*)();
using t_mono_domain_get_friendly_name       = const char* (*)(MonoDomain*);
using t_mono_domain_get_id                  = int32_t (*)(MonoDomain*);
using t_mono_field_static_get_value         = void (*)(MonoVTable*, MonoField*, void*);
//using t_mono_field_static_get_value         = void (*)(MonoVTable*, MonoClassField*, void*);
using t_mono_field_static_set_value         = void (*)(MonoVTable*, MonoField*, void*);
using t_mono_get_corlib                     = MonoImage* (*)();
using t_mono_get_root_domain                = MonoDomain* (*)();
using t_mono_image_get_assembly             = MonoAssembly* (*)(MonoImage*);
using t_mono_image_get_name                 = const char* (*)(MonoImage*);
//using t_mono_jit_info_get_method            = MonoMethod* (*)(void*);
//using t_mono_jit_info_table_find            = void* (*)(MonoDomain*, void*);
//using t_mono_lookup_internal_call           = void* (*)(void*);
using t_mono_method_get_name                = const char* (*)(MonoMethod*);
using t_mono_method_get_unmanaged_thunk     = void* (*)(MonoMethod*);
using t_mono_method_signature               = MonoMethodSignature* (*)(MonoMethod*);
using t_mono_object_get_class               = MonoClass* (*)(MonoObject*);
//using t_mono_object_get_class               = MonoClass* (*)(MonoObject*);
using t_mono_object_new                     = MonoObject * (*)(MonoDomain*, MonoClass*);
using t_mono_object_to_string               = MonoString* (*)(MonoObject*);
using t_mono_object_unbox                   = void* (*)(MonoObject*);
using t_mono_print_unhandled_exception      = void (*)(MonoObject*);
using t_mono_runtime_invoke                 = MonoObject* (*)(MonoMethod*, void*, void**, MonoException**);
using t_mono_runtime_object_init            = void (*)(MonoObject*);
using t_mono_signature_get_param_count      = uint32_t (*)(MonoMethodSignature*);
using t_mono_signature_get_params           = MonoType* (*)(MonoMethodSignature*, void**);
using t_mono_signature_get_return_type      = MonoType* (*)(MonoMethodSignature*);
using t_mono_string_new                     = MonoString* (*)(MonoDomain*, const char*);
//using t_mono_string_new_wrapper             = MonoString* (*)(const char*);
using t_mono_string_to_utf8                 = char* (*)(MonoString*);
using t_mono_thread_attach                  = MonoThread* (*)(MonoDomain*);
using t_mono_thread_current                 = MonoThread* (*)();
using t_mono_thread_detach                  = void (*)(MonoThread*);
using t_mono_type_get_name                  = char* (*)(MonoType*);
using t_mono_type_get_object                = MonoReflectionType* (*)(MonoDomain*, MonoType*);
using t_mono_type_is_void                   = bool (*)(MonoType*);
using t_mono_value_box                      = MonoObject* (*)(MonoDomain* domain, MonoClass* klass, void* val);
using t_mono_vtable_get_static_field_data   = void* (*)(MonoVTable*);
using t_mono_gchandle_new                   = uint32_t (*)(MonoObject*, bool);
using t_mono_gchandle_new_weakref           = uint32_t (*)(MonoObject*, bool);
using t_mono_gchandle_get_target            = MonoObject* (*)(uint32_t);
using t_mono_gchandle_free                  = void (*)(uint32_t);
using t_mono_class_from_mono_type           = MonoClass* (*)(MonoType*);

static t_mono_assembly_foreach              mono_assembly_foreach;
static t_mono_assembly_get_image            mono_assembly_get_image;
static t_mono_class_from_name               mono_class_from_name;
static t_mono_class_get_field_from_name     mono_class_get_field_from_name;
static t_mono_class_get_method_from_name    mono_class_get_method_from_name;
static t_mono_class_get_methods             mono_class_get_methods;
static t_mono_class_get_name                mono_class_get_name;
static t_mono_class_get_namespace           mono_class_get_namespace;
static t_mono_class_get_type                mono_class_get_type;
static t_mono_class_vtable                  mono_class_vtable;
static t_mono_compile_method                mono_compile_method;
static t_mono_domain_assembly_open          mono_domain_assembly_open;
static t_mono_domain_get                    mono_domain_get;
static t_mono_domain_get_friendly_name      mono_domain_get_friendly_name;
static t_mono_domain_get_id                 mono_domain_get_id;
static t_mono_field_static_get_value        mono_field_static_get_value;
static t_mono_field_static_set_value        mono_field_static_set_value;
static t_mono_get_corlib                    mono_get_corlib;
static t_mono_get_root_domain               mono_get_root_domain;
static t_mono_image_get_assembly            mono_image_get_assembly;
static t_mono_image_get_name                mono_image_get_name;
//static t_mono_jit_info_get_method           mono_jit_info_get_method;
//static t_mono_jit_info_table_find           mono_jit_info_table_find;
//static t_mono_lookup_internal_call          mono_lookup_internal_call;
static t_mono_method_get_name               mono_method_get_name;
static t_mono_method_get_unmanaged_thunk    mono_method_get_unmanaged_thunk;
static t_mono_method_signature              mono_method_signature;
static t_mono_object_get_class              mono_object_get_class;
static t_mono_object_new                    mono_object_new;
static t_mono_object_to_string              mono_object_to_string;
static t_mono_object_unbox                  mono_object_unbox;
static t_mono_runtime_invoke                mono_runtime_invoke;
static t_mono_runtime_object_init           mono_runtime_object_init;
static t_mono_signature_get_param_count     mono_signature_get_param_count;
static t_mono_signature_get_params          mono_signature_get_params;
static t_mono_signature_get_return_type     mono_signature_get_return_type;
static t_mono_string_new                    mono_string_new;
//static t_mono_string_new_wrapper            mono_string_new_wrapper;
static t_mono_string_to_utf8                mono_string_to_utf8;
static t_mono_thread_attach                 mono_thread_attach;
static t_mono_thread_current                mono_thread_current;
static t_mono_thread_detach                 mono_thread_detach;
static t_mono_type_get_name                 mono_type_get_name;
static t_mono_type_get_object               mono_type_get_object;
static t_mono_type_is_void                  mono_type_is_void;
static t_mono_value_box                     mono_value_box;
static t_mono_vtable_get_static_field_data  mono_vtable_get_static_field_data;
static t_mono_gchandle_new                  mono_gchandle_new;
static t_mono_gchandle_new_weakref          mono_gchandle_new_weakref;
static t_mono_gchandle_get_target           mono_gchandle_get_target;
static t_mono_gchandle_free                 mono_gchandle_free;
static t_mono_class_from_mono_type          mono_class_from_mono_type;

namespace mono
{
    void Initialize()
    {
        HMODULE handle = static_cast<HMODULE>(HaxSdk::GetBackendHandle());
        GET_BACKEND_API(mono_assembly_foreach);
        GET_BACKEND_API(mono_assembly_get_image);
        GET_BACKEND_API(mono_class_from_name);
        GET_BACKEND_API(mono_class_get_field_from_name);
        GET_BACKEND_API(mono_class_get_methods);
        GET_BACKEND_API(mono_class_get_name);
        GET_BACKEND_API(mono_class_get_namespace);
        GET_BACKEND_API(mono_class_get_type);
        GET_BACKEND_API(mono_class_vtable);
        GET_BACKEND_API(mono_compile_method);
        GET_BACKEND_API(mono_domain_assembly_open);
        GET_BACKEND_API(mono_domain_get);
        GET_BACKEND_API(mono_field_static_get_value);
        GET_BACKEND_API(mono_field_static_set_value);
        GET_BACKEND_API(mono_get_root_domain);
        GET_BACKEND_API(mono_image_get_name);
        //GET_BACKEND_API(mono_jit_info_get_method);
        //GET_BACKEND_API(mono_jit_info_table_find);
        //GET_BACKEND_API(mono_lookup_internal_call);
        GET_BACKEND_API(mono_method_get_name);
        GET_BACKEND_API(mono_method_signature);
        GET_BACKEND_API(mono_object_get_class);
        GET_BACKEND_API(mono_object_new);
        GET_BACKEND_API(mono_object_unbox);
        GET_BACKEND_API(mono_runtime_invoke);
        GET_BACKEND_API(mono_runtime_object_init);
        GET_BACKEND_API(mono_signature_get_param_count);
        GET_BACKEND_API(mono_signature_get_params);
        GET_BACKEND_API(mono_signature_get_return_type);
        GET_BACKEND_API(mono_string_new);
        GET_BACKEND_API(mono_string_to_utf8);
        GET_BACKEND_API(mono_thread_attach);
        GET_BACKEND_API(mono_thread_detach);
        GET_BACKEND_API(mono_type_get_name);
        GET_BACKEND_API(mono_type_get_object);
        GET_BACKEND_API(mono_vtable_get_static_field_data);
        GET_BACKEND_API(mono_thread_current);
        GET_BACKEND_API_OPT(mono_method_get_unmanaged_thunk);
        GET_BACKEND_API_OPT(mono_object_to_string);
        GET_BACKEND_API_OPT(mono_type_is_void);
        GET_BACKEND_API_OPT(mono_domain_get_friendly_name);
        GET_BACKEND_API(mono_domain_get_id);
        GET_BACKEND_API(mono_class_get_method_from_name);
        GET_BACKEND_API(mono_get_corlib);
        GET_BACKEND_API(mono_image_get_assembly);
        //GET_BACKEND_API(mono_string_new_wrapper);
        GET_BACKEND_API(mono_value_box);
        GET_BACKEND_API(mono_gchandle_new);
        GET_BACKEND_API(mono_gchandle_new_weakref);
        GET_BACKEND_API(mono_gchandle_get_target);
        GET_BACKEND_API(mono_gchandle_free);
        GET_BACKEND_API(mono_class_from_mono_type);
    }
}

static void _cdecl AssemblyEnumerator(void* assembly, void* params)
{
    MonoAssembly* monoAssembly = (MonoAssembly*)assembly;
    auto converted = (std::pair<const char*, MonoAssembly**>*)params;
    MonoImage* monoImage = mono_assembly_get_image(monoAssembly);
    const char* assemblyName = mono_image_get_name(monoImage);
    if (strcmp(assemblyName, converted->first) == 0)
        *converted->second = monoAssembly;
}

static void GetMethodSignature(MonoMethod* method, char* buff, size_t buffSize)
{
    MonoMethodSignature* signature = mono_method_signature(method);
    MonoType* returnType = mono_signature_get_return_type(signature);
    strcpy_s(buff, buffSize, mono_type_get_name(returnType));
    strcat_s(buff, buffSize, "(");
    uint32_t nParams = mono_signature_get_param_count(signature);
    if (nParams > 0)
    {
        void* iter = nullptr;
        strcat_s(buff, buffSize, mono_type_get_name(mono_signature_get_params(signature, &iter)));
        while (MonoType* paramType = mono_signature_get_params(signature, &iter))
        {
            strcat_s(buff, buffSize, ",");
            strcat_s(buff, buffSize, mono_type_get_name(paramType));
        }
    }
    strcat_s(buff, buffSize, ")");
}

MonoObject* MonoObject::Box(MonoClass* klass, void* data)
{
    return mono_value_box(mono_get_root_domain(), klass, data);
}

MonoObject* MonoObject::New(MonoClass* klass)
{
    return mono_object_new(mono_get_root_domain(), klass);
}

void MonoObject::Ctor()
{
    return mono_runtime_object_init(this);
}

MonoDomain* MonoDomain::GetRoot() 
{ 
    return mono_get_root_domain(); 
}

MonoAssembly* MonoDomain::GetAssembly(const char* name)
{
    MonoAssembly* assembly = nullptr;
    auto params = std::make_pair(name, &assembly);
    mono_assembly_foreach(AssemblyEnumerator, &params);
    return assembly;
}

MonoThread* MonoThread::Attach() 
{ 
    return mono_thread_attach(mono_get_root_domain());
}

void MonoThread::Detach() 
{ 
    mono_thread_detach(this); 
}

MonoImage* MonoAssembly::GetImage()
{
    return mono_assembly_get_image(this);
}

MonoImage* MonoImage::GetCorlib()
{
    return mono_get_corlib();
}

MonoImage* MonoImage::GetUnityCore()
{
    MonoDomain* domain = mono_get_root_domain();
    MonoAssembly* assembly = domain->GetAssembly("UnityEngine.CoreModule");
    if (!assembly)
        domain->GetAssembly("UnityEngine");
    return assembly ? assembly->GetImage() : nullptr;
}

MonoClass* MonoImage::GetClass(const char* nameSpace, const char* name)
{
    return mono_class_from_name(this, nameSpace, name);
}

MonoVTable* MonoClass::GetVTable()
{
    return mono_class_vtable(mono_get_root_domain(), this);
}

MonoField* MonoClass::GetField(const char* name)
{
    return mono_class_get_field_from_name(this, name);
}

MonoMethod* MonoClass::GetMethod(const char* name, const char* signature)
{
    void* iter = nullptr;
    while (MonoMethod* method = mono_class_get_methods(this, &iter))
    {
        const char* methodName = mono_method_get_name(method);
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

void* MonoClass::GetStaticFieldData()
{
    return mono_vtable_get_static_field_data(GetVTable());
}

MonoType* MonoClass::GetType()
{
    return mono_class_get_type(this);
}

const char* MonoClass::GetName()
{
    return mono_class_get_name(this);
}

void* MonoMethod::GetPointer()
{
    return mono_compile_method(this);
}

void* MonoMethod::GetThunk()
{
    return mono_method_get_unmanaged_thunk ? mono_method_get_unmanaged_thunk(this) : nullptr;
}

MonoObject* MonoMethod::Invoke(void* __this, void** args, MonoException** ex)
{
    return mono_runtime_invoke(this, __this, args, ex);
}

MonoReflectionType* MonoType::GetReflectionType()
{
    return mono_type_get_object(mono_get_root_domain(), this);
}

MonoClass* MonoType::GetClass()
{
    return mono_class_from_mono_type(this);
}

void MonoField::GetStaticValue(void* value)
{
    mono_field_static_get_value(GetParent()->GetVTable(), this, value);
}

void* MonoField::GetValuePtr(void* __this)
{
    if (IsStatic())
        return (char*)parent->GetStaticFieldData() + offset;
    return (char*)__this + offset;
}

MonoString* MonoString::New(const char* str)
{
    return mono_string_new(mono_get_root_domain(), str);
}

uint32_t MonoGCHandle::New(MonoObject* obj, bool pinned)
{
    return mono_gchandle_new(obj, pinned);
}

uint32_t MonoGCHandle::NewWeak(MonoObject* obj, bool trackResurrection)
{
    return mono_gchandle_new_weakref(obj, trackResurrection);
}

MonoObject* MonoGCHandle::GetTarget(uint32_t handle)
{
    return mono_gchandle_get_target(handle);
}

void MonoGCHandle::Free(uint32_t handle)
{
    mono_gchandle_free(handle);
}