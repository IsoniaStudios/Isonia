namespace Isonia::Math
{
	extern inline constexpr bool intersectRayPlane(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float& f)
	{
		return glm::intersectRayPlane(v1, v2, v3, v4, f);
	}
}
