local directory = "lua"

local __require = require
require = function(path)
    __require(directory .. "." .. path)
end

run = function(file)
    file = directory .. "/" .. file
    local script = assert(loadfile(file))
    return script()
end

require "lib.class"
