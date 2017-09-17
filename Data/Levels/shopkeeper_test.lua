-- a save game

levelData = {
	monetaryTarget = 1200,
	timeLimit = 300
}

playerState = {
	balance = 10
}

mapData = {
	width = 30,
	height = 20,
	
	buildings = {
		{
			rct = {
				position = { x=13,y=9,a=0,b=0 },
				size = { width=7, height=5 },
			},
			uid = 348756,
			type = "shop",
			destroyed = false,
		}
	},
	
	units = {
		{
			destroyed = false,
			rct = {
				position = { x=4,y=10,a=0,b=0 },
				size = { width=1,height=1 }
			},
			type = "shopkeeper",
			uid = 79,
			controller = 104,
			dest = { x=4,y=10,a=0,b=0 },
			hired = false,
			mode = "IDLE",
			route = {  }
		},
		{
			destroyed = false,
			rct = {
				position = { x=10,y=10,a=0,b=0 },
				size = { width=1,height=1 }
			},
			type = "customer",
			uid = 80,
			controller = 2,
			dest = { },
			hired = false,
			mode = "IDLE",
			route = {  }
		},
	},
	furnishings = {
		{
			destroyed = false,
			rct = {
				position = { x=0,y=0,a=0,b=0 },
				size = { width=2,height=2 }
			},
			type = "barberschair",
			uid = 3,
			purchased = true
		}
	},
	spawnPoints = {
		
	},
	controllers = {
		{
			destroyed = false,
			rct = {
				position = { x=0,y=0,a=0,b=0 },
				size = { width=0,height=0 }
			},
			type = "CustomerController",
			uid = 2,
			passbackMap = {  },
			customer = 80,
			failureStage = 100,
			stage = 3,
			timeWaited = 100
		},
		{
			destroyed = false,
			rct = {
				position = { x=0,y=0,a=0,b=0 },
				size = { width=0,height=0 }
			},
			type = "ShopkeeperController",
			uid = 104,
			passbackMap = {  },
			shopkeeper = 79,
			failureStage = 100,
			stage = 1,
			timeWaited = 100
		}
	},
	inactiveControllers = {
		
	}
}
