__class_name = "enemy"
__base_node = "sprite"
__props = {
    velocity = vec2.new(0, 1),
    move_speed = 100,
}

function script_added()
    self:add_to_group("enemies")
end

local screen_bound_buffer = 50

function fixed_update()
    self:translate(self:property("velocity") * self:property("move_speed") * fixed_delta)

    local x, y = self:position()
    local w, h = view_size()
    if w > 0 and h > 0 then
        if x < -screen_bound_buffer or x > w + screen_bound_buffer
        or y < -screen_bound_buffer or y > h + screen_bound_buffer then
            self:mark_for_deletion()
        end
    end
end