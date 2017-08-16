require("buildings")

--level properties
width  = 55
height = 45
startingBalance = 70
moneyLimit = 1200		--victory when reached
timeLimit = 300			--in seconds for now
allowedBuildings = {	-- buildings which are allowed to appear in the level
	"barber",
	"pieshop"
}

--level contents
normalShopLayout = {
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
	{0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1},
	{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2},
	{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3},
				  {2,4}, {3,4}
}
normalShopDoors = {
	{ 2,4, "downward" },
	{ 3,4, "downward" },
}
flippedShopLayout = {
	              {2,0}, {3,0},
	{0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1},
	{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2},
	{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}
}
flippedShopDoors = {
	{2,0, "upward"},
	{3,0, "downward"}
}
normalHouseLayout = {
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0},
	{0,1}, {1,1}, {2,1}, {3,1}, {4,1},
	{0,2}, {1,2}, {2,2}, {3,2}, {4,2},
	{0,3}, {1,3}, {2,3}, {3,3}, {4,3},
	              {2,4}
}
normalHouseDoors = {
	{2,4, "downward"}
}
flippedHouseLayout = {
	              {2,0},
	{0,1}, {1,1}, {2,1}, {3,1}, {4,1},
	{0,2}, {1,2}, {2,2}, {3,2}, {4,2},
	{0,3}, {1,3}, {2,3}, {3,3}, {4,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4}
}
flippedHouseDoors = {
	{2,0, "upward"}
}
rotated90HouseLayout = {
	       {1,0}, {2,0}, {3,0}, {4,0},
	       {1,1}, {2,1}, {3,1}, {4,1},
	{0,2}, {1,2}, {2,2}, {3,2}, {4,2},
	       {1,3}, {2,3}, {3,3}, {4,3},
	       {1,4}, {2,4}, {3,4}, {4,4}
}
rotated90HouseDoors = {
	{0,2, "leftward"}
}
rotated90ShopLayout = {
	{0,0}, {1,0}, {2,0}, {3,0},
	{0,1}, {1,1}, {2,1}, {3,1},
	{0,2}, {1,2}, {2,2}, {3,2}, {4,2},
	{0,3}, {1,3}, {2,3}, {3,3}, {4,3},
	{0,4}, {1,4}, {2,4}, {3,4},
	{0,5}, {1,5}, {2,5}, {3,5}
}
rotated90ShopDoors = {
	{4,2, "rightward"},
	{4,3, "rightward"}
}
rotated270ShopLayout = {
	       {1,0}, {2,0}, {3,0}, {4,0},
	       {1,1}, {2,1}, {3,1}, {4,1},
	{0,2}, {1,2}, {2,2}, {3,2}, {4,2},
	{0,3}, {1,3}, {2,3}, {3,3}, {4,3},
	       {1,4}, {2,4}, {3,4}, {4,4},
	       {1,5}, {2,5}, {3,5}, {4,5}
}
rotated270ShopDoors = {
	{0,2, "leftward"},
	{0,3, "leftward"}
}


buildings = {

	-- FIRST STREET
	{ x = 2,  y = 2, ["type"] = "barber",  groundplan = normalShopLayout,  doors = normalShopDoors },
	{ x = 9,  y = 2, ["type"] = "pieshop", groundplan = normalShopLayout,  doors = normalShopDoors },
	{ x = 16, y = 2, ["type"] = "home",    groundplan = normalHouseLayout, doors = normalHouseDoors },
	{ x = 22, y = 2, ["type"] = "barber",  groundplan = normalShopLayout,  doors = normalShopDoors },
	{ x = 29, y = 2, ["type"] = "home",    groundplan = normalHouseLayout, doors = normalHouseDoors },
	{ x = 35, y = 2, ["type"] = "pieshop", groundplan = normalShopLayout,  doors = normalShopDoors },
	
	{ x = 2,  y = 10, ["type"] = "home",    groundplan = flippedHouseLayout, doors = flippedHouseDoors },
	{ x = 8,  y = 10, ["type"] = "pieshop", groundplan = flippedShopLayout,  doors = flippedShopDoors },
	{ x = 15, y = 10, ["type"] = "home",    groundplan = flippedHouseLayout, doors = flippedHouseDoors },
	{ x = 21, y = 10, ["type"] = "barber",  groundplan = flippedShopLayout,  doors = flippedShopDoors },
	{ x = 28, y = 10, ["type"] = "home",    groundplan = flippedHouseLayout, doors = flippedHouseDoors },
	{ x = 34, y = 10, ["type"] = "barber",  groundplan = flippedShopLayout,  doors = flippedShopDoors },
	
	-- WALL
	{
		x = 2, y = 16, ["type"] = "home", groundplan = {
	 			{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0}, {6,0}, {7,0}, {8,0}, {9,0}, {10,0},       {12,0}, {13,0}, {14,0}, {15,0}, {16,0}, {17,0}, {18,0}, {19,0}, {20,0}, {21,0}, {22,0}, {23,0}, {24,0}, {25,0}, {26,0}, {27,0}, {28,0},     {30,0}, {31,0}, {32,0}, {33,0}, {34,0}, {35,0}, {36,0}, {37,0}, {38,0},
			{0,1},                 {10,1},    {12,1},    {20,1},    {28,1},    {30,1},
			{0,2},                 {10,2},    {12,2},    {20,2},    {28,2},    {30,2},
			{0,3}, {1,3}, {2,3},   {10,3},    {12,3},    {20,3},    {28,3},    {30,3},      {38,1},
			                       {10,4},    {12,4},    {20,4},    {28,4},    {30,4},      {38,2},
			{0,5}, {1,5}, {2,5},   {10,5},    {12,5},    {20,5},    {28,5},    {30,5},
			{0,6},                 {10,6},    {12,6},    {20,6},    {28,6},    {30,6},      {38,7},
			{0,7},                 {10,7},    {12,7},    {20,7},    {28,7},    {30,7},      {38,8},
			                                                                   {30,8}, {30,9}, {31,9}, {32,9}, {33,9}, {34,9}, {35,9}, {36,9}, {37,9}, {38, 9},
			{0,8}, {1,8}, {2,8}, {3,8}, {4,8}, {5,8}, {6,8}, {7,8}, {8,8}, {9,8},          {10,8},

		},
		doors = { }
	},
	
	-- WALLED-IN HOUSES
	{ x = 6,  y = 18, ["type"] = "home",   groundplan = rotated90HouseLayout, doors = rotated90HouseDoors },
	{ x = 16, y = 18, ["type"] = "home",   groundplan = normalHouseLayout,    doors = normalHouseDoors },
	{ x = 24, y = 18, ["type"] = "home",   groundplan = normalHouseLayout,    doors = normalHouseDoors },
	{ x = 34, y = 18, ["type"] = "pieshop", groundplan = rotated90ShopLayout,  doors = rotated90ShopDoors },
	
	-- WALL THE 2ND
	{
		x = 42, y = 2, ["type"] = "home",
		groundplan = {
			{0,0},
			{0,1},
			{0,2},
			{0,3},
			{0,4},
			{0,5},
			
			{0,7},
			{0,8},
			{0,9},
			{0,10},
			{0,11},
			{0,12},
			
			{0,14},
			{0,15},
			{0,16},
			{0,17},
			{0,18},
			{0,19},
			{0,20},

			{0,22},
			{0,23},
			{0,24},
			{0,25},
			{0,26},
			{0,27},
		},
		doors = { }
	},
	
	-- SMALL RIGHTWARD GANGLION OF SHOPS
	{ x = 44, y = 2,  ["type"] = "pieshop", groundplan = normalShopLayout, doors = normalShopDoors },
	{ x = 48, y = 8,  ["type"] = "barber", groundplan = rotated270ShopLayout, doors = rotated270ShopDoors },
	{ x = 44, y = 15, ["type"] = "home", groundplan = flippedHouseLayout, doors = flippedHouseDoors },
	{ x = 48, y = 22, ["type"] = "barber", groundplan = normalShopLayout, doors = normalShopDoors },
	{ x = 48, y = 31, ["type"] = "pieshop", groundplan = flippedShopLayout, doors = flippedShopDoors },
	-- { x = 34, y = 18, ["type"] = "pieshop", groundplan = rotated90ShopLayout,  doors = rotated90ShopDoors },
	
	-- BOTTOM ROW OF BUILDINGS
	
	{ x = 2, y = 31,  ["type"] = "home", groundplan = flippedHouseLayout, doors = flippedHouseDoors },
	{ x = 9, y = 32,  ["type"] = "pieshop", groundplan = normalShopLayout, doors = normalShopDoors },
	{ x = 17, y = 31,  ["type"] = "barber", groundplan = flippedShopLayout, doors = flippedShopDoors },
	{ x = 25, y = 32,  ["type"] = "home", groundplan = normalHouseLayout, doors = normalHouseDoors },
	{ x = 32, y = 31,  ["type"] = "home", groundplan = flippedHouseLayout, doors = flippedHouseDoors },
	{ x = 39, y = 32,  ["type"] = "pieshop", groundplan = normalShopLayout, doors = normalShopDoors },
	-- { x = 48, y = 8,  ["type"] = "barber", groundplan = rotated270ShopLayout, doors = rotated270ShopDoors },
	
}

spawnPoints = {
	{ x = 4,  y = 13, name = "something" },
	{ x = 18, y = 4, name = "something" },
	{ x = 17, y = 13, name = "something" },
	{ x = 31, y = 4, name = "something" },
	{ x = 30, y = 13, name = "something" },
	{ x = 8,  y = 20, name = "something" },
	{ x = 18, y = 20, name = "something" },
	{ x = 26, y = 20, name = "something" },
	{ x = 46, y = 18, name = "something" },
	{ x = 34, y = 34, name = "something" },
	{ x = 27, y = 34, name = "something" },
	{ x = 4,  y = 34, name = "something" }
}

-- spawnPoints = {
-- 	{
-- 		name = "OffTheEdgeOfTheMap1EndNameHere",
-- 		x = 0,
-- 		y = 15,
-- 		-- rate = 70,
-- 		-- types = {
-- 		-- 	{ Peasants, 5 },
-- 		-- 	{ Judges, 8 },
-- 		-- },
-- 	},
-- 	{
-- 		name = "InAHouse",
-- 		x = 33,
-- 		y = 11,
-- 		-- rate = 80,
-- 		-- types = {
-- 		-- 	{ Peasants, 5 },
-- 		-- 	{ Judges, 8 },
-- 		-- },
-- 	},
-- 	{
-- 		name = "InAHouse1",
-- 		x = 33,
-- 		y = 21,
-- 		-- rate = 90,
-- 		-- types = {
-- 		-- 	{ Peasants, 5 },
-- 		-- 	{ Judges, 8 },
-- 		-- },
-- 	}
-- }
