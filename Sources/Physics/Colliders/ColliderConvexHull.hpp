#pragma once

#include "Models/Model.hpp"
#include "Collider.hpp"

class btConvexHullShape;

namespace acid {
class ACID_EXPORT ColliderConvexHull : public Collider::Registrar<ColliderConvexHull> {
public:
	explicit ColliderConvexHull(const std::vector<float> &pointCloud = {}, const Transform &localTransform = {});

	~ColliderConvexHull();

	btCollisionShape *GetCollisionShape() const override;

	uint32_t GetPointCount() const { return m_pointCount; }
	void SetPointCount(const std::vector<float> &pointCloud);

	friend const Node &operator>>(const Node &node, ColliderConvexHull &collider);
	friend Node &operator<<(Node &node, const ColliderConvexHull &collider);

private:
	static bool registered;

	std::unique_ptr<btConvexHullShape> m_shape;
	std::shared_ptr<Model> m_model;
	uint32_t m_pointCount = 0;
};
}
