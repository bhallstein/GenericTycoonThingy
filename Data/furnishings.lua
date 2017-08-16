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
		}
	},
	sofa = {
		colour = "purple",
		hoverColour = "green",
		groundplan = {
			{0,0}, {1,0}
		},
		compatibleBehaviours = {  },
		interactionPoints = {  }
	},
	piecounter = {
		colour = "brown",
		hoverColour = "white",
		groundplan = {
			{0,0}, {1,0}, {2,0}
		},
		compatibleBehaviours = { "piesale" },
		interactionPoints = {  }
	}
}

numberOfAubergines = 18
