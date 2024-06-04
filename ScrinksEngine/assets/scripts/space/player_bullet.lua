local screen_bound_buffer = 30

velocity = vec2.new(0, -1)
move_speed = 16

function script_added()
    velocity = self:property("velocity") or velocity
    velocity = velocity:normalize()
    move_speed = self:property("speed") or move_speed
end

function fixed_update()
    self:translate(velocity * move_speed)

    local x, y = self:position()
    local w, h = view_size()
    if x < -screen_bound_buffer or x > w + screen_bound_buffer
    or y < -screen_bound_buffer or y > h + screen_bound_buffer then
        self:mark_for_deletion()
    end
end