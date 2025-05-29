
/*

[Index of this file]
|
|- [SECTION] Headers
|- [SECTION] HaxSdk API
|
|- [SECTION] Forward declarations and basic types
|
|- [SECTION] System
|    |
|    |- [STRUCTS]
│    |    |- Vector2
|    |
|    |- [CLASSES]
│         |- Object
│         |- String
│         |- Array
│         |- Type
│         |- AppDomain
│         |- Exception
│         |- NullReferenceException
│         |- ArgumentOutOfRangeException
|
|- [SECTION] System.Collections.Generic
|    |
|    |- [ENUMS]
│    |    |- 
|    |
|    |- [STRUCTS]
│    |    |- 
|    |
|    |- [CLASSES]
|         |- Dictionary<TKey, TValue>
│         |- List<T>
|
|- [SECTION] System.Runtime.InteropServices
|    |
|    |- [ENUMS]
│    |    |- GCHandleType
|    |
|    |- [STRUCTS]
│    |    |- GCHandle
|    |
|    |- [CLASSES]
│         |- 
|
|- [SECTION] System.Reflection
|    |
|    |- [CLASSES]
|         |- Assembly
│         |- FieldInfo
│         |- MethodInfo
│         |- TargetException
|
|- [SECTION] HaxSdk Helper Structures
|- [SECTION] TypeOfs

*/

// Enumerations:
// - Unity enums are typically represented as signed 32-bit integers (int32_t).
// - Enums can be passed directly as integers when calling managed methods.
// - Use static_cast<EnumType>(intValue) for clarity and type safety when setting or reading values.
// - For enums with flag behavior (e.g., HideFlags), bitwise operations (|, &, ~) are supported.
// - EnumValue helper allows resolving enum constants by name at initialization, and will assert if the value was not found.
//   This ensures the metadata is consistent with the current Unity version.

// Structures and their boxed classes
// - All structs (e.g., Vector3, Quaternion, Bounds) are value types in C#.
// - In Mono, value types must be boxed (wrapped into a System.Object) to be passed into managed methods expecting an object.
// - In IL2CPP, structs are passed directly by value and boxing is generally not required.
// - HaxSdk provides lightweight boxed wrappers (e.g., Vector3_Boxed, Int32_Boxed) that can be stack-allocated
//   and passed to Mono thunks or inserted into managed containers like List<object>.
// - Boxing is done manually by allocating a managed object and writing the struct's memory layout directly into the object's buffer.
// - This approach avoids heap allocations when performance matters, and allows precise control over managed interop.

// Classes (managed reference types)
// - All C# classes (e.g., Object, Array, Type...) are represented in this SDK as lightweight wrappers around a raw MonoObject*.
// - These wrappers do not manage lifetime or reference count — they merely provide typed access to the managed object.
// - Constructors in these wrappers DO NOT allocate objects on the managed heap (unlike new in C#), they only wrap existing pointers.
// - To create new managed objects (e.g., new String("abc")), use explicitly provided static "New" methods like String::New(), Array::New(), etc.
//
// ================== BE CAREFUL ==================
// - Wrapping a pointer does NOT protect the managed object from garbage collection.
//   If the object is not referenced elsewhere, the GC is free to collect it at any point.
// - Attempting to use a wrapper around a collected object leads to invalid memory access and game crashes.
// - Tip: To pin the object and prevent it from being collected, use System::GCHandle.
//   This adds a GC root and ensures the object stays alive as long as needed. See GCHandle for details.
// =================================================
//
// - All wrappers implement Null() checks and assert if called on invalid or null pointers.
// - Accessor methods use CHECK_NULL() and other runtime guards to detect invalid state.
// - When an error is detected (e.g., missing method, null instance), the SDK throws a managed System::Exception
//   (e.g., NullReferenceException, ArgumentOutOfRangeException), implemented internally via Mono API.
// - These exceptions can be caught using regular C++ try/catch. The entire ImGui rendering pipeline is wrapped in try/catch
//   to prevent crashes and allow graceful failure during drawing.

//-----------------------------------------------------------------------------
// [SECTION] Headers
//-----------------------------------------------------------------------------

#pragma once

// STD Headers
#include <type_traits>

// HaxSdk Headers
#include "backend/haxsdk_backend.h"
#include "haxsdk_tools.h"
#include "haxsdk_assertion.h"

// Shorter null check
#define CHECK_NULL()        \
if (Null())                 \
    HaxSdk::ThrowNullRef();

// Shorter range check
#define CHECK_RANGE(i, l, r)\
if (i < l || i > r)         \
    HaxSdk::ThrowOutOfRange();

#define STATIC // Used for definitions of static methods
#define OUT // Used for parameters that keep output value

//-----------------------------------------------------------------------------
// [SECTION] HaxSdk API
//-----------------------------------------------------------------------------

namespace HaxSdk
{
    // Initializes system module
    // Must be called once before using this module
    void InitSystem();

    // Throws NullReferenceException
    // Purpose: Escape circular dependency
    void ThrowNullRef();

    // Throws OutOfRangeException
    // Purpose: Escape circular dependency
    void ThrowOutOfRange();

    // Sets line where exception was raised
    void SetLine(int line);
}

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations and basic types
//-----------------------------------------------------------------------------

// Primitive C# types
// TODO: Make structures instead of types
using Boolean   = bool;
using Byte      = uint8_t;
using SByte     = int8_t;
using Int16     = int16_t;
using UInt16    = uint16_t;
using Int32     = int32_t;
using UInt32    = uint32_t;
using Int64     = int64_t;
using UInt64    = uint64_t;
using Char      = wchar_t;
using Double    = double;
using Single    = float;
using IntPtr    = void*;

namespace System
{
    struct Assembly;
    struct Type;
    struct MethodInfo;
    struct FieldInfo;
}

//-----------------------------------------------------------------------------
// [SECTION] System
//-----------------------------------------------------------------------------

namespace System
{
    // Based on native MonoObject
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.object
    struct Object
    {
                                // Constructors
        explicit                Object() = default;
        explicit constexpr      Object(nullptr_t ptr) : m_Pointer(ptr) {}
        explicit constexpr      Object(unsafe::Object* ptr) : m_Pointer(ptr) {}
        explicit constexpr      Object(unsafe::Exception* ptr) : m_Pointer(ptr) {}
        explicit constexpr      Object(unsafe::String* ptr) : m_Pointer(ptr) {}

                                // Operators
        inline                  operator bool() const { return m_Pointer != nullptr; }

                                // Static Methods
        inline static bool      ReferenceEquals(const Object& objA, const Object& objB) { return objA.m_Pointer == objB.m_Pointer; }

                                // Public Methods
        Type                    GetType();

                                // HaxSdk Extension
        inline bool             Null() const { return m_Pointer == nullptr; }
        inline unsafe::Object*  GetPointer() const { return m_Pointer; }

    protected:
                                // Members
        unsafe::Object*         m_Pointer;
    };

    // Based on native MonoString
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.string
    struct String : Object
    {
                                // Constructors
        explicit                String(unsafe::Object* ptr) : Object(ptr) {}
        explicit                String(unsafe::String* ptr) : Object(ptr) {}
        static inline String    New(const char* str) { return String(unsafe::String::New(str)); }

                                // Operators
        Char&                   operator[](int i);
        const Char&             operator[](int i) const;
        bool                    operator==(const String& o) const;
                                operator bool() const { return m_Pointer != nullptr; }

                                // Iterators
        Char*                   begin();
        const Char*             begin() const;
        Char*                   end();
        const Char*             end() const;

                                // Static Methods
        static String           Concat(String s1, String s2);

                                // Public Methods
        int                     GetLength() const;
        bool                    StartsWith(const wchar_t* prefix);
        bool                    EndsWith(const wchar_t* postfix);
        bool                    Contains(const wchar_t* substr);

                                // Private Methods
        Char*                   GetRawStringData() const;

                                // HaxSdk Methods
        inline unsafe::String*  GetPointer() const { return (unsafe::String*)m_Pointer; }
        char*                   UTF8() const;
        void                    UTF8(char* buff, size_t buffSize) const;
    };

    // Based on native MonoArray
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.array
    template <typename T>
    struct Array : Object
    {
                                // Constructors
        explicit                Array(unsafe::Object* ptr) : Object(ptr) {}
        explicit                Array(unsafe::Array<T>* ptr) : Object(ptr) {}

                                // Operators
        T&                      operator[](size_t i)        { CHECK_NULL(); CHECK_RANGE(i + 1, 1, GetPointer()->GetLength()); return GetPointer()->operator[](i); }
        const T&                operator[](size_t i) const  { CHECK_NULL(); CHECK_RANGE(i + 1, 1, GetPointer()->GetLength()); return GetPointer()->operator[](i); }
        inline                  operator bool() const       { return m_Pointer != nullptr; }

                                // Iterators
        T*                      begin()                     { CHECK_NULL(); return GetPointer()->begin(); }
        const T*                begin() const               { CHECK_NULL(); return GetPointer()->begin(); }
        T*                      end()                       { CHECK_NULL(); return GetPointer()->end(); }
        const T*                end() const                 { CHECK_NULL(); return GetPointer()->end(); }

                                // Public Methods
        inline size_t           GetLength() const           { CHECK_NULL(); return GetPointer()->GetLength(); }

                                // HaxSdk Methods
        unsafe::Array<T>*       GetPointer() const          { return (unsafe::Array<T>*)m_Pointer; }
    };

    // Based on native MonoReflectionType
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.type
    struct Type : Object
    {
                                // Constructors
        explicit                Type(unsafe::Object* ptr) : Object(ptr) {}
        explicit                Type(const char* assembly, const char* nameSpace, const char* name);
        explicit                Type(unsafe::ReflectionType* ptr) : Object(ptr) {}

                                // Operators
        inline                  operator bool() const { return !Null(); }

                                // Public Methods
        FieldInfo               GetField(const char* name, bool doAssert = true);
        MethodInfo              GetMethod(const char* name, const char* sig = nullptr, bool doAssert = true);

                                // HaxSdk Methods
        unsafe::ReflectionType* GetPointer() const { return (unsafe::ReflectionType*)m_Pointer; }
    };

    // Based on native MonoDomain
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.appdomain
    struct AppDomain
    {
                                // Constructors
        explicit                AppDomain(unsafe::Domain* ptr) : m_Pointer(ptr) {}

                                // Static Methods
        static AppDomain        GetCurrent() { return AppDomain(unsafe::Domain::GetRoot()); }

                                // Public Methods
        inline Assembly         Load(const char* name, bool doAssert = true);

                                // HaxSdk Methods
        inline bool             Null() { return m_Pointer == nullptr; }

    private:
                                // Members
        unsafe::Domain*         m_Pointer;
    };

    // Based on native MonoException
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.exception
    struct Exception : Object
    {
                                // Constructors
        explicit                Exception(unsafe::Exception* ptr) : Object(ptr) {}
        explicit                Exception(const Il2CppExceptionWrapper& ex) : Object((unsafe::Exception*)ex.ex) {}

                                // Operators
        inline                  operator bool() const { return !Null(); }

                                // Public Methods
        inline String           GetMessage() { CHECK_NULL(); return String(GetPointer()->GetMessage()); }
        inline String           GetStackTrace() { CHECK_NULL(); return String(GetPointer()->GetStackTrace()); }
        String                  ToString();

                                // HaxSdk Methods
        unsafe::Exception*      GetPointer() const { return (unsafe::Exception*)m_Pointer; }

                                // Members
        static inline int       s_Line = 0;
    };

    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.nullreferenceexception
    struct NullReferenceException : Exception
    {
                                // Constructors
        explicit                NullReferenceException(unsafe::Exception* ptr) : Exception(ptr) {}
        static inline auto      New() { return NullReferenceException(unsafe::Exception::GetNullReference()); }

                                // Operators
        inline                  operator bool() const { return !Null(); }
    };

    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.argumentoutofrangeexception
    struct ArgumentOutOfRangeException : Exception
    {
                                // Constructors
        explicit                ArgumentOutOfRangeException(unsafe::Exception* ptr) : Exception(ptr) {}
        static inline auto      New() { return ArgumentOutOfRangeException(unsafe::Exception::GetArgumentOutOfRange()); }
        
                                // Operators
        inline                  operator bool() const { return !Null(); }
    };
}

//-----------------------------------------------------------------------------
// [SECTION] System.Reflection
//-----------------------------------------------------------------------------

namespace System
{
    // Based on native MonoImage.
    // Mono has MonoAssembly but MonoImage serves our purpose better.
    // ================== BE CAREFUL ==================
    // This class IS NOT equal to actual Reflection.FieldInfo
    // Do not pass it where .NET expects real FieldInfo!
    // =================================================
    // Docs: https://learn.microsoft.com/ru-ru/dotnet/api/system.reflection.assembly
    struct Assembly
    {
        // Constructors
        explicit                Assembly(unsafe::Image* ptr) : m_Pointer(ptr) {}

        // Public Methods
        Type                    GetType(const char* nameSpace, const char* name, bool doAssert = true);

        // HaxSdk Methods
        inline bool             Null() { return m_Pointer == nullptr; }
        static Assembly         Corlib() { return Assembly{ unsafe::Image::GetCorlib() }; }

    private:
        // Members
        unsafe::Image*          m_Pointer;
    };

    // Based on native MonoField.
    // ================== BE CAREFUL ==================
    // This class IS NOT equal to actual Reflection.FieldInfo
    // Do not pass it where .NET expects real FieldInfo!
    // =================================================
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.reflection.fieldinfo
    struct FieldInfo
    {
        // Constructors
        explicit                FieldInfo(unsafe::Field* ptr) : m_Pointer(ptr) {}

        // Public Methods
        inline bool             IsLiteral() { CHECK_NULL(); return m_Pointer->IsLiteral(); }
        inline bool             IsStatic() { CHECK_NULL(); return m_Pointer->IsStatic(); }
        void* GetValuePtr(const Object& __this);

        // Private Methods
        inline int              GetOffset() { CHECK_NULL();  return m_Pointer->GetOffset(); }

        // HaxSdk Extention
        inline bool             Null() { return m_Pointer == nullptr; }
        inline int              GetEnumValue() { CHECK_NULL(); int v; m_Pointer->GetStaticValue(&v); return v; }

    private:
        // Members
        unsafe::Field* m_Pointer;
    };

    // Based on native MonoMethod.
    // ================== BE CAREFUL ==================
    // This class IS NOT equal to actual Reflection.MethodInfo
    // Do not pass it where .NET expects real MethodInfo!
    // =================================================
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.reflection.methodinfo
    struct MethodInfo
    {
        explicit            MethodInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name, const char* sig = nullptr);

        explicit            MethodInfo(unsafe::Method* base) : m_Base(base)
        {
            m_Pointer = m_Orig = base->GetPointer();
            m_Thunk = base->GetThunk();
        }
        bool                Hook(void* detour);

        template <typename Ret, typename... Args>
        Ret Address(const Args&... args)
        {
            CHECK_NULL();
            try
            {
                if constexpr (std::is_void_v<Ret>)
                {
                    ((void(*)(Args...))m_Pointer)(args...);
                    return;
                }
                else if constexpr (std::is_base_of_v<Object, Ret>)
                {
                    unsafe::Object* res = ((unsafe::Object * (*)(Args...))m_Pointer)(args...);
                    return Ret(res);
                }
                else
                {
                    return ((Ret(*)(Args...))m_Pointer)(args...);
                }
            }
            catch (const Il2CppExceptionWrapper& ex)
            {
                throw System::Exception(ex);
            }
        }

        template <typename Ret, typename... Args>
        Ret Thunk(const Args&... args)
        {
            HAX_ASSERT_E(m_Thunk, "Thunk is not available for your game");
            CHECK_NULL();
            Exception ex(nullptr);
            if constexpr (std::is_void_v<Ret>)
            {
                ((void(*)(Args..., Exception*))m_Thunk)(args..., &ex);
                if (ex)
                {
                    System::Exception::s_Line = __LINE__;
                    throw ex;
                }
                return;
            }
            else if constexpr (std::is_base_of_v<Object, Ret>)
            {
                void* res = ((void* (*)(Args..., Exception*))m_Thunk)(args..., &ex);
                if (ex)
                {
                    System::Exception::s_Line = __LINE__;
                    throw ex;
                }
                return Ret((unsafe::Object*)res);
            }
            else
            {
                Ret res = ((Ret(*)(Args..., Exception*))m_Thunk)(args..., &ex);
                if (ex)
                {
                    System::Exception::s_Line = __LINE__;
                    throw ex;
                }
                return res;
            }
        }

        template <typename Ret, typename... Args>
        Ret Invoke(const Args&... args)
        {
            CHECK_NULL();
            static_assert(sizeof... (args) > 0);

            unsafe::Exception* ex = nullptr;
            void* packedArgs[] = { Pack(args)... };
            int nArgs = sizeof(packedArgs) / sizeof(void*);
            unsafe::Object* res = m_Base->Invoke(packedArgs[0], nArgs == 1 ? nullptr : packedArgs + 1, &ex);

            if (ex)
            {
                System::Exception::s_Line = __LINE__;
                throw System::Exception(ex);
            }

            if constexpr (std::is_void_v<Ret>) return;
            else if constexpr (std::is_base_of_v<System::Object, Ret>) return Ret(res);
            else return static_cast<Ret>(res);
        }

        template <typename Ret, typename... Args>
        Ret InvokeStatic(const Args&... args)
        {
            CHECK_NULL();
            if constexpr (sizeof...(args) == 0)
            {
                unsafe::Exception* ex = nullptr;
                unsafe::Object* res = m_Base->Invoke(nullptr, nullptr, &ex);

                if (ex)
                    throw System::Exception(ex);

                if constexpr (std::is_void_v<Ret>)
                    return;
                else if constexpr (std::is_base_of_v<System::Object, Ret>)
                    return Ret(res);
                else
                    return static_cast<Ret>(res);
            }
            else
            {
                unsafe::Exception* ex = nullptr;
                void* packedArgs[] = { Pack(args)... };
                unsafe::Object* res = m_Base->Invoke(nullptr, packedArgs, &ex);

                if (ex)
                    throw System::Exception(ex);

                if constexpr (std::is_void_v<Ret>)
                    return;
                else if constexpr (std::is_base_of_v<System::Object, Ret>)
                    return Ret(res);
                else
                    return static_cast<Ret>(res);
            }
        }

        inline bool         Null() { return m_Base == nullptr; }

    private:
        template <typename T>
        void* Pack(const T& val)
        {
            if constexpr (std::is_pointer_v<T>) return val;
            if constexpr (std::is_base_of_v<System::Object, T>) return val.GetPointer();
            else return (void*)&val;
        }

    public:
        unsafe::Method* m_Base;
        void* m_Pointer;
        void* m_Thunk;
        void* m_Orig;
    };

    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.reflection.targetexception
    struct TargetException : Exception
    {
        // Constructors
        explicit                TargetException(unsafe::Exception* ptr) : Exception(ptr) {}
        static inline auto      New(const String& message) { return TargetException(unsafe::Exception::GetTargetException(message.GetPointer())); }

        // Operators
        inline                  operator bool() const { return !Null(); }
    };
}

//-----------------------------------------------------------------------------
// [SECTION] System.Collections.Generic
//-----------------------------------------------------------------------------

namespace System
{
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.dictionary-2
    template <typename TKey, typename TValue>
    struct Dictionary : Object
    {
                                // Constructors
        explicit                Dictionary(unsafe::Object* ptr) : Object(ptr) {}

                                // Operators
        inline                  operator bool() const { return !Null(); }

                                // Iterators
        const auto              begin() const { CHECK_NULL(); return GetPointer()->begin(); }
        auto                    begin() { CHECK_NULL(); return GetPointer()->begin(); }
        const auto              end() const { CHECK_NULL(); return GetPointer()->end(); }
        auto                    end() { CHECK_NULL(); return GetPointer()->end(); }

                                // Public Methods
        bool                    ContainsKey(const TKey& key);
        TValue&                 GetItem(const TKey& key);
        int                     Count();

                                // HaxSdk Methods
        inline auto             GetPointer() { return (unsafe::Dictionary<TKey, TValue>*)m_Pointer; }
    };

    template <typename TKey, typename TValue>
    bool Dictionary<TKey, TValue>::ContainsKey(const TKey& key)
    {
        CHECK_NULL();
        return GetPointer()->GetItem(key) != nullptr;
    }

    template <typename TKey, typename TValue>
    TValue& Dictionary<TKey, TValue>::GetItem(const TKey& key)
    {
        CHECK_NULL();
        return *GetPointer()->GetItem(key);
    }

    template <typename TKey, typename TValue>
    int Dictionary<TKey, TValue>::Count()
    {
        CHECK_NULL();
        return GetPointer()->m_Count;
    }
    
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1
    template <typename T>
    struct List : Object
    {
        // Constructors
        explicit                List(unsafe::Object* ptr) : Object(ptr) {}
        explicit                List(unsafe::List<T>* ptr) : Object(ptr) {}

        // Operators
        T&                      operator[](size_t i) { CHECK_NULL(); CHECK_RANGE(i, 0, GetPointer()->GetLength() - 1); return GetPointer()->operator[](i); }
        const T&                operator[](size_t i) const { CHECK_NULL(); CHECK_RANGE(i, 0, GetPointer()->GetLength() - 1); return GetPointer()->operator[](i); }
        inline                  operator bool() const { return m_Pointer != nullptr; }

        // Pointers
        T*                      begin() { CHECK_NULL(); return GetPointer()->begin(); }
        const T*                begin() const { CHECK_NULL(); return GetPointer()->begin(); }
        T*                      end() { CHECK_NULL(); return GetPointer()->end(); }
        const T*                end() const { CHECK_NULL(); return GetPointer()->end(); }

        // Public Methods
        inline int              GetCount() const { CHECK_NULL(); return GetPointer()->GetLength(); }
        inline int              GetCapacity() const { CHECK_NULL(); return GetPointer()->GetCapacity(); }
        bool                    Contains(T val);

        // HaxSdk Methods
        inline unsafe::List<T>* GetPointer() const { return (unsafe::List<T>*)m_Pointer; }
    };

    template <typename T>
    bool List<T>::Contains(T val)
    {
        CHECK_NULL();

        static MethodInfo method = this->GetType().GetMethod("Contains");
        if (HaxSdk::IsMono())
            return method.Thunk<bool, List<T>, T>(*this, val);

        return method.Address<bool, List<T>, T>(*this, val);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] System.Runtime.InteropServices
//-----------------------------------------------------------------------------

namespace System
{
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.runtime.interopservices.gchandletype
    enum GCHandleType
    {
        Weak,
        Normal
    };

    // Based on native mono API
    // Provides RAII implementation
    // Docs: https://learn.microsoft.com/en-us/dotnet/api/system.runtime.interopservices.gchandle
    struct GCHandle
    {
                                // Constructors
        explicit                GCHandle(uint32_t handle) : m_Handle(handle) {}
                                GCHandle(const GCHandle&) = delete;
                                GCHandle(GCHandle&& other) noexcept { m_Handle = other.m_Handle; other.m_Handle = 0; }

                                // Destructors
                                ~GCHandle() { Free(); }

                                // Operators
        GCHandle&               operator=(const GCHandle&) = delete;
        GCHandle&               operator=(GCHandle&& other) noexcept;

                                // Static Methods
        static GCHandle         Alloc(const Object& obj) { return GCHandle(GetTargetHandle(obj, GCHandleType::Normal)); }
        static GCHandle         Alloc(const Object& obj, GCHandleType type) { return GCHandle(GetTargetHandle(obj, type)); }
        static uint32_t         GetTargetHandle(const Object& obj, GCHandleType type);

                                // Public Methods
        Object                  GetTarget();
        void                    Free();

        // Members
        uint32_t                m_Handle;
    };
}

//-----------------------------------------------------------------------------
// [SECTION] HaxSdk Helper Structures
//-----------------------------------------------------------------------------

namespace HaxSdk
{
    struct FieldOffset
    {
        explicit                FieldOffset(const char* assembly, const char* nameSpace, const char* klass, const char* name);

        int                     m_Offset = -1;
    };

    struct StaticField
    {
        explicit                StaticField(const char* assembly, const char* nameSpace, const char* klass, const char* name);

        void* m_Address = nullptr;
    };

    struct EnumValue
    {
        explicit                EnumValue(const char* assembly, const char* nameSpace, const char* klass, const char* name);

        int                     m_Value = -1;
    };
}

// Alternative to "null" in C#
#ifndef EXCLUDE_NULL
inline constexpr System::Object null{ nullptr };
#endif

//-----------------------------------------------------------------------------
// [SECTION] TypeOfs
//-----------------------------------------------------------------------------

template <typename T>
extern System::Type typeof;

template <>
inline System::Type typeof<int> = System::Type("mscorlib", "System", "Object");

template <>
inline System::Type typeof<System::Type> = System::Type("mscorlib", "System", "Type");

template <>
inline System::Type typeof<System::AppDomain> = System::Type("mscorlib", "System", "AppDomain");

template <>
inline System::Type typeof<System::Exception> = System::Type("mscorlib", "System", "Exception");

template <>
inline System::Type typeof<System::NullReferenceException> = System::Type("mscorlib", "System", "NullReferenceException");

template <>
inline System::Type typeof<System::ArgumentOutOfRangeException> = System::Type("mscorlib", "System", "ArgumentOutOfRangeException");

template <>
inline System::Type typeof<System::String> = System::Type("mscorlib", "System", "String");