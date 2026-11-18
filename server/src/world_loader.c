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
//Objetivo: Localizar un nodo dentro de una lista enlazada comparando su identificador canónico con id.
// Retorno: El puntero al nodo encontrado, o NULL si no existe.
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
t_room *find_room_by_id(t_room *list, const char *id);
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
    
}
