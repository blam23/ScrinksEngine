local screen_bound_buffer = 30

velocity = vec2.new(0, -1)
move_speed = 16

function script_added()
    velocity = node:get_property("velocity") or velocity
    velocity = velocity:normalize()
    move_speed = node:get_property("speed") or move_speed
end

function fixed_update()
    transform:translate(velocity * move_speed)

    local x, y = transform:get_position()
    local w, h = get_view_size()
    if x < -screen_bound_buffer or x > w + screen_bound_buffer
    or y < -screen_bound_buffer or y > h + screen_bound_buffer then
        node:mark_for_deletion()
    end
end