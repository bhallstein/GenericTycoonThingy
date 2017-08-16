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

-- level contents
buildings = {
	{
		["type"] = "shop",
		position = {2,2},
		plan = {
			{ pos = {0,0}, sz = {5,3} }
		}
	},
	{
		["type"] = "shop",
		position = {2,10},
		plan = {
			{ pos = {0,0}, sz = {5,3} }
		}
	}
}

furnishings = {
	{ ["type"] = "door90", position = {4,4}	 },
	{ ["type"] = "door90", position = {4,12} }
}

spawnPoints = {
	{ pos = {4, 13} },
	{ pos = {18, 4} }
}
