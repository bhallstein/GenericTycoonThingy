-- Unit definitions

defaultColour = "black"
defaultHoverColour = "blue"

unitTypes = {
	civilian = {
		compatibleBehaviours = {
			"seekhaircut", "havehaircut",
			"seekpie", "piesale"
		}
	},
	staff = {
		colour = "green",
		hoverColour = "white",
		isStaff = true,
		hireCost = 10,
		compatibleBehaviours = {
			"havehaircut",
			"piesale"
		}
	}
}

numberOfAubergines = 6
