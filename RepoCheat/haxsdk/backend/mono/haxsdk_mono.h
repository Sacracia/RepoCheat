// Mono module (haxsdk_mono.h)
// - This header provides low-level definitions of Mono runtime structures used internally by Unity's scripting engine
//   (for games using the Mono backend, not IL2CPP).
// - The goal is to expose accurate C++ representations of key Mono types (MonoObject, MonoClass, MonoType, MonoMethod, etc.),
//   so they can be accessed directly from native code when building tools, debuggers, or runtime modifications.
//
// - The structures declared in this file mirror those used internally by the Mono runtime, based on publicly available
//   source code from the Mono project (https://github.com/Unity-Technologies/mono) and export functions of mono module.
//
// Version Variability
// - Mono is not a stable binary API. Different versions of Unity bundle different builds of Mono,
//   and these versions may change internal structure layouts or omit/export certain functions entirely.
// - For example, the layout of MonoClassField can differ across Unity 2018, 2019, 2020, etc.
// - Similarly, mono.dll may expose different symbols.
//
// - Therefore, you should NEVER assume a single set of Mono headers will work universally across games.
//   Instead, the SDK provides version-specific struct definitions in the `headers/` folder.
//
// Using Correct Struct Definitions
// - In the `headers/` directory, we include multiple versions of Mono structure definitions
//   adapted from official Mono source
// - To select the correct version, edit the `haxsdk_properties.h` file and define the appropriate version macro.
//   This will include the matching headers and ensure field layouts align with the game you're targeting.
//
// - If you're working with a new or custom Mono version, you can generate updated struct definitions yourself.
//   The `headers/tools/` folder includes a generator that can parse Mono source code and output updated C++ bindings.
//
// =======================================================================================
// About the Structures
// - All types in this file are declared as plain C++ structs to match Mono's C ABI.
// - Most are opaque to the user, but some contain fields that can be safely accessed (e.g., MonoMethod::name).
// - The most important types include:
//
//     struct MonoObject         - Base class for all managed objects
//     struct MonoClass          - Type metadata and layout information
//     struct MonoType           - Type descriptors, including generics, arrays, value types, etc.
//     struct MonoMethod         - Managed method, can be invoked from native code
//     struct MonoField          - Field metadata, including offset, name, and access flags
//     struct MonoException      - Managed exception object, can be constructed manually
//     struct MonoDomain         - Represents a runtime domain (AppDomain in .NET)
//     struct MonoGCHandle       - Used to pin managed objects and prevent GC from moving/collecting them
//
// - Most structs are only partially defined — only the fields needed by the SDK are declared.
//   Internal Mono fields that are unused or variable across versions are left as `/* ... */`.
//

#pragma once

#undef GetMessage

#include <cstdint>

/* Field attributes from Mono */
#define FIELD_ATTRIBUTE_FIELD_ACCESS_MASK     0x0007
#define FIELD_ATTRIBUTE_COMPILER_CONTROLLED   0x0000
#define FIELD_ATTRIBUTE_PRIVATE               0x0001
#define FIELD_ATTRIBUTE_FAM_AND_ASSEM         0x0002
#define FIELD_ATTRIBUTE_ASSEMBLY              0x0003
#define FIELD_ATTRIBUTE_FAMILY                0x0004
#define FIELD_ATTRIBUTE_FAM_OR_ASSEM          0x0005
#define FIELD_ATTRIBUTE_PUBLIC                0x0006

#define FIELD_ATTRIBUTE_STATIC                0x0010
#define FIELD_ATTRIBUTE_INIT_ONLY             0x0020
#define FIELD_ATTRIBUTE_LITERAL               0x0040
#define FIELD_ATTRIBUTE_NOT_SERIALIZED        0x0080
#define FIELD_ATTRIBUTE_SPECIAL_NAME          0x0200
#define FIELD_ATTRIBUTE_PINVOKE_IMPL          0x2000

/* For runtime use only */
#define FIELD_ATTRIBUTE_RESERVED_MASK         0x9500
#define FIELD_ATTRIBUTE_RT_SPECIAL_NAME       0x0400
#define FIELD_ATTRIBUTE_HAS_FIELD_MARSHAL     0x1000
#define FIELD_ATTRIBUTE_HAS_DEFAULT           0x8000
#define FIELD_ATTRIBUTE_HAS_FIELD_RVA         0x0100

/* Forward declaration of structures */
struct MonoAssembly;
struct MonoAssemblyName;
struct MonoClass;
struct MonoDomain;
struct MonoType;
struct MonoImage;
struct MonoMethod;
struct MonoObject;
struct MonoString;
struct MonoThread;
struct MonoVTable;
struct MonoMethodSignature;
struct MonoException;
struct MonoThreadsSync;
struct MonoAppDomainSetup;
struct MonoAppContext;
struct MonoGHashTable;
struct MonoAppDomain;
struct MonoReflectionType;
struct MonoArrayType;
struct MonoGenericParam;
struct MonoGenericClass;
struct MonoExceptionWrapper;
struct MonoArrayBounds;
struct MonoArray;
struct MonoField;

// Module API
namespace mono
{
    void Initialize();
}

enum MonoTypeEnum : uint8_t {
	MONO_TYPE_END        = 0x00,       /* End of List */
	MONO_TYPE_VOID       = 0x01,
	MONO_TYPE_BOOLEAN    = 0x02,
	MONO_TYPE_CHAR       = 0x03,
	MONO_TYPE_I1         = 0x04,
	MONO_TYPE_U1         = 0x05,
	MONO_TYPE_I2         = 0x06,
	MONO_TYPE_U2         = 0x07,
	MONO_TYPE_I4         = 0x08,
	MONO_TYPE_U4         = 0x09,
	MONO_TYPE_I8         = 0x0a,
	MONO_TYPE_U8         = 0x0b,
	MONO_TYPE_R4         = 0x0c,
	MONO_TYPE_R8         = 0x0d,
	MONO_TYPE_STRING     = 0x0e,
	MONO_TYPE_PTR        = 0x0f,       /* arg: <type> token */
	MONO_TYPE_BYREF      = 0x10,       /* arg: <type> token */
	MONO_TYPE_VALUETYPE  = 0x11,       /* arg: <type> token */
	MONO_TYPE_CLASS      = 0x12,       /* arg: <type> token */
	MONO_TYPE_VAR	     = 0x13,	   /* number */
	MONO_TYPE_ARRAY      = 0x14,       /* type, rank, boundsCount, bound1, loCount, lo1 */
	MONO_TYPE_GENERICINST= 0x15,	   /* <type> <type-arg-count> <type-1> \x{2026} <type-n> */
	MONO_TYPE_TYPEDBYREF = 0x16,
	MONO_TYPE_I          = 0x18,
	MONO_TYPE_U          = 0x19,
	MONO_TYPE_FNPTR      = 0x1b,	      /* arg: full method signature */
	MONO_TYPE_OBJECT     = 0x1c,
	MONO_TYPE_SZARRAY    = 0x1d,       /* 0-based one-dim-array */
	MONO_TYPE_MVAR	     = 0x1e,       /* number */
	MONO_TYPE_CMOD_REQD  = 0x1f,       /* arg: typedef or typeref token */
	MONO_TYPE_CMOD_OPT   = 0x20,       /* optional arg: typedef or typref token */
	MONO_TYPE_INTERNAL   = 0x21,       /* CLR internal type */

	MONO_TYPE_MODIFIER   = 0x40,       /* Or with the following types */
	MONO_TYPE_SENTINEL   = 0x41,       /* Sentinel for varargs method signature */
	MONO_TYPE_PINNED     = 0x45,       /* Local var that points to pinned object */

	MONO_TYPE_ENUM       = 0x55        /* an enumeration */
};

struct MonoVTable
{
    MonoClass* klass;

    /* ... */
};

struct MonoObject
{
    static MonoObject* Box(MonoClass* klass, void* data);
    static MonoObject* New(MonoClass* klass);

    void Ctor();
    inline MonoClass* GetClass() { return vtable->klass; }

    MonoVTable* vtable;
    MonoThreadsSync* synchronisation;
};

struct MonoException : MonoObject
{
    inline MonoString*& GetMessage() { return message; }
    inline MonoString* GetStackTrace() { return stack_trace; }

    static MonoException* GetNullReference();
    static MonoException* GetArgumentOutOfRange();

    MonoString* class_name;
    MonoString* message;
    MonoObject* _data;
    MonoObject* inner_ex;
    MonoString* help_link;
    /* Stores the IPs and the generic sharing infos
       (vtable/MRGCTX) of the frames. */
    MonoArray** trace_ips;
    MonoString* stack_trace;
    MonoString* remote_stack_trace;
    int32_t	    remote_stack_index;
    /* Dynamic methods referenced by the stack trace */
    MonoObject* dynamic_methods;
    int32_t	    hresult;
    MonoString* source;
    MonoObject* serialization_manager;
    MonoObject* captured_traces;
    MonoArray** native_trace_ips;
    int32_t caught_in_unmanaged;
};

struct MonoDomain
{
    static MonoDomain* GetRoot();
    MonoAssembly* GetAssembly(const char* name);
};

struct MonoThread
{
    static MonoThread* Attach();
    void Detach();
};

struct MonoAssembly
{
    MonoImage* GetImage();
};

struct MonoImage
{
    static MonoImage* GetCorlib();
    static MonoImage* GetUnityCore();
    MonoClass* GetClass(const char* nameSpace, const char* name);
};

struct MonoClass
{
    MonoVTable* GetVTable();
    MonoField* GetField(const char* name);
    MonoMethod* GetMethod(const char* name, const char* signature);
    void* GetStaticFieldData();
    MonoType* GetType();
    const char* GetName();
};

struct MonoMethod
{
    void* GetPointer();
    void* GetThunk();
    MonoObject* Invoke(void* __this, void** args, MonoException** ex);
    const char* GetName() { return name; }
    MonoClass* GetClass() { return klass; }

    uint16_t flags;
    uint16_t iflags;
    uint32_t token;
    MonoClass* klass;
    MonoMethodSignature* signature;
    const char* name;
    /* ... */
};

struct MonoType
{
    MonoReflectionType* GetReflectionType();
    MonoClass* GetClass();

    union {
        MonoClass* klass; /* for VALUETYPE and CLASS */
        MonoType* type;   /* for PTR */
        MonoArrayType* array; /* for ARRAY */
        MonoMethodSignature* method;
        MonoGenericParam* generic_param; /* for VAR and MVAR */
        MonoGenericClass* generic_class; /* for GENERICINST */
    } data;
    uint16_t attrs; /* param attributes or field flags */
    MonoTypeEnum type;

    /* ... */
};

struct MonoField
{
    inline MonoClass* GetParent() { return parent; }
    inline int GetOffset() { return offset; }
    inline bool IsStatic() { return type->attrs & FIELD_ATTRIBUTE_STATIC; }
    inline const char* GetName() { return name; }
    inline bool IsLiteral() { return type->attrs & FIELD_ATTRIBUTE_LITERAL; }

    void GetStaticValue(void* value);

    void* GetValuePtr(void* __this);

    MonoType* type;
    const char* name;
    MonoClass* parent;
    int offset;
};

struct MonoReflectionType : MonoObject
{
    MonoType* type;
};

struct MonoString
{
    static MonoString* New(const char* str);
};

struct MonoGCHandle
{
    static uint32_t New(MonoObject* obj, bool pinned);
    static uint32_t NewWeak(MonoObject* obj, bool trackResurrection);
    static MonoObject* GetTarget(uint32_t handle);
    static void Free(uint32_t handle);
};