#include "AABB_collisions.h"

void set_AABB(AABB *box) {
	if (!box) return;
}
Bool check_intersect(Vector3D minSelf, Vector3D maxSelf, Vector3D minOther, Vector3D maxOther) {
	if (
		minSelf.x <= maxOther.x
		&& minSelf.y <= maxOther.y
		&& minSelf.z <= maxOther.z
		&& maxSelf.x >= minOther.x
		&& maxSelf.y >= minOther.y
		&& maxSelf.z >= minOther.z)
		return true;
	return false;


}
Vector3D get_Intersection(Vector3D minSelf, Vector3D maxSelf, Vector3D minOther, Vector3D maxOther) {
	if (!check_intersect( minSelf, maxSelf,minOther,maxOther)){
		return;
	}
	//vector_add(minSelf,)
	//vector_add(maxSelf)
}