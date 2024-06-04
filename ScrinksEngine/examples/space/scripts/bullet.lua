__class_name = "bullet"
__base_node = "sprite"
__props = {
    velocity = vec2.new(0,-1),
    move_speed = 400,
    hit_player = false,
    hit_enemy = true,
}

local screen_bound_buffer = 30
local hit_radius = 10

function fixed_update()
    self:translate(self:property("velocity") * self:property("move_speed") * fixed_delta)

    local x, y = self:position()
    local w, h = view_size()
    if x < -screen_bound_buffer or x > w + screen_bound_buffer
    or y < -screen_bound_buffer or y > h + screen_bound_buffer then
        self:mark_for_deletion()
    end

    if self:property("hit_enemy") then
        local enemies = get_nodes_in_group("enemies")
        for i = 1, #enemies do
            local px, py = enemies[i]:position()
            local dist2 = (px - x) * (px - x) + (py - y) * (py - y)
            if dist2 < hit_radius * hit_radius then
                self:mark_for_deletion()
                enemies[i]:mark_for_deletion()
            end
        end
    end
end