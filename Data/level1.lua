require("buildings")

--level properties
width=100
height=80
allowedBuildings = {	-- buildings which are allowed to appear in the level
	"barber",
	"pieshop"
}

--level contents
buildings = {
	{
		["type"] = "barber",
		x = 1,
		y = 2,
	},
	{
		["type"] = "pieshop",
		x = 12,
		y = 15,
	},
	{
		["type"] = "home",
		x = 28,
		y = 31,
	},
	{
		["type"] = "home",
		x = 37,
		y = 18,
	}
}
spawnPoints = {
	{
		name = "OffTheEdgeOfTheMap1EndNameHere",
		x = 0,
		y = 0,
		rate = 30,
		types = {
			{ Peasants, 5 },
			{ Judges, 8 },
		},
	},
	{
		name = "InAHouse",
		x = 37,
		y = 18,
		rate = 20,
		types = {
			{ Peasants, 5 },
			{ Judges, 8 },
		},
	},
	{
		name = "InAHouse1",
		x = 12,
		y = 15,
		rate = 40,
		types = {
			{ Peasants, 5 },
			{ Judges, 8 },
		},
	},
	{
		name = "InAHouse2",
		x = 1,
		y = 2,
		rate = 30,
		types = {
			{ Peasants, 5 },
			{ Judges, 8 },
		},
	},
}
