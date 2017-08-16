-- Unit definitions

defaultColour = "transparent black"
defaultHoverColour = "blue"

unitTypes = {
	civilian = {
		compatibleBehaviours = {
			"seek:haircut", "service:haircut",
			"seek:pie", "service:piesale"
		}
	},
	staff = {
		colour = "green",
		hoverColour = "white",
		isStaff = true,
		hireCost = 10,
		compatibleBehaviours = {
			"service:haircut", "service:piesale"
		}
	}
}

numberOfAubergines = 6
