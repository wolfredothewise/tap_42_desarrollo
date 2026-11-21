#include "tap_server.h"
#include "tap_world.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool parse_items(cJSON *items_arr, t_world *world);
static bool parse_npcs(cJSON *npcs_arr, t_world *world);
static bool parse_quests(cJSON *quests_arr, t_world *world);
static bool parse_exits(cJSON *exits_obj, t_room *room);
static bool parse_room_items(cJSON *items_arr, t_world *world, t_room *room);
static bool parse_room_npcs(cJSON *npcs_arr, t_world *world, t_room *room);
static bool parse_rooms(cJSON *rooms_arr, t_world *world);
static bool validate_and_link_world(t_world *world);

static bool validate_and_link_world(t_world *world);

t_world *load_world(const char *filepath)
{
    char *raw_json = read_file_to_string(filepath);
    if (!raw_json)
        return NULL;

    cJSON *root = cJSON_Parse(raw_json);
    free(raw_json); // Liberar inmediatamente, sin importar si cJSON_Parse falló
    if (!root)
        return NULL;

    t_world *world = calloc(1, sizeof(t_world));
    if (!world)
    {
        cJSON_Delete(root);
        return NULL;
    }

    cJSON *spawn_room = cJSON_GetObjectItemCaseSensitive(root, "spawn_room");
    cJSON *items = cJSON_GetObjectItemCaseSensitive(root, "items");
    cJSON *npcs = cJSON_GetObjectItemCaseSensitive(root, "npcs");
    cJSON *quests = cJSON_GetObjectItemCaseSensitive(root, "quests");
    cJSON *rooms = cJSON_GetObjectItemCaseSensitive(root, "rooms");

    if (!cJSON_IsString(spawn_room) || !spawn_room->valuestring ||//valuestring, cosas de cJSON struct
        !cJSON_IsArray(items) || !cJSON_IsArray(npcs) ||
        !cJSON_IsArray(quests) || !cJSON_IsArray(rooms))
    {
        destroy_world(world);
        cJSON_Delete(root);
        return NULL;
    }

    world->spawn_room_id = strdup(spawn_room->valuestring);//el nombre
    if (!world->spawn_room_id)
    {
        destroy_world(world);
        cJSON_Delete(root);
        return NULL;
    }

    //cargar los catalogos y misiones
    if (!parse_items(items, world) || !parse_npcs(npcs, world) ||
        !parse_quests(quests, world) || !parse_rooms(rooms, world) ||
        !validate_and_link_world(world))
    {
        destroy_world(world);
        cJSON_Delete(root);
        return NULL;
    }
    



    cJSON_Delete(root);
    return world;
}



static bool parse_items(cJSON *items_arr, t_world *world)
{
    if (!items_arr || !world)
        return false;
    cJSON *item_json;
    cJSON_ArrayForEach(item_json, items_arr)
    {
        if (!cJSON_IsObject(item_json))
            return false;

        cJSON *id = cJSON_GetObjectItem(item_json, "id");
        cJSON *name = cJSON_GetObjectItem(item_json, "name");
        cJSON *description = cJSON_GetObjectItem(item_json, "description");
        cJSON *obtainable = cJSON_GetObjectItem(item_json, "obtainable");

        if (!cJSON_IsString(id) || !cJSON_IsString(name) ||
         !cJSON_IsString(description) || !cJSON_IsBool(obtainable))
            return false;
        
        t_item *res = calloc(1, sizeof(t_item));
        if (!res)
            return false;
        res->id = strdup(id->valuestring);
        res->name = strdup(name->valuestring);
        res->description = strdup(description->valuestring);
        res->obtainable = cJSON_IsTrue(obtainable);

        if(!res->id || !res->name || !res->description)
        {
            free_items(res);
            return false;
        }  
        item_add_back(&world->item_catalog, res);
    }

    return true;
}


static bool parse_npcs(cJSON *npcs_arr, t_world *world)
{
    if (!npcs_arr || !world)
        return false;
    cJSON *npc_json;
    cJSON_ArrayForEach(npc_json, npcs_arr)
    {
        if (!cJSON_IsObject(npc_json))
            return false;

        cJSON *id = cJSON_GetObjectItem(npc_json, "id");
        cJSON *name = cJSON_GetObjectItem(npc_json, "name");
        cJSON *role = cJSON_GetObjectItem(npc_json, "role");
        cJSON *hp = cJSON_GetObjectItem(npc_json, "hp");
        cJSON *damage = cJSON_GetObjectItem(npc_json, "damage");
        cJSON *dialogue = cJSON_GetObjectItem(npc_json, "dialogue");
        cJSON *quest_id = cJSON_GetObjectItem(npc_json, "quest_id");


        if (!cJSON_IsString(id) || !cJSON_IsString(name) ||
         !cJSON_IsString(role) ||  !cJSON_IsString(dialogue) ||
         !cJSON_IsNumber(hp) || !cJSON_IsNumber(damage) ||
         (!cJSON_IsString(quest_id) && !cJSON_IsNull(quest_id)))
            return false;
        
        t_npc *res = calloc(1, sizeof(t_npc));
        if (!res)
            return false;
        res->id = strdup(id->valuestring);
        res->name = strdup(name->valuestring);
        res->role = strdup(role->valuestring);
        res->hp = hp->valueint;
        res->max_hp = hp->valueint;
        res->damage = damage->valueint;
        res->dialogue = strdup(dialogue->valuestring);
        if (!quest_id|| cJSON_IsNull(quest_id))
            res->quest_id = NULL;
        else
            res->quest_id = strdup(quest_id->valuestring);

        if(!res->id || !res->name ||
        !res->role || !res->dialogue)
        {
            free_npcs(res);
            return false;
        }
        npc_add_back(&world->npc_catalog, res);
    }

    return true;
}

static bool parse_quests(cJSON *quests_arr, t_world *world)
{
    if (!quests_arr || !world)
        return false;
    cJSON *quest_json;
    cJSON_ArrayForEach(quest_json, quests_arr)
    {
        if (!cJSON_IsObject(quest_json))
            return false;

        cJSON *id = cJSON_GetObjectItem(quest_json, "id");
        cJSON *description = cJSON_GetObjectItem(quest_json, "description");
        cJSON *type = cJSON_GetObjectItem(quest_json, "type");
        cJSON *target_id = cJSON_GetObjectItem(quest_json, "target_id");
        cJSON *count = cJSON_GetObjectItem(quest_json, "count");
        cJSON *reward_item_id = cJSON_GetObjectItem(quest_json, "reward_item_id");



        if (!cJSON_IsString(id) || !cJSON_IsString(description) ||
         !cJSON_IsString(type) ||  !cJSON_IsString(target_id) ||
          !cJSON_IsString(reward_item_id) || !cJSON_IsNumber(count))
            return false;
        
        t_quest *res = calloc(1, sizeof(t_quest));
        if (!res)
            return false;
        res->id = strdup(id->valuestring);
        res->description = strdup(description->valuestring);
        res->type = strdup(type->valuestring);
        res->count = count->valueint;
        res->target_id = strdup(target_id->valuestring);
        res->reward_item_id = strdup(reward_item_id->valuestring);

        if(!res->id || !res->description ||
        !res->type || !res->target_id ||
        !res->reward_item_id)
        {
            free_quests(res);
            return false;
        }

        quest_add_back(&world->quests, res);
    }
    return true;
}

static bool	parse_exits(cJSON *exits_obj, t_room *room)
{
	cJSON	*exit_json;

	if (!exits_obj || !room)
		return (false);
	cJSON_ArrayForEach(exit_json, exits_obj)
	{
		if (!cJSON_IsString(exit_json) || !exit_json->string
			|| !exit_json->valuestring)
			return (false);
		t_exit *res = calloc(1, sizeof(t_exit));
		if (!res)
			return (false);
		res->direction = strdup(exit_json->string);
		res->target_room_id = strdup(exit_json->valuestring);
		if (!res->direction || !res->target_room_id)
		{
			free_exits(res);
			return (false);
		}
		exit_add_back(&room->exits, res);
	}
	return (true);
}

static bool	parse_room_items(cJSON *items_arr, t_world *world, t_room *room)
{
	cJSON	*id_json;

	cJSON_ArrayForEach(id_json, items_arr)
	{
		if (!cJSON_IsString(id_json) || !id_json->valuestring)
			return (false);
		t_item *tmpl = find_item_by_id(world->item_catalog, id_json->valuestring);
		if (!tmpl)
			return (false);
		t_item *clone = clone_item(tmpl);
		if (!clone)
			return (false);
		item_add_back(&room->items, clone);
	}
	return (true);
}

static bool	parse_room_npcs(cJSON *npcs_arr, t_world *world, t_room *room)
{
	cJSON	*id_json;

	cJSON_ArrayForEach(id_json, npcs_arr)
	{
		if (!cJSON_IsString(id_json) || !id_json->valuestring)
			return (false);
		t_npc *tmpl = find_npc_by_id(world->npc_catalog, id_json->valuestring);
		if (!tmpl)
			return (false);
		t_npc *clone = clone_npc(tmpl);
		if (!clone)
			return (false);
		npc_add_back(&room->npcs, clone);
	}
	return (true);
}

static bool	parse_rooms(cJSON *rooms_arr, t_world *world)
{
	cJSON	*room_json;

	if (!rooms_arr || !world)
		return (false);
	cJSON_ArrayForEach(room_json, rooms_arr)
	{
		if (!cJSON_IsObject(room_json))
			return (false);
		cJSON *id = cJSON_GetObjectItem(room_json, "id");
		cJSON *name = cJSON_GetObjectItem(room_json, "name");
		cJSON *description = cJSON_GetObjectItem(room_json, "description");
		cJSON *exits = cJSON_GetObjectItem(room_json, "exits");
		cJSON *items = cJSON_GetObjectItem(room_json, "items");
		cJSON *npcs = cJSON_GetObjectItem(room_json, "npcs");

		if (!cJSON_IsString(id) || !cJSON_IsString(name)
			|| !cJSON_IsString(description) || !cJSON_IsObject(exits)
			|| !cJSON_IsArray(items) || !cJSON_IsArray(npcs))
			return (false);

		t_room *res = calloc(1, sizeof(t_room));
		if (!res)
			return (false);
		res->id = strdup(id->valuestring);
		res->name = strdup(name->valuestring);
		res->description = strdup(description->valuestring);
		if (!res->id || !res->name || !res->description
			|| !parse_exits(exits, res)
			|| !parse_room_items(items, world, res)
			|| !parse_room_npcs(npcs, world, res))
		{
			free_rooms(res);
			return (false);
		}
		room_add_back(&world->rooms, res);
	}
	return (true);
}



static bool	validate_and_link_world(t_world *world)
{
	t_room	*curr_room;
	t_exit	*curr_exit;
	t_npc	*curr_npc;
	t_quest	*curr_quest;

	if (!world || !world->rooms)
		return (false);
	world->spawn_room = find_room_by_id(world->rooms, world->spawn_room_id);
	if (!world->spawn_room)
		return (false);
	curr_room = world->rooms;
	while (curr_room)
	{
		curr_exit = curr_room->exits;
		while (curr_exit)
		{
			curr_exit->target_room = find_room_by_id(world->rooms,
					curr_exit->target_room_id);
			if (!curr_exit->target_room)
				return (false);
			curr_exit = curr_exit->next;
		}
		curr_room = curr_room->next;
	}
	curr_npc = world->npc_catalog;
	while (curr_npc)
	{
		if (strcmp(curr_npc->role, "dialogue") != 0
			&& strcmp(curr_npc->role, "quest") != 0
			&& strcmp(curr_npc->role, "enemy") != 0)
			return (false);
		if (curr_npc->quest_id
			&& !find_quest_by_id(world->quests, curr_npc->quest_id))
			return (false);
		curr_npc = curr_npc->next;
	}
	curr_quest = world->quests;
	while (curr_quest)
	{
		if (curr_quest->count <= 0
			|| !find_item_by_id(world->item_catalog, curr_quest->reward_item_id))
			return (false);
		if (strcmp(curr_quest->type, "fetch") == 0)
		{
			if (!find_item_by_id(world->item_catalog, curr_quest->target_id))
				return (false);
		}
		else if (strcmp(curr_quest->type, "kill") == 0)
		{
			if (!find_npc_by_id(world->npc_catalog, curr_quest->target_id))
				return (false);
		}
		else
			return (false);
		curr_quest = curr_quest->next;
	}
	return (true);
}