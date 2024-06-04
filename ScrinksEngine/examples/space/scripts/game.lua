require_asset "space.timer"

local spawn_timer = timer:new(1)

function init()
    ship = player.new(0, 100, 100)
end

function spawn()
    local w, h = view_size()
    local enemy = enemy.new(3, math.random() * w, -20)
    enemy:property("move_speed", math.random() * 100 + 100)
end

function fixed_update()
    if spawn_timer:tick() then
        spawn()
    end
end