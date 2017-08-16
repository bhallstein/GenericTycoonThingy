-- Behaviours

seekBehaviours = {
	["seek:pie"] = {
		requisiteBuildingType = "pieshop",
		followingBehaviour = "service:piesale"
	},
	["seek:haircut"] = {
		requisiteBuildingType = "barber",
		followingBehaviour = "service:haircut"
	}
}

serviceBehaviours = {
	["service:piesale"] = {
		charge = 2
	},
	["service:haircut"] = {
		charge = 3
	}
}

numberOfAubergines = 15
