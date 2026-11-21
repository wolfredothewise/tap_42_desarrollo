#include <stdio.h>
#include "tap_world.h"



int	main(int argc, char **argv)
{
	t_world	*world;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <world_json_path>\n", argv[0]);
		return (1);
	}
	world = load_world(argv[1]);
	if (!world)
	{
		fprintf(stderr, "Error: Failed to load world from %s\n", argv[1]);
		return (1);
	}
	printf("World loaded! Spawn room: %s (%s)\n",
		world->spawn_room->name, world->spawn_room->id);
	destroy_world(world);
	return (0);
}