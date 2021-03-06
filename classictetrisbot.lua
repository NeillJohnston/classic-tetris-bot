local winapi = require("winapi")
local guiutil = require("src/emu-util/gui")
local memutil = require("src/emu-util/memory")
local ram = memutil.ram
local pieceutil = require("src/emu-util/piece")

-- Command-line args
local guidebug = false
local botname = "random"

if arg then
	for x in arg:gmatch("%S+") do
		if x:sub(1,1) == "-" then
			if x == "-d" then
				print("GUI debug on.")
				guidebug = true
			end
		else
			botname = x
		end
	end
end
if botname == "random" then
	print("No bot name supplied, using random instead.")
end

-- Main 
-- emu.speedmode("normal");

local process, pipe = winapi.spawn_process("bin/bot " .. botname)
emu.registerexit(function()
	pipe:write("20\n")
	process:kill()
	print("Stoping.")
end)
-- Initial message
print(pipe:read())

local states = {
	menu1 = -1,
	menu2 = 0,
	deciding = 1,
	reading = 2,
	placing1 = 3,
	placing2 = 4,
	waiting = 5
}
local state = states.menu1
local decision = nil
local hint = nil

local function blankjoypad()
	return {
		up=false, down=false, left=false, right=false,
		A=false, B=false, start=false, select=false
	}
end

while true do
	local x = memory.readbyte(ram.x)
	local y = memory.readbyte(ram.y)
	local pieceid = memory.readbyte(ram.pieceid)
	local nextpieceid = memory.readbyte(ram.nextpieceid)
	local phase = memory.readbyte(ram.phase)

	if guidebug and hint then
		guiutil.drawpiece(hint, "green")
	end

	if state == states.menu1 then
		local input = blankjoypad()
		input.A = true
		joypad.write(1, input)

		state = states.menu2
	
	elseif state == states.menu2 then
		local input = blankjoypad()
		input.A = true
		input.start = true
		joypad.write(1, input)

		state = states.waiting
	
	elseif state == states.deciding then
		pipe:write(string.format("%d\n", pieceid))
		pipe:write(string.format("%d\n", nextpieceid))
		for i=0,19 do
			local line = 0
			for j=0,9 do
				line = line * 2
				if memory.readbyte(0x0400 + (9-j) + 10*i) ~= 239 then
					line = line + 1
				end
			end
			pipe:write(string.format("%d\n", line))
		end

		state = states.reading

	elseif state == states.reading then
		decisionstring = pipe:read()
		local temp = {}
		for token in decisionstring:gmatch("[^ ]+") do
			table.insert(temp, token)
		end
		decision = {
			pieceid = tonumber(temp[1]),
			x = tonumber(temp[2]),
			y = tonumber(temp[3]),
			action = tonumber(temp[4])
		}
		state = states.placing1

		if guidebug then
			local x = decision.x + 1
			local y = decision.y + 1
			local pieceid = decision.pieceid + 1
			local action = decision.action

			if action == 1 then
				x = x - 1
			elseif action == 2 then
				x = x + 1
			elseif action == 3 then
				pieceid = pieceutil.rotateleft[pieceid]
			elseif action == 4 then
				pieceid = pieceutil.rotateright[pieceid]
			end

			hint = pieceutil.makepiece(x, y, pieceid)
		end	

	elseif state == states.placing1 then
		if decision then
			local input = blankjoypad()
			if x > decision.x then
				input.left = true
			elseif x < decision.x then
				input.right = true
			end
			if pieceid ~= decision.pieceid then
				input.A = true
			end
			if y == decision.y then
				if decision.action == 1 then
					input.left = true
				elseif decision.action == 2 then
					input.right = true
				elseif decision.action == 3 then
					input.B = true
				elseif decision.action == 4 then
					input.A = true
				end

				decision = nil
			end

			state = states.placing2
			joypad.write(1, input)
		end

		if phase ~= 1 then
			decision = nil
			state = states.waiting
		end
	
	elseif state == states.placing2 then
		local input = blankjoypad()

		if decision then
			if y == decision.y then
				if decision.action == 1 then
					input.left = true
				elseif decision.action == 2 then
					input.right = true
				elseif decision.action == 3 then
					input.B = true
				elseif decision.action == 4 then
					input.A = true
				end

				decision = nil
			end

			joypad.write(1, input)
			state = states.placing1
		end

		if phase ~= 1 then
			decision = nil
			state = states.waiting
		end

	elseif state == states.waiting then
		if phase == 1 then
			state = states.deciding
		end
	end

	emu.frameadvance()
end

process:kill()
