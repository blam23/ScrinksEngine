require_asset "space.timer"

local spawn_timer = timer:new(0.1)

function init()
    local w = 1240
    local h = 720
    tree = quadtree.new(0, 0, w, h, 5)
    ship = player.new(tree, 0, 100, 100)
end

function spawn(to_spawn)
    local w, h = view_size()
    for i = 1, to_spawn do
        local enemy = enemy.new(tree, 3, math.random() * w, -20)
        enemy:property("move_speed", math.random() * 100 + 100)

        local vec = vec2.new((math.random() - 0.5) * 0.1, 1)
        enemy:property("velocity", vec)
    end
end

function fixed_update()
    if spawn_timer:tick() then
        spawn(300)
    end
end