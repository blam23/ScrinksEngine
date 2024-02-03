require "lib.key_map"
require_asset "space.timer"
local move_speed = 400

local attack_timer = timer:new(0.1)
local bullet_script = load_script("assets/scripts/space/player_bullet.lua")
local bullet_count = 3
local bullet_spread = 0.3

function script_added()
    print(get_view_size())
    transform:set_position(vec2.new(100, 100))
end

function fixed_update()
    local velocity = vec2.new(0,0)
    local input_down = false

    if attack_timer:tick() then
        local x, y = transform:get_position()
        for i = 1, bullet_count do
            local offset = (-bullet_count / 2) + i - 0.5
            local bullet = create_sprite_node(
                16,
                x + 12,
                y
            )
            bullet:set_script(bullet_script)
            --bullet.data.velocity = vec2.new(offset * bullet_spread, -1)
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
        velocity = velocity:normalize() * move_speed * fixed_delta
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