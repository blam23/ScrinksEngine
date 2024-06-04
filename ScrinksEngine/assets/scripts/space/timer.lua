-- require 'lib.class'

-- timer = {}
-- local timer_mt = class(timer)

-- function timer:new(cooldown)
--     return setmetatable({ cooldown = cooldown, current = 0, tick_rate = 1.0 }, timer_mt)
-- end

-- function timer:tick()
--     self.current = self.current - (fixed_delta * self.tick_rate)

--     if self.current <= 0 then
--         self.current = self.cooldown
--         return true
--     end

--     return false
-- end

require 'lib.class'

timer = {}
local timer_mt = class(timer)

function timer:new(cooldown)
    return setmetatable({ cooldown = cooldown, last_tick = 0, tick_rate = 1.0 }, timer_mt)
end

function timer:tick()
    local current_time = fixed_delta + self.last_tick
    local elapsed = current_time >= self.cooldown
    self.last_tick = current_time

    if elapsed then
        self.last_tick = 0
    end

    return elapsed
end