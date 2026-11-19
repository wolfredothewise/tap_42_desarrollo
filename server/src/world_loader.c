#include "tap_server.h"
#include "tap_world.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    



    cJSON_Delete(root);
    return world;
}

t_item *find_item_by_id(t_item *list, const char *id)
{
    if (!list->id || !id)
        return NULL;
    while(list)
    {
        if (strcmp(list->id, id) == 0)
                return list;
        list = list->next;
    }
    return NULL;
}

t_npc *find_npc_by_id(t_npc *list, const char *id)
{
    if (!list->id || !id)
        return NULL;
    while(list)
    {
        if (strcmp(list->id, id) == 0)
                return list;
        list = list->next;
    }
    return NULL;
}
t_room *find_room_by_id(t_room *list, const char *id)
{
    if (!list->id || !id)
        return NULL;
    while(list)
    {
        if (strcmp(list->id, id) == 0)
                return list;
        list = list->next;
    }
    return NULL;
}

//parser
// Objetivo: Extraer los datos del árbol cJSON y
//  poblar world->item_catalog, world->npc_catalog, world->quests y world->rooms 
//  (incluyendo las salidas, ítems y NPCs iniciales de cada sala). 
//  Conéctalas dentro de load_world antes de destruir root.   
//  Retorno: true si todo se carga y valida correctamente; false si falta algún dato o 
//  hay cualquier incongruencia en el mapa
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
        cJSON *max_hp = cJSON_GetObjectItem(npc_json, "max_hp");
        cJSON *damage = cJSON_GetObjectItem(npc_json, "damage");
        cJSON *dialogue = cJSON_GetObjectItem(npc_json, "dialogue");
        cJSON *quest_id = cJSON_GetObjectItem(npc_json, "quest_id");


        if (!cJSON_IsString(id) || !cJSON_IsString(name) ||
         !cJSON_IsString(role) ||  !cJSON_IsString(dialogue) ||
          !cJSON_IsString(quest_id) || !cJSON_IsNumber(hp) ||
           !cJSON_IsNumber(max_hp) || !cJSON_IsNumber(damage))
            return false;
        
        t_npc *res = calloc(1, sizeof(t_npc));
        if (!res)
            return false;
        res->id = strdup(id->valuestring);
        res->name = strdup(name->valuestring);
        res->role = strdup(role->valuestring);
        res->hp = hp->valueint;
        res->max_hp = max_hp->valueint;
        res->damage = damage->valueint;
        res->dialogue = strdup(dialogue->valuestring);
        res->quest_id = strdup(quest_id->valuestring);

        if(!res->id || !res->name ||
        !res->role || !res->dialogue ||
        !res->quest_id)
        {
            free_npcs(res);
            return false;
        }
        
        npc_add_back(&world->npc_catalog, res);
    }

    return true;
}