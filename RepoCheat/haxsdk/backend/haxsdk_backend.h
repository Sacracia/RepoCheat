// Backend module (haxsdk_backend.h)
// - This file provides the low-level unified interface to both Mono and IL2CPP scripting backends used in Unity.
// - It acts as a foundational abstraction layer for backend-specific runtime representations (MonoObject, Il2CppObject, etc.),
//   enabling the rest of the SDK (System, Unity) to work agnostically across different Unity configurations.

// Architectural Role
// - This module implements the **Bridge** design pattern — it decouples higher-level safe API logic (e.g., in haxsdk_system.h, haxsdk_unity.h)
//   from backend-specific memory layouts and function sets.
// - It unifies Mono and IL2CPP object access under shared interface types such as `unsafe::Class`, `unsafe::Field`, `unsafe::Method`, etc.
//
// - All types in this file are considered unsafe:
//   - They expose raw pointers and assume the developer knows the internal runtime state.
//   - There are no null checks — invalid access will result in immediate crashes.

// Use with Care
// - The types in this file are designed for internal or advanced use. They should NOT be used directly for mod/game logic.
// - Instead, developers should rely on the safe high-level API in `haxsdk_system.h` and `haxsdk_unity.h`, which wrap these interfaces
//   with proper null checks, GC handling, boxing/unboxing, and exception management.

// Backend Switching
// - Backend support is toggled internally based on what Unity uses at runtime:
//     - If the game uses Mono, the `unsafe::` types operate on native `MonoObject`, `MonoClass`, etc.
//     - If the game uses IL2CPP, the same `unsafe::` types transparently redirect to `Il2CppObject`, `Il2CppClass`, etc.
// - Backend state is determined and initialized at runtime via `HaxSdk::InitBackend()`.
// - You can check backend type using:
//     - `HaxSdk::IsMono()`
//     - `HaxSdk::IsIl2Cpp()`

#pragma once

#include <cstdint>

#include "mono/haxsdk_mono.h"
#include "il2cpp/haxsdk_il2cpp.h"

#undef GetMessage

#define ALIGN(x) __declspec(align(x))

namespace unsafe
{
    struct Class;
    struct Field;
    struct Image;
    struct Method;
    struct ReflectionType;
    struct String;
    struct Type;
    struct VTable;

    struct FieldOffset;
    struct EnumValue;
}

enum HaxBackend
{
    HaxBackend_None,
    HaxBackend_Mono,
    HaxBackend_Il2Cpp
};

namespace HaxSdk
{
    void InitBackend();
    void AttachThread();
    bool IsMono();
    bool IsIl2Cpp();
    void* GetBackendHandle();
}

namespace unsafe
{
    struct Object
    {
        explicit            Object(VTable* vtable) : m_VTable(vtable), m_Monitor(nullptr) {}

        static Object*      Box(Class* klass, void* data);
        static Object*      New(Class* klass);
        
        Object*             Ctor();
        Class*              GetClass();
        void*               Unbox();

        VTable*             m_VTable;
        void*               m_Monitor;
    };

    template <typename T>
    struct Array : Object
    {
        inline T&           operator[](size_t i)            { return m_Items[i]; }
        inline const T&     operator[](size_t i) const      { return m_Items[i]; }

        inline T*           begin()                         { return m_Items; }
        inline const T*     begin() const                   { return m_Items; }
        inline T*           end()                           { return m_Items + m_Size; }
        inline const T*     end() const                     { return m_Items + m_Size; }

        inline size_t       GetLength()                     { return m_Size; }

    private:
        void*               m_Bounds;
        size_t              m_Size;
        ALIGN(8) T          m_Items[1];
    };

    struct Assembly
    {
        Image*              GetImage();

        union
        {
            MonoAssembly    m_Mono;
            Il2CppAssembly  m_Il2Cpp;
        };
    };

    struct Class
    {
        Field*              GetField(const char* name, bool doAssert = true);
        Method*             GetMethod(const char* name, const char* sig = nullptr, bool doAssert = true);
        Type*               GetType();
        VTable*             GetVTable();
        const char*         GetName();

        union
        {
            MonoClass       m_Mono;
            Il2CppClass     m_Il2Cpp;
        };
    };

    struct Domain
    {
        static Domain*      GetRoot();
        Assembly*           GetAssembly(const char* name, bool doAssert = true);
        Image*              GetImage(const char* name, bool doAssert = true);

        union
        {
            Il2CppDomain    m_Il2Cpp;
            MonoDomain      m_Mono;
        };
    };

    struct Exception : Object
    {
        String*&            GetMessage();
        String*             GetStackTrace();

        static Exception*   GetNullReference();
        static Exception*   GetArgumentOutOfRange();
        static Exception*   GetTargetException(String* message);

        inline MonoException*   ToMono()                    { return (MonoException*)this; }
        inline Il2CppException* ToIl2Cpp()                  { return (Il2CppException*)this; }

        //union
        //{
        //    MonoException m_Mono;
        //    Il2CppException m_Il2Cpp;
        //};
    };

    struct Field
    {
        bool                IsStatic();
        int                 GetOffset();
        const char*         GetName();
        void                GetStaticValue(void* value);
        bool                IsLiteral();
        void*               GetValuePtr(void* __this);

        union
        {
            MonoField       m_Mono;
            Il2CppField     m_Il2Cpp;
        };
    };

    struct GCHandle
    {
        static uint32_t     New(Object* obj, bool pinned);
        static uint32_t     NewWeak(Object* obj, bool trackResurrection);
        static Object*      GetTarget(uint32_t handle);
        static void         Free(uint32_t handle);
    };

    struct Image
    {
        static Image*       FromName();
        static Image*       GetCorlib();
        static Image*       GetUnityCore();

        Class*              GetClass(const char* nameSpace, const char* name, bool doAssert = true);
    
        union
        {
            MonoImage       m_Mono;
            Il2CppImage     m_Il2Cpp;
        };
    };

    template <typename T>
    struct List : Object
    {
        inline T&           operator[](size_t i)            { return m_Items->operator[](i); }
        inline const T&     operator[](size_t i) const      { return m_Items->operator[](i); }

        inline T*           begin()                         { return m_Items->begin(); }
        inline const T*     begin() const                   { return m_Items->begin(); }
        inline T*           end()                           { return m_Items->begin() + m_Size; }
        inline const T*     end() const                     { return m_Items->begin() + m_Size; }

        inline int          GetLength()                     { return m_Size; }
        inline size_t       GetCapacity()                   { return m_Items->m_Size; }

        Array<T>*           m_Items;
        int                 m_Size;
        int                 m_Version;
        void*               m_SyncRoot;
    };

    template <typename TKey, typename TValue>
    struct Dictionary : Object
    {
        struct Entry
        {
            int m_HashCode;
            int m_Next;
            TKey m_Key;
            TValue m_Value;
        };

        const Entry*    begin() const { return m_Entries->begin(); }
        Entry*          begin() { return m_Entries->begin(); }
        const Entry*    end() const { return m_Entries->begin() + m_Count; }
        Entry*          end() { return m_Entries->begin() + m_Count; }

        TValue* GetItem(const TKey& key) 
        {  
            for (int i = 0; i < m_Count; ++i)
                if (m_Entries->operator[](i).m_Key == key)
                    return &m_Entries->operator[](i).m_Value;
            return nullptr;
        }

        Array<int>* m_Buckets;
        Array<Entry>* m_Entries;
        void* m_Comparer;
        void* m_Keys;
        void* m_Values;
        void* m_SyncRoot;
        int m_Count;
        /* ... */
    };

    struct Method
    {
        void*               GetPointer();
        Object*             Invoke(void* __this, void** args, Exception** ex);

        inline void*        GetThunk()                      { return HaxSdk::IsMono() ? m_Mono.GetThunk() : nullptr; }
        const char*         GetName() { return HaxSdk::IsMono() ? m_Mono.GetName() : m_Il2Cpp.GetName(); }
        Class*              GetClass() { return HaxSdk::IsMono() ? (Class*)m_Mono.GetClass() : (Class*)m_Il2Cpp.GetClass(); }

        union
        {
            MonoMethod      m_Mono;
            Il2CppMethod    m_Il2Cpp;
        };
    };

    struct ReflectionType : Object
    {
        explicit            ReflectionType(Type* type);
        explicit            ReflectionType() : Object(nullptr), m_Type(nullptr) {}

        inline Type*        GetType()                       { return m_Type; }

        Type*               m_Type;
    };

    struct String : Object
    {
        inline wchar_t&         operator[](int i)           { return m_Chars[i]; }
        inline const wchar_t&   operator[](int i) const     { return m_Chars[i]; }

        inline wchar_t*         begin()                     { return m_Chars; }
        inline const wchar_t*   begin() const               { return m_Chars; }
        inline wchar_t*         end()                       { return m_Chars + m_Length; }
        inline const wchar_t*   end() const                 { return m_Chars + m_Length; }

        static String*          New(const char* str);

        inline wchar_t*         GetRawStringData()          { return m_Chars; }
        inline int              GetLength()                 { return m_Length; }

    private:
        int                     m_Length;
        wchar_t                 m_Chars[1];
    };

    struct Thread
    {
        static Thread*          Attach();
        void                    Detach();

        union
        {
            MonoThread          m_Mono;
            Il2CppThread        m_Il2Cpp;
        };
    };

    struct Type
    {
        ReflectionType*         GetReflectionType();
        ReflectionType*         CreateReflectionType();
        Class*                  GetClass();

        union
        {
            MonoType            m_Mono;
            Il2CppType          m_Il2Cpp;
        };
    };

    struct VTable
    {
        Class*                  GetClass();

        union
        {
            MonoVTable          m_Mono;
            Il2CppVTable        m_Il2Cpp;
        };
    };

    namespace Unity
    {
        struct Object : unsafe::Object
        {
            void* m_CachedPtr;
        };
    }
}