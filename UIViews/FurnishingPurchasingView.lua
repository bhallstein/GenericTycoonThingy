-- FurnishingPurchasingView - furnishing purchasing view
-- not done yet: previously populated from furnishingTypes vector

for (int i=0; i < furnishingTypes->size(); i++) {
	std::string s("buy furnishing ");
	s += furnishingTypes->at(i);
	W::rect r = {
		W::position(7 + (20+10)*i, 30),
		W::size(20, 20)
	};
	buttons.push_back(new W::Button(r, s.c_str()));
}

landscape = {
	
	["100"] = {
		size = "140,220",
		position = "47,47",
		positioning_corner = "top left",
		draggable = true,
		elements = {
			OpenHiringViewBtn = {
				position = "7,30",
				size = "20,20",
				["type"] = "button",
				positioning_corner = "top left"
			}
		}
	}
	
}
