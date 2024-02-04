require "lib.key_map"
require_asset "space.timer"
local move_speed = 10

local attack_timer = timer:new(0.02)
local bullet_script = load_script("assets/scripts/space/player_bullet.lua")
local bullet_count = 10
local total_arc_angle = math.rad(30)
local arc_rotation = (-total_arc_angle/2) - math.rad(90)
local bullet_change = 5
local random_spread = 0.05

function script_added()
    print(get_view_size())
    transform:set_position(vec2.new(1920/2, 1050/2))
end

function fixed_update()
    local velocity = vec2.new(0,0)
    local input_down = false

    if attack_timer:tick() then
        local x, y = transform:get_position()
        for i = 1, bullet_count do
            local arc_factor = total_arc_angle/bullet_count * (i-0.5)
            local a = arc_factor + arc_rotation + (math.random()-0.5) * random_spread
            local vec = vec2.new(math.cos(a), math.sin(a))
            local bullet = create_sprite_node(
                17,
                x + 8 + (vec.x * 10),
                y + (vec.y * 10)
            )
            bullet:set_script(bullet_script)
            bullet:set_property("velocity", vec)
        end
    end

    if is_key_down(key_map.A) then
        velocity.x = -1
        input_down = true
    elseif is_key_down(key_map.D) then
        velocity.x = 1
        input_down = true
    end

    if is_key_down(key_map.W) then
        velocity.y = -1
        input_down = true
    elseif is_key_down(key_map.S) then
        velocity.y = 1
        input_down = true
    end

    if input_down then
        velocity = velocity:normalize() * move_speed
        transform:translate(velocity)

    --     if velocity.x < 0 then
    --         sprite:set_tile_index(1)
    --     elseif velocity.x > 0 then
    --         sprite:set_tile_index(2)
    --     end
    -- else
    --     sprite:set_tile_index(0)
    end


end