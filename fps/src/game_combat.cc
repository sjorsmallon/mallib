#include "game.h"

#include "entity_manager.h" 
#include "glm/vec3.hpp"

namespace
{
	float g_knockback = 1000.f;
	float g_knockback_damage_threshold = 200.f;

	float g_self_damage_scale = 0.25f;	
}


enum Damage_Flags
{
	DMG_DEFAULT_FLAG = 0,
	DMG_NO_KNOCKBACK = 1 << 0 //1
	// 1 << 1
	// 1 << 2
	// 1 << 3
};


void damage(Entity& target, Entity& inflictor, Entity& attacker,const glm::vec3 dmg_direction_in, int damage)
{
    	if (!target.can_take_damage) return;

    	int32_t damage_flags{};

    	// if there is no inflictor: the world did.
    	// if there is no attacker: the world did.

    	// Is the target noclipping?
    	// client = target.client;
		// if (client && client.noclip)
		// {
		// 	return;
		// }

    	glm::vec3 dmg_direction{};
    	// is there a damage direction?
 		if (abs(dmg_direction.x) < 0.1f && abs(dmg_direction.y) < 0.1f && abs(dmg_direction.z) < 0.1f)
		{
				damage_flags |= DMG_NO_KNOCKBACK;
		}
		else
		{
			 dmg_direction = glm::normalize(dmg_direction_in);
		}

		int32_t knockback = damage;
		const int32_t knockback_threshold = static_cast<int32_t>(g_knockback_damage_threshold);
		if (knockback > knockback_threshold) knockback = knockback_threshold;
		// if (target.flags & FL_NO_KNOCKBACK) knockback = 0;
		if (damage_flags & DMG_NO_KNOCKBACK) knockback = 0;
    	

		// figure out knockback before any dmg reduction
		if (knockback)//  && target.client)
		{
			float mass = 200;
			glm::vec3 knockback_velocity = dmg_direction * (g_knockback * static_cast<float>(knockback) / mass);
			target.movement_vector += knockback_velocity;

			// set the timer so that the other client can't cancel
			// out the movement immediately
			// if ( !targ->client->ps.pm_time ) {
			// 	int		t;

			// 	t = knockback * 2;
			// 	if ( t < 50 ) {
			// 		t = 50;
			// 	}
			// 	if ( t > 200 ) {
			// 		t = 200;
			// 	}
			// 	targ->client->ps.pm_time = t;
			// 	targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
			// }
		}

    	// even if target is immune, can still suffer knockback
		{

		}

		// always give 0.25 if hurting self
		// calculated after knockback, so rocket jumping works
		if (target.id == attacker.id)
		{
			damage *= g_self_damage_scale;
		}

		// can't heal from damage!
		if(damage < 1)
		{
			damage = 1;
		}
		// adjust the damage based on many, many things.

		// actually do the damage
		if (damage)
		{
			target.health = target.health - damage;

			// if ( targ->client ) {
			// 	targ->client->ps.stats[STAT_HEALTH] = targ->health;
			// }
				
			if (target.health <= 0)
			{
				return;
			}
		}
}