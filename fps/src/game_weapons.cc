#include "game.h"
#include "entity_manager.h"

namespace
{
	float wp_rocket_speed = 900;
}

// /*
// =================
// fire_rocket
// =================
// */
// gentity_t *fire_rocket (gentity_t *self, vec3_t start, vec3_t dir) {
// 	gentity_t	*bolt;

// 	VectorNormalize (dir);

// 	bolt = G_Spawn();
// 	bolt->classname = "rocket";
// 	bolt->nextthink = level.time + 15000;
// 	bolt->think = G_ExplodeMissile;
// 	bolt->s.eType = ET_MISSILE;
// 	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
// 	bolt->s.weapon = WP_ROCKET_LAUNCHER;
// 	bolt->r.ownerNum = self->s.number;
// 	bolt->parent = self;
// 	bolt->damage = 100;
// 	bolt->splashDamage = 100;
// 	bolt->splashRadius = 120;
// 	bolt->methodOfDeath = MOD_ROCKET;
// 	bolt->splashMethodOfDeath = MOD_ROCKET_SPLASH;
// 	bolt->clipmask = MASK_SHOT;
// 	bolt->target_ent = NULL;

// 	bolt->s.pos.trType = TR_LINEAR;
// 	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;		// move a bit on the very first frame
// 	VectorCopy( start, bolt->s.pos.trBase );
// 	VectorScale( dir, 900, bolt->s.pos.trDelta );
// 	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
// 	VectorCopy (start, bolt->r.currentOrigin);

// 	return bolt;
// }


// it seems bolt_velocity is 900.
Entity& fire_rocket(Entity_Manager& entity_manager, Entity& self, glm::vec3 start, glm::vec3 direction)
{
	Entity& bolt = spawn_entity(entity_manager, Entity_Type::Rocket);
	bolt.type = Entity_Type::Rocket;
	bolt.parent_id = self.id;
	bolt.damage = 100;
	bolt.mesh_name = "cube";
	bolt.position = start;
	bolt.movement_vector = direction * wp_rocket_speed;

	// bolt.splash_damage = 100;
	// bolt.splash_radius = 120;
	// bolt.target_ent = 0;
	// bolt.next_think = current_time + 1500ms?;
	return bolt;
}