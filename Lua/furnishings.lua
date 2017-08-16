-- Furnishing definitions

defaultColour = "black"
defaultHoverColour = "blue"

furnishingTypes = {
	door = {
		plan = {
			{ pos = {0,0}, sz = {2,1} }
		}
	},
	door90 = {
		plan = {
			{ pos = {0,0}, sz = {1,2} }
		}
	},
	barberschair = {
		plan = {
			{ pos = {0,0}, sz = {2,2} }
		},
		compatibleBehaviours = { "service:haircut" },
		interactionPoints = {
			civilian = {-1,0},
			staff = {0,-1}
		},
		cost = 20
	},
	-- sofa = {
	-- 	colour = "purple",
	-- 	hoverColour = "green",
	-- 	groundplan = {
	-- 		{0,0}, {1,0}
	-- 	},
	-- 	compatibleBehaviours = {  },
	-- 	interactionPoints = {  },
	-- 	cost = 5
	-- },
	-- piecounter = {
	-- 	colour = "red",
	-- 	hoverColour = "white",
	-- 	groundplan = {
	-- 		{0,0}, {1,0}, {2,0}
	-- 	},
	-- 	compatibleBehaviours = { "service:piesale" },
	-- 	interactionPoints = {
	-- 		staff = {1,-1},
	-- 		civilian = {1,1}
	-- 	},
	-- 	cost = 20
	-- }
}

numberOfAubergines = 18
