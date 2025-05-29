/*

[Index of this file]
|
|- [SECTION] Definitions from HaxSdk API
|- [SECTION] Definitions from UnityEngine.CoreModule
|- [SECTION] Definitions from UnityEngine.PhysicsModule
|- [SECTION] Definitions from UnityEngine.UI
|- [SECTION] Definitions from PhotonUnityNetworking

*/

#include "haxsdk_unity.h"

namespace vtables
{
    unsafe::VTable* g_Bounds;
    unsafe::VTable* g_Color;
    unsafe::VTable* g_Matrix4x4;
    unsafe::VTable* g_Quaternion;
    unsafe::VTable* g_Vector3;
}

//-----------------------------------------------------------------------------
// [SECTION] Definitions from HaxSdk API
//-----------------------------------------------------------------------------

void HaxSdk::InitUnity()
{
    static bool s_Initialized = false;
    if (!s_Initialized)
    {
        s_Initialized = true;

        HaxSdk::InitSystem();

        vtables::g_Vector3 = unsafe::Image::GetUnityCore()->GetClass("UnityEngine", "Vector3")->GetVTable();
        vtables::g_Quaternion = unsafe::Image::GetUnityCore()->GetClass("UnityEngine", "Quaternion")->GetVTable();
        vtables::g_Matrix4x4 = unsafe::Image::GetUnityCore()->GetClass("UnityEngine", "Matrix4x4")->GetVTable();
        vtables::g_Bounds = unsafe::Image::GetUnityCore()->GetClass("UnityEngine", "Bounds")->GetVTable();
        vtables::g_Color = unsafe::Image::GetUnityCore()->GetClass("UnityEngine", "Color")->GetVTable();
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Definitions from UnityEngine.CoreModule
//-----------------------------------------------------------------------------

namespace Unity
{
    bool AsyncOperation::GetIsDone()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<AsyncOperation>.GetMethod("get_isDone"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<bool, AsyncOperation>(*this);

        return method.Address<bool, AsyncOperation>(*this);
    }

    float AsyncOperation::GetProgress()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<AsyncOperation>.GetMethod("get_progress"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<float, AsyncOperation>(*this);

        return method.Address<float, AsyncOperation>(*this);
    }

    bool Behaviour::GetEnabled()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Behaviour>.GetMethod("get_enabled"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<bool, Behaviour>(*this);

        return method.Address<bool, Behaviour>(*this);
    }

    bool Behaviour::GetIsActiveAndEnabled()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Behaviour>.GetMethod("get_isActiveAndEnabled"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<bool, Behaviour>(*this);

        return method.Address<bool, Behaviour>(*this);
    }

    void Behaviour::SetEnabled(bool value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Behaviour>.GetMethod("set_enabled"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Behaviour, bool>(*this, value);
            return;
        }

        return method.Address<void, Behaviour, bool>(*this, value);
    }

    Bounds_Boxed Bounds::Box() const
    {
        return Bounds_Boxed(*this);
    }

    Bounds_Boxed::Bounds_Boxed(const Bounds& b) : Object(vtables::g_Bounds), m_Value(b)
    {

    }

    //
    // Camera
    //
    STATIC Camera Camera::GetMain()
    {
        static System::MethodInfo method(typeof<Camera>.GetMethod("get_main"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Camera>();

        return method.Address<Camera>();
    }

    Vector3 Camera::WorldToScreenPoint(const Vector3& worldPos, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, float screenWidth, float screenHeight)
    {
        Unity::Vector4 worldPos4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

        // 2. View * World
        Unity::Vector4 viewSpace = viewMatrix * worldPos4;

        // 3. Projection * View
        Unity::Vector4 clipSpace = projectionMatrix * viewSpace;

        if (clipSpace.w < 0.1f)
            return Unity::Vector3(-9999, -9999, -9999); // объект за камерой

        // 4. Преобразование в нормализованные координаты устройства (NDC)
        Unity::Vector3 ndc;
        ndc.x = clipSpace.x / clipSpace.w;
        ndc.y = clipSpace.y / clipSpace.w;
        ndc.z = clipSpace.z / clipSpace.w;

        // 5. Преобразование в экранные координаты
        Unity::Vector3 screenPos;
        screenPos.x = (ndc.x * 0.5f + 0.5f) * screenWidth;
        screenPos.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenHeight;
        screenPos.z = ndc.z;

        return screenPos;
    }

    Vector3 Camera::WorldToScreenPoint(const Vector3& pos)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("WorldToScreenPoint", "UnityEngine.Vector3(UnityEngine.Vector3)"));
        
        if (HaxSdk::IsMono())
        {
            Vector3_Boxed boxed(pos);
            return method.Thunk<Vector3_Boxed*, Camera, Vector3_Boxed*>(*this, &boxed)->m_Value;
        }
        
        return method.Address<Vector3, Camera, Vector3>(*this, pos);
    }

    float Camera::GetOrthographicSize()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_orthographicSize"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<float, Camera>(*this);

        return method.Address<float, Camera>(*this);
    }

    void Camera::SetOrthographicSize(float value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("set_orthographicSize"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<void, Camera, float>(*this, value);

        return method.Address<void, Camera, float>(*this, value);
    }

    int Camera::GetPixelWidth()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_pixelWidth"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<int, Camera>(*this);

        return method.Address<int, Camera>(*this);
    }

    int Camera::GetPixelHeight()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_pixelHeight"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<int, Camera>(*this);

        return method.Address<int, Camera>(*this);
    }

    Matrix4x4 Camera::GetProjectionMatrix()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_projectionMatrix"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Matrix4x4_Boxed*, Camera>(*this)->m_Value;

        return method.Address<Matrix4x4, Camera>(*this);
    }

    void Camera::GetProjectionMatrix_Injected(Matrix4x4* ret)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_projectionMatrix_Injected"));
        
        if (HaxSdk::IsMono())
        {
            Matrix4x4_Boxed boxed(*ret);
            method.Thunk<void, Camera, Matrix4x4_Boxed*>(*this, &boxed);
            *ret = boxed.m_Value;
            return;
        }

        method.Address<void, Camera, Matrix4x4*>(*this, ret);
    }

    Matrix4x4 Camera::GetWorldToCameraMatrix()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_worldToCameraMatrix"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Matrix4x4_Boxed*, Camera>(*this)->m_Value;

        return method.Address<Matrix4x4, Camera>(*this);
    }

    void Camera::GetWorldToCameraMatrix_Injected(Matrix4x4* ret)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_worldToCameraMatrix_Injected"));
        
        if (HaxSdk::IsMono())
        {
            Matrix4x4_Boxed boxed(*ret);
            method.Thunk<void, Camera, Matrix4x4_Boxed*>(*this, &boxed);
            *ret = boxed.m_Value;
            return;
        }

        method.Address<void, Camera, Matrix4x4*>(*this, ret);
    }

    float Camera::GetFarClipPlane()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_farClipPlane"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<float, Camera>(*this);

        return method.Address<float, Camera>(*this);
    }

    void Camera::SetFarClipPlane(float value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_farClipPlane"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Camera, float>(*this, value);
            return;
        }

         method.Address<void, Camera, float>(*this, value);
    }

    float Camera::GetNearClipPlane()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_nearClipPlane"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<float, Camera>(*this);

        return method.Address<float, Camera>(*this);
    }

    float Camera::GetFieldOfView()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("get_fieldOfView"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<float, Camera>(*this);

        return method.Address<float, Camera>(*this);
    }

    //
    // Color
    //
    Color_Boxed::Color_Boxed(const Color& c) : Object(vtables::g_Color), m_Value(c)
    {

    }

    //
    // Component
    //
    Transform Component::GetTransform()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Component>.GetMethod("get_transform"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Transform, Component>(*this);

        return method.Address<Transform, Component>(*this);
    }

    GameObject Component::GetGameObject()
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Component>.GetMethod("get_gameObject");
        
        if (HaxSdk::IsMono())
            return method.Thunk<GameObject, Component>(*this);

        return method.Address<GameObject, Component>(*this);
    }

    Component Component::GetComponentInChildren(System::Type type)
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Component>.GetMethod("GetComponentInChildren", "UnityEngine.Component(System.Type)");
        
        if (HaxSdk::IsMono())
            return method.Thunk<Component, Component, System::Type>(*this, type);

        return method.Address<Component, Component, System::Type>(*this, type);
    }

    System::Array<Component> Component::GetComponentsInChildren(System::Type type)
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Component>.GetMethod("GetComponentsInChildren", "UnityEngine.Component[](System.Type)");
        
        if (HaxSdk::IsMono())
            return method.Thunk<System::Array<Component>, Component, System::Type>(*this, type);

        return method.Address<System::Array<Component>, Component, System::Type>(*this, type);
    }

    Component Component::GetComponent(System::Type type)
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Component>.GetMethod("GetComponent", "UnityEngine.Component(System.Type)");
        
        if (HaxSdk::IsMono())
            return method.Thunk<Component, Component, System::Type>(*this, type);

        return method.Address<Component, Component, System::Type>(*this, type);
    }

    //
    // GameObject
    //
    STATIC GameObject GameObject::New()
    {
        return GameObject(unsafe::Object::New(typeof<GameObject>.GetPointer()->GetType()->GetClass())->Ctor());
    }

    STATIC GameObject GameObject::New(const char* name)
    {
        static System::MethodInfo method(typeof<GameObject>.GetMethod(".ctor", "System.Void(System.String)"));
        unsafe::Object* obj = unsafe::Object::New(typeof<GameObject>.GetPointer()->GetType()->GetClass());

        if (HaxSdk::IsMono())
            method.Thunk<void, unsafe::Object*>(obj);
        else
            method.Address<void, unsafe::Object*>(obj);

        return GameObject(obj);
    }

    Transform GameObject::GetTransform()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<GameObject>.GetMethod("get_transform"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Transform, GameObject>(*this);

        return method.Address<Transform, GameObject>(*this);
    }

    void GameObject::SetLayer(int value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<GameObject>.GetMethod("set_layer"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, GameObject, int>(*this, value);
            return;
        }

        method.Address<void, GameObject, int>(*this, value);
    }

    bool GameObject::GetActiveSelf()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<GameObject>.GetMethod("get_activeSelf"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<bool, GameObject>(*this);

        return method.Address<bool, GameObject>(*this);
    }

    Component GameObject::GetComponent(System::Type type)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<GameObject>.GetMethod("GetComponent", "UnityEngine.Component(System.Type)"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Component, GameObject, System::Type>(*this, type);

        return method.Address<Component, GameObject, System::Type>(*this, type);
    }

    Component GameObject::AddComponent(System::Type componentType)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<GameObject>.GetMethod("AddComponent", "UnityEngine.Component(System.Type)"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Component, GameObject, System::Type>(*this, componentType);

        return method.Address<Component, GameObject, System::Type>(*this, componentType);
    }

    System::Array<Component> GameObject::GetComponentsInChildren(System::Type type, bool includeInactive)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<GameObject>.GetMethod("GetComponentsInChildren", "Component[](System.Type,System.Boolean)"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<System::Array<Component>, GameObject, System::Type, bool>(*this, type, includeInactive);

        return method.Address<System::Array<Component>, GameObject, System::Type, bool>(*this, type, includeInactive);
    }

    void GameObject::SetActive(bool value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<GameObject>.GetMethod("set_active"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, GameObject, bool>(*this, value);
            return;
        }

        method.Address<void, GameObject, bool>(*this, value);
    }

    bool GameObject::GetActive()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<GameObject>.GetMethod("get_active"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<bool, GameObject>(*this);

        return method.Address<bool, GameObject>(*this);
    }

    bool GameObject::GetActiveInHierarchy()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<GameObject>.GetMethod("get_activeInHierarchy"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<bool, GameObject>(*this);

        return method.Address<bool, GameObject>(*this);
    }

    //
    // Light
    //
    void Light::SetIntensity(float value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Light>.GetMethod("set_intensity"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Light, float>(*this, value);
            return;
        }

        return method.Address<void, Light, float>(*this, value);
    }

    float Light::GetIntensity()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Light>.GetMethod("get_intensity"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<float, Light>(*this);

        return method.Address<float, Light>(*this);
    }

    void Light::SetRange(float value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Light>.GetMethod("set_range"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Light, float>(*this, value);
            return;
        }

        return method.Address<void, Light, float>(*this, value);
    }

    float Light::GetRange()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Light>.GetMethod("get_range"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<float, Light>(*this);

        return method.Address<float, Light>(*this);
    }

    //
    // LineRenderer
    //
    void LineRenderer::SetStartWidth(float value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<LineRenderer>.GetMethod("set_startWidth"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<void, LineRenderer, float>(*this, value);
        else
            return method.Address<void, LineRenderer, float>(*this, value);
    }

    void LineRenderer::SetEndWidth(float value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<LineRenderer>.GetMethod("set_endWidth"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<void, LineRenderer, float>(*this, value);
        else
            return method.Address<void, LineRenderer, float>(*this, value);
    }

    void LineRenderer::SetPositionCount(int value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<LineRenderer>.GetMethod("set_positionCount"));
        
        if (HaxSdk::IsMono())
            method.Thunk<void, LineRenderer, int>(*this, value);
        else
            method.Address<void, LineRenderer, int>(*this, value);
    }

    void LineRenderer::SetPosition(int index, const Vector3& position)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<LineRenderer>.GetMethod("SetPosition"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, LineRenderer, int, const Vector3_Boxed&>(*this, index, position.Box());
            return;
        }

        method.Address<void, LineRenderer, int, Vector3>(*this, index, position);
    }

    //
    // Material
    //
    STATIC Material Material::New(Shader shader)
    {
        static System::MethodInfo method(typeof<Material>.GetMethod(".ctor", "System.Void(UnityEngine.Shader)"));
        Material mat = Material(unsafe::Object::New(typeof<Material>.GetPointer()->GetType()->GetClass()));

        if (HaxSdk::IsMono())
            method.Thunk<void, Material, Shader>(mat, shader);
        else
            method.Address<void, Material, Shader>(mat, shader);

        return mat;
    }

    void Material::SetInt(System::String name, int value)
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Material>.GetMethod("SetInt", "System.Void(System.String,System.Int32)");
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Material, System::String, int>(*this, name, value);
            return;
        }

        method.Address<void, Material, System::String, int>(*this, name, value);
    }

    void Material::SetColor(Color color)
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Material>.GetMethod("set_color");
        
        if (HaxSdk::IsMono())
        {
            Color_Boxed boxed(color);
            method.Thunk<void, Material, Color_Boxed*>(*this, &boxed);
            return;
        }

        method.Address<void, Material, Color>(*this, color);
    }

    //
    // Matrix4x4
    //
    Matrix4x4_Boxed::Matrix4x4_Boxed(const Matrix4x4& m) : Object(vtables::g_Matrix4x4), m_Value(m)
    {

    }

    Vector4 Matrix4x4::operator*(const Unity::Vector4& v) const
    {
        Unity::Vector4 result = { 0 };
        result.x = m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w;
        result.y = m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w;
        result.z = m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w;
        result.w = m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w;
        return result;
    }

    Unity::Vector3 Unity::Matrix4x4::MultiplyPoint(Unity::Vector3& point)
    {
        Vector3 result;
        result.x = m00 * point.x + m01 * point.y + m02 * point.z + m03;
        result.y = m10 * point.x + m11 * point.y + m12 * point.z + m13;
        result.z = m20 * point.x + m21 * point.y + m22 * point.z + m23;
        float num = m30 * point.x + m31 * point.y + m32 * point.z + m33;
        num = 1.f / num;
        result.x *= num;
        result.y *= num;
        result.z *= num;
        return result;
    }

    //
    // Object
    //
    Object Object::Instantiate(Object original)
    {
        static System::MethodInfo method = typeof<Object>.GetMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object)");
        
        if (HaxSdk::IsMono())
            return method.Thunk<Object, Object>(original);

        return method.Address<Object, Object>(original);
    }

    Object Object::Instantiate(Object original, const Vector3& position, const Quaternion& rotation)
    {
        static System::MethodInfo method = typeof<Object>.GetMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object,UnityEngine.Vector3,UnityEngine.Quaternion)");
        
        if (HaxSdk::IsMono())
            return method.Thunk<Object, Object, Vector3_Boxed, Quaternion_Boxed>(original, Vector3_Boxed(position), Quaternion_Boxed(rotation));

        return method.Address<Object, Object, Vector3, Quaternion>(original, position, rotation);
    }

    System::Array<Object> Object::FindObjectsOfType(System::Type type)
    {
        static System::MethodInfo method = typeof<Object>.GetMethod("FindObjectsOfType", "UnityEngine.Object[](System.Type)");
        
        if (HaxSdk::IsMono())
            return method.Thunk<System::Array<Object>, System::Type>(type);

        return method.Address<System::Array<Object>, System::Type>(type);
    }

    Object Object::FindObjectOfType(System::Type type)
    {
        static System::MethodInfo method = typeof<Object>.GetMethod("FindObjectOfType", "UnityEngine.Object(System.Type)");
        
        if (HaxSdk::IsMono())
            return method.Thunk<Object, System::Type>(type);

        return method.Address<Object, System::Type>(type);
    }

    void Object::Destroy(Object obj, float t)
    {
        static System::MethodInfo method = typeof<Object>.GetMethod("Destroy");
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Object, float>(obj, t);
            return;
        }

        method.Address<void, Object, float>(obj, t);
    }

    System::String Object::GetName()
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Object>.GetMethod("get_name");
        
        if (HaxSdk::IsMono())
            return method.Thunk<System::String, Object>(*this);

        return method.Address<System::String, Object>(*this);
    }

    void Object::SetName(System::String name)
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Object>.GetMethod("set_name");
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Object, System::String>(*this, name);
            return;
        }

        method.Address<void, Object, System::String>(*this, name);
    }

    void Object::SetHideFlags(HideFlags flags)
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Object>.GetMethod("set_hideFlags");
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Object, HideFlags>(*this, flags);
            return;
        }

        method.Address<void, Object, HideFlags>(*this, flags);
    }

    //
    // Quaternion
    //
    Quaternion_Boxed::Quaternion_Boxed(float x, float y, float z, float w) : Object(vtables::g_Quaternion), m_Value(x, y, z, w)
    {

    }

    Quaternion_Boxed::Quaternion_Boxed(const Quaternion& v) : Object(vtables::g_Quaternion), m_Value(v)
    {

    }

    //
    // Renderer
    //
    Bounds Renderer::GetBounds()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Renderer>.GetMethod("get_bounds"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Bounds_Boxed*, Renderer>(*this)->m_Value;

        return method.Address<Bounds, Renderer>(*this);
    }

    Material Renderer::GetSharedMaterial()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Renderer>.GetMethod("get_sharedMaterial"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Material, Renderer>(*this);

        return method.Address<Material, Renderer>(*this);
    }

    void Renderer::SetSharedMaterial(Material material)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Renderer>.GetMethod("set_sharedMaterial"));
        
        if (HaxSdk::IsMono())
            method.Thunk<void, Renderer, Material>(*this, material);
        else
            method.Address<void, Renderer, Material>(*this, material);
    }

    void Renderer::SetMaterial(Material material)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Renderer>.GetMethod("set_material"));
        
        if (HaxSdk::IsMono())
            method.Thunk<void, Renderer, Material>(*this, material);
        else
            method.Address<void, Renderer, Material>(*this, material);
    }

    void Renderer::SetEnabled(bool value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Renderer>.GetMethod("set_enabled"));
        
        if (HaxSdk::IsMono())
            method.Thunk<void, Renderer, bool>(*this, value);
        else
            method.Address<void, Renderer, bool>(*this, value);
    }

    void Renderer::SetSortingOrder(int value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Renderer>.GetMethod("set_sortingOrder"));
        
        if (HaxSdk::IsMono())
            method.Thunk<void, Renderer, int>(*this, value);
        else
            method.Address<void, Renderer, int>(*this, value);
    }

    //
    // RenderSettings
    //
    bool RenderSettings::GetFog()
    {
        static System::MethodInfo method(typeof<RenderSettings>.GetMethod("get_fog"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<bool>();

        return method.Address<bool>();
    }

    void RenderSettings::SetFog(bool value)
    {
        static System::MethodInfo method(typeof<RenderSettings>.GetMethod("set_fog"));
        
        if (HaxSdk::IsMono())
            method.Thunk<void, bool>(value);
        else
            method.Address<void, bool>(value);
    }

    //
    // Screen
    //
    int Screen::GetHeight()
    {
        static System::MethodInfo method(typeof<Screen>.GetMethod("get_height"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<int>();

        return method.Address<int>();
    }

    int Screen::GetWidth()
    {
        static System::MethodInfo method(typeof<Screen>.GetMethod("get_width"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<int>();

        return method.Address<int>();
    }

    //
    // Shader
    //
    STATIC Shader Shader::Find(System::String name)
    {
        static System::MethodInfo method(typeof<Shader>.GetMethod("Find"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Shader, System::String>(name);

        return method.Address<Shader, System::String>(name);
    }

    //
    // Transform
    //
    Vector3 Transform::GetPosition()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("get_position"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Vector3_Boxed*, Transform>(*this)->m_Value;

        return method.Address<Vector3, Transform>(*this);
    }

    void Transform::SetPosition(const Unity::Vector3& value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("set_position"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Transform, Vector3_Boxed>(*this, Vector3_Boxed(value));
            return;
        }

        return method.Address<void, Transform, Vector3>(*this, value);
    }

    Transform Transform::GetParent()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("get_parent"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Transform, Transform>(*this);

        return method.Address<Transform, Transform>(*this);
    }

    void Transform::SetParent(const Unity::Transform& value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("set_parent"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<Transform, Transform>(*this);
            return;
        }

        method.Address<Transform, Transform>(*this);
    }

    Vector3 Transform::GetForward()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("get_forward"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Vector3_Boxed*, Transform>(*this)->m_Value;

        return method.Address<Vector3, Transform>(*this);
    }

    //
    // Vector3
    //
    STATIC float Vector3::Distance(const Unity::Vector3& a, const Unity::Vector3& b)
    {
        Vector3 vector(a.x - b.x, a.y - b.y, a.z - b.z);
        return std::sqrt(vector.x * vector.x + vector.y + vector.y + vector.z + vector.z);
    }

    float Vector3::Distance(const Unity::Vector3& other)
    {
        Vector3 vector(x - other.x, y - other.y, z - other.z);
        return std::sqrt(vector.x * vector.x + vector.y + vector.y + vector.z + vector.z);
    }

    Vector3 Vector3::Min(const Vector3& a, const Vector3& b)
    {
        return Vector3((std::min)(a.x, b.x), (std::min)(a.y, b.y), (std::min)(a.z, b.z));
    }

    Vector3 Vector3::Max(const Vector3& a, const Vector3& b)
    {
        return Vector3((std::max)(a.x, b.x), (std::max)(a.y, b.y), (std::max)(a.z, b.z));
    }

    Vector3_Boxed Vector3::Box() const
    {
        return Vector3_Boxed(*this);
    }

    Vector3_Boxed::Vector3_Boxed(float _x, float _y, float _z) : Object(vtables::g_Vector3), m_Value(Vector3(_x, _y, _z))
    {

    }

    Vector3_Boxed::Vector3_Boxed(const Vector3& v) : Object(vtables::g_Vector3), m_Value(v)
    {

    }

    Quaternion_Boxed Quaternion::Box() const
    {
        return Quaternion_Boxed(*this);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Definitions from UnityEngine.PhysicsModule
//-----------------------------------------------------------------------------

namespace Unity
{
    bool Collider::GetEnabled()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Collider>.GetMethod("get_enabled"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<bool, Collider>(*this);

        return method.Address<bool, Collider>(*this);
    }

    void Collider::SetEnabled(bool value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Collider>.GetMethod("set_enabled"));
        
        if (HaxSdk::IsMono())
        {
            method.Thunk<void, Collider, bool>(*this, value);
            return;
        }

        method.Address<void, Collider, bool>(*this, value);
    }

    Bounds Collider::GetBounds()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Collider>.GetMethod("get_bounds"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Bounds_Boxed*, Collider>(*this)->m_Value;

        return method.Address<Bounds, Collider>(*this);
    }

    Vector3 BoxCollider::GetCenter()
    {
        static System::MethodInfo method(typeof<BoxCollider>.GetMethod("get_center"));
        if (HaxSdk::IsMono())

            return method.Thunk<Vector3_Boxed*, BoxCollider>(*this)->m_Value;

        return method.Address<Vector3, BoxCollider>(*this);
    }

    Vector3 BoxCollider::GetSize()
    {
        static System::MethodInfo method(typeof<BoxCollider>.GetMethod("get_size"));
        
        if (HaxSdk::IsMono())
            return method.Thunk<Vector3_Boxed*, BoxCollider>(*this)->m_Value;

        return method.Address<Vector3, BoxCollider>(*this);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Definitions from UnityEngine.UI
//-----------------------------------------------------------------------------

namespace Unity
{
    //
    // EventSystem
    //
    EventSystem EventSystem::GetCurrent()
    {
        static System::MethodInfo method(typeof<EventSystem>.GetMethod("get_current"));
        if (HaxSdk::IsMono())
            return method.Thunk<EventSystem>();

        return method.Address<EventSystem>();
    }
}

//-----------------------------------------------------------------------------
// [SECTION] PhotonUnityNetworking
//-----------------------------------------------------------------------------

namespace Photon
{
    Player PhotonView::GetOwner()
    {
        CHECK_NULL();
        static System::MethodInfo method = System::AppDomain::GetCurrent().Load("PhotonUnityNetworking").GetType("Photon.Pun", "PhotonView").GetMethod("get_Owner");
        if (HaxSdk::IsMono())
            return method.Thunk<Player, PhotonView>(*this);
        return method.Address<Player, PhotonView>(*this);
    }

    Unity::GameObject PhotonNetwork::InstantiateRoomObject(System::String name, const Unity::Vector3& position, const Unity::Quaternion& rotation, char8_t group, void* data)
    {
        static System::MethodInfo method = System::AppDomain::GetCurrent().Load("PhotonUnityNetworking").GetType("Photon.Pun", "PhotonNetwork").GetMethod("InstantiateRoomObject");
        if (HaxSdk::IsMono())
            return method.Thunk<Unity::GameObject, 
                                System::String, 
                                const Unity::Vector3_Boxed&, 
                                const Unity::Quaternion_Boxed&, 
                                char8_t, 
                                void*>(name, position.Box(), rotation.Box(), group, data);
        return method.Address<Unity::GameObject, 
                            System::String, 
                            Unity::Vector3, 
                            Unity::Quaternion, 
                            char8_t, 
                            void*>(name, position, rotation, group, data); 
    }

    Player PhotonNetwork::GetMasterClient()
    {
        static System::MethodInfo method = System::AppDomain::GetCurrent().Load("PhotonUnityNetworking").GetType("Photon.Pun", "PhotonNetwork").GetMethod("get_MasterClient");
        if (HaxSdk::IsMono())
            return method.Thunk<Player>();
        return method.Address<Player>();
    }
}