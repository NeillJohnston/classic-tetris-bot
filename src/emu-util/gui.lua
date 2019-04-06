local memutil = require("memory")

local exports = {}

local function drawpiece(piece, color)
	for i, block in pairs(piece) do
		gui.opacity(0.4)
		local x = 96 + 8*(block.x-1)
		local y = 48 + 8*(block.y-1)
		gui.box(x, y, x+7, y+7, color)
	end
end

exports.drawpiece = drawpiece

return exports
