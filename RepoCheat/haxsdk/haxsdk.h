#pragma once

// [SECTION] Headers
// [SECTION] Forward declarations and basic types
// [SECTION] Dear ImGui end-user API functions

#include <cstdint>
#include <string_view>
#include <type_traits>

#ifndef HAX_ASSERT
#include <assert.h>
#define HAX_ASSERT(_EXPR, _TEXT) if (!_EXPR) \
                                    _wassert(_CRT_WIDE(_TEXT), L"?", (unsigned)(__LINE__))
#endif

#ifndef OUT
#define OUT
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations and basic types
//-----------------------------------------------------------------------------
// 
// .NET Framework basic system types
typedef bool Boolean;
typedef int8_t Byte;
typedef uint8_t SByte;
typedef float Single;
typedef int16_t Int16;
typedef uint16_t UInt16;
typedef int32_t Int32;
typedef uint32_t UInt32;
typedef int64_t Int64;
typedef uint64_t UInt64;
typedef wchar_t Char;
typedef double Double;

// HaxSdk custom classes and enums
enum HaxBackend;
struct HaxGlobals;
struct HaxLogger;

// Mono / IL2CPP basic classes
struct Assembly;
struct Class;
struct Domain;
struct Field;
struct Image;
struct Method;
struct Thread;
struct Type;

// C# basic system classes
namespace System {
    template <class T> struct Array;
    template <class TKey, class TValue> struct Dictionary;
    struct Enum;
    template <class T> struct List;
    struct Object;
    struct String;
    struct Type;
}

//-----------------------------------------------------------------------------
// [SECTION] HaxSdk end-user API functions
//-----------------------------------------------------------------------------

enum HaxBackend {
    HaxBackend_None = 0,
    HaxBackend_Mono = 1 << 0,
    HaxBackend_IL2CPP = 1 << 1
};

struct HaxGlobals {
    void Save();
    void Load();

    HaxBackend backend;
    void* backendHandle;
    bool visible = true;
    int hotkey = 0xC0;
    int locale = 0;
    void* cheatModule;
    void* gameHWND;
    void* oWndProc;
    bool shouldExit;
    float spawnColor[4] = { 0.357f, 0.427f, 0.969f, 1.f };
    float extractionColor[4] = { 0.357f, 0.427f, 0.969f, 1.f };
};

namespace HaxSdk {
    HaxGlobals& GetGlobals();
    void DetourAttach(void** ppPointer, void* pDetour);
    void DetachAllHooks();
    void InitLogger(bool useConsole);
    void Log(std::string_view text);
    void InitializeCore();
    Thread* UnityAttachThread();
}

struct Assembly {
    Assembly() = delete;
    Assembly(const Assembly&) = delete;

    static Assembly* Find(const char* name);
    static bool Exists(const char* name, OUT Assembly*& pRes);

    Image* GetImage();
};

struct Type {
    struct Il2CppType {
        void* data;
        unsigned int            attrs : 16;
        int                     type : 8;
        unsigned int            num_mods : 5;
        unsigned int            byref : 1;
        unsigned int            pinned : 1;
        unsigned int            valuetype : 1;
    };

    struct MonoType {
    };

    Type() = delete;
    Type(const Type&) = delete;

    System::Type* GetSystemType();
private:
    union {
        Il2CppType il2cpp;
        MonoType mono;
    };
};

struct Class {
    Class() = delete;
    Class(const Class&) = delete;

    static bool Exists(const char* assembly, const char* nameSpace, const char* name, OUT Class*& pClass);
    static Class* Find(const char* assembly, const char* nameSpace, const char* name);

    Method* FindMethod(const char* name, const char* signature = nullptr);
    Field* FindField(const char* name);
    void* FindStaticField(const char* name);
    void* GetStaticFieldsData();
    const char* GetName();
    System::Type* GetSystemType();
};

struct Domain {
    Domain() = delete;
    Domain(const Domain&) = delete;

    static Domain* Root();
    static Domain* Current();

    Thread* AttachThread();
};

struct Field {
    Field() = delete;
    Field(const Field&) = delete;

    int32_t Offset();
    void GetStaticValue(void* pValue);
    void SetStaticValue(void* pValue);
};

struct Method {
    Method() = delete;
    Method(const Method&) = delete;

    System::Object* Invoke(void* __this, void** ppArgs);
    void* GetAddress();
};

template <typename T>
struct HaxMethod {
    HaxMethod() : ptr(nullptr), orig(nullptr), pBase(nullptr) {}
    HaxMethod(Method* pMethod) { ptr = orig = (T)pMethod->GetAddress(); pBase = pMethod; }

    System::Object* Invoke(void* __this, void** args)       { return pBase->Invoke(__this, args); }

    Method* pBase;
    T ptr;
    T orig;
};

struct Thread {
    Thread() = delete;
    Thread(const Thread&) = delete;

    void Detach();
};

struct System::Object {
    static Object*      New(Class* pClass);

    Object*             Ctor();
    Class*              GetClass()                          { return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *ppClass : pClass; }
    void*               Unbox();

    Int32               GetHashCode();
    String*             ToString();

    union {
        Class* pClass;
        Class** ppClass;
    };
    void* monitor;
};

// Represents .NET Framework System.Array
// May be unsafe when T is not a pointer, but generally it is.
template <class T> 
struct System::Array : System::Object {
    System::Array<T>() = delete;
    System::Array<T>(const System::Array<T>& src) = delete;

    inline T*           begin()                             { return &vector[0]; }
    inline const T*     begin() const                       { return &vector[0]; }
    inline T*           end()                               { return &vector[length]; }
    inline const T*     end() const                         { return &vector[length]; }

    inline T&           operator[](size_t i)                { return vector[i]; }
    inline const T&     operator[](size_t i) const          { return vector[i]; }
    
    static Class*       GetClass();
    static void         Clear(System::Array<T>* pArray, Int32 index, Int32 length);
    static void         Sort(System::Array<T>* pArray, Int32 index, Int32 length);

    void* pBounds;
    size_t length;
    T vector[1];
};

template <class TKey, class TValue>
struct Entry {
    UInt32          hashCode;
    Int32           next;
    TKey            key;
    TValue          value;
};

template <class TKey, class TValue>
struct System::Dictionary : System::Object {

    TValue              GetItem(TKey key);
    bool                ContainsKey(TKey key);
    Int32               Count();

    Array<Entry<TKey, TValue>>* GetEntries();
};

template<class TKey, class TValue>
TValue System::Dictionary<TKey, TValue>::GetItem(TKey key) {
    static Method* pFunc = this->GetClass()->FindMethod("get_Item");
    void* args[1] = {0};
    if constexpr (std::is_pointer_v<TKey>) {
        args[0] = key;
    }
    else {
        args[0] = &key;
    }
    System::Object* res = pFunc->Invoke(this, args);
    if constexpr (std::is_pointer_v<TValue>) {
        return static_cast<TValue>(res);
    }
    else {
        return *static_cast<TValue*>(res->Unbox());
    }
}

template<class TKey, class TValue>
bool System::Dictionary<TKey, TValue>::ContainsKey(TKey key) {
    static Method* pFunc = this->GetClass()->FindMethod("ContainsKey");
    void* args[1] = {0};
    if constexpr (std::is_pointer_v<TKey>) {
        args[0] = key;
    }
    else {
        args[0] = &key;
    }
    return *(bool*)pFunc->Invoke(this, args)->Unbox();
}

template<class TKey, class TValue>
Int32 System::Dictionary<TKey, TValue>::Count() {
    static Method* pFunc = this->GetClass()->FindMethod("get_Count");
    return *(Int32*)pFunc->Invoke(this, nullptr)->Unbox();
}

template<class TKey, class TValue>
inline System::Array<Entry<TKey, TValue>>* System::Dictionary<TKey, TValue>::GetEntries() {
    static int32_t offset = this->GetClass()->FindField("_entries")->Offset();
    return *(System::Array<Entry<TKey, TValue>>**)((char*)this + offset);
}

// Represents .NET Framework System.Enum
struct System::Enum : System::Object {
    System::Enum() = delete;
    System::Enum(const System::Enum&) = delete;

    static Class* GetClass();
    static Array<String*>* GetNames(System::Type* pType);
    static Array<Int32>* GetValues(System::Type* pType);
    static String* GetName(System::Type* pType, System::Enum* pObj);

    Int32 value__;
};

template <class T> 
struct System::List : System::Object {
    System::List<T>() = delete;

    inline T&           operator[](size_t i)                { return pItems->operator[](i); }
    inline const T&     operator[](size_t i) const          { return pItems->operator[](i); }

    inline T*           begin()                             { return pItems->begin(); }
    inline const T*     begin() const                       { return pItems->begin(); }
    inline T*           end()                               { return &pItems->vector[length]; }
    inline const T*     end() const                         { return &pItems->vector[length]; }

    bool                Contains(T val);
    void                Add(T val);

    Array<T>*           pItems;
    Int32               length;
    Int32               version;
    void*               pSyncRoot;
};

template<class T>
inline bool System::List<T>::Contains(T val) {
    static Method* pFunc = this->GetClass()->FindMethod("Contains");
    void* args[1] = {0};
    if constexpr (std::is_pointer_v<T>) {
        args[0] = val;
    }
    else {
        args[0] = &val;
    }
    return *(bool*)pFunc->Invoke(this, args)->Unbox();
}

template<class T>
inline void System::List<T>::Add(T val) {
    static Method* pFunc = this->GetClass()->FindMethod("Add");
    void* args[1] = {0};
    if constexpr (std::is_pointer_v<T>) {
        args[0] = val;
    }
    else {
        args[0] = &val;
    }
    pFunc->Invoke(this, args);
}

struct System::String : System::Object {
    System::String() = delete;
    System::String(const System::String&) = delete;

    static Class* GetClass();

    static System::String* New(const char* data);
    static System::String* Concat(System::String* s1, System::String* s2);

    System::String* operator+(System::String* s) { return System::String::Concat(this, s); }

    Int32 CompareTo(System::String* value);

    wchar_t* Data()   { return chars; }
    Int32 Length()    { return length; }
    char* UTF8();

    Int32 length;
    Char chars[1];
};

struct System::Type : System::Object {
    System::Type() = delete;
    System::Type(const System::Type&) = delete;

    ::Type* pType;
};

