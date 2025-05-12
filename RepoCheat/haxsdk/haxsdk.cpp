#include "haxsdk.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <iostream>
#include <format>
#include <filesystem>
#include <fstream>
#include <vector>

#ifdef _WIN64
#include "third_party/detours/x64/detours.h"
#else
#include "third_party/detours/x86/detours.h"
#endif

#define BACKEND_API_DECL(_RET, _FUNC, _PARAMS) using t_ ## _FUNC = _RET (*)_PARAMS; static t_ ## _FUNC _FUNC = nullptr
#define BACKEND_API_DEF(_FUNC, _MODULE) _FUNC = (t_ ## _FUNC)GetProcAddress(_MODULE, #_FUNC); HAX_ASSERT(_FUNC, #_FUNC)
#define BACKEND_API_DEF_OPT(_FUNC, _MODULE) _FUNC = (t_ ## _FUNC)GetProcAddress(_MODULE, #_FUNC)

using t_mono_get_root_domain                = Domain* (*)();
using t_mono_domain_assembly_open           = Assembly* (*)(Domain*, const char*);
using t_mono_thread_attach                  = Thread* (*)(Domain*);
using t_mono_domain_get                     = Domain* (*)();
using t_mono_string_new                     = System::String* (*)(Domain*, const char*);
using t_mono_assembly_get_image             = Image* (*)(Assembly*);
typedef void(__cdecl* GFunc)          (void* data, void* user_data);
using t_mono_assembly_foreach = void (*)(GFunc, void*);
using t_mono_class_from_name                = Class* (*)(Image*, const char*, const char*);
using t_mono_class_get_methods              = Method* (*)(Class*, void**);
using t_mono_class_get_field_from_name      = Field* (*)(Class*, const char*);
using t_mono_class_vtable                   = void* (*)(Domain*, Class*);
using t_mono_class_get_namespace            = const char* (*)(Class*);
using t_mono_class_get_name                 = const char* (*)(Class*);
using t_mono_object_get_class               = Class* (*)(System::Object*);
using t_mono_class_get_type                 = Type* (*)(Class*);
using t_mono_object_get_class               = Class* (*)(System::Object*);
using t_mono_method_signature               = void* (*)(void*);
using t_mono_method_get_name                = const char* (*)(void*);
using t_mono_compile_method                 = void* (*)(void*);
using t_mono_jit_info_table_find            = void* (*)(Domain*, void*);
using t_mono_jit_info_get_method            = Method* (*)(void*);
using t_mono_runtime_invoke                 = System::Object* (*)(void*, void*, void**, System::Object**);
using t_mono_print_unhandled_exception      = void (*)(System::Object*);
using t_mono_object_unbox                   = void* (*)(System::Object*);
using t_mono_vtable_get_static_field_data   = void* (*)(void*);
using t_mono_type_get_object                = System::Type* (*)(Domain*, Type*);
using t_mono_object_new                     = System::Object* (*)(Domain*, Class*);
using t_mono_runtime_object_init            = void (*)(System::Object*);
using t_mono_signature_get_return_type      = Type* (*)(void*);
using t_mono_type_get_name                  = char* (*)(Type*);
using t_mono_signature_get_param_count      = uint32_t (*)(void*);
using t_mono_signature_get_params           = Type* (*)(void*, void**);
using t_mono_method_get_unmanaged_thunk     = void* (*)(void*);
using t_mono_string_to_utf8                 = char* (*)(System::String*);
using t_mono_field_static_get_value         = void (*)(void* pVtable, Field* pField, void* pValue);
using t_mono_field_static_set_value         = void (*)(void* pVtable, Field* pField, void* pValue);
using t_mono_thread_detach                  = void (*)(Thread*);
using t_mono_image_get_name                 = const char* (*)(Image*);
using t_mono_lookup_internal_call           = void* (*)(void*);
using t_mono_object_to_string               = System::String* (*)(System::Object*);
using t_mono_type_is_void                   = bool (*)(Type* type);

using t_il2cpp_object_new                   = System::Object* (*)(Class* pClass);
using t_il2cpp_object_unbox                 = void* (*)(System::Object* obj);
using t_il2cpp_runtime_object_init          = void (*)(System::Object* obj);
using t_il2cpp_domain_get                   = Domain* (*)();
using t_il2cpp_domain_get_assemblies        = Assembly* *(*)(Domain* domain, size_t* size);
using t_il2cpp_thread_attach                = Thread* (*)(Domain* domain);
using t_il2cpp_assembly_get_image           = Image* (*)(Assembly* assembly);
using t_il2cpp_class_from_name              = Class* (*)(Image* image, const char* namespaze, const char* name);
using t_il2cpp_class_get_type               = Type* (*)(Class* pClass);
using t_il2cpp_class_get_methods            = Method* (*)(Class* pClass, void** ppIter);
using t_il2cpp_type_get_object              = System::Type* (*)(Type* type);
using t_il2cpp_type_get_name                = const char* (*)(Type* type);
using t_il2cpp_class_get_field_from_name    = Field* (*)(Class* pClass, const char* name);
using t_il2cpp_runtime_invoke               = System::Object* (*)(Method* pMethod, void* obj, void** params, void** exc);
using t_il2cpp_method_get_param_count       = int32_t(*)(Method* pMethod);
using t_il2cpp_method_get_param             = Type* (*)(Method* pMethod, uint32_t index);
using t_il2cpp_method_get_class             = Class* (*)(Method* pMethod);
using t_il2cpp_method_get_name              = const char* (*)(Method* pMethod);
using t_il2cpp_method_get_return_type       = Type* (*)(Method* pMethod);
using t_il2cpp_string_new                   = System::String* (*)(const char* str);
using t_il2cpp_class_get_static_field_data  = void* (*)(Class*);
using t_il2cpp_field_static_get_value       = void (*)(Field* pField, void* pValue);
using t_il2cpp_field_static_set_value       = void (*)(Field* pField, void* pValue);
using t_il2cpp_thread_detach                = void (*)(Thread*);

static t_mono_get_root_domain               mono_get_root_domain;
static t_mono_domain_assembly_open          mono_domain_assembly_open;
static t_mono_thread_attach                 mono_thread_attach;
static t_mono_domain_get                    mono_domain_get;
static t_mono_string_new                    mono_string_new;
static t_mono_assembly_get_image            mono_assembly_get_image;
static t_mono_assembly_foreach              mono_assembly_foreach;
static t_mono_class_from_name               mono_class_from_name;
static t_mono_class_get_methods             mono_class_get_methods;
static t_mono_class_get_field_from_name     mono_class_get_field_from_name;
static t_mono_class_get_namespace           mono_class_get_namespace;
static t_mono_class_get_name                mono_class_get_name;
static t_mono_class_vtable                  mono_class_vtable;
static t_mono_object_get_class              mono_object_get_class;
static t_mono_class_get_type                mono_class_get_type;
static t_mono_method_signature              mono_method_signature;
static t_mono_method_get_name               mono_method_get_name;
static t_mono_compile_method                mono_compile_method;
static t_mono_jit_info_table_find           mono_jit_info_table_find;
static t_mono_jit_info_get_method           mono_jit_info_get_method;
static t_mono_runtime_invoke                mono_runtime_invoke;
static t_mono_object_unbox                  mono_object_unbox;
static t_mono_vtable_get_static_field_data  mono_vtable_get_static_field_data;
static t_mono_type_get_object               mono_type_get_object;
static t_mono_object_new                    mono_object_new;
static t_mono_runtime_object_init           mono_runtime_object_init;
static t_mono_signature_get_return_type     mono_signature_get_return_type;
static t_mono_type_get_name                 mono_type_get_name;
static t_mono_signature_get_param_count     mono_signature_get_param_count;
static t_mono_signature_get_params          mono_signature_get_params;
static t_mono_method_get_unmanaged_thunk    mono_method_get_unmanaged_thunk;
static t_mono_string_to_utf8                mono_string_to_utf8;
static t_mono_field_static_get_value        mono_field_static_get_value;
static t_mono_field_static_set_value        mono_field_static_set_value;
static t_mono_thread_detach                 mono_thread_detach;
static t_mono_image_get_name                mono_image_get_name;
static t_mono_lookup_internal_call          mono_lookup_internal_call;
static t_mono_object_to_string              mono_object_to_string;
static t_mono_type_is_void                  mono_type_is_void;

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

struct HaxLogger {
    void Log(std::string_view message);
    void Init(bool useConsole);

    std::filesystem::path filePath;
    bool initialized = false;
    bool useConsole = false;
};

static HaxGlobals g_globals;
static HaxLogger g_logger;
static std::vector<std::pair<void**, void*>> g_hooks;

namespace System {
    static const char* MODULE = "mscorlib";
    static const char* NAMESPACE = "System";
}

static void DetermineBackend();
static void DefineBackendApi();
static void GetIl2CppSignature(Method* pMethod, char* buff);
static void GetMonoSignature(Method* pMethod, char* buff);

namespace HaxSdk {
    HaxGlobals& GetGlobals() {
        return g_globals;
    }

    void DetourAttach(void** ppPointer, void* pDetour) {
        if (::DetourAttach(ppPointer, pDetour) == 0) {
            g_hooks.push_back(std::make_pair(ppPointer, pDetour));
        }
    }

    void DetachAllHooks() {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        for (const auto& hook : g_hooks) {
            DetourDetach(hook.first, hook.second);
        }
        DetourTransactionCommit();
    }

    void InitLogger(bool useConsole) {
        g_logger.Init(useConsole);
    }

    void Log(std::string_view text) {
        g_logger.Log(text);
    }

    void InitializeCore() {
        DetermineBackend();
        HAX_ASSERT(g_globals.backend != HaxBackend_None, "Unable to determine unity backend");
        DefineBackendApi();
        UnityAttachThread();
    }

    Thread* UnityAttachThread() {
        return Domain::Root()->AttachThread();
    }
}

void HaxGlobals::Save() {
    std::ofstream file("haxsdk.ini", std::ios::out);
    file << std::format("Key={}\nLanguage={}\nSpawnColor={},{},{},{}\nExtrColor={},{},{},{}", 
        this->hotkey, 
        this->locale,
        this->spawnColor[0], this->spawnColor[1], this->spawnColor[2], this->spawnColor[3],
        this->extractionColor[0], this->extractionColor[1], this->extractionColor[2], this->extractionColor[3]
    ).c_str();
    file.close();
}

void HaxGlobals::Load() {
    const std::filesystem::path ini{"haxsdk.ini"};
    if (std::filesystem::exists(ini)) {
        std::ifstream file("haxsdk.ini");
        std::string line;
        if (file.is_open()) {
            int x;
            float r, g, b, a;
            while (getline(file, line)) {
                if (sscanf_s(line.c_str(), "Language=%d", &x) == 1) { this->locale = x; }
                if (sscanf_s(line.c_str(), "Key=%d", &x) == 1) { this->hotkey = x; }
                if (sscanf_s(line.c_str(), "SpawnColor=%f,%f,%f,%f", &r, &g, &b, &a) == 4) { 
                    this->spawnColor[0] = r; 
                    this->spawnColor[1] = g;
                    this->spawnColor[2] = b;
                    this->spawnColor[3] = a;
                }
                if (sscanf_s(line.c_str(), "ExtrColor=%f,%f,%f,%f", &r, &g, &b, &a) == 4) {
                    this->extractionColor[0] = r;
                    this->extractionColor[1] = g;
                    this->extractionColor[2] = b;
                    this->extractionColor[3] = a;
                }
            }
            HaxSdk::Log(std::format("GLOBALS LOADED: Lang={} | Key={}\n", this->locale, this->hotkey));
            file.close();
        }
    }
}

void HaxLogger::Log(std::string_view message) {
    if (!initialized)
        return;

    auto t = std::time(nullptr);
    struct tm newtime;
    ::localtime_s(&newtime, &t);
    std::stringstream ss{};
    ss << std::put_time(&newtime, "%d-%m-%Y %H:%M:%S") << " [" << std::left << std::setw(7) << "DEBUG" << "] " << message;

    std::ofstream file(filePath, std::ios_base::app);
    file << ss.str();
    file.close();

    if (useConsole)
        std::cout << ss.str();
}

void HaxLogger::Init(bool useConsole) {
    if (initialized)
        return;

    this->useConsole = useConsole;
    if (useConsole) {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    }

    char buff[MAX_PATH];
    GetModuleFileName(NULL, buff, MAX_PATH);
    const auto path = std::filesystem::path(buff);

    const auto logPath = path.parent_path() / "haxsdk-logs.txt";
    const auto prevLogPath = path.parent_path() / "haxsdk-prev-logs.txt";

    std::error_code errCode;
    std::filesystem::remove(prevLogPath, errCode);
    std::filesystem::rename(logPath, prevLogPath, errCode);
    std::filesystem::remove(logPath, errCode);

    filePath = logPath;
    initialized = true;
}

static void DetermineBackend() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    MODULEENTRY32W moduleEntry = {0};
    moduleEntry.dwSize = sizeof(moduleEntry);
    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (wcsncmp(moduleEntry.szModule, L"mono", 4) == 0) {
                CloseHandle(hSnapshot);
                g_globals.backend = HaxBackend_Mono;
                g_globals.backendHandle = (void*)moduleEntry.hModule;
                return;
            }
            if (wcscmp(moduleEntry.szModule, L"GameAssembly.dll") == 0) {
                CloseHandle(hSnapshot);
                g_globals.backend = HaxBackend_IL2CPP;
                g_globals.backendHandle = (void*)moduleEntry.hModule;
                return;
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }
    CloseHandle(hSnapshot);
    g_globals.backend = HaxBackend_None;
}

static void DefineBackendApi() {
    HMODULE hModule = (HMODULE)g_globals.backendHandle;
    if (g_globals.backend & HaxBackend_IL2CPP) {
        BACKEND_API_DEF(il2cpp_object_new, hModule);
        BACKEND_API_DEF(il2cpp_object_unbox, hModule);
        BACKEND_API_DEF(il2cpp_runtime_object_init, hModule);
        BACKEND_API_DEF(il2cpp_domain_get, hModule);
        BACKEND_API_DEF(il2cpp_domain_get_assemblies, hModule);
        BACKEND_API_DEF(il2cpp_thread_attach, hModule);
        BACKEND_API_DEF(il2cpp_assembly_get_image, hModule);
        BACKEND_API_DEF(il2cpp_class_from_name, hModule);
        BACKEND_API_DEF(il2cpp_class_get_type, hModule);
        BACKEND_API_DEF(il2cpp_class_get_methods, hModule);
        BACKEND_API_DEF(il2cpp_type_get_object, hModule);
        BACKEND_API_DEF(il2cpp_type_get_name, hModule);
        BACKEND_API_DEF(il2cpp_class_get_field_from_name, hModule);
        BACKEND_API_DEF(il2cpp_runtime_invoke, hModule);
        BACKEND_API_DEF(il2cpp_method_get_param_count, hModule);
        BACKEND_API_DEF(il2cpp_method_get_param, hModule);
        BACKEND_API_DEF(il2cpp_method_get_class, hModule);
        BACKEND_API_DEF(il2cpp_method_get_name, hModule);
        BACKEND_API_DEF(il2cpp_method_get_return_type, hModule);
        BACKEND_API_DEF(il2cpp_string_new, hModule);
        BACKEND_API_DEF(il2cpp_field_static_get_value, hModule);
        BACKEND_API_DEF(il2cpp_field_static_set_value, hModule);
        BACKEND_API_DEF_OPT(il2cpp_class_get_static_field_data, hModule);
        BACKEND_API_DEF(il2cpp_thread_detach, hModule);
    }
    else {
        BACKEND_API_DEF(mono_get_root_domain, hModule);
        BACKEND_API_DEF(mono_domain_assembly_open, hModule);
        BACKEND_API_DEF(mono_thread_attach, hModule);
        BACKEND_API_DEF(mono_domain_get, hModule);
        BACKEND_API_DEF(mono_string_new, hModule);
        BACKEND_API_DEF(mono_assembly_get_image, hModule);
        BACKEND_API_DEF(mono_assembly_foreach, hModule);
        BACKEND_API_DEF(mono_class_from_name, hModule);
        BACKEND_API_DEF(mono_class_get_methods, hModule);
        BACKEND_API_DEF(mono_class_get_field_from_name, hModule);
        BACKEND_API_DEF(mono_class_vtable, hModule);
        BACKEND_API_DEF(mono_class_get_name, hModule);
        BACKEND_API_DEF(mono_class_get_namespace, hModule);
        BACKEND_API_DEF(mono_object_get_class, hModule);
        BACKEND_API_DEF(mono_class_get_type, hModule);
        BACKEND_API_DEF(mono_method_signature, hModule);
        BACKEND_API_DEF(mono_method_get_name, hModule);
        BACKEND_API_DEF(mono_compile_method, hModule);
        BACKEND_API_DEF(mono_jit_info_table_find, hModule);
        BACKEND_API_DEF(mono_jit_info_get_method, hModule);
        BACKEND_API_DEF(mono_runtime_invoke, hModule);
        BACKEND_API_DEF(mono_object_unbox, hModule);
        BACKEND_API_DEF(mono_vtable_get_static_field_data, hModule);
        BACKEND_API_DEF(mono_type_get_object, hModule);
        BACKEND_API_DEF(mono_object_new, hModule);
        BACKEND_API_DEF(mono_runtime_object_init, hModule);
        BACKEND_API_DEF(mono_signature_get_return_type, hModule);
        BACKEND_API_DEF(mono_type_get_name, hModule);
        BACKEND_API_DEF(mono_signature_get_param_count, hModule);
        BACKEND_API_DEF(mono_signature_get_params, hModule);
        BACKEND_API_DEF(mono_string_to_utf8, hModule);
        BACKEND_API_DEF(mono_field_static_get_value, hModule);
        BACKEND_API_DEF(mono_field_static_set_value, hModule);
        BACKEND_API_DEF(mono_thread_detach, hModule);
        BACKEND_API_DEF(mono_image_get_name, hModule);
        BACKEND_API_DEF(mono_lookup_internal_call, hModule);
        BACKEND_API_DEF(mono_object_to_string, hModule);
        BACKEND_API_DEF(mono_type_is_void, hModule);
        BACKEND_API_DEF(mono_object_to_string, hModule);
        BACKEND_API_DEF_OPT(mono_method_get_unmanaged_thunk, hModule);
    }
}

struct Il2CppAssembly {
    Image*      pImage;
    uint32_t    token;
    int32_t     referencedAssemblyStart;
    int32_t     referencedAssemblyCount;
    const char* name;
};

struct MonoAssembly {
};

struct Il2CppType {
    void* m_data;
    unsigned int            m_attrs : 16;
    int                     m_type : 8;
    unsigned int            m_num_mods : 5;
    unsigned int            m_byref : 1;
    unsigned int            m_pinned : 1;
    unsigned int            m_valuetype : 1;
};

struct Il2CppField {
    const char*             name;
    Type*                   type;
    Class*                  parent;
    int32_t                 offset;
    uint32_t                token;
};

struct MonoField {
    Type*                   type;
    const char*             name;
    Class*                  parent;
    int32_t                 offset;
};

struct Il2CppClass {
    Image*                  image;
    void*                   gcDesc;
    const char*             name;
    const char*             nameSpace;
    Type                    byvalArg;
    Type                    thisArg;
    void*                   __space[15];
    void*                   staticFields;
};

struct Il2CppMethod {
    void*                   ptr;
    // <...>
};

Assembly* Assembly::Find(const char* name) {
    Assembly* pAssembly;
    if (Exists(name, pAssembly))
        return pAssembly;

    HaxSdk::Log(std::format("Assembly {} not found\n", name));
    HAX_ASSERT(false, "Assembly not found.\nSee logs for more information");
    return pAssembly;
}

void _cdecl AssemblyEnumerator(void* assembly, void* params) {
    auto params2 = (std::pair<const char*, Assembly**>*)params;
    const char* assemblyName = mono_image_get_name(((Assembly*)(assembly))->GetImage());
    if (strcmp(assemblyName, params2->first) == 0) {
        *params2->second = (Assembly*)assembly;
    }
}

bool Assembly::Exists(const char* name, OUT Assembly*& pRes) {
    if (g_globals.backend & HaxBackend_Mono) {
        auto params = std::make_pair(name, &pRes);
        mono_assembly_foreach(AssemblyEnumerator, &params);
        return pRes != nullptr;
    }

    size_t nDomains = 0;
    Assembly** ppAssembly = il2cpp_domain_get_assemblies(Domain::Root(), &nDomains);
    for (size_t i = 0; i < nDomains; ++i) {
        if (strcmp(((Il2CppAssembly*)ppAssembly[i])->name, name) == 0) {
            pRes = ppAssembly[i];
            return true;
        }
    }

    return false;
}

Image* Assembly::GetImage() {
    return g_globals.backend & HaxBackend_IL2CPP ? ((Il2CppAssembly*)this)->pImage : mono_assembly_get_image(this);
}

bool Class::Exists(const char* assembly, const char* nameSpace, const char* name, OUT Class*& pClass) {
    Assembly* pAssembly;
    if (!Assembly::Exists(assembly, pAssembly)) {
        pClass = nullptr;
        printf("ASSEMBLY NOT FOUND\n");
        return false;
    }

    pClass = g_globals.backend & HaxBackend_IL2CPP ? il2cpp_class_from_name(pAssembly->GetImage(), nameSpace, name) : 
        mono_class_from_name(pAssembly->GetImage(), nameSpace, name);
    return pClass != nullptr;
}

Class* Class::Find(const char* assembly, const char* nameSpace, const char* name) {
    Class* pClass;
    if (Exists(assembly, nameSpace, name, pClass))
        return pClass;
    
    HaxSdk::Log(std::format("Class {}.{} not found in {}\n", nameSpace, name, assembly));
    HAX_ASSERT(false, "Class not found.\nSee logs for more information");
    return pClass;
}

Method* Class::FindMethod(const char* name, const char* signature) {
    bool isMono = HaxSdk::GetGlobals().backend & HaxBackend_Mono;
    void* iter = nullptr;

    while (Method* pMethod = isMono ? mono_class_get_methods(this, &iter) : il2cpp_class_get_methods(this, &iter)) {
        const char* methodName = isMono ? mono_method_get_name(pMethod) : il2cpp_method_get_name(pMethod);
        if (strcmp(methodName, name) == 0) {
            if (!signature || signature[0] == '\0')
                return pMethod;

            char buff[256] = {0};
            isMono ? GetMonoSignature(pMethod, buff) : GetIl2CppSignature(pMethod, buff);
            if (strcmp(signature, buff) == 0)
                return pMethod;
        }
    }

    HaxSdk::Log(std::format("Method {} of {} not found in {}\n", name, signature ? signature : "?", this->GetName()));
    HAX_ASSERT(false, "Method not found.\nSee logs for more information");
    return nullptr;
}

Field* Class::FindField(const char* name) {
    Field* pField = g_globals.backend & HaxBackend_Mono ? mono_class_get_field_from_name(this, name) : il2cpp_class_get_field_from_name(this, name);
    if (!pField) {
        HaxSdk::Log(std::format("Field {} not found in {}\n", name, this->GetName()));
        HAX_ASSERT(false, "Field not found.\nSee logs for more information");
    }
    return pField;
}

void* Class::FindStaticField(const char* name) {
    int32_t offset = FindField(name)->Offset();
    void* ptr = (void*)((char*)GetStaticFieldsData() + offset);
    return ptr;
}

void* Class::GetStaticFieldsData() {
    if (g_globals.backend & HaxBackend_IL2CPP)
        return il2cpp_class_get_static_field_data ? il2cpp_class_get_static_field_data(this) : ((Il2CppClass*)this)->staticFields;
    return mono_vtable_get_static_field_data(mono_class_vtable(Domain::Root(), this));
}

int32_t Field::Offset() {
    return g_globals.backend & HaxBackend_IL2CPP ? ((Il2CppField*)this)->offset : ((MonoField*)this)->offset;
}

void Field::GetStaticValue(void* pValue) {
    if (g_globals.backend & HaxBackend_Mono) {
        void* pVtable = mono_class_vtable(Domain::Root(), ((MonoField*)this)->parent);
        mono_field_static_get_value(pVtable, this, pValue);
        return;
    }
    il2cpp_field_static_get_value(this, pValue);
}

void Field::SetStaticValue(void* pValue) {
    if (g_globals.backend & HaxBackend_Mono) {
        void* pVtable = mono_class_vtable(Domain::Root(), ((MonoField*)this)->parent);
        mono_field_static_set_value(pVtable, this, pValue);
        return;
    }
    il2cpp_field_static_set_value(this, pValue);
}

const char* Class::GetName() {
    return g_globals.backend & HaxBackend_Mono ? mono_class_get_name(this) : ((Il2CppClass*)this)->name;
}

System::Type* Class::GetSystemType() {
    return g_globals.backend & HaxBackend_Mono ? mono_class_get_type(this)->GetSystemType() : il2cpp_class_get_type(this)->GetSystemType();
}

Domain* Domain::Root() {
    return g_globals.backend & HaxBackend_Mono ? mono_get_root_domain() : il2cpp_domain_get();
}

Domain* Domain::Current() {
    return g_globals.backend & HaxBackend_Mono ? mono_domain_get() : il2cpp_domain_get();
}

Thread* Domain::AttachThread() {
    Domain* pDomain = Domain::Root();
    return g_globals.backend & HaxBackend_Mono ? mono_thread_attach(pDomain) : il2cpp_thread_attach(pDomain);
}

namespace System {
    Class* Enum::GetClass() {
        static Class* pClass = Class::Find(System::MODULE, System::NAMESPACE, "Enum");
        return pClass;
    }

    Array<String*>* Enum::GetNames(Type* pType) {
        static HaxMethod<Array<String*>*(*)(Type*)> method(Enum::GetClass()->FindMethod("GetNames"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Array<String*>*)method.Invoke(nullptr, args);
        }

        return method.ptr(pType);
    }

    Array<Int32>* Enum::GetValues(Type* pType) {
        static HaxMethod<Array<Int32>*(*)(Type*)> method(Enum::GetClass()->FindMethod("GetValues"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Array<Int32>*)method.Invoke(nullptr, args);
        }

        return method.ptr(pType);
    }

    String* Enum::GetName(System::Type* pType, System::Enum* pObj) {
        static HaxMethod<Int32(*)(Type*)> method(Enum::GetClass()->FindMethod("GetName", "System.String(System.Type,System.Object)"));
        void* args[] = { pType, pObj };
        return (String*)method.Invoke(nullptr, args);
    }
}

System::Type* Type::GetSystemType() {
    return g_globals.backend & HaxBackend_Mono ? mono_type_get_object(Domain::Root(), this) : il2cpp_type_get_object(this);
}

void Thread::Detach() {
    g_globals.backend& HaxBackend_Mono ? mono_thread_detach(this) : il2cpp_thread_detach(this);
}

System::Object* System::Object::New(Class* pClass) {
    return g_globals.backend & HaxBackend_Mono ? mono_object_new(Domain::Root(), pClass) : il2cpp_object_new(pClass);
}

System::Object* System::Object::Ctor() {
    g_globals.backend& HaxBackend_Mono ? mono_runtime_object_init(this) : il2cpp_runtime_object_init(this);
    return this;
}

void* System::Object::Unbox() {
    return g_globals.backend & HaxBackend_Mono ? mono_object_unbox(this) : il2cpp_object_unbox(this);
}

Int32 System::Object::GetHashCode() {
    static HaxMethod<Int32(*)(Object*)> method(Object::GetClass()->FindMethod("GetHashCode"));
    return *(Int32*)method.Invoke(this, nullptr)->Unbox();
}

System::String* System::Object::ToString() {
    static HaxMethod<System::String*(*)(Object*)> method(Object::GetClass()->FindMethod("ToString"));
    return (System::String*)method.Invoke(this, nullptr);
}

System::Object* Method::Invoke(void* __this, void** ppArgs) {
    if (g_globals.backend & HaxBackend_Mono) {
        bool isVoid = mono_type_is_void(mono_signature_get_return_type(mono_method_signature(this)));
        System::Object* pException = nullptr;
        auto pRes = mono_runtime_invoke(this, __this, ppArgs, &pException);
        if (pException) {
            System::String* s = mono_object_to_string(pException);
            HaxSdk::Log(std::format("ERROR: {}\n", s->UTF8()));
            if (!isVoid)
                throw std::runtime_error("MonoException");
        }
        return pRes;
    }
    return il2cpp_runtime_invoke(this, __this, ppArgs, nullptr);
}

void* Method::GetAddress() {
    if (g_globals.backend & HaxBackend_Mono) {
        return mono_compile_method(this);
    }
    return ((Il2CppMethod*)this)->ptr;
}

Class* System::String::GetClass() {
    static Class* pClass = Class::Find(System::MODULE, System::NAMESPACE, "String");
    return pClass;
}

System::String* System::String::New(const char* data) {
    return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? mono_string_new(Domain::Root(), data) : il2cpp_string_new(data);
}

System::String* System::String::Concat(System::String* s1, System::String* s2) {
    static HaxMethod<String*(*)(String*, String*)> method(String::GetClass()->FindMethod("Concat", "System.String(System.String,System.String)"));
    void* args[] = { s1, s2 };
    return (System::String*)method.Invoke(nullptr, args);
}

Int32 System::String::CompareTo(System::String* value) {
    static HaxMethod<String*(*)(String*, String*)> method(String::GetClass()->FindMethod("CompareTo", "System.Int32(System.String)"));
    void* args[] = { value };
    return *(Int32*)method.Invoke(this, args)->Unbox();
}

char* System::String::UTF8() {
    static char buff[256] = {0};

    //if (HaxSdk::GetGlobals().backend & HaxBackend_IL2CPP) {
    if (!this->chars)
        return nullptr;
    memset(buff, 0, sizeof(buff));
    int nBytesNeeded = WideCharToMultiByte(CP_UTF8, 0, this->chars, this->length, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, this->chars, this->length, buff, nBytesNeeded, NULL, NULL);
    return buff;
    //}

    //return mono_string_to_utf8(this);
}

template <typename T>
Class* System::Array<T>::GetClass() {
    static Class* pClass = Class::Find(System::MODULE, System::NAMESPACE, "Array");
    return pClass;
}

template <typename T>
void System::Array<T>::Clear(System::Array<T>* pArray, Int32 index, Int32 length) {
    HaxMethod<void(*)(System::Array<T>*,Int32,Int32)> method(System::Array<T>::GetClass()->FindMethod("Clear", "System.Void(System.Array,System.Int32,System.Int32"));
    if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
        void* args[] = { this, &index, &length };
        method.Invoke(nullptr, args);
        return;
    }

    method.ptr(this, index, length);
}

template <typename T>
void System::Array<T>::Sort(System::Array<T>* pArray, Int32 index, Int32 length) {
    HaxMethod<void(*)(System::Array<T>*,Int32,Int32)> method(System::Array<T>::GetClass()->FindMethod("Sort", "System.Void(System.Array,System.Int32,System.Int32)"));
    if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
        void* args[] = { this, &index, &length };
        method.Invoke(nullptr, args);
        return;
    }

    method.ptr(this, index, length);
}

void GetMonoSignature(Method* pMethod, char* buff) {
    void* pSignature = mono_method_signature(pMethod);
    Type* pReturnType = mono_signature_get_return_type(pSignature);
    strcpy_s(buff, 255, mono_type_get_name(pReturnType));
    strcat_s(buff, 255, "(");
    uint32_t nParams = mono_signature_get_param_count(pSignature);
    if (nParams > 0) {
        Type* pType;
        void* iter = 0;
        strcat_s(buff, 255, mono_type_get_name(mono_signature_get_params(pSignature, &iter)));
        while (pType = mono_signature_get_params(pSignature, &iter)) {
            strcat_s(buff, 255, ",");
            strcat_s(buff, 255, mono_type_get_name(pType));
        }
    }
    strcat_s(buff, 255, ")");
}

void GetIl2CppSignature(Method* pMethod, char* buff) {
    strcpy_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_return_type((Method*)pMethod)));
    strcat_s(buff, 255, "(");
    uint32_t nParams = il2cpp_method_get_param_count((Method*)pMethod);
    if (nParams > 0) {
        strcat_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_param((Method*)pMethod, 0)));
        for (uint32_t i = 1; i < nParams; ++i) {
            strcat_s(buff, 255, ",");
            strcat_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_param((Method*)pMethod, i)));
        }
    }
    strcat_s(buff, 255, ")");
}