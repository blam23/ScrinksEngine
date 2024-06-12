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
local hit_width = 30
local hit_height = 30

function fixed_update()
    self:translate(self:property("velocity") * self:property("move_speed") * fixed_delta)

    local x, y = self:position()
    local w, h = view_size()
    if x < -screen_bound_buffer or x > w + screen_bound_buffer
    or y < -screen_bound_buffer or y > h + screen_bound_buffer then
        self:mark_for_deletion()
    end

    if self:property("hit_enemy") then
        local enemies = as_quadtree(self:parent()):query(x-hit_width/2,y-hit_height/2,hit_width,hit_height)
        for i = 1, #enemies do
            if enemies[i]:in_group("enemies") then
                self:mark_for_deletion()
                enemies[i]:mark_for_deletion()
            end
        end
    end
end