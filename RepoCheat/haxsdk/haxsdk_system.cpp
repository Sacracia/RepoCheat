/*

[Index of this file]
|
|- [SECTION] Headers
|- [SECTION] Preinitialize data
|- [SECTION] Definitions from HaxSdk API
|- [SECTION] Definitions from System
|- [SECTION] Definitions from System.Runtime.InteropServices
|- [SECTION] Definitions from System.Reflection
|- [SECTION] Definitions from HaxSdk Helper Structures

*/

//-----------------------------------------------------------------------------
// [SECTION] Headers
//-----------------------------------------------------------------------------

// File Header
#include "haxsdk_system.h"

// C Headers
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// STD Headers
#include <format>
#include <unordered_map>
#include <vector>
#include <string_view>

// HaxSdk Headers
#include "backend/haxsdk_backend.h"
#include "haxsdk_assertion.h"
#include "haxsdk_logger.h"

// Third-Party Headers
#include "third_party/detours/x64/detours.h"

//-----------------------------------------------------------------------------
// [SECTION] Preinitialize data
//-----------------------------------------------------------------------------

struct FieldOffsetData
{
    OUT int*        m_Offset;       // Pointer where result will be written
    const char*     m_Assembly;     // Assembly name of the field
    const char*     m_Namespace;    // Namespace name of the field
    const char*     m_Class;        // Class name of the field
    const char*     m_Name;         // Name of the field
};

struct StaticFieldData
{
    OUT void**      m_Address;      // Pointer where result will be written
    const char*     m_Assembly;     // Assembly name of the field
    const char*     m_Namespace;    // Namespace name of the field
    const char*     m_Class;        // Class name of the field
    const char*     m_Name;         // Name of the field
};

struct EnumValueData
{
    OUT int*        m_Value;        // Pointer where result will be written
    const char*     m_Assembly;     // Assembly name of the field
    const char*     m_Namespace;    // Namespace name of the field
    const char*     m_Class;        // Class name of the field
    const char*     m_Name;         // Name of the field
};

struct MethodData
{
    OUT System::MethodInfo& m_Method; // Reference where result will be written
    const char*     m_Assembly;       // Assembly name of the method
    const char*     m_Namespace;      // Namespace name of the method
    const char*     m_Class;          // Class name of the method
    const char*     m_Name;           // Name of the method
    const char*     m_Signature;      // Signature of the method
};

struct TypeData
{
    OUT System::Type& m_Type;       // Reference where result will be written
    const char*     m_Assembly;     // Assembly name of the type
    const char*     m_Namespace;    // Namespace name of the type
    const char*     m_Name;         // Name of the type
};

//-----------------------------------------------------------------------------
// [SECTION] Global variable
//-----------------------------------------------------------------------------

namespace vtables
{
    static unsafe::VTable*          g_Int32;
}

static bool                         g_Initialized = false;
static std::vector<FieldOffsetData> g_PreInitOffsets;
static std::vector<StaticFieldData> g_PreInitStatics;
static std::vector<EnumValueData>   g_PreInitEnums;
static std::vector<MethodData>      g_PreInitMethods;
static std::vector<TypeData>        g_PreInitTypes;

//-----------------------------------------------------------------------------
// [SECTION] Definitions from HaxSdk API
//-----------------------------------------------------------------------------

namespace HaxSdk
{
    void InitSystem()
    {
        if (!g_Initialized)
        {
            g_Initialized = true;

            // Initialize backend in case user havent done that
            HaxSdk::InitBackend();

            // Initialize all vtables
            vtables::g_Int32 = unsafe::Image::GetCorlib()->GetClass("System", "Int32")->GetVTable();

            // Initialize predefined field offsets
            for (auto& data : g_PreInitOffsets)
            {
                *data.m_Offset = System::AppDomain::GetCurrent().Load(data.m_Assembly).GetType(data.m_Namespace, data.m_Class).GetField(data.m_Name).GetOffset();
                HAX_LOG_DEBUG("[PREOFFSET] {}__{}. Offset {}", data.m_Class, data.m_Name, *data.m_Offset);
            }

            // Initialize predefined static addresses
            for (auto& data : g_PreInitStatics)
            {
                *data.m_Address = System::AppDomain::GetCurrent().Load(data.m_Assembly).GetType(data.m_Namespace, data.m_Class).GetField(data.m_Name).GetValuePtr(null);
                HAX_LOG_DEBUG("[PRESTATIC] {}__{}. Address {}", data.m_Class, data.m_Name, *data.m_Address);
            }

            // Initialize predefined field offsets
            for (auto& data : g_PreInitEnums)
            {
                *data.m_Value = System::AppDomain::GetCurrent().Load(data.m_Assembly).GetType(data.m_Namespace, data.m_Class).GetField(data.m_Name).GetEnumValue();
                HAX_LOG_DEBUG("[PRESTATIC] {}__{}. Enum {}", data.m_Class, data.m_Name, *data.m_Value);
            }

            // Initialize predefined methods
            for (auto& data : g_PreInitMethods)
            {
                data.m_Method = System::AppDomain::GetCurrent().Load(data.m_Assembly).GetType(data.m_Namespace, data.m_Class).GetMethod(data.m_Name, data.m_Signature);
                HAX_LOG_DEBUG("[PREMETHOD] {}__{} {}", data.m_Class, data.m_Name, (void*)data.m_Method.m_Pointer);
            }

            // Initialize predefined types
            for (auto& data : g_PreInitTypes)
            {
                data.m_Type = System::AppDomain::GetCurrent().Load(data.m_Assembly).GetType(data.m_Namespace, data.m_Name);
                HAX_LOG_DEBUG("[RRETYPE] {}.{} {}", data.m_Namespace, data.m_Name, (void*)data.m_Type.GetPointer());
            }
        }
    }

    void ThrowNullRef()
    {
        throw System::NullReferenceException::New();
    }

    void ThrowOutOfRange()
    {
        throw System::ArgumentOutOfRangeException::New();
    }

    void SetLine(int line)
    {
        System::Exception::s_Line = line;
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Definitions from System
//-----------------------------------------------------------------------------

namespace System
{
    Type Object::GetType()
    {
        CHECK_NULL(); 
        return Type(m_Pointer->GetClass()->GetType()->GetReflectionType());
    }

    Type Assembly::GetType(const char* nameSpace, const char* name, bool doAssert)
    {
        CHECK_NULL();
        unsafe::Class* klass = m_Pointer->GetClass(nameSpace, name, doAssert);
        return klass ? Type(klass->GetType()->CreateReflectionType()) : Type((unsafe::Object*)nullptr);
    }

    FieldInfo Type::GetField(const char* name, bool doAssert)
    {
        CHECK_NULL(); 
        return FieldInfo(GetPointer()->GetType()->GetClass()->GetField(name, doAssert));
    }

    MethodInfo Type::GetMethod(const char* name, const char* sig, bool doAssert)
    { 
        CHECK_NULL(); 
        return MethodInfo(GetPointer()->GetType()->GetClass()->GetMethod(name, sig, doAssert)); 
    }

    Type::Type(const char* assembly, const char* nameSpace, const char* name)
    {
        if (!g_Initialized)
            g_PreInitTypes.emplace_back(*this, assembly, nameSpace, name);
    }

    Char& String::operator[](int i)
    {
        CHECK_NULL();

        if (i < 0 || i >= GetPointer()->GetLength() - 1)
            throw ArgumentOutOfRangeException::New();

        return GetPointer()->operator[](i);
    }

    const Char& String::operator[](int i) const
    {
        CHECK_NULL();

        if (i < 0 || i >= GetPointer()->GetLength() - 1)
            throw ArgumentOutOfRangeException::New();

        return GetPointer()->operator[](i);
    }

    bool String::operator==(const String& o) const
    {
        if (o.m_Pointer == m_Pointer)
            return true;
        if (!o.m_Pointer || !m_Pointer)
            return false;
        return o.GetLength() == GetLength() && wcscmp(o.GetRawStringData(), GetRawStringData()) == 0;
    }

    Char* String::begin()
    {
        CHECK_NULL();

        return GetPointer()->begin();
    }

    const Char* String::begin() const
    {
        CHECK_NULL();

        return GetPointer()->begin();
    }

    Char* String::end()
    {
        CHECK_NULL();

        unsafe::String* native = GetPointer();
        return native->begin() + native->GetLength();
    }

    const Char* String::end() const
    {
        CHECK_NULL();

        unsafe::String* native = GetPointer();
        return native->begin() + native->GetLength();
    }

    String String::Concat(String s1, String s2)
    {
        static MethodInfo method = Assembly::Corlib().GetType("System", "String").GetMethod("Concat", "System.String(System.String,System.String)");
        if (HaxSdk::IsMono())
            return method.Thunk<String, String, String>(s1, s2);
        return method.Address<String, String, String>(s1, s2);
    }

    int String::GetLength() const
    {
        CHECK_NULL();

        return GetPointer()->GetLength();
    }

    Char* String::GetRawStringData() const
    {
        CHECK_NULL();

        return GetPointer()->GetRawStringData();
    }

    char* String::UTF8() const
    {
        CHECK_NULL();

        unsafe::String* native = GetPointer();
        return HaxSdk::UTF8(native->GetRawStringData(), native->GetLength());
    }

    void String::UTF8(char* buff, size_t buffSize) const
    {
        CHECK_NULL();

        unsafe::String* native = GetPointer();
        const char* utf8 = HaxSdk::UTF8(native->GetRawStringData(), native->GetLength());
        strcpy_s(buff, buffSize, utf8);
    }

    bool String::StartsWith(const wchar_t* prefix)
    {
        CHECK_NULL();

        std::wstring_view view(GetPointer()->GetRawStringData());
        return view.starts_with(prefix);
    }

    bool String::EndsWith(const wchar_t* postfix)
    {
        CHECK_NULL();

        std::wstring_view view(GetPointer()->GetRawStringData());
        return view.ends_with(postfix);
    }

    bool String::Contains(const wchar_t* substr)
    {
        CHECK_NULL();

        std::wstring_view view(GetPointer()->GetRawStringData());
        return view.find(substr) != std::wstring_view::npos;
    }

    bool MethodInfo::Hook(void* detour)
    {
        CHECK_NULL();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        LONG err = DetourAttach(&m_Orig, detour);
        if (err != NO_ERROR)
            HAX_LOG_ERROR("Unable to hook {}.{} with error {}", m_Base->GetClass()->GetName(), m_Base->GetName(), err);
        DetourTransactionCommit();
        return err == NO_ERROR;
    }

    Assembly AppDomain::Load(const char* name, bool doAssert) 
    { 
        CHECK_NULL(); 
        return Assembly(m_Pointer->GetImage(name, doAssert)); 
    }

    String Exception::ToString()
    {
        CHECK_NULL();
        static MethodInfo method = Assembly::Corlib().GetType("System", "Exception").GetMethod("ToString", "System.String()");
        if (HaxSdk::IsMono())
            return method.Thunk<String, Exception>(*this);
        return method.Address<String, Exception>(*this);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Definitions from System.Runtime.InteropServices
//-----------------------------------------------------------------------------

namespace System
{
    GCHandle& GCHandle::operator=(GCHandle&& other) noexcept
    {
        if (&other != this)
        {
            m_Handle = other.m_Handle;
            other.m_Handle = 0;
        }
        return *this;
    }

    Object GCHandle::GetTarget()
    {
        if (!m_Handle)
            return Object(nullptr);

        return Object(unsafe::GCHandle::GetTarget(m_Handle));
    }

    void GCHandle::Free()
    {
        if (!m_Handle)
            unsafe::GCHandle::Free(m_Handle);
    }

    uint32_t GCHandle::GetTargetHandle(const Object& obj, GCHandleType type)
    {
        if (!obj)
            return 0;

        if (type == GCHandleType::Normal)
            return unsafe::GCHandle::New(obj.GetPointer(), false);

        return unsafe::GCHandle::NewWeak(obj.GetPointer(), false);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Definitions from System.Reflection
//-----------------------------------------------------------------------------

namespace System
{
    void* FieldInfo::GetValuePtr(const Object& __this)
    {
        CHECK_NULL();
        if (!IsStatic() && !__this)
            throw TargetException::New(String::New("Non-static field requires a target"));
        return m_Pointer->GetValuePtr(__this.GetPointer());
    }

    MethodInfo::MethodInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name, const char* sig) : m_Base(nullptr), m_Orig(nullptr), m_Pointer(nullptr), m_Thunk(nullptr)
    {
        if (!g_Initialized)
            g_PreInitMethods.emplace_back(*this, assembly, nameSpace, klass, name, sig);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Definitions from HaxSdk Helper Structures
//-----------------------------------------------------------------------------

namespace HaxSdk
{
    FieldOffset::FieldOffset(const char* assembly, const char* nameSpace, const char* klass, const char* name)
    {
        if (!g_Initialized)
            g_PreInitOffsets.emplace_back(&m_Offset, assembly, nameSpace, klass, name);
    }

    StaticField::StaticField(const char* assembly, const char* nameSpace, const char* klass, const char* name)
    {
        if (!g_Initialized)
            g_PreInitStatics.emplace_back(&m_Address, assembly, nameSpace, klass, name);
    }

    EnumValue::EnumValue(const char* assembly, const char* nameSpace, const char* klass, const char* name)
    {
        if (!g_Initialized)
            g_PreInitEnums.emplace_back(&m_Value, assembly, nameSpace, klass, name);
    }
}