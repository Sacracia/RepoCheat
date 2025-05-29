#include "haxsdk_backend.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <string.h>

#include <unordered_map>
#include <vector>

#include "../haxsdk_assertion.h"
#include "../haxsdk_tools.h"
#include "mono/haxsdk_mono.h"
#include "il2cpp/haxsdk_il2cpp.h"

#undef GetMessage

static HaxBackend GetBackend(OUT HMODULE& handle);

static HaxBackend g_Backend = HaxBackend_None;
static HMODULE g_BackendHandle;
static bool g_Initialized = false;

namespace vtables
{
    static unsafe::VTable* g_RuntimeType;
    static unsafe::VTable* g_Vector3;
    static unsafe::VTable* g_NullReferenceException;
    static unsafe::VTable* g_ArgumentOutOfRangeException;
    static unsafe::VTable* g_TargetException;
}

namespace HaxSdk
{
    void InitBackend()
    {
        if (!g_Initialized)
        {
            g_Initialized = true;

            g_Backend = GetBackend(g_BackendHandle);
            HAX_ASSERT_E(g_Backend, "Unable to determine backend");
            IsMono() ? mono::Initialize() : il2cpp::Initialize();

            unsafe::Thread::Attach();
            unsafe::Image* corlib = unsafe::Image::GetCorlib();
            vtables::g_RuntimeType = corlib->GetClass("System", "RuntimeType")->GetVTable();
            vtables::g_Vector3 = unsafe::Image::GetUnityCore()->GetClass("UnityEngine", "Vector3")->GetVTable();
            vtables::g_NullReferenceException = unsafe::Image::GetCorlib()->GetClass("System", "NullReferenceException")->GetVTable();
            vtables::g_ArgumentOutOfRangeException = unsafe::Image::GetCorlib()->GetClass("System", "ArgumentOutOfRangeException")->GetVTable();
            vtables::g_TargetException = unsafe::Image::GetCorlib()->GetClass("System.Reflection", "TargetException")->GetVTable();
        }
    }

    bool IsMono()
    {
        return g_Backend == HaxBackend_Mono;
    }

    bool IsIl2Cpp()
    {
        return g_Backend == HaxBackend_Il2Cpp;
    }

    void* GetBackendHandle()
    {
        return g_BackendHandle;
    }
}

static HaxBackend GetBackend(OUT HMODULE& handle)
{
    MODULEENTRY32W me = { 0 };
    me.dwSize = sizeof(MODULEENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (Module32FirstW(snapshot, &me))
    {
        do
        {
            if (wcsstr(me.szModule, L"mono") != nullptr)
            {
                handle = me.hModule;
                CloseHandle(snapshot);
                return HaxBackend_Mono;
            }
            if (wcscmp(me.szModule, L"GameAssembly.dll") == 0)
            {
                handle = me.hModule;
                CloseHandle(snapshot);
                return HaxBackend_Il2Cpp;
            }
        } while (Module32NextW(snapshot, &me));
    }

    CloseHandle(snapshot);
    return HaxBackend_None;
}

using namespace HaxSdk;
namespace unsafe
{
    Image* Assembly::GetImage()
    {
        return IsMono() ? (Image*)m_Mono.GetImage() : (Image*)m_Il2Cpp.GetImage();
    }

    Field* Class::GetField(const char* name, bool doAssert)
    {
        Field* field = IsMono() ? (Field*)m_Mono.GetField(name) : (Field*)m_Il2Cpp.GetField(name);
        if (doAssert)
            HAX_ASSERT_E(field, "Field {} not found", name);
        return field;
    }

    Method* Class::GetMethod(const char* name, const char* sig, bool doAssert)
    {
        Method* method = IsMono() ? (Method*)m_Mono.GetMethod(name, sig) : (Method*)m_Il2Cpp.GetMethod(name, sig);
        if (doAssert)
            HAX_ASSERT_E(method, "Method {} of {} not found", name, sig ? sig : "...");
        return method;
    }

    Type* Class::GetType()
    {
        return IsMono() ? (Type*)m_Mono.GetType() : (Type*)m_Il2Cpp.GetType();
    }

    VTable* Class::GetVTable()
    {
        return IsMono() ? (VTable*)m_Mono.GetVTable() : (VTable*)this;
    }

    const char* Class::GetName()
    {
        return IsMono() ? m_Mono.GetName() : m_Il2Cpp.GetName();
    }

    Domain* Domain::GetRoot()
    {
        return IsMono() ? (Domain*)MonoDomain::GetRoot() : (Domain*)Il2CppDomain::GetRoot();
    }

    Assembly* Domain::GetAssembly(const char* name, bool doAssert)
    {
        Assembly* assembly = IsMono() ? (Assembly*)m_Mono.GetAssembly(name) : (Assembly*)m_Il2Cpp.GetAssembly(name);
        if (doAssert)
            HAX_ASSERT_E(assembly, "Assembly {} not found", name);
        return assembly;
    }

    Image* Domain::GetImage(const char* name, bool doAssert)
    {
        Assembly* assembly = GetAssembly(name, doAssert);
        return assembly ? assembly->GetImage() : nullptr;
    }

    String*& Exception::GetMessage()
    { 
        return HaxSdk::IsMono() ? (String*&)ToMono()->GetMessage() : (String*&)ToIl2Cpp()->GetMessage(); 
    }

    String* Exception::GetStackTrace()
    {
        return HaxSdk::IsMono() ? (String*)ToMono()->GetStackTrace() : (String*)ToIl2Cpp()->GetStackTrace();
    }

    Exception* Exception::GetNullReference()
    {
        return (Exception*)Object::New(vtables::g_NullReferenceException->GetClass())->Ctor();
    }

    Exception* Exception::GetArgumentOutOfRange()
    {
        return (Exception*)Object::New(vtables::g_ArgumentOutOfRangeException->GetClass())->Ctor();
    }

    Exception* Exception::GetTargetException(String* message)
    {
        Exception* ex = (Exception*)Object::New(vtables::g_TargetException->GetClass())->Ctor();
        ex->GetMessage() = message;
        return ex;
    }

    bool Field::IsStatic() 
    { 
        return HaxSdk::IsMono() ? m_Mono.IsStatic() : m_Il2Cpp.IsStatic(); 
    }
    
    int Field::GetOffset() 
    { 
        return HaxSdk::IsMono() ? m_Mono.GetOffset() : m_Il2Cpp.GetOffset(); 
    }

    const char* Field::GetName()
    { 
        return HaxSdk::IsMono() ? m_Mono.GetName() : m_Il2Cpp.GetName(); 
    }
    void Field::GetStaticValue(void* value)
    { 
        return HaxSdk::IsMono() ? m_Mono.GetStaticValue(value) : m_Il2Cpp.GetStaticValue(value); 
    }

    bool Field::IsLiteral()
    { 
        return HaxSdk::IsMono() ? m_Mono.IsLiteral() : m_Il2Cpp.IsLiteral(); 
    }

    void* Field::GetValuePtr(void* __this)
    {
        HAX_ASSERT_E(!IsLiteral(), "Use GetStaticValue for literals");
        HAX_ASSERT_E(__this || IsStatic(), "THIS not provided for non-static field {}", GetName());
        return IsMono() ? m_Mono.GetValuePtr(__this) : m_Il2Cpp.GetValuePtr(__this);
    }

    uint32_t GCHandle::New(Object* obj, bool pinned)
    {
        return HaxSdk::IsMono() ? MonoGCHandle::New((MonoObject*)obj, pinned) : Il2CppGCHandle::New((Il2CppObject*)obj, pinned);
    }

    uint32_t GCHandle::NewWeak(Object* obj, bool trackResurrection)
    {
        return HaxSdk::IsMono() ? MonoGCHandle::NewWeak((MonoObject*)obj, trackResurrection) : Il2CppGCHandle::NewWeak((Il2CppObject*)obj, trackResurrection);
    }

    Object* GCHandle::GetTarget(uint32_t handle)
    {
        return HaxSdk::IsMono() ? (Object*)MonoGCHandle::GetTarget(handle) : (Object*)Il2CppGCHandle::GetTarget(handle);
    }

    void GCHandle::Free(uint32_t handle)
    {
        if (handle > 0)
            HaxSdk::IsMono() ? MonoGCHandle::Free(handle) : Il2CppGCHandle::Free(handle);
    }

    Image* Image::GetCorlib()
    {
        return IsMono() ? (Image*)MonoImage::GetCorlib() : (Image*)Il2CppImage::GetCorlib();
    }

    Image* Image::GetUnityCore()
    {
        static Image* image = IsMono() ? (Image*)MonoImage::GetUnityCore() : (Image*)Il2CppImage::GetUnityCore();
        HAX_ASSERT_E(image, "Unable to find Unity core module");
        return image;
    }

    Class* Image::GetClass(const char* nameSpace, const char* name, bool doAssert)
    {
        Class* klass = IsMono() ? (Class*)m_Mono.GetClass(nameSpace, name) : (Class*)m_Il2Cpp.GetClass(nameSpace, name);
        if (doAssert)
            HAX_ASSERT_E(klass, "Class {}.{} not found", nameSpace, name);
        return klass;
    }

    void* Method::GetPointer()
    {
        return IsMono() ? m_Mono.GetPointer() : m_Il2Cpp.GetPointer();
    }

    Object* Method::Invoke(void* __this, void** args, Exception** ex)
    {
        return HaxSdk::IsMono() ? (Object*)m_Mono.Invoke(__this, args, (MonoException**)ex) : (Object*)m_Il2Cpp.Invoke(__this, args, (Il2CppException**)ex);
    }

    Object* Object::New(Class* klass)
    {
        return HaxSdk::IsMono() ? (Object*)(MonoObject::New((MonoClass*)klass)) : (Object*)(Il2CppObject::New((Il2CppClass*)klass));
    }

    Object* Object::Ctor()
    {
        HaxSdk::IsMono() ? ((MonoObject*)this)->Ctor() : ((Il2CppObject*)this)->Ctor();
        return this;
    }

    Object* Object::Box(Class* klass, void* data)
    {
        return IsMono() ? (Object*)MonoObject::Box(&klass->m_Mono, data) : (Object*)Il2CppObject::Box(&klass->m_Il2Cpp, data);
    }

    Class* Object::GetClass()
    {
        return IsMono() ? (Class*)((MonoObject*)this)->GetClass() : (Class*)((Il2CppObject*)this)->GetClass();
    }

    void* Object::Unbox()
    {
        return (char*)this + sizeof(Object);
    }

    ReflectionType::ReflectionType(Type* type) : Object(vtables::g_RuntimeType), m_Type(type)
    {

    }

    String* String::New(const char* str)
    {
        return HaxSdk::IsMono() ? (String*)MonoString::New(str) : (String*)Il2CppString::New(str);
    }

    Thread* Thread::Attach()
    {
        return IsMono() ? (Thread*)MonoThread::Attach() : (Thread*)Il2CppThread::Attach();
    }

    void Thread::Detach()
    {
        IsMono() ? m_Mono.Detach() : m_Il2Cpp.Detach();
    }

    ReflectionType* Type::GetReflectionType()
    {
        static std::unordered_map<Type*, ReflectionType> cache;
        if (!cache.contains(this))
            cache[this] = ReflectionType(this);
        return &cache[this];
    }

    ReflectionType* Type::CreateReflectionType()
    {
        return HaxSdk::IsMono() ? (ReflectionType*)m_Mono.GetReflectionType() : (ReflectionType*)m_Il2Cpp.GetReflectionType();
    }

    Class* Type::GetClass()
    {
        return HaxSdk::IsMono() ? (Class*)m_Mono.GetClass() : (Class*)m_Il2Cpp.GetClass();
    }

    Class* VTable::GetClass()
    {
        return HaxSdk::IsMono() ? (Class*)m_Mono.klass : (Class*)this;
    }
}