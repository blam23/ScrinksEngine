__class_name = "bullet"
__base_node = "sprite"
__props = {
    velocity = vec2.new(0,-1),
    move_speed = 400,
    hit_player = false,
    hit_enemy = false,
}

local screen_bound_buffer = 30

function fixed_update()
    self:translate(self:property("velocity") * self:property("move_speed") * fixed_delta)

    local x, y = self:position()
    local w, h = view_size()
    if x < -screen_bound_buffer or x > w + screen_bound_buffer
    or y < -screen_bound_buffer or y > h + screen_bound_buffer then
        self:mark_for_deletion()
    end

    -- todo: do some hit detection
end