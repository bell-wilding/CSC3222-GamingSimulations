#pragma once
#include "../../Common/Vector2.h"
#include "../../Common/Vector4.h"
#include "RigidBody.h"
#include <set>

namespace NCL {
	using namespace Maths;

	namespace Rendering {
		class TextureBase;
	}
	namespace CSC3222 {
		class GameSimsRenderer;
		class TextureManager;
		class FruitWizardGame;
		class RigidBody;
		class CollisionVolume;

		class SimObject	: public RigidBody {
		public:
			SimObject(std::string tag = "");
			~SimObject();

			void SetCollider(CollisionVolume * c) {
				collider = c;
			}

			CollisionVolume* GetCollider() const {
				return collider;
			}

			void	UpdateAnimFrame(float dt);

			void	UpdateCollision(SimObject* collidingObject);
			void	UpdateCollisions();

			virtual bool UpdateObject(float dt) = 0;

			virtual void DrawObject(GameSimsRenderer &r);

			virtual void OnCollisionEnter(SimObject* col);

			virtual void OnCollisionExit(SimObject* col);

			virtual void OnCollision(SimObject* col);

			bool CollidingWith(std::string tag);

			static void InitObjects(FruitWizardGame* game, TextureManager* manager);

			void AddNonPhysicalCollision(std::string tag) { nonPhysicalCollisionTags.insert(tag); }
			void AddNonPhysicalCollisions(std::set<std::string> tags) { nonPhysicalCollisionTags.insert(tags.begin(), tags.end()); }
			void RemoveNonPhysicalCollision(std::string tag) { nonPhysicalCollisionTags.erase(tag); }
			void RemoveNonPhysicalCollisions(std::set<std::string> tags) { nonPhysicalCollisionTags.erase(tags.begin(), tags.end()); }

			void AddNonImpulseCollision(std::string tag) { nonImpulseCollisionTags.insert(tag); }
			void AddNonImpulselCollisions(std::set<std::string> tags) { nonImpulseCollisionTags.insert(tags.begin(), tags.end()); }
			void RemoveNonImpulseCollision(std::string tag) { nonImpulseCollisionTags.erase(tag); }
			void RemoveNonImpulseCollisions(std::set<std::string> tags) { nonImpulseCollisionTags.erase(tags.begin(), tags.end()); }

			bool PhysicalCollisionAllowed(std::string tag) {
				return nonPhysicalCollisionTags.find(tag) == nonPhysicalCollisionTags.end();
			}

			bool ImpulseCollisionAllowed(std::string tag) {
				return nonImpulseCollisionTags.find(tag) == nonImpulseCollisionTags.end();
			}

			void Remove() { active = false; };

		protected:

			CollisionVolume*		collider;
			Rendering::TextureBase* texture;


			Vector4			animFrameData;
			int				currentanimFrame;
			bool			flipAnimFrame;
			int				animFrameCount;
			float			frameTime;
			float			frameRate;
			bool			active;

			std::set<SimObject*> currentFrameCollisions;
			std::set<SimObject*> prevFrameCollisions;

			std::set<std::string> nonImpulseCollisionTags;

			std::set<std::string> nonPhysicalCollisionTags;

			static TextureManager*  texManager;
			static FruitWizardGame*		game;

		};
	}
}

