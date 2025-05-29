
/*

[Index of this file]
|
|- [SECTION] Headers
|- [SECTION] HaxSdk API
|
|- [SECTION] UnityEngine.CoreModule
|    |
|    |- [ENUMS]
│    |    |- HideFlags
|    |
|    |- [STRUCTS]
│    |    |- Vector2
│    |    |- Vector3
│    |    |- Vector4
│    |    |- Quaternion
│    |    |- Bounds
│    |    |- Color
│    |    |- Matrix4x4
│    |    |- Rect
|    |
|    |- [CLASSES]
│         |- AsyncOperation
│         |- Object
│         |- Component
│         |- Transform
│         |- Behaviour
│         |- MonoBehaviour
│         |- GameObject
│         |- Camera
│         |- Light
│         |- Shader
│         |- Material
│         |- Renderer
│         |- MeshRenderer
│         |- LineRenderer
│         |- Screen
│         |- Sprite
│         |- RenderSettings
|
|- [SECTION] UnityEngine.PhysicsModule
|    |
|    |- [CLASSES]
│         |- Collider2D
│         |- Collider
│         |- BoxCollider
|
|- [SECTION] UnityEngine.UI
|    |
|    |- [CLASSES]
│         |- EventSystem
|
|- [SECTION] PhotonUnityNetworking
|    |
|    |- [CLASSES]
│         |- Player
│         |- PhotonView
│         |- PhotonNetwork
|
|- [SECTION] TypeOfs

*/

// Enumerations:
// - Unity enums are typically represented as signed 32-bit integers (int32_t).
// - Enums can be passed directly as integers when calling managed methods.

// Structures and their boxed classes
// - Unity structs (e.g., Vector3, Quaternion, Bounds) are value types in C#.
// - In Mono, value types must be boxed (wrapped into a System.Object) to be passed into managed methods expecting an object.
// - In IL2CPP, structs are passed directly by value and boxing is generally not required.
// - HaxSdk provides lightweight boxed wrappers (e.g., Vector3_Boxed) that can be stack-allocated and passed to Mono thunks.

// Classes (managed reference types)
// - All C# classes (e.g., Object, GameObject, Transform) are represented in this SDK as lightweight wrappers around a raw MonoObject*.
// - These wrappers do not manage lifetime or reference count — they merely provide typed access to the managed object.
// - Constructors in these wrappers DO NOT allocate objects on the managed heap (unlike `new` in C#), they only wrap existing pointers.
// - To create new managed objects (e.g., new GameObject()), use explicitly provided static methods like `GameObject::New()`.
// ================== BE CAREFUL ==================
// - Wrapping a pointer does not protect the managed object from garbage collection. Once collected, the pointer becomes invalid.
// - Attempting to use a wrapper around a collected object leads to crashes (invalid memory access).
// - Tip: To pin the object and prevent it from being collected, use `System::GCHandle` (see `GCHandle` docs in haxsdk_system.h).
// ================================================
// - All wrappers implement `Null()` checks and perform validation before accessing underlying data.
// - If a wrapper detects invalid memory (e.g., null pointer, invalid VTable), it throws a managed `System::Exception`.
//   These exceptions can be caught (see `System::Exception`), and the main ImGui render path is already wrapped in a try/catch block to prevent crashes during UI drawing.

//-----------------------------------------------------------------------------
// [SECTION] Headers
//-----------------------------------------------------------------------------

#pragma once

#define EXCLUDE_NULL
#include "haxsdk_system.h"

//-----------------------------------------------------------------------------
// [SECTION] HaxSdk API
//-----------------------------------------------------------------------------

namespace HaxSdk
{
    // Initializes unity module
    // Must be called once before using this module
    void InitUnity();
}

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

namespace Unity
{
    enum HideFlags : int;

    class GameObject;
    class Transform;
    class Vector3_Boxed;
    class Quaternion_Boxed;
    class Bounds_Boxed;
}

//-----------------------------------------------------------------------------
// [SECTION] UnityEngine.CoreModule
//-----------------------------------------------------------------------------

namespace Unity
{
    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/HideFlags.html
    enum HideFlags : int
    {
        None                    = 0,
        HideInHierarchy         = 1,
        HideInInspector         = 2,
        DontSaveInEditor        = 4,
        NotEditable             = 8,
        DontSaveInBuild         = 16,
        DontUnloadUnusedAsset   = 32,
        DontSave                = 52,
        HideAndDontSave         = 61
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Vector2.html
    struct Vector2
    {
        // Members
        float x;
        float y;
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Vector3.html
    struct Vector3
    {
        // Constructors
        explicit                        Vector3() : x(0.f), y(0.f), z(0.f) {}
        explicit                        Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
        explicit                        Vector3(float x_, float y_) : x(x_), y(y_), z(0.f) {}

        // Operators
        inline Vector3                  operator+(const Unity::Vector3& a) const { return Vector3(x + a.x, y + a.y, z + a.z); }
        inline Vector3                  operator-(const Unity::Vector3& a) const { return Vector3(x - a.x, y - a.y, z - a.z); }
        inline Vector3                  operator*(float mult) const { return Vector3(x * mult, y * mult, z * mult); }

        // Static Properties
        static inline Vector3           zero() { return Vector3(0, 0, 0); }
        static inline Vector3           one() { return Vector3(1, 1, 1); }
        static inline Vector3           up() { return Vector3(0, 1, 0); }
        static inline Vector3           down() { return Vector3(0, -1, 0); }
        static inline Vector3           left() { return Vector3(-1, 0, 0); }
        static inline Vector3           right() { return Vector3(1, 0, 0); }
        static inline Vector3           forward() { return Vector3(0, 0, 1); }
        static inline Vector3           back() { return Vector3(0, 0, -1); }

        // Static Methods
        static float                    Distance(const Unity::Vector3& a, const Unity::Vector3& b);
        static Vector3                  Min(const Vector3& a, const Vector3& b);
        static Vector3                  Max(const Vector3& a, const Vector3& b);

        // HaxSdk Extension
        float                           Distance(const Unity::Vector3& other);
        inline Vector2                  ToVector2() { return Unity::Vector2(this->x, this->y); }
        Vector3_Boxed                   Box() const;

        // Members
        float                           x;
        float                           y;
        float                           z;
    };

    class Vector3_Boxed : unsafe::Object
    {
    public:
        // Constructors
        explicit                        Vector3_Boxed(float x, float y, float z);
        explicit                        Vector3_Boxed(const Vector3& v);

        // Members
        Vector3                         m_Value;
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Vector4.html
    struct Vector4
    {
        // Members
        float                           x;
        float                           y;
        float                           z;
        float                           w;
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Quaternion.html
    struct Quaternion
    {
        // Constructors
        explicit                        Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

        // Static Properties
        static inline Unity::Quaternion identity() { return Quaternion(0.f, 0.f, 0.f, 1.f); }

        // HaxSdk Extension
        Quaternion_Boxed                Box() const;

        // Members
        float                           x;
        float                           y;
        float                           z;
        float                           w;
    };

    class Quaternion_Boxed : unsafe::Object
    {
    public:
        // Constructors
        explicit                        Quaternion_Boxed(float x, float y, float z, float w);
        explicit                        Quaternion_Boxed(const Quaternion& v);

        // Members
        Quaternion                      m_Value;
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Bounds.html
    struct Bounds
    {
        // Constructors
        explicit                        Bounds(const Vector3& center, const Vector3& size) : m_Center(center), m_Extents(size * 0.5f) {}

        // Properties
        inline Vector3                  GetSize() const { return m_Extents * 2.f; }
        inline Vector3                  SetSize(const Vector3& size) { m_Extents = size * 0.5f; }
        inline Vector3                  GetMin() { return m_Center - m_Extents; }
        inline Vector3                  GetMax() { return m_Center + m_Extents; }

        // Public Methods
        inline void                     SetMinMax(const Vector3& min, const Vector3& max) { m_Extents = (max - min) * 0.5f; m_Center = min + m_Extents; }
        inline void                     Encapsulate(const Vector3& point) { SetMinMax(Vector3::Min(GetMin(), point), Vector3::Max(GetMax(), point)); }
        inline void                     Encapsulate(const Bounds& bounds) { Encapsulate(bounds.m_Center - bounds.m_Extents); Encapsulate(bounds.m_Center + bounds.m_Extents); }

        // HaxSdk Extension
        Bounds_Boxed                    Box() const;

        // Members
        Vector3                         m_Center;
        Vector3                         m_Extents;
    };

    class Bounds_Boxed : unsafe::Object
    {
    public:
        // Constructors
        explicit                        Bounds_Boxed(const Bounds& b);

        // Members
        Bounds                          m_Value;
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Color.html
    struct Color
    {
        // Constructors
        explicit                        Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}

        // Static Properties
        static inline Color             red() { return Color(1.f, 0.f, 0.f, 1.f); }
        static inline Color             green() { return Color(0.f, 1.f, 0.f, 1.f); }
        static inline Color             blue() { return Color(0.f, 0.f, 1.f, 1.f); }
        static inline Color             white() { return Color(1.f, 1.f, 1.f, 1.f); }
        static inline Color             black() { return Color(0.f, 0.f, 0.f, 1.f); }
        static inline Color             yellow() { return Color(1.f, 0.92156863f, 0.015686275f, 1.f); }
        static inline Color             cyan() { return Color(0.f, 1.f, 1.f, 1.f); }
        static inline Color             magenta() { return Color(1.f, 0.f, 1.f, 1.f); }
        static inline Color             gray() { return Color(0.5f, 0.5f, 0.5f, 0.5f); }
        static inline Color             grey() { return Color(0.5f, 0.5f, 0.5f, 0.5f); }
        static inline Color             clear() { return Color(0.f, 0.f, 0.f, 0.f); }

        // Members
        float                           r;
        float                           g;
        float                           b;
        float                           a;
    };

    class Color_Boxed : unsafe::Object
    {
    public:
        // Constructors
        explicit                        Color_Boxed(const Color& c);

        // Members
        Color                           m_Value;
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Matrix4x4.html
    struct Matrix4x4
    {
        // Operators
        Vector4                         operator*(const Vector4& v) const;

        // Public Methods
        Vector3                         MultiplyPoint(Vector3& point);

        // Members
        float                           m00, m10, m20, m30;
        float                           m01, m11, m21, m31;
        float                           m02, m12, m22, m32;
        float                           m03, m13, m23, m33;
    };

    class Matrix4x4_Boxed : unsafe::Object
    {
    public:
        // Constructors
        Matrix4x4_Boxed(const Matrix4x4& m);
        // Members
        Matrix4x4                       m_Value;
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Rect.html
    struct Rect
    {
        // Constructors
        explicit                        Rect(float _x, float _y, float _width, float _height) : x(_x), y(_y), width(_width), height(_height) {}
        explicit                        Rect(const Vector2& pos, const Vector2& size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}
        explicit                        Rect(const Rect& source) = default;

        // Properties
        inline Vector2                  GetCenter() { return Vector2(x + width / 2.f, y + height / 2.f); }

        // Static Methods
        static inline Rect              MinMaxRect(float xmin, float ymin, float xmax, float ymax) { return Rect(xmin, ymin, xmax - xmin, ymax - ymin); }

        // Members
        float                           x;
        float                           y;
        float                           width;
        float                           height;
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/AsyncOperation.html
    class AsyncOperation : public System::Object
    {
    public:
        // Constructors
        explicit                        AsyncOperation(unsafe::Object* ptr) : System::Object(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        bool                            GetIsDone();
        float                           GetProgress();
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Object.html
    class Object : public System::Object
    {
    public:
        // Constructors
        explicit constexpr              Object(unsafe::Object* ptr) : System::Object(ptr) {}
        Object(const Object& o) = default;

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        System::String                  GetName();
        void                            SetName(System::String name);
        void                            SetHideFlags(HideFlags flags);

        // Static Methods
        static System::Array<Object>    FindObjectsOfType(System::Type type);
        static Object                   FindObjectOfType(System::Type type);
        static void                     Destroy(Object obj, float t = 0.f);

        // Public Methods
        static Object                   Instantiate(Object original);
        static Object                   Instantiate(Object original, const Vector3& position, const Quaternion& rotation);
        template <class T> static T     Instantiate(T original) { static_assert(std::is_base_of_v<Object, T>); return T(Object::Instantiate(Object(original.GetPointer())).m_Pointer); }
        template <class T> static T     Instantiate(T original, const Vector3& position, const Quaternion& rotation) { static_assert(std::is_base_of_v<Object, T>); return T(Object::Instantiate(Object(original.GetPointer()), position, rotation).m_Pointer); }

        // HaxSdk Extension
        inline bool                     Null() const { return !m_Pointer || !GetPointer()->m_CachedPtr; }
        inline unsafe::Unity::Object*   GetPointer() const { return (unsafe::Unity::Object*)m_Pointer; }
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Component.html
    class Component : public Object
    {
    public:
        // Constructors
        explicit                        Component(unsafe::Object* ptr) : Object(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        Transform                       GetTransform();
        GameObject                      GetGameObject();

        // Public Methods
        Component                       GetComponentInChildren(System::Type);
        template <typename T> T         GetComponentInChildren() { static_assert(std::is_base_of_v<Component, T>); return T(GetComponentInChildren(typeof<T>).m_Pointer); }
        System::Array<Component>        GetComponentsInChildren(System::Type);
        template <typename T> System::Array<T> GetComponentsInChildren() { static_assert(std::is_base_of_v<Component, T>); return System::Array<T>(GetComponentsInChildren(typeof<T>).GetPointer()); }
        Component                       GetComponent(System::Type);
        template <typename T> T         GetComponent() { static_assert(std::is_base_of_v<Component, T>); return T(GetComponent(typeof<T>).GetPointer()); }
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Transform.html
    class Transform : public Component
    {
    public:
        // Constructors
        explicit                        Transform(unsafe::Object* ptr) : Component(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        Vector3                         GetPosition();
        void                            SetPosition(const Unity::Vector3& value);
        Transform                       GetParent();
        void                            SetParent(const Unity::Transform& value);
        Vector3                         GetForward();
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Behaviour.html
    class Behaviour : public Component
    {
    public:
        // Constructors
        explicit                        Behaviour(unsafe::Object* ptr) : Component(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        bool                            GetEnabled();
        void                            SetEnabled(bool value);
        bool                            GetIsActiveAndEnabled();
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/MonoBehaviour.html
    class MonoBehaviour : public Behaviour
    {
    public:
        // Constructors
        explicit                        MonoBehaviour(unsafe::Object* ptr) : Behaviour(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/GameObject.html
    class GameObject : public Object
    {
    public:
        // Constructors
        explicit                        GameObject(unsafe::Object* ptr) : Object(ptr) {}
        static GameObject               New();
        static GameObject               New(const char* name);

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        Transform                       GetTransform();
        void                            SetLayer(int value);
        bool                            GetActiveSelf();
        void                            SetActive(bool value);
        bool                            GetActive();
        bool                            GetActiveInHierarchy();

        // Public Methods
        Component                       GetComponent(System::Type type);
        template <class T> T            GetComponent() { static_assert(std::is_base_of_v<Component, T>); return T(GetComponent(typeof<T>).GetPointer()); }
        Component                       AddComponent(System::Type componentType);
        template <typename T> T         AddComponent() { static_assert(std::is_base_of_v<Component, T>); return T(AddComponent(typeof<T>).GetPointer()); }
        System::Array<Component>        GetComponentsInChildren(System::Type pType, bool includeInactive);
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Camera.html
    class Camera : public Behaviour
    {
    public:
        // Constructors
        explicit                        Camera(unsafe::Object* ptr) : Behaviour(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }
        inline bool                     operator==(const Camera& o) const { return m_Pointer == o.m_Pointer; }

        // Static Properties
        static Camera                   GetMain();

        // Properties
        float                           GetOrthographicSize();
        void                            SetOrthographicSize(float value);
        int                             GetPixelWidth();
        int                             GetPixelHeight();
        Matrix4x4                       GetProjectionMatrix();
        void                            GetProjectionMatrix_Injected(Matrix4x4* ret);
        Matrix4x4                       GetWorldToCameraMatrix();
        void                            GetWorldToCameraMatrix_Injected(Matrix4x4* ret);
        float                           GetFarClipPlane();
        void                            SetFarClipPlane(float value);
        float                           GetNearClipPlane();
        float                           GetFieldOfView();

        // Public Methods
        Vector3                         WorldToScreenPoint(const Vector3& position);

        // HaxSdk Extension
        static Vector3                  WorldToScreenPoint(const Vector3& worldPos, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, float screenWidth, float screenHeight);
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Light.html
    class Light : public Behaviour
    {
    public:
        // Constructors
        explicit                        Light(unsafe::Object* ptr) : Behaviour(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        void                            SetIntensity(float value);
        float                           GetIntensity();
        void                            SetRange(float value);
        float                           GetRange();
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Shader.html
    class Shader : public Object
    {
    public:
        // Constructors
        explicit                        Shader(unsafe::Object* ptr) : Object(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Static Methods
        static Shader                   Find(System::String name);
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Material.html
    class Material : public Object
    {
    public:
        // Constructors
        explicit                        Material(unsafe::Object* ptr) : Object(ptr) {}
        static Material                 New(Shader shader);

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        void                            SetInt(System::String name, int value);
        void                            SetColor(Color color);
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Renderer.html
    class Renderer : public Component
    {
    public:
        // Constructors
        explicit                        Renderer(unsafe::Object* ptr) : Component(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        Bounds                          GetBounds();
        Material                        GetSharedMaterial();
        void                            SetSharedMaterial(Material pMaterial);
        void                            SetMaterial(Material pMaterial);
        void                            SetEnabled(bool value);
        void                            SetSortingOrder(int value);
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/MeshRenderer.html
    class MeshRenderer : public Renderer
    {
    public:
        // Constructors
        explicit                        MeshRenderer(unsafe::Object* ptr) : Renderer(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }
    };

    // [Class] LineRenderer
    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/LineRenderer.html
    class LineRenderer : public Renderer
    {
    public:
        // Constructors
        LineRenderer() : Renderer(nullptr) {}
        explicit                        LineRenderer(unsafe::Object* ptr) : Renderer(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        void                            SetStartWidth(float value);
        void                            SetEndWidth(float value);
        void                            SetPositionCount(int value);
        void                            SetPosition(int index, const Vector3& position);
    };

    // [Class] Screen
    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Screen.html
    class Screen
    {
    public:
        // Static Methods
        static int                      GetHeight();
        static int                      GetWidth();
    };

    // [Class] Sprite
    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Sprite.html
    class Sprite : public Object
    {

    };

    // [Class] RenderSettings
    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/RenderSettings.html
    class RenderSettings : public Object
    {
    public:
        // Constructors
        explicit                        RenderSettings(unsafe::Object* ptr) : Object(ptr) {}

        // Static Properties
        static bool                     GetFog();
        static void                     SetFog(bool value);
    };
}

//-----------------------------------------------------------------------------
// [SECTION] UnityEngine.PhysicsModule
//-----------------------------------------------------------------------------

namespace Unity
{
    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Collider2D.html
    class Collider2D : public Behaviour
    {
    public:
        // Constructors
        explicit                        Collider2D(unsafe::Object* ptr) : Behaviour(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/Collider.html
    class Collider : public Component
    {
    public:
        // Constructors
        explicit                        Collider(unsafe::Object* ptr) : Component(ptr) {}

        // Operators
        inline                          operator bool() { return !Null(); }

        // Properties
        bool                            GetEnabled();
        void                            SetEnabled(bool value);
        Bounds                          GetBounds();
    };

    // Docs: https://docs.unity3d.com/6000.1/Documentation/ScriptReference/BoxCollider.html
    class BoxCollider : public Collider
    {
    public:
        // Constructors
        explicit                        BoxCollider(unsafe::Object* ptr) : Collider(ptr) {}

        // Operators
        inline                          operator bool() const { return !Null(); }

        // Properties
        Vector3                         GetCenter();
        Vector3                         GetSize();
    };
}

//-----------------------------------------------------------------------------
// [SECTION] UnityEngine.UI
//-----------------------------------------------------------------------------

// Docs: https://docs.unity3d.com/2018.2/Documentation/ScriptReference/EventSystems.EventSystem.html
namespace Unity
{
    class EventSystem : public MonoBehaviour
    {
    public:
        // Constructors
        explicit                        EventSystem(unsafe::Object* ptr) : MonoBehaviour(ptr) {}

        // Static Properties
        static EventSystem              GetCurrent();
    };
}

//-----------------------------------------------------------------------------
// [SECTION] PhotonUnityNetworking
//-----------------------------------------------------------------------------

namespace Photon
{
    // Docs: https://doc-api.photonengine.com/en/pun/current/class_photon_1_1_realtime_1_1_player.html
    struct Player : System::Object
    {
        // Constructors
        explicit                        Player(unsafe::Object* ptr) : System::Object(ptr) {}

        // Operators
        inline bool                     operator==(const Player& p) const { return p.m_Pointer == m_Pointer; }
    };

    // Docs: https://doc-api.photonengine.com/en/pun/current/class_photon_1_1_pun_1_1_photon_view.html
    struct PhotonView : Unity::MonoBehaviour
    {
        // Properties
        Player                          GetOwner();
    };

    // Docs: https://doc-api.photonengine.com/en/pun/current/class_photon_1_1_pun_1_1_photon_network.html
    struct PhotonNetwork
    {
        // Static Properties
        static Player                   GetMasterClient();

        // Public Methods
        static Unity::GameObject        InstantiateRoomObject(System::String name,
            const Unity::Vector3& position,
            const Unity::Quaternion& rotation,
            char8_t group = 0,
            void* data = nullptr);
    };
}

inline constexpr Unity::Object null(nullptr);

//-----------------------------------------------------------------------------
// [SECTION] typeOf
//-----------------------------------------------------------------------------

template <>
inline System::Type typeof<Unity::AsyncOperation> = System::Type("UnityEngine.CoreModule", "UnityEngine", "AsyncOperation");

template <>
inline System::Type typeof<Unity::Behaviour> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Behaviour");

template <>
inline System::Type typeof<Unity::BoxCollider> = System::Type("UnityEngine.PhysicsModule", "UnityEngine", "BoxCollider");

template <>
inline System::Type typeof<Unity::Camera> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Camera");

template <>
inline System::Type typeof<Unity::Collider> = System::Type("UnityEngine.PhysicsModule", "UnityEngine", "Collider");

template <>
inline System::Type typeof<Unity::Object> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Object");

template <>
inline System::Type typeof<Unity::Component> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Component");

template <>
inline System::Type typeof<Unity::Transform> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Transform");

template <>
inline System::Type typeof<Unity::Screen> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Screen");

template <>
inline System::Type typeof<Unity::Vector3> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Vector3");

template <>
inline System::Type typeof<Unity::GameObject> = System::Type("UnityEngine.CoreModule", "UnityEngine", "GameObject");

template <>
inline System::Type typeof<Unity::Light> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Light");

template <>
inline System::Type typeof<Unity::LineRenderer> = System::Type("UnityEngine.CoreModule", "UnityEngine", "LineRenderer");

template <>
inline System::Type typeof<Unity::Renderer> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Renderer");

template <>
inline System::Type typeof<Unity::Shader> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Shader");

template <>
inline System::Type typeof<Unity::Material> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Material");

template <>
inline System::Type typeof<Unity::MeshRenderer> = System::Type("UnityEngine.CoreModule", "UnityEngine", "MeshRenderer");

template <>
inline System::Type typeof<Unity::RenderSettings> = System::Type("UnityEngine.CoreModule", "UnityEngine", "RenderSettings");

template <>
inline System::Type typeof<Unity::EventSystem> = System::Type("UnityEngine.UI", "UnityEngine.EventSystems", "EventSystem");


