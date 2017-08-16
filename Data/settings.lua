settings = {

	--graphics
	ScreenWidth = {
		args = "screenwidth", --valid override arguments
		value = 800 --overrideSettings(args) or 800, --if the override returns nil, the saved setting will be used
	},
	ScreenHeight = {
		value = 600,
	},
	FullScreen = {
		value = false,
	},
	FramerateLimit = {
		value = 60,
	},
}

--include CFG
--require("dbt.cfg")

--process any arguments in the CFG or on the command line (command line overrides CFG)
