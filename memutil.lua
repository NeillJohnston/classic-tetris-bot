local exports = {}

local ram = {
	nextpieceid = 0x00BF,
	pieceid = 0x0042,
	x = 0x0040,
	y = 0x0041,
	phase = 0x0048
}

local function readgrid()
	local grid = {}
	for i=1,20 do
		grid[i] = {}
		for j=1,10 do
			block = (memory.readbyte(0x0400 + (j-1) + 10*(i-1)) ~= 239)
			grid[i][j] = block
		end
	end

	return grid
end

exports.ram = ram
exports.readgrid = readgrid

return exports
