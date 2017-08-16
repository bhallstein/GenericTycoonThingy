-- Furnishing definitions

defaultColour = "black"
defaultHoverColour = "blue"

furnishingTypes = {
	barberschair = {
		groundplan = {
			{0,0}, {1,0},
			{0,1}, {1,1}
		},
		compatibleBehaviours = { "shave", "havehaircut" },
		interactionPoints = {
			civilian = {-1,0},
			staff = {0,-1}
		},
		cost=20
	},
	sofa = {
		colour = "purple",
		hoverColour = "green",
		groundplan = {
			{0,0}, {1,0}
		},
		compatibleBehaviours = {  },
		interactionPoints = {  },
		cost=5
	},
	piecounter = {
		colour = "red",
		hoverColour = "white",
		groundplan = {
			{0,0}, {1,0}, {2,0}
		},
		compatibleBehaviours = { "piesale" },
		interactionPoints = {
			staff = {1,-1},
			civilian = {1,1}
		},
		cost=20
	}
}

numberOfAubergines = 18
