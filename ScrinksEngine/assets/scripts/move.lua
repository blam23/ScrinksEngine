local t = 0

function node.fixed_update()
    set_pos(sin(t), 0, cos(t))
end