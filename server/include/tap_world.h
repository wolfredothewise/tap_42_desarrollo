#ifndef TAP_WORLD_H
# define TAP_WORLD_H


# include <stdbool.h>
# include <stdlib.h>
# include <string.h>


typedef struct s_room t_room;

typedef struct s_item
{
	char			*id;
	char			*name;
	char			*description;
	bool			obtainable;
	struct s_item	*next;
}	t_item;

typedef struct s_npc
{
	char			*id;
	char			*name;
	char			*role;
	int				hp;
	int				max_hp;
	int				damage;
	char			*dialogue;
	char			*quest_id;
	struct s_npc	*next;
}	t_npc;

typedef struct s_quest
{
	char			*id;
	char			*description;
	char			*type;
	char			*target_id;
	int				count;
	char			*reward_item_id;
	struct s_quest	*next;
}	t_quest;

typedef struct s_exit
{
	char			*direction;
	char			*target_room_id;
	t_room			*target_room;
	struct s_exit	*next;
}	t_exit;

typedef struct s_room
{
	char			*id;
	char			*name;
	char			*description;
	t_exit			*exits;
	t_item			*items;
	t_npc			*npcs;
	struct s_room	*next;
}	t_room;

typedef struct s_world
{
	char			*spawn_room_id;
	t_room			*spawn_room;
	t_room			*rooms;
	t_quest			*quests;
	t_item			*item_catalog;
	t_npc			*npc_catalog;
}	t_world;


//ver los return bien

//Prototipos de world_utils.c

//destructores
void	free_items(t_item *node);
void	free_npcs(t_npc *node);
void	free_quests(t_quest *node);
void	free_exits(t_exit *node);
void	free_rooms(t_room *node);
void	destroy_world(t_world *world);

//clonadores de Plantillas
t_item *clone_item(const t_item *tmpl);
t_npc *clone_npc(const t_npc *tmpl);	

//Inserción de elementos en listas
void item_add_back(t_item **head, t_item *new);
void npc_add_back(t_npc **head, t_npc *new);
void quest_add_back(t_quest **head, t_quest *new);
void exit_add_back(t_exit **head, t_exit *new);
void room_add_back(t_room **head, t_room *new);

//Load world
t_world *load_world(const char *filepath);

	//buscadores por IDS
t_item *find_item_by_id(t_item *list, const char *id);
t_npc *find_npc_by_id(t_npc *list, const char *id);
t_room *find_room_by_id(t_room *list, const char *id);
//parser de entidades y salas static

// static bool parse_items(cJSON *items_arr, t_world *world);

// static bool parse_npcs(cJSON *npcs_arr, t_world *world);

// static bool parse_quests(cJSON *quests_arr, t_world *world);

// static bool parse_rooms(cJSON *rooms_arr, t_world *world);

#endif