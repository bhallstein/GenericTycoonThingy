function overrideSettings(args)
	--[[
	this function checks all valid arguments for a given setting
	by trying to getglobal() on each one.
	if getglobal(arg) != nil then we have an override :)
	
	note that we process arguments in the order they exist in the setting's args table
	if multiple arguments for the same setting are registered, the later occurrence
	from the arg table takes precedence.
	
	also, we check CFG file first, then command line, to maintain a sensible order of
	override precedence for both methods.
	--]]
	local val = nil --this will be our return value
	
	--
	if args ~= nil then --make sure there are arguments, else return
		if type(args)=="string" then --only one argument
			val = _G[args] --always set val, since no further values to override, and nil is allowed
		else --table
			for i=1,#args do --for 1 - args.size
				if _G[args[i]] ~= nil then --access the global variable with the name of this argument
					val = _G[args[i]] --set it to val if it's not nil
				end
			end
		end
	end
	print(val)
	return val --val will still be nil if no globals existed, and the setting will default! :)
end

settings = {

	--graphics
	ScreenWidth = {
		args = "screenwidth", --valid override arguments
		value = 1280 --overrideSettings(args) or 800, --if the override returns nil, the saved setting will be used
	},
	ScreenHeight = {
		value = 720,
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
