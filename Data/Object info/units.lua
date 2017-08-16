-- Unit type definitions

unitTypes = {
	customer = {
		compatibleBehaviours = {
			"seek:haircut", "service:haircut",
			"seek:pie", "service:piesale"
		}
	},
	shopkeeper = {
		isStaff = true,
		hireCost = 10,
		compatibleBehaviours = {
			"service:haircut", "service:piesale"
		}
	}
}

numberOfAubergines = 6
