require "lib.key_map"

local move_speed = 700

function script_added()
    if this_node.data.velocity == nil then
        this_node.data.velocity = vec2.new(0, -1)
    end
end

function fixed_update()
    local velocity = this_node.data.velocity:normalize() * move_speed * fixed_delta
    transform:translate(velocity)
end