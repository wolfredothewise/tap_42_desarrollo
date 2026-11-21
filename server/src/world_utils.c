# include "tap_world.h"



//destructores
void	free_items(t_item *node)
{
    while (node != NULL)
    {
        t_item *tmp;
        tmp = node->next;
        free(node->id);
        free(node->name);
        free(node->description);
        free(node);
        node = tmp;
    }
    return;

}

void	free_npcs(t_npc *node)
{
    while(node != NULL)
    {
        t_npc *tmp;
        tmp = node->next;
        free(node->id);
        free(node->name);
        free(node->role);
        free(node->dialogue);
        free(node->quest_id);
        free(node);
        node = tmp;
    }
    return;

}

void	free_quests(t_quest *node)
{
    while(node != NULL)
    {
        t_quest *tmp;
        tmp = node->next;
        free(node->id);
        free(node->description);
        free(node->type);
        free(node->target_id);
        free(node->reward_item_id);
        free(node);
        node = tmp;
    }
    return;

}

void	free_exits(t_exit *node)
{
    while(node != NULL)
    {
        t_exit *tmp;
        tmp = node->next;
        free(node->direction);
        free(node->target_room_id);
        free(node);
        node = tmp;
    }
    return;

}

void	free_rooms(t_room *node)
{
    while(node != NULL)
    {
        t_room *tmp;
        tmp = node->next;
        free(node->id);
        free(node->name);
        free(node->description);
        free_exits(node->exits);
        free_items(node->items);
        free_npcs(node->npcs);
        free(node);
        node = tmp;
    }
    return;

}

void	destroy_world(t_world *world)
{
    if(world != NULL)
    {
        free(world->spawn_room_id);
        free_rooms(world->rooms);
        free_quests(world->quests);
        free_items(world->item_catalog);
        free_npcs(world->npc_catalog);
        free(world);
    }
    return;
}

//clonadores de plantilla
t_item *clone_item(const t_item *tmpl)
{
    t_item *new;

    if (!tmpl)
        return (NULL);
    new = calloc(1, sizeof(t_item));//todos los punterios empioezan en NULL
    if (!new)
        return (NULL);
    new->id = strdup(tmpl->id);
    new->name = strdup(tmpl->name);
    new->description = strdup(tmpl->description);
    new->obtainable = tmpl->obtainable;
    if(!new->id || !new->name || !new->description)
        {   free_items(new);
            return NULL;
        }
    return new;
}

t_npc *clone_npc(const t_npc *tmpl)
{
    t_npc *new;
    if (!tmpl)
        return (NULL);
    new = calloc(1, sizeof(t_npc));//todos los punterios empioezan en NULL
    if (!new)
        return (NULL);
    new->id = strdup(tmpl->id);
    new->name = strdup(tmpl->name);
    new->role = strdup(tmpl->role);
    new->hp = tmpl->hp;
    new->max_hp = tmpl->max_hp;
    new->damage = tmpl->damage;
    new->dialogue = strdup(tmpl->dialogue);
    if (tmpl->quest_id == NULL)
        new->quest_id = NULL;
    else
        new->quest_id = strdup(tmpl->quest_id);
    if(!new->id || !new->name || !new->role || !new->dialogue || (tmpl->quest_id && !new->quest_id))
        {   
            free_npcs(new);
            return NULL;
        }
   
    return new;
}

//Inserción de elementos en listas
void item_add_back(t_item **head, t_item *new)
{
    if (!head || !new)
        return;
    if (*head == NULL)
        {
            *head = new;
            return;
        }
    t_item *curr = *head;
    while(curr != NULL)
    {
        if (curr->next == NULL)
            {
                curr->next = new;
                return;
            }
        curr = curr->next;
    } 
    return;
}

void npc_add_back(t_npc **head, t_npc *new)
{
    if (!head || !new)
        return;
    if (*head == NULL)
        {
            *head = new;
            return;
        }
    t_npc *curr = *head;
    while(curr != NULL)
    {
        if (curr->next == NULL)
            {
                curr->next = new;
                return;
            }
        curr = curr->next;
    }
    return;
}


void quest_add_back(t_quest **head, t_quest *new)
{
    if (!head || !new)
        return;
    if (*head == NULL)
        {
            *head = new;
            return;
        }
    t_quest *curr = *head;
    while(curr != NULL)
    {
        if (curr->next == NULL)
            {
                curr->next = new;
                return;
            }
        curr = curr->next;
    }
    return;
}


void exit_add_back(t_exit **head, t_exit *new)
{
    if (!head || !new)
        return;
    if (*head == NULL)
        {
            *head = new;
            return;
        }
    t_exit *curr = *head;
    while(curr != NULL)
    {
        if (curr->next == NULL)
            {
                curr->next = new;
                return;
            }
        curr = curr->next;
    }
    return;
}

//
void room_add_back(t_room **head, t_room *new)
{
    if (!head || !new)
        return;
    if (*head == NULL)
        {
            *head = new;
            return;
        }
    t_room *curr = *head;
    while(curr != NULL)
    {
        if (curr->next == NULL)
            {
                curr->next = new;
                return;
            }
        curr = curr->next;
    }
    return;
}

t_item *find_item_by_id(t_item *list, const char *id)
{
    if (!list || !list->id || !id)
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
    if (!list || !list->id || !id)
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
    if (!list || !list->id || !id)
        return NULL;
    while(list)
    {
        if (strcmp(list->id, id) == 0)
                return list;
        list = list->next;
    }
    return NULL;
}
t_quest *find_quest_by_id(t_quest *list, const char *id)
{
    if (!list || !list->id || !id)
        return NULL;
    while(list)
    {
        if (strcmp(list->id, id) == 0)
                return list;
        list = list->next;
    }
    return NULL;
}