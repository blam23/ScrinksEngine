local directory = "lua"
local asset_directory = "assets/scripts"

local __require = require
require = function(path)
    __require(directory .. "." .. path)
end

require_asset = function(path)
    __require(asset_directory .. "." .. path)
end

run = function(file)
    file = directory .. "/" .. file
    local script = assert(loadfile(file))
    return script()
end

-- scripts changing environment will just make things complex
local __fenv = setfenv
setfenv = nil

require "lib.class"