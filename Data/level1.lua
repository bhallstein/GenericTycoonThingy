require("buildings")

--level properties
width  = 50
height = 40
startingBalance = 70
allowedBuildings = {	-- buildings which are allowed to appear in the level
	"barber",
	"pieshop"
}

--level contents
buildings = {
	{
		["type"] = "barber",
		x = 5,
		y = 5,
		groundplan = {
			{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
			{0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1}, {6,1},
			{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2},
			{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3},
						  {2,4}, {3,4}
		},
		doors = {
			{ 2,4, "downward" },
			{ 3,4, "downward" },
			{ 6,1, "rightward" }
		}
	},
	{
		["type"] = "pieshop",
		x = 11,
		y = 14,
		groundplan = {
			{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0}, {6,0}, {7,0}, {8,0},
			       {1,1}, {2,1}, {3,1}, {4,1}, {5,1}, {6,1}, {7,1}, {8,1},
			       {1,2}, {2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2}, {8,2},
			       {1,3}, {2,3},                      {6,3}, {7,3}, {8,3}, {9,3},
			       {1,4}, {2,4}, {3,4},                      {7,4}, {8,4},
			       {1,5}, {2,5}, {3,5}, {4,5}, {5,5}, {6,5}, {7,5}, {8,5},
			       {1,6}, {2,6}, {3,6}, {4,6}, {5,6}, {6,6}, {7,6}, {8,6},
			       {1,7}, {2,7}, {3,7}, {4,7}, {5,7}, {6,7}, {7,7}, {8,7},
								 {4,8}, {5,8}
		},
		doors = {
			{ 4,8, "downward" },
			{ 5,8, "downward" },
			{ 2,3, "rightward" },
			{ 7,4, "leftward" },
			{ 9,3, "rightward" },
			{ 0,0, "leftward" },
			{ 0,0, "upward" }
		}
	},
	{
		["type"] = "home",
		x = 32,
		y = 10,
		groundplan = {
			       {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
			       {1,1}, {2,1}, {3,1}, {4,1}, {5,1},
			{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2},
			{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3},
			       {1,4}, {2,4}, {3,4}, {4,4}, {5,4},
			       {1,5}, {2,5}, {3,5}, {4,5}, {5,5},
		},
		doors = {
			{ 0,2, "leftward" },
			{ 0,3, "leftward" }
		}
	},
	{
		["type"] = "home",
		x = 32,
		y = 20,
		groundplan = {
			       {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
			       {1,1}, {2,1}, {3,1}, {4,1}, {5,1},
			{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2},
			{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3},
			       {1,4}, {2,4}, {3,4}, {4,4}, {5,4},
			       {1,5}, {2,5}, {3,5}, {4,5}, {5,5},
		},
		doors = {
			{ 0,2, "leftward" },
			{ 0,3, "leftward" }
		}
	},
	{
		["type"] = "asylum",
		x = 20,
		y = 5,
		groundplan = {
			{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
			{0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1},
			{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2},
			{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3},
						  {2,4}, {3,4}
		},
		doors = {
			{ 2,4, "downward" },
			{ 3,4, "downward" },
		}
	},
}
spawnPoints = {
	{
		name = "OffTheEdgeOfTheMap1EndNameHere",
		x = 0,
		y = 15,
		rate = 70,
		types = {
			{ Peasants, 5 },
			{ Judges, 8 },
		},
	},
	{
		name = "InAHouse",
		x = 33,
		y = 11,
		rate = 80,
		types = {
			{ Peasants, 5 },
			{ Judges, 8 },
		},
	},
	{
		name = "InAHouse1",
		x = 33,
		y = 21,
		rate = 90,
		types = {
			{ Peasants, 5 },
			{ Judges, 8 },
		},
	}
}
