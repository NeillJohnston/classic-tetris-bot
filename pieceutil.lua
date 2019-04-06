local exports = {}

local pieces = {
	{{x=0, y=-1}, {x=-1, y=0}, {x=0, y=0}, {x=1, y=0}},
	{{x=0, y=-1}, {x=0, y=0}, {x=1, y=0}, {x=0, y=1}},
	{{x=-1, y=0}, {x=0, y=0}, {x=1, y=0}, {x=0, y=1}},
	{{x=0, y=-1}, {x=-1, y=0}, {x=0, y=0}, {x=0, y=1}},
	{{x=0, y=-1}, {x=0, y=0}, {x=-1, y=1}, {x=0, y=1}},
	{{x=-1, y=-1}, {x=-1, y=0}, {x=0, y=0}, {x=1, y=0}},
	{{x=0, y=-1}, {x=1, y=-1}, {x=0, y=0}, {x=0, y=1}},
	{{x=-1, y=0}, {x=0, y=0}, {x=1, y=0}, {x=1, y=1}},
	{{x=-1, y=0}, {x=0, y=0}, {x=0, y=1}, {x=1, y=1}},
	{{x=1, y=-1}, {x=0, y=0}, {x=1, y=0}, {x=0, y=1}},
	{{x=-1, y=0}, {x=0, y=0}, {x=-1, y=1}, {x=0, y=1}},
	{{x=0, y=0}, {x=1, y=0}, {x=-1, y=1}, {x=0, y=1}},
	{{x=0, y=-1}, {x=0, y=0}, {x=1, y=0}, {x=1, y=1}},
	{{x=0, y=-1}, {x=0, y=0}, {x=0, y=1}, {x=1, y=1}},
	{{x=-1, y=0}, {x=0, y=0}, {x=1, y=0}, {x=-1, y=1}},
	{{x=-1, y=-1}, {x=0, y=-1}, {x=0, y=0}, {x=0, y=1}},
	{{x=1, y=-1}, {x=-1, y=0}, {x=0, y=0}, {x=1, y=0}},
	{{x=0, y=-2}, {x=0, y=-1}, {x=0, y=0}, {x=0, y=1}},
	{{x=-2, y=0}, {x=-1, y=0}, {x=0, y=0}, {x=1, y=0}}
}

local piecetypes = {
	"T", "T", "T", "T",
	"J", "J", "J", "J",
	"Z", "Z",
	"O",
	"S", "S",
	"L", "L", "L", "L",
	"I", "I"
}

local pieceidsbytype = {
	T = {1, 2, 3, 4},
	J = {5, 6, 7, 8},
	Z = {9, 10},
	O = {11},
	S = {12, 13},
	L = {14, 15, 16, 17},
	I = {18, 19}
}

local rotateleft = {
	4, 1, 2, 3,
	8, 5, 6, 7,
	10, 9,
	11,
	13, 12,
	17, 14, 15, 16,
	19, 18
}

local rotateright = {
	2, 3, 4, 1,
	6, 7, 8, 5,
	10, 9,
	11,
	13, 12,
	15, 16, 17, 14,
	19, 18
}

local xranges = {
	{2, 9}, {1, 9}, {2, 9}, {2, 10},
	{2, 10}, {2, 9}, {1, 9}, {2, 9},
	{2, 9}, {1, 9},
	{2, 10},
	{2, 9}, {1, 9},
	{1, 9}, {2, 9}, {2, 10}, {2, 9},
	{1, 10}, {3, 9}
}

local function makepiece(x, y, pieceid)
	local piece = pieces[pieceid]
	if not piece then
		return {}
	end
	local piece_ = {}
	for i, block in pairs(piece) do
		table.insert(piece_, {x = (block.x + x), y = (block.y + y)})
	end
	return piece_
end

local function shadow(x, pieceid, grid)
	local prevpiece = makepiece(x, 1, pieceid)
	local y = 1
	while y <= 20 do
		piece = makepiece(x, y, pieceid)
		for i, block in pairs(piece) do
			if block.y > 20 then
				return prevpiece
			end
			if block.y >= 1 and grid[block.y][block.x] then
				return prevpiece
			end
		end
		prevpiece = piece
		y = y + 1
	end
	return prevpiece
end

local function projectshadow(x, pieceid, grid)
	local piece = shadow(x, pieceid, grid)
	local newgrid = {}
	for i, row in pairs(grid) do
		newgrid[i] = {}
		for j, cell in pairs(row) do
			newgrid[i][j] = grid[i][j]
		end
	end
	for i, block in pairs(piece) do
		if block.y >= 1 then
			newgrid[block.y][block.x] = true
		end
	end

	return newgrid
end

exports.pieces = pieces
exports.piecetypes = piecetypes
exports.pieceidsbytype = pieceidsbytype
exports.xranges = xranges
exports.makepiece = makepiece
exports.shadow = shadow
exports.hint = hint
exports.projectshadow = projectshadow
exports.rotateleft = rotateleft
exports.rotateright = rotateright

return exports
