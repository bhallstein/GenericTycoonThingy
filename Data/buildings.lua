require([[buildingactions]])

function barber()
	building = {}
	
	building.type = "Barber"
		
	--colours
	building.colour = {
		r = 0,
		g = 0,
		b = 255,
		a = 255,
	}
	
	return building
	
end

function pieshop()
	building = {}
	
	building.type = "Pie Shop"
	
	--colours
	building.colour = {
		r = 255,
		g = 0,
		b = 0,
		a = 255,
	}
	
	building.sateDesires = {
		"hairyness"
	}
	
	building.actions = {
		shave(),
		kill(),
	}
	
	return building
	
end

function home()
	building = {}
	
	building.type = "Home"
	
	--colours
	building.colour = {
		r = 0,
		g = 0,
		b = 0,
		a = 255,
	}
	
	return building

end

function derelict()
	building = {}
	
	building.type = "Derelict"
	
	--colours
	building.colour = {
		r = 255,
		g = 255,
		b = 255,
		a = 255,
	}
	
	return building

end

function asylum()
	building = {}
	
	building.type = "Asylum"
	
	--colours
	building.colour = {
		r = 120,
		g = 0,
		b = 200,
		a = 255,
	}
	
	return building

end