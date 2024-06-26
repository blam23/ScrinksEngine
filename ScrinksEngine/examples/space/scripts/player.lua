require "lib.key_map"
require_asset "space.timer"

__class_name = "player"
__base_node = "sprite"
__props = {
    move_speed = 400
}

-- move stuff
local move_target_x = 0
local y_height = 100

-- attack stuff
local attack_timer = timer:new(0.1)
local bullet_count = 100
local total_arc_angle = math.rad(20)
local arc_rotation = (-total_arc_angle/2) - math.rad(90)
local max_arc_rotation = (-total_arc_angle/2) - math.rad(70)
local min_arc_rotation = (-total_arc_angle/2) - math.rad(110)
local arc_rotate = 0.001
local bullet_change = 5
local random_spread = 0.02

function script_added()
    self:position(vec2.new(1920/2, 1050/2))
end

function handle_attack()
    if attack_timer:tick() then
        local x, y = self:position()
        for i = 1, bullet_count do
            local arc_factor = total_arc_angle/bullet_count * (i-0.5)
            local a = arc_factor + arc_rotation + (math.random()-0.5) * random_spread
            local vec = vec2.new(math.cos(a), math.sin(a))

            -- local bullet = bullet.new({
            --     tile_index = 17,
            --     position = vec2.new(x + 16 + vec.x * 20, y + vec.y * 20),
            --     layer = self:layer(),
            --     velocity = vec,
            --     hit_player = false,
            --     hit_enemy = true
            -- })

            local projectile = bullet.new(
                self:parent(),
                17,
                x + 13 + vec.x * 10,
                y + vec.y * 10)
            
            projectile:property("move_speed", math.random() * 300 + 300)
            projectile:property("velocity", vec)

            arc_rotation = arc_rotation + arc_rotate
            if arc_rotation > max_arc_rotation or arc_rotation < min_arc_rotation then
                arc_rotate = -arc_rotate
            end
        end
    end
end

function handle_input()
    velocity = vec2.new(0,0)
    local input_down = false

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

    return input_down
end

function fixed_update()
    handle_attack()

    if handle_input() then
        velocity = velocity:normalize()
        self:translate(velocity * self:property("move_speed") * fixed_delta)
    end
end